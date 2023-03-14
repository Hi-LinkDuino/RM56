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

#ifndef HOS_CAMERA_V4L2_CONTROL_H
#define HOS_CAMERA_V4L2_CONTROL_H

#include <linux/videodev2.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "v4l2_common.h"
#if defined(V4L2_UTEST) || defined (V4L2_MAIN_TEST)
#include "v4l2_temp.h"
#else
#include <camera.h>
#endif

namespace OHOS::Camera {
class HosV4L2Control {
public:
    HosV4L2Control();
    ~HosV4L2Control();

    RetCode V4L2GetCtrl(int fd, unsigned int id, int& value);
    RetCode V4L2SetCtrl(int fd, unsigned int id, int value);
    RetCode V4L2GetControls(int fd, std::vector<DeviceControl>& control);
    RetCode V4L2SetCtrls(int fd, std::vector<DeviceControl>& control, const int numControls);
    RetCode V4L2GetCtrls(int fd, std::vector<DeviceControl>& control, const int numControls);

private:
    void V4L2SetValue(int fd, std::vector<DeviceControl>& control, DeviceControl& ctrl,
        v4l2_queryctrl& qCtrl);
    int ExtControl(int fd, struct v4l2_queryctrl *ctrl);
    void V4L2EnumExtControls(int fd, std::vector<DeviceControl>& control);
    void V4L2EnumControls(int fd, std::vector<DeviceControl>& control);
    int V4L2GetControl(int fd, std::vector<DeviceControl>& control, unsigned int id);
};
} // namespace OHOS::Camera

#endif // HOS_CAMERA_V4L2_CONTROL_H
