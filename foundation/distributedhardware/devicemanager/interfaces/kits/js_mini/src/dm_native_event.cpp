/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dm_native_event.h"
#include "jsi.h"
#include "dm_log.h"
#include "js_async_work.h"

using namespace OHOS::DistributedHardware;
using namespace std;

namespace OHOS {
namespace ACELite {
std::map<std::string, std::shared_ptr<DmEventListener>> DmNativeEvent::eventMap_;

DmNativeEvent::DmNativeEvent()
{
    LOGI("DmNativeEvent::DmNativeEvent() in");
}

DmNativeEvent::DmNativeEvent(JSIValue thisVar)
{
    LOGI("DmNativeEvent::DmNativeEvent(JSIValue thisVar) in");
}

DmNativeEvent::~DmNativeEvent()
{
    LOGI("DmNativeEvent::~DmNativeEvent() in");
}

void DmNativeEvent::On(std::string &eventType, JSIValue handle, JSIValue thisVal)
{
    LOGI("DmNativeEvent On in for event: %s", eventType.c_str());
    std::shared_ptr<DmEventListener> listener= std::make_shared<DmEventListener>();
    
    listener->eventType = eventType;
    listener->handlerRef = JSI::AcquireValue(handle);
    
    listener->thisVarRef_ = JSI::AcquireValue(thisVal);
    eventMap_[eventType] = listener;
}

void DmNativeEvent::Off(std::string &eventType)
{
    LOGI("DmNativeEvent Off in for event: %s", eventType.c_str());
    auto iter = eventMap_.find(eventType);
    if (iter == eventMap_.end()) {
        LOGE("eventType %s not find", eventType.c_str());
        return;
    }
    std::shared_ptr<DmEventListener> listener = iter->second;
    JSI::ReleaseValue(listener->handlerRef);
     
    JSI::ReleaseValue(listener->thisVarRef_);
    eventMap_.erase(eventType);
}

void DmNativeEvent::OnEvent(const std::string &eventType, uint8_t argsSize, const JSIValue *data)
{
    LOGI("OnEvent for %s", eventType.c_str());

    auto iter = eventMap_.find(eventType);
    if (iter == eventMap_.end()) {
        LOGE("eventType %s not find", eventType.c_str());
        return;
    }
    auto listener = iter->second;
    if (!JSI::ValueIsFunction(listener->handlerRef)) {
        LOGI("OnEvent for %s handlerRef is null", eventType.c_str());
        return;
    }
    
    FuncParams* params = new FuncParams();
    params->handlerRef = listener->handlerRef;
    params->thisVarRef_ = listener->thisVarRef_;
    params->args = data;
    params->argsSize = argsSize;
    
    LOGI("OnEventAsyncWorkFunc for %s in", eventType.c_str());
    JsAsyncWork::DispatchAsyncWork(OnEventAsyncWorkFunc, reinterpret_cast<void *>(params));
}

void DmNativeEvent::OnEventAsyncWorkFunc(void *data)
{
    LOGI("OnEventAsyncWorkFunc in ");
    FuncParams* params = reinterpret_cast<FuncParams *>(data);
    JSI::CallFunction(params->handlerRef, params->thisVarRef_, params->args, params->argsSize);
}
}
}