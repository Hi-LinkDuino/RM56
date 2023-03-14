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

#ifndef STREAM_OPERATOR_CALLBACK_CLIENT_STUB_H
#define STREAM_OPERATOR_CALLBACK_CLIENT_STUB_H

#include "istream_operator_callback.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "parcel.h"

namespace OHOS::Camera {
class StreamOperatorCallbackStub : public IRemoteStub<IStreamOperatorCallback> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
    int32_t OnCaptureStartedStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t OnCaptureEndedStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t OnCaptureErrorStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t OnFrameShutterStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
}
#endif // STREAM_OPERATOR_CALLBACK_CLIENT_STUB_H