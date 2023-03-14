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

#define HST_LOG_TAG "FFMpeg_Muxer"

#include "ffmpeg_muxer_plugin.h"

#include <functional>
#include <set>

#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "plugin/interface/plugin_definition.h"
#include "plugin/plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"
#include "plugin/plugins/ffmpeg_adapter/utils/ffmpeg_codec_map.h"

namespace {
using namespace OHOS::Media::Plugin;
using namespace Ffmpeg;

std::function<int32_t(uint32_t)> ui2iFunc = [](uint32_t i) {return i;};

std::map<std::string, std::shared_ptr<AVOutputFormat>> g_pluginOutputFmt;

std::set<std::string> g_supportedMuxer = {"mp4", "h264"};

bool IsMuxerSupported(const char* name)
{
    return g_supportedMuxer.count(name) == 1;
}

bool UpdatePluginInCapability(AVCodecID codecId, CapabilitySet& capSet)
{
    if (codecId != AV_CODEC_ID_NONE) {
        Capability cap;
        if (!FFCodecMap::CodecId2Cap(codecId, true, cap)) {
            return false;
        } else {
            capSet.emplace_back(cap);
        }
    }
    return true;
}

bool UpdatePluginCapability(const AVOutputFormat* oFmt, MuxerPluginDef& pluginDef)
{
    if (!FFCodecMap::FormatName2Cap(oFmt->name, pluginDef.outCaps)) {
        MEDIA_LOG_D(PUBLIC_LOG_S " is not supported now", oFmt->name);
        return false;
    }
    UpdatePluginInCapability(oFmt->audio_codec, pluginDef.inCaps);
    UpdatePluginInCapability(oFmt->video_codec, pluginDef.inCaps);
    UpdatePluginInCapability(oFmt->subtitle_codec, pluginDef.inCaps);
    return true;
}

Status RegisterMuxerPlugins(const std::shared_ptr<Register>& reg)
{
    MEDIA_LOG_D("register muxer plugins.");
    FALSE_RETURN_V_MSG_E(reg != nullptr, Status::ERROR_INVALID_PARAMETER,
                         "RegisterPlugins failed due to null pointer for reg.");
    const AVOutputFormat* outputFormat = nullptr;
    void* ite = nullptr;
    while ((outputFormat = av_muxer_iterate(&ite))) {
        MEDIA_LOG_DD("check ffmpeg muxer name: " PUBLIC_LOG_S, outputFormat->name);
        if (!IsMuxerSupported(outputFormat->name)) {
            continue;
        }
        if (outputFormat->long_name != nullptr) {
            if (!strncmp(outputFormat->long_name, "raw ", 4)) { // 4
                continue;
            }
        }
        std::string pluginName = "ffmpegMux_" + std::string(outputFormat->name);
        ReplaceDelimiter(".,|-<> ", '_', pluginName);
        MuxerPluginDef def;
        if (!UpdatePluginCapability(outputFormat, def)) {
            continue;
        }
        def.name = pluginName;
        def.description = "ffmpeg muxer";
        def.rank = 100; // 100
        def.creator = [](const std::string& name) -> std::shared_ptr<MuxerPlugin> {
            return std::make_shared<FFmpegMuxerPlugin>(name);
        };
        if (reg->AddPlugin(def) != Status::OK) {
            MEDIA_LOG_W("fail to add plugin " PUBLIC_LOG_S, pluginName.c_str());
            continue;
        }
        g_pluginOutputFmt[pluginName] = std::shared_ptr<AVOutputFormat>(const_cast<AVOutputFormat*>(outputFormat),
                                                                        [](AVOutputFormat* ptr) {}); // do not delete
    }
    return Status::OK;
}
PLUGIN_DEFINITION(FFmpegMuxers, LicenseType::LGPL, RegisterMuxerPlugins, [] {g_pluginOutputFmt.clear();})

Status SetCodecByMime(const AVOutputFormat* fmt, const std::string& mime, AVStream* stream)
{
    AVCodecID id = AV_CODEC_ID_NONE;
    FALSE_RETURN_V_MSG_E(FFCodecMap::Mime2CodecId(mime, id), Status::ERROR_UNSUPPORTED_FORMAT,
                         "mime " PUBLIC_LOG_S " has no corresponding codec id", mime.c_str());
    auto ptr = avcodec_find_encoder(id);
    FALSE_RETURN_V_MSG_E(ptr != nullptr, Status::ERROR_UNSUPPORTED_FORMAT,
                         "codec of mime " PUBLIC_LOG_S " is not founder as encoder", mime.c_str());
    bool matched = true;
    switch (ptr->type) {
        case AVMEDIA_TYPE_VIDEO:
            matched = id == fmt->video_codec;
            break;
        case AVMEDIA_TYPE_AUDIO:
            matched = id == fmt->audio_codec;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            matched = id == fmt->subtitle_codec;
            break;
        default:
            matched = false;
    }
    FALSE_RETURN_V_MSG_E(matched, Status::ERROR_UNSUPPORTED_FORMAT,  "codec of mime " PUBLIC_LOG_S
        " is not matched with " PUBLIC_LOG_S " muxer", mime.c_str(), fmt->name);
    stream->codecpar->codec_id = id;
    stream->codecpar->codec_type = ptr->type;
    return Status::OK;
}

Status SetCodecOfTrack(const AVOutputFormat* fmt, AVStream* stream, const TagMap& tagMap)
{
    auto ite = tagMap.Find(Tag::MIME);
    FALSE_RETURN_V_MSG_E(ite != std::end(tagMap), Status::ERROR_UNSUPPORTED_FORMAT, "mime is missing!");
    FALSE_RETURN_V(ite->second.SameTypeWith(typeid(std::string)), Status::ERROR_MISMATCHED_TYPE);
    // todo specially for audio/mpeg audio/mpeg we should check mpegversion and mpeglayer

    return SetCodecByMime(fmt, AnyCast<std::string>(ite->second), stream);
}

template<typename T, typename U>
Status SetSingleParameter(Tag tag, const TagMap& tagMap, U& target, std::function<U(T)> func)
{
    auto ite = tagMap.Find(tag);
    if (ite != std::end(tagMap)) {
        FALSE_RETURN_V_MSG_E(ite->second.SameTypeWith(typeid(T)), Status::ERROR_MISMATCHED_TYPE,
                             "tag " PUBLIC_LOG_U32 " type mismatched", tag);
        target = func(AnyCast<T>(ite->second));
    }
    return Status::OK;
}

Status SetParameterOfAuTrack(AVStream* stream, const TagMap& tagMap)
{
    auto ret = SetSingleParameter<AudioSampleFormat, int32_t>(Tag::AUDIO_SAMPLE_FORMAT, tagMap,
                                                              stream->codecpar->format, ConvP2FfSampleFmt);
    NOK_RETURN(ret);
    ret = SetSingleParameter<uint32_t, int32_t>(Tag::AUDIO_CHANNELS, tagMap, stream->codecpar->channels, ui2iFunc);
    NOK_RETURN(ret);
    ret = SetSingleParameter<uint32_t, int32_t>(Tag::AUDIO_SAMPLE_RATE, tagMap, stream->codecpar->sample_rate,
                                                ui2iFunc);
    NOK_RETURN(ret);
    ret = SetSingleParameter<uint32_t, int32_t>(Tag::AUDIO_SAMPLE_PER_FRAME, tagMap, stream->codecpar->frame_size,
                                                ui2iFunc);
    NOK_RETURN(ret);
    ret = SetSingleParameter<AudioChannelLayout, uint64_t>(Tag::AUDIO_CHANNEL_LAYOUT, tagMap,
        stream->codecpar->channel_layout, [](AudioChannelLayout layout) {return (uint64_t)layout;});
    return ret;
}

Status SetParameterOfVdTrack(AVStream* stream, const TagMap& tagMap)
{
    auto ret = SetSingleParameter<VideoPixelFormat, int32_t>(Tag::VIDEO_PIXEL_FORMAT, tagMap,
        stream->codecpar->format, ConvertPixelFormatToFFmpeg);
    NOK_RETURN(ret);
    ret = SetSingleParameter<uint32_t, int32_t>(Tag::VIDEO_WIDTH, tagMap, stream->codecpar->width, ui2iFunc);
    NOK_RETURN(ret);
    ret = SetSingleParameter<uint32_t, int32_t>(Tag::VIDEO_HEIGHT, tagMap, stream->codecpar->height, ui2iFunc);
    NOK_RETURN(ret);
    ret = SetSingleParameter<int64_t, int64_t>(Tag::MEDIA_BITRATE, tagMap, stream->codecpar->bit_rate,
                                               [](int64_t bitRate) {return bitRate;});
    NOK_RETURN(ret);
    ret = SetSingleParameter<VideoH264Profile, int32_t>(Tag::VIDEO_H264_PROFILE, tagMap, stream->codecpar->profile,
                                                        ConvH264ProfileToFfmpeg);
    NOK_RETURN(ret);
    return SetSingleParameter<uint32_t, int32_t>(Tag::VIDEO_H264_LEVEL, tagMap, stream->codecpar->level,
                                                 ui2iFunc);
}

Status SetParameterOfSubTitleTrack(AVStream* stream, const TagMap& tagMap)
{
    // todo add subtitle
    MEDIA_LOG_E("should add subtitle tack parameter setter");
    return Status::ERROR_UNKNOWN;
}

void ResetCodecParameter(AVCodecParameters* par)
{
    av_freep(&par->extradata);
    (void)memset_s(par, sizeof(*par), 0, sizeof(*par));
    par->codec_type = AVMEDIA_TYPE_UNKNOWN;
    par->codec_id = AV_CODEC_ID_NONE;
    par->format = -1;
    par->profile = FF_PROFILE_UNKNOWN;
    par->level = FF_LEVEL_UNKNOWN;
    par->field_order = AV_FIELD_UNKNOWN;
    par->color_range = AVCOL_RANGE_UNSPECIFIED;
    par->color_primaries = AVCOL_PRI_UNSPECIFIED;
    par->color_trc = AVCOL_TRC_UNSPECIFIED;
    par->color_space = AVCOL_SPC_UNSPECIFIED;
    par->chroma_location = AVCHROMA_LOC_UNSPECIFIED;
    par->sample_aspect_ratio = AVRational {0, 1};
}

Status SetTagsOfTrack(const AVOutputFormat* fmt, AVStream* stream, const TagMap& tagMap)
{
    FALSE_RETURN_V(stream != nullptr, Status::ERROR_INVALID_PARAMETER);
    ResetCodecParameter(stream->codecpar);
    // firstly mime
    auto ret = SetCodecOfTrack(fmt, stream, tagMap);
    NOK_RETURN(ret);
    if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) { // audio
        ret = SetParameterOfAuTrack(stream, tagMap);
    } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) { // video
        ret = SetParameterOfVdTrack(stream, tagMap);
    } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) { // subtitle
        ret = SetParameterOfSubTitleTrack(stream, tagMap);
    } else {
        MEDIA_LOG_W("unknown codec type of stream " PUBLIC_LOG_D32, stream->index);
    }
    NOK_RETURN(ret);
    // others
    ret = SetSingleParameter<int64_t, int64_t>(Tag::MEDIA_BITRATE, tagMap, stream->codecpar->bit_rate,
         [](int64_t rate) {return rate;});
    NOK_RETURN(ret);
    // extra data
    auto ite = tagMap.Find(Tag::MEDIA_CODEC_CONFIG);
    if (ite != std::end(tagMap)) {
        FALSE_RETURN_V_MSG_E(ite->second.SameTypeWith(typeid(CodecConfig)), Status::ERROR_MISMATCHED_TYPE,
                          "tag " PUBLIC_LOG_D32 " type mismatched", Tag::MEDIA_CODEC_CONFIG);
        auto codecConfig = AnyCast<CodecConfig>(ite->second);
        if (!codecConfig.empty()) {
            auto extraSize = codecConfig.size();
            stream->codecpar->extradata = static_cast<uint8_t *>(av_mallocz(extraSize + AV_INPUT_BUFFER_PADDING_SIZE));
            FALSE_RETURN_V(stream->codecpar->extradata != nullptr, Status::ERROR_NO_MEMORY);
            (void)memcpy_s(stream->codecpar->extradata, extraSize, codecConfig.data(), extraSize);
            stream->codecpar->extradata_size = extraSize;
        }
    }
    return Status::OK;
}

