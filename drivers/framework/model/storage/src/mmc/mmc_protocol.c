/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mmc_emmc.h"
#include "mmc_sd.h"
#include "mmc_sdio.h"
#include "securec.h"

#define HDF_LOG_TAG mmc_protocol_c

#define INIT_CMD_RETRY_TIMES 100
#define MMC_CMD_DEFAULT_RETRY_TIMES 3
#define SEND_STATUS_CMD_RETRY_TIMES 100
#define STOP_TRANSMISSION_CMD_RETRY_TIMES 5
#define BITS_NUMBER_OF_4_BYTES 32

/* TRAN_SPEED: Frequency unit 0 = 100KHz, 1 = 1MHz, 2 = 10MHz, 3 = 100MHz, 4...7 = reserved */
uint32_t g_tranSpeedUnit[] = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };

/* TAAC: Time unit 0 = 1ns, 1 = 10ns, 2 = 100ns, 3 = 1us, 4 = 10us, 5 = 100us, 6 = 1ms, 7 = 10ms */
uint32_t g_taccUnit[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000 };

/*
 * TRAN_SPEED/TAAC: Multiplier factor 0 = reserved, 1 = 1.0, 2 = 1.2, 3 = 1.3, 4 = 1.5, 5 = 2.0, 6 = 2.5,
 * 7 = 3.0, 8 = 3.5, 9 = 4.0, A = 4.5, B = 5.0, C = 5.5, D = 6.0, E = 7.0, F = 8.0
 */
uint32_t g_commFactor[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

/* Little-endian and Big-endian interconversion. */
static uint32_t MmcEndianConversion(uint32_t x)
{
    uint32_t val = x;

    return (uint32_t)(
            (((uint32_t)(val) & (uint32_t)0x000000ffUL) << 24) |
            (((uint32_t)(val) & (uint32_t)0x0000ff00UL) << 8) |
            (((uint32_t)(val) & (uint32_t)0x00ff0000UL) >> 8) |
            (((uint32_t)(val) & (uint32_t)0xff000000UL) >> 24));
}

/* Decoding algorithm */
static uint32_t MmcParseBits(uint32_t *data, uint32_t bitsLen, uint32_t start, uint32_t size)
{
    uint32_t index;
    uint32_t shift;
    uint32_t ret;

    if (start >= bitsLen || size == 0) {
        HDF_LOGE("MmcParseBits: input invalid!");
        return 0;
    }
    if (size > BITS_NUMBER_OF_4_BYTES) {
        HDF_LOGE("MmcParseBits: not support!");
        return 0;
    }

    index = (bitsLen / BITS_NUMBER_OF_4_BYTES) - (start / BITS_NUMBER_OF_4_BYTES) - 1;
    shift = start & (BITS_NUMBER_OF_4_BYTES - 1);
    ret = data[index] >> shift;
    if (size + shift > BITS_NUMBER_OF_4_BYTES) {
        ret |= data[index - 1] << (BITS_NUMBER_OF_4_BYTES - shift);
    }

    if (size < BITS_NUMBER_OF_4_BYTES) {
        return (ret & ((1u << size) - 1));
    }
    return (ret & 0xFFFFFFFF);
}

static int32_t MmcSendCmd(struct MmcCntlr *cntlr, struct MmcCmd *cmd, struct MmcData *data, uint32_t retryTimes)
{
    uint32_t i;
    int32_t ret;

    if (cntlr == NULL || cmd == NULL || retryTimes == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (cntlr->ops == NULL || cntlr->ops->request == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    cmd->data = data;
    for (i = 0; i < retryTimes; i++) {
        ret = MmcCntlrDoRequest(cntlr, cmd);
        if (ret != HDF_SUCCESS) {
            continue;
        }
        if (cmd->returnError != HDF_SUCCESS) {
            continue;
        }
        if (data != NULL && data->returnError != HDF_SUCCESS) {
            continue;
        }
        break;
    }
    if (data == NULL) {
        return cmd->returnError;
    }
    if (cmd->returnError != HDF_SUCCESS) {
        return cmd->returnError;
    }
    if (data->returnError != HDF_SUCCESS) {
        return data->returnError;
    }
    return HDF_SUCCESS;
}

static void MmcGoIdleState(struct MmcCntlr *cntlr)
{
    struct MmcCmd cmd = {0};

    /* Command GO_IDLE_STATE (CMD0) is the software reset command and sets all cards into Idle State. */
    cmd.cmdCode = GO_IDLE_STATE;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    /* Broadcast Commands (bc), no response. */
    cmd.respType = MMC_RESP_SPI_R1 | MMC_RESP_NONE | MMC_CMD_TYPE_BC;
    (void)MmcSendCmd(cntlr, &cmd, NULL, 1);
    OsalMDelay(1);
}

static int32_t MmcSendOpCond(struct MmcCntlr *cntlr, uint32_t arg, uint32_t *ocr)
{
    struct MmcCmd cmd = {0};
    int32_t err;
    uint32_t i;

    /*
     * The SEND_OP_COND (CMD1) command is designed to provide MultiMediaCard hosts with a mechanism
     * to identify and reject cards which do not match the VDD range desired by the host.
     */
    cmd.cmdCode = SEND_OP_COND;
    /* [31:0] OCR. */
    cmd.argument = arg;
    cmd.respType = MMC_RESP_SPI_R1 | MMC_RESP_R3 | MMC_CMD_TYPE_BCR;
    /*
     * The host must poll the card (by repeatedly sending CMD1 or ACMD41) until the 'in-idle-state' bit in
     * the card response indicates (by being set to 0) that the card completed its initialization processes
     * and is ready for the next command.
     */
    for (i = 0; i < INIT_CMD_RETRY_TIMES; i++) {
        err = MmcSendCmd(cntlr, &cmd, NULL, 1);
        if (err != HDF_SUCCESS) {
            break;
        }
        if (arg == 0 || (cmd.resp[0] & MMC_CARD_BUSY_STATUS) > 0) {
            break;
        }

        err = HDF_ERR_TIMEOUT;
        OsalMDelay(10);
    }

    if (ocr != NULL) {
        *ocr = cmd.resp[0];
    }
    return err;
}

static int32_t MmcAllSendCid(struct MmcCntlr *cntlr)
{
    struct MmcCmd cmd = {0};
    uint32_t *cid = NULL;
    int32_t err;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = ALL_SEND_CID;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    /* Broadcast Commands with Response(bcr). */
    cmd.respType = MMC_RESP_R2 | MMC_CMD_TYPE_BCR;
    err = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
    if (err != HDF_SUCCESS) {
        return err;
    }
    cid = cntlr->curDev->reg.rawCid;
    if (memcpy_s(cid, sizeof(cntlr->curDev->reg.rawCid), cmd.resp, sizeof(cmd.resp)) != EOK) {
        HDF_LOGE("MmcAllSendCid: memcpy_s fail!");
        return HDF_FAILURE;
    }
    return err;
}

static int32_t MmcSetRelativeAddr(struct MmcCntlr *cntlr)
{
    struct MmcCmd cmd = {0};

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = SET_RELATIVE_ADDR;
    /* [31:16] RCA, [15:0] stuff bits. */
    cmd.argument = (cntlr->curDev->reg.rca << 16);
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    return  MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
}

static int32_t MmcSelectCard(struct MmcCntlr *cntlr)
{
    struct MmcCmd cmd = {0};

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = SELECT_CARD;
    /* [31:16] RCA, [15:0] stuff bits. */
    cmd.argument = (cntlr->curDev->reg.rca << 16);
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    return  MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
}

static int32_t MmcSendExtCsd(struct MmcCntlr *cntlr, uint8_t *extCsd, uint32_t len)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};

    cmd.cmdCode = SEND_EXT_CSD;
    cmd.argument = 0;
    cmd.respType = MMC_RESP_SPI_R1 | MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;

    /* The card sends the EXT_CSD register as a block of data, 512 bytes long. */
    data.blockSize = len;
    data.blockNum = 1;
    data.dataFlags = DATA_READ;
    data.dataBuffer = extCsd;

    return MmcSendCmd(cntlr, &cmd, &data, 1);
}

static int32_t MmcSendCsd(struct MmcCntlr *cntlr)
{
    struct MmcCmd cmd = {0};
    uint32_t *csd = NULL;
    int32_t err;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = SEND_CSD;
    cmd.argument = (cntlr->curDev->reg.rca << 16);
    cmd.respType = MMC_RESP_R2 | MMC_CMD_TYPE_AC;
    err = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
    if (err != HDF_SUCCESS) {
        return err;
    }
    csd = cntlr->curDev->reg.rawCsd;
    if (memcpy_s(csd, sizeof(cntlr->curDev->reg.rawCsd), cmd.resp, sizeof(cmd.resp)) != EOK) {
        HDF_LOGE("memcpy_s fail!");
        return HDF_FAILURE;
    }
    return err;
}

int32_t MmcSendStatus(struct MmcCntlr *cntlr, uint32_t *status)
{
    struct MmcCmd cmd = {0};
    int32_t error;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = SEND_STATUS;
    /* [31:16] RCA, [15:0] stuff bits. */
    cmd.argument = (cntlr->curDev->reg.rca << 16);
    cmd.respType = MMC_RESP_SPI_R2 | MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    error = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
    if (error != HDF_SUCCESS) {
        return error;
    }
    if (status != NULL) {
        *status = cmd.resp[0];
    }
    return error;
}

static int32_t MmcSwitch(struct MmcCntlr *cntlr, uint8_t set, uint8_t index, uint8_t value)
{
    int32_t error;
    struct MmcCmd cmd = {0};
    uint32_t status;

    cmd.cmdCode = SWITCH;
    /*
     * [31:26] Set to 0; [25:24] Access; [23:16] Index;
     * [15:8] Value; [7:3] Set to 0; [2:0] Cmd Set.
     */
    cmd.argument = (EMMC_EXT_CSD_WRITE_BYTE << 24)
            | (index << 16)
            | (value << 8)
            | set;
    cmd.respType = MMC_RESP_SPI_R1B | MMC_RESP_R1B | MMC_CMD_TYPE_AC;
    error = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("MmcSwitch: send cmd fail!");
        return error;
    }

    /*
     * The SWITCH command response is of type R1b, therefore, the host should read the card status, using
     * SEND_STATUS command, after the busy signal is de-asserted, to check the result of the SWITCH operation.
     */
    do {
        error = MmcSendStatus(cntlr, &status);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("MmcSwitch: send status cmd fail!");
            return error;
        }
    } while ((!(status & READY_FOR_DATA)) || MmcCntlrDevBusy(cntlr) == true);

    /*
     * When the host tries to access a partition which has not been created before, the devices sets the
     * SWITCH_ERROR bit in the status register and will not change the PARTITION_ACCESS bits.
     */
    if ((status & SWITCH_ERROR) > 0) {
        return HDF_MMC_ERR_SWITCH_FAIL;
    }
    return error;
}

static int32_t MmcAppCmd(struct MmcCntlr *cntlr, uint32_t acmd)
{
    int32_t err;
    struct MmcCmd cmd = {0};

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /*
     * After the bus is activated the host will request the cards to send their valid operation conditions
     * (ACMD41 preceding with APP_CMD - CMD55 with RCA = 0x0000).
     */
    cmd.cmdCode = APP_CMD;
    if (acmd != SD_ACMD_OP_COND) {
        /* [31:16] RCA, [15:0] stuff bits */
        cmd.argument = (cntlr->curDev->reg.rca << 16);
        cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    } else {
        cmd.argument = 0;
        cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_BCR;
    }
    err = MmcSendCmd(cntlr, &cmd, NULL, 1);
    if (err != HDF_SUCCESS) {
        return err;
    }
    if (!(cmd.resp[0] & IS_APP_CMD)) {
        return HDF_FAILURE;
    }
    return err;
}

static int32_t MmcWaitCardReady(struct MmcCntlr *cntlr)
{
    int error;
    uint32_t status;
    int32_t retryTimes = SEND_STATUS_CMD_RETRY_TIMES;

    do {
        error = MmcSendStatus(cntlr, &status);
        if (error != HDF_SUCCESS) {
            return error;
        }

        if (retryTimes >= 0) {
            retryTimes--;
            OsalMSleep(10);
        } else {
            return HDF_ERR_TIMEOUT;
        }
    } while (!(status & READY_FOR_DATA) || (MMC_CARD_CURRENT_STATE(status) == STATE_PRG));
    return error;
}

