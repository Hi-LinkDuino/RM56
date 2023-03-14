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

#ifndef HISTREAMER_SDL_VIDEO_SINK_PLUGIN_H
#define HISTREAMER_SDL_VIDEO_SINK_PLUGIN_H

#ifdef VIDEO_SUPPORT

#include <atomic>
#include <memory>
#include "SDL.h"
#include "plugin/interface/video_sink_plugin.h"
#include "plugin/common/plugin_video_tags.h"

#ifdef DUMP_RAW_DATA
#include <cstdio>
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Sdl {
class SdlVideoSinkPlugin : public VideoSinkPlugin, public std::enable_shared_from_this<SdlVideoSinkPlugin> {
public:
    explicit SdlVideoSinkPlugin(std::string name);
    ~SdlVideoSinkPlugin() override = default;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType &value) override;

    Status SetParameter(Tag tag, const ValueType &value) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;

    Status Pause() override;

    Status Resume() override;

    Status Write(const std::shared_ptr<Buffer> &input) override;

    Status Flush() override;

    Status GetLatency(uint64_t &nanoSec) override;

private:

    uint32_t windowWidth_;
    uint32_t windowHeight_;
    uint32_t pixelWidth_;
    uint32_t pixelHeight_;
    uint32_t pixelFormat_;

    std::shared_ptr<SDL_Window> window_ {nullptr};
    std::shared_ptr<SDL_Renderer> renderer_ {nullptr};
    SDL_RendererInfo rendererInfo_ = {0};
    std::shared_ptr<SDL_Texture> texture_ {nullptr};
    SDL_Rect textureRect_ = {0};
    uint64_t curPts_ = {0};

#ifdef DUMP_RAW_DATA
    std::FILE* dumpFd_;
#endif

    void UpdateTextureRect();
    Status CreateSdlDispTexture();
    Status CreateSdlDispContext();
    bool IsFormatYUV();
    bool IsFormatNV();
    bool IsFormatRGB();
    Status VideoImageDisaplay(const std::shared_ptr<Buffer> &inputInfo);
    bool HandleSdlEvent();

    int32_t UpdateYUVTexture(const uint8_t **data, int32_t *lineSize, const std::shared_ptr<VideoBufferMeta> &videoMeta,
                     int32_t ySize, int32_t uvSize) const;

    int32_t UpdateNVTexture(const uint8_t **data, int32_t *lineSize, const std::shared_ptr<VideoBufferMeta> &videoMeta,
                    int32_t ySize, const uint8_t *ptr) const;
};
}
}
}
}

#endif

#endif // MEDIA_PIPELINE_SDL_VIDEO_SINK_PLUGIN_H
