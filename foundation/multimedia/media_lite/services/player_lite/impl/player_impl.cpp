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

#include "player_impl.h"
#include <cinttypes>
#include <climits>
#include <string>
#include <sys/prctl.h>
#include "unistd.h"
#include "securec.h"
#include "format_type.h"
#include "hi_liteplayer_err.h"
#include "hi_liteplayer.h"
#include "player_define.h"
#include "media_log.h"
extern "C"
{
#include "codec_interface.h"
}

using namespace std;
using OHOS::Media::AdapterStreamCallback;

namespace OHOS {
namespace Media {
const int32_t INVALID_MEDIA_POSITION = -1;
const int32_t DEFAULT_REWIND_TIME = 0;
const int32_t IDLE_QUEQUE_SLEEP_TIME_US = 5000;
const float MAX_MEDIA_VOLUME = 300.0f;
const int32_t POS_NOTIFY_INTERVAL_MS = 300;

#define CHECK_FAILED_PRINT(value, target, printfString) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s", printfString ? printfString : " "); \
    } \
} while (0)

#define CHECK_FAILED_RETURN(value, target, ret, printfString) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, ret:%d", printfString ? printfString : " ", ret); \
        return ret; \
    } \
} while (0)

#define CHK_NULL_RETURN(ptr) \
do { \
    if (ptr == nullptr) { \
        MEDIA_ERR_LOG("ptr null"); \
        return -1; \
    } \
} while (0)

PlayerImpl::PlayerImpl()
    : player_(nullptr), speed_(1.0), playerControlState_(PLAY_STATUS_IDLE),
      isSingleLoop_(false),
      currentPosition_(0),
      rewindPosition_(INVALID_MEDIA_POSITION),
      surface_(nullptr),
      currentState_(PLAYER_IDLE),
      rewindMode_(PLAYER_SEEK_PREVIOUS_SYNC),
      currentRewindMode_(PLAYER_SEEK_PREVIOUS_SYNC),
      audioStreamType_(0),
      callback_(nullptr),
      inited_ (false),
      released_(false),
      isStreamSource_(false),
      bufferSource_(nullptr),
      streamCallback_(nullptr),
      pauseAfterPlay_(false),
      extraRewind_(false)
{
    (void)memset_s(&formatFileInfo_, sizeof(formatFileInfo_), 0, sizeof(FormatFileInfo));
    formatFileInfo_.s32UsedVideoStreamIndex = -1;
    formatFileInfo_.s32UsedAudioStreamIndex = -1;
    formatFileInfo_.s64Duration = -1;
    buffer_.idx = -1;
    buffer_.flag = 0;
    buffer_.offset = 0;
    buffer_.size = 0;
    buffer_.timestamp = 0;
    (void)memset_s(&mediaAttr_, sizeof(mediaAttr_), 0, sizeof(PlayerControlStreamAttr));
}

int32_t PlayerImpl::Init(void)
{
    if (inited_ == true) {
        return 0;
    }
    int ret = CodecInit();
    if (ret != 0) {
		MEDIA_ERR_LOG("PlayerImpl::Init CodecInit err\n");
        return ret;
    }
    if (memset_s(&buffer_, sizeof(QueBuffer), 0, sizeof(QueBuffer)) != EOK) {
        MEDIA_ERR_LOG("PlayerImpl::Init memset err\n");
    }
    buffer_.idx = -1;
    inited_ = true;
    MEDIA_INFO_LOG("process success");
    return 0;
}

int32_t PlayerImpl::DeInit(void)
{
    if (inited_ != true) {
        return 0;
    }
    if (released_ == false) {
        Release();
    }
    inited_ = false;
    return 0;
}

PlayerImpl::~PlayerImpl()
{
    DeInit();
    player_ = nullptr;
    MEDIA_INFO_LOG("~PlayerImpl process");
}

int32_t PlayerImpl::SetSource(const Source &source)
{
    Init();
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ != PLAYER_IDLE) {
        MEDIA_ERR_LOG("failed, current state is:%d", currentState_);
        return -1;
    }

    SourceType sType = source.GetSourceType();
    if (sType == SourceType::SOURCE_TYPE_FD) {
        MEDIA_ERR_LOG("not support fdSource now");
        return -1;
    }

    GetPlayer();
    CHK_NULL_RETURN(player_);

    int32_t ret = -1;
    if (sType == SourceType::SOURCE_TYPE_URI) {
        ret = SetUriSource(source);
    } else if (sType == SourceType::SOURCE_TYPE_STREAM) {
        ret = SetStreamSource(source);
    } else {
        MEDIA_ERR_LOG("SetSource failed, source type is %d", static_cast<int32_t>(sType));
    }

    if (ret != 0) {
        ResetInner();
    }
    return ret;
}

static void ShowFileInfo(const FormatFileInfo *fileInfo)
{
    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        const StreamResolution *resolution = &fileInfo->stSteamResolution[i];
        if (resolution->u32Width == 0 || resolution->u32Height == 0) {
            break;
        }
        MEDIA_INFO_LOG("video[%d],w=%u,h=%u,index=%d ", i, resolution->u32Width,
            resolution->u32Height, resolution->s32VideoStreamIndex);
    }
    MEDIA_INFO_LOG("audio channel_cnt=%u,sampleRate=%u,AudioStreamIndex=%d videoIndex:%d",
        fileInfo->u32AudioChannelCnt, fileInfo->u32SampleRate, fileInfo->s32UsedAudioStreamIndex,
        fileInfo->s32UsedVideoStreamIndex);
}