int32_t MmcStopTransmission(struct MmcCntlr *cntlr, bool writeFlag, uint32_t *stopStatus)
{
    int32_t err;
    struct MmcCmd cmd = {0};

    if (stopStatus == NULL) {
        HDF_LOGE("%s: stopStatus is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = STOP_TRANSMISSION;
    /* R1 for read cases and R1b for write cases. */
    if (writeFlag == true) {
        cmd.respType = MMC_RESP_R1B | MMC_CMD_TYPE_AC;
    } else {
        cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    }
    err = MmcSendCmd(cntlr, &cmd, NULL, STOP_TRANSMISSION_CMD_RETRY_TIMES);
    if (err != HDF_SUCCESS) {
        return err;
    }

    *stopStatus = cmd.resp[0];
    /* No need to check card status in case of read. */
    if (writeFlag == false) {
        return err;
    }
    return MmcWaitCardReady(cntlr);
}

int32_t MmcSendTuning(struct MmcCntlr *cntlr, uint32_t cmdCode, bool sendStop)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    /* Tuning Block Pattern UHS-I. */
    uint8_t tuningBlk4bit[] = {
        0xFF, 0x0F, 0xFF, 0x00, 0xFF, 0xCC, 0xC3, 0xCC, 0xC3, 0x3C, 0xCC, 0xFF,
        0xFE, 0xFF, 0xFE, 0xEF, 0xFF, 0xDF, 0xFF, 0xDD, 0xFF, 0xFB, 0xFF, 0xFB,
        0xBF, 0xFF, 0x7F, 0xFF, 0x77, 0xF7, 0xBD, 0xEF, 0xFF, 0xF0, 0xFF, 0xF0,
        0x0F, 0xFC, 0xCC, 0x3C, 0xCC, 0x33, 0xCC, 0xCF, 0xFF, 0xEF, 0xFF, 0xEE,
        0xFF, 0xFD, 0xFF, 0xFD, 0xDF, 0xFF, 0xBF, 0xFF, 0xBB, 0xFF, 0xF7, 0xFF,
        0xF7, 0x7f, 0x7B, 0xDE,
    };
    /* Tuning block pattern for 8 bit mode for HS200. */
    uint8_t tuningBlk8bit[] = {
        0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xCC, 0xCC,
        0xCC, 0x33, 0xCC, 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0xCC, 0xFF, 0xFF,
        0xFF, 0xEE, 0xFF, 0xFF, 0xFF, 0xEE, 0xEE, 0xFF, 0xFF, 0xFF, 0xDD, 0xFF,
        0xFF, 0xFF, 0xDD, 0xDD, 0xFF, 0xFF, 0xFF, 0xBB, 0xFF, 0xFF, 0xFF, 0xBB,
        0xBB, 0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x77, 0x77, 0xFF, 0x77,
        0xBB, 0xDD, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
        0x00, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0x33, 0xCC, 0xCC, 0xCC, 0x33, 0x33,
        0xCC, 0xCC, 0xCC, 0xFF, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF, 0xFF, 0xEE, 0xEE,
        0xFF, 0xFF, 0xFF, 0xDD, 0xFF, 0xFF, 0xFF, 0xDD, 0xDD, 0xFF, 0xFF, 0xFF,
        0xBB, 0xFF, 0xFF, 0xFF, 0xBB, 0xBB, 0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF,
        0xFF, 0x77, 0x77, 0xFF, 0x77, 0xBB, 0xDD, 0xEE,
    };

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.cmdCode = cmdCode;
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;

    if (cntlr->curDev->workPara.width == BUS_WIDTH4) {
        data.blockSize = sizeof(tuningBlk4bit);
        data.dataBuffer = tuningBlk4bit;
    } else if (cntlr->curDev->workPara.width == BUS_WIDTH8) {
        data.blockSize = sizeof(tuningBlk8bit);
        data.dataBuffer = tuningBlk8bit;
    } else {
        HDF_LOGE("MmcSendTuning: work width is 1, can not tune!");
        return HDF_FAILURE;
    }
    data.blockNum = 1;
    data.dataFlags = DATA_READ;
    if (sendStop == true) {
        data.sendStopCmd = true;
        data.stopCmd.cmdCode = STOP_TRANSMISSION;
        data.stopCmd.respType = MMC_RESP_R1B | MMC_CMD_TYPE_AC;
    }

    return MmcSendCmd(cntlr, &cmd, &data, 1);
}

static int32_t MmcSendEraseStartCmd(struct MmcCntlr *cntlr, uint32_t arg)
{
    struct MmcCmd cmd = {0};
    struct MmcDevice *dev = cntlr->curDev;

    if (dev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* SD: CMD32; EMMC: CMD35 */
    if (dev->type == MMC_DEV_SD) {
        cmd.cmdCode = SD_CMD_ERASE_WR_BLK_START;
    } else {
        cmd.cmdCode = ERASE_GROUP_START;
    }
    /* [31:0]data address. */
    cmd.argument = arg;
    /*
     * Data address for media =<2GB is a 32bit byte address and data address for media > 2GB is
     * a 32bit sector (512B) address.
     */
    if (dev->state.bits.blockAddr == 0) {
        cmd.argument <<= MMC_MAX_BLOCKSIZE_SHIFT;
    }
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    return MmcSendCmd(cntlr, &cmd, NULL, 1);
}

static int32_t MmcSendEraseEndCmd(struct MmcCntlr *cntlr, uint32_t arg)
{
    struct MmcCmd cmd = {0};
    struct MmcDevice *dev = cntlr->curDev;

    if (dev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* SD: CMD33; EMMC: CMD36 */
    if (dev->type == MMC_DEV_SD) {
        cmd.cmdCode = SD_CMD_ERASE_WR_BLK_END;
    } else {
        cmd.cmdCode = ERASE_GROUP_END;
    }
    /* [31:0]data address. */
    cmd.argument = arg;
    /*
     * Data address for media =<2GB is a 32bit byte address and data address for media > 2GB is
     * a 32bit sector (512B) address.
     */
    if (dev->state.bits.blockAddr == 0) {
        cmd.argument <<= MMC_MAX_BLOCKSIZE_SHIFT;
    }
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    return MmcSendCmd(cntlr, &cmd, NULL, 1);
}

static int32_t MmcSendEraseCmd(struct MmcCntlr *cntlr, uint32_t arg)
{
    struct MmcCmd cmd = {0};
    struct MmcDevice *dev = cntlr->curDev;

    if (dev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* ERASE cmd38 */
    cmd.cmdCode = MMC_ERASE;
    cmd.argument = arg;
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    return MmcSendCmd(cntlr, &cmd, NULL, 1);
}

static int32_t MmcAlignEraseSize(struct MmcCntlr *cntlr, uint32_t *start, uint32_t *end, uint32_t size)
{
    uint32_t curSize = size;
    uint32_t tmp;

    if (cntlr->curDev->eraseSize == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }

    /* align start. */
    tmp = (*start) % cntlr->curDev->eraseSize;
    if (tmp > 0) {
        tmp = cntlr->curDev->eraseSize - tmp;
        (*start) += tmp;
        if (curSize > tmp) {
            curSize -= tmp;
        } else {
            return HDF_ERR_NOT_SUPPORT;
        }
    }

    /* align size. */
    tmp = curSize % cntlr->curDev->eraseSize;
    if (tmp > 0) {
        curSize -= tmp;
    }
    if (curSize == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }

    (*end) = (*start) + curSize;
    return HDF_SUCCESS;
}

int32_t MmcSendErase(struct MmcCntlr *cntlr, uint32_t startSec, uint32_t nSec)
{
    int32_t ret;
    uint32_t start = startSec;
    uint32_t end = start + nSec;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cntlr->curDev->eraseSize == 0 ||
        (cntlr->curDev->reg.csd.ccc & MMC_CSD_CCC_ERASE) == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = MmcAlignEraseSize(cntlr, &start, &end, nSec);
    if (ret != HDF_SUCCESS) {
        /* after align, no need to erase. */
        return HDF_SUCCESS;
    }

    /*
     * emmc:
     * Starting the erase process is a three steps sequence.
     * First the host defines the start address of the range using the ERASE_GROUP_START (CMD35) command,
     * next it defines the last address of the range using the ERASE_GROUP_END (CMD36) command and
     * finally it starts the erase process by issuing the ERASE (CMD38) command with argument bits set to zero.
     * sd:
     * The host should adhere to the following command sequence: ERASE_WR_BLK_START(cmd32), ERASE_WR_BLK_END(cmd33)
     * and ERASE(CMD38).
     */
    ret = MmcSendEraseStartCmd(cntlr, start);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("send erase start cmd fail, err = %d!", ret);
        return ret;
    }

    ret = MmcSendEraseEndCmd(cntlr, end);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("send erase end cmd fail, err = %d!", ret);
        return ret;
    }

    ret = MmcSendEraseCmd(cntlr, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("send erase cmd fail, err = %d!", ret);
        return ret;
    }

    return MmcWaitCardReady(cntlr);
}

void MmcSetupReadWriteBlocksCmd(struct MmcDevice *mmc, struct MmcCmd *cmd, struct MmcRwData *info)
{
    cmd->data->blockSize = BYTES_PER_BLOCK;
    cmd->data->blockNum = info->sectors;
    cmd->data->dataBuffer = info->buf;

    cmd->respType = MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;
    /* [31:0]data address. */
    cmd->argument = info->startSector;
    if (mmc->state.bits.blockAddr == 0) {
        /* SDSC Card uses byte unit address and SDHC and SDXC Cards use block unit address(512 Bytes unit). */
        cmd->argument <<= MMC_MAX_BLOCKSIZE_SHIFT;
    }
    if (info->writeFlag == true) {
        cmd->data->dataFlags = DATA_WRITE;
        cmd->cmdCode = WRITE_BLOCK;
        if (info->sectors > 1) {
            cmd->cmdCode = WRITE_MULTIPLE_BLOCK;
        }
    } else {
        cmd->data->dataFlags = DATA_READ;
        cmd->cmdCode = READ_SINGLE_BLOCK;
        if (info->sectors > 1) {
            cmd->cmdCode = READ_MULTIPLE_BLOCK;
        }
    }
    if ((info->sectors > 1) && (mmc->cntlr->caps.bits.cmdStop > 0)) {
        cmd->data->stopCmd.cmdCode = STOP_TRANSMISSION;
        cmd->data->sendStopCmd = true;
    }
}

int32_t MmcSendReadWriteBlocks(struct MmcCntlr *cntlr, struct MmcRwData *info)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cmd.data = &data;
    MmcSetupReadWriteBlocksCmd(cntlr->curDev, &cmd, info);
    return MmcSendCmd(cntlr, &cmd, &data, 1);
}

static int32_t EmmcDecodeCsd(struct MmcCntlr *cntlr)
{
    struct MmcCsd *csd = NULL;
    uint32_t unit, factor;
    uint32_t *rawCsd = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    rawCsd = cntlr->curDev->reg.rawCsd;
    csd = &(cntlr->curDev->reg.csd);

    /* CSD_STRUCTURE: [127:126]. */
    csd->structure = MmcParseBits(rawCsd, CSD_BITS, 126, 2);
    if (csd->structure == 0) {
        HDF_LOGE("EmmcDecodeCsd: structure is invalid!");
        return HDF_ERR_INVALID_PARAM;
    }

    /* SPEC_VERS: [125:122]. */
    csd->specVers = MmcParseBits(rawCsd, CSD_BITS, 122, 4);
    /* TAAC: [119:112]; TAAC bit position-->Time unit: [2:0], Multiplier factor: [6:3]. */
    factor = MmcParseBits(rawCsd, CSD_BITS, 115, 4);
    unit = MmcParseBits(rawCsd, CSD_BITS, 112, 3);
    csd->taccNs = (g_taccUnit[unit] * g_commFactor[factor] + 9) / 10;
    /* NSAC: [111:104] */
    csd->taccClks = MmcParseBits(rawCsd, CSD_BITS, 104, 8) * 100;

    /* TRAN_SPEED: [103:96]; TRAN_SPEED bit-->Frequency unit: [2:0], Multiplier factor: [6:3]. */
    factor = MmcParseBits(rawCsd, CSD_BITS, 99, 4);
    unit = MmcParseBits(rawCsd, CSD_BITS, 96, 3);
    csd->maxDtr = g_tranSpeedUnit[unit] * g_commFactor[factor];
    csd->ccc = MmcParseBits(rawCsd, CSD_BITS, 84, 12);

    /* C_SIZE: [73:62] */
    factor = MmcParseBits(rawCsd, CSD_BITS, 62, 12);
    /* C_SIZE_MULT: [49:47] */
    unit = MmcParseBits(rawCsd, CSD_BITS, 47, 3);
    csd->capacity = (1 + factor) << (unit + 2);
    /* READ_BL_LEN: [83:80]. */
    csd->readBlkLen = MmcParseBits(rawCsd, CSD_BITS, 80, 4);
    /* READ_BL_PARTIAL: [79:79] */
    csd->rdPartial = MmcParseBits(rawCsd, CSD_BITS, 79, 1);
    /* WRITE_BLK_MISALIGN: [78:78] */
    csd->wrMisalign = MmcParseBits(rawCsd, CSD_BITS, 78, 1);
    /* READ_BLK_MISALIGN: [77:77] */
    csd->rdMisalign = MmcParseBits(rawCsd, CSD_BITS, 77, 1);
    /* Write speed factor(R2W_FACTOR) :[28:26] */
    csd->r2wFactor = MmcParseBits(rawCsd, CSD_BITS, 26, 3);
    /* WRITE_BL_LEN: [25:22] */
    csd->writeBlkLen = MmcParseBits(rawCsd, CSD_BITS, 22, 4);
    /* WRITE_BL_PARTIAL: [21:21] */
    csd->wrPartial = MmcParseBits(rawCsd, CSD_BITS, 21, 1);

    /*
     * Note that the support for 512B read access is mandatory for all cards.
     * And that the cards has to be in 512B block length mode by default after power-on, or software reset.
     */
    if (csd->writeBlkLen >= MMC_MAX_BLOCKSIZE_SHIFT) {
        /* ERASE_GRP_SIZE: [46:42] */
        unit = MmcParseBits(rawCsd, CSD_BITS, 42, 5);
        /* ERASE_GRP_MULT: [41:37] */
        factor = MmcParseBits(rawCsd, CSD_BITS, 37, 5);
        /* size of erasable unit = (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULT + 1) */
        csd->eraseSize = (unit + 1) * (factor + 1);
        csd->eraseSize <<= (csd->writeBlkLen - MMC_MAX_BLOCKSIZE_SHIFT);
    }
    return HDF_SUCCESS;
}

static int32_t EmmcDecodeCid(struct MmcCntlr *cntlr)
{
    uint32_t i;
    struct MmcCid *cid = NULL;
    uint32_t *rawCid = NULL;
    uint8_t specVers, cbx;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    rawCid = cntlr->curDev->reg.rawCid;
    cid = &(cntlr->curDev->reg.cid);
    specVers = cntlr->curDev->reg.csd.specVers;
    if (specVers > MMC_CSD_SPEC_VER_4) {
        //HDF_LOGE("EmmcDecodeCid: specVers %x is invalid!", specVers);
        //return HDF_ERR_NOT_SUPPORT;
    }

    /*
     * The manufacturing date is composed of two hexadecimal digits, four bits each,
     * representing a two digits date code m/y;
     */
    cid->month = MmcParseBits(rawCid, CID_BITS, 12, 4);
    /* The "y" field, least significant nibble, is the year code. 0 is 1997. */
    cid->year = MmcParseBits(rawCid, CID_BITS, 8, 4) + 1997;

    if (specVers == MMC_CSD_SPEC_VER_0 || specVers == MMC_CSD_SPEC_VER_1) {
        cid->mid = MmcParseBits(rawCid, CID_BITS, 104, 24);
        for (i = 0; i < 7; i++) {
            cid->pnm[i] = (char)MmcParseBits(rawCid, CID_BITS, CID_PNM_START_BIT - (i * BITS_PER_BYTE), BITS_PER_BYTE);
        }
        cid->pnm[7] = '\0';
        cid->hwPrv = MmcParseBits(rawCid, CID_BITS, 44, 4);
        cid->fwPrv = MmcParseBits(rawCid, CID_BITS, 40, 4);
        cid->psn = MmcParseBits(rawCid, CID_BITS, 16, 24);
    } else {
        /* Manufacturer ID(MID): [127:120] */
        cid->mid = MmcParseBits(rawCid, CID_BITS, 120, 8);
        /* OEM/Application ID(OID): [119:104] */
        cid->oid = MmcParseBits(rawCid, CID_BITS, 104, 16);
        /* Product name(PNM): [103:56] */
        for (i = 0; i < 6; i++) {
            cid->pnm[i] = (char)MmcParseBits(rawCid, CID_BITS, CID_PNM_START_BIT - (i * BITS_PER_BYTE), BITS_PER_BYTE);
        }
        cid->pnm[6] = '\0';
        /* Product serial number(PSN): [47:16] */
        cid->psn = MmcParseBits(rawCid, CID_BITS, 16, 32);
        /*
         * Card or BGA(CBX): [113:112]
         * 00: Card (removable); 01: BGA (Discrete embedded); 10: POP; 11: Reserved.
         */
        cbx = MmcParseBits(rawCid, CID_BITS, 112, 2);
        if (cbx == 0) {
            cntlr->curDev->state.bits.removable = 1;
            HDF_LOGD("Emmc is removable!");
        }
    }
    return HDF_SUCCESS;
}

static void EmmcDecodeExtCsdSector(struct EmmcDevice *emmcDev, struct EmmcExtCsd *extCsd)
{
    uint32_t i;
    uint32_t shift = 0;

    extCsd->sectors = 0;
    for (i = 0; i < EMMC_EXT_CSD_SEC_CNT_BYTES; i++) {
        extCsd->sectors |= (uint32_t)(extCsd->rawSectors[i] << shift);
        shift += BITS_PER_BYTE;
    }
    /* Device density > 2GiB are sector addressed. */
    if (extCsd->sectors > (2u * 1024 * 1024 * 1024) / 512) {
        emmcDev->mmc.state.bits.blockAddr = 1;
    }
}

static void EmmcDecodeExtCsdCardType(struct EmmcExtCsd *extCsd)
{
    uint8_t cardType = extCsd->rawCardType & EMMC_EXT_CSD_CARD_TYPE_MASK;

    switch (cardType) {
        case EMMC_EXT_CSD_CARD_TYPE_SDR_ALL:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_1_8V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_1_2V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_52:
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_200;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_SDR_200;
            break;
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_1_8V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_1_2V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_52:
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_200;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V;
            break;
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_1_8V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_1_2V:
        case EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_52:
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_200;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V;
            break;
        case (EMMC_EXT_CSD_CARD_TYPE_DDR_52 | EMMC_EXT_CSD_CARD_TYPE_52 | EMMC_EXT_CSD_CARD_TYPE_26):
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_52;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_DDR_52;
            break;
        case (EMMC_EXT_CSD_CARD_TYPE_DDR_1_2V | EMMC_EXT_CSD_CARD_TYPE_52 | EMMC_EXT_CSD_CARD_TYPE_26):
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_52;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_DDR_1_2V;
            break;
        case (EMMC_EXT_CSD_CARD_TYPE_DDR_1_8V | EMMC_EXT_CSD_CARD_TYPE_52 | EMMC_EXT_CSD_CARD_TYPE_26):
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_52;
            extCsd->cardType = EMMC_EXT_CSD_CARD_TYPE_DDR_1_8V;
            break;
        case (EMMC_EXT_CSD_CARD_TYPE_52 | EMMC_EXT_CSD_CARD_TYPE_26):
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_52;
            break;
        case EMMC_EXT_CSD_CARD_TYPE_26:
            extCsd->hsMaxDtr = EMMC_EXT_CSD_HIGH_SPEED_26;
            break;
        default:
            HDF_LOGD("EmmcDecodeExtCsdCardType: not support high-speed!");
            break;
    }
}

static void EmmcDecodeExtCsdRev13Field(struct EmmcExtCsd *extCsd, uint8_t *rawExtCsd)
{
    uint8_t shift = rawExtCsd[EMMC_EXT_CSD_S_A_TIMEOUT];

    extCsd->eraseGroupDef = rawExtCsd[EMMC_EXT_CSD_ERASE_GROUP_DEF];
    extCsd->partConfig = rawExtCsd[EMMC_EXT_CSD_PARTITION_CONFIG];
    extCsd->partSwitchTime = 10 * rawExtCsd[EMMC_EXT_CSD_PARTITION_SWITCH_TIME];

    if (shift > 0 && shift <= MAX_S_A_TIMEOUT_VALUE) {
        extCsd->saTimeout = 1 << shift;
    }
    extCsd->relWrSecorCount = rawExtCsd[EMMC_EXT_CSD_REL_WR_SEC_C];
    extCsd->hcEraseTimeout = 300 * rawExtCsd[EMMC_EXT_CSD_ERASE_TIMEOUT_MULT];
    extCsd->hcEraseSize = (extCsd->rawHcEraseGrpSize) << 10;
    extCsd->bootSize = rawExtCsd[EMMC_EXT_CSD_BOOT_MULTI] << 17;
}

static void EmmcDecodeExtCsdEnhanceArea(struct EmmcDevice *emmcDev,
    struct EmmcExtCsd *extCsd, uint8_t *rawExtCsd)
{
    uint32_t i;
    uint32_t shift = 0;

    extCsd->enhAreaEnable = true;
    extCsd->enhAreaOffset = 0;
    for (i = 0; i < EMMC_EXT_CSD_ENH_START_ADDR_BYTES; i++) {
        extCsd->enhAreaOffset |= ((uint64_t)rawExtCsd[EMMC_EXT_CSD_ENH_START_ADDR + i] << shift);
        shift += BITS_PER_BYTE;
    }
    /*
     * Start address of the Enhanced User Data Area segment in the User Data Area
     * (expressedin bytes or in sectors in case of high capacity devices).
     */
    if (emmcDev->mmc.state.bits.blockAddr == 1) {
        extCsd->enhAreaOffset <<= MMC_MAX_BLOCKSIZE_SHIFT;
    }

    shift = 0;
    extCsd->enhAreaSize = 0;
    for (i = 0; i < EMMC_EXT_CSD_ENH_SIZE_MULT_BYTES; i++) {
        extCsd->enhAreaSize |= (uint32_t)(rawExtCsd[EMMC_EXT_CSD_ENH_SIZE_MULT + i] << shift);
        shift += BITS_PER_BYTE;
    }
    /* Max Enhanced Area = MAX_ENH_SIZE_MULT * HC_WP_GRP_SIZE * HC_ERASE_GPR_SIZE * 512kBytes */
    extCsd->enhAreaSize *= (uint32_t)(extCsd->rawHcEraseGrpSize * rawExtCsd[EMMC_EXT_CSD_HC_WP_GRP_SIZE]);
    extCsd->enhAreaSize <<= MMC_MAX_BLOCKSIZE_SHIFT;
}

static void EmmcDecodeExtCsdPowerClassValue(struct EmmcExtCsd *extCsd, uint8_t *rawExtCsd)
{
    extCsd->pwrClF52V195 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_52_195];
    extCsd->pwrClF26V195 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_26_195];
    extCsd->pwrClF52V360 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_52_360];
    extCsd->pwrClF26V360 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_26_360];
    extCsd->pwrClF200V195 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_200_195];
    extCsd->pwrClF200V360 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_200_360];
    extCsd->pwrClDdrF52V195 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_DDR_52_195];
    extCsd->pwrClDdrF52V360 = rawExtCsd[EMMC_EXT_CSD_PWR_CL_DDR_52_360];
}

static void EmmcDecodeExtCsdRev14Field(struct EmmcDevice *emmcDev,
    struct EmmcExtCsd *extCsd, uint8_t *rawExtCsd)
{
    if ((rawExtCsd[EMMC_EXT_CSD_PARTITIONING_SUPPORT] & PARTITIONING_SUPPORT_ENH_ATTRIBUTE_EN) > 0 &&
        (rawExtCsd[EMMC_EXT_CSD_PARTITIONS_ATTRIBUTE] & PARTITIONS_ATTRIBUTE_ENH_USR) > 0) {
        EmmcDecodeExtCsdEnhanceArea(emmcDev, extCsd, rawExtCsd);
    }
    extCsd->secTrimMult = extCsd->rawSecTrimMult;
    extCsd->secEraseMult = extCsd->rawSecEraseMult;
    /* TRIM Timeout = 300ms x TRIM_MULT. */
    extCsd->trimTimeout = 300 * extCsd->rawTrimMult;
    EmmcDecodeExtCsdPowerClassValue(extCsd, rawExtCsd);
}

static void EmmcDecodeExtCsdRev15Field(struct EmmcExtCsd *extCsd, uint8_t *rawExtCsd)
{
    /* whether the eMMC card supports HPI. */
    if ((rawExtCsd[EMMC_EXT_CSD_HPI_FEATURES] & EMMC_EXT_CSD_HPI_SUPPORT) > 0) {
        extCsd->hpi = true;
        if ((rawExtCsd[EMMC_EXT_CSD_HPI_FEATURES] & EMMC_EXT_CSD_HPI_IMPLEMENTATION) > 0) {
            extCsd->hpiCmd = STOP_TRANSMISSION;
        } else {
            extCsd->hpiCmd = SEND_STATUS;
        }
        /*
         * This field indicates the maximum timeout to close a command interrupted by HPI –time between the end
         * bit of CMD12/13 to the DAT0 release by the device.
         * Time is expressed in units of 10-milliseconds.
         */
        extCsd->outOfInterruptTime = rawExtCsd[EMMC_EXT_CSD_OUT_OF_INTERRUPT_TIME] * 10;
    }
    extCsd->wrRelParam = rawExtCsd[EMMC_EXT_CSD_WR_REL_PARAM];
}

