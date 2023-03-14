/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_CORE_H
#define MMC_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "mmc_caps.h"
#include "mmc_protocol.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_time.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MMC_CNTLR_NR_MAX 3
#define MMC_DEV_NR_MAX   3
#define MMC_SEC_SIZE     512

#define MMC_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MMC_MAX(x, y) (((x) < (y)) ? (y) : (x))

struct MmcCntlr;
struct MmcCntlrOps;
struct MmcDevice;
struct MmcMsg;
struct MmcData;
struct MmcCmd;
enum MmcMsgCode;
enum MmcCmdCode;
union MmcDevState;

enum MmcDevType {
    MMC_DEV_EMMC = 0,
    MMC_DEV_SD,
    MMC_DEV_SDIO,
    MMC_DEV_COMBO,
    MMC_DEV_INVALID
};

struct MmcCntlr {
    struct IDeviceIoService service;
    struct HdfDeviceObject *hdfDevObj;
    struct PlatformDevice device;
    struct OsalMutex mutex;
    struct OsalSem released;
    uint32_t devType;
    struct MmcDevice *curDev;
    struct MmcCntlrOps *ops;
    struct PlatformQueue *msgQueue;
    uint16_t index;
    uint16_t voltDef;
    uint32_t vddBit;
    uint32_t freqMin;
    uint32_t freqMax;
    uint32_t freqDef;
    union MmcOcr ocrDef;
    union MmcCaps caps;
    union MmcCaps2 caps2;
    uint32_t maxBlkNum;
    uint32_t maxBlkSize;
    uint32_t maxReqSize;
    bool devPluged;
    bool detecting;
    void *priv;
};

struct MmcCntlrOps {
    int32_t (*request)(struct MmcCntlr *cntlr, struct MmcCmd *cmd);
    int32_t (*setClock)(struct MmcCntlr *cntlr, uint32_t clock);
    int32_t (*setPowerMode)(struct MmcCntlr *cntlr, enum MmcPowerMode mode);
    int32_t (*setBusWidth)(struct MmcCntlr *cntlr, enum MmcBusWidth width);
    int32_t (*setBusTiming)(struct MmcCntlr *cntlr, enum MmcBusTiming timing);
    int32_t (*setSdioIrq)(struct MmcCntlr *cntlr, bool enable);
    int32_t (*hardwareReset)(struct MmcCntlr *cntlr);
    int32_t (*systemInit)(struct MmcCntlr *cntlr);
    int32_t (*setEnhanceSrobe)(struct MmcCntlr *cntlr, bool enable);
    int32_t (*switchVoltage)(struct MmcCntlr *cntlr, enum MmcVolt volt);
    bool (*devReadOnly)(struct MmcCntlr *cntlr);
    bool (*devPluged)(struct MmcCntlr *cntlr);
    bool (*devBusy)(struct MmcCntlr *cntlr);
    int32_t  (*tune)(struct MmcCntlr *cntlr, uint32_t cmdCode);
    int32_t (*rescanSdioDev)(struct MmcCntlr *cntlr);
};

/* controller management */
int32_t MmcCntlrAdd(struct MmcCntlr *cntlr, bool needQueue);
void MmcCntlrRemove(struct MmcCntlr *cntlr);
struct MmcDevice *MmcCntlrGetDevice(struct MmcCntlr *cntlr);

static inline struct MmcCntlr *MmcCntlrGet(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL && PlatformDeviceGet(&cntlr->device) == HDF_SUCCESS) {
        return cntlr;
    }
    return NULL;
}

static inline struct MmcCntlr *MmcCntlrGetByNr(uint16_t number)
{
    struct PlatformDevice *device = PlatformManagerGetDeviceByNumber(PlatformManagerGet(PLATFORM_MODULE_MMC), number);

    if (device == NULL) {
        return NULL;
    }
    return CONTAINER_OF(device, struct MmcCntlr, device);
}

static inline void MmcCntlrPut(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        PlatformDevicePut(&cntlr->device);
    }
}

static inline bool MmcCntlrDevPresent(struct MmcCntlr *cntlr)
{
    return (cntlr != NULL && cntlr->curDev != NULL);
}

static inline void MmcCntlrLock(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexLock(&cntlr->mutex);
    }
}

static inline void MmcCntlrUnlock(struct MmcCntlr *cntlr)
{
    if (cntlr != NULL) {
        (void)OsalMutexUnlock(&cntlr->mutex);
    }
}

