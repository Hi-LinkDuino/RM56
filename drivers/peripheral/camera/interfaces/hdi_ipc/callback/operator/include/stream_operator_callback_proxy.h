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

#ifndef STREAM_OPERATOR_CALLBACK_CLIENT_PROXY_H
#define STREAM_OPERATOR_CALLBACK_CLIENT_PROXY_H

#include <iremote_proxy.h>
#include <hdf_log.h>
#include "istream_operator_callback.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class StreamOperatorCallbackProxy : public IRemoteProxy<IStreamOperatorCallback> {
public:
    explicit StreamOperatorCallbackProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IStreamOperatorCallback>(impl) {}

    virtual ~StreamOperatorCallbackProxy() = default;

    virtual void OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamId) override;
    virtual void OnCaptureEnded(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureEndedInfo>> &info) override;
    virtual void OnCaptureError(int32_t captureId,
        const std::vector<std::shared_ptr<CaptureErrorInfo>> &info) override;
    virtual void OnFrameShutter(int32_t captureId,
        const std::vector<int32_t> &streamId, uint64_t timestamp) override;

private:
    static inline BrokerDelegator<StreamOperatorCallbackProxy> delegator_;
};
}
#endif // STREAM_OPERATOR_CALLBACK_CLIENT_PROXY_H