static int32_t EmmcDecodeExtCsd(struct MmcCntlr *cntlr, uint8_t *rawExtCsd, uint32_t len)
{
    struct EmmcDevice *emmcDev = (struct EmmcDevice *)cntlr->curDev;
    struct EmmcExtCsd *extCsd = &(emmcDev->emmcReg.extCsd);
    uint32_t i;

    extCsd->rawCsdStructure = rawExtCsd[EMMC_EXT_CSD_STRUCTURE];
    if (emmcDev->mmc.reg.csd.structure == MMC_CSD_STRUCTURE_VER_OTHER) {
        if (extCsd->rawCsdStructure > EMMC_EXT_CSD_STRUCTURE_VER_1_2) {
            HDF_LOGE("EmmcDecodeExtCsd: rawCsdStructure is invalid!");
            return HDF_ERR_INVALID_PARAM;
        }
    }
    extCsd->rev = rawExtCsd[EMMC_EXT_CSD_REV];
    for (i = 0; i < EMMC_EXT_CSD_SEC_CNT_BYTES; i++) {
        extCsd->rawSectors[i] = rawExtCsd[EMMC_EXT_CSD_SEC_CNT + i];
    }
    if (extCsd->rev >= EMMC_EXT_CSD_REV_1_2) {
        EmmcDecodeExtCsdSector(emmcDev, extCsd);
    }

    extCsd->strobeSupport = rawExtCsd[EMMC_EXT_CSD_STROBE_SUPPORT];
    extCsd->rawCardType = rawExtCsd[EMMC_EXT_CSD_CARD_TYPE];
    EmmcDecodeExtCsdCardType(extCsd);

    extCsd->rawSaTimeout = rawExtCsd[EMMC_EXT_CSD_S_A_TIMEOUT];
    extCsd->rawEraseTimeoutMult = rawExtCsd[EMMC_EXT_CSD_ERASE_TIMEOUT_MULT];
    extCsd->rawHcEraseGrpSize = rawExtCsd[EMMC_EXT_CSD_HC_ERASE_GRP_SIZE];
    if (extCsd->rev >= EMMC_EXT_CSD_REV_1_3) {
        EmmcDecodeExtCsdRev13Field(extCsd, rawExtCsd);
    }

    extCsd->rawSecTrimMult = rawExtCsd[EMMC_EXT_CSD_SEC_TRIM_MULT];
    extCsd->rawSecEraseMult = rawExtCsd[EMMC_EXT_CSD_SEC_ERASE_MULT];
    extCsd->rawSecFeatureSupport = rawExtCsd[EMMC_EXT_CSD_SEC_FEATURE_SUPPORT];
    extCsd->rawTrimMult = rawExtCsd[EMMC_EXT_CSD_TRIM_MULT];
    if (extCsd->rev >= EMMC_EXT_CSD_REV_1_4) {
        EmmcDecodeExtCsdRev14Field(emmcDev, extCsd, rawExtCsd);
    }

    if (extCsd->rev >= EMMC_EXT_CSD_REV_1_5) {
        EmmcDecodeExtCsdRev15Field(extCsd, rawExtCsd);
    }
    return HDF_SUCCESS;
}

static void EmmcSetBlockCapacity(struct MmcCntlr *cntlr)
{
    struct EmmcDevice *emmcDev = (struct EmmcDevice *)cntlr->curDev;
    uint32_t gibVal;
    uint32_t mibVal;

    emmcDev->mmc.reg.csd.eraseSize = 0x1f;
    emmcDev->mmc.reg.csd.capacity = 0x8;
    /*
     * ERASE_GROUP_DEF  Bit0: ENABLE:
     * 0x0 : Use old erase group size and write protect group size definition (default)
     * 0x1 : Use high-capacity erase unit size, high capacity erase timeout, and high-capacity write protect
     * group size definition.
     */
    if (emmcDev->emmcReg.extCsd.eraseGroupDef == 1) {
        emmcDev->mmc.eraseSize = emmcDev->emmcReg.extCsd.hcEraseSize;
    } else {
        emmcDev->mmc.eraseSize = emmcDev->mmc.reg.csd.eraseSize;
    }

    if (emmcDev->mmc.state.bits.blockAddr > 0) {
        emmcDev->mmc.capacity = emmcDev->emmcReg.extCsd.sectors;
    } else {
        emmcDev->mmc.capacity = (size_t)emmcDev->mmc.reg.csd.capacity <<
            (emmcDev->mmc.reg.csd.readBlkLen - MMC_MAX_BLOCKSIZE_SHIFT);
    }

    gibVal = emmcDev->mmc.capacity >> 21;
    mibVal = (emmcDev->mmc.capacity & ~(gibVal << 21)) >> 11;
    HDF_LOGD("Emmc dev capacity %u.%u Gib", gibVal, mibVal);
}

static int32_t EmmcCheckExtCsd(struct MmcCntlr *cntlr, enum MmcBusWidth width)
{
    uint8_t extCsd[EXT_CSD_BYTES_LEN] = {0};

    if (width == BUS_WIDTH1) {
        return 0;
    }

    return MmcSendExtCsd(cntlr, extCsd, EXT_CSD_BYTES_LEN);
}

static int32_t EmmcSwitchVoltage(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev)
{
    uint32_t cardType = emmcDev->emmcReg.extCsd.cardType;

    /* Host Voltage Switch. */
    if (cntlr->caps2.bits.hs200Sdr1v2 == 1 && (cardType & EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V) > 0) {
        (void)MmcCntlrSwitchVoltage(cntlr, VOLT_1V2);
    }
    if (cntlr->caps2.bits.hs200Sdr1v8 && (cardType & EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V) > 0) {
        (void)MmcCntlrSwitchVoltage(cntlr, VOLT_1V8);
    }
    return HDF_SUCCESS;
}

static int32_t EmmcSelectHighSpeedBusWidth(struct MmcCntlr *cntlr)
{
    int err = HDF_FAILURE;
    enum MmcBusWidth width = BUS_WIDTH1;

    if (cntlr->caps.bits.cap8Bit > 0) {
        err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH, EMMC_EXT_CSD_BUS_WIDTH_8);
        if (err != HDF_SUCCESS) {
            HDF_LOGD("EmmcSelectHighSpeedBusWidth: switch 8 bit bus width fail!");
        } else {
            MmcCntlrSetBusWidth(cntlr, BUS_WIDTH8);
            width = BUS_WIDTH8;
        }
    }
    if (err != HDF_SUCCESS) {
        err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH, EMMC_EXT_CSD_BUS_WIDTH_4);
        if (err != HDF_SUCCESS) {
            HDF_LOGD("EmmcSelectHighSpeedBusWidth: switch 4 bit bus width fail!");
        } else {
            MmcCntlrSetBusWidth(cntlr, BUS_WIDTH4);
            width = BUS_WIDTH4;
        }
    }
    if (err != HDF_SUCCESS) {
        return err;
    }

    return EmmcCheckExtCsd(cntlr, width);
}

static int32_t EmmcCheckSwitchStatus(struct MmcCntlr *cntlr)
{
    int err;
    uint32_t status;

    err = MmcSendStatus(cntlr, &status);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcCheckSwitchStatus: send status cmd fail!");
        return err;
    }
    if ((status & SWITCH_ERROR) > 0) {
        return HDF_MMC_ERR_SWITCH_FAIL;
    }
    return HDF_SUCCESS;
}

static int32_t EmmcSwitchHighSpeed(struct MmcCntlr *cntlr)
{
    int err;

    /*
     * Switch dev to HS mode.
     * HS_TIMING must be set to "0x1" before setting BUS_WIDTH for dual data rate operation (values 5 or 6).
     */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HS_TIMING, EMMC_EXT_CSD_BUS_TIMING_HS);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSwitchHighSpeed: switch hs fail!");
        return err;
    }
    /* change host freq to 52M according to JEDEC Standard No.84-B51, Page49. */
    MmcCntlrSetClock(cntlr, EMMC_EXT_CSD_HIGH_SPEED_52);
    /* Set host controller to HS timing. */
    MmcCntlrSetBusTiming(cntlr, BUS_TIMING_MMC_HS);
    return HDF_SUCCESS;
}

static int32_t EmmcSelectHs400(struct MmcCntlr *cntlr)
{
    int err;

    err = EmmcSwitchHighSpeed(cntlr);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400: switch hs fail!");
        return err;
    }

    /* Switch dev to DDR. */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH, EMMC_EXT_CSD_DDR_BUS_WIDTH_8);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400: switch to ddr fail!");
        return err;
    }
    /* Switch dev to HS400. */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HS_TIMING, EMMC_EXT_CSD_BUS_TIMING_HS400);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400: switch to hs400 fail!");
        return err;
    }
    MmcCntlrSetClock(cntlr, EMMC_EXT_CSD_HIGH_SPEED_200);
    /* Set host controller to HS400 timing and frequency. */
    MmcCntlrSetBusTiming(cntlr, BUS_TIMING_MMC_HS400);
    return EmmcCheckSwitchStatus(cntlr);
}

static int32_t EmmcSelectHs400es(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev)
{
    int err;

    err = EmmcSwitchVoltage(cntlr, emmcDev);
    if (err != HDF_SUCCESS) {
        return err;
    }

    err = EmmcSelectHighSpeedBusWidth(cntlr);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400es: select hs width fail!");
        return err;
    }

    err = EmmcSwitchHighSpeed(cntlr);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400es: switch hs fail!");
        return err;
    }

    /* Switch dev to DDR with strobe bit. */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH,
        EMMC_EXT_CSD_DDR_BUS_WIDTH_8 | EMMC_EXT_CSD_BUS_WIDTH_STROBE);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400es: switch to ddr fail!");
        return err;
    }
    /* Switch dev to HS400. */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HS_TIMING, EMMC_EXT_CSD_BUS_TIMING_HS400);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs400es: switch to hs400 fail!");
        return err;
    }
    MmcCntlrSetClock(cntlr, EMMC_EXT_CSD_HIGH_SPEED_200);
    /* Set host controller to HS400 timing and frequency. */
    MmcCntlrSetBusTiming(cntlr, BUS_TIMING_MMC_HS400);
    MmcCntlrSetEnhanceSrobe(cntlr, true);
    return EmmcCheckSwitchStatus(cntlr);
}

static int32_t EmmcSelectHs200(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev)
{
    int err;

    err = EmmcSwitchVoltage(cntlr, emmcDev);
    if (err != HDF_SUCCESS) {
        return err;
    }

    err = EmmcSelectHighSpeedBusWidth(cntlr);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs200: select hs width fail!");
        return err;
    }

    /* Switch dev to HS200. */
    err = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HS_TIMING, EMMC_EXT_CSD_BUS_TIMING_HS200);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("EmmcSelectHs200: switch to hs200 fail!");
        return err;
    }
    MmcCntlrSetBusTiming(cntlr, BUS_TIMING_MMC_HS200);
    return EmmcCheckSwitchStatus(cntlr);
}

static uint32_t EmmcGetPowerClassValue(struct MmcCntlr *cntlr, struct EmmcExtCsd *extCsd)
{
    uint32_t val = 0;
    uint32_t vdd, busWidthBit, clock;

    busWidthBit = (cntlr->curDev->workPara.width == BUS_WIDTH8) ?
        EMMC_EXT_CSD_BUS_WIDTH_8 : EMMC_EXT_CSD_BUS_WIDTH_4;
    vdd = 1 << (cntlr->vddBit);
    clock = cntlr->curDev->workPara.clock;

    if (vdd == MMC_OCR_1V65_1V95) {
        if (clock <= EMMC_EXT_CSD_HIGH_SPEED_26) {
            val = extCsd->pwrClF26V195;
        } else if (clock <= EMMC_EXT_CSD_HIGH_SPEED_52) {
            val = extCsd->pwrClF52V195;
        } else if (clock <= EMMC_EXT_CSD_HIGH_SPEED_200) {
            val = extCsd->pwrClF200V195;
        }
    } else if (vdd >= MMC_OCR_2V7_2V8 && vdd <= MMC_OCR_3V5_3V6) {
        if (clock <= EMMC_EXT_CSD_HIGH_SPEED_26) {
            val = extCsd->pwrClF26V360;
        } else if (clock <= EMMC_EXT_CSD_HIGH_SPEED_52) {
            val = extCsd->pwrClF52V360;
        } else if (clock <= EMMC_EXT_CSD_HIGH_SPEED_200) {
            val = extCsd->pwrClF200V360;
        }
    } else {
        return 0;
    }

    if (busWidthBit == EMMC_EXT_CSD_BUS_WIDTH_8) {
        val = (val & EMMC_EXT_CSD_PWR_CL_8BIT_MASK) >> EMMC_EXT_CSD_PWR_CL_8BIT_SHIFT;
    } else {
        val = (val & EMMC_EXT_CSD_PWR_CL_4BIT_MASK) >> EMMC_EXT_CSD_PWR_CL_4BIT_SHIFT;
    }
    return val;
}

static int32_t EmmcSelectPowerClass(struct MmcCntlr *cntlr, struct EmmcExtCsd *extCsd)
{
    int32_t error = 0;
    uint32_t val;

    if (cntlr->curDev->reg.csd.specVers < MMC_CSD_SPEC_VER_4) {
        return HDF_SUCCESS;
    }

    val = EmmcGetPowerClassValue(cntlr, extCsd);
    if (val > 0) {
        error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_POWER_CLASS, val);
    }
    return error;
}

static int32_t EmmcSelectActivateHighSpeed(struct MmcCntlr *cntlr,
    struct EmmcDevice *emmcDev, struct EmmcExtCsd *extCsd)
{
    int32_t error;

    if (extCsd->hsMaxDtr > 0) {
        error = HDF_SUCCESS;
        if (MmcCntlrSupportHighSpeed200(cntlr) == true &&
            extCsd->hsMaxDtr > EMMC_EXT_CSD_HIGH_SPEED_52) {
            error = EmmcSelectHs200(cntlr, emmcDev);
            if (error == HDF_SUCCESS) {
                emmcDev->mmc.state.bits.hs200 = 1;
            }
        } else if (cntlr->caps.bits.highSpeed == 1) {
            error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HS_TIMING,
                EMMC_EXT_CSD_BUS_TIMING_HS);
            if (error == HDF_SUCCESS) {
                emmcDev->mmc.state.bits.highSpeed = 1;
                MmcCntlrSetBusTiming(cntlr, BUS_TIMING_MMC_HS);
            }
        }
        if (error != HDF_SUCCESS && error != HDF_MMC_ERR_SWITCH_FAIL) {
            return error;
        }
    }

    return HDF_SUCCESS;
}

static int32_t EmmcSelectBusSpeedMode(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev, struct EmmcExtCsd *extCsd)
{
    int32_t error;

    /* HS400ES mode requires 8-bit bus width. */
    if (extCsd->strobeSupport > 0 && MmcCntlrSupportHighSpeed400EnhancedStrobe(cntlr) == true) {
        error = EmmcSelectHs400es(cntlr, emmcDev);
        if (error != HDF_SUCCESS) {
            return error;
        }
        emmcDev->mmc.state.bits.hs400es = 1;
    } else {
        /* Activate high speed if supported. */
        error = EmmcSelectActivateHighSpeed(cntlr, emmcDev, extCsd);
        if (error != HDF_SUCCESS) {
            return error;
        }
        /* host set clock. */
        if (emmcDev->mmc.state.bits.hs200 == 1 || emmcDev->mmc.state.bits.highSpeed == 1) {
            if (extCsd->hsMaxDtr > 0) {
                MmcCntlrSetClock(cntlr, extCsd->hsMaxDtr);
            }
        } else if (emmcDev->mmc.reg.csd.maxDtr > 0) {
            MmcCntlrSetClock(cntlr, emmcDev->mmc.reg.csd.maxDtr);
        }
    }

    if (emmcDev->mmc.state.bits.hs400es == 1) {
        error = EmmcSelectPowerClass(cntlr, extCsd);
        if (error != HDF_SUCCESS) {
            HDF_LOGD("EmmcSelectBusSpeedMode: hs400es select power class fail!");
        }
    } else if (emmcDev->mmc.state.bits.hs200 == 1) {
        if ((cntlr->caps2.bits.hs200Sdr1v8 | cntlr->caps2.bits.hs200Sdr1v2) > 0) {
            error = MmcCntlrTune(cntlr, SEND_TUNING_BLOCK_HS200);
            if (error != HDF_SUCCESS) {
                return error;
            }
        }

        if ((extCsd->rawCardType & EMMC_EXT_CSD_CARD_TYPE_HS400) > 0 &&
            MmcCntlrSupportHighSpeed400(cntlr) == true) {
            error = EmmcSelectHs400(cntlr);
            if (error != HDF_SUCCESS) {
                return error;
            }
            emmcDev->mmc.state.bits.hs400 = 1;
            emmcDev->mmc.state.bits.hs200 = 0;
        }

        error = EmmcSelectPowerClass(cntlr, extCsd);
        if (error != HDF_SUCCESS) {
            HDF_LOGD("EmmcSelectBusSpeedMode: hs200 select power class fail!");
        }
    }

    return HDF_SUCCESS;
}

static uint32_t EmmcGetDdrMode(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev, struct EmmcExtCsd *extCsd)
{
    uint32_t ddrMode = MMC_BUS_MODE_NULL;

    /* Indicate DDR mode (if supported). */
    if (emmcDev->mmc.state.bits.highSpeed == 1) {
        if ((extCsd->cardType & EMMC_EXT_CSD_CARD_TYPE_DDR_1_8V) > 0 &&
            (cntlr->caps.bits.ddr1v8 > 0) &&
            (cntlr->caps.bits.uhsDdr50 > 0)) {
            ddrMode = MMC_1_8V_DDR_MODE;
        } else if ((extCsd->cardType & EMMC_EXT_CSD_CARD_TYPE_DDR_1_2V) > 0 && cntlr->caps.bits.ddr1v2 > 0) {
            ddrMode = MMC_1_2V_DDR_MODE;
        }
    }
    return ddrMode;
}

static int32_t EmmcSwitchDdrMode(struct MmcCntlr *cntlr, uint32_t ddrMode,
    enum MmcBusWidth width, uint32_t widthBit)
{
    int32_t error;

    error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH, widthBit);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("EmmcSwitchDdrMode: switch BUS_WIDTH fail!");
        return error;
    }

    if (ddrMode == MMC_1_2V_DDR_MODE) {
        error = MmcCntlrSwitchVoltage(cntlr, VOLT_1V2);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("EmmcSwitchDdrMode: switch 1.2V fail!");
            return error;
        }
    }
    cntlr->curDev->state.bits.ddrMode = 1;
    MmcCntlrSetBusTiming(cntlr, BUS_TIMING_UHS_DDR50);
    MmcCntlrSetBusWidth(cntlr, width);
    return HDF_SUCCESS;
}

static int32_t EmmcSelectSwitchDdrMode(struct MmcCntlr *cntlr, struct EmmcDevice *emmcDev,
    struct EmmcExtCsd *extCsd)
{
    int32_t error;
    uint32_t index, ddrMode;
    enum MmcBusWidth width;
    enum MmcBusWidth busWidths[] = { BUS_WIDTH8, BUS_WIDTH4, BUS_WIDTH1 };
    uint32_t busWidthBit[][2] = {
        { EMMC_EXT_CSD_BUS_WIDTH_8, EMMC_EXT_CSD_DDR_BUS_WIDTH_8 },
        { EMMC_EXT_CSD_BUS_WIDTH_4, EMMC_EXT_CSD_DDR_BUS_WIDTH_4 },
        { EMMC_EXT_CSD_BUS_WIDTH_1, EMMC_EXT_CSD_BUS_WIDTH_1 },
    };

