/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdmi_core.h"

#define HDF_LOG_TAG hdmi_hdcp_c

static int32_t HdmiHdcpRead(struct HdmiHdcp *hdcp, enum HdmiHdcpPortOffset offset, uint8_t *buffer, uint32_t len)
{
    struct HdmiDdcCfg cfg = {0};
    struct HdmiCntlr *cntlr = NULL;

    if (hdcp == NULL || hdcp->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = (struct HdmiCntlr *)hdcp->priv;
    cfg.type = HDMI_DDC_DEV_HDCP;
    cfg.readFlag = true;
    cfg.devAddr = HDMI_DDC_HDCP_DEV_ADDRESS;
    cfg.data = buffer;
    cfg.dataLen = len;
    cfg.offset = offset;
    cfg.mode = HDMI_DDC_MODE_READ_MUTIL_NO_ACK;
    return HdmiDdcTransfer(&(cntlr->ddc), &cfg);
}

static int32_t HdmiHdcpWrite(struct HdmiHdcp *hdcp, enum HdmiHdcpPortOffset offset, uint8_t *buffer, uint32_t len)
{
    struct HdmiDdcCfg cfg = {0};
    struct HdmiCntlr *cntlr = NULL;

    if (hdcp == NULL || hdcp->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = (struct HdmiCntlr *)hdcp->priv;
    cfg.type = HDMI_DDC_DEV_HDCP;
    cfg.readFlag = false;
    cfg.devAddr = HDMI_DDC_HDCP_DEV_ADDRESS;
    cfg.data = buffer;
    cfg.dataLen = len;
    cfg.offset = offset;
    cfg.mode = HDMI_DDC_MODE_WRITE_MUTIL_ACK;
    return HdmiDdcTransfer(&(cntlr->ddc), &cfg);
}

static bool HdmiHdcpCheckKvs(uint8_t *ksv, uint32_t len)
{
    uint32_t i, j;
    uint32_t cnt = 0;

    for (i = 0; i < len; i++) {
        for (j = 0; j < HDMI_ONE_BYTE_SHIFT; j++) {
            if (((ksv[i] >> j) & 0x01) > 0) {
                cnt++;
            }
        }
    }
    /* KSV contains 20 ones and 20 zeros. */
    if (cnt != HDMI_HDCP_AKSV_ONE_NUM) {
        HDF_LOGE("HdmiHdcpCheckKvs: cnt = %d.", cnt);
        return false;
    }
    return true;
}

static int32_t HdmiHdcpWriteMsgToReg(struct HdmiHdcp *hdcp, enum HdmiHdcpRegOptType type, uint8_t *data, uint32_t len)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)hdcp->priv;

    if (cntlr->ops == NULL || cntlr->ops->hdcpOptReg == NULL) {
        HDF_LOGD("not support hdcp write reg.");
        return HDF_ERR_INVALID_PARAM;
    }
    return cntlr->ops->hdcpOptReg(cntlr, type, data, len);
}

static int32_t HdmiHdcpReadMsgBksv(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    ret = HdmiHdcpRead(hdcp, HDMI_HDCP_BKSV, hdcp->bksv, sizeof(hdcp->bksv));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (HdmiHdcpCheckKvs(hdcp->bksv, sizeof(hdcp->bksv)) == false) {
        HDF_LOGE("auth part I: bksv check fail.");
        return HDF_ERR_INVALID_PARAM;
    }
    return HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_BKSV, hdcp->bksv, sizeof(hdcp->bksv));
}

static int32_t HdmiHdcpReadMsgRi(struct HdmiHdcp *hdcp)
{
    int32_t ret;
    ret = HdmiHdcpRead(hdcp, HDMI_HDCP_RI, hdcp->ri, sizeof(hdcp->ri));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    return HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_RI, hdcp->ri, sizeof(hdcp->ri));
}

static int32_t HdmiHdcpWriteMsgAksv(struct HdmiHdcp *hdcp)
{
    if (HdmiHdcpCheckKvs(hdcp->aksv, sizeof(hdcp->aksv)) == false) {
        HDF_LOGE("auth part I: aksv check fail.");
        return HDF_ERR_INVALID_PARAM;
    }
    return HdmiHdcpWrite(hdcp, HDMI_HDCP_AN, hdcp->aksv, sizeof(hdcp->aksv));
}

