/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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
#ifdef RECORDER_SUPPORT
#define HST_LOG_TAG "HiRecorderImpl"

#include "hirecorder_impl.h"
#include <regex>
#include "foundation/osal/filesystem/file_system.h"
#include "ipc_skeleton.h"
#include "pipeline/factory/filter_factory.h"
#include "plugin/common/media_sink.h"
#include "plugin/common/plugin_time.h"
#include "recorder_utils.h"
#include "utils/steady_clock.h"

#ifndef WIN32
    #include <unistd.h> // include the dup function on Linux system
#endif


namespace OHOS {
namespace Media {
namespace Record {
using namespace Pipeline;

HiRecorderImpl::HiRecorderImpl() : fsm_(*this), curFsmState_(StateId::INIT)
{
    MEDIA_LOG_I("hiRecorderImpl ctor");
    FilterFactory::Instance().Init();
    muxer_ = FilterFactory::Instance().CreateFilterWithType<MuxerFilter>(
            "builtin.recorder.muxer", "muxer");
    outputSink_ = FilterFactory::Instance().CreateFilterWithType<OutputSinkFilter>(
            "builtin.recorder.output_sink", "output_sink");
    FALSE_RETURN(muxer_ != nullptr);
    FALSE_RETURN(outputSink_ != nullptr);
    pipeline_ = std::make_shared<PipelineCore>();
}

HiRecorderImpl::~HiRecorderImpl()
{
    fsm_.SendEvent(Intent::RESET);
    fsm_.Stop();
    MEDIA_LOG_D("dtor called.");
}

ErrorCode HiRecorderImpl::Init()
{
    if (initialized_.load()) {
        return ErrorCode::SUCCESS;
    }
    mediaStatStub_.Reset();
    pipeline_->Init(this, nullptr);
    ErrorCode ret = pipeline_->AddFilters({muxer_.get(), outputSink_.get()});
    if (ret == ErrorCode::SUCCESS) {
        ret = pipeline_->LinkFilters({muxer_.get(), outputSink_.get()});
    }
    FALSE_LOG(ret == ErrorCode::SUCCESS);
    if (ret == ErrorCode::SUCCESS) {
        fsm_.SetStateCallback(this);
        fsm_.Start();
        initialized_ = true;
    } else {
        pipeline_->UnlinkPrevFilters();
        pipeline_->RemoveFilterChain(muxer_.get());
    }
    return ret;
}

int32_t HiRecorderImpl::SetAudioSource(AudioSourceType source, int32_t& sourceId)
{
    MEDIA_LOG_I("SetAudioSource enter");
    PROFILE_BEGIN("SetAudioSource begin");
    sourceId = INVALID_SOURCE_ID;
    FALSE_RETURN_V(source != AudioSourceType::AUDIO_SOURCE_INVALID,
                   TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE));
    FALSE_RETURN_V(audioCount_ < static_cast<int32_t>(AUDIO_SOURCE_MAX_COUNT),
                   TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION));
    auto tempSourceId = SourceIdGenerator::GenerateAudioSourceId(audioCount_);
    auto ret = SetAudioSourceInternal(source, tempSourceId);
    if (ret == ErrorCode::SUCCESS) {
        audioCount_++;
        audioSourceId_ = tempSourceId;
        sourceId = static_cast<int32_t>(audioSourceId_);
    }
    PROFILE_END("SetAudioSource end.");
    return TransErrorCode(ret);
}

int32_t HiRecorderImpl::SetVideoSource(VideoSourceType source, int32_t& sourceId)
{
#ifdef VIDEO_SUPPORT
    MEDIA_LOG_I("SetVideoSource enter");
    PROFILE_BEGIN("SetVideoSource begin");
    sourceId = INVALID_SOURCE_ID;
    FALSE_RETURN_V(source != VideoSourceType::VIDEO_SOURCE_BUTT,
                   TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE));
    FALSE_RETURN_V(videoCount_ < static_cast<int32_t>(VIDEO_SOURCE_MAX_COUNT),
                   TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION));
    auto tempSourceId = SourceIdGenerator::GenerateVideoSourceId(videoCount_);
    auto ret = SetVideoSourceInternal(source, tempSourceId);
    if (ret == ErrorCode::SUCCESS) {
        videoCount_++;
        videoSourceId_ = tempSourceId;
        sourceId = videoSourceId_;
    }
    PROFILE_END("SetVideoSource end.");
    return TransErrorCode(ret);
