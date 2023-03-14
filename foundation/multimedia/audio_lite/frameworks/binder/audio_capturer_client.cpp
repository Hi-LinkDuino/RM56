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

#include "audio_capturer_client.h"

#include <cstdio>
#include "audio_capturer_server.h"
#include "liteipc_adapter.h"
#include "media_log.h"
#include "samgr_lite.h"
#include "securec.h"
#include "surface_impl.h"

extern "C" void __attribute__((weak)) OHOS_SystemInit(void)
{
    SAMGR_Bootstrap();
}

using namespace OHOS::Media;
using namespace std;
namespace OHOS {
namespace Audio {
constexpr int32_t SURFACE_QUEUE_SIZE = 5;
constexpr int32_t SURFACE_SIZE = 8192;
constexpr int32_t SURFACE_HEIGHT = 1;
constexpr int32_t SURFACE_WIDTH = 8192;

struct CallBackPara {
    int funcId;
    int32_t ret;
    void* data;
};


AudioCapturer::AudioCapturerClient *AudioCapturer::AudioCapturerClient::GetInstance()
{
    static AudioCapturerClient client;
    return &client;
}

static int32_t ProxyCallbackFunc(void *owner, int code, IpcIo *reply)
{
    if (code) {
        MEDIA_ERR_LOG("callback error, code = %d", code);
        return -1;
    }

    if (owner == nullptr) {
        return -1;
    }

    CallBackPara* para = static_cast<CallBackPara*>(owner);
    AudioCapturerFuncId funcId = (AudioCapturerFuncId)para->funcId;
    para->ret = IpcIoPopInt32(reply);
    switch (funcId) {
        case AUD_CAP_FUNC_CONNECT:
        case AUD_CAP_FUNC_DISCONNECT:
        case AUD_CAP_FUNC_SET_INFO:
        case AUD_CAP_FUNC_START:
        case AUD_CAP_FUNC_STOP:
        case AUD_CAP_FUNC_RELEASE:
        case AUD_CAP_FUNC_SET_SURFACE:
            break;
        case AUD_CAP_FUNC_GET_FRAME_COUNT:
            (*reinterpret_cast<uint64_t*>(para->data)) = IpcIoPopUint64(reply);
            break;
        case AUD_CAP_FUNC_GET_STATUS:
            (*reinterpret_cast<uint32_t*>(para->data)) = IpcIoPopUint32(reply);
            break;
        case AUD_CAP_FUNC_GET_INFO: {
            uint32_t size = 0;
            void *bufferAdd = IpcIoPopFlatObj(reply, &size);
            if (bufferAdd == nullptr || !size) {
                MEDIA_INFO_LOG("IpcIoPopFlatObj info failed");
                return -1;
            }
            (void)memcpy_s(para->data, sizeof(AudioCapturerInfo), bufferAdd, size);
            break;
        }
        case AUD_CAP_FUNC_GET_MIN_FRAME_COUNT:
            (*reinterpret_cast<uint32_t*>(para->data)) = IpcIoPopUint32(reply);
            break;
        default :
            MEDIA_INFO_LOG("Callback, unknown funcId = %d", para->funcId);
            break;
    }
    return 0;
}

int32_t AudioCapturer::AudioCapturerClient::InitSurface(void)
{
    MEDIA_DEBUG_LOG("AudioCapturerClient InitSurface");
    Surface *surface = Surface::CreateSurface();
    if (surface == nullptr) {
        return -1;
    }

    surface->RegisterConsumerListener(*this);
    surface_.reset(surface);

    surface->SetWidthAndHeight(SURFACE_WIDTH, SURFACE_HEIGHT);
    surface->SetQueueSize(SURFACE_QUEUE_SIZE);
    surface->SetSize(SURFACE_SIZE);
    return 0;
}

int32_t AudioCapturer::AudioCapturerClient::DeleteSurface(void)
{
    /* release all surface buffer */
    if (surface_ == nullptr) {
        return -1;
    }
    ReleaseAllBuffer();
    surface_->UnregisterConsumerListener();
    surface_.reset();
    surface_ = nullptr;
    return 0;
}

AudioCapturer::AudioCapturerClient::AudioCapturerClient()
{
    OHOS_SystemInit();
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(AUDIO_CAPTURER_SERVICE_NAME);
    if (iUnknown == nullptr) {
        MEDIA_ERR_LOG("iUnknown is nullptr");
        throw runtime_error("Ipc proxy GetDefaultFeatureApi failed.");
    }

    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&proxy_);
    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("QueryInterface failed");
        throw runtime_error("Ipc proxy init failed.");
    }

    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_CONNECT, .ret = MEDIA_IPC_FAILED, .data = this};
    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_CONNECT, nullptr, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Connect audio capturer server failed, ret=%d", ret);
        throw runtime_error("Ipc proxy Invoke failed.");
    }

    /* Creating a Surface and Initializing Settings */
    MEDIA_DEBUG_LOG("InitSurface audio capturer.");
    InitSurface();
    /* The surface is transferred to the server for processing */
    timeStampValid_ = false;
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 1);
    dynamic_cast<SurfaceImpl *>(surface_.get())->WriteIoIpcIo(io);
    para = {.funcId = AUD_CAP_FUNC_SET_SURFACE, .ret = MEDIA_IPC_FAILED, .data = this};
    ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_SET_SURFACE, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Connect audio capturer set surface failed, ret=%d", ret);
        throw runtime_error("Ipc proxy Invoke failed.");
    }

    MEDIA_INFO_LOG("Create audio capturer client succeed.");
}

