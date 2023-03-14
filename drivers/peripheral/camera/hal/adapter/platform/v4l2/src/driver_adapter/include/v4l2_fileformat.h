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

#ifndef HOS_CAMERA_V4L2_FILEFORMAT_H
#define HOS_CAMERA_V4L2_FILEFORMAT_H

#include <vector>
#include <cstring>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "v4l2_common.h"
#if defined(V4L2_UTEST) || defined (V4L2_MAIN_TEST)
#include "v4l2_temp.h"
#else
#include <camera.h>
#endif

namespace OHOS::Camera {
class HosFileFormat {
public:
    HosFileFormat();
    ~HosFileFormat();
    RetCode V4L2GetFmt(int fd, DeviceFormat& format);
    RetCode V4L2SetFmt(int fd, DeviceFormat& format);
    RetCode V4L2GetCrop(int fd, DeviceFormat& format);
    RetCode V4L2SetCrop(int fd, DeviceFormat& format);
    RetCode V4L2GetCropCap(int fd, DeviceFormat& format);
    RetCode V4L2GetFmtDescs(int fd, std::vector<DeviceFormat>& fmtDesc);
    int V4L2OpenDevice(const std::string& deviceName);
    void V4L2CloseDevice(int fd);
    void V4L2MatchDevice(std::vector<std::string>& cameraIDs);
    int V4L2SearchBufType(int fd);

private:
    RetCode V4L2GetCapability(int fd, const std::string& dev_name, std::string& cameraId);
    RetCode V4L2SearchFormat(int fd, std::vector<DeviceFormat>& fmtDesc);
    enum v4l2_buf_type bufType_ = V4L2_BUF_TYPE_PRIVATE;
};
} // namespace OHOS::Camera
#endif // HOS_CAMERA_V4L2_FILEFORMAT_H
