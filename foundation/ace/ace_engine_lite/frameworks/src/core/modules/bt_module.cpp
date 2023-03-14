/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "bt_module.h"
#include "ace_log.h"
#include "js_app_context.h"
#include "async_task_manager.h"

namespace OHOS {
namespace ACELite {

#ifndef LOSCFG_SOC_SERIES_BES2600
extern "C" void jsUI_BtSearchDeviceCancelMsg(void);
extern "C" void jsUI_BtNotify_bth_incoming_response(uint8_t );
#endif
extern "C" void jsUI_BtSearchDeviceStartMsg(void);
extern "C" void jsUI_BtConnectDeviceByNameMsg(char *bt_name);
extern "C" void jsUI_BleWTInit(void);
extern "C" void jsUI_BleWTDeinit(void);
extern "C" void jsUI_BleWTStartScan(void);
extern "C" void isUI_BleWTStopScan(void);
extern "C" void jsUI_BleWTStartCapture(void);
extern "C" void jsUI_BleWTStopCapture(void);


JSIValue BtModule::m_on_callback = NULL;
JSIValue BtModule::m_on_this = NULL;
uint8_t BtModule::m_bt_scan_status = 0;

bt_event_map_t BtModule::m_event_map[BT_EVENT_MAP_NUM] = {
    {"connect",    0, 0},
    {"disconnect", 0, 0},
    {"dial",     0, 0},
    {"hungup", 0, 0},
    {"incoming_number", 0, 0},
    {"bt_scan", 0, 0},
    {"search_result", 0, 0},
    {"walkie_talkie_dial", 0, 0},
    {"walkie_talkie_online", 0, 0},
    {"audio_start", 0, 0},
    {"audio_pause", 0, 0},
};

int BtModule::GetEventIdx(const JSIValue target)
{
    int idx = -1;
    int total = sizeof(m_event_map)/sizeof(m_event_map[0]);
    char* str_target = NULL;
    for(int ii=0; ii<total; ii++)
    {
        str_target = JSI::ValueToString(target);
        if(memcmp(str_target,m_event_map[ii].m_event_owner, strlen(str_target) ) == 0)
        {
            idx = ii;
            break;
        }
    }
    return idx;
}

int BtModule::SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this)
{
    int total = sizeof(m_event_map)/sizeof(m_event_map[0]);
    if(_idx >= total || _idx < 0)
    {
        return -1;
    }

    m_event_map[_idx].m_callback = _callback;
    m_event_map[_idx].m_this = _this;

    return 0;
}

int BtModule::SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this)
{
    int _idx = GetEventIdx(_owner);

    return SetEventMap(_idx, _callback, _this);
}

int BtModule::GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this)
{
    int total = sizeof(m_event_map)/sizeof(m_event_map[0]);
    if(_idx >= total || _idx < 0)
    {
        return -1;
    }

    *_callback = m_event_map[_idx].m_callback;
    *_this     = m_event_map[_idx].m_this;

    return 0;
}

int BtModule::GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this)
{
    int _idx = GetEventIdx(_owner);

    return GetEventMap(_idx, _callback, _this);
}

