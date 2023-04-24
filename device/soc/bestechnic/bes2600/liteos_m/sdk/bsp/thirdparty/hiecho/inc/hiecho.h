
/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __HIECHO_H__
#define __HIECHO_H__

#ifdef HIECHO_ENABLE

#include "bluetooth.h"

#define HIECHO_ADDR_LEN             6
#define ICARRY_NAM_MAX_TLV_LEN      64
#define DTS_NAME_MAX_LEN            32
#define SERVICE_CAP_MAX_LEN         10

enum HiechoParseMsgType {
    SERVICE_CAP         = 0x01,
    AUDIO_SCENE         = 0x02,
    SPORT_SCENE         = 0x03,
    AUDIO_DELAY         = 0x04,
    SDP_QUERY           = 0x09,
    SECU_ADV            = 0x11,
    ICARRY_ROLE         = 0x12,
    ICARRY_DEVICE       = 0x13,
    ICARRY_NAME         = 0x14,
    ICARRY_STATE        = 0x15,
    ICARRY_CAP          = 0x16,
    AUTO_PAIR_STATE     = 0x17,
    AUDIO_CODEC_RATE    = 0x20,
    NOTIFY_PROFILE_STATE = 0x21,
    NOTIFY_OUTPUT_PATH   = 0x22
};

enum HiechoErrorCode {
    HIECHO_SUCCESS                    = 0x0,
    HIECHO_ERR_UNKOWN_ERROR_TYPE      = 0x01,
    HIECHO_ERR_UNSUPPORTED_SERVICE    = 0x02,
    HIECHO_ERR_UNSUPPORTED_COMMAND    = 0x03,
    HIECHO_ERR_NO_PERMISSION          = 0x04,
    HIECHO_ERR_SYSTEM_BUSY            = 0x05,
    HIECHO_ERR_REQUEST_FORMAT_ERROR   = 0x06,
    HIECHO_ERR_PARA_ERROR             = 0x07,
    HIECHO_ERR_ALLOC_MEMORY_FAIL      = 0x08,
    HIECHO_ERR_RSP_TIMEOUT            = 0x09
};

enum HiechoTransport {
    HIECHO_TRANSPORT_BR_EDR = 0x01,
    HIECHO_TRANSPORT_LE     = 0x02
};

enum HiechoType{
    HIECHO_REQ = 0x01,
    HIECHO_RSP = 0x02
};

typedef struct{
    uint8_t bd[HIECHO_ADDR_LEN];
    uint8_t linkType;
    uint8_t id;
} HiechoRecvMsgArg;

typedef struct {
    uint8_t msgType;
    uint16_t datalen;
    uint8_t data[0];
} HiechoParseMsgInd;

typedef struct {
    uint8_t rspResult;
} HiechoRspCommonResult;

typedef struct {
    uint8_t rspData;
} HiechoRecvRspMsg;

//serviceCap inquiry rsp&req
typedef struct {
    uint8_t serivceCap[SERVICE_CAP_MAX_LEN];
} ServiceManangeData;

enum AudioSceneValue {
    AUDIO_SCENE_TYPE_MUSIC    = 0x10,
    AUDIO_SCENE_TYPE_GAME     = 0x11,
};

enum SportSceneValue {
    SPORT_SCENE_TYPE_WALKING    = 0x01,
    SPORT_SCENE_TYPE_RUNNING    = 0x02,
    SPORT_SCENE_TYPE_RIDING     = 0x03
};

//AudioScene inquiry rsp&req
typedef struct {
    uint8_t audioScene;
} AudioSceneData;

//SportScene inquiry req
typedef struct {
    uint8_t sportScene;
} SportSceneData;

typedef struct {
    uint8_t audioDelay;
} AudioDelayData;

typedef struct {
    uint8_t mode;
    uint32_t profile;
    uint8_t product;
    uint16_t twsVersion;
} SdpCapPairConnNormal;

typedef struct {
    uint8_t pairType;
    union {
        SdpCapPairConnNormal sdpNormal;
        HiechoRspCommonResult sdpAbnormal;
    } p;
} SdpCapPairConn;

