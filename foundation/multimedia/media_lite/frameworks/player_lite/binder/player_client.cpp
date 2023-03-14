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

#include <cstdio>
#include "player_client.h"
#include "samgr_lite.h"
#include "surface.h"
#include "surface_impl.h"

extern "C" void __attribute__((weak)) OHOS_SystemInit(void)
{
    SAMGR_Bootstrap();
}

namespace OHOS {
namespace Media {
bool Player::PlayerClient::InitPlayerClient()
{
    OHOS_SystemInit();
    if (proxy_ == nullptr) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(SERVICE_NAME);
        if (iUnknown == nullptr) {
            MEDIA_ERR_LOG("iUnknown is NULL");
            return false;
        }
        (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&proxy_);
        if (proxy_ == nullptr) {
            MEDIA_ERR_LOG("QueryInterface failed");
            return false;
        }
    }
    return true;
}

void Player::PlayerClient::ClearIpcMsg(void *ipcMsg)
{
    if (ipcMsg == nullptr) {
        return;
    }
    uint32_t flag = 0;
    GetFlag(ipcMsg, &flag);
    if (flag == LITEIPC_FLAG_DEFAULT) {
        // send reply to client, the second param is reply
        SendReply(nullptr, ipcMsg, nullptr);
    } else {
        FreeBuffer(nullptr, ipcMsg);
    }
}

int Player::PlayerClient::Callback(void* owner, int code, IpcIo *reply)
{
    if (code != 0) {
        MEDIA_ERR_LOG("callback error, code = %d", code);
        return -1;
    }

    if (owner == nullptr) {
        return -1;
    }
    CallBackPara* para = (CallBackPara*)owner;
    switch (para->funcId) {
        case PLAYER_SERVER_SET_SOURCE: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            if ((*ret == 0) && (para->type == (int32_t)SourceType::SOURCE_TYPE_STREAM) && (para->data != nullptr)) {
                Source* source = (Source*)para->data;
                auto stream = source->GetSourceStream();
                Surface* surface = SurfaceImpl::GenericSurfaceByIpcIo(*reply);
                if (surface != nullptr) {
                    stream->SetSurface(surface);
                } else {
                    MEDIA_ERR_LOG("Stream source GenericSurfaceByIpcIo failed");
                }
            }
            break;
        }
        case PLAYER_SERVER_PREPARE: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_PLAY: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_IS_PLAYING: {
            bool* ret = static_cast<bool*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_PAUSE: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_STOP: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_REWIND: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_SET_VOLUME: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_SET_VIDEO_SURFACE: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_ENABLE_SINGLE_LOOPING: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_IS_SINGLE_LOOPING: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_GET_CURRENT_TIME: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int64_t* data = static_cast<int64_t*>(para->data);
            *data = IpcIoPopInt64(reply);
            break;
        }
        case PLAYER_SERVER_GET_DURATION: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int64_t* data = static_cast<int64_t*>(para->data);
            *data = IpcIoPopInt64(reply);
            break;
        }
        case PLAYER_SERVER_GET_VIDEO_WIDTH: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int32_t* data = static_cast<int32_t*>(para->data);
            *data = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_GET_VIDEO_HEIGHT: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int32_t* data = static_cast<int32_t*>(para->data);
            *data = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_RESET: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_RELEASE: {
            int32_t* ret = static_cast<int32_t*>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_SET_PLAYER_CALLBACK: {
            break;
        }
        case PLAYER_SERVER_GET_STATE: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int32_t *data = static_cast<int32_t *>(para->data);
            *data = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_SET_SPEED: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_GET_SPEED: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            float *data = static_cast<float *>(para->data);
            *data = IpcIoPopFloat(reply);
            break;
        }
        case PLAYER_SERVER_SET_AUDIO_STREAM_TYPE: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_GET_AUDIO_STREAM_TYPE: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            int32_t *data = static_cast<int32_t *>(para->data);
            *data = IpcIoPopInt32(reply);
            break;
        }
        case PLAYER_SERVER_SET_PARAMETER: {
            int32_t *ret = static_cast<int32_t *>(para->ret);
            *ret = IpcIoPopInt32(reply);
            break;
        }
        default:
            break;
    }
    return 0;
}

int32_t Player::PlayerClient::SetSource(const Source &source)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t sourceType = (int32_t)source.GetSourceType();
    IpcIoPushInt32(&io, sourceType);
    switch ((SourceType)sourceType) {
        case SourceType::SOURCE_TYPE_URI: {
            std::string uri = source.GetSourceUri();
            const char* str = uri.c_str();
            IpcIoPushString(&io, str);
            break;
        }
        case SourceType::SOURCE_TYPE_FD:
            MEDIA_ERR_LOG("unsupport now: SOURCE_TYPE_FD");
            break;
        case SourceType::SOURCE_TYPE_STREAM: {
            break;
        }
        default:
            break;
    }
    int32_t ans = -10;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_SOURCE;
    para.ret = &ans;
    para.type = sourceType;
    para.data = (void *)&source;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_SOURCE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetSource failed, ret=%d", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Prepare()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_PREPARE;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_PREPARE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Prepare failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Play()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_PLAY;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_PLAY, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Play failed, ret=%d\n", ret);
    }
    return ans;
}

