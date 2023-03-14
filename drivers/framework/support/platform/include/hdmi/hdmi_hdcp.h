/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_HDCP_H
#define HDMI_HDCP_H

#include "hdf_base.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* HDCP(High-bandwidth Digital Content Protection) Sysytem. */

enum HdmiHdcpPortOffset {
    /* HDCP 1.4 Spec */
    HDMI_HDCP_BKSV = 0x00,       /* Rd, 5 bytes */
    HDMI_HDCP_RI = 0x08,         /* Rd, 2 bytes */
    HDMI_HDCP_PJ = 0x0A,         /* Rd, 1 byte */
    HDMI_HDCP_AKSV = 0x10,       /* Wr, 5 bytes */
    HDMI_HDCP_AINFO = 0x15,      /* Wr, 1 byte */
    HDMI_HDCP_AN = 0x18,         /* Wr, 8 bytes */
    HDMI_HDCP_V_H0 = 0x20,       /* Rd, 4 bytes */
    HDMI_HDCP_V_H1 = 0x24,       /* Rd, 4 bytes */
    HDMI_HDCP_V_H2 = 0x28,       /* Rd, 4 bytes */
    HDMI_HDCP_V_H3 = 0x2C,       /* Rd, 4 bytes */
    HDMI_HDCP_V_H4 = 0x30,       /* Rd, 4 bytes */
    HDMI_HDCP_BCAPS = 0x40,      /* Rd, 1 byte */
    HDMI_HDCP_BSTATUS = 0x41,    /* Rd, 2 bytes */
    HDMI_HDCP_KSV_FIFO = 0x43,   /* Rd, 1 byte */

    /* HDCP 2.2 Spec */
    HDMI_HDCP_HDCP2VERSION = 0x50, /* Rd, 1 byte */
    HDMI_HDCP_WRITE_MSG = 0x60,    /* Wr, 1 byte */
    HDMI_HDCP_RXSTATUS = 0x70,     /* Rd, 2 bytes */
    HDMI_HDCP_READ_MSG = 0x80,     /* Rd, 1 byte */
};

/* HDCP 2.2 Read/Write Message Id */
enum HdmiHdcpMsgId {
    HDMI_HDCP_MSG_AKE_INIT = 2,                       /* Write, 12 bytes */
    HDMI_HDCP_MSG_AKE_SEND_CERT = 3,                  /* Read, 534 bytes */
    HDMI_HDCP_MSG_AKE_NO_STORED_KM = 4,               /* Write, 129 bytes */
    HDMI_HDCP_MSG_AKE_STORED_KM = 5,                  /* Write, 33 bytes */
    HDMI_HDCP_MSG_AKE_SEND_H_PRIME = 7,               /* Read, 33 bytes */
    HDMI_HDCP_MSG_AKE_SEND_PAIRING_INFO = 8,          /* Read, 17 bytes */
    HDMI_HDCP_MSG_LC_INIT = 9,                        /* Write, 9 bytes */
    HDMI_HDCP_MSG_LC_SEND_L_PRIME = 10,               /* Read, 33 bytes */
    HDMI_HDCP_MSG_SKE_SEND_EKS = 11,                  /* Write, 25 bytes */
    HDMI_HDCP_MSG_REPEATER_AUTH_SEND_RCVID_LIST = 12, /* Read, 22 + 5 * device_count bytes */
    HDMI_HDCP_MSG_REPEATER_AUTH_SEND_ACK = 15,        /* Write, 17 bytes */
    HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_MANAGE = 16,   /* Write, 6 + 2 * k bytes */
    HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_READY = 17,    /* Read, 33 bytes */
};

enum HdmiHdcpRegOptType {
    /* HDCP 1.4 Spec */
    HDMI_HDCP_OPT_WRITE_BKSV = 0x00,
    HDMI_HDCP_OPT_WRITE_RI = 0x01,
    HDMI_HDCP_OPT_WRITE_PJ = 0x02,
    HDMI_HDCP_OPT_WRITE_V_H0 = 0x03,
    HDMI_HDCP_OPT_WRITE_V_H1 = 0x04,
    HDMI_HDCP_OPT_WRITE_V_H2 = 0x05,
    HDMI_HDCP_OPT_WRITE_V_H3 = 0x06,
    HDMI_HDCP_OPT_WRITE_V_H4 = 0x07,
    HDMI_HDCP_OPT_WRITE_V_H_ALL = 0x08,
    HDMI_HDCP_OPT_WRITE_BCAPS = 0x0C,
    HDMI_HDCP_OPT_WRITE_BSTATUS = 0x0D,
    HDMI_HDCP_OPT_WRITE_KSV_FIFO = 0x0E,
    HDMI_HDCP_OPT_RESET_SHA = 0x0F,
    HDMI_HDCP_OPT_CHECK_V = 0x10,
    HDMI_HDCP_OPT_CHECK_R = 0x11,
    HDMI_HDCP_OPT_AUTH_FAIL = 0x12,
    HDMI_HDCP_OPT_AUTH_DONE = 0x13,
    HDMI_HDCP_OPT_AUTH_CLOSE = 0x14,
};

