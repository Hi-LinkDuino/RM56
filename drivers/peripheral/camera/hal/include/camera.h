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

#ifndef HOS_CAMERA_H
#define HOS_CAMERA_H

#include "securec.h"
#include <cstdint>
#include <cstdio>
#include <functional>
#include <hdf_log.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef HITRACE_LOG_ENABLED
#include "hitrace.h"
#define DFX_LOCAL_HITRACE_BEGIN     \
    HiviewDFX::HiTraceId _trace_id; \
    _trace_id = OHOS::HiviewDFX::HiTrace::Begin(__FUNCTION__, HITRACE_FLAG_DEFAULT);
#define DFX_LOCAL_HITRACE_END OHOS::HiviewDFX::HiTrace::End(_trace_id);
#else
#define DFX_LOCAL_HITRACE_BEGIN
#define DFX_LOCAL_HITRACE_END
#endif // HITRACE_LOG_ENABLED

namespace OHOS {
namespace Camera {
#define HDF_LOG_TAG camera_host
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#ifndef OHOS_DEBUG
#define DECORATOR_HDFLOG(op, fmt, args...)             \
    do {                                               \
        op("%{public}s() " fmt, __FUNCTION__, ##args); \
    } while (0)
#else
#define DECORATOR_HDFLOG(op, fmt, args...)                                     \
    do {                                                                       \
        op("{%s()-%s:%d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args); \
    } while (0)
#endif

#define CAMERA_LOGE(fmt, ...) DECORATOR_HDFLOG(HDF_LOGE, fmt, ##__VA_ARGS__)
#define CAMERA_LOGW(fmt, ...) DECORATOR_HDFLOG(HDF_LOGW, fmt, ##__VA_ARGS__)
#define CAMERA_LOGI(fmt, ...) DECORATOR_HDFLOG(HDF_LOGI, fmt, ##__VA_ARGS__)
#define CAMERA_LOGV(fmt, ...) DECORATOR_HDFLOG(HDF_LOGV, fmt, ##__VA_ARGS__)
#define CAMERA_LOGD(fmt, ...) DECORATOR_HDFLOG(HDF_LOGD, fmt, ##__VA_ARGS__)

constexpr uint32_t FRAME_SIZE_TAG = 100;

using RetCode = uint32_t;
enum Ret : uint32_t {
    RC_OK = 0,
    RC_ERROR,
};

enum BufferSourceType {
    CAMERA_BUFFER_SOURCE_TYPE_NONE = -1,
    CAMERA_BUFFER_SOURCE_TYPE_GRALLOC,
    CAMERA_BUFFER_SOURCE_TYPE_HEAP,
    CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL,
    CAMERA_BUFFER_SOURCE_TYPE_MAX,
};

enum CameraBufferUsage : uint64_t {
    CAMERA_USAGE_SW_READ_OFTEN = (1 << 0),
    CAMERA_USAGE_SW_WRITE_OFTEN = (1 << 1),
    CAMERA_USAGE_MEM_DMA = (1 << 2),
    CAMERA_USAGE_MEM_SHARE = (1 << 3),
    CAMERA_USAGE_MEM_MMZ = (1 << 4),
    CAMERA_USAGE_MEM_MMZ_CACHE = (1 << 5),
    CAMERA_USAGE_MEM_FB = (1 << 6),
};

enum CameraBufferFormat : uint32_t {
    CAMERA_FORMAT_INVALID,      /* invalid format */
    CAMERA_FORMAT_RGB_565,      /* RGB565 format */
    CAMERA_FORMAT_RGBA_5658,    /* RGBA5658 format */
    CAMERA_FORMAT_RGBX_4444,    /* RGBX4444 format */
    CAMERA_FORMAT_RGBA_4444,    /* RGBA4444 format */
    CAMERA_FORMAT_RGB_444,      /* RGB444 format */
    CAMERA_FORMAT_RGBX_5551,    /* RGBX5551 format */
    CAMERA_FORMAT_RGBA_5551,    /* RGBA5551 format */
    CAMERA_FORMAT_RGB_555,      /* RGB555 format */
    CAMERA_FORMAT_RGBX_8888,    /* RGBX8888 format */
    CAMERA_FORMAT_RGBA_8888,    /* RGBA8888 format */
    CAMERA_FORMAT_RGB_888,      /* RGB888 format */
    CAMERA_FORMAT_BGR_565,      /* BGR565 format */
    CAMERA_FORMAT_BGRX_4444,    /* BGRX4444 format */
    CAMERA_FORMAT_BGRA_4444,    /* BGRA4444 format */
    CAMERA_FORMAT_BGRX_5551,    /* BGRX5551 format */
    CAMERA_FORMAT_BGRA_5551,    /* BGRA5551 format */
    CAMERA_FORMAT_BGRX_8888,    /* BGRX8888 format */
    CAMERA_FORMAT_BGRA_8888,    /* BGRA8888 format */
    CAMERA_FORMAT_YUV_422_I,    /* YUV422 interleaved format */
    CAMERA_FORMAT_YCBCR_422_SP, /* YCBCR422 semi-planar format */
    CAMERA_FORMAT_YCRCB_422_SP, /* YCRCB422 semi-planar format */
    CAMERA_FORMAT_YCBCR_420_SP, /* YCBCR420 semi-planar format */
    CAMERA_FORMAT_YCRCB_420_SP, /* YCRCB420 semi-planar format */
    CAMERA_FORMAT_YCBCR_422_P,  /* YCBCR422 planar format */
    CAMERA_FORMAT_YCRCB_422_P,  /* YCRCB422 planar format */
    CAMERA_FORMAT_YCBCR_420_P,  /* YCBCR420 planar format */
    CAMERA_FORMAT_YCRCB_420_P,  /* YCRCB420 planar format */
    CAMERA_FORMAT_YUYV_422_PKG, /* YUYV422 packed format */
    CAMERA_FORMAT_UYVY_422_PKG, /* UYVY422 packed format */
    CAMERA_FORMAT_YVYU_422_PKG, /* YVYU422 packed format */
    CAMERA_FORMAT_VYUY_422_PKG, /* VYUY422 packed format */
};

enum CameraEncodeType : int32_t {
    CAMERA_ENCODE_NULL = 0,
    CAMERA_ENCODE_H264 = 1,
    CAMERA_ENCODE_H265 = 2,
    CAMERA_ENCODE_JPEG = 3,
};

enum FlashMode : uint32_t { FlASH_OFF = 0, FlASH_SINGLE, FLASH_TORCH, FLASH_AUTO };

enum AdapterCmd : uint32_t { CMD_AE_EXPO, CMD_AWB_MODE, CMD_AE_EXPOTIME, CMD_AWB_COLORGAINS };

enum AwbMode : uint32_t {
    AWB_MODE_AUTO,
    AWB_MODE_CLOUDY_DAYLIGHT,
    AWB_MODE_TWILIGHT,
    AWB_MODE_FLUORESCENT,
    AWB_MODE_WARM_FLUORESCENT,
};

using EsFrameInfo = struct EsFrameInfo {
    int32_t size;
    int32_t align;
    int32_t isKey;
    int64_t timestamp;
    int32_t frameNum;
};

#define CHECK_IF_NOT_EQUAL_RETURN_VALUE(arg1, arg2, ret)                                                            \
    if ((arg1) != (arg2)) {                                                                                         \
        CAMERA_LOGE("%{public}u, %{public}s is not equal to %{public}s, return %{public}s", __LINE__, #arg1, #arg2, \
                    #ret);                                                                                          \
        return (ret);                                                                                               \
    }

#define CHECK_IF_EQUAL_RETURN_VALUE(arg1, arg2, ret)                                                                   \
    if ((arg1) == (arg2)) {                                                                                            \
        CAMERA_LOGE("%{public}u, %{public}s is equal to %{public}s, return %{public}s", __LINE__, #arg1, #arg2, #ret); \
        return (ret);                                                                                                  \
    }

#define CHECK_IF_PTR_NULL_RETURN_VALUE(ptr, ret) CHECK_IF_EQUAL_RETURN_VALUE(ptr, nullptr, ret)

#define CHECK_IF_NOT_EQUAL_RETURN_VOID(arg1, arg2)                                                        \
    if ((arg1) != (arg2)) {                                                                               \
        CAMERA_LOGE("%{public}u, %{public}s is not equal to %{public}s, return", __LINE__, #arg1, #arg2); \
        return;                                                                                           \
    }

#define CHECK_IF_EQUAL_RETURN_VOID(arg1, arg2)                                                        \
    if ((arg1) == (arg2)) {                                                                           \
        CAMERA_LOGE("%{public}u, %{public}s is equal to %{public}s, return", __LINE__, #arg1, #arg2); \
        return;                                                                                       \
    }

#define CHECK_IF_PTR_NULL_RETURN_VOID(ptr) CHECK_IF_EQUAL_RETURN_VOID(ptr, nullptr)
} // namespace Camera
} // namespace OHOS
#endif
