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

#ifndef HOS_CAMERA_V4L2_BUFFER_H
#define HOS_CAMERA_V4L2_BUFFER_H

#include <mutex>
#include <map>
#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include "v4l2_common.h"
#if defined(V4L2_UTEST) || defined (V4L2_MAIN_TEST)
#include "v4l2_temp.h"
#else
#include <stream.h>
#include <camera.h>
#endif

namespace OHOS::Camera {
class HosV4L2Buffers {
public:
    HosV4L2Buffers(enum v4l2_memory memType, enum v4l2_buf_type bufferType);
    ~HosV4L2Buffers();

    RetCode V4L2ReqBuffers(int fd, int unsigned buffCont);
    RetCode V4L2ReleaseBuffers(int fd);

    RetCode V4L2QueueBuffer(int fd, const std::shared_ptr<FrameSpec>& frameSpec);
    RetCode V4L2DequeueBuffer(int fd);

    RetCode V4L2AllocBuffer(int fd, const std::shared_ptr<FrameSpec>& frameSpec);

    void SetCallback(BufCallback cb);

    RetCode Flush(int fd);

private:
    BufCallback dequeueBuffer_;

    using FrameMap = std::map<unsigned int, std::shared_ptr<FrameSpec>>;
    std::map<int, FrameMap> queueBuffers_;

    std::mutex bufferLock_;

    enum v4l2_memory memoryType_;
    enum v4l2_buf_type bufferType_;
};
} // namespace OHOS::Camera
#endif // HOS_CAMERA_V4L2_BUFFER_H