    ddrMode = EmmcGetDdrMode(cntlr, emmcDev, extCsd);
    if (emmcDev->mmc.state.bits.hs400es == 0 &&
        emmcDev->mmc.state.bits.hs400 == 0 &&
        emmcDev->mmc.state.bits.hs200 == 0 &&
        cntlr->curDev->reg.csd.specVers >= MMC_CSD_SPEC_VER_4 &&
        (cntlr->caps.bits.cap4Bit | cntlr->caps.bits.cap8Bit) > 0) {
        index = 1;
        if (cntlr->caps.bits.cap8Bit > 0) {
            index = 0;
        }
        for (; index < sizeof(busWidthBit) / sizeof(busWidthBit[0]); index++) {
            width = busWidths[index];
            /* no DDR for 1-bit width. */
            if (width == BUS_WIDTH1) {
                ddrMode = MMC_BUS_MODE_NULL;
            }
            error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_BUS_WIDTH, busWidthBit[index][0]);
            if (error != HDF_SUCCESS) {
                continue;
            }
            MmcCntlrSetBusWidth(cntlr, width);
            error = EmmcCheckExtCsd(cntlr, width);
            if (error == HDF_SUCCESS) {
                break;
            }
        }
        /* switch DDR mode. */
        if (error == HDF_SUCCESS && ddrMode > MMC_BUS_MODE_NULL) {
            error = EmmcSwitchDdrMode(cntlr, ddrMode, width, busWidthBit[index][1]);
        }
        if (error != HDF_SUCCESS) {
            HDF_LOGE("EmmcSelectSwitchDdrMode: switch ddr mode fail!");
            return error;
        }
    }
    return HDF_SUCCESS;
}

static int32_t EmmcActivateHpiMechanism(struct MmcCntlr *cntlr, struct EmmcExtCsd *extCsd)
{
    int32_t error;

    if (extCsd->hpi == true) {
        error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_HPI_MGMT, 1);
        if (error == HDF_SUCCESS) {
            extCsd->hpiEnable = true;
        } else if (error != HDF_MMC_ERR_SWITCH_FAIL) {
            HDF_LOGE("EmmcActivateHpiMechanism: switch HPI_MGMT fail!");
            return error;
        }
    }
    return HDF_SUCCESS;
}

static int32_t EmmcSwitchOperationMode(struct MmcCntlr *cntlr)
{
    struct EmmcDevice *emmcDev = (struct EmmcDevice *)cntlr->curDev;
    struct EmmcExtCsd *extCsd = &(emmcDev->emmcReg.extCsd);
    int32_t error;

    if (extCsd->enhAreaEnable == true) {
        error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_ERASE_GROUP_DEF, 1);
        if (error == HDF_SUCCESS) {
            extCsd->eraseGroupDef = 1;
        } else if (error != HDF_MMC_ERR_SWITCH_FAIL) {
            HDF_LOGE("EmmcSwitchOperationMode: switch ERASE_GROUP_DEF fail!");
            return error;
        }
    }

    if ((extCsd->partConfig & EMMC_EXT_CSD_PART_CONFIG_ACCESS_MASK) > 0) {
        /* No access to boot partition (default). */
        extCsd->partConfig &= ~EMMC_EXT_CSD_PART_CONFIG_ACCESS_MASK;
        error = MmcSwitch(cntlr, EMMC_EXT_CSD_CMD_SET_NORMAL, EMMC_EXT_CSD_PARTITION_CONFIG, extCsd->partConfig);
        if (error != HDF_SUCCESS && error != HDF_MMC_ERR_SWITCH_FAIL) {
            HDF_LOGE("EmmcSwitchOperationMode: switch PARTITION_CONFIG fail!");
            return error;
        }
    }

    error = EmmcSelectBusSpeedMode(cntlr, emmcDev, extCsd);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("EmmcSwitchOperationMode: select bus speed mode fail!");
        return error;
    }

    error = EmmcSelectSwitchDdrMode(cntlr, emmcDev, extCsd);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("EmmcSwitchOperationMode: select switch ddr mode fail!");
        return error;
    }
    return EmmcActivateHpiMechanism(cntlr, extCsd);
}

static int32_t EmmcReadExtCsd(struct MmcCntlr *cntlr)
{
    uint8_t extCsd[EXT_CSD_BYTES_LEN] = {0};
    int32_t error;

    if (cntlr->curDev->reg.csd.specVers < MMC_CSD_SPEC_VER_4) {
        return HDF_SUCCESS;
    }

    error = MmcSendExtCsd(cntlr, extCsd, EXT_CSD_BYTES_LEN);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("EmmcReadExtCsd: send cmd8 fail, error = %d.", error);
        return error;
    }
    error = EmmcDecodeExtCsd(cntlr, extCsd, EXT_CSD_BYTES_LEN);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("EmmcReadExtCsd: decode ext csd fail, error = %d.", error);
        return error;
    }

    return error;
}

static int32_t EmmcSelect(struct MmcCntlr *cntlr, union MmcOcr ocr)
{
    union MmcOcr curOcr;
    int32_t error;

    MmcGoIdleState(cntlr);
    ocr.bits.hcs = 1;
    /* set dev work voltage. */
    error = MmcSendOpCond(cntlr, ocr.ocrData, &curOcr.ocrData);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc cmd1(set voltage) fail, error = %d.", error);
        return error;
    }

    error = MmcAllSendCid(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc cmd2(get cid) fail, error = %d.", error);
        return error;
    }

    /* card is identified. */
    cntlr->curDev->reg.rca = 1;
    error = MmcSetRelativeAddr(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc cmd3(set rca) fail, error = %d.", error);
        return error;
    }

    error = MmcSendCsd(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc send cmd9(get csd) fail, error = %d.", error);
        return error;
    }
    error = EmmcDecodeCsd(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc decode csd fail, error = %d.", error);
        return error;
    }
    error = EmmcDecodeCid(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc decode cid fail, error = %d.", error);
        return error;
    }
    error = MmcSelectCard(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc send cmd7 fail, error = %d.", error);
        return error;
    }
    error = EmmcReadExtCsd(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Emmc read ext csd fail, error = %d.", error);
        return error;
    }

    return EmmcSwitchOperationMode(cntlr);
}

static int32_t EmmcDeviceAdd(struct MmcCntlr *cntlr)
{
    EmmcSetBlockCapacity(cntlr);
    /* add dev. */
    if (MmcDeviceAdd(cntlr->curDev) != HDF_SUCCESS) {
        HDF_LOGE("EmmcDeviceAdd: Add device fail!");
        return HDF_FAILURE;
    }
    cntlr->curDev->state.bits.present = 1;
    return HDF_SUCCESS;
}

static int32_t EmmcInit(struct MmcCntlr *cntlr)
{
    int32_t error;
    union MmcOcr ocr = {0};

    /* cmd1, detect emmc dev and get the voltage range. */
    error = MmcSendOpCond(cntlr, 0, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd1(detect emmc) fail, error = %d.", error);
        return error;
    }

    MmcCntlrSelectWorkVoltage(cntlr, &ocr);
    if (cntlr->curDev->reg.ocr.ocrData == 0) {
        HDF_LOGE("Emmc select work voltage fail!");
        return HDF_ERR_INVALID_PARAM;
    }
    /* work voltage is low voltage, host should switch. */
    if (cntlr->curDev->reg.ocr.bits.vdd1v65To1v95 > 0) {
        HDF_LOGD("Emmc switch to 1.8V!");
        MmcCntlrSwitchVoltage(cntlr, VOLT_1V8);
    }

    error = EmmcSelect(cntlr, cntlr->curDev->reg.ocr);
    if (error != HDF_SUCCESS) {
        return error;
    }

    return EmmcDeviceAdd(cntlr);
}

static int32_t EmmcDetect(struct MmcCntlr *cntlr)
{
    int32_t ret;

    HDF_LOGD("Detect emmc dev start...");
    MmcGoIdleState(cntlr);
    ret = EmmcInit(cntlr);
    if (ret == HDF_SUCCESS) {
        HDF_LOGD("Detect emmc dev success! %s dev at address 0x%x!",
            cntlr->curDev->state.bits.uhs ? "Ultra high speed" :
            (cntlr->curDev->state.bits.highSpeed ? "High speed" : ""),
            cntlr->curDev->reg.rca);
    }
    return ret;
}

static int32_t SdSendAppCmd(struct MmcCntlr *cntlr, struct MmcCmd *cmd,
    struct MmcData *data, uint32_t retryTimes)
{
    uint32_t i;
    int32_t err;

    if (cntlr == NULL || cmd == NULL || retryTimes == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i <= retryTimes; i++) {
        err = MmcAppCmd(cntlr, cmd->cmdCode);
        if (err != HDF_SUCCESS) {
            continue;
        }
        err = MmcSendCmd(cntlr, cmd, data, 1);
        if (err == HDF_SUCCESS) {
            break;
        }
    }
    return err;
}

static int32_t SdAcmdOpCond(struct MmcCntlr *cntlr, uint32_t arg, uint32_t *ocr)
{
    struct MmcCmd cmd = {0};
    int32_t err;
    uint32_t i;

    cmd.cmdCode = SD_ACMD_OP_COND;
    /* [31]reserved bit, [30]HCS, [29]reserved for eSD, [28]XPC, [27:25]reserved bits, [24]S18R, [23:0]VDD voltage. */
    cmd.argument = arg;
    /* Broadcast Commands with Response(bcr). */
    cmd.respType = MMC_RESP_R3 | MMC_CMD_TYPE_BCR;
    /*
     * The host must poll the card (by repeatedly sending CMD1 or ACMD41) until the 'in-idle-state' bit in
     * the card response indicates (by being set to 0) that the card completed its initialization processes
     * and is ready for the next command.
     */
    for (i = 0; i < INIT_CMD_RETRY_TIMES; i++) {
        err = SdSendAppCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
        if (err != HDF_SUCCESS) {
            break;
        }
        /* if probing, just single pass */
        if (arg == 0) {
            break;
        }
        /*
         * wait until init complete.
         * 0--On Initialization; 1--Initialization Complete.
         */
        if ((cmd.resp[0] & MMC_CARD_BUSY_STATUS) > 0) {
            break;
        }
        err = HDF_ERR_TIMEOUT;
        OsalMDelay(20);
    }
    if (ocr != NULL) {
        *ocr = cmd.resp[0];
    }

    return err;
}

static int32_t SdAcmdSdStatus(struct MmcCntlr *cntlr, uint32_t *ssr, uint32_t len)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    int32_t error;
    uint32_t i;

    if (cntlr == NULL || ssr == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    cmd.cmdCode = SD_ACMD_SD_STATUS;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    /* Addressed (point-to-point) Data Transfer Commands (adtc), data transfer on DAT. */
    cmd.respType = MMC_RESP_SPI_R2 | MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;
    data.blockSize = SSR_BYTES_LEN;
    data.blockNum = 1;
    data.dataFlags = DATA_READ;
    data.dataBuffer = (uint8_t *)ssr;
    error = SdSendAppCmd(cntlr, &cmd, &data, 1);
    if (error != HDF_SUCCESS) {
        return error;
    }
    for (i = 0; i < len; i++) {
        ssr[i] = MmcEndianConversion(ssr[i]);
    }
    return error;
}

static int32_t SdAppSendScr(struct MmcCntlr *cntlr, uint32_t *scr, uint32_t len)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    int32_t error;
    uint32_t i;

    if (cntlr == NULL || scr == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    cmd.cmdCode = SD_ACMD_SEND_SCR;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    /* Addressed (point-to-point) Data Transfer Commands (adtc), data transfer on DAT. */
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;
    data.blockSize = SCR_BYTES_LEN;
    data.blockNum = 1;
    data.dataFlags = DATA_READ;
    data.dataBuffer = (uint8_t *)scr;
    error = SdSendAppCmd(cntlr, &cmd, &data, 1);
    if (error != HDF_SUCCESS) {
        return error;
    }
    for (i = 0; i < len; i++) {
        scr[i] = MmcEndianConversion(scr[i]);
    }
    return error;
}

static int32_t SdAcmdSetBusWidth(struct MmcCntlr *cntlr, uint32_t width)
{
    struct MmcCmd cmd = {0};

    cmd.cmdCode = SD_ACMD_SET_BUS_WIDTH;
    /* [31:2] stuff bits; [1:0]bus width: '00' = 1bit and '10' = 4bits. */
    cmd.argument = width;
    /* Addressed (point-to-point) Commands(ac), no data transfer on DAT. */
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;

    return SdSendAppCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
}

