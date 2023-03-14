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

#ifdef VIDEO_SUPPORT

#define HST_LOG_TAG "SdlVideoSinkPlugin"

#include "sdl_video_sink_plugin.h"
#include <functional>
#include "SDL_events.h"
#include "foundation/log.h"
#include "utils/constants.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "plugin/common/plugin_video_tags.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"

namespace {
using namespace OHOS::Media::Plugin;
using namespace Sdl;

std::shared_ptr<VideoSinkPlugin> VideoSinkPluginCreator(const std::string& name)
{
    return std::make_shared<SdlVideoSinkPlugin>(name);
}

const Status SdlVideoRegister(const std::shared_ptr<Register>& reg)
{
    VideoSinkPluginDef definition;
    definition.name = "sdl_video_sink";
    definition.rank = 100; // 100
    Capability cap(OHOS::Media::MEDIA_MIME_VIDEO_RAW);
    cap.AppendDiscreteKeys<VideoPixelFormat>(
        Capability::Key::VIDEO_PIXEL_FORMAT,
        {VideoPixelFormat::YUV420P, VideoPixelFormat::NV12, VideoPixelFormat::NV21, VideoPixelFormat::RGBA});
    definition.inCaps.emplace_back(cap);
    definition.creator = VideoSinkPluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(SdlVideoSink, LicenseType::LGPL, SdlVideoRegister, [] {});
} // namespace

namespace OHOS {
namespace Media {
namespace Plugin {
const uint32_t DEFAULT_WINDOW_WIDTH = 640;
const uint32_t DEFAULT_WINDOW_HEIGHT = 480;

static uint32_t TranslatePixelFormat(const VideoPixelFormat pixelFormat)
{
    uint32_t sdlFormat = SDL_PIXELFORMAT_UNKNOWN;
    switch (pixelFormat) {
        case VideoPixelFormat::YUV420P:
            sdlFormat = SDL_PIXELFORMAT_IYUV;
            break;
        case VideoPixelFormat::YUYV422:
            sdlFormat = SDL_PIXELFORMAT_YUY2;
            break;
        case VideoPixelFormat::RGB24:
            sdlFormat = SDL_PIXELFORMAT_RGB24;
            break;
        case VideoPixelFormat::BGR24:
            sdlFormat = SDL_PIXELFORMAT_BGR24;
            break;
        case VideoPixelFormat::RGBA:
            sdlFormat = SDL_PIXELFORMAT_ABGR8888;
            break;
        case VideoPixelFormat::ARGB:
            sdlFormat = SDL_PIXELFORMAT_BGRA8888;
            break;
        case VideoPixelFormat::ABGR:
            sdlFormat = SDL_PIXELFORMAT_RGBA8888;
            break;
        case VideoPixelFormat::BGRA:
            sdlFormat = SDL_PIXELFORMAT_ARGB8888;
            break;
        case VideoPixelFormat::YUV422P:
        case VideoPixelFormat::YUV444P:
        case VideoPixelFormat::YUV410P:
        case VideoPixelFormat::YUV411P:
        case VideoPixelFormat::GRAY8:
        case VideoPixelFormat::MONOWHITE:
        case VideoPixelFormat::MONOBLACK:
        case VideoPixelFormat::PAL8:
        case VideoPixelFormat::YUVJ420P:
        case VideoPixelFormat::YUVJ422P:
        case VideoPixelFormat::YUVJ444P:
            break;
        case VideoPixelFormat::NV12:
            sdlFormat = SDL_PIXELFORMAT_NV12;
            break;
        case VideoPixelFormat::NV21:
            sdlFormat = SDL_PIXELFORMAT_NV21;
            break;
        default:
            break;
    }
    return sdlFormat;
}

SdlVideoSinkPlugin::SdlVideoSinkPlugin(std::string name)
    : VideoSinkPlugin(std::move(name)),
      windowWidth_(DEFAULT_WINDOW_WIDTH),
      windowHeight_(DEFAULT_WINDOW_HEIGHT),
      pixelWidth_(0),
      pixelHeight_(0),
      pixelFormat_(0),
      curPts_(0)
{
}

Status SdlVideoSinkPlugin::Init()
{
    std::weak_ptr<SdlVideoSinkPlugin> weakPtr(shared_from_this());
    if (SDL_Init(SDL_INIT_VIDEO)) {
        MEDIA_LOG_E("Init SDL fail: " PUBLIC_LOG_S, SDL_GetError());
        return Status::ERROR_UNKNOWN;
    }
    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
#ifdef DUMP_RAW_DATA
    dumpFd_ = std::fopen("./vsink_out.yuv", "wb");
#endif
    return Status::OK;
}

Status SdlVideoSinkPlugin::Deinit()
{
    // SDL_Quit(); // avoid video/audio duplicate quit hang on.
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    return Status::OK;
}

void SdlVideoSinkPlugin::UpdateTextureRect()
{
    /* we suppose the screen has a 1/1 sample aspect ratio */
    if (windowWidth_ && windowHeight_) {
        /* fit in the window */
        double_t pixelRational = static_cast<double_t>(pixelWidth_) / static_cast<double_t>(pixelHeight_);
        double_t winRational = static_cast<double_t>(windowWidth_) / static_cast<double_t>(windowHeight_);
        if (pixelRational > winRational) {
            /* fit in width */
            textureRect_.w = static_cast<int32_t>(windowWidth_);
            textureRect_.h = static_cast<int32_t>(
                static_cast<double_t>(static_cast<uint64_t>(textureRect_.w) * static_cast<uint64_t>(pixelHeight_)) /
                static_cast<double_t>(pixelWidth_));
        } else {
            /* fit in height */
            textureRect_.h = static_cast<int32_t>(windowHeight_);
            textureRect_.w = static_cast<int32_t>(
                static_cast<double_t>(static_cast<uint64_t>(textureRect_.h) * static_cast<uint64_t>(pixelWidth_)) /
                static_cast<double_t>(pixelHeight_));
        }
    } else {
        textureRect_.h = static_cast<int32_t>(pixelHeight_);
        textureRect_.w = static_cast<int32_t>(
            static_cast<double_t>(static_cast<uint64_t>(textureRect_.h) * static_cast<uint64_t>(pixelWidth_)) /
            static_cast<double_t>(pixelHeight_));
        windowWidth_ = textureRect_.w;
        windowHeight_ = textureRect_.h;
    }

    textureRect_.x = (static_cast<int32_t>(windowWidth_) - textureRect_.w) / 2;  // 2
    textureRect_.y = (static_cast<int32_t>(windowHeight_) - textureRect_.h) / 2; // 2
    MEDIA_LOG_D("pixelWH[" PUBLIC_LOG_U32 ", " PUBLIC_LOG_U32 "], windowWH[" PUBLIC_LOG_U32 ", " PUBLIC_LOG_U32
        "], textureWH[" PUBLIC_LOG_U32 ", " PUBLIC_LOG_U32 "], textureXY[" PUBLIC_LOG_U32 ", "
        PUBLIC_LOG_U32 "]", pixelWidth_, pixelHeight_,
        windowWidth_, windowHeight_, textureRect_.w, textureRect_.h, textureRect_.x, textureRect_.y);
}

Status SdlVideoSinkPlugin::CreateSdlDispContext()
{
    UpdateTextureRect();
    uint32_t sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    // also can call SDL_CreateWindowAndRenderer() to combine SDL_CreateWindow() and SDL_CreateRenderer()
    // can use width_ and height_ to set screen_x and screen_y
    SDL_Window* screen =
        SDL_CreateWindow("SDL2 Video Sink", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         static_cast<int32_t>(windowWidth_), static_cast<int32_t>(windowHeight_), sdlFlags);
    if (screen == nullptr) {
        MEDIA_LOG_E("Create window fail: " PUBLIC_LOG_S, SDL_GetError());
        return Status::ERROR_UNKNOWN;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    window_ = std::shared_ptr<SDL_Window>(screen, [](SDL_Window* ptr) {
        if (ptr) {
            SDL_DestroyWindow(ptr);
        }
    });
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window_.get(), -1,
        SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC); // flags: SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    if (renderer != nullptr) {
        if (!SDL_GetRendererInfo(renderer, &rendererInfo_)) {
            MEDIA_LOG_I("Init " PUBLIC_LOG_S " renderer success", rendererInfo_.name);
        }
    }
    if ((renderer == nullptr) || (!rendererInfo_.num_texture_formats)) {
        MEDIA_LOG_E("Create renderer fail: " PUBLIC_LOG_S, SDL_GetError());
        return Status::ERROR_UNKNOWN;
    }
    renderer_ = std::shared_ptr<SDL_Renderer>(renderer, [](SDL_Renderer* ptr) {
        if (ptr) {
            SDL_DestroyRenderer(ptr);
        }
    });
    return Status::OK;
}

Status SdlVideoSinkPlugin::CreateSdlDispTexture()
{
    SDL_Texture* texture =
        SDL_CreateTexture(renderer_.get(), pixelFormat_, SDL_TEXTUREACCESS_STREAMING, pixelWidth_, pixelHeight_);
    if (texture == nullptr) {
        MEDIA_LOG_E("Create texture fail: " PUBLIC_LOG_S, SDL_GetError());
        return Status::ERROR_UNKNOWN;
    }
    texture_ = std::shared_ptr<SDL_Texture>(texture, [](SDL_Texture* ptr) {
        if (ptr) {
            SDL_DestroyTexture(ptr);
        }
    });
    return Status::OK;
}

Status SdlVideoSinkPlugin::Prepare()
{
    auto err = CreateSdlDispContext();
    if (err != Status::OK) {
        return err;
    }
    err = CreateSdlDispTexture();
    if (err != Status::OK) {
        return err;
    }
    return Status::OK;
}

Status SdlVideoSinkPlugin::Reset()
{
    window_ = nullptr;
    renderer_ = nullptr;
    texture_ = nullptr;
    SDL_memset(static_cast<void*>(&rendererInfo_), sizeof(SDL_RendererInfo), 0);
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    return Status::OK;
}

Status SdlVideoSinkPlugin::Start()
{
    MEDIA_LOG_I("SDL video sink start ...");
    return Status::OK;
}

Status SdlVideoSinkPlugin::Stop()
{
    MEDIA_LOG_I("SDL video sink stop ...");
    return Status::OK;
}

Status SdlVideoSinkPlugin::GetParameter(Tag tag, ValueType& value)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlVideoSinkPlugin::SetParameter(Tag tag, const ValueType& value)
{
    switch (tag) {
        case Tag::VIDEO_WIDTH: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                pixelWidth_ = Plugin::AnyCast<uint32_t>(value);
                MEDIA_LOG_D("pixelWidth_: " PUBLIC_LOG_U32, pixelWidth_);
            }
            break;
        }
        case Tag::VIDEO_HEIGHT: {
            if (value.SameTypeWith(typeid(uint32_t))) {
                pixelHeight_ = Plugin::AnyCast<uint32_t>(value);
                MEDIA_LOG_D("pixelHeight_: " PUBLIC_LOG_U32, pixelHeight_);
            }
            break;
        }
        case Tag::VIDEO_PIXEL_FORMAT: {
            if (value.SameTypeWith(typeid(VideoPixelFormat))) {
                VideoPixelFormat format = Plugin::AnyCast<VideoPixelFormat>(value);
                pixelFormat_ = TranslatePixelFormat(format);
                MEDIA_LOG_D("SDL pixelFormat: " PUBLIC_LOG_U32, pixelFormat_);
            }
            break;
        }
        default:
            MEDIA_LOG_I("Unknown key");
            break;
    }
    return Status::OK;
}

std::shared_ptr<Allocator> SdlVideoSinkPlugin::GetAllocator()
{
    return nullptr;
}

Status SdlVideoSinkPlugin::SetCallback(Callback* cb)
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlVideoSinkPlugin::Pause()
{
    return Status::OK;
}

Status SdlVideoSinkPlugin::Resume()
{
    return Status::OK;
}

bool SdlVideoSinkPlugin::IsFormatYUV()
{
    return pixelFormat_ == SDL_PIXELFORMAT_IYUV;
}

bool SdlVideoSinkPlugin::IsFormatNV()
{
    return pixelFormat_ == SDL_PIXELFORMAT_NV12 || pixelFormat_ == SDL_PIXELFORMAT_NV21;
}

bool SdlVideoSinkPlugin::IsFormatRGB()
{
    return pixelFormat_ == SDL_PIXELFORMAT_RGB332 || pixelFormat_ == SDL_PIXELFORMAT_RGB444 ||
           pixelFormat_ == SDL_PIXELFORMAT_RGB555 || pixelFormat_ == SDL_PIXELFORMAT_BGR555 ||
           pixelFormat_ == SDL_PIXELFORMAT_RGB565 || pixelFormat_ == SDL_PIXELFORMAT_BGR565 ||
           pixelFormat_ == SDL_PIXELFORMAT_RGB24 || pixelFormat_ == SDL_PIXELFORMAT_BGR24 ||
           pixelFormat_ == SDL_PIXELFORMAT_RGB888 || pixelFormat_ == SDL_PIXELFORMAT_RGBX8888 ||
           pixelFormat_ == SDL_PIXELFORMAT_BGR888 || pixelFormat_ == SDL_PIXELFORMAT_BGRX8888 ||
           pixelFormat_ == SDL_PIXELFORMAT_ARGB8888 || pixelFormat_ == SDL_PIXELFORMAT_RGBA8888 ||
           pixelFormat_ == SDL_PIXELFORMAT_ABGR8888 || pixelFormat_ == SDL_PIXELFORMAT_BGRA8888;
}

Status SdlVideoSinkPlugin::VideoImageDisaplay(const std::shared_ptr<Buffer>& inputInfo)
{
    int32_t ret = -1;
    const uint8_t* data[4] = {nullptr}; // 4
    int32_t lineSize[4] = {0};          // 4
    auto bufferMeta = inputInfo->GetBufferMeta();
    if (bufferMeta == nullptr || bufferMeta->GetType() != BufferMetaType::VIDEO) {
        MEDIA_LOG_E("Invalid video buffer");
        return Status::ERROR_INVALID_DATA;
    }
    std::shared_ptr<VideoBufferMeta> videoMeta = std::dynamic_pointer_cast<VideoBufferMeta>(bufferMeta);
    uint32_t frameFormat = TranslatePixelFormat(videoMeta->videoPixelFormat);
    if (frameFormat != pixelFormat_) {
        MEDIA_LOG_I("pixel format change from " PUBLIC_LOG_U32 " to " PUBLIC_LOG_U32, pixelFormat_, frameFormat);
        pixelFormat_ = frameFormat;
    }
    if ((videoMeta->width != pixelWidth_) || (videoMeta->height != pixelHeight_)) {
        MEDIA_LOG_E("WH[" PUBLIC_LOG_U32 "," PUBLIC_LOG_U32 "] change to WH[" PUBLIC_LOG_U32 "," PUBLIC_LOG_U32 "]",
                    pixelWidth_, pixelHeight_, videoMeta->width, videoMeta->height);
        // do something
    }
    int32_t ySize = 0;
    int32_t uvSize = 0;
    auto bufferMem = inputInfo->GetMemory();
    auto ptr = bufferMem->GetReadOnlyData();
    data[0] = ptr;
    lineSize[0] = static_cast<int32_t>(videoMeta->stride[0]);
    MEDIA_LOG_DD("Display one frame: WHS[" PUBLIC_LOG_U32 "," PUBLIC_LOG_U32 "," PUBLIC_LOG_U32 "]",
                 pixelWidth_, pixelHeight_, lineSize[0]);
    if (IsFormatYUV()) {
        if (videoMeta->planes != 3) { // 3
            MEDIA_LOG_E("Invalid video buffer, planes: " PUBLIC_LOG_U32, videoMeta->planes);
            return Status::ERROR_INVALID_DATA;
        }
        ret = UpdateYUVTexture(data, lineSize, videoMeta, ySize, uvSize);
    } else if (IsFormatNV()) {
        if (videoMeta->planes != 2) { // 2
            MEDIA_LOG_E("Invalid video buffer, planes: " PUBLIC_LOG_U32, videoMeta->planes);
            return Status::ERROR_INVALID_DATA;
        }
        ret = UpdateNVTexture(data, lineSize, videoMeta, ySize, ptr);
    } else if (IsFormatRGB()) {
        ret = SDL_UpdateTexture(texture_.get(), NULL, data[0], lineSize[0]);
    } else {
        MEDIA_LOG_E("Unsupported pixel format");
    }
    SDL_RenderClear(renderer_.get());
    SDL_RenderCopy(renderer_.get(), texture_.get(), NULL, &textureRect_);
    SDL_RenderPresent(renderer_.get());
    return (ret != 0) ? Status::ERROR_UNKNOWN : Status::OK;
}

int32_t SdlVideoSinkPlugin::UpdateNVTexture(const uint8_t** data, int32_t* lineSize,
                                            const std::shared_ptr<VideoBufferMeta>& videoMeta, int32_t ySize,
                                            const uint8_t* ptr) const
{
    int32_t ret;
    lineSize[1] = static_cast<int32_t>(videoMeta->stride[1]);
    ySize = lineSize[0] * static_cast<int32_t>(AlignUp(pixelHeight_, 16)); // 16
    MEDIA_LOG_D("lineSize[0]: " PUBLIC_LOG_D32 ", lineSize[1]: " PUBLIC_LOG_D32 ", ySize: " PUBLIC_LOG_D32,
                lineSize[0], lineSize[1], ySize);
    data[1] = ptr + ySize;
#ifdef DUMP_RAW_DATA
    if (dumpFd_ && data[0] != nullptr && lineSize[0] != 0) {
        std::fwrite(reinterpret_cast<const char*>(data[0]), lineSize[0] * pixelHeight_,
                    1, dumpFd_);
    }
    if (dumpFd_ && data[1] != nullptr && lineSize[1] != 0) {
        std::fwrite(reinterpret_cast<const char*>(data[1]), lineSize[1] * pixelHeight_ / 2, // 2
                    1, dumpFd_);
    }
#endif
    ret = SDL_UpdateTexture(texture_.get(), NULL, data[0], lineSize[0]);
    return ret;
}

int32_t SdlVideoSinkPlugin::UpdateYUVTexture(const uint8_t** data, int32_t* lineSize,
                                             const std::shared_ptr<VideoBufferMeta>& videoMeta, int32_t ySize,
                                             int32_t uvSize) const
{
    int32_t ret;
    lineSize[1] = static_cast<int32_t>(videoMeta->stride[1]);
    lineSize[2] = static_cast<int32_t>(videoMeta->stride[2]); // 2
    ySize = lineSize[0] * static_cast<int32_t>(AlignUp(pixelHeight_, 16)); // 16
    uvSize = lineSize[1] * static_cast<int32_t>(AlignUp(pixelHeight_, 16)) / 2; // 2, 16
    data[1] = data[0] + ySize;
    data[2] = data[1] + uvSize; // 2
#ifdef DUMP_RAW_DATA
    if (dumpFd_ && data[0] != nullptr && lineSize[0] != 0) {
        std::fwrite(reinterpret_cast<const char*>(data[0]), lineSize[0] * pixelHeight_,
                    1, dumpFd_);
    }
    if (dumpFd_ && data[1] != nullptr && lineSize[1] != 0) {
        std::fwrite(reinterpret_cast<const char*>(data[1]), lineSize[1] * pixelHeight_ / 2, // 2
                    1, dumpFd_);
    }
    if (dumpFd_ && data[2] != nullptr && lineSize[2] != 0) {                        // 2
        std::fwrite(reinterpret_cast<const char*>(data[2]), lineSize[2] * pixelHeight_ / 2, // 2
                    1, dumpFd_);
    }
#endif
    ret = SDL_UpdateYUVTexture(texture_.get(), NULL, data[0], lineSize[0], data[1], lineSize[1], data[2], // 2
                               lineSize[2]); // 2
    return ret;
}

bool SdlVideoSinkPlugin::HandleSdlEvent()
{
    SDL_Event event;
    bool isQuit = false;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isQuit = true;
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        windowWidth_ = event.window.data1;
                        windowHeight_ = event.window.data2;
                        UpdateTextureRect();
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return isQuit;
}

Status SdlVideoSinkPlugin::Write(const std::shared_ptr<Buffer>& inputInfo)
{
    MEDIA_LOG_DD("SDL sink write begin");
    if (inputInfo == nullptr || inputInfo->IsEmpty()) {
        return Status::OK;
    }
    if (HandleSdlEvent() == true) {
        MEDIA_LOG_W("SDL_QUIT, write nothing");
        return Status::ERROR_NOT_ENOUGH_DATA;
    }
    return VideoImageDisaplay(inputInfo);
}

Status SdlVideoSinkPlugin::Flush()
{
    return Status::OK;
}

Status SdlVideoSinkPlugin::GetLatency(uint64_t& nanoSec)
{
    nanoSec = 10 * HST_MSECOND; // 10 ms
    return Status::OK;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif