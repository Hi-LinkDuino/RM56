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

#ifndef HOS_CAMERA_TYPES_C_IF_H
#define HOS_CAMERA_TYPES_C_IF_H

#ifdef __cplusplus
extern "C" {
#endif

enum CameraStatusCIF {
    UNAVAILABLE = 0,
    AVAILABLE = 1,
};

enum FlashlightStatusCIF {
    FLASHLIGHT_OFF = 0,
    FLASHLIGHT_ON = 1,
    FLASHLIGHT_UNAVAILABLE = 2,
};

enum RetCodeCIF {
    NO_ERROR = 0,
    CAMERA_BUSY = 1,
    INSUFFICIENT_RESOURCES = 2,
    INVALID_ARGUMENT = 3,
    METHOD_NOT_SUPPORTED = 4,
    CAMERA_CLOSED = 5,
    DEVICE_ERROR = 6,
};

typedef int MetaTypeCIF;

enum ErrorTypeCIF {
    FATAL_ERROR,
};

enum OperationModeCIF {
    NORMAL = 0,
};

enum StreamIntentCIF {
    /**
     * 用来送显的流类型
     */
    PREVIEW = 0,
    /**
     * 送给VideoEncoder的流类型
     */
    VIDEO = 1,
    /**
     * 进行拍照的流配型
     */
    STILL_CAPTURE = 2,
    /**
     * 用来进行拍照回显的流类型
     */
    POST_VIEW = 3,
    /**
     * 用来做图像分析的流类型
     */
    ANALYZE = 4,
    /**
     * 普通自定义类型
     */
    CUSTOM = 5,
};

enum StreamSupportTypeCIF {
    /**
     * 流的组合和参数支持，并且基于当前流的配置可以动态切换
     */
    DYNAMIC_SUPPORTED,
    /**
     * 流的组合和参数支持，但是需要服务重新配置所有流
     */
    RE_CONFIGURED_REQUIRED,
    /**
     * 流的组合或者参数不支持
     */
    NOT_SUPPORTED,
};

typedef struct StreamAttributeCIF {
    /**
     * 流的标志Id号，设备级别唯一
     */
    int streamId;
    /**
     * 请求的图像长度
     */
    int width;
    /**
     * 请求的图像高度
     */
    int height;
    /**
     * HAL覆盖的图像格式
     */
    int overrideFormat;
    /**
     * HAL覆盖的图像颜色空间
     */
    int overrideDatasapce;
    /**
     * HAL需要的usage(需要等Graphic提供正式的BuferQueue定义)
     */
    int producerUsage;
    /**
     * HAL需要的buffer个数
     */
    int producerBufferCount;
    /**
     * 通过batch模式在一次Capture中能下发的捕获帧数
     */
    int maxBatchCaptureCount;
    /**
     * 可以最大的并发拍照请求次数
     */
    int maxCaptureCount;
} StreamAttributeCIF;

typedef struct CaptureInfoCIF {
    /**
     * 拍照的流Id
     */
    int* streamIds;
    int count;
    /**
     * 拍照的流参数
     */
    // CameraMetadata* captureSetting;
    /**
     * 使能shutter回调
     */
    int enableShutterCallback;
} CaptureInfoCIF;

struct CaptureEndedInfoCIF {
    /**
     * 流Id
     */
    int streamId;

    /**
     * 捕获结束时正常抓取的帧数
     */
    int frameCount;
};

enum StreamErrorCIF {
    /**
     * 流未知错误
     */
    UNKNOWN_ERROR = 0,
    /**
     * 丢包
     */
    BUFFER_LOST = 1,
};

typedef struct CaptureErrorInfoCIF {
    /**
     *  流Id
     */
    int streamId;

    /**
     *  错误类型
     */
    StreamErrorCIF error;
} CaptureErrorInfoCIF;

enum ResultCallbackModeCIF {
    /**
     * 每帧上报Result
     */
    PER_FRAME,
    /**
     * 设备状态变化时才上报Result
     */
    ON_CHANGED
};

typedef struct GraphicBufferCIF {
    int fd;
    int width;
    int stride;
    int height;
    int size;
    int format;
    long long usage;
    void* virAddr;
    unsigned long long phyAddr;
    int fence;
    int streamId;
} GraphicBufferCIF;

typedef struct BufferProducerCIF {
    int (*RequestBuffer)(GraphicBufferCIF* buffer);
    int (*CancelBuffer)(GraphicBufferCIF* buffer);
    int (*FlushBuffer)(GraphicBufferCIF* buffer);
    int (*GetQueueSize)(int streamId);
    unsigned int (*SetQueueSize)(int streamId, int size);
    void (*DetachBufferProducer)(int streamId);
} BufferProducerCIF;

typedef struct StreamInfoCIF {
    int streamId;
    int width;
    int height;
    int format;
    int datasapce;
    int /*StreamIntentCIF*/ intent;
    int tunneledMode;
    BufferProducerCIF producer;
    int minFrameDuration;
} StreamInfoCIF;

#ifdef __cplusplus
}
#endif

#endif
