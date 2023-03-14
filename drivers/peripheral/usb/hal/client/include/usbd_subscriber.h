/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef USBD_SUBSCRIBER_H
#define USBD_SUBSCRIBER_H

#include "ipc_object_stub.h"
#include "usb_info.h"
#include "usbd_type.h"

namespace OHOS {
namespace USB {
class UsbdSubscriber : public IPCObjectStub {
public:
    explicit UsbdSubscriber() : IPCObjectStub(u"ohos.usb.IUsbManagerSubscriber") {};
    virtual ~UsbdSubscriber() = default;
    virtual int32_t DeviceEvent(const UsbInfo &info) = 0;
    virtual int32_t PortChangedEvent(int32_t portId, int32_t powerRole, int32_t dataRole, int32_t mode) = 0;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    static int32_t ParserUsbInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option, UsbInfo &info);
    static int32_t ParserPortInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option, PortInfo &info);
    static inline const std::u16string metaDescriptor_ = u"hdf.usb.usbdsubscriber";
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }
};
} // namespace USB
} // namespace OHOS
#endif // USBD_SUBSCRIBER_H
