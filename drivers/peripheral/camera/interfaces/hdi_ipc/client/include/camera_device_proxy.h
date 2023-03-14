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

#ifndef HDI_CAMERA_DEVICE_CLIENT_PROXY_H
#define HDI_CAMERA_DEVICE_CLIENT_PROXY_H

#include <iremote_proxy.h>
#include "icamera_device.h"
#include "istream_operator_callback.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class CameraDeviceProxy : public IRemoteProxy<ICameraDevice> {
public:
    explicit CameraDeviceProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ICameraDevice>(impl) {}
    virtual ~CameraDeviceProxy() = default;

    virtual CamRetCode GetStreamOperator(
        const OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IStreamOperator> &streamOperator) override;
    virtual CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting> &settings) override;
    virtual CamRetCode SetResultMode(const ResultCallbackMode &mode) override;
    virtual CamRetCode GetEnabledResults(std::vector<MetaType> &results) override;
    virtual CamRetCode EnableResult(const std::vector<MetaType> &results) override;
    virtual CamRetCode DisableResult(const std::vector<MetaType> &results) override;
    virtual void Close() override;

private:
    static inline BrokerDelegator<CameraDeviceProxy> delegator_;
};
}
#endif // HDI_CAMERA_DEVICE_CLIENT_PROXY_H