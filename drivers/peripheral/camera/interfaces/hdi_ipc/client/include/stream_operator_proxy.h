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

#ifndef HDI_STREAM_OPERATOR_CLIENT_PROXY_H
#define HDI_STREAM_OPERATOR_CLIENT_PROXY_H

#include <iremote_proxy.h>
#include "istream_operator.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class StreamOperatorProxy : public IRemoteProxy<IStreamOperator> {
public:
    explicit StreamOperatorProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IStreamOperator>(impl) {}
    virtual ~StreamOperatorProxy() = default;

    virtual CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::vector<std::shared_ptr<StreamInfo>>& pInfo,
        StreamSupportType &pType) override;
    virtual CamRetCode CreateStreams(
        const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) override;
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    virtual CamRetCode CommitStreams(OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting) override;
    virtual CamRetCode GetStreamAttributes(
        std::vector<std::shared_ptr<StreamAttribute>> &attributes) override;
    virtual CamRetCode AttachBufferQueue(int streamId,
        const OHOS::sptr<OHOS::IBufferProducer> &producer) override;
    virtual CamRetCode DetachBufferQueue(int streamId) override;
    virtual CamRetCode Capture(int captureId,
        const std::shared_ptr<CaptureInfo> &pInfo,  bool isStreaming) override;
    virtual CamRetCode CancelCapture(int captureId) override;
    virtual CamRetCode ChangeToOfflineStream(const std::vector<int> &streamIds,
        OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IOfflineStreamOperator> &offlineOperator) override;

private:
    static inline BrokerDelegator<StreamOperatorProxy> delegator_;
};
}
#endif // HDI_STREAM_OPERATOR_CLIENT_PROXY_H