void PlayerImpl::UpdateState(PlayerImpl *curPlayer, PlayerStatus state)
{
    if (curPlayer == nullptr) {
        return;
    }

    curPlayer->playerControlState_ = state;
    MEDIA_INFO_LOG("player UpdateState, state:%d", state);
}

void PlayerImpl::PlayerControlEventCb(void* pPlayer, PlayerControlEvent enEvent, const void* pData)
{
    PlayerControlError subErr = PLAYERCONTROL_ERROR_BUTT;
    PlayerImpl *curPlayer = (PlayerImpl *)pPlayer;

    if (curPlayer == nullptr) {
        MEDIA_ERR_LOG("the handle is error");
        return;
    }
    switch (enEvent) {
        case PLAYERCONTROL_EVENT_STATE_CHANGED:
            if (pData == nullptr) {
                return;
            }
            curPlayer->UpdateState(curPlayer, *reinterpret_cast<const PlayerStatus *>(pData));
            break;
        case PLAYERCONTROL_EVENT_EOF:
            MEDIA_INFO_LOG("end of file");
            curPlayer->NotifyPlaybackComplete(curPlayer);
            break;
        case PLAYERCONTROL_EVENT_SOF:
            MEDIA_INFO_LOG("start of file");
            break;
        case PLAYERCONTROL_EVENT_ERROR:
            if (pData == nullptr) {
                return;
            }
            subErr = *reinterpret_cast<const PlayerControlError *>(pData);
            MEDIA_ERR_LOG("error: %d", subErr);
            if (curPlayer->callback_ != nullptr) {
                curPlayer->callback_->OnError(0, subErr);
            }
            break;
        case PLAYERCONTROL_EVENT_PROGRESS:
            if (pData == nullptr) {
                return;
            }
            curPlayer->currentPosition_ = *reinterpret_cast<const int64_t *>(pData);
            break;
        case PLAYERCONTROL_EVENT_SEEK_END:
            if (pData == nullptr) {
                return;
            }
            MEDIA_INFO_LOG("seek action end, time is %lld",  *reinterpret_cast<const int64_t *>(pData));
            curPlayer->NotifySeekComplete(curPlayer, *reinterpret_cast<const int64_t *>(pData));
            break;
        case PLAYERCONTROL_FIRST_VIDEO_FRAME:
            MEDIA_INFO_LOG("render FirstVideoFrame");
            if (curPlayer->pauseAfterPlay_) {
                curPlayer->player_->Pause();
            }
            break;
        case PLAYERCONTROL_FIRST_AUDIO_FRAME:
            if (curPlayer->pauseAfterPlay_ && curPlayer->formatFileInfo_.s32UsedVideoStreamIndex == -1) {
                curPlayer->player_->Pause();
            }
            break;
        default:
            break;
    }
}

void PlayerImpl::ReportVideoSizeChange(void)
{
    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        if (formatFileInfo_.stSteamResolution[i].s32VideoStreamIndex == formatFileInfo_.s32UsedVideoStreamIndex) {
            if (callback_ != nullptr) {
                callback_->OnVideoSizeChanged(formatFileInfo_.stSteamResolution[i].u32Width,
                    formatFileInfo_.stSteamResolution[i].u32Height);
            }
            break;
        }
    }
}

int32_t PlayerImpl::Prepare()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    CHK_NULL_RETURN(player_);

    if (currentState_ == PLAYER_PREPARED) {
        MEDIA_ERR_LOG("have operated prepare before");
        return 0;
    }
    if (currentState_ != PLAYER_INITIALIZED) {
        MEDIA_ERR_LOG("Can not Prepare, currentState_ is %d", currentState_);
        return -1;
    }

    PlayerCtrlCallbackParam param;
    param.player = this;
    param.callbackFun = PlayerControlEventCb;
    int ret = player_->RegCallback(param);
    if (ret != 0) {
        MEDIA_ERR_LOG("RegCallback exec failed ");
        return -1;
    }

    currentState_ = PLAYER_PREPARING;
    ret = player_->Prepare();
    if (ret != 0) {
        MEDIA_ERR_LOG("Prepare exec failed");
        currentState_ = PLAYER_INITIALIZED;
        return -1;
    }
    currentState_ = PLAYER_PREPARED;
    (void)player_->SetAudioStreamType(audioStreamType_);

    ret = player_->GetFileInfo(formatFileInfo_);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetFileInfo failed");
        return ret;
    }
    ShowFileInfo(&formatFileInfo_);
    if (formatFileInfo_.s32UsedVideoStreamIndex == -1) {
        MEDIA_INFO_LOG("process out");
        return 0;
    }

    /* report video solution */
    ReportVideoSizeChange();
    MEDIA_INFO_LOG("process out");
    return 0;
}

