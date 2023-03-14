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

#ifndef __WIFI_APP_VOICE__
#define __WIFI_APP_VOICE__

#ifdef __cplusplus
extern "C" {
#endif


// #define DMA_BUFFER_SIZE (4096*2)

enum APP_VOICE_TYPE_T {
    APP_VOICE_TYPE_HELLO = 1,
	APP_VOICE_TYPE_AIRKISS_START,
	APP_VOICE_TYPE_AIRKISS_SUCCESS,
	APP_VOICE_TYPE_WECHAT_NEWMSG,
	APP_VOICE_TYPE_WECHAT_NO_NEWMSG,
	APP_VOICE_TYPE_WECHAT_SEND_SUCCESS,
	APP_VOICE_TYPE_OTA_START,
	APP_VOICE_TYPE_POWER_ON,
	APP_VOICE_TYPE_POWER_OFF,
	APP_VOICE_TYPE_SERVER_CONNECTED,
	APP_VOICE_TYPE_SERVER_DISCONNECTED,

    AUD_VOICE_NUM,
};

// extern int answer_voice_state;

void voice_play_start(enum APP_VOICE_TYPE_T voice_id);
void voice_play_stop(void);
int voice_play_stat(void);

#ifdef __cplusplus
}
#endif

#endif

