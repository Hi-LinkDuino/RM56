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

#ifndef PLAYER_SERVER_H
#define PLAYER_SERVER_H

#include "liteipc_adapter.h"
#include "serializer.h"
#include "player_interface.h"

namespace OHOS {
namespace Media {
class PalyerCallbackImpl : public PlayerCallback {
public:
    PalyerCallbackImpl(SvcIdentity &sid) : sid_(sid) {}
    ~PalyerCallbackImpl() {}
    void OnPlaybackComplete();
    void OnError(int32_t errorType, int32_t errorCode);
    void OnInfo(int type, int extra);
    void OnVideoSizeChanged(int width, int height);
    void OnRewindToComplete();
private:
    SvcIdentity sid_;
};

class PlayerServer {
public:
    static PlayerServer* GetInstance()
    {
        static PlayerServer server;
        return &server;
    }

    static void PlayerServerRequestHandle(int funcId, void *origin, IpcIo *req, IpcIo *reply);
    int32_t PlayerServerInit();
private:
    std::shared_ptr<PlayerInterface> player_;
    std::shared_ptr<StreamSource> stream_;
    std::shared_ptr<PlayerCallback> playerCallback_;
    SvcIdentity *sid_;
    PlayerServer() : player_(nullptr), stream_(nullptr), sid_(nullptr) {}
    ~PlayerServer() {}

    void SetSource(IpcIo *req, IpcIo *reply);
    void Prepare(IpcIo *req, IpcIo *reply);
    void Play(IpcIo *req, IpcIo *reply);
    void IsPlaying(IpcIo *req, IpcIo *reply);
    void Pause(IpcIo *req, IpcIo *reply);
    void Stop(IpcIo *req, IpcIo *reply);
    void Rewind(IpcIo *req, IpcIo *reply);
    void SetVolume(IpcIo *req, IpcIo *reply);
    void SetSurface(IpcIo *req, IpcIo *reply);
    void SetLoop(IpcIo *req, IpcIo *reply);
    void IsSingleLooping(IpcIo *req, IpcIo *reply);
    void GetCurrentPosition(IpcIo *req, IpcIo *reply);
    void GetDuration(IpcIo *req, IpcIo *reply);
    void GetVideoWidth(IpcIo *req, IpcIo *reply);
    void GetVideoHeight(IpcIo *req, IpcIo *reply);
    void Reset(IpcIo *req, IpcIo *reply);
    void Release(IpcIo *req, IpcIo *reply);
    void SetPlayerCallback(IpcIo *req, IpcIo *reply);
    void GetPlayerState(IpcIo *req, IpcIo *reply);
    void SetPlaybackSpeed(IpcIo *req, IpcIo *reply);
    void GetPlaybackSpeed(IpcIo *req, IpcIo *reply);
    void SetParameter(IpcIo *req, IpcIo *reply);
    void SetAudioStreamType(IpcIo *req, IpcIo *reply);
    void GetAudioStreamType(IpcIo *req, IpcIo *reply);
    void SetStreamSource(IpcIo *reply);
};
}
}
#endif