#else
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
#endif
}

sptr<Surface> HiRecorderImpl::GetSurface(int32_t sourceId)
{
#ifdef VIDEO_SUPPORT
    FALSE_RETURN_V(SourceIdGenerator::IsVideo(sourceId) && sourceId == static_cast<int32_t>(videoSourceId_) &&
        videoCapture_ != nullptr, nullptr);
    Plugin::Any any;
    FALSE_RETURN_V(videoCapture_ ->GetParameter(static_cast<int32_t>(Plugin::Tag::VIDEO_SURFACE), any)
                   != ErrorCode::SUCCESS, nullptr);
    return any.SameTypeWith(typeid(sptr<Surface>)) ? Plugin::AnyCast<sptr<Surface>>(any) : nullptr;
#else
    return nullptr;
#endif
}

int32_t HiRecorderImpl::SetOutputFormat(OutputFormatType format)
{
    MEDIA_LOG_I("OutputFormatType enter, format = " PUBLIC_LOG_D32, static_cast<int32_t>(format));
    FALSE_RETURN_V(format != OutputFormatType::FORMAT_BUTT, TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION));
    FALSE_RETURN_V((audioCount_ + videoCount_) > static_cast<uint32_t>(0),
                   TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION));
    outputFormatType_ = format;
    auto ret = fsm_.SendEvent(Intent::SET_OUTPUT_FORMAT, outputFormatType_);
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("SetOutputFormat failed with error " PUBLIC_LOG_S, GetErrorName(ret));
    }
    return TransErrorCode(ret);
}

int32_t HiRecorderImpl::SetObs(const std::weak_ptr<IRecorderEngineObs>& obs)
{
    MEDIA_LOG_I("SetObs enter");
    obs_ = obs;
    return TransErrorCode(ErrorCode::SUCCESS);
}

int32_t HiRecorderImpl::Configure(int32_t sourceId, const RecorderParam& recParam)
{
    MEDIA_LOG_I("Configure enter");
    FALSE_RETURN_V(outputFormatType_ != OutputFormatType::FORMAT_BUTT,
                   TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION));
    FALSE_RETURN_V(CheckParamType(sourceId, recParam), TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE));
    HstRecParam hstRecParam;
    auto castRet  = CastRecorderParam(sourceId, recParam, hstRecParam);
    FALSE_RETURN_V(castRet, TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE));
    auto ret = fsm_.SendEvent(Intent::CONFIGURE, hstRecParam);
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("Configure failed with error " PUBLIC_LOG_S, GetErrorName(ret));
    }
    return TransErrorCode(ret);
}

