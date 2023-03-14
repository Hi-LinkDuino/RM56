/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "recorder_service.h"
#include "media_log.h"
#include "surface_impl.h"

namespace OHOS {
namespace Media {
RecorderClientMng *RecorderClientMng::GetInstance()
{
    static RecorderClientMng mng;
    return &mng;
}

RecorderImpl *RecorderClientMng::GetRecorder(pid_t pid)
{
    return (pid == client_) ? rec_ : nullptr;
}

bool RecorderClientMng::AcceptClient(pid_t pid)
{
    if (client_ == -1) {
        rec_ = new RecorderImpl;
        client_ = pid;
        return true;
    }
    return false;
}

void RecorderClientMng::DropClient(pid_t pid)
{
    if (pid == client_) {
        delete rec_;
        rec_ = nullptr;
        client_ = -1;
    }
}

void RecorderClientMng::Dispatch(int32_t funcId, pid_t pid, IpcIo *req, IpcIo *reply)
{
    if (funcId == REC_FUNC_CONNECT) {
        AcceptClient(pid);
        return;
    }
    auto recorder = GetRecorder(pid);
    if (recorder == nullptr) {
        MEDIA_ERR_LOG("Cannot find client object.(pid=%d)", pid);
        IpcIoPushInt32(reply, MEDIA_IPC_FAILED);
        return;
    }
    switch (funcId) {
        case REC_FUNC_DISCONNECT:
            DropClient(pid);
            break;
        case REC_FUNC_SET_VIDEOSOURCE: {
            uint32_t objSize;
            VideoSourceType *src = (VideoSourceType *)IpcIoPopFlatObj(req, &objSize);
            int32_t srcId;
            int32_t ret = recorder->SetVideoSource(*src, srcId);
            IpcIoPushInt32(reply, ret);
            IpcIoPushInt32(reply, srcId);
            break;
        }
        case REC_FUNC_SET_VIDEOENCODER: {
            int32_t src = IpcIoPopInt32(req);
            uint32_t objSize;
            VideoCodecFormat *enc = (VideoCodecFormat *)IpcIoPopFlatObj(req, &objSize);
            int32_t ret = recorder->SetVideoEncoder(src, *enc);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_VIDEOSIZE: {
            int32_t src = IpcIoPopInt32(req);
            int32_t width = IpcIoPopInt32(req);
            int32_t height = IpcIoPopInt32(req);
            int32_t ret = recorder->SetVideoSize(src, width, height);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_VIDEOFRAMERATE: {
            int32_t sourceId = IpcIoPopInt32(req);
            int32_t frameRate = IpcIoPopInt32(req);
            int32_t ret = recorder->SetVideoFrameRate(sourceId, frameRate);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_VIDEOENCODINGBITRATE: {
            int32_t sourceId = IpcIoPopInt32(req);
            int32_t rate = IpcIoPopInt32(req);
            int32_t ret = recorder->SetVideoEncodingBitRate(sourceId, rate);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_CAPTURERATE: {
            int32_t sourceId = IpcIoPopInt32(req);
            uint32_t objSize;
            double *fps = (double *)IpcIoPopFlatObj(req, &objSize);
            int32_t ret = recorder->SetCaptureRate(sourceId, *fps);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_GET_SURFACE: {
            int32_t sourceId = IpcIoPopInt32(req);
            std::shared_ptr<OHOS::Surface> surface = recorder->GetSurface(sourceId);
            if (surface != nullptr) {
                dynamic_cast<OHOS::SurfaceImpl *>(surface.get())->WriteIoIpcIo(*reply);
            }
            break;
        }
        case REC_FUNC_SET_AUDIOSOURCE: {
            uint32_t objSize;
            AudioSourceType *src = (AudioSourceType *)IpcIoPopFlatObj(req, &objSize);
            int32_t srcId;
            int32_t ret = recorder->SetAudioSource(*src, srcId);
            IpcIoPushInt32(reply, ret);
            IpcIoPushInt32(reply, srcId);
            break;
        }
        case REC_FUNC_SET_AUDIOENCODER: {
            int32_t sourceId = IpcIoPopInt32(req);
            uint32_t objSize;
            AudioCodecFormat *enc = (AudioCodecFormat *)IpcIoPopFlatObj(req, &objSize);
            int32_t ret = recorder->SetAudioEncoder(sourceId, *enc);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_AUDIOSAMPLERATE: {
            int32_t sourceId = IpcIoPopInt32(req);
            int32_t rate = IpcIoPopInt32(req);
            int32_t ret = recorder->SetAudioSampleRate(sourceId, rate);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_AUDIOCHANNELS: {
            int32_t sourceId = IpcIoPopInt32(req);
            int32_t num = IpcIoPopInt32(req);
            int32_t ret = recorder->SetAudioChannels(sourceId, num);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_AUDIOENCODINGBITRATE: {
            int32_t sourceId = IpcIoPopInt32(req);
            int32_t bitRate = IpcIoPopInt32(req);
            int32_t ret = recorder->SetAudioEncodingBitRate(sourceId, bitRate);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_MAXDURATION: {
            int32_t duration = IpcIoPopInt32(req);
            int32_t ret = recorder->SetMaxDuration(duration);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_OUTPUTFORMAT: {
            uint32_t objSize;
            OutputFormatType *format = (OutputFormatType *)IpcIoPopFlatObj(req, &objSize);
            int32_t ret = recorder->SetOutputFormat(*format);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_OUTPUTPATH: {
            size_t strSize;
            char *path = (char *)IpcIoPopString(req, &strSize);
            int32_t ret = recorder->SetOutputPath(string(path));
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_OUTPUTFILE: {
            int32_t fd = IpcIoPopFd(req);
            int32_t ret = recorder->SetOutputFile(fd);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_NEXTOUTPUTFILE: {
            int32_t fd = IpcIoPopFd(req);
            int32_t ret = recorder->SetNextOutputFile(fd);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_MAXFILESIZE: {
            int64_t size = IpcIoPopInt64(req);
            int32_t ret = recorder->SetMaxFileSize(size);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_RECORDERCALLBACK: {
            svc_ = *IpcIoPopSvc(req);
#ifdef __LINUX__
            BinderAcquire(svc_.ipcContext, svc_.handle);
#endif
            std::shared_ptr<RecorderCallbackClient> client(new RecorderCallbackClient(&svc_));
            int32_t ret = recorder->SetRecorderCallback(client);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_PREPARE: {
            int32_t ret = recorder->Prepare();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_START: {
            int32_t ret = recorder->Start();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_PAUSE: {
            int32_t ret = recorder->Pause();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_RESUME: {
            int32_t ret = recorder->Resume();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_STOP: {
            bool block = IpcIoPopBool(req);
            int32_t ret = recorder->Stop(block);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_RESET: {
            int32_t ret = recorder->Reset();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_RELEASE: {
            int32_t ret = recorder->Release();
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_FILESPLITDURATION: {
            uint32_t objSize;
            FileSplitType *type = (FileSplitType *)IpcIoPopFlatObj(req, &objSize);
            int64_t timestamp = IpcIoPopInt64(req);
            uint32_t duration = IpcIoPopUint32(req);
            int32_t ret = recorder->SetFileSplitDuration(*type, timestamp, duration);
            IpcIoPushInt32(reply, ret);
            break;
        }
        case REC_FUNC_SET_PARAMETER: {
            break;
        }
        case REC_FUNC_SET_DATASOURCE: {
            uint32_t objSize;
            DataSourceType *src = (DataSourceType *)IpcIoPopFlatObj(req, &objSize);
            int32_t srcId;
            int32_t ret = recorder->SetDataSource(*src, srcId);
            IpcIoPushInt32(reply, ret);
            IpcIoPushInt32(reply, srcId);
            break;
        }
        default:
            break;
    }
}

void RecorderCallbackClient::OnError(int32_t errorType, int32_t errorCode)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt32(&io, errorType);
    IpcIoPushInt32(&io, errorCode);
    int32_t ret = Transact(nullptr, *sid_, REC_ANONYMOUS_FUNC_ON_ERROR, &io, nullptr, LITEIPC_FLAG_ONEWAY, nullptr);
    if (ret != LITEIPC_OK) {
        MEDIA_ERR_LOG("Recorder server transact OnError failed.(ret=%d)", ret);
    }
}

void RecorderCallbackClient::OnInfo(int32_t type, int32_t extra)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt32(&io, type);
    IpcIoPushInt32(&io, extra);
    int32_t ret = Transact(nullptr, *sid_, REC_ANONYMOUS_FUNC_ON_INFO, &io, nullptr, LITEIPC_FLAG_ONEWAY, nullptr);
    if (ret != LITEIPC_OK) {
        MEDIA_ERR_LOG("Recorder server transact OnInfo failed.(ret=%d)", ret);
    }
}
} // namespace Media
} // namespace OHOS