void AudioCapturer::AudioCapturerClient::ReleaseAllBuffer()
{
    timeStampValid_ = false;
    while (true) {
        SurfaceBuffer *surfaceBuf = surface_->AcquireBuffer();
        if (surfaceBuf == nullptr) {
            break;
        }
        surface_->ReleaseBuffer(surfaceBuf);
    }
}

AudioCapturer::AudioCapturerClient::~AudioCapturerClient()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_DISCONNECT, .ret = MEDIA_IPC_FAILED};
    uint32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_DISCONNECT, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Disconnect audioCapturer server failed, ret=%d", ret);
    }

    /* release all surface buffer */
    if (surface_ != nullptr) {
        DeleteSurface();
    }
    MEDIA_INFO_LOG("dector");
}

bool AudioCapturer::AudioCapturerClient::GetMinFrameCount(int32_t sampleRate, int32_t channelCount,
                                                          AudioCodecFormat audioFormat, size_t &frameCount)
{
    AudioCapturerClient *client = AudioCapturer::AudioCapturerClient::GetInstance();
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt32(&io, sampleRate);
    IpcIoPushInt32(&io, channelCount);
    IpcIoPushFlatObj(&io, &audioFormat, sizeof(audioFormat));
    CallBackPara para = {.funcId = AUD_CAP_FUNC_GET_MIN_FRAME_COUNT, .ret = MEDIA_IPC_FAILED, .data = &frameCount};
    uint32_t ret = client->proxy_->Invoke(client->proxy_, AUD_CAP_FUNC_GET_MIN_FRAME_COUNT, &io, &para,
                                        ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("GetMinFrameCount failed, ret=%d", ret);
        return false;
    }
    return (!para.ret) ? true : false;
}

uint64_t AudioCapturer::AudioCapturerClient::GetFrameCount()
{
    IpcIo io;
    uint64_t frameCount;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_GET_FRAME_COUNT, .ret = MEDIA_IPC_FAILED, .data = &frameCount};

    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("GetFrameCount failed, proxy_ value is nullptr");
        return 0;
    }

    uint32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_GET_FRAME_COUNT, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("GetFrameCount failed, ret=%d", ret);
    }

    return frameCount;
}

State AudioCapturer::AudioCapturerClient::GetStatus()
{
    IpcIo io;
    uint32_t state;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_GET_STATUS, .ret = MEDIA_IPC_FAILED, .data = &state};

    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("GetStatus failed, proxy_ value is nullptr");
        return (State)state;
    }

    uint32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_GET_STATUS, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("GetStatus failed, ret=%d", ret);
    }

    return (State)state;
}