int32_t HiRecorderImpl::Prepare()
{
    MEDIA_LOG_D("Prepare entered, current fsm state: " PUBLIC_LOG_S ".", fsm_.GetCurrentState().c_str());
    PROFILE_BEGIN();
    auto ret = fsm_.SendEvent(Intent::PREPARE);
    if (ret != ErrorCode::SUCCESS) {
        PROFILE_END("Prepare failed,");
        MEDIA_LOG_E("Prepare failed with error " PUBLIC_LOG_S, GetErrorName(ret));
        return TransErrorCode(ret);
    } else {
        PROFILE_END("Prepare successfully,");
    }
    OSAL::ScopedLock lock(stateMutex_);
    if (curFsmState_ == StateId::RECORDING_SETTING) { // Wait state change to ready
        cond_.Wait(lock, [this] { return curFsmState_ != StateId::RECORDING_SETTING; });
    }
    MEDIA_LOG_D("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    PROFILE_END("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    if (curFsmState_ == StateId::READY) {
        ret = ErrorCode::SUCCESS;
    } else {
        ret = ErrorCode::ERROR_UNKNOWN;
    }
    return TransErrorCode(ret);
}

int32_t HiRecorderImpl::Start()
{
    MEDIA_LOG_I("Start enter");
    PROFILE_BEGIN();
    ErrorCode ret;
    if (curFsmState_ == StateId::PAUSE) {
        ret = fsm_.SendEvent(Intent::RESUME);
    } else {
        ret = fsm_.SendEvent(Intent::START);
    }
    PROFILE_END("Start ret = " PUBLIC_LOG_D32, TransErrorCode(ret));
    return TransErrorCode(ret);
}

int32_t HiRecorderImpl::Pause()
{
    MEDIA_LOG_I("Pause enter");
    PROFILE_BEGIN();
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::PAUSE));
    PROFILE_END("Pause ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiRecorderImpl::Resume()
{
    MEDIA_LOG_I("Resume enter");
    PROFILE_BEGIN();
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::RESUME));
    PROFILE_END("Resume ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiRecorderImpl::Stop(bool isDrainAll)
{
    MEDIA_LOG_I("Stop enter");
    PROFILE_BEGIN();
    MEDIA_LOG_D("Stop start, isDrainAll: " PUBLIC_LOG_U32, static_cast<uint32_t>(isDrainAll));
    outputFormatType_ = OutputFormatType::FORMAT_BUTT;
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::STOP, isDrainAll));
    PROFILE_END("Stop ret = " PUBLIC_LOG_D32, ret);
    FALSE_RETURN_V_MSG_E(ret == ERR_OK, ret, "send STOP event to fsm fail");
    OSAL::ScopedLock lock(stateMutex_);
    cond_.WaitFor(lock, 3000, [this] { // 3000: time out
        return curFsmState_ == StateId::ERROR || curFsmState_ == StateId::INIT;
    });
    FALSE_RETURN_V_MSG_E(curFsmState_ == StateId::INIT, ERR_UNKNOWN_REASON, "stop fail");
    return ERR_OK;
}

int32_t HiRecorderImpl::Reset()
{
    MEDIA_LOG_I("Reset enter");
    PROFILE_BEGIN();
    if (curFsmState_ == StateId::RECORDING) {
        Stop(false); // avoid start then reset the file is not be saved
    }
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::RESET));
    PROFILE_END("Resume ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiRecorderImpl::SetParameter(int32_t sourceId, const RecorderParam &recParam)
{
    return Configure(sourceId, recParam);
}

void HiRecorderImpl::OnEvent(const Event& event)
{
    MEDIA_LOG_D("[HiStreamer] OnEvent (" PUBLIC_LOG_D32 ")", event.type);
    switch (event.type) {
        case EventType::EVENT_ERROR: {
            fsm_.SendEventAsync(Intent::NOTIFY_ERROR, event.param);
            auto ptr = obs_.lock();
            if (ptr != nullptr) {
                ptr->OnError(IRecorderEngineObs::ErrorType::ERROR_INTERNAL,
                             TransErrorCode(Plugin::AnyCast<ErrorCode>(event.param)));
            }
            break;
        }
        case EventType::EVENT_READY: {
            fsm_.SendEventAsync(Intent::NOTIFY_READY);
            break;
        }
        case EventType::EVENT_COMPLETE:
            mediaStatStub_.ReceiveEvent(event);
            if (mediaStatStub_.IsEventCompleteAllReceived()) {
                fsm_.SendEventAsync(Intent::NOTIFY_COMPLETE);
            }
            break;
        default:
            MEDIA_LOG_E("Unknown event(" PUBLIC_LOG_D32 ")", event.type);
    }
}

void HiRecorderImpl::OnStateChanged(StateId state)
{
    MEDIA_LOG_I("OnStateChanged from " PUBLIC_LOG_D32 " to " PUBLIC_LOG_D32, curFsmState_.load(), state);
    {
        OSAL::ScopedLock lock(stateMutex_);
        curFsmState_ = state;
        cond_.NotifyOne();
    }
    auto ptr = obs_.lock();
    if (ptr != nullptr) {
        ptr->OnInfo(IRecorderEngineObs::InfoType::INTERNEL_WARNING, CppExt::to_underlying(state));
    }
}

ErrorCode HiRecorderImpl::DoSetVideoSource(const Plugin::Any& param) const
{
#ifdef VIDEO_SUPPORT
    FALSE_RETURN_V_MSG_E(videoCapture_ != nullptr, ErrorCode::ERROR_INVALID_OPERATION,
                         "videoCapture is NULL");
    using SrcInputPair = std::pair<int32_t, Plugin::SrcInputType>;
    if (param.SameTypeWith(typeid(SrcInputPair))) {
        auto srcType = Plugin::AnyCast<SrcInputPair>(param).second;
        return videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::SRC_INPUT_TYPE), srcType);
    } else {
        return ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
#else
    return ErrorCode::ERROR_UNIMPLEMENTED;
#endif
}