static int32_t SdCmdSendIfCond(struct MmcCntlr *cntlr, uint32_t ocr)
{
    struct MmcCmd cmd = {0};
    int32_t err;
    uint32_t vhs;

    cmd.cmdCode = SD_CMD_SEND_IF_COND;
    /*
     * [31:12]reserved bits, [11:8]supply voltage(VHS), [7:0]check pattern.
     * VHS = 0001, 2.7-3.6V.
     */
    vhs = ((ocr & 0xFF8000U) > 0) ? 1 : 0;
    cmd.argument = (vhs << 8) | 0xAA;
    cmd.respType = MMC_RESP_SPI_R7 | MMC_RESP_R7 | MMC_CMD_TYPE_BCR;

    err = MmcSendCmd(cntlr, &cmd, NULL, 1);
    if (err != HDF_SUCCESS) {
        return err;
    }
    /*
     * Check pattern is used for the host to check validity of communication between the host and the card.
     * In the Response, the card echoes back the check pattern set in argument.
     * If check pattern is not matched, CMD8 communication is not valid.
     */
    if ((cmd.resp[0] & 0xFF) != 0xAA) {
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t SdCmdSendRelativeAddr(struct MmcCntlr *cntlr, uint32_t *rca)
{
    int32_t error;
    struct MmcCmd cmd = {0};

    cmd.cmdCode = SD_CMD_SEND_RELATIVE_ADDR;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    cmd.respType = MMC_RESP_R6 | MMC_CMD_TYPE_BCR;
    error = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
    if (error != HDF_SUCCESS) {
        return error;
    }
    if (rca != NULL) {
        /* New published RCA [31:16] of the card. */
        *rca = cmd.resp[0] >> 16;
    }
    return error;
}

static int32_t SdCmdSwitchVoltage(struct MmcCntlr *cntlr)
{
    int32_t error;
    struct MmcCmd cmd = {0};

    /* Voltage switch command to change signaling level 3.3V to 1.8V. */
    cmd.cmdCode = SD_CMD_SWITCH_VOLTAGE;
    /* [31:0] stuff bits. */
    cmd.argument = 0;
    cmd.respType = MMC_RESP_R1 | MMC_CMD_TYPE_AC;
    error = MmcSendCmd(cntlr, &cmd, NULL, 1);
    if (error != HDF_SUCCESS) {
        return error;
    }
    if (cmd.resp[0] & GENERAL_ERROR) {
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t SdCmdSwitchFunc(struct MmcCntlr *cntlr, struct SdSwitchFuncParam *param,
    uint8_t *status, uint32_t len)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};

    cmd.cmdCode = SD_CMD_SWITCH_FUNC;
    /*
     * [31]Mode. The switch command can be used in two modes:
     * Mode 0 (Check function) is used to query if the card supports a specific function or functions.
     * Mode 1 (set function) is used to switch the functionality of the card.
     */
    cmd.argument = (param->mode << 31) | 0x00FFFFFF;
    /*
     * [30:24] reserved(All '0'); [23:20] reserved for function group 6(All '0' or 0xF);
     * [19:16] reserved for function group 5(All '0' or 0xF); [15:12] function group 4 for Power Limit;
     * [11:8] function group 3 for Drive Strength; [7:4] function group 2 for command system;
     * [3:0] function group 1 for access mode.
     */
    cmd.argument &= ~(0xFU << (param->group * 4));
    cmd.argument |= (param->value & 0xF) << (param->group * 4);
    cmd.respType = MMC_RESP_SPI_R1 | MMC_RESP_R1 | MMC_CMD_TYPE_ADTC;
    /*
     * As a response to the switch function command, the SD Memory Card will send R1 response on the CMD line,
     * and 512 bits of status on the DAT lines.
     */
    data.blockSize = len;
    data.blockNum = 1;
    data.dataFlags = DATA_READ;
    data.dataBuffer = status;

    return MmcSendCmd(cntlr, &cmd, &data, 1);
}

static int32_t SdDecodeScr(struct MmcCntlr *cntlr)
{
    struct SdScr *scr = NULL;
    uint32_t *rawScr = NULL;
    struct SdDevice *sdDev = NULL;
    uint32_t scrStruct;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    sdDev = (struct SdDevice *)cntlr->curDev;
    rawScr = sdDev->reg.rawScr;
    scr = &(sdDev->reg.scr);

    /*
     * SCR_STRUCTURE: [63:60];
     * SCR_STRUCTURE equal 0, SCR version 1.0; SCR_STRUCTURE equal 1-15, reserved.
     */
    scrStruct = (uint8_t)MmcParseBits(rawScr, SCR_BITS, 60, 4);
    if (scrStruct != 0) {
        HDF_LOGE("SdDecodeScr: scrStruct is invalid!");
        return HDF_ERR_INVALID_PARAM;
    }
    /*
     * SD_SPEC: [59:56].
     * Describes the SD Memory Card Physical Layer Specification version supported by this card.
     * SD_SPEC = 0 , Version 1.0-1.01; SD_SPEC = 1, Version 1.1; SD_SPEC = 2, Version >= 2.0.
     */
    scr->sdSpec = (uint8_t)MmcParseBits(rawScr, SCR_BITS, 56, 4);
    if (scr->sdSpec == SD_SCR_SPEC_2) {
        /* SD_SPEC3: [47:47] */
        scr->sdSpec3 = (uint8_t)MmcParseBits(rawScr, SCR_BITS, 47, 1);
    }
    /*
     * SD_BUS_WIDTHS: [51:48].
     * SD_BUS_WIDTHS equal 0, 1 bit (DAT0); SD_BUS_WIDTHS equal 2, 4 bit (DAT0-3).
     */
    scr->sdBusWidths = (uint8_t)MmcParseBits(rawScr, SCR_BITS, 48, 4);
    /* CMD_SUPPORT: [35:32] */
    scr->cmdSupport = (uint8_t)MmcParseBits(rawScr, SCR_BITS, 32, 2);
    return HDF_SUCCESS;
}

static int32_t SdDecodeSSr(struct MmcCntlr *cntlr, uint32_t *rawSsr, uint32_t len)
{
    struct SdSsr *ssr = NULL;
    struct SdScr *scr = NULL;
    struct SdDevice *sdDev = NULL;
    uint32_t eraseSize, eraseTimeout;

    if (cntlr == NULL || cntlr->curDev == NULL || rawSsr == NULL || len == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    sdDev = (struct SdDevice *)cntlr->curDev;
    ssr = &(sdDev->reg.ssr);
    scr = &(sdDev->reg.scr);
    /* SPEED_CLASS: [447:440] */
    ssr->speedClass = MmcParseBits(rawSsr, SSR_BITS, 440, 8);
    /* AU_SIZE: [431: 428]. */
    ssr->auSize = MmcParseBits(rawSsr, SSR_BITS, 428, 4);
    if (ssr->auSize > 0) {
        if ((ssr->auSize <= MMC_MAX_BLOCKSIZE_SHIFT) || scr->sdSpec3 > 0) {
            ssr->auValue = 1 << (ssr->auSize + 4);
            /*
             * ERASE_SIZE: [423:408]. If this field is set to 0, the erase timeout calculation is not supported.
             * ERASE_SIZE = 1, value = 1AU; ERASE_SIZE = 2, value = 2AU...
             */
            eraseSize = MmcParseBits(rawSsr, SSR_BITS, 408, 16);
            /* ERASE_TIMEOUT: [407:402] */
            eraseTimeout = MmcParseBits(rawSsr, SSR_BITS, 402, 6);
            if (eraseSize > 0) {
                ssr->eraseTimeout = (eraseTimeout * 1000) / eraseSize;
                /* ERASE_OFFSET: [401:400] */
                ssr->eraseOffset = 1000 * MmcParseBits(rawSsr, SSR_BITS, 400, 2);
            }
        } else {
            HDF_LOGD("SD Status: Invalid AU.");
        }
    }
    /* UHS_SPEED_GRADE: [399:396] */
    ssr->uhsSpeedGrade = MmcParseBits(rawSsr, SSR_BITS, 396, 4);
    return HDF_SUCCESS;
}

static void SdDecodeCid(struct MmcCntlr *cntlr)
{
    struct MmcCid *cid = NULL;
    uint32_t *rawCid = NULL;
    uint32_t i;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return;
    }

    rawCid = cntlr->curDev->reg.rawCid;
    cid = &(cntlr->curDev->reg.cid);
    /* Manufacturer ID(MID): [127:120] */
    cid->mid = MmcParseBits(rawCid, CID_BITS, 120, 8);
    /* OEM/Application ID(OID): [119:104] */
    cid->oid = MmcParseBits(rawCid, CID_BITS, 104, 16);
    /* Product name(PNM): [103:64] */
    for (i = 0; i < 5; i++) {
        cid->pnm[i] = (char)MmcParseBits(rawCid, CID_BITS, CID_PNM_START_BIT - (i * BITS_PER_BYTE), BITS_PER_BYTE);
    }
    cid->pnm[5] = '\0';
    /*
     * Product revision(PRV): [63:56].
     * The product revision is composed of two Binary Coded Decimal (BCD) digits, four bits each,
     * representingan "n.m" revision number.
     */
    cid->hwPrv = MmcParseBits(rawCid, CID_BITS, 60, 4);
    cid->fwPrv = MmcParseBits(rawCid, CID_BITS, 56, 4);
    /* Product serial number(PSN): [55:24] */
    cid->psn = MmcParseBits(rawCid, CID_BITS, 24, 32);
    /*
     * Manufacturing date(MDT): [19:8].
     * The manufacturing date composed of two hexadecimal digits, one is 8 bit representing the year(y)
     * and the other is four bits representing the month(m).
     * The "m" field [11:8] is the month code. 1 = January.
     * The "y" field [19:12] is the year code. 0 = 2000.
     */
    cid->year = MmcParseBits(rawCid, CID_BITS, 12, 8) + 2000;
    cid->month = MmcParseBits(rawCid, CID_BITS, 8, 4);
}

static void SdSetBlockCapacity(struct MmcCntlr *cntlr)
{
    struct SdDevice *sdDev = (struct SdDevice *)cntlr->curDev;
    uint32_t gibVal, mibVal;

    sdDev->mmc.eraseSize = sdDev->mmc.reg.csd.eraseSize;
    sdDev->mmc.capacity = sdDev->mmc.reg.csd.capacity <<
        (sdDev->mmc.reg.csd.readBlkLen - MMC_MAX_BLOCKSIZE_SHIFT);

    gibVal = sdDev->mmc.capacity >> 21;
    mibVal = (sdDev->mmc.capacity & ~(gibVal << 21)) >> 11;
    HDF_LOGD("SD dev capacity %d.%d Gib", gibVal, mibVal);
}

static void SdDecodeCsdRev1Field(struct MmcCntlr *cntlr, struct MmcCsd *csd, uint32_t *rawCsd)
{
    uint32_t unit, factor;

    /* TAAC: [119:112]; TAAC bit position-->Time unit: [2:0], Multiplier factor: [6:3]. */
    factor = MmcParseBits(rawCsd, CSD_BITS, 115, 4);
    unit = MmcParseBits(rawCsd, CSD_BITS, 112, 3);
    csd->taccNs = (g_taccUnit[unit] * g_commFactor[factor] + 9) / 10;
    /* NSAC: [111:104], the unit for NSAC is 100 clock cycles */
    csd->taccClks = MmcParseBits(rawCsd, CSD_BITS, 104, 8) * 100;

    /* TRAN_SPEED: [103:96]; TRAN_SPEED bit-->Frequency unit: [2:0], Multiplier factor: [6:3]. */
    factor = MmcParseBits(rawCsd, CSD_BITS, 99, 4);
    unit = MmcParseBits(rawCsd, CSD_BITS, 96, 3);
    csd->maxDtr = g_tranSpeedUnit[unit] * g_commFactor[factor];
    /* card command classes: [95:84] */
    csd->ccc = MmcParseBits(rawCsd, CSD_BITS, 84, 12);
    /* C_SIZE: [73:62] */
    unit = MmcParseBits(rawCsd, CSD_BITS, 62, 12);
    /* C_SIZE_MULT: [49:47] */
    factor = MmcParseBits(rawCsd, CSD_BITS, 47, 3);
    csd->capacity = (1 + unit) << (factor + 2);
    /* READ_BL_LEN: [83:80]. */
    csd->readBlkLen = MmcParseBits(rawCsd, CSD_BITS, 80, 4);
    /* READ_BL_PARTIAL: [79:79] */
    csd->rdPartial = MmcParseBits(rawCsd, CSD_BITS, 79, 1);
    /* WRITE_BLK_MISALIGN: [78:78] */
    csd->wrMisalign = MmcParseBits(rawCsd, CSD_BITS, 78, 1);
    /* READ_BLK_MISALIGN: [77:77] */
    csd->rdMisalign = MmcParseBits(rawCsd, CSD_BITS, 77, 1);
    /* Write speed factor(R2W_FACTOR) :[28:26] */
    csd->r2wFactor = MmcParseBits(rawCsd, CSD_BITS, 26, 3);
    /* WRITE_BL_LEN: [25:22] */
    csd->writeBlkLen = MmcParseBits(rawCsd, CSD_BITS, 22, 4);
    /* WRITE_BL_PARTIAL: [21:21] */
    csd->wrPartial = MmcParseBits(rawCsd, CSD_BITS, 21, 1);
    /*
     * erase single block enable(ERASE_BLK_EN): [46:46]
     * If ERASE_BLK_EN is '0' erase area is unit of SECTOR_SIZE.
     * if ERASE_BLK_EN is '1' erase area is unit of SECTOR_SIZE or unit of WRITE_BL_LEN.
     */
    if (MmcParseBits(rawCsd, CSD_BITS, 46, 1) == 1) {
        csd->eraseSize = 1;
    } else if (csd->writeBlkLen >= MMC_MAX_BLOCKSIZE_SHIFT) {
        /*
         * erase sector size(SECTOR_SIZE): [45:39].
         * The actual size is computed by increasing this number by one.
         * A value of zero means 1 write block, 127 means 128 write blocks.
         */
        csd->eraseSize = MmcParseBits(rawCsd, CSD_BITS, 39, 7) + 1;
        csd->eraseSize <<= (csd->writeBlkLen - MMC_MAX_BLOCKSIZE_SHIFT);
    }
}

static void SdDecodeCsdRev2Field(struct MmcCntlr *cntlr, struct MmcCsd *csd, uint32_t *rawCsd)
{
    uint32_t unit, factor;

    cntlr->curDev->state.bits.blockAddr = 1;
    /* TRAN_SPEED: [103:96]; TRAN_SPEED bit-->Frequency unit: [2:0], Multiplier factor: [6:3]. */
    factor = MmcParseBits(rawCsd, CSD_BITS, 99, 4);
    unit = MmcParseBits(rawCsd, CSD_BITS, 96, 3);
    csd->maxDtr = g_tranSpeedUnit[unit] * g_commFactor[factor];
    /* card command classes: [95:84] */
    csd->ccc = MmcParseBits(rawCsd, CSD_BITS, 84, 12);
    /* device size(C_SIZE): [69:48] */
    csd->cSize = MmcParseBits(rawCsd, CSD_BITS, 48, 22);
    /* The Minimum value of C_SIZE for SDXC in CSD Version 2.0 is 00FFFFh(65535). */
    if (csd->cSize >= 0xFFFF) {
        cntlr->curDev->state.bits.sdxc = 1;
    }
    /* memory capacity = (C_SIZE + 1) * 512KByte */
    csd->capacity = (1 + csd->cSize) << 10;

    csd->taccNs = 0;
    csd->taccClks = 0;
    csd->rdPartial = 0;
    csd->readBlkLen = MMC_MAX_BLOCKSIZE_SHIFT;
    csd->rdMisalign = 0;
    csd->wrMisalign = 0;
    csd->writeBlkLen = MMC_MAX_BLOCKSIZE_SHIFT;
    csd->wrPartial = 0;
    csd->r2wFactor = 2;
    csd->eraseSize = 1;
}

static int32_t SdDecodeCsd(struct MmcCntlr *cntlr)
{
    struct MmcCsd *csd = NULL;
    uint32_t *rawCsd = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    rawCsd = cntlr->curDev->reg.rawCsd;
    csd = &(cntlr->curDev->reg.csd);

    /* CSD_STRUCTURE: [127:126]. */
    csd->structure = MmcParseBits(rawCsd, CSD_BITS, 126, 2);
    if (csd->structure == 0) {
        /* CSD Version 1.0 */
        SdDecodeCsdRev1Field(cntlr, csd, rawCsd);
    } else if (csd->structure == 1) {
        /* CSD Version 2.0 */
        SdDecodeCsdRev2Field(cntlr, csd, rawCsd);
    } else {
        HDF_LOGE("SdDecodeCsd: not support, structure = %d.", csd->structure);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static uint32_t SdGetMaxClock(struct MmcCntlr *cntlr)
{
    uint32_t clock = 0xFFFFFFFF;
    struct MmcDevice *mmcDev = cntlr->curDev;
    struct SdDevice *sdDev = (struct SdDevice *)mmcDev;

    if (mmcDev->state.bits.highSpeed == 1) {
        if (sdDev->reg.swCaps.hsMaxDtr > 0) {
            clock = sdDev->reg.swCaps.hsMaxDtr;
        }
    } else if (mmcDev->reg.csd.maxDtr > 0) {
        clock = mmcDev->reg.csd.maxDtr;
    }

    if (clock > cntlr->freqMax) {
        clock = cntlr->freqMax;
    }

    return clock;
}

static int32_t SdSelect(struct MmcCntlr *cntlr, uint32_t *rocr)
{
    int err;
    bool try = false;
    union MmcOcr ocr = cntlr->curDev->reg.ocr;

    do {
        /* dev state: ready -> idle. */
        MmcGoIdleState(cntlr);
        /* dev state: idle -> idle. */
        err = SdCmdSendIfCond(cntlr, cntlr->curDev->reg.ocr.ocrData);
        if (err == HDF_SUCCESS) {
            ocr.bits.hcs = 1;
        }
        if (cntlr->caps.bits.xpc330 == 1 || cntlr->caps.bits.xpc300 == 1 || cntlr->caps.bits.xpc180 == 1) {
            ocr.bits.sdXpc = 1;
        }
        if (MmcCntlrSupportUhs(cntlr) == true && try == false) {
            ocr.bits.s18r = 1;
        }
        /* dev state: idle -> ready. */
        err = SdAcmdOpCond(cntlr, ocr.ocrData, rocr);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("SdSelect: acmd41 fail, err = %d.", err);
            return err;
        }

        /* host not support sd procotol 3.0 */
        if (cntlr->caps.bits.sdSupportProtocol3 == 0) {
            break;
        }
        try = false;
        /*
         * Send cmd 11 to switch the volt. If host or device do not support,
         * we need set 3.3v again.
         */
        if (rocr != NULL && (*rocr & 0x41000000) == 0x41000000) {
            /* switch host and card to 1.8V
             * dev state: ready -> ready.
             */
            err = SdCmdSwitchVoltage(cntlr);
            if (err == HDF_SUCCESS) {
                err = MmcCntlrSwitchVoltage(cntlr, VOLT_1V8);
            }
            if (err != HDF_SUCCESS) {
                ocr.bits.s18r = 0;
                try = true;
            }
        }
    }while (try == true);

    /* get cid, dev state: ready -> ident. */
    err = MmcAllSendCid(cntlr);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdSelect: cmd2 fail, err = %d.", err);
        return err;
    }
    SdDecodeCid(cntlr);
    return err;
}

static int32_t SdReadCsd(struct MmcCntlr *cntlr)
{
    int32_t error;

    error = MmcSendCsd(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdReadCsd: cmd9 fail, error = %d.", error);
        return error;
    }
    return SdDecodeCsd(cntlr);
}

static int32_t SdReadScr(struct MmcCntlr *cntlr)
{
    int32_t error;
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;

    error = SdAppSendScr(cntlr, dev->reg.rawScr, SCR_LEN);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdReadScr: acmd51 fail, error = %d.", error);
        return error;
    }
    return SdDecodeScr(cntlr);
}

static int32_t SdReadSsr(struct MmcCntlr *cntlr)
{
    int32_t err;
    uint32_t rawSsr[SSR_LEN] = {0};

    /* don't support ACMD. */
    if ((cntlr->curDev->reg.csd.ccc & MMC_CSD_CCC_APP_SPEC) == 0) {
        return HDF_SUCCESS;
    }

    err = SdAcmdSdStatus(cntlr, rawSsr, SSR_LEN);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdReadSsr: acmd13 fail, err = %d.", err);
        return err;
    }
    return SdDecodeSSr(cntlr, rawSsr, SSR_LEN);
}

static int32_t SdReadSwitchCapbility(struct MmcCntlr *cntlr)
{
    int32_t err;
    uint8_t status[SD_SWITCH_FUNCTION_STATUS_BYTES_LEN] = {0};
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;
    struct SdSwitchFuncParam param = {0};

    if (dev->reg.scr.sdSpec < SD_SCR_SPEC_1) {
        return HDF_SUCCESS;
    }
    if ((cntlr->curDev->reg.csd.ccc & MMC_CSD_CCC_SWITCH) == 0) {
        return HDF_SUCCESS;
    }

    param.mode = SD_SWITCH_FUNC_MODE_CHECK;
    param.group = SD_SWITCH_FUNC_GROUP_1;
    param.value = 1;
    /* The data(status) is in reverse order relative to the protocol. */
    err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdReadSwitchCapbility: swutch func group 1 fail, err = %d.", err);
        return err;
    }
    /* [415:400]Function Group 1 information, [407:400]-->status[13]. */
    if ((status[13] & SD_BUS_SPEED_MODE_HS) > 0) {
        dev->reg.swCaps.hsMaxDtr = SD_HIGH_SPEED_MAX_DTR;
    }

    if (dev->reg.scr.sdSpec3 == 1) {
        dev->reg.swCaps.sdSpec3BusMode.data = status[13];
        param.group = SD_SWITCH_FUNC_GROUP_3;
        err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("SdReadSwitchCapbility: swutch func group 3 fail!");
            return err;
        }
        /* [447:432]Function Group 3 information, [447:440]-->status[9]. */
        dev->reg.swCaps.sdSpec3DrvType = (enum SdDrvType)status[9];
        param.group = SD_SWITCH_FUNC_GROUP_4;
        err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("SdReadSwitchCapbility: swutch func group 4 fail!");
            return err;
        }
        /* [463:448]Function Group 4 information, [463:456]-->status[7]. */
        dev->reg.swCaps.sdSpec3CurrLimit = (enum SdMaxCurrentLimitBit)status[7];
    }
    return HDF_SUCCESS;
}

static void SdFillBusSpeedMode(struct MmcCntlr *cntlr)
{
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;

    if (MmcCntlrSupportUhs(cntlr) == false || dev->reg.scr.sdSpec3 == 0) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_DS;
        return;
    }

    /* select max speed mode supported by host and device. */
    if ((cntlr->caps.bits.uhsSdr104 == 1) && (dev->reg.swCaps.sdSpec3BusMode.bits.uhsSdr104 == 1)) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_UHS_SDR104;
    } else if ((cntlr->caps.bits.uhsDdr50 == 1) && (dev->reg.swCaps.sdSpec3BusMode.bits.uhsDdr50 == 1)) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_UHS_DDR50;
    } else if ((cntlr->caps.bits.uhsSdr104 == 1 || cntlr->caps.bits.uhsSdr50 == 1) &&
        (dev->reg.swCaps.sdSpec3BusMode.bits.uhsSdr50 == 1)) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_UHS_SDR50;
    } else if ((cntlr->caps.bits.uhsSdr104 == 1 || cntlr->caps.bits.uhsSdr50 == 1 ||
        cntlr->caps.bits.uhsSdr25 == 1) && (dev->reg.swCaps.sdSpec3BusMode.bits.uhsSdr25 == 1)) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_UHS_SDR25;
    } else if ((cntlr->caps.bits.uhsSdr104 == 1 || cntlr->caps.bits.uhsSdr50 == 1 ||
        cntlr->caps.bits.uhsSdr25 == 1 || cntlr->caps.bits.uhsSdr12 == 1) &&
        (dev->reg.swCaps.sdSpec3BusMode.bits.uhsSdr12 == 1)) {
        dev->busSpeedMode = SD_BUS_SPEED_MODE_UHS_SDR12;
    }
}

static int32_t SdReadRegisters(struct MmcCntlr *cntlr)
{
    int32_t error;

    /* get SCR */
    error = SdReadScr(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }

    /* get SSR */
    error = SdReadSsr(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }

    /* get sw cap */
    error = SdReadSwitchCapbility(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }

    if (MmcCntlrDevReadOnly(cntlr) == true) {
        cntlr->curDev->state.bits.readonly = 1;
    }
    SdFillBusSpeedMode(cntlr);
    return HDF_SUCCESS;
}

static int32_t SdExecuteTuning(struct MmcCntlr *cntlr)
{
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;

    if (dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_DDR50) {
        return MmcCntlrTune(cntlr, SD_CMD_SWITCH_FUNC);
    }
    if ((dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_SDR104) ||
        (dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_SDR50)) {
        return MmcCntlrTune(cntlr, SD_CMD_SEND_TUNING_BLOCK);
    }
    return HDF_SUCCESS;
}

static uint32_t SdGetDevMaxCurrentLimitValue(enum SdMaxCurrentLimitBit devCap)
{
    uint32_t currentLimit = 0;

    if (devCap == SD_MAX_CURRENT_LIMIT_800) {
        currentLimit = SD_MAX_CURRENT_LIMIT_800_VALUE;
    } else if (devCap == SD_MAX_CURRENT_LIMIT_600) {
        currentLimit = SD_MAX_CURRENT_LIMIT_600_VALUE;
    } else if (devCap == SD_MAX_CURRENT_LIMIT_400) {
        currentLimit = SD_MAX_CURRENT_LIMIT_400_VALUE;
    } else if (devCap == SD_MAX_CURRENT_LIMIT_200) {
        currentLimit = SD_MAX_CURRENT_LIMIT_200_VALUE;
    }
    return currentLimit;
}

static uint32_t SdGetMaxCurrentLimitValue(union MmcCaps *hostCap, enum SdMaxCurrentLimitBit devCap)
{
    uint32_t currentLimit;

    /* get max support by dev. */
    currentLimit = SdGetDevMaxCurrentLimitValue(devCap);
    if (hostCap->bits.maxCurrentLimit800 == 1) {
        currentLimit = ((currentLimit < SD_MAX_CURRENT_LIMIT_800_VALUE) ?
            currentLimit : SD_MAX_CURRENT_LIMIT_800_VALUE);
    } else if (hostCap->bits.maxCurrentLimit600 == 1) {
        currentLimit = ((currentLimit < SD_MAX_CURRENT_LIMIT_600_VALUE) ?
            currentLimit : SD_MAX_CURRENT_LIMIT_600_VALUE);
    } else if (hostCap->bits.maxCurrentLimit400 == 1) {
        currentLimit = ((currentLimit < SD_MAX_CURRENT_LIMIT_400_VALUE) ?
            currentLimit : SD_MAX_CURRENT_LIMIT_400_VALUE);
    } else if (hostCap->bits.maxCurrentLimit200 == 1) {
        currentLimit = ((currentLimit < SD_MAX_CURRENT_LIMIT_200_VALUE) ?
            currentLimit : SD_MAX_CURRENT_LIMIT_200_VALUE);
    }
    return currentLimit;
}

