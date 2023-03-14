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

#ifndef HOS_CAMERA_STREAM_OPERATOR_C_IF_H
#define HOS_CAMERA_STREAM_OPERATOR_C_IF_H

#include "camera_types_c_if.h"
#include "camera_metadata_c_if.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StreamOperatorCallbackCIF {
    void (*OnCaptureStarted)(int captureId, int* streamId, int count);
    void (*OnCaptureEnded)(int captureId, CaptureEndedInfoCIF* info, int count);
    void (*OnCaptureError)(int captureId, CaptureErrorInfoCIF* info, int count);
    void (*OnFrameShutter)(int captureId, int* streamId, int count, long timestamp);
} StreamOperatorCallbackCIF;

typedef struct OfflineStreamOperatorCIF {
    int (*CancelCapture)(int captureId);
    int (*ReleaseStreams)(int* streamIds, int count);
    int (*Release)();
} OfflineStreamOperatorCIF;

typedef struct StreamOperatorCIF {
    int (*IsStreamSupported)(int /*OperationModeCIF*/ mode,
                             CameraMetadataCIF* meta,
                             StreamInfoCIF info,
                             int* /*StreamSupportTypeCIF*/ support);

    int (*CreateStreams)(StreamInfoCIF* info, int count);
    int (*ReleaseStreams)(int* streamIds, int count);
    int (*CommitStreams)(int /*OperationModeCIF*/ mode, CameraMetadataCIF* meta);
    int (*GetStreamAttributes)(StreamAttributeCIF** attributes, int* count);
    int (*AttachBufferQueue)(int streamId, BufferProducerCIF producer);
    int (*DetachBufferQueue)(int streamId);
    int (*Capture)(int captureId, CaptureInfoCIF info, int isStreaming);
    int (*CancelCapture)(int captureId);
    int (*ChangeToOfflineStream)(int* streamIds,
                                 int count,
                                 StreamOperatorCallbackCIF callback,
                                 OfflineStreamOperatorCIF* op);
} StreamOperatorCIF;

#ifdef __cplusplus
}
#endif

#endif