ErrorCode HiRecorderImpl::DoSetAudioSource(const Plugin::Any& param) const
{
    FALSE_RETURN_V_MSG_E(audioCapture_ != nullptr, ErrorCode::ERROR_INVALID_OPERATION,
                         "audioCapture is NULL");
    using SrcInputPair = std::pair<int32_t, Plugin::SrcInputType>;
    if (param.SameTypeWith(typeid(SrcInputPair))) {
        auto srcType = Plugin::AnyCast<SrcInputPair>(param).second;
        return audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::SRC_INPUT_TYPE), srcType);
    } else {
        return ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
}

ErrorCode HiRecorderImpl::DoConfigure(const Plugin::Any &param) const
{
    if (!param.SameTypeWith(typeid(HstRecParam))) {
        return ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
    ErrorCode ret  = ErrorCode::SUCCESS;
    const auto* hstParam = Plugin::AnyCast<HstRecParam>(&param);
    switch (hstParam->stdParamType) {
        case RecorderPublicParamType::AUD_SAMPLERATE:
        case RecorderPublicParamType::AUD_CHANNEL:
        case RecorderPublicParamType::AUD_BITRATE:
        case RecorderPublicParamType::AUD_ENC_FMT:
            ret = DoConfigureAudio(*hstParam);
            break;
        case RecorderPublicParamType::VID_CAPTURERATE:
        case RecorderPublicParamType::VID_RECTANGLE:
        case RecorderPublicParamType::VID_BITRATE:
        case RecorderPublicParamType::VID_FRAMERATE:
        case RecorderPublicParamType::VID_ENC_FMT:
            ret = DoConfigureVideo(*hstParam);
            break;
        case RecorderPublicParamType::OUT_PATH:
        case RecorderPublicParamType::OUT_FD:
        case RecorderPublicParamType::VID_ORIENTATION_HINT:
        case RecorderPublicParamType::GEO_LOCATION:
            ret = DoConfigureOther(*hstParam);
            break;
        default:
            break;
    }
    return ret;
}

ErrorCode HiRecorderImpl::DoSetOutputFormat(const Plugin::Any& param) const
{
    ErrorCode ret {ErrorCode::SUCCESS};
    if (param.SameTypeWith(typeid(OutputFormatType))) {
        auto outputFormatType = Plugin::AnyCast<OutputFormatType>(param);
        if (g_outputFormatToMimeMap.find(outputFormatType) != g_outputFormatToMimeMap.end()) {
            ret = muxer_->SetOutputFormat(g_outputFormatToMimeMap.at(outputFormatType));
        } else {
            ret = ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
        }
    } else {
        ret = ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("SetOutputFormat failed with error " PUBLIC_LOG_D32, static_cast<int>(ret));
    }
    return ret;
}

ErrorCode HiRecorderImpl::DoPrepare()
{
    return pipeline_->Prepare();
}

ErrorCode HiRecorderImpl::DoStart()
{
    return pipeline_->Start();
}

ErrorCode HiRecorderImpl::DoPause()
{
    return curFsmState_ == StateId::READY ? ErrorCode::SUCCESS: pipeline_->Pause();
}

ErrorCode HiRecorderImpl::DoResume()
{
    return pipeline_->Resume();
}

ErrorCode HiRecorderImpl::DoStop(const Plugin::Any& param)
{
    ErrorCode ret = ErrorCode::SUCCESS;
    mediaStatStub_.Reset();
    if (Plugin::AnyCast<bool>(param)) {
        if (audioCapture_) {
            ret = audioCapture_->SendEos();
        }
#ifdef VIDEO_SUPPORT
        if (videoCapture_) {
            ret = videoCapture_->SendEos();
        }
#endif
    } else {
        ret = muxer_->SendEos();
    }
    audioCount_ = 0;
    videoCount_ = 0;
    audioSourceId_ = 0;
    videoSourceId_ = 0;
    return ret;
}

ErrorCode HiRecorderImpl::DoReset()
{
    ErrorCode ret = pipeline_->Stop();
    mediaStatStub_.Reset();
    audioCount_ = 0;
    videoCount_ = 0;
    audioSourceId_ = 0;
    videoSourceId_ = 0;
    return ret;
}

ErrorCode HiRecorderImpl::DoOnComplete()
{
    return pipeline_->Stop();
}

ErrorCode HiRecorderImpl::SetAudioSourceInternal(AudioSourceType source, int32_t sourceId)
{
    if (!audioCapture_) {
        audioCapture_ = FilterFactory::Instance().CreateFilterWithType<AudioCaptureFilter>(
                "builtin.recorder.audiocapture", "audiocapture");
        audioEncoder_ = FilterFactory::Instance().CreateFilterWithType<AudioEncoderFilter>(
                "builtin.recorder.audioencoder", "audioencoder");
        auto ret = pipeline_->AddFilters({audioCapture_.get(), audioEncoder_.get()});
        FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "AddFilters audioCapture to pipeline fail");
        ret = pipeline_->LinkFilters({audioCapture_.get(), audioEncoder_.get()});
        FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "LinkFilters audioCapture and audioEncoder fail");
        std::shared_ptr<InPort> muxerInPort {nullptr};
        ret = muxer_->AddTrack(muxerInPort);
        FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "muxer AddTrack fail");
        ret = pipeline_->LinkPorts(audioEncoder_->GetOutPort(PORT_NAME_DEFAULT), muxerInPort);
        FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "LinkPorts audioEncoderOutPort and muxerInPort fail");
    }
    FALSE_RETURN_V_MSG_E(audioCapture_ != nullptr && audioEncoder_ != nullptr, ErrorCode::ERROR_UNKNOWN,
                         "create audioCapture/audioEncoder filter fail");
    appTokenId_ = IPCSkeleton::GetCallingTokenID();
    appUid_ = IPCSkeleton::GetCallingUid();
    audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::APP_TOKEN_ID), appTokenId_);
    audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::APP_UID), appUid_);
    return fsm_.SendEvent(Intent::SET_AUDIO_SOURCE,
                          std::pair<int32_t, Plugin::SrcInputType>(sourceId, TransAudioInputType(source)));
}

