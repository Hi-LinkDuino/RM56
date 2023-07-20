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
#if (ALIPAY_ENABLE == 1)
#include "alipay_module.h"
#include "ace_log.h"
#include "js_ability_impl.h"
#include "js_app_context.h"
#include "js_profiler.h"
#include "jsi/internal/jsi_internal.h"

#include "hal_tick.h"

#ifdef __cplusplus
extern "C"{
#endif

//#include "alipay.h"

#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace ACELite {

uint8_t  code_result[100]={0};
uint32_t code_length = 100;

extern "C" void jsUI_AlipayRequestBindingString(void);
extern "C" void jsUI_AlipayRequestBindingstate(void);
extern "C" void jsUI_AlipayRequestBindingpaycode(void);
extern "C" void jsUI_AlipayRequestBindinglogonid(void);
extern "C" void jsUI_AlipayRequestBindingnicname(void);

JSIValue ALiPayModule::m_on_callback = NULL;
JSIValue ALiPayModule::m_on_this = NULL;

alipay_event_map_t ALiPayModule::m_event_map[ALiPAY_EVENT_MAP_NUM] = {
    {"bindResponse",    0, 0}
};

int ALiPayModule::GetEventIdx(const JSIValue target)
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


void InitALiPayModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "bindResponse", ALiPayModule::BindResponse);
    JSI::SetModuleAPI(exports, "readBindCode", ALiPayModule::ReadBindCode);
    JSI::SetModuleAPI(exports, "readBindStatus", ALiPayModule::ReadBindStatus);
    JSI::SetModuleAPI(exports, "readPayCode", ALiPayModule::ReadPayCode);
    JSI::SetModuleAPI(exports, "readUserInfo", ALiPayModule::ReadUserInfo);
    JSI::SetModuleAPI(exports, "readNickname", ALiPayModule::ReadNickname);
}

static void alipay_response(uint8_t resType,void* data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "alipay_response failed with null input parameter");
        return;
    }
    auto number = static_cast<const char *>(data);
    JSIValue _callback = NULL;
    JSIValue _this     = NULL;
    JSIValue key = JSI::CreateString("bindResponse");
    ALiPayModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);
    if(_callback == NULL || _this == NULL)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s called alipay response failure", __func__);
        return;
    }
    JSIValue value = JSI::CreateString(number);
    JSIValue type = JSI::CreateNumber(resType);
    JSIValue number_argv[ARGC_TWO] = {
        type,
        value
    };
    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, number_argv, ARGC_TWO);
    JSI::ReleaseValue(value);
}

static void alipay_notify_bind_code(void*number){
    alipay_response(0,number);
}

static void alipay_notify_bind_status(void*number){
    alipay_response(1,number);
}

static void alipay_notify_pay_code(void*number){
    alipay_response(2,number);
}

static void alipay_notify_login_id(void*number){
    alipay_response(3,number);
}

static void alipay_notify_nickname(void*number){
    alipay_response(4,number);
}
extern "C" void alipay_notify_bounding_number_ui(uint8_t resType,void* number)
{
    uint16_t res;
    switch(resType){
        case 0:
            //AsyncTaskManager::GetInstance().AddDispatch(alipay_notify_bind_code,number);
            res = AsyncTaskManager::GetInstance().Dispatch(alipay_notify_bind_code,number);
            break;
        case 1:
            //AsyncTaskManager::GetInstance().AddDispatch(alipay_notify_bind_status,number);
            res = AsyncTaskManager::GetInstance().Dispatch(alipay_notify_bind_status,number);
            break;
        case 2:
            //AsyncTaskManager::GetInstance().AddDispatch(alipay_notify_pay_code,number);
            res = AsyncTaskManager::GetInstance().Dispatch(alipay_notify_pay_code,number);
            break;
        case 3:
            //AsyncTaskManager::GetInstance().AddDispatch(alipay_notify_login_id,number);
            res = AsyncTaskManager::GetInstance().Dispatch(alipay_notify_login_id,number);
            break;
        case 4:
            //AsyncTaskManager::GetInstance().AddDispatch(alipay_notify_nickname,number);
            res = AsyncTaskManager::GetInstance().Dispatch(alipay_notify_nickname,number);
            break;
        default:
            res = DISPATCH_FAILURE;
            break;
    }

    if(res == DISPATCH_FAILURE){
         HILOG_ERROR(HILOG_MODULE_ACE, "ALiPayModule alipay_notify_bounding_number_ui dispatch failed");
    }
}

JSIValue ALiPayModule::ReadBindCode(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    jsUI_AlipayRequestBindingString();
    return JSI::CreateUndefined();
}


JSIValue ALiPayModule::BindResponse(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    SetEventMap(args[0], args[1], thisVal);
    return JSI::CreateUndefined();
}

JSIValue ALiPayModule::ReadBindStatus(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    jsUI_AlipayRequestBindingstate();
    return JSI::CreateUndefined();
}

JSIValue ALiPayModule::ReadPayCode(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    jsUI_AlipayRequestBindingpaycode();
    return JSI::CreateUndefined();
}

JSIValue ALiPayModule::ReadUserInfo(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    jsUI_AlipayRequestBindinglogonid();
    return JSI::CreateUndefined();
}

JSIValue ALiPayModule::ReadNickname(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum)
{
    jsUI_AlipayRequestBindingnicname();
    return JSI::CreateUndefined();
}

int ALiPayModule::SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this)
{
    int _idx = GetEventIdx(_owner);

    return SetEventMap(_idx, _callback, _this);
}

int ALiPayModule::SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this)
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

int ALiPayModule::GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this)
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

int ALiPayModule::GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this)
{
    int _idx = GetEventIdx(_owner);

    return GetEventMap(_idx, _callback, _this);
}

} // namespace ACELite
} // namespace OHOS


#endif