Status SetTagsOfGeneral(AVFormatContext* fmtCtx, const TagMap& tags)
{
    for (const auto& pair: tags) {
        std::string metaName;
        if (!FindAvMetaNameByTag(pair.first, metaName)) {
            MEDIA_LOG_I("tag " PUBLIC_LOG_U32 " will not written as general meta", pair.first);
            continue;
        }
        if (!pair.second.SameTypeWith(typeid(std::string))) {
            continue;
        }
        auto value = AnyCast<std::string>(pair.second);
        av_dict_set(&fmtCtx->metadata, metaName.c_str(), value.c_str(), 0);
    }
    return Status::OK;
}
}

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
FFmpegMuxerPlugin::FFmpegMuxerPlugin(std::string name) : MuxerPlugin(std::move(name)) {}

FFmpegMuxerPlugin::~FFmpegMuxerPlugin()
{
    Release();
}
Status FFmpegMuxerPlugin::Release()
{
    outputFormat_.reset();
    cachePacket_.reset();
    formatContext_.reset();
    return Status::OK;
}

Status FFmpegMuxerPlugin::InitFormatCtxLocked()
{
    if (formatContext_ == nullptr) {
        auto ioCtx = InitAvIoCtx();
        FALSE_RETURN_V(ioCtx != nullptr, Status::ERROR_NO_MEMORY);
        auto fmt = avformat_alloc_context();
        FALSE_RETURN_V(fmt != nullptr, Status::ERROR_NO_MEMORY);
        fmt->pb = ioCtx;
        fmt->oformat = outputFormat_.get();
        fmt->flags = static_cast<uint32_t>(fmt->flags) | static_cast<uint32_t>(AVFMT_FLAG_CUSTOM_IO);
        formatContext_ = std::shared_ptr<AVFormatContext>(fmt, [](AVFormatContext* ptr) {
            if (ptr) {
                DeInitAvIoCtx(ptr->pb);
                avformat_free_context(ptr);
            }
        });
    }
    return Status::OK;
}

