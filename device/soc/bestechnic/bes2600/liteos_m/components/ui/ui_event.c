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
#include "ohos_init.h"
#include "log.h"
//#include <cJSON.h>
#include "cmsis_os2.h"
// #include "sys2bth_bt_source.h"
#include "ui_event.h"
#include "string.h"


osThreadId_t jsUI_EventTaskId = NULL;
osMessageQueueId_t jsUI_EventQueueId = NULL;

JSUI_BT_DEVICE_LIST jsui_bt_device_list;

//extern "C" int register_ui_bt_event_msg_send_func(UI_BT_EVENT_MSG_SEND_FUNC p_func);
//extern "C" void reflesh_bt_source_search_result(const void* bt_name);

#ifdef IMS_LTE
extern int tmtc_volte_init(void);
extern int tmtc_volte_reg(void);
extern int tmtc_voip_call(char *pcUri);
extern int tmtc_volte_answer(void);
extern int tmtc_volte_term(void);

void jsUI_VolteInitReg()
{
    tmtc_volte_init();
    tmtc_volte_reg();
}

void jsUI_VolteCallkey(char *phone_num)
{
    tmtc_voip_call(phone_num);
}

void jsUI_VolteAnswer()
{
    tmtc_volte_answer();
}

void jsUI_VolteTerm()
{
    tmtc_volte_term();
}

void jsUI_CallIn(void)
{
    jsUI_SendMsgToEventManager_MulPara(LTE_TO_UI_MSG_CALL_IN_EVENT, NULL, 0);
}
#endif

void mcu_notify_bth_incoming_response(uint8_t answer)
{

}

void jsUI_BtSearchDeviceStartMsg(void)
{
    // jsUI_SendMsgToEventManager_MulPara(UI_TO_BT_MSG_SEARCH_START_EVENT, NULL, 0);
}

void jsUI_BtConnectDeviceByNameMsg(char *bt_name)
{
    // jsUI_SendMsgToEventManager_MulPara(UI_TO_BT_MSG_CONNECT_DEVICE_EVENT, bt_name, strlen(bt_name));
}

int jsUI_SendMsgToEventManager(JSUI_EVENT_MSG *pMsg)
{
    int ret;
    ret = (int)osMessageQueuePut(jsUI_EventQueueId, pMsg, 0U, 0U);
    return ret;
}

static int jsUI_SendMsgToEventManager_MulPara(JSUI_EVENT_MSG_TYPE type, uint8_t *data, uint8_t data_len)
{
    int ret;
    JSUI_EVENT_MSG msg;
    memset(&msg, 0 ,sizeof(JSUI_EVENT_MSG));
    msg.msg_type = type;
    msg.msg_arg_len = data_len;
    if(data != NULL)
    {
        memcpy(msg.msg_data, data, data_len);
    }
    ret = jsUI_SendMsgToEventManager(&msg);
    return ret;
}

static jsUI_EventManagerStatus_t jsUI_MatchAndConnectListDevice(char *bt_name, uint8_t name_len)
{
    HILOG_ERROR(HILOG_MODULE_APP, "%s...connect...%s\r\n",__func__, bt_name);
    int ret = 0;
    uint8_t list_device_num = jsui_bt_device_list.device_num;
    for(uint8_t i = 0; i < list_device_num; i++)
    {
        ret = memcmp( bt_name, jsui_bt_device_list.index[i].bt_name, name_len);
        if( ret == 0)
        {
            // bt_search_connect_device_by_addr(&jsui_bt_device_list.index[i].bt_addr);
            return JSUI_CONNECT_DEVICE_SUCCESS;
        }
    }
    return JSUI_CONNECT_DEVICE_FAIL;
}

static void jsUI_BtConnectDeviceByName(char *bt_name)
{
    if(bt_name == NULL)
    {
        return;
    }

    jsUI_EventManagerStatus_t ret =0;
    uint8_t name_len = 0;
    JSUI_BT_DEVICE device_temp;
    memset(&device_temp, 0, sizeof(JSUI_BT_DEVICE));
    name_len = strlen(bt_name);
    if(name_len > JSUI_BT_NAME_MAX_LEN)
    {
        name_len = JSUI_BT_NAME_MAX_LEN;
    }
    memcpy(&device_temp.bt_name, bt_name, name_len);
    ret = jsUI_MatchAndConnectListDevice(&device_temp.bt_name, name_len);
}

static void jsUI_EventMsgQueueInit(void)
{
    jsUI_EventQueueId = osMessageQueueNew(JSUI_EVENT_MSG_QUEUE_CNT, JSUI_EVENT_MSG_SIZE, NULL);
    if (jsUI_EventQueueId == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "Failed to create %s\r\n",__func__);
    }
}


