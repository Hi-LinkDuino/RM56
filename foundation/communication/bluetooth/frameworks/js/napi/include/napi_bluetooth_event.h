/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef NAPI_BLUETOOTH_EVENT_H_
#define NAPI_BLUETOOTH_EVENT_H_

#include <string>
#include <map>
#include <napi_bluetooth_utils.h>

namespace OHOS {
namespace Bluetooth {

class AsyncEventData : public BluetoothCallbackInfo {
public:
    std::function<napi_value ()> packResult;

    explicit AsyncEventData(const std::shared_ptr<BluetoothCallbackInfo> &cb, std::function<napi_value ()> p)
    {
        this->env_ = cb->env_;
        this->callback_ = cb->callback_;
        this->state_ = cb->state_;
        this->deviceId_ = cb->deviceId_;
        this->info_ = cb->info_;
        packResult = p;
    }

    AsyncEventData() = delete;

    virtual ~AsyncEventData() {
    }
};

class NapiEvent {
public:
    static napi_value CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb, int value);
    static napi_value CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb,
        BluetoothOppTransferInformation &information);
    static napi_value CreateResult(const std::shared_ptr<BluetoothCallbackInfo> &cb,
        GattCharacteristic &characteristic);
    static napi_value OnEvent(napi_env env, napi_callback_info cbinfo,
        std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> &callbackInfos);
    static napi_value OffEvent(napi_env env, napi_callback_info cbinfo,
        std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> &callbackInfos);
    static void EventNotify(AsyncEventData *asyncEvent);

    template<typename T>
    static void CheckAndNotify(const std::shared_ptr<BluetoothCallbackInfo> &cb, const T& obj)
    {
        if (cb == nullptr) {
            HILOGI("checkAndNotify event cb is nullptr!");
            return;
        }
        auto func = std::bind(
            [](const std::shared_ptr<BluetoothCallbackInfo> &cb, T& obj) -> napi_value {
			    return NapiEvent::CreateResult(cb, obj);
			}, cb, obj);
        AsyncEventData *asyncEvent = new (std::nothrow)AsyncEventData(cb, func);
        if (asyncEvent == nullptr) {
            HILOGI("asyncEvent is nullptr!");
            return;
        }
        EventNotify(asyncEvent);
    }
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_EVENT_H_ */