#define HDMI_HDCP_MSG_BKSV_LEN 5
#define HDMI_HDCP_MSG_RI_LEN 2
#define HDMI_HDCP_MSG_PJ_LEN 1
#define HDMI_HDCP_MSG_AKSV_LEN 5
#define HDMI_HDCP_MSG_AINFO_LEN 1
#define HDMI_HDCP_MSG_AN_LEN 8
#define HDMI_HDCP_MSG_PER_VH_LEN 4
#define HDMI_HDCP_MSG_ALL_VH_LEN 20
#define HDMI_HDCP_MSG_BCAPS_LEN 1
#define HDMI_HDCP_MSG_BSTATUS_LEN 2
#define HDMI_HDCP_MSG_KSV_FIFO_LEN 1
#define HDMI_HDCP_DEVICE_COUNT_MAX 127
#define HDMI_HDCP_MSG_KSV_MAX_LEN (5 * HDMI_HDCP_DEVICE_COUNT_MAX)
#define HDMI_HDCP_GET_KSV_LEN(x) (5 * (x))

#define HDMI_HDCP_AKSV_ONE_NUM 20
#define HDMI_HDCP_BCAPS_REPEATER_MARK 0x40
#define HDMI_HDCP_BCAPS_KSV_FIFO_READY_MARK 0x20
#define HDMI_HDCP_DEFAULT_READ_REG_TRY 100
#define HDMI_HDCP_WAIT_KSV_LIST_TIMES 5000 /* 5s */
#define HDMI_HDCP_WAIT_KSV_LIST_READ_REG_INTERVAL 20 /* 20ms */
#define HDMI_HDCP_WAIT_KSV_LIST_READ_REG_CNT \
    (HDMI_HDCP_WAIT_KSV_LIST_TIMES / HDMI_HDCP_WAIT_KSV_LIST_READ_REG_INTERVAL)

#define HDMI_HDCP_BSTATUS_DEVICE_COUNT_MARK 0x7F
#define HDMI_HDCP_BSTATUS_MAX_DEVS_EXCEEDED_MARK (1 << 7)
#define HDMI_HDCP_BSTATUS_MAX_CASCADE_EXCEEDED_MARK (1 << 11)
#define HDMI_HDCP_BSTATUS_DEPTH_MARK 0x07
#define HDMI_HDCP_BSTATUS_DEPTH_SHIFT 8


#define HDMI_HDCP_MSG_HDCP2VERSION_LEN 1
#define HDMI_HDCP_MSG_RXSTATUS_LEN 2
#define HDMI_HDCP_MSG_AKE_INIT_LEN 12
#define HDMI_HDCP_MSG_AKE_CERT_LEN 534
#define HDMI_HDCP_MSG_AKE_NO_STORED_KM_LEN 129
#define HDMI_HDCP_MSG_AKE_STORED_KM_LEN 33
#define HDMI_HDCP_MSG_AKE_H_PRIME_LEN 33
#define HDMI_HDCP_MSG_AKE_PAIRING_INFO 17
#define HDMI_HDCP_MSG_LC_INIT_LEN 9
#define HDMI_HDCP_MSG_LC_L_PRIME_LEN 33
#define HDMI_HDCP_MSG_SKE_EKS_LEN 25
#define HDMI_HDCP_MSG_AUTH_RCVID_LIST_LEN (22 + 5 * 31)
#define HDMI_HDCP_MSG_REPEATER_AUTH_ACK_LEN 17
#define HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_MANAGE_LEN (6 + 2 * 31)
#define HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_READY_LEN 33

#define HDMI_HDCP_2_2_VERSION 2
#define HDMI_HDCP_GET_AUTH_RCVID_LIST_LEN(x) (22 + (5 * (x)))

