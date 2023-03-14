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

#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_map_mce_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMceObserverStub : public IRemoteStub<IBluetoothMapMceObserver> {
public:
    BluetoothMapMceObserverStub();
    virtual ~BluetoothMapMceObserverStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode OnMapActionCompletedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnMapEventReportedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnBmessageCompletedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnMessagesListingCompletedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnConversationListingCompletedInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHostFunc = ErrCode (BluetoothMapMceObserverStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHostFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothMapMceObserverStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_STUB_H