Status FFmpegMuxerPlugin::Init()
{
    MEDIA_LOG_D("Init entered.");
    FALSE_RETURN_V(g_pluginOutputFmt.count(pluginName_) != 0, Status::ERROR_UNSUPPORTED_FORMAT);
    outputFormat_ = g_pluginOutputFmt[pluginName_];
    auto pkt = av_packet_alloc();
    cachePacket_ = std::shared_ptr<AVPacket> (pkt, [] (AVPacket* packet) {av_packet_free(&packet);});
    OSAL::ScopedLock lock(fmtMutex_);
    return InitFormatCtxLocked();
}

Status FFmpegMuxerPlugin::Deinit()
{
    return Release();
}

AVIOContext* FFmpegMuxerPlugin::InitAvIoCtx()
{
    constexpr int bufferSize = 4096; // 4096
    auto buffer = static_cast<unsigned char*>(av_malloc(bufferSize));
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr,  "AllocAVIOContext failed to av_malloc...");
    AVIOContext* avioContext = avio_alloc_context(buffer, bufferSize, AVIO_FLAG_WRITE, static_cast<void*>(&ioContext_),
                                                  IoRead, IoWrite, IoSeek);
    if (avioContext == nullptr) {
        MEDIA_LOG_E("AllocAVIOContext failed to avio_alloc_context...");
        av_free(buffer);
        return nullptr;
    }
    avioContext->seekable = AVIO_SEEKABLE_NORMAL;
    return avioContext;
}