static int32_t HdmiHdcpWriteMsgAn(struct HdmiHdcp *hdcp)
{
    return HdmiHdcpWrite(hdcp, HDMI_HDCP_AN, hdcp->an, sizeof(hdcp->an));
}

static int32_t HdmiHdcpReadMsgHv(struct HdmiHdcp *hdcp)
{
    uint8_t offset, i;
    uint8_t vhNum = HDMI_HDCP_MSG_ALL_VH_LEN / HDMI_HDCP_MSG_PER_VH_LEN;
    int32_t ret;

    if (hdcp == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < vhNum; i++) {
        offset = HDMI_HDCP_V_H0 + i * HDMI_HDCP_MSG_PER_VH_LEN;
        ret = HdmiHdcpRead(hdcp, offset, &(hdcp->vh[i]), HDMI_HDCP_MSG_PER_VH_LEN);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    return HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_V_H_ALL, hdcp->vh, sizeof(hdcp->vh));
}

static int32_t HdmiHdcpReadMsgBcaps(struct HdmiHdcp *hdcp)
{
    int32_t ret;
    ret = HdmiHdcpRead(hdcp, HDMI_HDCP_BCAPS, &(hdcp->bcaps), sizeof(hdcp->bcaps));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    return HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_BCAPS, &(hdcp->bcaps), sizeof(hdcp->bcaps));
}

static int32_t HdmiHdcpAuthenticationGenerateAksvAn(struct HdmiHdcp *hdcp)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)hdcp->priv;

    if (cntlr->ops == NULL || cntlr->ops->hdcpGenerateAksvAndAn == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    return cntlr->ops->hdcpGenerateAksvAndAn(cntlr);
}

static void HdmiHdcpRegPrepare(struct HdmiHdcp *hdcp)
{
    struct HdmiCntlr *cntlr = (struct HdmiCntlr *)hdcp->priv;

    if (cntlr->ops == NULL || cntlr->ops->hdcpRegInit == NULL) {
        return;
    }
    cntlr->ops->hdcpRegInit(cntlr);
}

static int32_t HdmiHdcpAuthenticationPrepare(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    HdmiHdcpRegPrepare(hdcp);
    if (hdcp->aksvValid == false) {
        ret = HdmiHdcpAuthenticationGenerateAksvAn(hdcp);
        if (ret != HDF_SUCCESS) {
            hdcp->state = HDMI_HDCP_AUTH_AKSV_INVALID;
            return ret;
        }
    }
    hdcp->aksvValid = true;
    return HDF_SUCCESS;
}