bool Player::PlayerClient::IsPlaying()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    bool ans = false;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_IS_PLAYING;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_IS_PLAYING, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("IsPlaying failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Pause()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_PAUSE;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_PAUSE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Pause failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Stop()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_STOP;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_STOP, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Stop failed, ret=%d\n", ret);
    }
    return ans;
}
int32_t Player::PlayerClient::Rewind(int64_t mSeconds, int32_t mode)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt64(&io, mSeconds);
    IpcIoPushInt32(&io, mode);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_REWIND;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_REWIND, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Rewind failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetVolume(float leftVolume, float rightVolume)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushFlatObj(&io, &leftVolume, sizeof(float));
    IpcIoPushFlatObj(&io, &rightVolume, sizeof(float));
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_VOLUME;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_VOLUME, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVolume failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetSurface(Surface *surface)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    std::string position_x = surface->GetUserData("region_position_x");
    const char* str_x = position_x.c_str();
    IpcIoPushString(&io, str_x);
    std::string position_y = surface->GetUserData("region_position_y");
    const char* str_y = position_y.c_str();
    IpcIoPushString(&io, str_y);
    std::string width = surface->GetUserData("region_width");
    const char* str_width = width.c_str();
    IpcIoPushString(&io, str_width);
    std::string height = surface->GetUserData("region_height");
    const char* str_height = height.c_str();
    IpcIoPushString(&io, str_height);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_VIDEO_SURFACE;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_VIDEO_SURFACE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVideoSurfacee failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetLoop(bool loop)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushBool(&io, loop);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_ENABLE_SINGLE_LOOPING;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_ENABLE_SINGLE_LOOPING, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetLoop failed, ret=%d\n", ret);
    }
    return ans;
}

bool Player::PlayerClient::IsSingleLooping()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    bool ans = false;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_IS_SINGLE_LOOPING;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_IS_SINGLE_LOOPING, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("IsSingleLooping failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::GetCurrentPosition(int64_t &time) const
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;

    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_CURRENT_TIME;
    para.ret = &ans;
    para.data = &time;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_CURRENT_TIME, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetCurrentPosition failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::GetDuration(int64_t &duration) const
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_DURATION;
    para.ret = &ans;
    para.data = &duration;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_DURATION, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetDuration failed, ret=%d\n", ret);
    }
    return ans;
}
int32_t Player::PlayerClient::GetVideoWidth(int32_t &videoWidth)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_VIDEO_WIDTH;
    para.ret = &ans;
    para.data = &videoWidth;
    int32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_VIDEO_WIDTH, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetVideoWidth failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::GetVideoHeight(int32_t &videoHeight)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_VIDEO_HEIGHT;
    para.ret = &ans;
    para.data = &videoHeight;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_VIDEO_HEIGHT, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetVideoHeight failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Reset()
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_RESET;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_RESET, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Reset failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::Release()
{
    if (sid_ != nullptr) {
        UnregisterIpcCallback(*sid_);
        delete sid_;
        sid_ = nullptr;
    }
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_RELEASE;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_RELEASE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("Release failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::PlayerCommonCallback(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg)
{
    if (ipcMsg == nullptr) {
        MEDIA_ERR_LOG("call back error, ipcMsg is null\n");
        return -1;
    }
    auto playerCallback = static_cast<PlayerCallback*>(arg);
    if (playerCallback == nullptr) {
        MEDIA_ERR_LOG("call back error, playerCallback is null");
        ClearIpcMsg(ipcMsg);
        return -1;
    }
    uint32_t funcId = COMMAND_ERROR;
    (void)GetCode(ipcMsg, &funcId);
    MEDIA_INFO_LOG("PlayerCommonCallback, funcId=%d", funcId);
    switch (funcId) {
        case ON_PALYBACK_COMPLETE: {
            playerCallback->OnPlaybackComplete();
            break;
        }
        case ON_ERROR: {
            int32_t errorType = IpcIoPopInt32(io);
            int32_t errorCode = IpcIoPopInt32(io);
            playerCallback->OnError(errorType, errorCode);
            break;
        }
        case ON_INFO: {
            int type = static_cast<int32_t>(IpcIoPopInt32(io));
            int extra = static_cast<int32_t>(IpcIoPopInt32(io));
            playerCallback->OnInfo(type, extra);
            break;
        }
        case ON_VIDEO_SIZE_CHANGED: {
            int width = static_cast<int32_t>(IpcIoPopInt32(io));
            int height = static_cast<int32_t>(IpcIoPopInt32(io));
            playerCallback->OnVideoSizeChanged(width, height);
            break;
        }
        case ON_REWIND_TO_COMPLETE: {
            playerCallback->OnRewindToComplete();
            break;
        }
        default: {
            MEDIA_ERR_LOG("unsupport funId\n");
            break;
        }
    }
    ClearIpcMsg(ipcMsg);
    return 0;
}

void Player::PlayerClient::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    MEDIA_INFO_LOG("PlayerClient::SetPlayerCallback\n");
    if (sid_ == nullptr) {
        sid_ = new SvcIdentity();
    }
    int32_t ret = RegisterIpcCallback(PlayerCommonCallback, 0, IPC_WAIT_FOREVER, sid_, cb.get());
    if (ret != LITEIPC_OK) {
        MEDIA_ERR_LOG("RegisteIpcCallback failed\n");
        return;
    }
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 1);
    IpcIoPushInt32(&io, ret);
    IpcIoPushSvc(&io, sid_);
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_PLAYER_CALLBACK;
    uint32_t ans = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_PLAYER_CALLBACK, &io, &para, Callback);
    if (ans != 0) {
        MEDIA_ERR_LOG("SetPlayerCallback : Invoke failed, ret=%d\n", ans);
    }
}

