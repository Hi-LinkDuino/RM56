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

#ifndef OHOS_ACELITE_BT_MODULE_H
#define OHOS_ACELITE_BT_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "non_copyable.h"


namespace OHOS {
namespace ACELite {

typedef struct _ota_callback
{
    const char* m_event_owner;
    JSIValue m_callback;
    JSIValue m_this;
}ota_event_map_t;


#define OTA_EVENT_MAP_NUM    1

/**
 * @brief: The module of alipay
 */
class OtaModule final : public MemoryHeap {
private:
    bool isCallback = false;
public:
    ACE_DISALLOW_COPY_AND_MOVE(OtaModule);
    OtaModule();
    ~OtaModule();
    static JSIValue BindResponse(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);

    static JSIValue m_on_callback;
    static JSIValue m_on_this;

    static int GetEventIdx(const JSIValue);
    static ota_event_map_t m_event_map[OTA_EVENT_MAP_NUM];
    static int SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this);
    static int SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this);
    static int GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this);
    static int GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this);
    void InitOtaModule(JSIValue exports);
};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_BT_MODULE_H