JSIValue BtModule::TestJSICallback(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    if ((args == nullptr) || (argsNum == 0) || (JSI::ValueIsUndefined(args[0]))) {
        return undefValue;
    }

    m_on_this     = thisVal;
    m_on_callback = JSI::GetNamedProperty(args[0], CB_CALLBACK);
    HILOG_INFO(HILOG_MODULE_ACE,"%s %d regiter callback=%#x this %p", __func__,__LINE__, m_on_callback, m_on_this);
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothON(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    char *type = JSI::ValueToString(args[0]);

    SetEventMap(args[0], args[1], thisVal);

    JSIValue callback = args[1];
    JSIValue argv[ARGC_ONE] = {JSI::CreateNumber(argsNum)};
    //JSI::CallFunction(callback, thisVal, argv, ARGC_ONE);
    HILOG_ERROR(HILOG_MODULE_ACE,"%s %d %s register callback=%#x this %p", __func__,__LINE__,type,callback, thisVal);
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothOFF(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    char *type = JSI::ValueToString(args[0]);

    SetEventMap(args[0], 0, 0);
    HILOG_ERROR(HILOG_MODULE_ACE,"%s %d %s this %p", __func__,__LINE__,type, thisVal);
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if(!BtModule::m_bt_scan_status)
    {
        BtModule::m_bt_scan_status = 1;
        jsUI_BtSearchDeviceStartMsg();
    }
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothScanCancel(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if(BtModule::m_bt_scan_status)
    {
        BtModule::m_bt_scan_status = 0;
#ifndef LOSCFG_SOC_SERIES_BES2600
        jsUI_BtSearchDeviceCancelMsg();
#endif
    }

    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    
     char *bt_name = JSI::ValueToString(args[0]);
    jsUI_BtConnectDeviceByNameMsg(bt_name);
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothHungUp(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#ifndef LOSCFG_SOC_SERIES_BES2600
    jsUI_BtNotify_bth_incoming_response(2);
#endif
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothAnswer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
#ifndef LOSCFG_SOC_SERIES_BES2600
    jsUI_BtNotify_bth_incoming_response(1);
#endif
    return JSI::CreateUndefined();
}

#ifdef BLE_WALKIE_TALKIE
JSIValue BtModule::BluetoothBleWTInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    jsUI_BleWTInit();
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothBleWTDeinit(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    jsUI_BleWTDeinit();
    return JSI::CreateUndefined();
}


JSIValue BtModule::BluetoothBleWTStartScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    jsUI_BleWTStartScan();
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothBleWTStopScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    isUI_BleWTStopScan();
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothBleWTStartCapture(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    jsUI_BleWTStartCapture();
    return JSI::CreateUndefined();
}

JSIValue BtModule::BluetoothBleWTStopCapture(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    jsUI_BleWTStopCapture();
    return JSI::CreateUndefined();
}
#endif

static void bt_notify_async(void* data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "bt notify async failed with null input parameter");
        return;
    }

    auto event = static_cast<const char *>(data);

    HILOG_INFO(HILOG_MODULE_ACE,"%s notify event %s", __func__, event);

    JSIValue _callback = nullptr;
    JSIValue _this     = nullptr;

    JSIValue key = JSI::CreateString(event);
    BtModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);

    if(_callback == nullptr || _this == nullptr)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s notify event %s failed", __func__, event);
        return;
    }   
    

    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, nullptr, 0);
}


extern "C" void bt_notify_incoming_call_ui(int flag)
{
    void* data = nullptr;
    if(flag){
        data = (void*)("dial");
    }else{
         data = (void*)("hungup");
    }

      // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(bt_notify_async, data);

    if(res == DISPATCH_FAILURE){
         HILOG_ERROR(HILOG_MODULE_ACE, "bt_module bt_notify_async dispatch failed"); 
    }
}


extern "C" void notify_update_bt_ui(int flag)
{

    void* data = nullptr;

    if(flag){
        data = (void*)("connect");
    }else{
         data = (void*)("disconnect");
    }
    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(bt_notify_async, data);

    if(res == DISPATCH_FAILURE){
         HILOG_ERROR(HILOG_MODULE_ACE, "bt_module bt_notify_async dispatch failed"); 
    }

}

static void incoming_number_async(void* data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "search result async failed with null input parameter");
        return;
    }

    
    JSIValue _callback = NULL;
    JSIValue _this     = NULL;

    JSIValue key = JSI::CreateString("incoming_number");
    BtModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);

    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called incoming number callback failure", __func__);
        return;
    }   
    
    auto number = static_cast<const char *>(data);

    JSIValue number_value = JSI::CreateString((const char *)number);
    JSIValue number_argv[ARGC_ONE] = {JSI::CreateString((const char *)number),};
    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, number_argv, ARGC_ONE);
    JSI::ReleaseValue(number_value);
}

extern "C" void bt_notify_incoming_call_number_ui(void* number)
{
    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(incoming_number_async, number);

    if(res == DISPATCH_FAILURE){
         HILOG_ERROR(HILOG_MODULE_ACE, "bt_module reflesh_bt_source_search_result dispatch failed"); 
    }
    
}




