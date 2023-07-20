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

#ifndef OHOS_ACELITE_ROUTER_MODULE_H
#define OHOS_ACELITE_ROUTER_MODULE_H

#include "jsi.h"
#include "non_copyable.h"


#define ALiPAY_EVENT_MAP_NUM 3

namespace OHOS {
namespace ACELite {

typedef struct _alipay_callback
{
    const char* m_event_owner;
    JSIValue m_callback;
    JSIValue m_this;
}alipay_event_map_t;

typedef struct _alipay_data{
    void *data;
    uint8_t type;
}alipay_response_task;

/**
 * @brief: The module of alipay
 */
class ALiPayModule final : public MemoryHeap {
private:
    bool isCallback = false;
public:
    ACE_DISALLOW_COPY_AND_MOVE(ALiPayModule);
    ALiPayModule() ;
    ~ALiPayModule();
    static JSIValue BindResponse(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);

    static JSIValue ReadBindCode(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue ReadBindStatus(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue ReadPayCode(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue ReadUserInfo(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
    static JSIValue ReadNickname(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);

    static JSIValue m_on_callback;
    static JSIValue m_on_this;


    static int GetEventIdx(const JSIValue);
    static alipay_event_map_t m_event_map[ALiPAY_EVENT_MAP_NUM];
    static int SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this);
    static int SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this);
    static int GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this);
    static int GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this);
    void InitALiPayModule(JSIValue exports);
};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_ROUTER_MODULE_H

#endif