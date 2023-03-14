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

#include "recorder_utils.h"
#include "media_errors.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_video_tags.h"

namespace OHOS {
namespace Media {
namespace {
const std::pair<ErrorCode, int32_t> g_errorCodePair[] = {
    {ErrorCode::SUCCESS, MSERR_OK},
    {ErrorCode::ERROR_UNKNOWN, MSERR_UNKNOWN},
    {ErrorCode::ERROR_AGAIN, MSERR_UNKNOWN},
    {ErrorCode::ERROR_UNIMPLEMENTED, MSERR_UNSUPPORT},
    {ErrorCode::ERROR_INVALID_PARAMETER_VALUE, MSERR_INVALID_VAL},
    {ErrorCode::ERROR_INVALID_PARAMETER_TYPE, MSERR_INVALID_VAL},
    {ErrorCode::ERROR_INVALID_OPERATION, MSERR_INVALID_OPERATION},
    {ErrorCode::ERROR_UNSUPPORTED_FORMAT, MSERR_UNSUPPORT_CONTAINER_TYPE},
    {ErrorCode::ERROR_NOT_EXISTED, MSERR_OPEN_FILE_FAILED},
    {ErrorCode::ERROR_TIMED_OUT, MSERR_EXT_TIMEOUT},
    {ErrorCode::ERROR_NO_MEMORY, MSERR_EXT_NO_MEMORY},
    {ErrorCode::ERROR_INVALID_STATE, MSERR_INVALID_STATE},
};
const std::map<OutputFormatType, std::string> g_outputFormatToFormat = {
    {OutputFormatType::FORMAT_DEFAULT, std::string("audio_%Y%m%d%H%M%S.m4a")},
    {OutputFormatType::FORMAT_MPEG_4, std::string("video_%Y%m%d%H%M%S.mp4")},
    {OutputFormatType::FORMAT_M4A, std::string("audio_%Y%m%d%H%M%S.m4a")},
};
}  // namespace
namespace Record {
int TransErrorCode(ErrorCode errorCode)
{
    for (const auto& errPair : g_errorCodePair) {
        if (errPair.first == errorCode) {
            return errPair.second;
        }
    }
    return MSERR_UNKNOWN;
}
Plugin::SrcInputType TransAudioInputType(OHOS::Media::AudioSourceType sourceType)
{
    const static std::pair<OHOS::Media::AudioSourceType, Plugin::SrcInputType> mapArray[] = {
        {OHOS::Media::AudioSourceType::AUDIO_MIC, Plugin::SrcInputType::AUD_MIC},
        {OHOS::Media::AudioSourceType::AUDIO_SOURCE_DEFAULT, Plugin::SrcInputType::AUD_MIC},
    };
    for (const auto& item : mapArray) {
        if (item.first == sourceType) {
            return item.second;
        }
    }
    return Plugin::SrcInputType::UNKNOWN;
}
Plugin::SrcInputType TransVideoInputType(OHOS::Media::VideoSourceType sourceType)
{
    const static std::pair<OHOS::Media::VideoSourceType, Plugin::SrcInputType> mapArray[] = {
        {OHOS::Media::VideoSourceType::VIDEO_SOURCE_SURFACE_YUV, Plugin::SrcInputType::VID_SURFACE_YUV},
        {OHOS::Media::VideoSourceType::VIDEO_SOURCE_SURFACE_ES, Plugin::SrcInputType::VID_SURFACE_ES},
    };
    for (const auto& item : mapArray) {
        if (item.first == sourceType) {
            return item.second;
        }
    }
    return Plugin::SrcInputType::UNKNOWN;
}
bool TransAudioEncoderFmt(OHOS::Media::AudioCodecFormat aFormat, Plugin::Meta& encoderMeta)
{
    switch (aFormat) {
        case OHOS::Media::AudioCodecFormat::AUDIO_DEFAULT:
        case OHOS::Media::AudioCodecFormat::AAC_LC:
            encoderMeta.SetString(Plugin::MetaID::MIME, MEDIA_MIME_AUDIO_AAC);
            encoderMeta.SetData(Plugin::MetaID::AUDIO_AAC_PROFILE, Plugin::AudioAacProfile::LC);
            return true;
        default:
            break;
    }
    return false;
}

bool TransVideoEncoderFmt(OHOS::Media::VideoCodecFormat vFormat, Plugin::Meta& encoderMeta)
{
    bool ret = true;
    switch (vFormat) {
        case OHOS::Media::VideoCodecFormat::H264:
            encoderMeta.SetString(Plugin::MetaID::MIME, MEDIA_MIME_VIDEO_H264);
            encoderMeta.SetData(Plugin::MetaID::VIDEO_H264_PROFILE, Plugin::VideoH264Profile::BASELINE);
            encoderMeta.SetUint32(Plugin::MetaID::VIDEO_H264_LEVEL, 32); // 32: LEVEL 3.2
            break;
        case OHOS::Media::VideoCodecFormat::MPEG4:
            encoderMeta.SetString(Plugin::MetaID::MIME, MEDIA_MIME_VIDEO_MPEG4);
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}

bool GenerateFilePath(const std::string& dirPath, OutputFormatType outputFormatType, std::string& filePath)
{
    filePath = dirPath;
    if (filePath[filePath.size() - 1] != '/') {
        filePath += "/";
    }
    if (g_outputFormatToFormat.count(outputFormatType) != 0) {
        char fileName[32] = { 0 }; /// magic number 32
        auto tm = time(nullptr);
        strftime(fileName, sizeof(fileName), g_outputFormatToFormat.at(outputFormatType).c_str(), localtime(&tm));
        filePath += fileName;
        return true;
    } else {
        return false;
    }
}
namespace {
#define CAST_TO_ASSIGN(type, param, any) \
(any) = static_cast<const type&>(param)
bool CastAudRecorderParam(const RecorderParam& param, Plugin::ValueType& val)
{
    auto ret = true;
    switch (param.type) {
        case RecorderPublicParamType::AUD_SAMPLERATE:
            CAST_TO_ASSIGN(AudSampleRate, param, val);
            break;
        case RecorderPublicParamType::AUD_CHANNEL:
            CAST_TO_ASSIGN(AudChannel, param, val);
            break;
        case RecorderPublicParamType::AUD_BITRATE:
            CAST_TO_ASSIGN(AudBitRate, param, val);
            break;
        case RecorderPublicParamType::AUD_ENC_FMT:
            CAST_TO_ASSIGN(AudEnc, param, val);
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}
bool CastVidRecorderParam(const RecorderParam& param, Plugin::ValueType& val)
{
    auto ret = true;
    switch (param.type) {
        case RecorderPublicParamType::VID_CAPTURERATE:
            CAST_TO_ASSIGN(CaptureRate, param, val);
            break;
        case RecorderPublicParamType::VID_RECTANGLE:
            CAST_TO_ASSIGN(VidRectangle, param, val);
            break;
        case RecorderPublicParamType::VID_BITRATE:
            CAST_TO_ASSIGN(VidBitRate, param, val);
            break;
        case RecorderPublicParamType::VID_FRAMERATE:
            CAST_TO_ASSIGN(VidFrameRate, param, val);
            break;
        case RecorderPublicParamType::VID_ENC_FMT:
            CAST_TO_ASSIGN(VidEnc, param, val);
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}

bool CastDummyRecorderParam(const RecorderParam& param, Plugin::ValueType& val)
{
    auto ret  = true;
    switch (param.type) {
        case RecorderPublicParamType::OUT_PATH:
            CAST_TO_ASSIGN(OutFilePath, param, val);
            break;
        case RecorderPublicParamType::OUT_FD:
            CAST_TO_ASSIGN(OutFd, param, val);
            break;
        case RecorderPublicParamType::VID_ORIENTATION_HINT:
            CAST_TO_ASSIGN(RotationAngle, param, val);
            break;
        case RecorderPublicParamType::GEO_LOCATION:
            CAST_TO_ASSIGN(GeoLocation, param, val);
            break;
        case RecorderPublicParamType::MAX_DURATION:
            CAST_TO_ASSIGN(MaxDuration, param, val);
            break;
        case RecorderPublicParamType::MAX_SIZE:
            CAST_TO_ASSIGN(MaxFileSize, param, val);
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}
#undef CAST_TO_ASSIGN
}
bool CastRecorderParam(int32_t sourceId, const RecorderParam& param, HstRecParam& out)
{
    out.srcId = sourceId;
    out.stdParamType = static_cast<RecorderPublicParamType>(param.type);
    if (param.IsAudioParam()) {
        return CastAudRecorderParam(param, out.val);
    } else if (param.IsVideoParam()) {
        return CastVidRecorderParam(param, out.val);
    } else {
        return CastDummyRecorderParam(param, out.val);
    }
}
}  // namespace Record
}  // namespace Media
}  // namespace OHOS