int32_t PlayerImpl::SetMediaStream(void)
{
    int32_t ret;

    mediaAttr_.s32VidStreamId = formatFileInfo_.s32UsedVideoStreamIndex;
    mediaAttr_.s32AudStreamId = formatFileInfo_.s32UsedAudioStreamIndex;
    ret = player_->SetMedia(mediaAttr_);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetMedia  exec failed");
        return  ret;
    }

    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        StreamResolution *resolution = &formatFileInfo_.stSteamResolution[i];
        if (resolution->s32VideoStreamIndex == mediaAttr_.s32VidStreamId) {
            MEDIA_INFO_LOG("used video w=%u,h=%u,index=%d",
                resolution->u32Width, resolution->u32Height, mediaAttr_.s32VidStreamId);
            break;
        }
    }
    return 0;
}

int32_t PlayerImpl::Play()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    int ret;
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    CHK_NULL_RETURN(player_);
    if (currentState_ == PLAYER_STARTED) {
        MEDIA_INFO_LOG("no need to repeat operation");
        return 0;
    }
    if (currentState_ != PLAYER_PREPARED && currentState_ != PLAYER_PAUSED) {
        MEDIA_ERR_LOG("Can not Play, currentState is %d", currentState_);
        return -1;
    }
    if (currentState_ == PLAYER_PAUSED) {
        goto play;
    }

    ret = SetMediaStream();
    CHECK_FAILED_RETURN(ret, 0, ret, "SetMeidaStream failed");
play:
    ret = player_->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play exec failed %x", ret);
        return -1;
    }
    currentState_ = PLAYER_STARTED;
    MEDIA_INFO_LOG("process out");
    return 0;
}

bool PlayerImpl::IsPlaying()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, 0, "have released or not create");
    bool isPlaying = false;
    if (player_ != nullptr) {
        isPlaying = (currentState_ != PLAYER_STARTED) ? false : true;
    }
    return isPlaying;
}

int32_t PlayerImpl::Pause()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    CHK_NULL_RETURN(player_);
    if (currentState_ == PLAYER_PAUSED) {
        MEDIA_ERR_LOG("currentState_ is %d", currentState_);
        return 0;
    }
    if (speed_ != 1.0) {
        MEDIA_ERR_LOG("failed, currentState_ speed is %f", speed_);
        return -1;
    }
    if (pauseAfterPlay_ && currentState_ == PLAYER_PREPARED) {
        int32_t ret;
        ret = SetMediaStream();
        CHECK_FAILED_RETURN(ret, 0, ret, "SetMeidaStream failed");
        ret = player_->Play();
        CHECK_FAILED_RETURN(ret, 0, ret, "Play failed");
        currentState_ = PLAYER_PAUSED;
        return 0;
    }
    if (currentState_ != PLAYER_STARTED) {
        MEDIA_ERR_LOG("Can not Pause, currentState_ is %d", currentState_);
        return -1;
    }

    player_->Pause();
    currentState_ = PLAYER_PAUSED;
    return 0;
}

int32_t PlayerImpl::Stop()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ == PLAYER_STOPPED) {
        return 0;
    }
    if ((currentState_ != PLAYER_STARTED) && (currentState_ != PLAYER_PAUSED) &&
        (currentState_ != PLAYER_PLAYBACK_COMPLETE) && (currentState_ != PLAYER_STATE_ERROR)) {
        MEDIA_INFO_LOG("current state: %d, no need to do stop", currentState_);
        return -1;
    }

    if (player_ != nullptr) {
        int32_t ret = player_->Stop();
        if (ret != 0) {
            MEDIA_ERR_LOG("Stop failed, ret is %d", ret);
        }
    }
    currentState_ = PLAYER_STOPPED;
    MEDIA_INFO_LOG("process out");
    return 0;
}

int32_t PlayerImpl::RewindInner(int64_t mSeconds, PlayerSeekMode mode)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    if (mSeconds < DEFAULT_REWIND_TIME) {
        MEDIA_WARNING_LOG("Attempt to rewind to invalid position %lld", mSeconds);
        mSeconds = DEFAULT_REWIND_TIME;
    }
    int32_t ret;
    int64_t durationMs = -1;
    GetDurationInner(durationMs);
    if ((durationMs > DEFAULT_REWIND_TIME) && (mSeconds > durationMs)) {
        MEDIA_WARNING_LOG("Attempt to rewind to past end of file, request is %lld, durationMs is %lld", mSeconds,
            durationMs);
        return -1;
    }
    currentRewindMode_ = mode;
    if (rewindPosition_ >= DEFAULT_REWIND_TIME) {
        rewindPosition_ = mSeconds;
        MEDIA_WARNING_LOG("is deal last rewind time:%lld", mSeconds);
        return 0;
    }

    rewindPosition_ = mSeconds;
    rewindMode_ = mode;
    ret = player_->Seek(mSeconds);
    if (ret != 0) {
        MEDIA_ERR_LOG("do seek failed, ret is %d", ret);
    }
    MEDIA_INFO_LOG("process out");
    return ret;
}

bool PlayerImpl::IsValidRewindMode(PlayerSeekMode mode)
{
    switch (mode) {
        case PLAYER_SEEK_PREVIOUS_SYNC:
        case PLAYER_SEEK_NEXT_SYNC:
        case PLAYER_SEEK_CLOSEST_SYNC:
        case PLAYER_SEEK_CLOSEST:
            break;
        default:
            MEDIA_ERR_LOG("Unknown rewind mode %d", mode);
            return false;
    }
    return true;
}

