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
#ifndef INPUT_SERVICE_STUB_H
#define INPUT_SERVICE_STUB_H

#include <functional>
#include <message_parcel.h>
#include <message_option.h>
#include <map>
#include <refbase.h>
#include <string>

#include "iinput_host.h"
#include "input_server.h"
#include "input_type.h"

namespace OHOS {
namespace Input {
class InputServerStub {
public:
    InputServerStub();
    virtual ~InputServerStub() {}
    int32_t InputServerStubOnRemoteRequest(int32_t cmdId,
                                           MessageParcel& data,
                                           MessageParcel& reply,
                                           MessageOption& option);
    int32_t InputServerStubScanInputDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubOpenInputDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubCloseInputDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetInputDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetInputDeviceList(MessageParcel& data,
                                              MessageParcel& reply,
                                              MessageOption& option) const;
    int32_t InputServerStubSetPowerStatus(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetPowerStatus(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetDeviceType(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetChipInfo(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetVendorName(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubGetChipName(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubSetGestureMode(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubRunCapacitanceTest(MessageParcel& data,
                                              MessageParcel& reply,
                                              MessageOption& option) const;
    int32_t InputServerStubRunExtraCommand(MessageParcel& data, MessageParcel& reply, MessageOption& option) const;
    int32_t InputServerStubRegisterReportCallback(MessageParcel &data,
                                                  MessageParcel &reply,
                                                  MessageOption &option) const;
    int32_t InputServerStubUnregisterReportCallback(MessageParcel &data,
                                                    MessageParcel &reply,
                                                    MessageOption &option) const;
    int32_t InputServerStubRegisterHotPlugCallback(MessageParcel &data,
                                                   MessageParcel &reply,
                                                   MessageOption &option) const;
    int32_t InputServerStubUnregisterHotPlugCallback(MessageParcel &data,
                                                     MessageParcel &reply,
                                                     MessageOption &option) const;
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }
private:
    static inline const std::u16string metaDescriptor_ = u"HDI.INPUT.V1_0";
    std::shared_ptr<InputServer> mService_ = std::make_shared<InputServer>();
};
}  // namespace Input
}  // namespace OHOS

void *InputStubInstance();
void InputStubRelease(void *obj);
int32_t InputServiceOnRemoteRequest(void *stub, int32_t cmdId, struct HdfSBuf& data, struct HdfSBuf& reply);
#endif // INPUT_SERVICE_STUB_H