int32_t Player::PlayerClient::GetPlayerState(int32_t &state) const
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_STATE;
    para.ret = &ans;
    para.data = &state;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_STATE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetPlayerState failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetPlaybackSpeed(float speed)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushFloat(&io, speed);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_SPEED;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_SPEED, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetPlaybackSpeed failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::GetPlaybackSpeed(float &speed)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_SPEED;
    para.ret = &ans;
    para.data = &speed;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_SPEED, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetPlaybackSpeed failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetParameter(const Format &params)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    
    std::map<std::string, FormatData *> formatParam = params.GetFormatMap();
    IpcIoPushInt32(&io, formatParam.size()); /* count */

    std::map<std::string, FormatData *> ::iterator iter;  
    for (iter = formatParam.begin(); iter != formatParam.end(); iter++) {
        IpcIoPushString(&io, iter->first.c_str()); /* key */
        FormatData *data = iter->second;
        FormatDataType type = data->GetType();
        IpcIoPushInt32(&io, type); /* type */

        /* value */
        if (type == FORMAT_TYPE_INT32) {
            int32_t value;
            data->GetInt32Value(value);
            IpcIoPushInt32(&io, value);
        } else if (type == FORMAT_TYPE_INT64) {
            int64_t value;
            data->GetInt64Value(value);
            IpcIoPushInt64(&io, value);
        } else if (type == FORMAT_TYPE_FLOAT) {
            float value;
            data->GetFloatValue(value);
            IpcIoPushFloat(&io, value);
        } else if (type == FORMAT_TYPE_DOUBLE) {
            double value;
            data->GetDoubleValue(value);
            IpcIoPushDouble(&io, value);
        } else if (type == FORMAT_TYPE_STRING) {
            std::string value;
            data->GetStringValue(value);
            IpcIoPushString(&io, value.c_str());
        } else {
            MEDIA_ERR_LOG("SetParameter failed, type:%d\n", type);
            return -1;
        }
    }

    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_PARAMETER;
    para.ret = &ans;
    int32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_PARAMETER, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("PlayerClient::SetParameter failed, ret=%d\n", ret);
    }
    return ans;
}

int32_t Player::PlayerClient::SetAudioStreamType(int32_t type)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    IpcIoPushInt32(&io, type);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_SET_AUDIO_STREAM_TYPE;
    para.ret = &ans;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_SET_AUDIO_STREAM_TYPE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetPlaybackSpeed failed, ret=%d\n", ret);
    }
    return ans;
}

void Player::PlayerClient::GetAudioStreamType(int32_t &type)
{
    IpcIo io;
    uint8_t tmpData[DEFAULT_IPC_SIZE];
    IpcIoInit(&io, tmpData, DEFAULT_IPC_SIZE, 0);
    int32_t ans = -1;
    CallBackPara para = {};
    para.funcId = PLAYER_SERVER_GET_AUDIO_STREAM_TYPE;
    para.ret = &ans;
    para.data = &type;
    uint32_t ret = proxy_->Invoke(proxy_, PLAYER_SERVER_GET_AUDIO_STREAM_TYPE, &io, &para, Callback);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetPlaybackSpeed failed, ret=%d\n", ret);
    }
}
}
}