ErrorCode HiRecorderImpl::SetVideoSourceInternal(VideoSourceType source, int32_t sourceId)
{
#ifdef VIDEO_SUPPORT
    videoCapture_ = FilterFactory::Instance().CreateFilterWithType<VideoCaptureFilter>(
            "builtin.recorder.videocapture", "videocapture");
    videoEncoder_ = FilterFactory::Instance().CreateFilterWithType<VideoEncoderFilter>(
            "builtin.recorder.videoencoder", "videoencoder");
    FALSE_RETURN_V_MSG_E(videoCapture_ != nullptr && videoEncoder_ != nullptr, ErrorCode::ERROR_UNKNOWN,
                         "create videoCapture/videoEncoder filter fail");
    auto ret = pipeline_->AddFilters({videoCapture_.get(), videoEncoder_.get()});
    FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "AddFilters videoCapture to pipeline fail");
    ret = pipeline_->LinkFilters({videoCapture_.get(), videoEncoder_.get()});
    FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "LinkFilters videoCapture and videoEncoder fail");
    std::shared_ptr<InPort> muxerInPort {nullptr};
    ret =muxer_->AddTrack(muxerInPort);
    FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "muxer AddTrack fail");
    ret = pipeline_->LinkPorts(videoEncoder_->GetOutPort(PORT_NAME_DEFAULT), muxerInPort);
    FALSE_RETURN_V_MSG_E(ret == ErrorCode::SUCCESS, ret, "LinkPorts videoEncoderOutPort and muxerInPort fail");
    MEDIA_LOG_E("SendEvent: SET_VIDEO_SOURCE");
    return fsm_.SendEvent(Intent::SET_VIDEO_SOURCE,
                          std::pair<int32_t, Plugin::SrcInputType>(sourceId, TransVideoInputType(source)));
