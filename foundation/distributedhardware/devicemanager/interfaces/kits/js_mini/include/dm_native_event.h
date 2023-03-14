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
#ifndef OHOS_DEVICE_MANAGER_NATIVE_EVENT_H
#define OHOS_DEVICE_MANAGER_NATIVE_EVENT_H

#include <map>
#include <memory>
#include <string>
#include "jsi.h"
#include "dm_device_info.h"

namespace OHOS {
namespace ACELite {
struct DmEventListener {
    std::string eventType;
    JSIValue handlerRef = JSI::CreateUndefined();
    JSIValue thisVarRef_ = JSI::CreateUndefined();
};

struct FuncParams {
    JSIValue handlerRef = JSI::CreateUndefined();
    JSIValue thisVarRef_ = JSI::CreateUndefined();
    const JSIValue *args = nullptr;
    uint8_t argsSize = 0;
};

class DmNativeEvent {
public:
    DmNativeEvent();
    DmNativeEvent(JSIValue thisVar);
    virtual ~DmNativeEvent();
    virtual void On(std::string &eventType, JSIValue handler, JSIValue thisVal);
    virtual void Off(std::string &eventType);
    virtual void OnEvent(const std::string &eventType, uint8_t argsSize, const JSIValue *data);
    static void OnEventAsyncWorkFunc(void *data);
protected:
    static std::map<std::string, std::shared_ptr<DmEventListener>> eventMap_;
};
}
}

#endif /* OHOS_DEVICE_MANAGER_NATIVE_EVENT_H */