static int32_t SdSetMaxCurrentLimit(struct MmcCntlr *cntlr)
{
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;
    uint8_t status[SD_SWITCH_FUNCTION_STATUS_BYTES_LEN] = {0};
    struct SdSwitchFuncParam param = {0};
    uint32_t currentLimit;
    int32_t err;

    if ((dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_SDR104) ||
        (dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_DDR50) ||
        (dev->busSpeedMode == SD_BUS_SPEED_MODE_UHS_SDR50)) {
        currentLimit = SdGetMaxCurrentLimitValue(&(cntlr->caps), dev->reg.swCaps.sdSpec3CurrLimit);
    } else {
        currentLimit = SD_MAX_CURRENT_LIMIT_200_VALUE;
    }

    param.mode = SD_SWITCH_FUNC_MODE_SET;
    param.group = SD_SWITCH_FUNC_GROUP_4;
    param.value = currentLimit;
    /* Current Limit is selected by CMD6 Function Group 4. */
    err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdSetMaxCurrentLimit: swutch func group 3 fail!");
        return err;
    }
    if (((status[15] >> 4) & 0x0F) != currentLimit) {
        HDF_LOGD("SdSetMaxCurrentLimit: status not match!");
    }

    return HDF_SUCCESS;
}

static int32_t SdSetBusSpeedMode(struct MmcCntlr *cntlr)
{
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;
    uint8_t status[SD_SWITCH_FUNCTION_STATUS_BYTES_LEN] = {0};
    struct SdSwitchFuncParam param = {0};
    int32_t err;
    enum MmcBusTiming timing;

    switch (dev->busSpeedMode) {
        case SD_BUS_SPEED_MODE_UHS_SDR104:
            timing = BUS_TIMING_UHS_SDR104;
            dev->reg.swCaps.uhsMaxDtr = SD_UHS_SDR104_MAX_DTR;
            break;
        case SD_BUS_SPEED_MODE_UHS_DDR50:
            timing = BUS_TIMING_UHS_DDR50;
            dev->reg.swCaps.uhsMaxDtr = SD_UHS_DDR50_MAX_DTR;
            break;
        case SD_BUS_SPEED_MODE_UHS_SDR50:
            timing = BUS_TIMING_UHS_SDR50;
            dev->reg.swCaps.uhsMaxDtr = SD_UHS_SDR50_MAX_DTR;
            break;
        case SD_BUS_SPEED_MODE_UHS_SDR25:
            timing = BUS_TIMING_UHS_SDR25;
            dev->reg.swCaps.uhsMaxDtr = SD_UHS_SDR25_MAX_DTR;
            break;
        case SD_BUS_SPEED_MODE_UHS_SDR12:
            timing = BUS_TIMING_UHS_SDR12;
            dev->reg.swCaps.uhsMaxDtr = SD_UHS_SDR12_MAX_DTR;
            break;
        default:
            return HDF_SUCCESS;
    }
    /* Bus Speed Mode is selected by CMD6 Function Group 1. */
    param.mode = SD_SWITCH_FUNC_MODE_SET;
    param.group = SD_SWITCH_FUNC_GROUP_1;
    param.value = dev->busSpeedMode;
    err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdSetBusSpeedMode: swutch func group 1 fail!");
        return err;
    }
    if ((status[16] & 0xF) != dev->busSpeedMode) {
        HDF_LOGD("SdSetBusSpeedMode: status not match!");
    } else {
        MmcCntlrSetBusTiming(cntlr, timing);
        MmcCntlrSetClock(cntlr, dev->reg.swCaps.uhsMaxDtr);
    }

    return HDF_SUCCESS;
}

static int32_t SdSwitch4BitBusWidth(struct MmcCntlr *cntlr, struct SdDevice *dev)
{
    int32_t err = HDF_SUCCESS;

    if ((dev->reg.scr.sdBusWidths & SD_SCR_BUS_WIDTHS_4) > 0 &&
        (cntlr->caps.bits.cap4Bit > 0)) {
        err = SdAcmdSetBusWidth(cntlr, BUS_WIDTH4);
        if (err != HDF_SUCCESS) {
            HDF_LOGE("SdSwitch4BitBusWidth: set 4-bits bus width fail!");
            return err;
        }
        MmcCntlrSetBusWidth(cntlr, BUS_WIDTH4);
    }
    return err;
}

static int32_t SdUltraHighSpeedDevInit(struct MmcCntlr *cntlr)
{
    int32_t err;
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;

    if (dev->reg.scr.sdSpec3 == 0) {
        return HDF_SUCCESS;
    }
    if ((cntlr->curDev->reg.csd.ccc & MMC_CSD_CCC_SWITCH) == 0) {
        return HDF_SUCCESS;
    }

    err = SdSwitch4BitBusWidth(cntlr, dev);
    if (err != HDF_SUCCESS) {
        return err;
    }
    err = SdSetMaxCurrentLimit(cntlr);
    if (err != HDF_SUCCESS) {
        return err;
    }
    err = SdSetBusSpeedMode(cntlr);
    if (err != HDF_SUCCESS) {
        return err;
    }
    return SdExecuteTuning(cntlr);
}

static int32_t SdSwitchHighSpeed(struct MmcCntlr *cntlr)
{
    int32_t err;
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;
    uint8_t status[SD_SWITCH_FUNCTION_STATUS_BYTES_LEN] = {0};
    struct SdSwitchFuncParam param = {0};

    if (dev->reg.swCaps.hsMaxDtr == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (dev->reg.scr.sdSpec < SD_SCR_SPEC_1) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if (cntlr->caps.bits.highSpeed == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    if ((cntlr->curDev->reg.csd.ccc & MMC_CSD_CCC_SWITCH) == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    /* Bus Speed Mode is selected by CMD6 Function Group 1. */
    param.mode = SD_SWITCH_FUNC_MODE_SET;
    param.group = SD_SWITCH_FUNC_GROUP_1;
    param.value = SD_BUS_SPEED_MODE_HS;
    err = SdCmdSwitchFunc(cntlr, &param, status, SD_SWITCH_FUNCTION_STATUS_BYTES_LEN);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdSwitchHighSpeed: switch func group 1 fail!");
        return err;
    }
    if ((status[16] & 0xF) != SD_BUS_SPEED_MODE_HS) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t SdHighSpeedDevInit(struct MmcCntlr *cntlr)
{
    int err;
    struct SdDevice *sdDev = (struct SdDevice *)cntlr->curDev;

    err = SdSwitchHighSpeed(cntlr);
    if (err == HDF_SUCCESS) {
        cntlr->curDev->state.bits.highSpeed = 1;
        MmcCntlrSetBusTiming(cntlr, BUS_TIMING_SD_HS);
    } else if (err != HDF_ERR_NOT_SUPPORT) {
        HDF_LOGE("SdHighSpeedDevInit: switch high speed fail!");
        return err;
    }
    MmcCntlrSetClock(cntlr, SdGetMaxClock(cntlr));
    return SdSwitch4BitBusWidth(cntlr, sdDev);
}

static int32_t SdDeviceAdd(struct MmcCntlr *cntlr)
{
    /* The SD dev must be removable. */
    cntlr->curDev->state.bits.removable = 1;
    SdSetBlockCapacity(cntlr);
    /* add dev. */
    if (MmcDeviceAdd(cntlr->curDev) != HDF_SUCCESS) {
        HDF_LOGE("SdDeviceAdd: add device fail!");
        return HDF_FAILURE;
    }
    cntlr->curDev->state.bits.present = 1;
    return HDF_SUCCESS;
}

static int32_t SdInit(struct MmcCntlr *cntlr)
{
    int32_t error;
    union MmcOcr ocr = {0};

    /* acmd41, detect sd dev and get the voltage range. dev state: idle -> ready. */
    error = SdAcmdOpCond(cntlr, 0, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("acmd41(detect sd) fail, err = %d!", error);
        return error;
    }

    MmcCntlrSelectWorkVoltage(cntlr, &ocr);
    if (cntlr->curDev->reg.ocr.ocrData == 0) {
        HDF_LOGE("SD work voltage is invalid!");
        return HDF_ERR_INVALID_PARAM;
    }
    error = SdSelect(cntlr, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        return error;
    }
    /* get RCA. dev state: ident -> stby. */
    error = SdCmdSendRelativeAddr(cntlr, &(cntlr->curDev->reg.rca));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd3(get RCA) fail!, error = %d.", error);
        return error;
    }
    /* get CSD, CMD9 should send in stby. dev state: stby -> stby. */
    error = SdReadCsd(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("sd read csd fail!, error = %d.", error);
        return error;
    }
    /* select card. dev state: stby -> tran. */
    error = MmcSelectCard(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd7(select card) fail!, error = %d.", error);
        return error;
    }
    /* dev state: tran -> tran. */
    error = SdReadRegisters(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }

    if (ocr.bits.s18r == 1) {
        /* uhs dev set */
        error = SdUltraHighSpeedDevInit(cntlr);
        if (error != HDF_SUCCESS) {
            return error;
        }
        cntlr->curDev->state.bits.uhs = 1;
    } else {
        /* highspeed dev set */
        error = SdHighSpeedDevInit(cntlr);
        if (error != HDF_SUCCESS) {
            return error;
        }
    }

    return SdDeviceAdd(cntlr);
}

static int32_t SdDetect(struct MmcCntlr *cntlr)
{
    int32_t ret;

    HDF_LOGD("Detect sd dev start...");
    /* dev state: idle. */
    MmcGoIdleState(cntlr);
    /* dev state: idle -> idle. */
    (void)SdCmdSendIfCond(cntlr, cntlr->ocrDef.ocrData);
    /* Initialize SD. */
    ret = SdInit(cntlr);
    if (ret == HDF_SUCCESS) {
        HDF_LOGD("Detect sd dev success! %s dev at address 0x%x!",
            cntlr->curDev->state.bits.uhs ? "Ultra high speed" :
            (cntlr->curDev->state.bits.highSpeed ? "High speed" : ""),
            cntlr->curDev->reg.rca);
    }
    return ret;
}

static int32_t SdioSendOpCond(struct MmcCntlr *cntlr, uint32_t arg, uint32_t *ocr)
{
    struct MmcCmd cmd = {0};
    int32_t err;
    uint32_t i;

    /*
     * An SDIO aware host will send CMD5 prior to the CMD55/ACMD41 pair, and thus would receive a valid OCR in
     * the R4 response to CMD5 and continue to initialize the card.
     */
    cmd.cmdCode = SDIO_SEND_OP_COND;
    /* [23:0] OCR; [24] S18R(Switching to 1.8V Request); [31:25] Stuff Bits. */
    cmd.argument = arg;
    cmd.respType = MMC_RESP_R4 | MMC_CMD_TYPE_BCR;
    for (i = 0; i < INIT_CMD_RETRY_TIMES; i++) {
        err = MmcSendCmd(cntlr, &cmd, NULL, MMC_CMD_DEFAULT_RETRY_TIMES);
        if (err != HDF_SUCCESS) {
            break;
        }
        if (arg == 0) {
            break;
        }
        if ((cmd.resp[0] & MMC_CARD_BUSY_STATUS) > 0) {
            break;
        }

        err = HDF_ERR_TIMEOUT;
        OsalMDelay(10);
    }
    if (ocr != NULL) {
        /*
         * Response R4 in Sd mode: [23:0] OCR; [24] S18A; [26:25] Stuff Bits;
         * [27] Memory Present; [30: 28] Number of I/O functions.
         */
        *ocr = cmd.resp[0];
    }

    return err;
}

static int32_t SdioRespR5Check(struct MmcCmd *cmd)
{
    if (cmd->resp[0] & SDIO_R5_ERROR) {
        HDF_LOGE("R5: error!");
        return HDF_ERR_IO;
    }
    if (cmd->resp[0] & SDIO_R5_OUT_OF_RANGE) {
        HDF_LOGE("R5: out of range error!");
        return HDF_ERR_INVALID_PARAM;
    }
    if (cmd->resp[0] & SDIO_R5_FUNCTION_NUMBER) {
        HDF_LOGE("R5: func num error!");
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

int32_t SdioRwDirect(struct MmcCntlr *cntlr, struct SdioCmdParam *param, uint8_t *out)
{
    struct MmcCmd cmd = {0};
    int32_t err;

    if (cntlr == NULL || param == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /*
     * The IO_RW_DIRECT is the simplest means to access a single register within the total 128K of register space
     * in any I/O function, including the common I/O area (CIA). This command reads or writes 1 byte using only 1
     * command/response pair. A common use is to initialize registers or monitor status values for I/O functions.
     */
    cmd.cmdCode = SDIO_RW_DIRECT;
    /* [31] R/W flag. */
    cmd.argument = ((param->writeflag == true) ? 0x80000000 : 0x00000000);
    /* [30:28] Function Number. */
    cmd.argument |= (param->funcNum << 28);
    /* [25:9] Register Address. */
    cmd.argument |= (param->regAddr << 9);
    /* [7:0] Write Data or Stuff Bits. */
    cmd.argument |= param->writeData;
    cmd.respType = MMC_RESP_SPI_R5 | MMC_RESP_R5 | MMC_CMD_TYPE_AC;
    err = MmcSendCmd(cntlr, &cmd, NULL, 1);
    if (err != HDF_SUCCESS) {
        return err;
    }

    /* resp error check. */
    err = SdioRespR5Check(&cmd);
    if (err != HDF_SUCCESS) {
        return err;
    }
    if (out != NULL) {
        *out = cmd.resp[0] & 0xFF;
    }
    return HDF_SUCCESS;
}

int32_t SdioRwExtended(struct MmcCntlr *cntlr, struct SdioCmdParam *param,
    uint8_t *buf, uint32_t blockNum, uint32_t blockSize)
{
    struct MmcCmd cmd = {0};
    struct MmcData data = {0};
    int32_t err;

    if (cntlr == NULL || param == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    /* Register Address: Start Address of I/O register to read or write. Range is [1FFFFh:0]. */
    if (param->regAddr != ((param->regAddr) & 0x1FFFF)) {
        return HDF_ERR_INVALID_PARAM;
    }

    cmd.cmdCode = SDIO_RW_EXTENDED;
    /* [31] R/W flag. */
    cmd.argument = ((param->writeflag == true) ? 0x80000000 : 0x00000000);
    /* [30:28] Function Number. */
    cmd.argument |= (param->funcNum << 28);
    /* [26] Op Code.  */
    cmd.argument |= ((param->incrAddrFlag == true) ? 0x04000000 : 0x00000000);
    /* [25:9] Register Address. */
    cmd.argument |= (param->regAddr << 9);
    /*
     * [8:0] Byte/Block Count.
     * If the command is operating on bytes(Block Mode = 0), this field contains the number of bytes
     * to read or write. A value of 0 shall cause 512 bytes to be read or written.
     * If the command is in block mode(Block Mode = 1), the Block Count field specifies the number
     * of Data Blocks to be transferred following this command.
     */
    if (blockNum == 1 && blockSize <= 512) {
        cmd.argument |= ((blockSize == 512) ? 0 : blockSize); /* byte mode */
    } else {
        /* [27] Block Mode. */
        cmd.argument |= (0x08000000 | blockNum);
    }
    cmd.respType = MMC_RESP_SPI_R5 | MMC_RESP_R5 | MMC_CMD_TYPE_ADTC;

    data.blockSize = blockSize;
    data.blockNum = blockNum;
    data.dataFlags = ((param->writeflag == true) ? DATA_WRITE : DATA_READ);
    if (param->scatterFlag == false) {
        data.dataBuffer = buf;
    } else {
        data.scatter = (void *)buf;
        data.scatterLen = param->scatterLen;
    }

    err = MmcSendCmd(cntlr, &cmd, &data, 1);
    if (err != HDF_SUCCESS) {
        return err;
    }

    /* resp error check. */
    return SdioRespR5Check(&cmd);
}

static void SdioIoReset(struct MmcCntlr *cntlr)
{
    struct SdioCmdParam param = {0};
    uint8_t out = 0;
    int32_t error;

    /*
     * In order to reset an I/O only card or the I/O portion of a combo card,
     * use CMD52 to write a 1 to the RES bit in the CCCR(bit3 of register 6),
     * because it can't issue CMD52 after CMD0.
     */
    param.regAddr = IO_ABORT;
    /* read register 6 of CCCR. */
    error = SdioRwDirect(cntlr, &param, &out);
    if (error < 0) {
        out = SDIO_CCCR_RES;
    } else {
        out |= SDIO_CCCR_RES;
    }
    /* write the RES bit to 1. */
    param.writeflag = true;
    param.writeData = out;
    (void)SdioRwDirect(cntlr, &param, NULL);
}

int32_t SdioReadWriteByte(struct MmcCntlr *cntlr, bool writeFlag,
    uint32_t funcNum, uint32_t addr, uint8_t *data)
{
    struct SdioCmdParam param = {0};

    if (cntlr == NULL || data == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    param.regAddr = addr;
    param.funcNum = funcNum;
    if (writeFlag == true) {
        param.writeflag = true;
        param.writeData = *data;
        return SdioRwDirect(cntlr, &param, NULL);
    }
    return SdioRwDirect(cntlr, &param, data);
}

static int32_t SdioReadWriteRemainBytes(struct MmcCntlr *cntlr, struct SdioCmdParam *param,
    uint8_t *data, uint32_t size, uint32_t addr)
{
    uint32_t maxBlkSize, curSize;
    struct SdioDevice *dev = (struct SdioDevice *)cntlr->curDev;
    uint32_t remLen = size;
    uint32_t curAddr = addr;
    uint8_t *buffer = data;
    int32_t err;

    maxBlkSize = MMC_MIN(cntlr->maxBlkSize, dev->curFunction->maxBlkSize);
    maxBlkSize = MMC_MIN(maxBlkSize, BYTES_PER_BLOCK);
    if (maxBlkSize == 0) {
        HDF_LOGE("max block size is invalid!");
        return HDF_ERR_INVALID_PARAM;
    }

    while (remLen > 0) {
        curSize = MMC_MIN(remLen, maxBlkSize);
        param->regAddr = curAddr;
        err = SdioRwExtended(cntlr, param, buffer, 1, curSize);
        if (err != HDF_SUCCESS) {
            HDF_LOGD("SdioReadWriteBlock: bytes mode, err = %d, addr = %d, curSize = %d!", err, addr, curSize);
            return err;
        }
        buffer += curSize;
        if (param->incrAddrFlag == true) {
            curAddr += curSize;
        }
        remLen -= curSize;
    }
    return HDF_SUCCESS;
}

static void SdioFillRwExtendedCmdParam(struct SdioCmdParam *param,
    struct SdioDevice *dev, struct SdioRwBlockInfo *info)
{
    param->funcNum = dev->curFunction->funcNum;
    param->incrAddrFlag = info->incrAddrFlag;
    param->writeflag = info->writeFlag;
    if (info->scatterFlag == true) {
        param->scatterFlag = true;
        param->scatterLen = info->scatterLen;
        param->regAddr = info->addr;
    }
}

int32_t SdioReadWriteBlock(struct MmcCntlr *cntlr, struct SdioRwBlockInfo *info)
{
    uint32_t maxBlkNum, maxBlkSize, curblkNum, curSize, curAddr, remLen;
    int32_t err;
    struct SdioCmdParam param = {0};
    struct SdioDevice *dev = NULL;
    uint8_t *buffer = NULL;

    if (cntlr == NULL || info == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    maxBlkSize = MMC_MIN(cntlr->maxBlkSize, dev->curFunction->maxBlkSize);
    maxBlkSize = MMC_MIN(maxBlkSize, BYTES_PER_BLOCK);
    if (maxBlkSize == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (dev->curFunction->curBlkSize == 0 || cntlr->maxBlkNum == 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    remLen = info->size;
    curAddr = info->addr;
    buffer = info->buf;
    SdioFillRwExtendedCmdParam(&param, dev, info);
    if (info->scatterFlag == true) {
        return SdioRwExtended(cntlr, &param, buffer, MMC_MAX(1, remLen / maxBlkSize), MMC_MIN(remLen, maxBlkSize));
    }
    /* send block. */
    if (dev->sdioReg.cccr.multiBlock > 0 && (remLen > maxBlkSize)) {
        maxBlkNum = MMC_MIN((cntlr->maxReqSize / dev->curFunction->curBlkSize), cntlr->maxBlkNum);
        maxBlkNum = MMC_MIN(maxBlkNum, SDIO_BLOCK_TRANSFER_MAX_BLKNUM);
        while (remLen > dev->curFunction->curBlkSize) {
            curblkNum = remLen / dev->curFunction->curBlkSize;
            curblkNum = MMC_MIN(curblkNum, maxBlkNum);
            curSize = curblkNum * dev->curFunction->curBlkSize;
            param.regAddr = curAddr;
            err = SdioRwExtended(cntlr, &param, buffer, curblkNum, dev->curFunction->curBlkSize);
            if (err != HDF_SUCCESS) {
                return err;
            }
            buffer += curSize;
            if (info->incrAddrFlag == true) {
                curAddr += curSize;
            }
            remLen -= curSize;
        }
    }

    /* send remaind bytes. */
    return SdioReadWriteRemainBytes(cntlr, &param, buffer, remLen, curAddr);
}

static int32_t SdioCdDisable(struct MmcCntlr *cntlr)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t ctrl;

    param.regAddr = BUS_INTERFACE_CONTROL;
    /* read register 7 of CCCR. */
    error = SdioRwDirect(cntlr, &param, &ctrl);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioCdDisable: read BIC fail!");
        return error;
    }
    /*
     * write the CD Disable bit to 1.
     * This bit shall be set to 1 before issuing CMD53.
     */
    ctrl |= SDIO_CCCR_CD_DISABLE;
    param.writeflag = true;
    param.writeData = ctrl;
    return SdioRwDirect(cntlr, &param, NULL);
}

static int32_t SdioReadCccrSdioRev(struct MmcCntlr *cntlr, struct SdioCccr *cccr, uint8_t *cccrRev)
{
    struct SdioCmdParam param = {0};
    int32_t err;
    uint8_t data;

    /* read register 0 of CCCR. */
    param.regAddr = CCCR_SDIO_REVISION;
    err = SdioRwDirect(cntlr, &param, &data);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdioReadCccrSdioRev: read sdio rev fail!");
        return err;
    }
    /* bit3-bit0: CCCR_REVISION */
    *cccrRev = data & 0x0f;
    if ((*cccrRev) > SDIO_CCCR_VERSION_3_00) {
        HDF_LOGE("SdioReadCccrSdioRev: cccr rev error!");
        return HDF_FAILURE;
    }
    /* bit7-bit4: SDIO_REVISION */
    cccr->sdioRev = (data & 0xf0) >> 4;
    return HDF_SUCCESS;
}

static int32_t SdioReadCccrCapbility(struct MmcCntlr *cntlr, struct SdioCccr *cccr)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t cap;

    /* read register 8 of CCCR. */
    param.regAddr = CARD_CAPBILITY;
    error = SdioRwDirect(cntlr, &param, &cap);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioReadCccrCapbility: read card cap fail!");
        return error;
    }

    if ((cap & SDIO_CCCR_CAP_4BLS) > 0) {
        cccr->lowSpeed4Bit = 1;
    }
    if ((cap & SDIO_CCCR_CAP_LSC) > 0) {
        cccr->lowSpeed = 1;
    }
    if ((cap & SDIO_CCCR_CAP_SMB) > 0) {
        cccr->multiBlock = 1;
    }
    return HDF_SUCCESS;
}

static int32_t SdioReadCccrPowerControl(struct MmcCntlr *cntlr, struct SdioCccr *cccr)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t ctrl;

    /* read register 18 of CCCR. */
    param.regAddr = POWER_CONTROL;
    error = SdioRwDirect(cntlr, &param, &ctrl);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioReadCccrPowerControl: read power control fail!");
        return error;
    }

    if ((ctrl & SDIO_CCCR_POWER_SMPC) > 0) {
        cccr->highPower = 1;
    }
    return HDF_SUCCESS;
}

static int32_t SdioReadCccrBusSpeed(struct MmcCntlr *cntlr, struct SdioCccr *cccr)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t speed;

    /* read register 19 of CCCR. */
    param.regAddr = BUS_SPEED_SELECT;
    error = SdioRwDirect(cntlr, &param, &speed);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioReadCccrBusSpeed: read bus speed select fail!");
        return error;
    }

    if ((speed & SDIO_CCCR_BUS_SPEED_SHS) > 0) {
        cccr->highSpeed = 1;
    }
    return HDF_SUCCESS;
}

int32_t SdioReadCccrIoEnable(struct MmcCntlr *cntlr, uint8_t *val)
{
    struct SdioCmdParam param = {0};

    if (cntlr == NULL || val == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* read register 2(IOEx) of CCCR. */
    param.regAddr = IO_ENABLE;
    return SdioRwDirect(cntlr, &param, val);
}

int32_t SdioCccrIoEnable(struct MmcCntlr *cntlr)
{
    struct SdioCmdParam param = {0};
    struct SdioDevice *dev = NULL;
    int32_t err;
    uint8_t data;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* read register 2(IOEx) of CCCR. */
    param.regAddr = IO_ENABLE;
    err = SdioRwDirect(cntlr, &param, &data);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdioCccrIoEnable: read io enable fail! err = %d.", err);
        return err;
    }
    /*
     * IOEx:Enable Function x.
     * If this bit is reset to 0, the function is disable. If this bit is set to 1, the function is enabled to start
     * its initialization. The complation of initialization is indicated in IORx. On power up or after a reset, the
     * card shall reset this bit to 0. The host can also use IOEx as a per function reset for error recovery. The host
     * sequence for a per function reset is to reset IOEx to 0, wait until IORx becomes 0 and then set IOEx to 1 again.
     */
    data |= (1 << dev->curFunction->funcNum);
    param.writeflag = true;
    param.writeData = data;
    /* write register 2(IOEx) of CCCR. */
    return SdioRwDirect(cntlr, &param, NULL);
}

int32_t SdioCccrIoDisable(struct MmcCntlr *cntlr)
{
    struct SdioCmdParam param = {0};
    struct SdioDevice *dev = NULL;
    int32_t err;
    uint8_t data;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* read register 2(IOEx) of CCCR. */
    param.regAddr = IO_ENABLE;
    err = SdioRwDirect(cntlr, &param, &data);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdioCccrIoDisable: read io enable fail! err = %d.", err);
        return err;
    }

    data &= (~(1 << dev->curFunction->funcNum));
    param.writeflag = true;
    param.writeData = data;
    /* write register 2(IOEx) of CCCR. */
    return SdioRwDirect(cntlr, &param, NULL);
}