int32_t PlayerImpl::Rewind(int64_t mSeconds, int32_t mode)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ != PLAYER_STARTED && currentState_ != PLAYER_PAUSED && currentState_ != PLAYER_PREPARED) {
        MEDIA_ERR_LOG("Can not Rewind, currentState_ is %d", currentState_);
        return -1;
    }
    if (speed_ != 1.0) {
        MEDIA_ERR_LOG("Can not Rewind, currentState_ speed is %f", speed_);
        return -1;
    }

    if (IsValidRewindMode((PlayerSeekMode)mode) != true) {
        MEDIA_ERR_LOG("Rewind failed, msec is %lld, mode is %d", mSeconds, mode);
        return -1;
    }

    if (isStreamSource_ == true) {
        MEDIA_ERR_LOG("Failed, streamsource not support Rewind");
        return -1;
    }

    std::lock_guard<std::mutex> rewindLock(rewindLock_);
    int32_t ret = RewindInner(mSeconds, (PlayerSeekMode)mode);
    if (ret != 0) {
        MEDIA_ERR_LOG("ReWind failed, ret is %d", ret);
    } else {
        currentPosition_ = mSeconds;
        extraRewind_ = true;
    }
    MEDIA_INFO_LOG("process out");
    return ret;
}

int32_t PlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    VolumeAttr attr;
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    CHK_NULL_RETURN(player_);
    if ((currentState_ != PLAYER_STARTED) && (currentState_ != PLAYER_PAUSED) &&
        (currentState_ != PLAYER_PREPARED) && (currentState_ != PLAYER_INITIALIZED)) {
        MEDIA_ERR_LOG("SetVolume failed, currentState_ is %d", currentState_);
        return -1;
    }
    if (leftVolume < 0 || leftVolume > MAX_MEDIA_VOLUME || rightVolume < 0 || rightVolume > MAX_MEDIA_VOLUME) {
        MEDIA_ERR_LOG("SetVolume failed, the volume should be set to a value ranging from 0 to 300");
        return -1;
    }
    attr.leftVolume = leftVolume;
    attr.rightVolume = rightVolume;
    int ret = player_->SetVolume(attr);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVolume failed %x", ret);
    }
    MEDIA_INFO_LOG("process out\n");
    return ret;
}

int32_t PlayerImpl::SetSurface(Surface *surface)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if ((currentState_ != PLAYER_PREPARED) && (currentState_ != PLAYER_INITIALIZED)) {
        MEDIA_ERR_LOG("SetSurface failed, currentState_ is %d", currentState_);
        return -1;
    }
    surface_ = surface;
    player_->SetSurface(surface);
    return 0;
}

bool PlayerImpl::IsSingleLooping()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, false, "have released or not create");
    bool isLoop = (player_ == nullptr) ? false : isSingleLoop_;
    return isLoop;
}

int32_t PlayerImpl::GetPlayerState(int32_t &state)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    state = currentState_;
    return 0;
}

int32_t PlayerImpl::GetCurrentPosition(int64_t &position)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    position = (currentPosition_ >= 0) ? currentPosition_ : 0;
    return 0;
}

void PlayerImpl::GetDurationInner(int64_t &durationMs)
{
    durationMs = formatFileInfo_.s64Duration;
}

int32_t PlayerImpl::GetDuration(int64_t &durationMs)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ == PLAYER_IDLE || currentState_ == PLAYER_INITIALIZED) {
        durationMs = 0;
        return 0;
    }
    GetDurationInner(durationMs);
    return 0;
}

int32_t PlayerImpl::GetVideoWidth(int32_t &videoWidth)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");

    videoWidth = 0;
    if (currentState_ != PLAYER_PREPARED && currentState_ != PLAYER_STARTED && currentState_ != PLAYER_PAUSED &&
        currentState_ != PLAYER_STOPPED && currentState_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_ERR_LOG("Can not GetVideoWidth, currentState_ is %d", currentState_);
        return -1;
    }

    if (formatFileInfo_.s32UsedVideoStreamIndex == -1) {
        return -1;
    }
    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        if (formatFileInfo_.stSteamResolution[i].s32VideoStreamIndex == formatFileInfo_.s32UsedVideoStreamIndex) {
            videoWidth = formatFileInfo_.stSteamResolution[i].u32Width;
            break;
        }
    }
    return 0;
}

int32_t PlayerImpl::GetVideoHeight(int32_t &videoHeight)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");

    videoHeight = 0;
    if (currentState_ != PLAYER_PREPARED && currentState_ != PLAYER_STARTED && currentState_ != PLAYER_PAUSED &&
        currentState_ != PLAYER_STOPPED && currentState_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_ERR_LOG("Can not GetVideoHeight, currentState_ is %d", currentState_);
        return -1;
    }
    if (formatFileInfo_.s32UsedVideoStreamIndex == -1) {
        return -1;
    }

    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        if (formatFileInfo_.stSteamResolution[i].s32VideoStreamIndex == formatFileInfo_.s32UsedVideoStreamIndex) {
            videoHeight = formatFileInfo_.stSteamResolution[i].u32Height;
            break;
        }
    }
    return 0;
}