void FFmpegMuxerPlugin::DeInitAvIoCtx(AVIOContext* ptr)
{
    if (ptr != nullptr) {
        ptr->opaque = nullptr;
        av_freep(&ptr->buffer);
        avio_context_free(&ptr);
    }
}

Status FFmpegMuxerPlugin::Prepare()
{
    for (const auto& pair: trackParameters_) {
        SetTagsOfTrack(outputFormat_.get(), formatContext_->streams[pair.first], pair.second);
    }
    SetTagsOfGeneral(formatContext_.get(), generalParameters_);
    formatContext_->flags |= AVFMT_TS_NONSTRICT;
    return Status::OK;
}
void FFmpegMuxerPlugin::ResetIoCtx(IOContext& ioContext)
{
    ioContext.dataSink_.reset();
    ioContext.pos_ = 0;
    ioContext.end_ = 0;
}
Status FFmpegMuxerPlugin::Reset()
{
    ResetIoCtx(ioContext_);
    generalParameters_.Clear();
    trackParameters_.clear();
    OSAL::ScopedLock lock(fmtMutex_);
    if (outputFormat_->deinit) {
        outputFormat_->deinit(formatContext_.get());
    }
    formatContext_.reset();
    return InitFormatCtxLocked();
}

Status FFmpegMuxerPlugin::GetParameter(Tag tag, ValueType& value)
{
    return PluginBase::GetParameter(tag, value);
}