static int32_t HdmiHdcpAuthenticationFirstPart(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    hdcp->state = HDMI_HDCP_AUTH_DOING;
    /*
    * Authentication is initiated by the HDCP Transmitter by sendind an initiation massage containing its KSV(aksv)
    * and a 64-bit pseudo-random value(An) generated by the HDCP Cipher function hdcpRngCipher to the HDCP Reciever.
    */
    ret = HdmiHdcpWriteMsgAksv(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = HdmiHdcpWriteMsgAn(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    /*
    * The HDCP Reciever responds by sending a response message containing the receiver's KSV(bksv) and
    * the REPEATER bit, which indicates if the receiver is an HDCP Repeater.
    */
    ret = HdmiHdcpReadMsgBksv(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = HdmiHdcpReadMsgBcaps(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    /*
     * The HDCP Transmitter must not read the R0' value sooner than 100ms after writing Aksv.
     */
    OsalMSleep(120);
    /* Read R0' */
    ret = HdmiHdcpReadMsgRi(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    /* Verify R0/R0' matched or not */
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_CHECK_R, NULL, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth, check V fail.");
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiHdcpAuthenticationCheckBstatus(struct HdmiHdcp *hdcp)
{
    int32_t ret;
    uint16_t bstatus;

    ret = HdmiHdcpRead(hdcp, HDMI_HDCP_BSTATUS, hdcp->bstatus, sizeof(hdcp->bstatus));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    /* phase bstatus. */
    bstatus = (hdcp->bstatus[1] << 8) | hdcp->bstatus[0];
    hdcp->deviceCount = bstatus & HDMI_HDCP_BSTATUS_DEVICE_COUNT_MARK;
    if (hdcp->deviceCount == 0) {
        HDF_LOGE("hdcp auth, check bstatus, no device attached to the repeater.");
        return HDF_DEV_ERR_NO_DEVICE;
    }
    if ((bstatus & HDMI_HDCP_BSTATUS_MAX_DEVS_EXCEEDED_MARK) > 0) {
        HDF_LOGE("hdcp auth, check bstatus, exceeds max allowed connected devices.");
        return HDF_ERR_IO;
    }
    if ((bstatus & HDMI_HDCP_BSTATUS_MAX_CASCADE_EXCEEDED_MARK) > 0) {
        HDF_LOGE("hdcp auth, check bstatus, exceeds max allowed cascade.");
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t HdmiHdcpAuthenticationWaitKsvListReady(struct HdmiHdcp *hdcp)
{
    uint32_t i;
    int32_t ret;
    bool ready = false;

    /* Set up 5s timer, poll for KSV list ready. Fail if timer expires prior to ready. */
    for (i = 0; i < HDMI_HDCP_WAIT_KSV_LIST_READ_REG_CNT; i++) {
        ret = HdmiHdcpRead(hdcp, HDMI_HDCP_BCAPS, &(hdcp->bcaps), sizeof(hdcp->bcaps));
        if (ret != HDF_SUCCESS) {
            return ret;
        }
        if ((hdcp->bcaps & HDMI_HDCP_BCAPS_KSV_FIFO_READY_MARK) > 0) {
            ready = true;
            break;
        }
        OsalMSleep(HDMI_HDCP_WAIT_KSV_LIST_READ_REG_INTERVAL);
    }
    if (ready == false) {
        return HDF_ERR_TIMEOUT;
    }

    ret = HdmiHdcpAuthenticationCheckBstatus(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_BCAPS, &(hdcp->bcaps), sizeof(hdcp->bcaps));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("hdcp auth wait ksv list ready, WRITE_BCAPS fail");
    }
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_BSTATUS, hdcp->bstatus, sizeof(hdcp->bstatus));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("hdcp auth wait ksv list ready, WRITE_BSTATUS fail");
    }
    return HDF_SUCCESS;
}

static int32_t HdmiHdcpAuthenticationReadKsvList(struct HdmiHdcp *hdcp)
{
    int32_t ret;
    uint32_t try = HDMI_HDCP_DEFAULT_READ_REG_TRY;

    hdcp->ksvLen = HDMI_HDCP_GET_KSV_LEN(hdcp->deviceCount);
    do {
        ret = HdmiHdcpRead(hdcp, HDMI_HDCP_KSV_FIFO, hdcp->ksvList, hdcp->ksvLen);
        if (ret == HDF_SUCCESS) {
            break;
        }
        OsalMSleep(HDMI_HDCP_WAIT_KSV_LIST_READ_REG_INTERVAL);
        try--;
    } while (try > 0);

    ret = HdmiHdcpReadMsgHv(hdcp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    /* We may need to reset SHA before write ksv list. */
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_RESET_SHA, NULL, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("hdcp auth read ksv list, RESET_SHA fail");
    }
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_WRITE_KSV_FIFO, hdcp->ksvList, hdcp->ksvLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("hdcp auth read ksv list, WRITE_KSV_FIFO fail");
    }
    return HDF_SUCCESS;
}

static int32_t HdmiHdcpAuthenticationSecondPart(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    /* Poll: KSV list ready. */
    ret = HdmiHdcpAuthenticationWaitKsvListReady(hdcp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth, wait ksv list ready fail.");
        return ret;
    }

    /* Read KSV list */
    ret = HdmiHdcpAuthenticationReadKsvList(hdcp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth, read ksv list fail.");
        return ret;
    }

    /* Verify V/V' matched or not */
    ret = HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_CHECK_V, NULL, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth, check V fail.");
        return ret;
    }
    return HDF_SUCCESS;
}

static void HdmiHdcpAuthenticationSucc(struct HdmiHdcp *hdcp)
{
    if (HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_AUTH_DONE, NULL, 0) != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth succ, AUTH_DONE fail.");
    }
    hdcp->state = HDMI_HDCP_AUTH_DONE;
    hdcp->authRetryCnt = 0;
}

static void HdmiHdcpAuthenticationFail(struct HdmiHdcp *hdcp)
{
    if (HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_AUTH_FAIL, NULL, 0) != HDF_SUCCESS) {
        HDF_LOGE("hdcp auth fail, AUTH_FAIL fail.");
    }
    hdcp->state = HDMI_HDCP_AUTH_FAIL;
}

static int32_t HdmiHdcpAuthentication(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    if (hdcp == NULL || hdcp->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    /* auth prepare, get aksv and driver clear registers. */
    ret = HdmiHdcpAuthenticationPrepare(hdcp);
    if (ret != HDF_SUCCESS) {
        HdmiHdcpAuthenticationFail(hdcp);
        return ret;
    }

    /*
     * First part of the authentication exchange.
     */
    ret = HdmiHdcpAuthenticationFirstPart(hdcp);
    if (ret != HDF_SUCCESS) {
        HdmiHdcpAuthenticationFail(hdcp);
        return ret;
    }
    HDF_LOGD("hdcp auth Part I success.");
    /* receiver (0), repeater (1) */
    if ((hdcp->bcaps & HDMI_HDCP_BCAPS_REPEATER_MARK) == 0) {
        HdmiHdcpAuthenticationSucc(hdcp);
        return ret;
    }

    /*
     * The second part of the authentication protocol is required if the HDCP Reciever is an HDCP Repeater.
     */
    ret = HdmiHdcpAuthenticationSecondPart(hdcp);
    if (ret != HDF_SUCCESS) {
        HdmiHdcpAuthenticationFail(hdcp);
        return ret;
    }

    HdmiHdcpAuthenticationSucc(hdcp);
    return ret;
}

static int32_t HdmiHdcpAuthenticationRetry(struct HdmiHdcp *hdcp)
{
    if (hdcp == NULL || hdcp->priv == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (hdcp->authRetryCnt > HDMI_HDCP_AUTH_MAX_RETRY_CNT) {
        HDF_LOGE("hdcp auth reach to max cnt, auth fail");
        return HDF_FAILURE;
    }
    return HdmiHdcpAuthentication(hdcp);
}

int32_t HdmiHdcpOpen(struct HdmiHdcp *hdcp)
{
    int32_t ret;

    (void)OsalMutexLock(&(hdcp->hdcpMutex));
    hdcp->state = HDMI_HDCP_AUTH_INACTIVE;
    hdcp->authRetryCnt = 0;
    ret = HdmiHdcpAuthentication(hdcp);
    if (ret == HDF_SUCCESS) {
        HDF_LOGD("hdcp Authentication success!");
        (void)OsalMutexUnlock(&(hdcp->hdcpMutex));
        return ret;
    }

    while (hdcp->authRetryCnt <= HDMI_HDCP_AUTH_MAX_RETRY_CNT) {
        ret = HdmiHdcpAuthenticationRetry(hdcp);
        if (ret == HDF_SUCCESS) {
            (void)OsalMutexUnlock(&(hdcp->hdcpMutex));
            return ret;
        }
        hdcp->authRetryCnt++;
    }
    (void)OsalMutexUnlock(&(hdcp->hdcpMutex));
    return ret;
}

void HdmiHdcpClose(struct HdmiHdcp *hdcp)
{
    if (hdcp == NULL) {
        return;
    }

    if (hdcp->state == HDMI_HDCP_AUTH_INACTIVE ||
        hdcp->state == HDMI_HDCP_AUTH_AKSV_INVALID) {
        return;
    }

    if (HdmiHdcpWriteMsgToReg(hdcp, HDMI_HDCP_OPT_AUTH_CLOSE, NULL, 0) != HDF_SUCCESS) {
        HDF_LOGD("hdcp close, AUTH_CLOSE fail");
    }
    hdcp->state = HDMI_HDCP_AUTH_INACTIVE;
    hdcp->authRetryCnt = 0;
}