int32_t SdioReadCccrIoReady(struct MmcCntlr *cntlr, uint8_t *val)
{
    struct SdioCmdParam param = {0};

    if (cntlr == NULL || val == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* read register 3(IORx) of CCCR. */
    param.regAddr = IO_READY;
    /*
     * IORx: I/O Function x Ready.
     * If this bit is set to 0, the function is not ready to operate. This may be caused by the function being
     * disabled or not ready due to internal causes such as a built-in-self-test in progress. If this bit is set to 1,
     * the function is ready to operate. On power up or after a reset, this bit shall be set to 0.
     */
    return SdioRwDirect(cntlr, &param, val);
}

int32_t SdioReadCccrIntPending(struct MmcCntlr *cntlr, uint8_t *val)
{
    struct SdioCmdParam param = {0};

    if (cntlr == NULL || val == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* read register 5(INTx) of CCCR. */
    param.regAddr = INT_PENDING;
    /*
     * INTx: Interrupt Pending for Function x.
     * If this bit is cleared to 0, this indicates that no Interrupts are pending from this function.
     * If this bit is set to 1, then this function has Interrupt pending.
     * Note that if the IENx or IENM bits are not set, the host cannot receive this pending Interrupt.
     */
    return SdioRwDirect(cntlr, &param, val);
}

int32_t SdioCccrIntEnable(struct MmcCntlr *cntlr)
{
    int32_t err;
    uint8_t val;
    struct SdioCmdParam param = {0};
    struct SdioDevice *dev = NULL;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* read register 4 of CCCR. */
    param.regAddr = INT_ENABLE;
    err = SdioRwDirect(cntlr, &param, &val);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdioCccrIntEnable: read int enable fail! err = %d.", err);
        return err;
    }
    /*
     * [0]IENM: Interrupt Enable Master.
     * If this bit is cleared to 0, no interrupts from this card shall be sent to the host.
     * If this bit is set to 1, then any function's interrupt shall be sent to the host.
     */
    val |= 1;
    /*
     * [1-7]IENx:Interrupt Enable for Function x.
     * If this bit is cleared to 0, any interrupt form this function shall not be sent to the host.
     * If this bit is set to 1, function's interrupt shall be sent to the host if the IENM is also set to 1.
     */
    val |= (1 << dev->curFunction->funcNum);
    param.writeflag = true;
    param.writeData = val;
    /* write register 4 of CCCR. */
    return SdioRwDirect(cntlr, &param, NULL);
}

int32_t SdioCccrIntDisable(struct MmcCntlr *cntlr)
{
    struct SdioDevice *dev = (struct SdioDevice *)cntlr->curDev;
    struct SdioCmdParam param = {0};
    int32_t err;
    uint8_t val;

    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* read register 4 of CCCR. */
    param.regAddr = INT_ENABLE;
    err = SdioRwDirect(cntlr, &param, &val);
    if (err != HDF_SUCCESS) {
        HDF_LOGE("SdioCccrIntDisable: read int enable fail! err = %d.", err);
        return err;
    }
    /* clear the function's interrupt. */
    val &= (~(1U << dev->curFunction->funcNum));
    if ((val & 0xFE) == 0) {
        val = 0;
    }
    param.writeflag = true;
    param.writeData = val;
    /* write register 4 of CCCR. */
    return SdioRwDirect(cntlr, &param, NULL);
}

static int32_t SdioReadCccr(struct MmcCntlr *cntlr)
{
    int32_t ret;
    uint8_t cccrRev;
    struct SdioDevice *sdioDev = NULL;
    struct SdioCccr *cccr = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    sdioDev = (struct SdioDevice *)cntlr->curDev;
    cccr = &(sdioDev->sdioReg.cccr);
    ret = SdioReadCccrSdioRev(cntlr, cccr, &cccrRev);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = SdioReadCccrCapbility(cntlr, cccr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (cccrRev >= SDIO_CCCR_VERSION_1_10) {
        ret = SdioReadCccrPowerControl(cntlr, cccr);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    /* SDIO version 1.20 cards indicate their support for High-Speed mode with the SHS bit in the CCCR. */
    if (cccrRev >= SDIO_CCCR_VERSION_1_20) {
        ret = SdioReadCccrBusSpeed(cntlr, cccr);
    }
    return ret;
}

static int32_t SdioBusSpeedSelect(struct MmcCntlr *cntlr, bool enableHighSpeed)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t speed;

    /* read register 19 of CCCR. */
    param.regAddr = BUS_SPEED_SELECT;
    error = SdioRwDirect(cntlr, &param, &speed);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioBusSpeedSelect: read bus speed select fail! err = %d.", error);
        return error;
    }
    /* fill BSS0. */
    if (enableHighSpeed == true) {
        speed |= SDIO_CCCR_BUS_SPEED_EHS;
    } else {
        speed &= (~SDIO_CCCR_BUS_SPEED_EHS);
    }
    /* write BSS0. */
    param.writeflag = true;
    param.writeData = speed;
    return SdioRwDirect(cntlr, &param, NULL);
}

static int32_t SdioSwitchHighSpeed(struct MmcCntlr *cntlr)
{
    struct SdioDevice *sdioDev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    sdioDev = (struct SdioDevice *)cntlr->curDev;
    if (cntlr->caps.bits.highSpeed == 0 || sdioDev->sdioReg.cccr.highSpeed == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return SdioBusSpeedSelect(cntlr, true);
}

static int32_t SdioEnableHighSpeed(struct MmcCntlr *cntlr)
{
    int32_t err;

    err = SdioSwitchHighSpeed(cntlr);
    if (cntlr->curDev->type == MMC_DEV_SDIO || err != HDF_SUCCESS) {
        return err;
    }

    /* COMBO card, need switch SD memory. */
    err = SdSwitchHighSpeed(cntlr);
    if (err == HDF_SUCCESS) {
        return err;
    }
    /* sd switch fail, need switch sdio to default speed. */
    if (SdioBusSpeedSelect(cntlr, false) != HDF_SUCCESS) {
        HDF_LOGD("SdioEnableHighSpeed: switch sdio to default speed fail.");
    }
    return err;
}

static int32_t SdioSetBusWidth(struct MmcCntlr *cntlr, uint8_t width)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t data;

    /* read register 7 of CCCR. */
    param.regAddr = BUS_INTERFACE_CONTROL;
    error = SdioRwDirect(cntlr, &param, &data);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioSetBusWidth: read BIC fail, error = %d!", error);
        return error;
    }

    data |= width;
    /* write bit1-bit0: Bus Width. */
    param.writeflag = true;
    param.writeData = data;
    return SdioRwDirect(cntlr, &param, NULL);
}

