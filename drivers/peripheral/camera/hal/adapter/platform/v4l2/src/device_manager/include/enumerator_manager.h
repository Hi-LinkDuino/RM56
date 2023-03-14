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

#ifndef HOS_CAMERA_ENUMERATOR_MANAGER_H
#define HOS_CAMERA_ENUMERATOR_MANAGER_H

#include "imanager.h"
#include "device_manager_adapter.h"
#include "v4l2_uvc.h"
#include "v4l2_common.h"


namespace OHOS::Camera {
using UvcDataCb = std::function<void(const std::string, std::vector<DeviceControl>&, std::vector<DeviceFormat>&, bool)>;
class EnumeratorManager {
public:
    EnumeratorManager();
    virtual ~EnumeratorManager();

    RetCode Init();
    void SetCallBack(UvcDataCb cb);
    void UvcCallBack(const std::string hardwareName, std::vector<DeviceControl>& deviceControl,
        std::vector<DeviceFormat>& deviceFormat, bool uvcState);

private:
    UvcDataCb uvcCb_;
    std::shared_ptr<HosV4L2UVC> uvcVideo_;
};
} // namespace OHOS::Camera
#endif