static int32_t PlayerControlCheckTPlayAttr(const float speed, TplayDirect direction)
{
    if ((direction != TPLAY_DIRECT_BACKWARD) && (direction != TPLAY_DIRECT_FORWARD)) {
        return HI_FAILURE;
    }

    if (speed != PLAY_SPEED_2X_FAST &&
        speed != PLAY_SPEED_4X_FAST &&
        speed != PLAY_SPEED_8X_FAST &&
        speed != PLAY_SPEED_16X_FAST &&
        speed != PLAY_SPEED_32X_FAST &&
        speed != PLAY_SPEED_64X_FAST &&
        speed != PLAY_SPEED_128X_FAST) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

int32_t PlayerImpl::SetPlaybackSpeed(float speed)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (speed <= 0.f) {
        return -1;
    }
    CHK_NULL_RETURN(player_);
    if (currentState_ != PLAYER_STARTED) {
        MEDIA_ERR_LOG("currentState_ is %d", currentState_);
        return -1;
    }
    if (speed == speed_) {
        return 0;
    }
    if ((isStreamSource_ == true) && (speed != 1.0f)) {
        MEDIA_ERR_LOG("Failed, streamsource not support abnormal speed");
        return -1;
    }

    if (formatFileInfo_.s32UsedVideoStreamIndex == -1) {
        MEDIA_ERR_LOG("audio movie not support abnormal speed");
        return -1;
    }

    if (speed_ > 1 && speed == 1) { // tplay  ---> play
        CHECK_FAILED_PRINT(player_->Play(), 0, "player failed");
    } else { // normal/tplay -->tplay
        TplayAttr tplayAttr;
        TplayDirect direction = (speed > 0) ? TPLAY_DIRECT_FORWARD : TPLAY_DIRECT_BACKWARD;
        float speedTmp = (speed > 0) ? speed : -speed;
        tplayAttr.direction = direction;
        tplayAttr.speed = speedTmp;
        if (PlayerControlCheckTPlayAttr(speedTmp, direction) != 0) {
            MEDIA_ERR_LOG("not support speed:%f", speed);
            return -1;
        }
        CHECK_FAILED_PRINT(player_->TPlay(tplayAttr), 0, "tplayer failed");
    }

    if (speed == 0.f && currentState_ == PLAYER_STARTED) {
        currentState_ = PLAYER_PAUSED;
    } else if (speed != 0.f && (currentState_ == PLAYER_PREPARED || currentState_ == PLAYER_PAUSED)) {
        currentState_ = PLAYER_STARTED;
    }
    speed_ = speed;
    return 0;
}

int32_t PlayerImpl::GetPlaybackSpeed(float &speed)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    speed = (currentState_ != PLAYER_PAUSED) ? speed_ : 0;
    return 0;
}

int32_t PlayerImpl::SetAudioStreamType(int32_t type)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ != PLAYER_IDLE && currentState_ != PLAYER_INITIALIZED) {
        MEDIA_ERR_LOG("failed, state %d,type:%d", currentState_, type);
        return -1;
    }
    audioStreamType_ = type;

    if (player_ != nullptr) {
        player_->SetAudioStreamType(type);
    }
    return 0;
}

void PlayerImpl::GetAudioStreamType(int32_t &type)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        return;
    }
    type = static_cast<int32_t>(audioStreamType_);
}

void PlayerImpl::ResetInner(void)
{
    isSingleLoop_ = false;
    if (player_ != nullptr) {
        if (currentState_ != PLAYER_IDLE && currentState_ != PLAYER_STOPPED) {
            CHECK_FAILED_PRINT(player_->Stop(), HI_SUCCESS, "stop failed");
        }
        (void)player_->Deinit();
        player_.reset();
        player_ = nullptr;
    }
    if (bufferSource_ != nullptr) {
        bufferSource_.reset();
        bufferSource_ = nullptr;
    }
    if (streamCallback_ != nullptr) {
        streamCallback_.reset();
        streamCallback_ = nullptr;
    }

    currentState_ = PLAYER_IDLE;
    currentRewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    rewindPosition_ = INVALID_MEDIA_POSITION;
    rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    isSingleLoop_ = false;
    speed_ = 1.0;
    currentPosition_ = 0;
    pauseAfterPlay_ = false;
    extraRewind_ = false;
    playerControlState_ = PLAY_STATUS_IDLE;
    isStreamSource_ = false;
    (void)memset_s(&formatFileInfo_, sizeof(FormatFileInfo), 0, sizeof(FormatFileInfo));
    formatFileInfo_.s32UsedVideoStreamIndex = -1;
    formatFileInfo_.s32UsedAudioStreamIndex = -1;
    formatFileInfo_.s64Duration = -1;
    (void)memset_s(&mediaAttr_, sizeof(PlayerControlStreamAttr), 0, sizeof(PlayerControlStreamAttr));
    (void)memset_s(&buffer_, sizeof(QueBuffer), 0, sizeof(QueBuffer));
    buffer_.idx = -1;
}

int32_t PlayerImpl::Reset(void)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    if (currentState_ == PLAYER_IDLE) {
        return 0;
    }
    ResetInner();
    return 0;
}

