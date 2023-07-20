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

#include "ota_module.h"
#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_profiler.h"
#include "jsi/internal/jsi_internal.h"

namespace OHOS {
namespace ACELite {

JSIValue OtaModule::m_on_callback = NULL;
JSIValue OtaModule::m_on_this = NULL;

ota_event_map_t OtaModule::m_event_map[OTA_EVENT_MAP_NUM] = {
    {"bindResponse", 0, 0},
};

void InitOtaModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "bindResponse", OtaModule::BindResponse);
}

JSIValue OtaModule::BindResponse(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    SetEventMap(args[0], args[1], thisVal);
    HILOG_ERROR(HILOG_MODULE_ACE,"%s %d this %p",  __func__,__LINE__, thisVal);
    return JSI::CreateUndefined();
}

static void ota_responde_progress(uint8_t resType, void* data)
{
    if (data == nullptr) {
        if((resType != 1) && (resType != 0) && (resType != 3)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "search result async failed with null input parameter");
            return;
        }
    }

    HILOG_ERROR(HILOG_MODULE_ACE,"%s %d, type = %d", __func__, __LINE__, resType);
    JSIValue _callback = NULL;
    JSIValue _this     = NULL;

    JSIValue key = JSI::CreateString("bindResponse");
    OtaModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);

    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called incoming number callback failure", __func__);
        return;
    }

    if(resType == 2) {
        auto number = static_cast<const int *>(data);
        JSIValue number_value = JSI::CreateNumber(*number);

        HILOG_ERROR(HILOG_MODULE_ACE,"%s %d, number_value = %p", __func__, __LINE__, number_value);
        JSIValue type = JSI::CreateNumber(resType);

        JSIValue number_argv[ARGC_TWO] = {type, JSI::CreateNumber(*number)};
        delete number;
        HILOG_INFO(HILOG_MODULE_ACE,"%s %d called callback=%#x this %p", __func__, __LINE__, _callback,_this);
        JSI::CallFunction(_callback, _this, number_argv, ARGC_TWO);
        JSI::ReleaseValue(number_value);
    } else {
        JSIValue type = JSI::CreateNumber(resType);
        JSIValue number_argv[ARGC_ONE] = {type, };
        HILOG_INFO(HILOG_MODULE_ACE,"%s %d called callback=%#x this %p", __func__, __LINE__, _callback,_this);
        JSI::CallFunction(_callback, _this, number_argv, ARGC_ONE);
    }
}

static void ota_notify_start_state(void* number)
{
    //HILOG_INFO(HILOG_MODULE_ACE, "ota_notify_start_state");
    ota_responde_progress(0, NULL);
}

static void ota_notify_end_state(void* number)
{
    //HILOG_INFO(HILOG_MODULE_ACE, "ota_notify_end_state");
    ota_responde_progress(1, NULL);
}

static void ota_notify_progress(void* number)
{
    //HILOG_INFO(HILOG_MODULE_ACE, "ota_notify_progress number = %d", number);
    ota_responde_progress(2, number);
}

static void ota_notify_abort_progress(void* number)
{
    HILOG_INFO(HILOG_MODULE_ACE, "ota_notify_abort_state");
    ota_responde_progress(3, NULL);
}

extern "C" void ota_notify_bounding_number_ui(uint8_t resType, int new_number)
{
    int *number = new int(new_number);
    HILOG_ERROR(HILOG_MODULE_ACE, "ota_notify_bounding_number_ui, type = %d, number = %d", resType, number);
    uint16_t res;
    switch(resType){
        case 0:
            res = AsyncTaskManager::GetInstance().Dispatch(ota_notify_start_state, number);
            break;
        case 1:
            res = AsyncTaskManager::GetInstance().Dispatch(ota_notify_end_state, number);
            break;
        case 2:
            res = AsyncTaskManager::GetInstance().Dispatch(ota_notify_progress, number);
            break;
        case 3:
            res = AsyncTaskManager::GetInstance().Dispatch(ota_notify_abort_progress, number);
            break;
        default:
            res = DISPATCH_FAILURE;
            break;
    }
    if(res == DISPATCH_FAILURE){
        delete number;
        HILOG_ERROR(HILOG_MODULE_ACE, "ota_notify_bounding_number_ui dispatch failed");
    }
}


int OtaModule::GetEventIdx(const JSIValue target)
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


int OtaModule::SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this)
{
    int _idx = GetEventIdx(_owner);
    return SetEventMap(_idx, _callback, _this);
}

int OtaModule::SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this)
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

int OtaModule::GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this)
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

int OtaModule::GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this)
{
    int _idx = GetEventIdx(_owner);
    return GetEventMap(_idx, _callback, _this);
}



} // namespace ACELite
} // namespace OHOS