bool AudioCapturer::AudioCapturerClient::GetAudioTime(Timestamp &timestamp, Timestamp::Timebase base)
{
    timestamp = curTimestamp_;
    return true;
}

int32_t AudioCapturer::AudioCapturerClient::SetCapturerInfo(const AudioCapturerInfo info)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushFlatObj(&io, &info, sizeof(info));
    CallBackPara para = {.funcId = AUD_CAP_FUNC_SET_INFO, .ret = MEDIA_IPC_FAILED};

    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("SetCapturerInfo failed, proxy_ value is nullptr");
        return 0;
    }

    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_SET_INFO, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("SetCapturerInfo failed, ret=%d", ret);
        return ret;
    }
    return para.ret;
}

int32_t AudioCapturer::AudioCapturerClient::GetCapturerInfo(AudioCapturerInfo &info)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_GET_INFO, .ret = MEDIA_IPC_FAILED, .data = &info};

    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("GetCapturerInfo failed, proxy_ value is nullptr");
        return 0;
    }

    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_GET_INFO, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("GetCapturerInfo failed, ret=%d", ret);
        return ret;
    }
    return para.ret;
}

bool AudioCapturer::AudioCapturerClient::Start()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_START, .ret = MEDIA_IPC_FAILED};
    
    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("Start failed, proxy_ value is nullptr");
        return false;
    }
    
    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_START, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Start failed, ret=%d", ret);
        return ret;
    }

    return para.ret;
}

bool AudioCapturer::AudioCapturerClient::Stop()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_STOP, .ret = MEDIA_IPC_FAILED};
    
    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("Stop failed, proxy_ value is nullptr");
        return false;
    }

    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_STOP, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Stop failed, ret=%d", ret);
        return ret;
    }

    return para.ret;
}

bool AudioCapturer::AudioCapturerClient::Release()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    CallBackPara para = {.funcId = AUD_CAP_FUNC_RELEASE, .ret = MEDIA_IPC_FAILED};
    
    if (proxy_ == nullptr) {
        MEDIA_ERR_LOG("Release failed, proxy_ value is nullptr");
        return false;
    }

    int32_t ret = proxy_->Invoke(proxy_, AUD_CAP_FUNC_RELEASE, &io, &para, ProxyCallbackFunc);
    if (ret) {
        MEDIA_ERR_LOG("Release failed, ret=%d", ret);
        return ret;
    }

    DeleteSurface();
    return para.ret;
}

int32_t AudioCapturer::AudioCapturerClient::Read(uint8_t *buffer, size_t userSize, bool isBlockingRead)
{
    if (buffer == nullptr || !userSize) {
        return ERR_INVALID_READ;
    }
    int32_t readLen = ERR_INVALID_READ;
    do {
        SurfaceBuffer *surfaceBuf = surface_->AcquireBuffer();
        if (surfaceBuf == nullptr) {
            if (isBlockingRead) {
                usleep(10000); // indicates 10000 microseconds
                continue;
            } else {
                break;
            }
        }

        uint8_t *buf = static_cast<uint8_t *> (surfaceBuf->GetVirAddr());
        int32_t dataSize = surfaceBuf->GetSize();
        if (dataSize - sizeof(Timestamp) > userSize) {
            surface_->ReleaseBuffer(surfaceBuf);
            MEDIA_ERR_LOG("input buffer size too small.");
            break;
        }

        (void)memcpy_s(buffer, userSize, buf + sizeof(Timestamp), dataSize - sizeof(Timestamp));
        (void)memcpy_s(&curTimestamp_, sizeof(Timestamp), buf, sizeof(Timestamp));
        timeStampValid_ = true;

        surface_->ReleaseBuffer(surfaceBuf);
        readLen = dataSize - sizeof(Timestamp);
        break;
    } while (isBlockingRead);

    return readLen;
}

void AudioCapturer::AudioCapturerClient::OnBufferAvailable()
{
    if (surface_ == nullptr) {
        MEDIA_ERR_LOG("OnBufferAvailable failed, surface_ is nullptr");
        return;
    }
}

IClientProxy *AudioCapturer::AudioCapturerClient::GetIClientProxy()
{
    return proxy_;
}
}  // namespace Audio
}  // namespace OHOS
