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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SINK_STUB_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SINK_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_bluetooth_a2dp_sink.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpSinkStub : public IRemoteStub<IBluetoothA2dpSink> {
public:
    BluetoothA2dpSinkStub();
    virtual ~BluetoothA2dpSinkStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetPlayingStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetConnectStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SendDelayInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothA2dpSinkServerFunc = ErrCode (BluetoothA2dpSinkStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothA2dpSinkServerFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothA2dpSinkStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SINK_STUB_H