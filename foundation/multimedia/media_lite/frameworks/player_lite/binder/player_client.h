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

#ifndef PLAYER_CLIENT_H
#define PLAYER_CLIENT_H

#include <unistd.h>
#include <pthread.h>
#include <iunknown.h>
#include <iproxy_server.h>
#include <iproxy_client.h>
#include "format.h"
#include "media_log.h"
#include "player_type.h"
#include "liteipc_adapter.h"
#include "serializer.h"
#include "surface.h"
#include "source.h"
#include "player.h"
namespace OHOS {
namespace Media {
class Player::PlayerClient {
public:
    static PlayerClient* GetInstance()
    {
        static PlayerClient client;
        if (client.InitPlayerClient() == false) {
            printf("InitPlayerClient failed\n");
            return nullptr;
        }
        return &client;
    }

    bool InitPlayerClient();
    static void ClearIpcMsg(void *ipcMsg);
    static int32_t PlayerCommonCallback(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg);

    static int Callback(void* owner, int code, IpcIo *reply);
    int32_t SetSource(const Source &source);
    int32_t Prepare();
    int32_t Play();
    bool IsPlaying();
    int32_t Pause();
    int32_t Stop();
    int32_t Rewind(int64_t mSeconds, int32_t mode);
    int32_t SetVolume(float leftVolume, float rightVolume);
    int32_t SetSurface(Surface *surface);
    int32_t SetLoop(bool loop);
    bool IsSingleLooping();
    int32_t GetCurrentPosition(int64_t &time) const;
    int32_t GetDuration(int64_t &duration) const;
    int32_t GetVideoWidth(int32_t &videoWidth);
    int32_t GetVideoHeight(int32_t &videoHeight);
    int32_t Reset();
    int32_t Release();
    void SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb);
    int32_t GetPlayerState(int32_t &state) const;
    int32_t SetPlaybackSpeed(float speed);
    int32_t GetPlaybackSpeed(float &speed);
    int32_t SetParameter(const Format &params);
    int32_t SetAudioStreamType(int32_t type);
    void GetAudioStreamType(int32_t &type);

private:
    PlayerClient() : proxy_(nullptr), sid_(nullptr) {}
    ~PlayerClient() {}
    IClientProxy *proxy_;
    SvcIdentity *sid_;
};
}
}
#endif