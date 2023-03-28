/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
////////////////////////////////////////////////////////////////
//

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

typedef enum {
    AUD_ID_INVALID  = -1,
    AUD_ID_POWER_ON = 0x0,
    AUD_ID_POWER_OFF = 0x01,
    AUD_ID_LANGUAGE_SWITCH = 0x02,

    AUD_ID_NUM_0 = 0x03,
    AUD_ID_NUM_1 = 0x04,
    AUD_ID_NUM_2 = 0x05,
    AUD_ID_NUM_3 = 0x06,
    AUD_ID_NUM_4 = 0x07,
    AUD_ID_NUM_5 = 0x08,
    AUD_ID_NUM_6 = 0x09,
    AUD_ID_NUM_7 = 0x0A,
    AUD_ID_NUM_8 = 0x0B,
    AUD_ID_NUM_9 = 0x0C,

    AUD_ID_BT_PAIR_ENABLE = 0x0D,
    AUD_ID_BT_PAIRING = 0x0E,
    AUD_ID_BT_PAIRING_SUC = 0x0F,
    AUD_ID_BT_PAIRING_FAIL = 0x10,
    AUD_ID_BT_CALL_REFUSE = 0x11,
    AUD_ID_BT_CALL_OVER = 0x12,
    AUD_ID_BT_CALL_ANSWER = 0x13,
    AUD_ID_BT_CALL_HUNG_UP = 0x14,
    AUD_ID_BT_CALL_INCOMING_CALL = 0x15,
    AUD_ID_BT_CALL_INCOMING_NUMBER = 0x16,
    AUD_ID_BT_CHARGE_PLEASE = 0x17,
    AUD_ID_BT_CHARGE_FINISH = 0x18,
    AUD_ID_BT_CLEAR_SUCCESS = 0x19,
    AUD_ID_BT_CLEAR_FAIL = 0x1A,
    AUD_ID_BT_CONNECTED = 0x1B,
    AUD_ID_BT_DIS_CONNECT = 0x1C,
    AUD_ID_BT_WARNING = 0x1D,
    AUDIO_ID_BT_ALEXA_START = 0x1E,
    AUDIO_ID_FIND_MY_BUDS = 0x1F,
    AUDIO_ID_FIND_TILE = 0x20,
    AUDIO_ID_BT_ALEXA_STOP = 0x21,
    AUDIO_ID_BT_GSOUND_MIC_OPEN = 0x22,
    AUDIO_ID_BT_GSOUND_MIC_CLOSE = 0x23,
    AUDIO_ID_BT_GSOUND_NC = 0x24,
    AUDIO_ID_BT_MUTE = 0x25,
    AUD_ID_RING_WARNING = 0x26,
#ifdef __INTERACTION__
    AUD_ID_BT_FINDME = 0x27,
#endif
    MAX_RECORD_NUM = 0x28,
    AUD_ID_ENUM_BOTTOM = 0x1001,
} AUD_ID_ENUM;

enum ENUM_RESOURCE_ID
{
    RES_ENGLISH_ID = 0xFF00,
    RES_CHINESE_ID = 0xFF01,

    MAX_RES_LANGUAGE_ID
};

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

void init_audio_resource(void* gResource);

UINT8* aud_get_reouce(AUD_ID_ENUM id, UINT32* leng, UINT16* type);

const char *aud_id2str(UINT16 aud_id);

extern UINT8 BIN_FILE[];

#endif//__RESOURCES_H__