Status FFmpegMuxerPlugin::GetTrackParameter(uint32_t trackId, Tag tag, Plugin::ValueType& value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status FFmpegMuxerPlugin::SetParameter(Tag tag, const ValueType& value)
{
    generalParameters_[tag] = value;
    return Status::OK;
}

Status FFmpegMuxerPlugin::SetTrackParameter(uint32_t trackId, Tag tag, const Plugin::ValueType& value)
{
    FALSE_RETURN_V(trackId < formatContext_->nb_streams, Status::ERROR_INVALID_PARAMETER);
    if (trackParameters_.count(trackId) == 0) {
        trackParameters_.insert({trackId, Plugin::TagMap()});
    }
    trackParameters_[trackId][tag] = value;
    return Status::OK;
}

Status FFmpegMuxerPlugin::AddTrack(uint32_t &trackId)
{
    OSAL::ScopedLock lock(fmtMutex_);
    FALSE_RETURN_V_MSG_E(formatContext_ != nullptr, Status::ERROR_WRONG_STATE, "formatContext_ is NULL");
    auto st = avformat_new_stream(formatContext_.get(), nullptr);
    FALSE_RETURN_V_MSG_E(st != nullptr, Status::ERROR_NO_MEMORY, "avformat_new_stream fail");
    st->codecpar->codec_type = AVMEDIA_TYPE_UNKNOWN;
    st->codecpar->codec_id = AV_CODEC_ID_NONE;
    trackId = static_cast<uint32_t>(st->index);
    return Status::OK;
}

Status FFmpegMuxerPlugin::SetDataSink(const std::shared_ptr<DataSink>& dataSink)
{
    ioContext_.dataSink_ = dataSink;
    return Status::OK;
}

Status FFmpegMuxerPlugin::WriteHeader()
{
    FALSE_RETURN_V(ioContext_.dataSink_ != nullptr && outputFormat_ != nullptr, Status::ERROR_WRONG_STATE);
    OSAL::ScopedLock lock(fmtMutex_);
    FALSE_RETURN_V(formatContext_ != nullptr, Status::ERROR_WRONG_STATE);
    int ret = avformat_write_header(formatContext_.get(), nullptr);
    FALSE_RETURN_V_MSG_E(ret >= 0, Status::ERROR_UNKNOWN, "failed to write header " PUBLIC_LOG_S,
        AVStrError(ret).c_str());
    return Status::OK;
}

Status FFmpegMuxerPlugin::WriteFrame(const std::shared_ptr<Plugin::Buffer>& buffer)
{
    FALSE_RETURN_V(buffer != nullptr && !buffer->IsEmpty(), Status::ERROR_INVALID_PARAMETER);
    uint32_t trackId = buffer->trackID;
    FALSE_RETURN_V(trackId < formatContext_->nb_streams, Status::ERROR_INVALID_PARAMETER);
    (void)memset_s(cachePacket_.get(), sizeof(AVPacket), 0, sizeof(AVPacket));
    auto memory = buffer->GetMemory();
    cachePacket_->data = const_cast<uint8_t*>(memory->GetReadOnlyData());
    cachePacket_->size = memory->GetSize();
    cachePacket_->stream_index = static_cast<int>(trackId);
    cachePacket_->pts = ConvertTimeToFFmpeg(buffer->pts, formatContext_->streams[trackId]->time_base);
    cachePacket_->dts = cachePacket_->pts;
    cachePacket_->flags = 0;
    if (buffer->flag & BUFFER_FLAG_KEY_FRAME) {
        MEDIA_LOG_D("It is key frame");
        cachePacket_->flags |= AV_PKT_FLAG_KEY;
    }
    cachePacket_->duration = ConvertTimeToFFmpeg(buffer->duration, formatContext_->streams[trackId]->time_base);
    auto ret = av_write_frame(formatContext_.get(), cachePacket_.get());
    if (ret < 0) {
        MEDIA_LOG_D("failed to write frame " PUBLIC_LOG_S, AVStrError(ret).c_str());
        av_packet_unref(cachePacket_.get());
        return Status::ERROR_UNKNOWN;
    }
    av_packet_unref(cachePacket_.get());
    return Status::OK;
}

Status FFmpegMuxerPlugin::WriteTrailer()
{
    FALSE_RETURN_V(ioContext_.dataSink_ != nullptr && outputFormat_ != nullptr, Status::ERROR_WRONG_STATE);
    OSAL::ScopedLock lock(fmtMutex_);
    FALSE_RETURN_V(formatContext_ != nullptr, Status::ERROR_WRONG_STATE);
    av_write_frame(formatContext_.get(), nullptr); // flush out cache data
    int ret = av_write_trailer(formatContext_.get());
    if (ret != 0) {
        MEDIA_LOG_E("failed to write trailer " PUBLIC_LOG_S, AVStrError(ret).c_str());
    }
    avio_flush(formatContext_->pb);
    return Status::OK;
}

Status FFmpegMuxerPlugin::SetCallback(Callback* cb)
{
    return Status::END_OF_STREAM;
}

std::shared_ptr<Allocator> FFmpegMuxerPlugin::GetAllocator()
{
    return {};
}

int32_t FFmpegMuxerPlugin::IoRead(void* opaque, uint8_t* buf, int bufSize)
{
    (void)opaque;
    (void)buf;
    (void)bufSize;
    return 0;
}
int32_t FFmpegMuxerPlugin::IoWrite(void* opaque, uint8_t* buf, int bufSize)
{
    auto ioCtx = static_cast<IOContext*>(opaque);
    if (ioCtx && ioCtx->dataSink_) {
        auto buffer = std::make_shared<Buffer>();
        auto bufferMem = buffer->AllocMemory(nullptr, bufSize);
        buffer->GetMemory()->Write(buf, bufSize, 0); // copy to buffer
        auto res = ioCtx->dataSink_->WriteAt(ioCtx->pos_, buffer);
        if (res == Status::OK) {
            ioCtx->pos_ += bufferMem->GetSize();
            if (ioCtx->pos_ > ioCtx->end_) {
                ioCtx->end_ = ioCtx->pos_;
            }
            return bufferMem->GetSize();
        }
        return 0;
    }
    return -1;
}

int64_t FFmpegMuxerPlugin::IoSeek(void* opaque, int64_t offset, int whence)
{
    auto ioContext = static_cast<IOContext*>(opaque);
    uint64_t newPos = 0;
    switch (whence) {
        case SEEK_SET:
            newPos = static_cast<uint64_t>(offset);
            ioContext->pos_ = newPos;
            MEDIA_LOG_I("AVSeek whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64 ", newPos = " PUBLIC_LOG_U64,
                        whence, offset, newPos);
            break;
        case SEEK_CUR:
            newPos = ioContext->pos_ + offset;
            MEDIA_LOG_I("AVSeek whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64 ", newPos = " PUBLIC_LOG_U64,
                        whence, offset, newPos);
            break;
        case SEEK_END:
        case AVSEEK_SIZE:
            newPos = ioContext->end_ + offset;
            MEDIA_LOG_I("AVSeek seek end whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64, whence, offset);
            break;
        default:
            MEDIA_LOG_E("AVSeek unexpected whence: " PUBLIC_LOG_D32, whence);
            break;
    }
    if (whence != AVSEEK_SIZE) {
        ioContext->pos_ = newPos;
    }
    MEDIA_LOG_DD("current offset: " PUBLIC_LOG_D64 ", new pos: " PUBLIC_LOG_U64, ioContext->pos_, newPos);
    return newPos;
}
} // Ffmpeg
} // Plugin
} // Media
} // OHOS