int32_t PlayerImpl::Release()
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    CHECK_FAILED_RETURN(released_, false, 0, "have released or not create");
    ResetInner();

    if (callback_ != nullptr) {
        callback_.reset();
        callback_ = nullptr;
    }
    currentState_ = PLAYER_STATE_ERROR;
    released_ = true;
    return 0;
}

int PlayerImpl::CreatePlayerParamCheck(PlayerControlParam &createParam)
{
    if (createParam.u32PlayPosNotifyIntervalMs < MIN_NOTIFY_INTERVAL_MS
        && createParam.u32PlayPosNotifyIntervalMs > 0) {
        MEDIA_ERR_LOG("notify interval small than min value %d", MIN_NOTIFY_INTERVAL_MS);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    if ((createParam.u32VideoEsBufSize < AV_ESBUF_SIZE_MIN && createParam.u32VideoEsBufSize > 0)
        || createParam.u32VideoEsBufSize > VIDEO_ESBUF_SIZE_LIMIT) {
        MEDIA_ERR_LOG("video esbuffer illegal %u", createParam.u32VideoEsBufSize);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    if ((createParam.u32AudioEsBufSize < AV_ESBUF_SIZE_MIN && createParam.u32AudioEsBufSize > 0)
        || createParam.u32AudioEsBufSize > AUDIO_ESBUF_SIZE_LIMIT) {
        MEDIA_ERR_LOG("audio esbuffer illegal %u", createParam.u32VideoEsBufSize);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    if ((createParam.u32VdecFrameBufCnt < VDEC_VBBUF_CONUT_MIN) &&
        (createParam.u32VdecFrameBufCnt != 0)) {
        MEDIA_ERR_LOG("VDEC vb buffer count %u small than %d", createParam.u32VdecFrameBufCnt, VDEC_VBBUF_CONUT_MIN);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    return 0;
}

int PlayerImpl::GetPlayer()
{
    MEDIA_INFO_LOG("process in");
    PlayerControlParam playerParam;
    if (player_ != nullptr) {
        return 0;
    }
    if (memset_s(&playerParam, sizeof(PlayerControlParam), 0x00, sizeof(playerParam)) != EOK) {
        MEDIA_INFO_LOG("memset_s playerParam failed");
        return -1;
    }

    playerParam.u32PlayPosNotifyIntervalMs = POS_NOTIFY_INTERVAL_MS;
    if (CreatePlayerParamCheck(playerParam) != 0) {
        MEDIA_ERR_LOG("CreatePlayerParamCheck failed");
        return -1;
    }
    player_ = std::make_shared<PlayerControl>();
    if (player_ == nullptr || player_.get() == nullptr) {
        MEDIA_ERR_LOG("playerControl new failed");
        return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
    }
    if (player_->Init(playerParam) != HI_SUCCESS) {
        MEDIA_ERR_LOG("playerControl init failed");
        return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
    }
    currentState_ = PLAYER_INITIALIZED;
    MEDIA_INFO_LOG("GetPlayer success");
    return 0;
}

void PlayerImpl::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    MEDIA_INFO_LOG("process in");
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        return;
    }
    callback_ = cb;
}

void PlayerImpl::NotifyPlaybackComplete(PlayerImpl *curPlayer)
{
    if (!isSingleLoop_) {
        if (curPlayer->formatFileInfo_.s64Duration == -1) {
            curPlayer->formatFileInfo_.s64Duration = curPlayer->currentPosition_;
        }
        curPlayer->currentState_ = PLAYER_PLAYBACK_COMPLETE;
        MEDIA_INFO_LOG("OnPlayBackComplete, iscallbackNull:%d", (curPlayer->callback_ == nullptr));
        if (curPlayer != nullptr && curPlayer->callback_ != nullptr) {
            curPlayer->callback_->OnPlaybackComplete();
        }
        return;
    }
    std::lock_guard<std::mutex> valueLock(curPlayer->rewindLock_);
    (void)curPlayer->RewindInner(0, PLAYER_SEEK_PREVIOUS_SYNC);
    curPlayer->currentPosition_ = 0;
    curPlayer->extraRewind_ = false;
}

void PlayerImpl::NotifySeekComplete(PlayerImpl *curPlayer, int64_t seekToMs)
{
    std::lock_guard<std::mutex> valueLock(curPlayer->rewindLock_);

    if (curPlayer->rewindPosition_ != -1 && curPlayer->rewindPosition_ != seekToMs) {
        int64_t seekTime = curPlayer->rewindPosition_;
        curPlayer->rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
        curPlayer->rewindPosition_ = -1;
        curPlayer->RewindInner(seekTime, curPlayer->currentRewindMode_);
        return;
    }

    curPlayer->currentRewindMode_ = curPlayer->rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    curPlayer->rewindPosition_ = -1;
    if (curPlayer->callback_ != nullptr && extraRewind_) {
        extraRewind_ = false;
        curPlayer->callback_->OnRewindToComplete();
    }
}

int32_t PlayerImpl::SetLoop(bool loop)
{
    std::lock_guard<std::mutex> valueLock(lock_);
    CHECK_FAILED_RETURN(released_, false, -1, "have released or not create");
    CHECK_FAILED_RETURN(isStreamSource_, false, -1, "stream source not support loop player");
    if (currentState_ == PLAYER_STOPPED || currentState_ == PLAYER_PLAYBACK_COMPLETE || currentState_ == PLAYER_IDLE) {
        MEDIA_ERR_LOG("currentState_ is %d", currentState_);
        return -1;
    }
    isSingleLoop_ = loop;
    return 0;
}

int32_t PlayerImpl::SetUriSource(const Source &source)
{
    MEDIA_INFO_LOG("process in");
    const std::string uri = source.GetSourceUri();
    if (uri.empty() || uri.c_str() == nullptr) {
        MEDIA_ERR_LOG("SetUriSource failed, uri source do not set uri parameter");
        return -1;
    }
    char filePath[PATH_MAX];
    if (realpath(uri.c_str(), filePath) == nullptr) {
        MEDIA_ERR_LOG("Realpath input file failed");
        return -1;
    }
    if (access(filePath, R_OK) == -1) {
        MEDIA_ERR_LOG("No permission to read the file");
        return -1;
    }
    int32_t ret = player_->SetDataSource(uri.c_str());
    if (ret != 0) {
        MEDIA_ERR_LOG("SetSource failed, ret is %d, uri is %s", ret, uri.c_str());
        return ret;
    }
    return 0;
}

AdapterStreamCallback::AdapterStreamCallback(std::shared_ptr<StreamSource> &stream,
    std::shared_ptr<BufferSource> &buffer)
    : streamProcess_(0),
      isRunning_(false)
{
    streamSource_ = stream;
    bufferSource_ = buffer;
    pthread_mutex_init(&mutex_, nullptr);
}

AdapterStreamCallback::~AdapterStreamCallback(void)
{
    DeInit();
    MEDIA_INFO_LOG("process out");
}

void *AdapterStreamCallback::IdleBufferProcess(void *arg)
{
    int ret;
    QueBuffer buffer;
    BufferInfo info;
    if (memset_s(&info, sizeof(info), 0x00, sizeof(info)) != EOK) {
        return nullptr;
    }
    AdapterStreamCallback *process = (AdapterStreamCallback*)arg;
    if (process == nullptr) {
        return nullptr;
    }

    prctl(PR_SET_NAME, "IdlbufProc", 0, 0, 0);
    MEDIA_INFO_LOG("process start");
    while (true) {
        pthread_mutex_lock(&process->mutex_);
        if (process->isRunning_ == false) {
            pthread_mutex_unlock(&process->mutex_);
            break;
        }
        pthread_mutex_unlock(&process->mutex_);
        if (process->bufferSource_ == nullptr) {
            MEDIA_ERR_LOG("bufferSource_ null break");
            break;
        }
        if (process->bufferSource_->GetIdleQueSize() == 0) {
            usleep(IDLE_QUEQUE_SLEEP_TIME_US);
            continue;
        }
        ret = process->bufferSource_->DequeIdleBuffer(&buffer, 0);
        if (ret == 0) {
            process->bufferSource_->GetBufferInfo(buffer.idx, &info);
            std::shared_ptr<StreamSource> stream = process->streamSource_.lock();
            if (stream == nullptr) {
                MEDIA_ERR_LOG("stream not exist break");
                break;
            }
            stream->OnBufferAvailable(buffer.idx, 0, info.bufLen);
        }
    };
    pthread_mutex_lock(&process->mutex_);
    process->isRunning_ = false;
    pthread_mutex_unlock(&process->mutex_);
    MEDIA_INFO_LOG("work end");
    return nullptr;
}

int AdapterStreamCallback::Init(void)
{
    MEDIA_INFO_LOG("process in");
    pthread_mutex_lock(&mutex_);
    isRunning_ = true;
    pthread_mutex_unlock(&mutex_);
    int32_t ret = pthread_create(&streamProcess_, nullptr, IdleBufferProcess, this);
    if (ret != 0) {
        MEDIA_ERR_LOG("pthread_create failed %d", ret);
        pthread_mutex_lock(&mutex_);
        isRunning_ = false;
        pthread_mutex_unlock(&mutex_);
        return -1;
    }
    return 0;
}

void AdapterStreamCallback::DeInit(void)
{
    MEDIA_INFO_LOG("process in");
    pthread_mutex_lock(&mutex_);
    isRunning_ = false;
    pthread_mutex_unlock(&mutex_);
    if (streamProcess_ != 0) {
        pthread_join(streamProcess_, nullptr);
    }
    pthread_mutex_destroy(&mutex_);
}

uint8_t *AdapterStreamCallback::GetBuffer(size_t index)
{
    BufferInfo info;
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return nullptr;
    }
    if (bufferSource_->GetBufferInfo(index, &info) != 0) {
        MEDIA_ERR_LOG("GetBufferInfo failed");
        return nullptr;
    }
    return (uint8_t*)info.virAddr;
}

void AdapterStreamCallback::QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags)
{
    QueBuffer buffer;
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return;
    }

    buffer.idx = index;
    buffer.flag = static_cast<int32_t>(flags);
    buffer.offset = offset;
    buffer.size = size;
    buffer.timestamp = timestampUs;
    if (bufferSource_->QueFilledBuffer(&buffer) != 0) {
        MEDIA_ERR_LOG("QueFilledBuffer failed");
    }
}

