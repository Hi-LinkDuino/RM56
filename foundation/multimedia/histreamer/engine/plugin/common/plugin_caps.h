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

#ifndef HISTREAMER_PLUGIN_COMMON_CAPS_H
#define HISTREAMER_PLUGIN_COMMON_CAPS_H

#include <utility>
#include <vector> // NOLINT: used it
#include "plugin_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/// Indicates that the available capability type is an fixed value.
template <typename T> using FixedCapability = T;

/// Indicates that the available capability type is an interval value.
template <typename T> using IntervalCapability  = std::pair<T, T>;

/// Indicates that the available capability types are discrete values.
template <typename T> using DiscreteCapability = std::vector<T>;

/**
 * @brief The Capability describes the input and output capabilities of the plugin.
 *
 * It is basically a set of tags attached to the mime-type in order to
 * describe the mime-type more closely.
 *
 * @since 1.0
 * @version 1.0
 */
struct Capability {
    /**
     * @enum Capability ID is used to describe plugin capabilities or support capability matching.
     * All Capability ID must come from Tag.
     *
     * For details about the definition and usage, to see enum Tag in file plugin_tags.h.
     *
     * @since 1.0
     * @version 1.0
     */
    enum struct Key : uint32_t {
        MEDIA_BITRATE = static_cast<uint32_t>(Tag::MEDIA_BITRATE),
        AUDIO_SAMPLE_RATE = static_cast<uint32_t>(Tag::AUDIO_SAMPLE_RATE),
        AUDIO_CHANNELS = static_cast<uint32_t>(Tag::AUDIO_CHANNELS),
        AUDIO_CHANNEL_LAYOUT = static_cast<uint32_t>(Tag::AUDIO_CHANNEL_LAYOUT),
        AUDIO_SAMPLE_FORMAT = static_cast<uint32_t>(Tag::AUDIO_SAMPLE_FORMAT),
        AUDIO_MPEG_VERSION = static_cast<uint32_t>(Tag::AUDIO_MPEG_VERSION),
        AUDIO_MPEG_LAYER = static_cast<uint32_t>(Tag::AUDIO_MPEG_LAYER),
        AUDIO_AAC_PROFILE = static_cast<uint32_t>(Tag::AUDIO_AAC_PROFILE),
        AUDIO_AAC_LEVEL = static_cast<uint32_t>(Tag::AUDIO_AAC_LEVEL),
        AUDIO_AAC_STREAM_FORMAT = static_cast<uint32_t>(Tag::AUDIO_AAC_STREAM_FORMAT),
        VIDEO_PIXEL_FORMAT = static_cast<uint32_t>(Tag::VIDEO_PIXEL_FORMAT),
    };

    /// Used to store the capability in the key-value format.
    using KeyMap = std::map<Key, ValueType>;

    /// default constructor
    Capability() = default;

    /**
     * @brief constructor one capability with mime of m
     *
     * @param m mime string
     */
    explicit Capability(std::string  m):mime(std::move(m)){}

    /**
     * @brief Append one fix key into KeyMap
     *
     * @tparam T type of value
     * @param key Capability::Key
     * @param val value
     * @return reference of object
     */
    template<typename T>
    Capability& AppendFixedKey(Key key, const T& val)
    {
        keys[key] = val;
        return *this;
    }

    /**
     * @brief Append one interval key i.e. [rangeStart, rangeEnd] into KeyMap
     *
     * @tparam T type of value
     * @param key Capability::Key
     * @param rangeStart range start
     * @param rangeEnd rang end
     * @return reference of object
     */
    template<typename T>
    Capability& AppendIntervalKey(Key key, const T& rangeStart, const T& rangeEnd)
    {
        keys[key] = std::make_pair(rangeStart, rangeEnd);
        return *this;
    }

    /**
     * @brief Append one discrete key i.e. {val1, val2, ....} into KeyMap
     *
     * @tparam T type of value
     * @param key Capability::Key
     * @param discreteValues values
     * @return reference of object
     */
    template<typename T>
    Capability& AppendDiscreteKeys(Key key, DiscreteCapability<T> discreteValues)
    {
        keys[key] = std::move(discreteValues);
        return *this;
    }

    /**
     * @brief set mime of this capability
     *
     * @param val mime value
     * @return reference of object
     */
    Capability& SetMime(std::string val)
    {
        mime = std::move(val);
        return *this;
    }

    /// mime of capability
    std::string mime;

    /// Store the parameters(Capability::Key, value pairs), which should be negotiated
    KeyMap keys;
};

/// A collection of multiple capabilities
using CapabilitySet = std::vector<Capability>;
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_COMMON_CAPS_H
