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

#ifndef OHOS_ACELITE_LTE_MODULE_H
#define OHOS_ACELITE_LTE_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {

typedef struct LteCallback
{
    const char* m_event_owner;
    JSIValue m_callback;
    JSIValue m_this;
}LteEventMap;

#define LTE_EVENT_MAP_NUM 2

class LteModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LteModule);
    LteModule() = default;
    ~LteModule() = default;
    static JSIValue Initreg(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue Callkey(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue Answer(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue Hungup(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue Listen(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);

    static int GetEventIdx(const JSIValue);
    static int SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this);
    static int SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this);

    static int GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this);
    static int GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this);
    static LteEventMap EventMap[LTE_EVENT_MAP_NUM];

private:
};

void InitLteModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "initreg", LteModule::Initreg);
    JSI::SetModuleAPI(exports, "callkey", LteModule::Callkey);
    JSI::SetModuleAPI(exports, "answer", LteModule::Answer);
    JSI::SetModuleAPI(exports, "hungup", LteModule::Hungup);
    JSI::SetModuleAPI(exports, "listen", LteModule::Listen);
}
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_LTE_MODULE_H