void AdapterStreamCallback::SetParameters(const Format &params)
{
    MEDIA_ERR_LOG("process, not support");
}

int32_t PlayerImpl::GetReadableSize(const void *handle)
{
    const PlayerImpl *playImpl = (const PlayerImpl*)handle;
    CHK_NULL_RETURN(playImpl);
    if (playImpl->bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return -1;
    }
    return playImpl->bufferSource_->GetFilledQueDataSize();
}

int32_t PlayerImpl::ReadDataPro(uint8_t *data, int32_t size, DataFlags &flags)
{
    int readLen;
    BufferInfo info;
    if (bufferSource_->GetBufferInfo(buffer_.idx, &info) != 0) {
        return 0;
    }
    /* read all buffer data */
    if (buffer_.size <= size) {
        if (buffer_.size == 0 && buffer_.flag == BUFFER_FLAG_EOS) {
            buffer_.offset = 0;
            buffer_.size = info.size;
            bufferSource_->QueIdleBuffer(&buffer_);
            buffer_.idx = -1;
            flags = DATA_FLAG_EOS;
            return 0;
        }
        if (memcpy_s(data, size, (unsigned char*)(info.virAddr) + buffer_.offset, buffer_.size) != EOK) {
            return -1;
        }
        flags = (buffer_.flag == BUFFER_FLAG_EOS) ? DATA_FLAG_EOS : DATA_FLAG_PARTIAL_FRAME;
        readLen = buffer_.size;
        buffer_.offset = 0;
        buffer_.size = info.size;
        bufferSource_->QueIdleBuffer(&buffer_);
        buffer_.idx = -1;
    } else {
        if (memcpy_s(data, size, (unsigned char*)(info.virAddr) + buffer_.offset, size) != EOK) {
            return -1;
        }
        buffer_.offset += size;
        buffer_.size -= size;
        flags = DATA_FLAG_PARTIAL_FRAME;
        readLen = size;
    }
    return readLen;
}