#else
    return ErrorCode::ERROR_UNIMPLEMENTED;
#endif
}

ErrorCode HiRecorderImpl::DoConfigureAudio(const HstRecParam& param) const
{
    FALSE_RETURN_V_MSG_E(audioCapture_ != nullptr && audioEncoder_ != nullptr, ErrorCode::ERROR_INVALID_OPERATION,
                         "audioCapture/audioEncoder is NULL");
    switch (param.stdParamType) {
        case RecorderPublicParamType::AUD_SAMPLERATE: {
            auto ptr = param.GetValPtr<AudSampleRate>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            MEDIA_LOG_I("set sample rate : " PUBLIC_LOG_D32, ptr->sampleRate);
            return audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::AUDIO_SAMPLE_RATE),
                                               static_cast<uint32_t>(ptr->sampleRate));
        }
        case RecorderPublicParamType::AUD_CHANNEL: {
            auto ptr = param.GetValPtr<AudChannel>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            MEDIA_LOG_I("set channel : " PUBLIC_LOG_D32, ptr->channel);
            return audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::AUDIO_CHANNELS),
                                               static_cast<uint32_t>(ptr->channel));
        }
        case RecorderPublicParamType::AUD_BITRATE: {
            auto ptr = param.GetValPtr<AudBitRate>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr && static_cast<int32_t>(ptr->bitRate) > 0,
                ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            MEDIA_LOG_I("set bitRate : " PUBLIC_LOG_D32, ptr->bitRate);
            return audioCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::MEDIA_BITRATE),
                                               static_cast<int64_t>(ptr->bitRate));
        }
        case RecorderPublicParamType::AUD_ENC_FMT: {
            auto ptr = param.GetValPtr<AudEnc>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            auto encoderMeta = std::make_shared<Plugin::Meta>();
            if (!TransAudioEncoderFmt(ptr->encFmt, *encoderMeta)) {
                return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
            }
            return audioEncoder_->SetAudioEncoder(param.srcId, encoderMeta);
        }
        default:
            MEDIA_LOG_W("RecorderPublicParamType " PUBLIC_LOG_U32 " not supported", param.stdParamType);
            return ErrorCode::SUCCESS;
    }
}

ErrorCode HiRecorderImpl::DoConfigureVideo(const HstRecParam& param) const
{
#ifdef VIDEO_SUPPORT
    FALSE_RETURN_V_MSG_E(videoCapture_ != nullptr && videoEncoder_ != nullptr, ErrorCode::ERROR_INVALID_OPERATION,
                         "videoCapture/videoEncoder is NULL");
    switch (param.stdParamType) {
        case RecorderPublicParamType::VID_RECTANGLE: {
            auto ptr = param.GetValPtr<VidRectangle>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            auto ret = videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::VIDEO_WIDTH),
                                                   static_cast<uint32_t>(ptr->width));
            if (ret == ErrorCode::SUCCESS) {
                ret = videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::VIDEO_HEIGHT),
                                                  static_cast<uint32_t>(ptr->height));
            }
            return ret;
        }
        case RecorderPublicParamType::VID_CAPTURERATE: {
            auto ptr = param.GetValPtr<CaptureRate>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            return videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::VIDEO_CAPTURE_RATE),
                                               static_cast<double>(ptr->capRate));
        }
        case RecorderPublicParamType::VID_BITRATE: {
            auto ptr = param.GetValPtr<VidBitRate>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            return videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::MEDIA_BITRATE),
                static_cast<int64_t>((ptr->bitRate >= 0) ? ptr->bitRate : 0));
        }
        case RecorderPublicParamType::VID_FRAMERATE: {
            auto ptr = param.GetValPtr<VidFrameRate>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            return videoCapture_->SetParameter(static_cast<int32_t>(Plugin::Tag::VIDEO_FRAME_RATE),
                static_cast<uint32_t>((ptr->frameRate >= 0) ? ptr->frameRate : 0));
        }
        case RecorderPublicParamType::VID_ENC_FMT: {
            auto ptr = param.GetValPtr<VidEnc>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            auto encoderMeta = std::make_shared<Plugin::Meta>();
            if (!TransVideoEncoderFmt(ptr->encFmt, *encoderMeta)) {
                return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
            }
            return videoEncoder_->SetVideoEncoder(param.srcId, encoderMeta);
        }
        default:
            MEDIA_LOG_W("ignore RecorderPublicParamType " PUBLIC_LOG_U32, param.stdParamType);
            return ErrorCode::SUCCESS;
    }
