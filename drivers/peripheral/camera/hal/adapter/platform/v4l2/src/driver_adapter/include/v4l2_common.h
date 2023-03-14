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

#ifndef HOS_CAMERA_V4L2_COMMON_H
#define HOS_CAMERA_V4L2_COMMON_H

#include <string>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#if defined(V4L2_UTEST) || defined (V4L2_MAIN_TEST)
#include "v4l2_temp.h"
#else
#include <stream.h>
#endif

namespace OHOS::Camera {
#define MAXSTREAMCOUNT         4
#define MAXVIDEODEVICE         24
#define DEVICENAMEX            "/dev/video"
#define MAXUVCNODE             10

struct V4l2Fract {
    int32_t   numerator;
    int32_t   denominator;
};

struct V4l2Rect {
    int32_t left;
    int32_t top;
    uint32_t width;
    uint32_t height;
};

struct V4l2CropCap {
    struct V4l2Rect bounds;
    struct V4l2Rect defrect;
    struct V4l2Fract pixelaspect;
};

struct V4l2FmtDesc {
    std::string description;
    uint32_t pixelformat;
    uint32_t width;
    uint32_t height;
    uint32_t sizeimage;
    struct V4l2Fract fps;
};

using DeviceFormat = struct _V4l2DeviceFormat {
    uint32_t portId;
    struct V4l2CropCap cropcap;
    struct V4l2Rect crop;
    struct V4l2FmtDesc fmtdesc;
};

struct V4l2Menu {
    uint32_t    id;
    uint32_t    index;
    int64_t    value;
    std::string    name;
};

using DeviceControl = struct _V4l2Control {
    uint32_t          id;
    uint32_t          ctrl_class;
    uint32_t          type;
    uint32_t          flags;
    int32_t           minimum;
    int32_t           maximum;
    int32_t           step;
    int32_t           default_value;
    int32_t           value;
    std::string       name;
    std::vector<V4l2Menu> menu;
};

enum V4l2FmtCmd : uint32_t {
    CMD_V4L2_GET_FORMAT,
    CMD_V4L2_SET_FORMAT,
    CMD_V4L2_GET_CROPCAP,
    CMD_V4L2_SET_CROP,
    CMD_V4L2_GET_CROP,
    CMD_V4L2_SET_FPS,
    CMD_V4L2_GET_FPS,
};

namespace {
    static constexpr uint32_t RCERRORFD = -1;
}

using BufCallback = std::function<void(std::shared_ptr<FrameSpec>)>;
using UvcCallback = std::function<void(const std::string, std::vector<DeviceControl>&,
    std::vector<DeviceFormat>&, bool)>;
}
#endif // HOS_CAMERA_V4L2_COMMON_H