static int32_t SdioSwitch4BitBusWidth(struct MmcCntlr *cntlr)
{
    struct SdioDevice *sdioDev = NULL;

    if (cntlr == NULL || cntlr->curDev == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    sdioDev = (struct SdioDevice *)cntlr->curDev;
    if (cntlr->caps.bits.cap4Bit == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    /*
     * Note that Low-Speed SDIO cards support 4-bit transfer as an optipn. When communicating with a
     * Low-Speed SDIO card, the host shall fist determine if the card supports 4-bit transfer prior to
     * attempting to select that mode.
     */
    if (sdioDev->sdioReg.cccr.lowSpeed > 0 && sdioDev->sdioReg.cccr.lowSpeed4Bit == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return SdioSetBusWidth(cntlr, SDIO_CCCR_WIDTH_4BIT);
}

static int32_t SdioEnable4BitBusWidth(struct MmcCntlr *cntlr)
{
    int32_t error;
    struct SdDevice *dev = (struct SdDevice *)cntlr->curDev;

    /*
     * For an SD memory card, the bus width for SD mode is set using ACMD6.
     * For an SDIO card a write to the CCCR usingCMD52 is used to select bus width.
     * In the case of a combo card, both selection methods exist.
     */
    error = SdioSwitch4BitBusWidth(cntlr);
    if (cntlr->curDev->type == MMC_DEV_SDIO || error != HDF_SUCCESS) {
        return error;
    }

    /* COMBO card, need switch SD memory. */
    if ((dev->reg.scr.sdBusWidths & SD_SCR_BUS_WIDTHS_4) > 0 &&
        (cntlr->caps.bits.cap4Bit > 0)) {
        error = SdAcmdSetBusWidth(cntlr, BUS_WIDTH4);
    }

    return error;
}

static int32_t SdioReadCisTplField(struct MmcCntlr *cntlr, uint32_t addr, uint8_t *data)
{
    struct SdioCmdParam param = {0};

    param.regAddr = addr;
    return SdioRwDirect(cntlr, &param, data);
}

static void SdioDecodeCisTplManfId(struct MmcCntlr *cntlr, struct SdioFunction *function,
    struct SdioCisTuple *tuple)
{
    uint16_t vendorId, deviceId;
    struct SdioDevice *dev = NULL;

    if (tuple->tplLink < SDIO_CIS_TPL_MANFID_MIN_SIZE) {
        return;
    }

    /*
     * CISTPL_MANFID Field: bit00[TPL_CODE(20)], bit01[TPL_LINK(at least 4)], bit02-03[SDIO Card manufacturer code],
     * bit04-05[manufacturer information(Part Number and /or Revision)].
     * bit02-03-->tplBody[0:1], bit04-05-->tplBody[2:3].
     */
    vendorId = (tuple->tplBody[1] << BITS_PER_BYTE) | tuple->tplBody[0];
    deviceId = (tuple->tplBody[3] << BITS_PER_BYTE) | tuple->tplBody[2];

    /* function CISTPL_MANFID. */
    if (function != NULL) {
        function->deviceId = deviceId;
        function->vendorId = vendorId;
        return;
    }

    /* common CISTPL_MANFID. */
    dev = (struct SdioDevice *)cntlr->curDev;
    dev->sdioReg.cis.deviceId = deviceId;
    dev->sdioReg.cis.vendorId = vendorId;
}

static void SdioDecodeCisTplFunceCommon(struct MmcCntlr *cntlr, struct SdioCisTuple *tuple)
{
    struct SdioDevice *dev = (struct SdioDevice *)cntlr->curDev;
    const uint8_t value[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    const uint32_t unit[8] = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };

    if (tuple->tplLink < SDIO_CIS_TPL_FUNCE_COMMON_MIN_SIZE) {
        return;
    }

    /*
     * CISTPL_FUNCE(common): bit00[TPL_CODE(22h)], bit01[TPL_LINK], bit02[TPLFE_TYPE(00h)],
     * bit03-04[TPLFE_FN0_BLK_SIZE], bit05[TPLFE_MAX_TRAN_SPEED].
     * bit2-->tplBody[0], bit03-04-->tplBody[1:2], bit05-->tplBody[3].
     */
    dev->sdioReg.cis.blkSize = (tuple->tplBody[2] << BITS_PER_BYTE) | tuple->tplBody[1];

    /*
     * This byte indicates the maximum transfer rate per one data line during data transfer. This value applies to
     * all functions in the SDIO card. This value shall be 25Mb/Sec(32h) for all Full-Speed SDIO card. The minimum
     * value for Low-Speed SDIO cards shall be 400 Kb/Sec(48h). The format is identical to the TRAN_SPEED value stored
     * in the CSD of SD memory cards. The maximum data transfer rate is coded according to the following method:
     * Bits 2:0 contain the transfer rate unit coded as follows:
     * 0=100kbit/s, 1=1Mbit/s, 2=10Mbit/s, 3=100Mbit/s, ... 7=reserved.
     * Bits 6:3 contain the time value coded as follows:
     * 0=reserved, 1=1.0, 2=1.2, 3=1.3, 4=1.5, 5=2.0, 6=2.5, 7=3.0, 8= 3.5, 9=4.0, A=4.5, B=5.0, C=5.5, D=6.0, E=7.0,
     * F= 8.0.
     * Bit 7 is reserved and shall be 0.
     */
    dev->sdioReg.cis.maxDtr = unit[tuple->tplBody[3] & 7] * value[(tuple->tplBody[3] >> 3) & 15];
}

static void SdioDecodeCisTplFunceFunction(struct SdioFunction *function, struct SdioCisTuple *tuple)
{
    uint32_t minSize;

    /* Rev 1.0, TPL_LINK 28bytes; >Rev 1.0, TPL_LINK 42bytes.  */
    minSize = (function->dev->sdioReg.cccr.sdioRev == SDIO_CCCR_SDIO_REV_1_00) ?
        SDIO_CIS_TPL_FUNCE_FUNCTION_PC_MIN_SIZE : SDIO_CIS_TPL_FUNCE_FUNCTION_PC_MAX_SIZE;
    if (tuple->tplLink < minSize) {
        return;
    }

    /*
     * CISTPL_FUNCE(function): bit00[TPL_CODE(22h)], bit01[TPL_LINK], bit02[TPLFE_TYPE(01h)],
     * bit0E-0F[TPLFE_MAX_BLK_SIZE], bit1E-1F[TPLFE_ENABLE_TIMEOUT_VAL].
     * bit2-->tplBody[0], bit0E-0F-->tplBody[12:13], bit1E-1F-->tplBody[28:29].
     */
    function->maxBlkSize = (tuple->tplBody[13] << BITS_PER_BYTE) | tuple->tplBody[12];

    /*
     * TPLFE_ENABLE_TIMEOUT_VAL is added in SDIO Rev 1.1. This 16-bit value indicates the function's required time-out
     * value for coming ready after being enabled. This per-function value indicated the time a host should wait from
     * asserting IOEx until expecting the card to indicate ready by asserting IORx. Different SDIO functions take
     * different amounts of time to become raedy after being enabled due to different internal initialization
     * requirements. The required time-out limit is in 10mS steps. If the card required no time-out, this field shall
     * be set to 0000h.
     */
    if (function->dev->sdioReg.cccr.sdioRev > SDIO_CCCR_SDIO_REV_1_00) {
        function->timeOut = (tuple->tplBody[28] | (tuple->tplBody[29] << BITS_PER_BYTE)) * 10;
    } else {
        function->timeOut = SDIO_CIS_TPLFE_ENABLE_TIMEOUT_VAL_DEF;
    }
}

static void SdioDecodeCisTplFunce(struct MmcCntlr *cntlr, struct SdioFunction *function,
    struct SdioCisTuple *tuple)
{
    /*
     * The Function Extension Tuple provides standard information about the card(common) and each individual function.
     * There shall be one CISTPL_FUNCE in each function's CIS. There are two versions of the CISTPL_FUNCE tuple, one
     * for the common CIS(function 0) and a version used by the individual function's CIS(1-7).
     */
    if (tuple->tplBody[0] == SDIO_CIS_TPL_FUNCE_COMMON) {
        if (function != NULL) {
            return;
        }
        SdioDecodeCisTplFunceCommon(cntlr, tuple);
        return;
    }

    if (tuple->tplBody[0] == SDIO_CIS_TPL_FUNCE_FUNCTION_PC) {
        if (function == NULL) {
            return;
        }
        SdioDecodeCisTplFunceFunction(function, tuple);
    }
}

static void SdioDecodeCisTplField(struct MmcCntlr *cntlr, struct SdioFunction *function,
    struct SdioCisTuple *tuple)
{
    /* decode MANFID. */
    if (tuple->tplCode == SDIO_CIS_TPL_MANFID) {
        SdioDecodeCisTplManfId(cntlr, function, tuple);
        return;
    }
    /* decode FUNCE. */
    if (tuple->tplCode == SDIO_CIS_TPL_FUNCE) {
        SdioDecodeCisTplFunce(cntlr, function, tuple);
    }
}

static int32_t SdioFillTplInfo(struct MmcCntlr *cntlr, struct SdioCisTuple *tuple,
    uint32_t *addr, uint8_t tplCode, uint8_t tplLink)
{
    int32_t ret;
    uint32_t i;

    tuple->tplCode = tplCode;
    tuple->tplLink = tplLink;
    /* read tuple body. */
    for (i = 0; i < tplLink; i++) {
        (*addr)++;
        ret = SdioReadCisTplField(cntlr, *addr, &(tuple->tplBody[i]));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioFillTplInfo: read tuple body fail, err = %d.", ret);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SdioDecodeCis(struct MmcCntlr *cntlr, struct SdioFunction *function, uint32_t cisStartAddr)
{
    int32_t ret = HDF_SUCCESS;
    uint8_t tplCode, tplLink;
    struct SdioCisTuple *tuple = NULL;
    uint32_t addr = cisStartAddr;

    while (ret == HDF_SUCCESS) {
        /* read TPL_CODE. */
        ret = SdioReadCisTplField(cntlr, addr, &tplCode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioDecodeCis: read TPL_CODE fail, err = %d.", ret);
            return ret;
        }
        if (tplCode == SDIO_CIS_TPL_END || tplCode == SDIO_CIS_TPL_NULL) {
            return HDF_SUCCESS;
        }
        /* read TPL_LINK. */
        addr++;
        ret = SdioReadCisTplField(cntlr, addr, &tplLink);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioDecodeCis: read TPL_LINK fail, err = %d.", ret);
            return ret;
        }
        if (tplLink == SDIO_CIS_TPL_END) {
            return HDF_SUCCESS;
        }
        /* If the link field is 0, the the tuple body is empty. */
        if (tplLink == SDIO_CIS_TPL_NULL) {
            continue;
        }

        tuple = (struct SdioCisTuple *)OsalMemCalloc(sizeof(*tuple) + tplLink);
        if (tuple == NULL) {
            HDF_LOGE("SdioDecodeCis: mem alloc fail!");
            return HDF_ERR_MALLOC_FAIL;
        }

        ret = SdioFillTplInfo(cntlr, tuple, &addr, tplCode, tplLink);
        if (ret != HDF_SUCCESS) {
            OsalMemFree(tuple);
            return ret;
        }
        /* decode. */
        SdioDecodeCisTplField(cntlr, function, tuple);
        OsalMemFree(tuple);
        tuple = NULL;
        addr++;
    }
    return ret;
}

static int32_t SdioReadCis(struct MmcCntlr *cntlr, struct SdioFunction *function)
{
    uint32_t funcNum, i, cisStartAddr;
    uint8_t data;
    int32_t ret;
    struct SdioCmdParam param = {0};

    if (function == NULL) {
        funcNum = 0;
    } else {
        funcNum = function->funcNum;
    }

    /* read CIS pointer. */
    cisStartAddr = 0;
    for (i = 0; i < SDIO_CCCR_CIS_START_ADDR_BYTES; i++) {
        /* read register 0xn09-0xn0B of FBR. */
        param.regAddr = SDIO_FBR_BASE_ADDR(funcNum) + SDIO_FBR_POINTER_CIS + i;
        ret = SdioRwDirect(cntlr, &param, &data);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioReadCis: read CIS pointer fail, err = %d.", ret);
            return ret;
        }
        cisStartAddr |= (uint32_t)(data << (i * BITS_PER_BYTE));
    }
    return SdioDecodeCis(cntlr, function, cisStartAddr);
}

static int32_t SdioReadFbr(struct MmcCntlr *cntlr, struct SdioFunction *func)
{
    struct SdioCmdParam param = {0};
    int32_t error;
    uint8_t data;

    /* read register 0xn00 of FBR. */
    param.regAddr = SDIO_FBR_BASE_ADDR(func->funcNum) + SDIO_FBR_STD_FUNCTION_INTERFACE_CODE;
    error = SdioRwDirect(cntlr, &param, &data);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioReadFbr: read SFIC fail, err = %d.", error);
        return error;
    }

    /* bit3-bit0: Standard SDIO Function Interface Code. */
    data &= 0x0f;
    if (data == SDIO_FBR_STD_SDIO_IF) {
        /* read register 0xn01 of FBR. */
        param.regAddr = SDIO_FBR_BASE_ADDR(func->funcNum) + SDIO_FBR_EXT_STD_FUNCTION_INTERFACE_CODE;
        error = SdioRwDirect(cntlr, &param, &data);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("SdioReadFbr: read ESFIC fail, err = %d.", error);
            return error;
        }
    }

    func->funcClass = data;
    return HDF_SUCCESS;
}

int32_t SdioSetFbrIoBlockSize(struct MmcCntlr *cntlr, uint32_t blkSize)
{
    struct SdioCmdParam param = {0};
    struct SdioDevice *dev = NULL;
    int32_t ret;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    dev = (struct SdioDevice *)cntlr->curDev;
    if (dev == NULL || dev->curFunction == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    /* write register 0xn10 of FBR. */
    param.regAddr = SDIO_FBR_BASE_ADDR(dev->curFunction->funcNum) + SDIO_FBR_IO_BLOCK_SIZE;
    param.writeflag = true;
    /*
     * Function 1-7 I/O Block Size.
     * This 16-bit register sets the block size for I/O block operations for each function(1-7).
     * The maximum block size is 2048 and the minimum is 1. At power-up or reset, this register
     * shall be initially loaded with a value of 0. The host is responsible for setting the appropriate
     * value for the block size supported by each function. This pointer is stored in little-endian format.
     */
    param.writeData = (blkSize & 0xff);
    ret = SdioRwDirect(cntlr, &param, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioSetFbrIoBlockSize: write I/O Block Size fail, err = %d.", ret);
        return ret;
    }

    /* write register 0xn11 of FBR. */
    param.regAddr++;
    param.writeData = ((blkSize >> BITS_PER_BYTE) & 0xff);
    return SdioRwDirect(cntlr, &param, NULL);
}

static uint32_t SdioGetMaxClock(struct MmcCntlr *cntlr)
{
    uint32_t clock;
    struct MmcDevice *mmcDev = cntlr->curDev;
    struct SdioDevice *sdioDev = (struct SdioDevice *)mmcDev;

    if (mmcDev->state.bits.highSpeed == 1) {
        /* The card operates in High-Speed timing mode with a clock rate up to 50MHz. */
        clock = 50000000;
    } else {
        clock = sdioDev->sdioReg.cis.maxDtr;
    }

    if (mmcDev->type == MMC_DEV_COMBO) {
        clock = (clock < SdGetMaxClock(cntlr)) ? clock : SdGetMaxClock(cntlr);
    }

    if (clock > cntlr->freqMax) {
        clock = cntlr->freqMax;
    }
    return clock;
}

static struct SdioFunction *SdioAllocFunction(struct SdioDevice *sdioDev, uint32_t funcNum)
{
    struct SdioFunction *function = NULL;

    function = (struct SdioFunction *)OsalMemCalloc(sizeof(struct SdioFunction));
    if (function == NULL) {
        HDF_LOGE("SdioAllocFunction: alloc fail!");
        return NULL;
    }
    function->dev = sdioDev;
    function->funcNum = funcNum;
    return function;
}

static void SdioDeleteFunction(struct SdioFunction *function)
{
    if (function == NULL) {
        return;
    }
    OsalMemFree(function);
}

static int32_t SdioAddFunctions(struct MmcCntlr *cntlr, uint32_t funcs)
{
    struct SdioDevice *sdioDev = (struct SdioDevice *)cntlr->curDev;
    struct SdioFunction *function = NULL;
    uint32_t i;
    int32_t ret;

    sdioDev->functions = 0;
    for (i = 0; i < funcs; i++) {
        function = SdioAllocFunction(sdioDev, i + 1);
        if (function == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
        ret = SdioReadFbr(cntlr, function);
        if (ret != HDF_SUCCESS) {
            SdioDeleteFunction(function);
            return ret;
        }
        ret = SdioReadCis(cntlr, function);
        if (ret != HDF_SUCCESS) {
            SdioDeleteFunction(function);
            return ret;
        }

        if (function->vendorId == 0) {
            function->vendorId = sdioDev->sdioReg.cis.vendorId;
            function->deviceId = sdioDev->sdioReg.cis.deviceId;
        }
        /* A value of zero is not valid and shall not be used. */
        if (function->maxBlkSize == 0) {
            function->maxBlkSize = sdioDev->sdioReg.cis.blkSize;
        }

        sdioDev->sdioFunc[i] = function;
        (sdioDev->functions)++;
    }
    return HDF_SUCCESS;
}

static void SdioDelete(struct SdioDevice *sdioDev)
{
    uint32_t i;

    for (i = 0; i < sdioDev->functions; i++) {
        SdioDeleteFunction(sdioDev->sdioFunc[i]);
        sdioDev->sdioFunc[i] = NULL;
    }
}

static int32_t SdioDeviceAdd(struct MmcCntlr *cntlr)
{
    /* add dev. */
    if (MmcDeviceAdd(cntlr->curDev) != HDF_SUCCESS) {
        HDF_LOGE("SdioDeviceAdd: Add device fail!");
        return HDF_FAILURE;
    }
    cntlr->curDev->state.bits.present = 1;
    return HDF_SUCCESS;
}

static int32_t SdioSelect(struct MmcCntlr *cntlr, uint32_t *rocr)
{
    int32_t error;

    /* cmd5, set the support voltage. */
    error = SdioSendOpCond(cntlr, cntlr->curDev->reg.ocr.ocrData, rocr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd5(set voltage) fail, err = %d.", error);
        return error;
    }

    /* Judge Combo Card. */
    if (((*rocr) & SDIO_R4_MEMORY_PRESENT) > 0) {
        if (SdSelect(cntlr, rocr) == 0) {
            cntlr->curDev->type = MMC_DEV_COMBO;
            HDF_LOGD("combo dev!!");
        }
    }
    /* get RCA. */
    error = SdCmdSendRelativeAddr(cntlr, &(cntlr->curDev->reg.rca));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd3(get RCA) fail, err = %d.", error);
        return error;
    }
    if (cntlr->curDev->type == MMC_DEV_COMBO) {
        /* get CSD, CMD9 should send in stby. */
        error = SdReadCsd(cntlr);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("combo dev, read csd fail, err = %d.", error);
            return error;
        }
    }
    /* select card. */
    error = MmcSelectCard(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd7(select card) fail, err = %d.", error);
        return error;
    }

    error = SdioReadCccr(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("read cccr fail, err = %d.", error);
        return error;
    }
    /* read common CIS. */
    error = SdioReadCis(cntlr, NULL);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("SdioInit: read cis fail, err = %d.", error);
        return error;
    }
    if (cntlr->curDev->type == MMC_DEV_COMBO) {
        error = SdReadRegisters(cntlr);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("combo dev, read registers fail, err = %d.", error);
            MmcGoIdleState(cntlr);
            cntlr->curDev->type = MMC_DEV_SDIO;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SdioInit(struct MmcCntlr *cntlr)
{
    int32_t error;
    union MmcOcr ocr = {0};

    /* cmd5, detect sdio dev and get the voltage range. */
    error = SdioSendOpCond(cntlr, 0, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("cmd5(detect sdio) fail, err = %d", error);
        return error;
    }

    MmcCntlrSelectWorkVoltage(cntlr, &ocr);
    if (cntlr->curDev->reg.ocr.ocrData == 0) {
        HDF_LOGE("SdioInit: ocr is invalid!");
        return HDF_ERR_INVALID_PARAM;
    }

    error = SdioSelect(cntlr, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        return error;
    }

    error = SdioCdDisable(cntlr);
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Cd disable fail, err = %d.", error);
        return error;
    }
    error = SdioEnableHighSpeed(cntlr);
    if (error == HDF_SUCCESS) {
        cntlr->curDev->state.bits.highSpeed = 1;
        MmcCntlrSetBusTiming(cntlr, BUS_TIMING_SD_HS);
    } else if (error != HDF_ERR_NOT_SUPPORT) {
        HDF_LOGE("Enable HS fail, err = %d.", error);
        return error;
    }
    MmcCntlrSetClock(cntlr, SdioGetMaxClock(cntlr));

    error = SdioEnable4BitBusWidth(cntlr);
    if (error == HDF_SUCCESS) {
        MmcCntlrSetBusWidth(cntlr, BUS_WIDTH4);
    } else if (error != HDF_ERR_NOT_SUPPORT) {
        HDF_LOGE("Enable 4-bits bus width fail, err = %d.", error);
        return error;
    }
    /* R4, [30: 28] Number of I/O functions. */
    error = SdioAddFunctions(cntlr, ((ocr.ocrData >> 28) & SDIO_MAX_FUNCTION_NUMBER));
    if (error != HDF_SUCCESS) {
        HDF_LOGE("Add functions fail, err = %d.", error);
        return error;
    }

    return SdioDeviceAdd(cntlr);
}

int32_t SdioReinit(struct MmcCntlr *cntlr)
{
    union MmcOcr ocr = {0};
    int error;

    MmcGoIdleState(cntlr);
    MmcCntlrSetClock(cntlr, cntlr->freqMin);

    /* cmd5, detect sdio dev and get the voltage range. */
    error = SdioSendOpCond(cntlr, 0, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        return error;
    }
    /* cmd5, set the support voltage. */
    error = SdioSendOpCond(cntlr, cntlr->curDev->reg.ocr.ocrData, &(ocr.ocrData));
    if (error != HDF_SUCCESS) {
        return error;
    }
    /* get RCA. */
    error = SdCmdSendRelativeAddr(cntlr, &(cntlr->curDev->reg.rca));
    if (error != HDF_SUCCESS) {
        return error;
    }
    /* select card. */
    error = MmcSelectCard(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }
    error = SdioEnableHighSpeed(cntlr);
    if (error != HDF_SUCCESS) {
        return error;
    }
    MmcCntlrSetClock(cntlr, SdioGetMaxClock(cntlr));

    error = SdioEnable4BitBusWidth(cntlr);
    if (error == HDF_SUCCESS) {
        MmcCntlrSetBusWidth(cntlr, BUS_WIDTH4);
    }
    return error;
}

static int32_t SdioDetect(struct MmcCntlr *cntlr)
{
    int32_t err;

    HDF_LOGD("Detect sdio dev start...");
    /*
     * After reset or power-up, all I/O functions on the card are disabled and the I/O portion of the card shall
     * not execute any operation except CMD5 or CMD0. If there is SD memory on the card, that memory shall
     * respond normally to all mandatory memory commands.
     */
    SdioIoReset(cntlr);
    MmcGoIdleState(cntlr);
    /* Initialize SDIO. */
    err = SdioInit(cntlr);
    if (err == HDF_SUCCESS) {
        HDF_LOGD("Detect sdio dev success! %s dev at address 0x%x!",
            cntlr->curDev->state.bits.highSpeed ? "High speed" : "", cntlr->curDev->reg.rca);
        return HDF_SUCCESS;
    }
    SdioDelete((struct SdioDevice *)cntlr->curDev);
    return err;
}

void MmcDeleteDev(struct MmcCntlr *cntlr)
{
    if (cntlr == NULL || cntlr->curDev == NULL) {
        return;
    }

    if (cntlr->curDev->state.bits.present == 0) {
        return;
    }
    cntlr->curDev->state.bits.present = 0;

    if (cntlr->curDev->type == MMC_DEV_SDIO || cntlr->curDev->type == MMC_DEV_COMBO) {
        SdioDelete((struct SdioDevice *)cntlr->curDev);
    }

    MmcDeviceRemove(cntlr->curDev);
    MmcCntlrFreeDev(cntlr);
}

int32_t MmcDoDetect(struct MmcCntlr *cntlr)
{
    int32_t error;
    enum MmcDevType devType;

    if (cntlr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    devType = (enum MmcDevType)cntlr->devType;
    if (devType >= MMC_DEV_COMBO) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (MmcCntlrAllocDev(cntlr, devType) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }

    MmcCntlrPowerUp(cntlr);
    if (devType == MMC_DEV_SDIO) {
        error = SdioDetect(cntlr);
    } else if (devType == MMC_DEV_SD) {
        error = SdDetect(cntlr);
    } else {
        error = EmmcDetect(cntlr);
    }

    if (error == HDF_SUCCESS) {
        HDF_LOGD("MmcDoDetect: host%d detect succ!", cntlr->index);
        return error;
    }

    HDF_LOGD("MmcDoDetect: host%d detect fail!", cntlr->index);
    MmcCntlrFreeDev(cntlr);
    MmcCntlrPowerOff(cntlr);
    return error;
}
