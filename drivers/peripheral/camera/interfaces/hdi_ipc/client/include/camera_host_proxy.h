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

#ifndef HDI_CAMERA_HOST_CLIENT_PROXY_H
#define HDI_CAMERA_HOST_CLIENT_PROXY_H

#include <iremote_proxy.h>
#include "icamera_host.h"
#include "cmd_common.h"

namespace OHOS::Camera {
class ICameraHostCallback;
class CameraHostProxy : public IRemoteProxy<ICameraHost> {
public:
    explicit CameraHostProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ICameraHost>(impl) {}
    virtual ~CameraHostProxy() {}

    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) override;
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) override;
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability) override;
    virtual CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &pDevice) override;
    virtual CamRetCode SetFlashlight(const std::string &cameraId, bool &isEnable) override;

private:
    static inline BrokerDelegator<CameraHostProxy> delegator_;
};
}
#endif // HDI_CAMERA_HOST_CLIENT_PROXY_H