struct HdmiHdcpMsg {
    /* HDCP 1.4 message */
    uint8_t bksv[HDMI_HDCP_MSG_BKSV_LEN];
    uint8_t ri[HDMI_HDCP_MSG_RI_LEN];
    uint8_t pj;
    uint8_t aksv[HDMI_HDCP_MSG_AKSV_LEN];
    uint8_t ainfo;
    uint8_t an[HDMI_HDCP_MSG_AN_LEN];
    uint8_t vh[HDMI_HDCP_MSG_ALL_VH_LEN];
    uint8_t bcaps;
    uint8_t bstatus[HDMI_HDCP_MSG_BSTATUS_LEN];
    uint8_t ksvList[HDMI_HDCP_MSG_KSV_MAX_LEN];
    uint32_t ksvLen;

    /* HDCP 2.2 Message */
    uint8_t Hdcp2Version;
    uint8_t rxStatus[HDMI_HDCP_MSG_RXSTATUS_LEN];
    uint8_t akeInit[HDMI_HDCP_MSG_AKE_INIT_LEN];
    uint8_t akeCert[HDMI_HDCP_MSG_AKE_CERT_LEN];
    uint8_t akeNoStoredKm[HDMI_HDCP_MSG_AKE_NO_STORED_KM_LEN];
    uint8_t akeStoredKm[HDMI_HDCP_MSG_AKE_STORED_KM_LEN];
    uint8_t akeHPrime[HDMI_HDCP_MSG_AKE_H_PRIME_LEN];
    uint8_t akePairingInfo[HDMI_HDCP_MSG_AKE_PAIRING_INFO];
    uint8_t lcInit[HDMI_HDCP_MSG_LC_INIT_LEN];
    uint8_t lcLPrime[HDMI_HDCP_MSG_LC_L_PRIME_LEN];
    uint8_t skeEks[HDMI_HDCP_MSG_SKE_EKS_LEN];
    uint8_t rxIdList[HDMI_HDCP_MSG_AUTH_RCVID_LIST_LEN];
    uint32_t rxIdListLen;
    uint8_t authAck[HDMI_HDCP_MSG_REPEATER_AUTH_ACK_LEN];
    uint8_t authStreamManage[HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_MANAGE_LEN];
    uint32_t authStreamManageLen;
    uint8_t authStreamReady[HDMI_HDCP_MSG_REPEATER_AUTH_STREAM_READY_LEN];
};

enum HdmiHdcpAuthState {
    HDMI_HDCP_AUTH_INACTIVE = 0,
    HDMI_HDCP_AUTH_AKSV_INVALID = 1,
    HDMI_HDCP_AUTH_DOING = 2,
    HDMI_HDCP_AUTH_DONE = 3,
    HDMI_HDCP_AUTH_FAIL = 4,
};

#define HDMI_HDCP_AUTH_MAX_RETRY_CNT 10
struct HdmiHdcp {
    struct OsalMutex hdcpMutex;
    enum HdmiHdcpAuthState state;
    uint8_t authRetryCnt;
    /* HDCP 1.4 */
    uint8_t aksv[HDMI_HDCP_MSG_AKSV_LEN];  /* need driver to generate */
    uint8_t an[HDMI_HDCP_MSG_AN_LEN];      /* need driver to generate */
    uint8_t bksv[HDMI_HDCP_MSG_BKSV_LEN];
    uint8_t bcaps;
    uint8_t ri[HDMI_HDCP_MSG_RI_LEN];
    uint8_t bstatus[HDMI_HDCP_MSG_BSTATUS_LEN];
    uint8_t deviceCount;
    uint8_t ksvList[HDMI_HDCP_MSG_KSV_MAX_LEN];
    uint32_t ksvLen;
    uint8_t vh[HDMI_HDCP_MSG_ALL_VH_LEN];
    bool aksvValid;
    void *priv;
};

struct HdmiHdcpStatus {
    bool hdcp22;
    bool hdcp14;
    uint8_t bksv[HDMI_HDCP_MSG_BKSV_LEN];
    uint8_t aksv[HDMI_HDCP_MSG_AKSV_LEN];
};

int32_t HdmiHdcpOpen(struct HdmiHdcp *hdcp);
void HdmiHdcpClose(struct HdmiHdcp *hdcp);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_HDCP_H */