/* controller common bussiness */
int32_t MmcCntlrDoRequest(struct MmcCntlr *cntlr, struct MmcCmd *cmd);
int32_t MmcCntlrAddMsgToQueue(struct MmcCntlr *cntlr, struct MmcCmd *cmd, int32_t code, bool block);
int32_t MmcCntlrAddRequestMsgToQueue(struct MmcCntlr *cntlr, struct MmcCmd *cmd);
int32_t MmcCntlrAddDetectMsgToQueue(struct MmcCntlr *cntlr);
int32_t MmcCntlrAddPlugMsgToQueue(struct MmcCntlr *cntlr);
int32_t MmcCntlrAddSdioRescanMsgToQueue(struct MmcCntlr *cntlr);
int32_t MmcCntlrParse(struct MmcCntlr *cntlr, struct HdfDeviceObject *obj);
int32_t MmcCntlrCreatSdioIrqThread(struct MmcCntlr *cntlr);
void MmcCntlrDestroySdioIrqThread(struct MmcCntlr *cntlr);
void MmcCntlrNotifySdioIrqThread(struct MmcCntlr *cntlr);

/* controller private bussiness */
void MmcCntlrSetClock(struct MmcCntlr *cntlr, uint32_t clock);
void MmcCntlrSetBusWidth(struct MmcCntlr *cntlr, enum MmcBusWidth width);
void MmcCntlrSetBusTiming(struct MmcCntlr *cntlr, enum MmcBusTiming timing);
void MmcCntlrSetEnhanceSrobe(struct MmcCntlr *cntlr, bool enable);
int32_t MmcCntlrSwitchVoltage(struct MmcCntlr *cntlr, enum MmcVolt voltage);
bool MmcCntlrDevReadOnly(struct MmcCntlr *cntlr);
bool MmcCntlrDevBusy(struct MmcCntlr *cntlr);
bool MmcCntlrDevPluged(struct MmcCntlr *cntlr);
int32_t MmcCntlrTune(struct MmcCntlr *cntlr, uint32_t cmdCode);
void MmcCntlrSelectWorkVoltage(struct MmcCntlr *cntlr, union MmcOcr *ocr);
void MmcCntlrPowerUp(struct MmcCntlr *cntlr);
void MmcCntlrPowerOff(struct MmcCntlr *cntlr);
int32_t MmcCntlrAllocDev(struct MmcCntlr *cntlr, enum MmcDevType devType);
void MmcCntlrFreeDev(struct MmcCntlr *cntlr);
bool MmcCntlrSupportUhs(struct MmcCntlr *cntlr);
bool MmcCntlrSupportHighSpeed400EnhancedStrobe(struct MmcCntlr *cntlr);
bool MmcCntlrSupportHighSpeed400(struct MmcCntlr *cntlr);
bool MmcCntlrSupportHighSpeed200(struct MmcCntlr *cntlr);
bool MmcCntlrSdSupportCmd23(struct MmcCntlr *cntlr);
bool MmcCntlrEmmcSupportCmd23(struct MmcCntlr *cntlr);

struct MmcDeviceWorkParam {
    uint32_t clock;
    enum MmcBusWidth width;
    enum MmcBusTiming timing;
};

struct MmcRegister {
    union MmcOcr ocr;
    uint32_t rca;
    uint32_t rawCid[CID_LEN];
    uint32_t rawCsd[CSD_LEN];
    struct MmcCid cid;
    struct MmcCsd csd;
};

union MmcDevState {
    uint32_t stateData;
    struct StateBitsData {
        uint32_t present : 1;
        uint32_t readonly : 1;
        uint32_t highSpeed : 1;
        uint32_t blockAddr : 1;
        uint32_t ddrMode : 1;
        uint32_t highSpeedDdr : 1;
        uint32_t uhs : 1;
        uint32_t sdxc : 1;
        uint32_t hs200 : 1;
        uint32_t sleep : 1;
        uint32_t removable : 1;
        uint32_t blkszForByteMode : 1;
        uint32_t hs400 : 1;
        uint32_t hs400es : 1;
        uint32_t reserverd : 17;
    } bits;
};

