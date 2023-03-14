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

#ifndef OHOS_HARDWARE_NFC_V1_0_CONNECTED_TAG_SERVER_STUB_H
#define OHOS_HARDWARE_NFC_V1_0_CONNECTED_TAG_SERVER_STUB_H
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>
#include "connected_tag_service.h"
#include "iconnected_tag_hdi.h"
namespace OHOS {
namespace HDI {
namespace NFC {
namespace V1_0 {
using namespace OHOS;
class ConnectedTagServerStub : public IRemoteStub<IConnectedTagHdi> {
public:
    virtual ~ConnectedTagServerStub() {}

    int32_t StubOnRemoteRequest(int cmdId, MessageParcel& data, MessageParcel& reply,
        MessageOption& option);

    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override;

    int32_t Init() override;

    int32_t Uninit() override;

    std::string ReadNdefTag() override;

    int32_t WriteNdefTag(std::string ndefData) override;
private:
    int32_t StubInit(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t StubUninit(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t StubReadNdef(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t StubWriteNdef(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    ConnectedTagService service_;
};
}  // namespace V1_0
}  // namespace NFC
}  // namespace HDI
}  // namespace OHOS

void *ConnectedTagServerStubInstance();

void ConnectedTagServerStubRelease(void *obj);

int32_t DispatchStubOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf* data, struct HdfSBuf* reply);

#endif  // OHOS_HARDWARE_NFC_V1_0_CONNECTED_TAG_SERVER_STUB_H
