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

#include "lte_module.h"
#include "ace_log.h"
#include "js_app_context.h"

using namespace OHOS::ACELite;

#include "async_task_manager.h"

static JSIValue g_thisVal = NULL;
static JSIValue g_callback = NULL;

extern "C" void jsUI_VolteInitReg();
extern "C" void jsUI_VolteCallkey(char *phone_num);
extern "C" void jsUI_VolteAnswer();
extern "C" void jsUI_VolteTerm();

namespace OHOS {
namespace ACELite {

JSIValue LteModule::Listen(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    char *type = JSI::ValueToString(args[0]);

    SetEventMap(args[0], args[1], thisVal);

    JSIValue callback = args[1];
    JSIValue argv[ARGC_ONE] = {JSI::CreateNumber(argsNum)};
    // JSI::CallFunction(callback, thisVal, argv, ARGC_ONE);
    HILOG_ERROR(HILOG_MODULE_ACE,"%s %d %s register callback=%#x this %p", __func__,__LINE__,type,callback, thisVal);
    return JSI::CreateUndefined();
}

LteEventMap LteModule::EventMap[LTE_EVENT_MAP_NUM] = {
    {"dial",     0, 0},
    {"hungup", 0, 0},
};

int LteModule::GetEventIdx(const JSIValue target)
{
    int idx = -1;
    int total = sizeof(EventMap)/sizeof(EventMap[0]);
    char* str_target = NULL;
    for(int ii=0; ii<total; ii++)
    {
        str_target = JSI::ValueToString(target);
        if(memcmp(str_target,EventMap[ii].m_event_owner, strlen(str_target) ) == 0)
        {
            idx = ii;
            break;
        }
    }
    return idx;
}

int LteModule::SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this)
{
    int total = sizeof(EventMap)/sizeof(EventMap[0]);
    if(_idx >= total || _idx < 0)
    {
        return -1;
    }

    EventMap[_idx].m_callback = _callback;
    EventMap[_idx].m_this = _this;

    return 0;
}

int LteModule::SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this)
{
    int _idx = GetEventIdx(_owner);

    return SetEventMap(_idx, _callback, _this);
}

int LteModule::GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this)
{
    int total = sizeof(EventMap)/sizeof(EventMap[0]);
    if(_idx >= total || _idx < 0)
    {
        return -1;
    }

    *_callback = EventMap[_idx].m_callback;
    *_this     = EventMap[_idx].m_this;

    return 0;
}

int LteModule::GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this)
{
    int _idx = GetEventIdx(_owner);

    return GetEventMap(_idx, _callback, _this);
}

static void LteNotifyAsync(void* data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "lte notify async failed with null input parameter");
        return;
    }

    auto event = static_cast<const char *>(data);

    HILOG_INFO(HILOG_MODULE_ACE,"%s notify event %s", __func__, event);

    JSIValue _callback = nullptr;
    JSIValue _this     = nullptr;

    JSIValue key = JSI::CreateString(event);
    LteModule::GetEventMap(key, &_callback, &_this);
    JSI::ReleaseValue(key);

    if(_callback == nullptr || _this == nullptr)
    {
        HILOG_ERROR(HILOG_MODULE_ACE,"%s notify event %s failed", __func__, event);
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE,"%s called callback=%#x this %p", __func__, _callback,_this);
    JSI::CallFunction(_callback, _this, nullptr, 0);
}

extern "C" void jsUI_LteNotifyIncomingCallUi(int flag)
{
    // If the jsUI_LteNotifyIncomingCallUi(1) function is called using at instructions
    // or other methods in the initialization interface,
    // it will jump to the UI interface for answering calls
    void *data = nullptr;
    if (flag) {
        data = (void*) ("dial");
    } else {
        data = (void*) ("hungup");
    }

    // dispatch to ui main
    uint16_t res = AsyncTaskManager::GetInstance().Dispatch(LteNotifyAsync, data);

    if(res == DISPATCH_FAILURE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "lte_modulb LteNotifyAsync dispatch failed");
    }
}

JSIValue LteModule::Initreg(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    HILOG_ERROR(HILOG_MODULE_ACE, "Initreg.");

    jsUI_VolteInitReg();

    return JSI::CreateUndefined();
}

JSIValue LteModule::Callkey(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    HILOG_ERROR(HILOG_MODULE_ACE, "Callkey.");
    char *phone_num = JSI::ValueToString(args[0]);

    jsUI_VolteCallkey(phone_num);

    return JSI::CreateUndefined();
}

JSIValue LteModule::Answer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    HILOG_ERROR(HILOG_MODULE_ACE, "Answer.");

    jsUI_VolteAnswer();

    return JSI::CreateUndefined();
}

JSIValue LteModule::Hungup(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    HILOG_ERROR(HILOG_MODULE_ACE, "Hungup.");

    jsUI_VolteTerm();

    return JSI::CreateUndefined();
}

} // namespace ACELite
} // namespace OHOS