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

#ifndef HDI_OFFLINE_STREAM_OPERATOR_CLIENT_PROXY_H
#define HDI_OFFLINE_STREAM_OPERATOR_CLIENT_PROXY_H

#include <iremote_proxy.h>
#include "ioffline_stream_operator.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class OfflineStreamOperatorProxy : public IRemoteProxy<IOfflineStreamOperator> {
public:
    explicit OfflineStreamOperatorProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IOfflineStreamOperator>(impl) {}
    virtual ~OfflineStreamOperatorProxy() {}

    virtual CamRetCode CancelCapture(int captureId) override;
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    virtual CamRetCode Release() override;

private:
    static inline BrokerDelegator<OfflineStreamOperatorProxy> delegator_;
};
}
#endif // HDI_OFFLINE_STREAM_OPERATOR_CLIENT_PROXY_H