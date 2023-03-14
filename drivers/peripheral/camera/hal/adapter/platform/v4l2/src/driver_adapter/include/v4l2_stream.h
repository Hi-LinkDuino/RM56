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

#ifndef HOS_CAMERA_V4L2_STREAM_H
#define HOS_CAMERA_V4L2_STREAM_H

#include "v4l2_common.h"
#if defined(V4L2_UTEST) || defined (V4L2_MAIN_TEST)
#include "v4l2_temp.h"
#else
#include <camera.h>
#endif

namespace OHOS::Camera {
class HosV4L2Streams {
public:
    HosV4L2Streams(enum v4l2_buf_type bufferType);
    ~HosV4L2Streams();

    RetCode V4L2StreamOn(int fd);
    RetCode V4L2StreamOff(int fd);
    RetCode V4L2StreamFPSGet(int fd, DeviceFormat& format);
    RetCode V4L2StreamFPSSet(int fd, DeviceFormat& format);

private:
    enum v4l2_buf_type bufferType_ = V4L2_BUF_TYPE_PRIVATE;
};
} // namespace OHOS::Camera
#endif // HOS_CAMERA_V4L2_STREAM_H
