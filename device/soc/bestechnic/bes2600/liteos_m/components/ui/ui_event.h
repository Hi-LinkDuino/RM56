/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GRAPHIC_LITE_UI_EVENT_H
#define GRAPHIC_LITE_UI_EVENT_H

#define JSUI_EVENT_MSG_QUEUE_CNT (64)
#define JSUI_EVENT_MSG_SIZE (128)
#define JSUI_BT_NAME_MAX_LEN (64)
#define JSUI_BT_MAC_LEN (6)
#define JSUI_BT_SEARCH_NUM (8)

typedef enum {
    JSUI_SEARCH_NEW_DEVICE = 0,
    JSUI_SEARCH_OLD_DEVICE,
    JSUI_SEARCH_FULL_DEVICE,
    JSUI_CONNECT_DEVICE_SUCCESS,
    JSUI_CONNECT_DEVICE_FAIL,
}jsUI_EventManagerStatus_t;

typedef enum{
    BT_TO_UI_MSG_SEARCH_RESULT_EVENT = 0,

    UI_TO_BT_MSG_SEARCH_START_EVENT = 0x0100,
    UI_TO_BT_MSG_CONNECT_DEVICE_EVENT,

}JSUI_EVENT_MSG_TYPE;


typedef struct
{
    uint16_t  msg_type;
    uint8_t msg_arg_len;
    uint8_t *msg_data;
}JSUI_EVENT_MSG;


typedef struct
{
    unsigned char bt_addr[JSUI_BT_MAC_LEN];
    char bt_name[JSUI_BT_NAME_MAX_LEN];
} JSUI_BT_DEVICE;

typedef struct 
{
    unsigned char device_num;
    JSUI_BT_DEVICE index[JSUI_BT_SEARCH_NUM];
} JSUI_BT_DEVICE_LIST;


int jsUI_SendMsgToEventManager(JSUI_EVENT_MSG *pMsg);

static int jsUI_SendMsgToEventManager_MulPara(JSUI_EVENT_MSG_TYPE type, uint8_t *data, uint8_t data_len);

#endif
