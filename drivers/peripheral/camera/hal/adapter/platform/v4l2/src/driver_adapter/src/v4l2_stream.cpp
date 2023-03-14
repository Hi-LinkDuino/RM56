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

#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include "v4l2_stream.h"

namespace OHOS::Camera {
HosV4L2Streams::HosV4L2Streams(enum v4l2_buf_type bufferType)
    : bufferType_(bufferType)
{
}

HosV4L2Streams::~HosV4L2Streams() {}

RetCode HosV4L2Streams::V4L2StreamOn(int fd)
{
    enum v4l2_buf_type buf_type;
    int rc;
    CAMERA_LOGD("HosV4L2Streams::V4L2StreamOn\n");

    buf_type = bufferType_;
    rc = ioctl(fd, VIDIOC_STREAMON, &buf_type);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_STREAMON failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosV4L2Streams::V4L2StreamOff(int fd)
{
    enum v4l2_buf_type buf_type;
    int rc;
    CAMERA_LOGD("HosV4L2Streams::V4L2StreamOff\n");

    buf_type = bufferType_;
    rc = ioctl(fd, VIDIOC_STREAMOFF, &buf_type);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_STREAMOFF failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosV4L2Streams::V4L2StreamFPSGet(int fd, DeviceFormat& format)
{
    struct v4l2_streamparm Stream_Parm = {};
    int rc;

    Stream_Parm.type = bufferType_;

    rc = ioctl(fd, VIDIOC_G_PARM, &Stream_Parm);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_G_PARM failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    format.fmtdesc.fps.numerator = Stream_Parm.parm.capture.timeperframe.numerator;
    format.fmtdesc.fps.denominator = Stream_Parm.parm.capture.timeperframe.denominator;

    return RC_OK;
}

RetCode HosV4L2Streams::V4L2StreamFPSSet(int fd, DeviceFormat& format)
{
    struct v4l2_streamparm Stream_Parm = {};
    int rc;

    Stream_Parm.type = bufferType_;

    Stream_Parm.parm.capture.timeperframe.denominator = format.fmtdesc.fps.denominator;
    Stream_Parm.parm.capture.timeperframe.numerator = format.fmtdesc.fps.numerator;

    rc = ioctl(fd, VIDIOC_S_PARM, &Stream_Parm);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_S_PARM failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    return RC_OK;
}
} // namespace OHOS::Camera