struct MmcDevice {
    struct PlatformDevice device;
    struct MmcCntlr *cntlr;
    struct MmcRegister reg;
    struct MmcDeviceWorkParam workPara;
    union MmcDevState state;
    enum MmcDevType type;
    size_t secSize;   // by bytes
    size_t capacity;  // by sectors
    size_t eraseSize; // by sectors
    struct MmcBlock *mb;
    void *priv;
};

/* device management */
int32_t MmcDeviceAdd(struct MmcDevice *mmc);
void MmcDeviceAddOps(struct MmcDevice *mmc, void *ops);
void MmcDeviceRemove(struct MmcDevice *mmc);
struct MmcDevice *MmcDeviceGet(struct MmcDevice *mmc);
void MmcDevicePut(struct MmcDevice *mmc);

/* device business */
ssize_t MmcDeviceRead(struct MmcDevice *mmc, uint8_t *buf, size_t startSec, size_t nSec);
ssize_t MmcDeviceWrite(struct MmcDevice *mmc, uint8_t *buf, size_t startSec, size_t nSec);
ssize_t MmcDeviceErase(struct MmcDevice *mmc, size_t startSec, size_t nSec);

static inline bool MmcDeviceIsPresent(struct MmcDevice *mmc)
{
    return (mmc != NULL && mmc->state.bits.present);
}

struct MmcCmd {
    uint32_t cmdCode;
    uint32_t argument;
    uint32_t resp[MMC_CMD_RESP_SIZE];
    uint32_t respType;
    int32_t returnError;
    struct MmcData *data;
};

#define DATA_WRITE (0x1 << 0)
#define DATA_READ (0x1 << 1)
#define DATA_STREAM (0x1 << 2)
struct MmcData {
    uint32_t blockSize; /* data block size, byte */
    uint32_t blockNum;
    int32_t returnError;
    uint8_t *dataBuffer;
    void *scatter;
    uint32_t scatterLen;
    uint32_t dataFlags;
    struct MmcCmd stopCmd; /* stop command */
    bool sendStopCmd;
};

enum MmcMsgCode {
    MMC_MSG_REQUEST,
    MMC_MSG_PLUG,
    MMC_MSG_UNPLUG,
    MMC_MSG_SDIO_RESCAN,
};

struct MmcMsg {
    struct PlatformMsg msg;
    struct MmcCmd *mmcCmd;
    struct OsalSem sem;
    bool block;
};

struct MmcRwData {
    uint8_t *buf;
    bool writeFlag;
    size_t startSector;
    size_t sectors;
};

int32_t MmcDoDetect(struct MmcCntlr *cntlr);
void MmcDeleteDev(struct MmcCntlr *cntlr);
int32_t MmcSendStatus(struct MmcCntlr *cntlr, uint32_t *status);
int32_t MmcStopTransmission(struct MmcCntlr *cntlr, bool writeFlag, uint32_t *stopStatus);
int32_t MmcSendTuning(struct MmcCntlr *cntlr, uint32_t cmdCode, bool sendStop);
int32_t MmcSendErase(struct MmcCntlr *cntlr, uint32_t startSec, uint32_t nSec);
void MmcSetupReadWriteBlocksCmd(struct MmcDevice *mmc, struct MmcCmd *cmd, struct MmcRwData *info);
int32_t MmcSendReadWriteBlocks(struct MmcCntlr *cntlr, struct MmcRwData *info);
int32_t SdioReinit(struct MmcCntlr *cntlr);
int32_t SdioReadCccrIoEnable(struct MmcCntlr *cntlr, uint8_t *val);
int32_t SdioCccrIntEnable(struct MmcCntlr *cntlr);
int32_t SdioCccrIntDisable(struct MmcCntlr *cntlr);
int32_t SdioCccrIoEnable(struct MmcCntlr *cntlr);
int32_t SdioCccrIoDisable(struct MmcCntlr *cntlr);
int32_t SdioReadCccrIntPending(struct MmcCntlr *cntlr, uint8_t *val);
int32_t SdioReadCccrIoReady(struct MmcCntlr *cntlr, uint8_t *val);
int32_t SdioSetFbrIoBlockSize(struct MmcCntlr *cntlr, uint32_t blkSize);
int32_t SdioReadWriteByte(struct MmcCntlr *cntlr, bool writeFlag,
    uint32_t funcNum, uint32_t addr, uint8_t *data);
int32_t SdioReadWriteBlock(struct MmcCntlr *cntlr, struct SdioRwBlockInfo *info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_CORE_H */
