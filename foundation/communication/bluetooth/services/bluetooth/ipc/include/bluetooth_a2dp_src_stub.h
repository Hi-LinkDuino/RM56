/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SRC_STUB_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SRC_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_bluetooth_a2dp_src.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpSrcStub : public IRemoteStub<IBluetoothA2dpSrc> {
public:
    BluetoothA2dpSrcStub();
    virtual ~BluetoothA2dpSrcStub();

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
    ErrCode SetActiveSinkDeviceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetActiveSinkDeviceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetCodecStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetCodecPreferenceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SwitchOptionalCodecsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetOptionalCodecsSupportStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StartPlayingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SuspendPlayingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StopPlayingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode WriteFrameInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetRenderPositionInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothA2dpSrcServerFunc = ErrCode (BluetoothA2dpSrcStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothA2dpSrcServerFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothA2dpSrcStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SRC_STUB_H