#else
    return ErrorCode::ERROR_UNIMPLEMENTED;
#endif
}

ErrorCode HiRecorderImpl::DoConfigureOther(const HstRecParam& param) const
{
    FALSE_RETURN_V_MSG_E(muxer_ != nullptr && outputSink_ != nullptr, ErrorCode::ERROR_INVALID_OPERATION,
                         "muxer/outputSink is NULL");
    switch (param.stdParamType) {
        case RecorderPublicParamType::OUT_PATH: {
            auto ptr = param.GetValPtr<OutFilePath>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            auto dirPath = ptr->path;
            std::regex reg("\\\\");
            dirPath= std::regex_replace(dirPath, reg, "/");
            FALSE_RETURN_V_MSG_E(!OSAL::FileSystem::IsRegularFile(dirPath)
                && OSAL::FileSystem::MakeMultipleDir(dirPath),
                ErrorCode::ERROR_INVALID_PARAMETER_VALUE, "OutFilePath is not a valid directory path");
            std::string filePath;
            FALSE_RETURN_V_MSG_E(GenerateFilePath(dirPath, outputFormatType_, filePath),
                                 ErrorCode::ERROR_INVALID_PARAMETER_VALUE, "generate file path error");
            MediaSink mediaSink {Plugin::ProtocolType::FILE};
            mediaSink.SetPath(filePath);
            return outputSink_->SetSink(mediaSink);
        }
        case RecorderPublicParamType::OUT_FD: {
            auto ptr = param.GetValPtr<OutFd>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            MediaSink mediaSink {Plugin::ProtocolType::FD};
            int32_t fd = ptr->fd;
#ifndef WIN32
            fd = dup(fd);
            FALSE_RETURN_V(fd >= 0, ErrorCode::ERROR_INVALID_PARAMETER_VALUE);
#endif
            mediaSink.SetFd(fd);
            return outputSink_->SetSink(mediaSink);
        }
        case RecorderPublicParamType::MAX_DURATION: {
            auto ptr = param.GetValPtr<MaxDuration>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            int64_t hstTime = 0;
            Plugin::Sec2HstTime(ptr->duration, hstTime);
            return muxer_->SetMaxDuration(hstTime > 0 ? hstTime : 0);
        }
        case RecorderPublicParamType::MAX_SIZE: {
            auto ptr = param.GetValPtr<MaxFileSize>();
            FALSE_RETURN_V_MSG_E(ptr != nullptr, ErrorCode::ERROR_INVALID_PARAMETER_VALUE,);
            return muxer_->SetMaxSize(ptr->size > 0 ? ptr->size : 0);
        }
        case RecorderPublicParamType::VID_ORIENTATION_HINT:
        case RecorderPublicParamType::GEO_LOCATION:
        default:
            MEDIA_LOG_W("ignore RecorderPublicParamType " PUBLIC_LOG_U32, param.stdParamType);
            return ErrorCode::SUCCESS;
    }
}

bool HiRecorderImpl::CheckParamType(int32_t sourceId, const RecorderParam& recParam) const
{
    FALSE_RETURN_V((SourceIdGenerator::IsAudio(sourceId) && recParam.IsAudioParam() &&
        static_cast<int32_t>(audioSourceId_) == sourceId) ||
        (SourceIdGenerator::IsVideo(sourceId) && recParam.IsVideoParam() &&
        static_cast<int32_t>(videoSourceId_) == sourceId) ||
        ((sourceId == DUMMY_SOURCE_ID) && !(recParam.IsAudioParam() || recParam.IsVideoParam())), false);
    return true;
}
} // namespace Record
} // namespace Media
} // namespace OHOS
#endif