static void search_result_async(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "search result async failed with null input parameter");
        return;
    }

    JSIValue _callback = NULL;
    JSIValue _this     = NULL;

    JSIValue key = JSI::CreateString("search_result");
    BtModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);
    
    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called dial callback failure", __func__);
        return;
    }   

    auto bt_name = static_cast<const char *>(data);

    JSIValue bt_name_value = JSI::CreateString(bt_name);
    JSIValue bt_name_argv[ARGC_ONE] = {
        bt_name_value,
    };

    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, bt_name_argv, ARGC_ONE);

    JSI::ReleaseValue(bt_name_value);
}


extern "C" void reflesh_bt_source_search_result(void* bt_name)
{
    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(search_result_async, bt_name);

    if(res == DISPATCH_FAILURE){
         HILOG_ERROR(HILOG_MODULE_ACE, "bt_module reflesh_bt_source_search_result dispatch failed"); 
    }
}

static void notify_a2dp_stream_state_change_async(void* data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "notify a2dp stream state change async failed with null input parameter");
        return;
    }

    auto event = static_cast<const char *>(data);

    HILOG_INFO(HILOG_MODULE_ACE,"%s notify event %s", __func__, event);

    JSIValue _callback = nullptr;
    JSIValue _this     = nullptr;

    JSIValue key = JSI::CreateString(event);
    BtModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);

    if(_callback == nullptr || _this == nullptr)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s notify event %s failed", __func__, event);
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, nullptr, 0);
}

extern "C" void notify_update_a2dp_stream_state_ui(int flag)
{
    void* data = nullptr;
    if(flag) {
        data = (void*)("audio_start");
    } else {
        data = (void*)("audio_pause");
    }

    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(notify_a2dp_stream_state_change_async, data);

    if(res == DISPATCH_FAILURE) {
         HILOG_ERROR(HILOG_MODULE_ACE, "bt_module notify_update_a2dp_stream_state_ui dispatch failed");
    }
}

#ifdef BLE_WALKIE_TALKIE
static void notify_incomming_wt_ui_async(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "search result async failed with null input parameter");
        return;
    }

    JSIValue _callback = NULL;
    JSIValue _this     = NULL;
    BtModule::GetEventMap(JSI::CreateString("walkie_talkie_dial"), &_callback, &_this);
    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called wt dial callback failure", __func__);
        delete data;
        return;
    }

    auto count = static_cast<int *>(data);
    JSIValue argv[ARGC_ONE] = {JSI::CreateNumber(*count)};
    delete count;
    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, argv, ARGC_ONE);
}

extern "C" void ble_notify_incomming_wt_ui(int count)
{
    int* new_count = new int(count);
    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(notify_incomming_wt_ui_async, new_count);

    if(res == DISPATCH_FAILURE){
        delete new_count;
        HILOG_ERROR(HILOG_MODULE_ACE, "bt_module ble_notify_incomming_wt_ui dispatch failed"); 
    }
}

static void notify_wt_nebor_changed_ui_async(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "search result async failed with null input parameter");
        return;
    }

    JSIValue _callback = NULL;
    JSIValue _this     = NULL;
    BtModule::GetEventMap(JSI::CreateString("walkie_talkie_online"), &_callback, &_this);
    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called nebor count callback failure", __func__);
        delete data;
        return;
    }

    auto count = static_cast<int *>(data);
    JSIValue argv[ARGC_ONE] = {JSI::CreateNumber(*count)};
    delete count;
    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, argv, ARGC_ONE);
}

extern "C" void ble_notify_wt_nebor_changed_ui(int count)
{
    int* new_count = new int(count);
    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(notify_wt_nebor_changed_ui_async, new_count);

    if(res == DISPATCH_FAILURE){
        delete new_count;
        HILOG_ERROR(HILOG_MODULE_ACE, "bt_module ble_notify_wt_nebor_changed_ui dispatch failed"); 
    }
}

#endif

} // namespace ACELite
} // namespace OHOS