int32_t PlayerImpl::ReadData(void *handle, uint8_t *data, int32_t size, int32_t timeOutMs, DataFlags *flags)
{
    PlayerImpl *playImpl = (PlayerImpl*)handle;

    if (playImpl == nullptr || playImpl->bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return -1;
    }
    if (data == nullptr || size < 0  || flags == nullptr) {
        MEDIA_ERR_LOG("data null or buffer size < 0");
        return -1;
    }

    if (playImpl->buffer_.idx != -1) {
        return playImpl->ReadDataPro(data, size, *flags);
    }

    if (playImpl->bufferSource_->GetFilledQueSize() <= 0) {
        return 0;
    }
    if (playImpl->bufferSource_->DequeFilledBuffer(&playImpl->buffer_, 0) != 0) {
        playImpl->buffer_.idx = -1;
        return 0;
    }

    return playImpl->ReadDataPro(data, size, *flags);
}

int32_t PlayerImpl::SetStreamSource(const Source &source)
{
    MEDIA_INFO_LOG("process in");

    isStreamSource_ = true;
    isSingleLoop_ = false;

    bufferSource_ = std::make_shared<BufferSource>();
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("new BufferSource failed");
        return -1;
    }

    bufferSource_->Init();
    std::shared_ptr<StreamSource> stream = source.GetSourceStream();
    if (stream.get() == nullptr) {
        MEDIA_ERR_LOG("GetSourceStream null");
        return -1;
    }

    streamCallback_ = std::make_shared<AdapterStreamCallback>(stream, bufferSource_);
    if (streamCallback_ == nullptr || streamCallback_.get() == nullptr) {
        MEDIA_ERR_LOG("new AdapterStreamCallback failed");
        return -1;
    }
    streamCallback_->Init();
    stream->SetStreamCallback(streamCallback_);

    BufferStream sourceTmp;
    sourceTmp.handle = this;
    sourceTmp.ReadData = ReadData;
    sourceTmp.GetReadableSize = GetReadableSize;
    int32_t ret = player_->SetDataSource(sourceTmp);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetDataSource  exec failed");
        return -1;
    }
    return 0;
}

int32_t PlayerImpl::EnablePauseAfterPlay(bool isPauseAfterPlay)
{
    if (currentState_ != PLAYER_IDLE && currentState_ != PLAYER_INITIALIZED) {
        MEDIA_ERR_LOG("currentState_ is %d", currentState_);
        return -1;
    }
    if (pauseAfterPlay_ == isPauseAfterPlay) {
        return 0;
    }

    pauseAfterPlay_ = isPauseAfterPlay;
    if (player_ != NULL) {
        return player_->Invoke(INVOKE_ENABLE_PAUSE_AFTER_PLAYER, &pauseAfterPlay_);
    }
    MEDIA_INFO_LOG("isPauseAfterPlay:%d", isPauseAfterPlay);
    return 0;
}

int32_t PlayerImpl::SetParameter(const Format &params)
{
    int32_t value;
    std::lock_guard<std::mutex> valueLock(lock_);

    if (params.GetIntValue(PAUSE_AFTER_PLAY, value) != true) {
        MEDIA_ERR_LOG("get pause after play failed");
        return -1;
    }
    if (value != 0 && value != 1) {
        MEDIA_ERR_LOG("pause after play flag error:%d", value);
        return -1;
    }

    return EnablePauseAfterPlay(value);
}
}  // namespace Media
}  // namespace OHOS