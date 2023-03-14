/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef AUDIO_CAPTURER_SERVER_H
#define AUDIO_CAPTURER_SERVER_H

#include <mutex>
#include <pthread.h>
#include <thread>
#include "audio_capturer_impl.h"
#include "liteipc_adapter.h"
#include "serializer.h"
#include "surface.h"

using OHOS::Audio::AudioCapturerImpl;
using OHOS::Surface;
namespace OHOS {
namespace Audio {
enum AudioCapturerFuncId {
    AUD_CAP_FUNC_CONNECT,
    AUD_CAP_FUNC_DISCONNECT,
    AUD_CAP_FUNC_GET_FRAME_COUNT,
    AUD_CAP_FUNC_GET_STATUS,
    AUD_CAP_FUNC_GET_TIME,
    AUD_CAP_FUNC_SET_INFO,
    AUD_CAP_FUNC_GET_INFO,
    AUD_CAP_FUNC_START,
    AUD_CAP_FUNC_STOP,
    AUD_CAP_FUNC_RELEASE,
    AUD_CAP_FUNC_GET_MIN_FRAME_COUNT,
    AUD_CAP_FUNC_SET_SURFACE,
    AUD_CAP_FUNC_BUTT,
};

static constexpr int32_t DEFAULT_IPC_SIZE = 100;
#define AUDIO_CAPTURER_SERVICE_NAME "AudioCapServer"

/* Since IPC is serialized, there is no concurrency problem */
class AudioCapturerServer {
public:
    AudioCapturerServer() = default;
    ~AudioCapturerServer() = default;

    int32_t AudioCapturerServerInit();
    static AudioCapturerServer *GetInstance();
    AudioCapturerImpl *GetAudioCapturer(pid_t pid);
    void AcceptServer(pid_t pid, IpcIo *reply);
    void DropServer(pid_t pid, IpcIo *reply);
    void Dispatch(int32_t funcId, pid_t pid, IpcIo *req, IpcIo *reply);
    static void *ReadAudioDataProcess(void *serverStr);

private:
    void GetMinFrameCount(IpcIo *req, IpcIo *reply);
    int32_t SetSurfaceProcess(Surface *surface);
    void SetInfo(AudioCapturerImpl *capturer, IpcIo *req, IpcIo *reply);
    void GetInfo(AudioCapturerImpl *capturer, IpcIo *reply);
    void Start(AudioCapturerImpl *capturer, IpcIo *reply);
    void Stop(AudioCapturerImpl *capturer, IpcIo *reply);
    void GetMiniFrameCount(IpcIo *req, IpcIo *reply);
    void GetFrameCount(AudioCapturerImpl *capturer, IpcIo *reply);
    void GetStatus(AudioCapturerImpl *capturer, IpcIo *reply);
    void SetSurface(IpcIo *req, IpcIo *reply);
    SurfaceBuffer *GetCacheBuffer(void);
    void CancelBuffer(SurfaceBuffer *buffer);
    void FreeCacheBuffer(void);

    pid_t clientPid_ = -1;
    AudioCapturerImpl *capturer_ = nullptr;
    Surface *surface_ = nullptr;
    // std::thread dataThreadId_;
    pthread_t dataThreadId_ = 0;
    std::mutex lock_;
    bool threadExit_ = false;
    SurfaceBuffer *bufCache_ = nullptr;
};

void AudioCapturerServiceReg();
}  // namespace Audio
}  // namespace OHOS
#endif // AUDIO_CAPTURER_SERVER_H