#define SEC_ADV_IRK_LEN     16
#define SEC_ADV_HBK_LEN     16
typedef struct {
    uint8_t secLevel;
    uint8_t irk[SEC_ADV_IRK_LEN];
    uint8_t hbk[SEC_ADV_HBK_LEN];
    uint8_t addr[HIECHO_ADDR_LEN];
} SecurityAdv;

typedef struct {
    uint8_t icarryRole;
} IcarryClassData;

enum IcarryAction {
    REMOVE_ICARRY        = 0x00,
    PREEMPT_ICARRY      = 0x01,
    CREATE_ICARRY       = 0x02,
    PREEMPT_EARBUDS     = 0x03
};

typedef struct {
    uint8_t icarryAction;
    uint8_t addr[HIECHO_ADDR_LEN];
    uint8_t earbudsAddr[HIECHO_ADDR_LEN];
} IcarryDeviceNotify;

typedef struct {
    uint8_t addr[HIECHO_ADDR_LEN];
    uint8_t nameLen;
    uint8_t nameStr[ICARRY_NAM_MAX_TLV_LEN];
} IcarryNameNotify;

enum IcarryOper {
    ICARRY_CONNECT_STATE    = 0x01,
    ICARRY_WEAR_STATE       = 0x02,
    ICARRY_ANSWER_STATE     = 0x03,
    ICARRY_BUSINESS_STATE   = 0x04,
};

typedef struct {
    uint8_t icarryOper;
    uint8_t icarryOperValue;
    uint8_t addr[HIECHO_ADDR_LEN];
} IcarryStateNotify;

typedef struct {
    uint8_t phoneAnswerCap;
    uint8_t wearState;
    uint8_t addr[HIECHO_ADDR_LEN];
} IcarryCapNotify;

typedef struct {
    uint8_t autoPairMode;
} AutoPairStateNotify;

typedef struct {
    uint8_t supportRate;
} HiechoSupportRate;

typedef struct {
    uint8_t a2dpProfile;
    uint8_t a2dpBusiness;
    uint8_t hfpProfile;
    uint8_t hfpBusiness;
    uint8_t a2dpNameLen;
    uint8_t hfpNameLen;
    uint8_t a2dpName[DTS_NAME_MAX_LEN];
    uint8_t hfpName[DTS_NAME_MAX_LEN];
} HiechoProfileState;

typedef struct {
    uint8_t hfpOperation;//
    uint8_t hfpBusiness;//?
    uint8_t a2dpOperation;
    uint8_t a2dpBusiness;
} HiechoOutDatapath;

typedef void (*HiechoSendReqMsgHandler) (uint8_t* device_addr, uint8_t* rsp_data, uint16_t rsp_data_len);
typedef void (*HiechoSendRspMsgHandler) (uint8_t* args, uint8_t* rsp_data, uint16_t rsp_data_len);
typedef void (*HiechoMsgIndHandler) (uint8_t echoType, uint8_t* rsp_data, uint16_t rsp_data_len, uint8_t* args);
typedef void (*HiechoMsgCfmHandler) (uint8_t echoType, uint8_t* rsp_data, uint16_t rsp_data_len, uint8_t* args);

typedef struct {
    HiechoSendReqMsgHandler sendReqFunc;
    HiechoSendRspMsgHandler sendRspFunc;
    HiechoMsgIndHandler     msgIndFunc;
    HiechoMsgCfmHandler     msgCfmFunc;
} HiechoRegisterFunc;

__attribute__((weak))  void HiechoRegisterProcess(HiechoRegisterFunc *cb);

__attribute__((weak)) bool HiechoRecvIndMsg (uint8_t* echoReq,uint16_t dataLen,uint8_t* args);
__attribute__((weak)) bool HiechoRecvCfmdMsg (uint8_t* echoRsp,uint16_t dataLen,uint8_t* args);
__attribute__((weak)) bool HiechoCombReqMsg (uint8_t* addr,uint8_t* source,uint16_t dataLen);
__attribute__((weak)) bool HiechoCombRspMsg (uint8_t* args,uint8_t* rspDta,uint16_t dataLen);

#endif /* HIECHO_ENABLE */

#endif /* __APP_BT_HIECHO_H__ */

