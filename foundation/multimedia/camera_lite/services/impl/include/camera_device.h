/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_CAMERA_DEVICE_H
#define OHOS_CAMERA_DEVICE_H

#include <cstdbool>
#include <thread>
#include <vector>

#include "camera_ability.h"
#include "camera_config.h"
#include "codec_type.h"
#include "frame_config.h"
#include "surface.h"
using namespace std;
namespace OHOS {
namespace Media {
enum LoopState {
    LOOP_IDLE,
    LOOP_READY,
    LOOP_LOOPING,
    LOOP_STOP,
    LOOP_ERROR,
};
const int32_t RECODER_MAX_NUM = 2;
class DeviceAssistant {
public:
    std::thread *thrd_ = nullptr;
    LoopState state_ = LOOP_IDLE;
    FrameConfig *fc_ = nullptr;
    uint32_t cameraId_;
    uint32_t streamId_;

    virtual int32_t SetFrameConfig(FrameConfig &fc, uint32_t *streamId)
    {
        return -1;
    }
    virtual int32_t Start(uint32_t streamId)
    {
        return -1;
    }
    virtual int32_t Stop()
    {
        return -1;
    }
};

class RecordAssistant : public DeviceAssistant {
public:
    int32_t SetFrameConfig(FrameConfig &fc, uint32_t *streamId) override;
    int32_t Start(uint32_t streamId) override;
    int32_t Stop() override;

    vector<CODEC_HANDLETYPE> vencHdls_;
    vector<list<Surface *>> vencSurfaces_;
    static int OnVencBufferAvailble(UINTPTR hComponent, UINTPTR dataIn, OutputInfo *buffer);
    static CodecCallback recordCodecCb_;
    int32_t streamIdNum_[RECODER_MAX_NUM] = {-1, -1};
};

class PreviewAssistant : public DeviceAssistant {
public:
    int32_t SetFrameConfig(FrameConfig &fc, uint32_t *streamId) override;
    int32_t Start(uint32_t streamId) override;
    int32_t Stop() override;
    Surface *capSurface_ = nullptr;
private:
    pthread_t threadId;
    static void *YuvCopyProcess(void *arg);
};

class CaptureAssistant : public DeviceAssistant {
    int32_t SetFrameConfig(FrameConfig &fc, uint32_t *streamId) override;
    int32_t Start(uint32_t streamId) override;
    int32_t Stop() override;
    CODEC_HANDLETYPE vencHdl_ = nullptr;
    Surface *capSurface_ = nullptr;
};

class CallbackAssistant : public DeviceAssistant {
public:
    int32_t SetFrameConfig(FrameConfig &fc, uint32_t *streamId) override;
    int32_t Start(uint32_t streamId) override;
    int32_t Stop() override;
    Surface *capSurface_ = nullptr;
private:
    pthread_t threadId;
    static void *StreamCopyProcess(void *arg);
};

class CameraDevice {
public:
    CameraDevice();
    explicit CameraDevice(uint32_t cameraId);
    virtual ~CameraDevice();

    int32_t Initialize();
    int32_t UnInitialize();
    int32_t SetCameraConfig();
    int32_t TriggerLoopingCapture(FrameConfig &fc, uint32_t *streamId);
    void StopLoopingCapture(int32_t type);
    int32_t TriggerSingleCapture(FrameConfig &fc, uint32_t *streamId);
    uint32_t GetCameraId();
private:
    uint32_t cameraId;
    RecordAssistant recordAssistant_;
    PreviewAssistant previewAssistant_;
    CaptureAssistant captureAssistant_;
    CallbackAssistant callbackAssistant_;
};
} // namespace Media
} // namespace OHOS
#endif // OHOS_CAMERA_DEVICE_H