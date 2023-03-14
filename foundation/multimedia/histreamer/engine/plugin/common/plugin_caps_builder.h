/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_COMMON_CAPS_BUILDER_H
#define HISTREAMER_PLUGIN_COMMON_CAPS_BUILDER_H
#include <string>
#include <vector>
#include "plugin_audio_tags.h"
#include "plugin_caps.h"
#include "plugin_video_tags.h"
namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief CapabilityBuilder is used for easily building of Capability.
 *
 * CapabilityBuilder offers several setter functions for Capability::Key, including bit rate, sample format, audio
 * channels etc. Utilize this builder, user do not need to explicitly specify detail types of keys. After setting the
 * keys, call Build() to receive an available Capability. Calling Reset() will clear the keys and mime which have been
 * set before.
 *
 * @since 1.0
 * @version 1.0
 */
class CapabilityBuilder {
#define DECL_SET_FIX_CAP_FUNC(key, keyName, type) \
CapabilityBuilder& Set##keyName(type val) \
{ \
    cap_.keys[key] = val; \
    return *this; \
}

#define DECL_SET_RNG_CAP_FUNC(key, keyName, type) \
CapabilityBuilder& Set##keyName##Range(type val1, type val2) \
{ \
    auto min = std::min(val1, val2); \
    auto max = std::max(val1, val2); \
    cap_.keys[key] = std::make_pair(min, max); \
    return *this; \
}

#define DECL_SET_LIST_CAP_FUNC(key, keyName, type) \
CapabilityBuilder& Set##keyName##List(std::vector<type> val) \
{ \
    cap_.keys[key] = val; \
    return *this; \
}

#define DECL_SET_FRL_CAP_FUNCS(key, keyName, type) \
DECL_SET_FIX_CAP_FUNC(key, keyName, type); \
DECL_SET_RNG_CAP_FUNC(key, keyName, type); \
DECL_SET_LIST_CAP_FUNC(key, keyName, type)

#define DECL_SET_FL_CAP_FUNCS(key, keyName, type) \
DECL_SET_FIX_CAP_FUNC(key, keyName, type); \
DECL_SET_LIST_CAP_FUNC(key, keyName, type)

public:
    CapabilityBuilder() = default;

    /**
     * set mime
     *
     * @param mime mime of this builder.
     * @return CapabilityBuilder it self.
     */
    CapabilityBuilder& SetMime(std::string mime)
    {
        cap_.mime = std::move(mime);
        return *this;
    }

    DECL_SET_FRL_CAP_FUNCS(Capability::Key::MEDIA_BITRATE, BitRate, uint32_t);

    DECL_SET_FRL_CAP_FUNCS(Capability::Key::AUDIO_SAMPLE_RATE, AudioSampleRate, uint32_t);

    DECL_SET_FRL_CAP_FUNCS(Capability::Key::AUDIO_CHANNELS, AudioChannel, uint32_t);

    DECL_SET_FL_CAP_FUNCS(Capability::Key::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout, AudioChannelLayout);

    DECL_SET_FL_CAP_FUNCS(Capability::Key::AUDIO_SAMPLE_FORMAT, AudioSampleFormat, AudioSampleFormat);

    DECL_SET_FRL_CAP_FUNCS(Capability::Key::AUDIO_MPEG_VERSION, AudioMpegVersion, uint32_t);
    DECL_SET_FRL_CAP_FUNCS(Capability::Key::AUDIO_MPEG_LAYER, AudioMpegLayer, uint32_t);

    DECL_SET_FL_CAP_FUNCS(Capability::Key::AUDIO_AAC_PROFILE, AudioAacProfile, AudioAacProfile);

    DECL_SET_FRL_CAP_FUNCS(Capability::Key::AUDIO_AAC_LEVEL, AudioAacLevel, uint32_t);

    DECL_SET_FL_CAP_FUNCS(Capability::Key::AUDIO_AAC_STREAM_FORMAT, AudioAacStreamFormat, AudioAacStreamFormat);

    DECL_SET_FL_CAP_FUNCS(Capability::Key::VIDEO_PIXEL_FORMAT, VideoPixelFormat, VideoPixelFormat);

    /**
     *  @brief Build one Capability.
     *
     * @return capability
     */
    const Capability& Build() const
    {
        return cap_;
    }

    /**
     * Reset the keys and mime which have been set already.
     */
    void Reset()
    {
        cap_.mime.clear();
        cap_.keys.clear();
    }
private:
    Capability cap_;
#undef DECL_SET_FRL_CAP_FUNCS
#undef DECL_SET_FL_CAP_FUNCS
#undef DECL_SET_FIX_CAP_FUNC
#undef DECL_SET_RNG_CAP_FUNC
#undef DECL_SET_LIST_CAP_FUNC
};
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_PLUGIN_COMMON_CAPS_BUILDER_H