static jsUI_EventManagerStatus_t jsUI_FilterBtDevice(JSUI_BT_DEVICE *device, uint8_t name_len)
{
    uint8_t list_device_num = jsui_bt_device_list.device_num;
    int ret_a = 0;
    int ret_b = 0; 

    if(list_device_num == 0)
    {
        return JSUI_SEARCH_NEW_DEVICE;
    }
    else if(list_device_num == JSUI_BT_SEARCH_NUM)
    {
        return JSUI_SEARCH_FULL_DEVICE;
    }

    for(uint8_t i = 0; i < list_device_num; i++)
    {
        ret_a = memcmp( device->bt_addr, jsui_bt_device_list.index[i].bt_addr, JSUI_BT_MAC_LEN);
        ret_b = memcmp( device->bt_name, jsui_bt_device_list.index[i].bt_name, name_len);
        if( (ret_a != 0) && (ret_b != 0))
        {
            return JSUI_SEARCH_NEW_DEVICE;
        }
    }
    return JSUI_SEARCH_OLD_DEVICE;
}

static jsUI_EventManagerStatus_t jsUI_CacheDeviceList(JSUI_EVENT_MSG *msg)
{
    jsUI_EventManagerStatus_t ret = 0;
    JSUI_BT_DEVICE device_temp;
    uint8_t name_len =  msg->msg_arg_len - JSUI_BT_MAC_LEN;
    memset(&device_temp, 0, sizeof(device_temp));
    memcpy(device_temp.bt_addr, msg->msg_data, JSUI_BT_MAC_LEN);
    memcpy(device_temp.bt_name, msg->msg_data+JSUI_BT_MAC_LEN, name_len);

    ret =  jsUI_FilterBtDevice(&device_temp, name_len);
    
    if(ret == JSUI_SEARCH_NEW_DEVICE)
    {
        memcpy(jsui_bt_device_list.index[jsui_bt_device_list.device_num].bt_addr, device_temp.bt_addr, JSUI_BT_MAC_LEN);
        memcpy(jsui_bt_device_list.index[jsui_bt_device_list.device_num].bt_name, device_temp.bt_name, name_len);
        if(jsui_bt_device_list.device_num < 8)
        {
            jsui_bt_device_list.device_num++;
        }
    }
    return ret;
}


static void jsUI_EventManagerTask(void *arg)
{
    osStatus_t status;
    JSUI_EVENT_MSG msg;

    while(1)
    {
        status = osMessageQueueGet(jsUI_EventQueueId, &msg, NULL, osWaitForever);
        HILOG_INFO(HILOG_MODULE_APP, "HILOG_INFO %s \r\n", __func__);
        if(status == osOK)
        {
            switch (msg.msg_type)
            {
            case BT_TO_UI_MSG_SEARCH_RESULT_EVENT:
                if(jsUI_CacheDeviceList(&msg) == JSUI_SEARCH_NEW_DEVICE)
                {
                    reflesh_bt_source_search_result(jsui_bt_device_list.index[jsui_bt_device_list.device_num - 1].bt_name);
                }
                break;
            case LTE_TO_UI_MSG_CALL_IN_EVENT:
                jsUI_LteNotifyIncomingCallUi(1);
                break;

            case UI_TO_BT_MSG_SEARCH_START_EVENT:
                memset(&jsui_bt_device_list, 0, sizeof(JSUI_BT_DEVICE_LIST));
                // sys_request_bth_bt_device_search();
                break;

            case UI_TO_BT_MSG_CONNECT_DEVICE_EVENT:
                jsUI_BtConnectDeviceByName(&msg.msg_data);
                break;

            default:
                break;
            }
        }
    }
}

static void jsUI_EventManagerInit(void)
{
    HILOG_INFO(HILOG_MODULE_APP, "HILOG_INFO %s\r\n", __func__);
    osThreadAttr_t attr = {0};
    attr.stack_size = 64*1024;
    attr.priority = osPriorityAboveNormal;
    attr.name = "jsUI_EventManager";
    jsUI_EventMsgQueueInit();
    jsUI_EventTaskId = osThreadNew((osThreadFunc_t)jsUI_EventManagerTask, NULL, &attr);
    if (jsUI_EventTaskId == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "Failed to create jsUI_event_manager_task\r\n");
    }
}

void jsUI_EventInit(void)
{
    jsUI_EventManagerInit();
#ifdef IMS_LTE
    Mtc_CallCbSetIncoming(jsUI_CallIn);
#endif
    // register_ui_bt_event_msg_send_func(jsUI_SendMsgToEventManager);
    HILOG_INFO(HILOG_MODULE_APP, "HILOG_INFO %s\r\n", __func__);
}
#ifdef IMS_LTE
APP_FEATURE_INIT(jsUI_EventInit);
#endif