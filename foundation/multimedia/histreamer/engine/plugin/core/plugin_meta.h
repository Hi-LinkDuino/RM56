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

#ifndef HISTREAMER_PLUGIN_META_H
#define HISTREAMER_PLUGIN_META_H

#include <vector>

#include "common/plugin_tags.h"
#include "common/plugin_types.h"
#include "foundation/cpp_ext/type_traits_ext.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * Meta ID is used to describe the metadata of media files or data streams.
 * All Meta ID must come from Tag.
 *
 * For details about the definition and usage, to see enum Tag in file plugin_tags.h.
 */
enum struct MetaID : uint32_t {
    MIME = CppExt::to_underlying(Tag::MIME),
    TRACK_ID = CppExt::to_underlying(Tag::TRACK_ID),
    MEDIA_CODEC_CONFIG = CppExt::to_underlying(Tag::MEDIA_CODEC_CONFIG),

    AUDIO_CHANNELS = CppExt::to_underlying(Tag::AUDIO_CHANNELS),
    AUDIO_SAMPLE_RATE = CppExt::to_underlying(Tag::AUDIO_SAMPLE_RATE),
    AUDIO_SAMPLE_FORMAT = CppExt::to_underlying(Tag::AUDIO_SAMPLE_FORMAT),
    AUDIO_SAMPLE_PER_FRAME = CppExt::to_underlying(Tag::AUDIO_SAMPLE_PER_FRAME),
    AUDIO_CHANNEL_LAYOUT = CppExt::to_underlying(Tag::AUDIO_CHANNEL_LAYOUT),
    AUDIO_OUTPUT_CHANNELS = CppExt::to_underlying(Tag::AUDIO_OUTPUT_CHANNELS),
    AUDIO_OUTPUT_CHANNEL_LAYOUT = CppExt::to_underlying(Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT),

    MEDIA_TITLE = CppExt::to_underlying(Tag::MEDIA_TITLE),
    MEDIA_ARTIST = CppExt::to_underlying(Tag::MEDIA_ARTIST),
    MEDIA_LYRICIST = CppExt::to_underlying(Tag::MEDIA_LYRICIST),
    MEDIA_ALBUM = CppExt::to_underlying(Tag::MEDIA_ALBUM),
    MEDIA_ALBUM_ARTIST = CppExt::to_underlying(Tag::MEDIA_ALBUM_ARTIST),
    MEDIA_DATE = CppExt::to_underlying(Tag::MEDIA_DATE),
    MEDIA_COMMENT = CppExt::to_underlying(Tag::MEDIA_COMMENT),
    MEDIA_GENRE = CppExt::to_underlying(Tag::MEDIA_GENRE),
    MEDIA_DESCRIPTION = CppExt::to_underlying(Tag::MEDIA_DESCRIPTION),
    MEDIA_COPYRIGHT = CppExt::to_underlying(Tag::MEDIA_COPYRIGHT),
    MEDIA_LANGUAGE = CppExt::to_underlying(Tag::MEDIA_LANGUAGE),
    MEDIA_LYRICS = CppExt::to_underlying(Tag::MEDIA_LYRICS),
    MEDIA_DURATION = CppExt::to_underlying(Tag::MEDIA_DURATION),
    MEDIA_BITRATE = CppExt::to_underlying(Tag::MEDIA_BITRATE),
    MEDIA_START_TIME = CppExt::to_underlying(Tag::MEDIA_START_TIME),
    MEDIA_FILE_EXTENSION = CppExt::to_underlying(Tag::MEDIA_FILE_EXTENSION),
    MEDIA_FILE_SIZE = CppExt::to_underlying(Tag::MEDIA_FILE_SIZE),
    MEDIA_SEEKABLE = CppExt::to_underlying(Tag::MEDIA_SEEKABLE),

    AUDIO_MPEG_VERSION = CppExt::to_underlying(Tag::AUDIO_MPEG_VERSION),
    AUDIO_MPEG_LAYER = CppExt::to_underlying(Tag::AUDIO_MPEG_LAYER),
    AUDIO_AAC_PROFILE = CppExt::to_underlying(Tag::AUDIO_AAC_PROFILE),
    AUDIO_AAC_LEVEL = CppExt::to_underlying(Tag::AUDIO_AAC_LEVEL),
    AUDIO_AAC_STREAM_FORMAT = CppExt::to_underlying(Tag::AUDIO_AAC_STREAM_FORMAT),

    VIDEO_WIDTH = CppExt::to_underlying(Tag::VIDEO_WIDTH),
    VIDEO_HEIGHT = CppExt::to_underlying(Tag::VIDEO_HEIGHT),
    VIDEO_PIXEL_FORMAT = CppExt::to_underlying(Tag::VIDEO_PIXEL_FORMAT),
    VIDEO_FRAME_RATE = CppExt::to_underlying(Tag::VIDEO_FRAME_RATE),
    VIDEO_H264_PROFILE = CppExt::to_underlying(Tag::VIDEO_H264_PROFILE),
    VIDEO_H264_LEVEL = CppExt::to_underlying(Tag::VIDEO_H264_LEVEL),

    BITS_PER_CODED_SAMPLE = CppExt::to_underlying(Tag::BITS_PER_CODED_SAMPLE),
};

class Meta {
public:
    explicit Meta() = default;
    ~Meta();
    bool Empty() const;
    bool SetString(Plugin::MetaID id, const std::string& value);
    bool SetInt32(Plugin::MetaID id, int32_t value);
    bool SetUint32(Plugin::MetaID id, uint32_t value);
    bool SetInt64(Plugin::MetaID id, int64_t value);
    bool SetUint64(Plugin::MetaID id, uint64_t value);
    bool SetFloat(Plugin::MetaID id, float value);

    /**
     * this function will copy from ptr with size.
     * @param ptr pointer
     * @param size size
     * @return
     */
    bool SetPointer(Plugin::MetaID, const void* ptr, size_t size); // NOLINT: void*

    bool GetString(Plugin::MetaID id, std::string& value) const;
    bool GetInt32(Plugin::MetaID id, int32_t& value) const;
    bool GetUint32(Plugin::MetaID id, uint32_t& value) const;
    bool GetInt64(Plugin::MetaID id, int64_t& value) const;
    bool GetUint64(Plugin::MetaID id, uint64_t& value) const;
    bool GetFloat(Plugin::MetaID id, float& value) const;

    /**
     * this function will copy from inner storage with size if exists. The user should delete the memory when it's not
     * needed.
     * @param ptr pointer
     * @param size size
     * @return
     */
    bool GetPointer(Plugin::MetaID, void** ptr, size_t& size) const; // NOLINT: void*

    template <typename T>
    bool GetData(Plugin::MetaID id, T& value) const
    {
        auto ite = items_.find(id);
        if (ite == items_.end() || !ite->second.SameTypeWith(typeid(T))) {
            return false;
        }
        value = Plugin::AnyCast<T>(ite->second);
        return true;
    }

    bool GetData(Plugin::MetaID id, Plugin::ValueType& value) const
    {
        auto ite = items_.find(id);
        if (ite == items_.end()) {
            return false;
        }
        value = ite->second;
        return true;
    }

    const Plugin::ValueType* GetData(MetaID id) const
    {
        auto ite = items_.find(id);
        if (ite == items_.end()) {
            return nullptr;
        }
        return &(ite->second);
    }

    void Clear();

    /**
     * remove item with the input name
     * @param name target name
     * @return true if the target exists; or false.
     */
    bool Remove(Plugin::MetaID id);

    void Update(const Meta& meta);

    /**
     * user should always use like SetData<T> to set data, otherwise it may be wrong to GetData.
     * e.g. for string  if use SetData(id, "abc") then GetData<std::string>(id, value) cannot work.
     * for uint64 if use SetData(id, 1), then GetData<uint64>(id, value) cannot work.
     *
     * @tparam T
     * @param id
     * @param value
     * @return
     */
    template <typename T>
    bool SetData(Plugin::MetaID id, const T& value)
    {
        items_[id] = value;
        return true;
    }

    bool SetData(Plugin::MetaID id, const Plugin::ValueType& value)
    {
        items_[id] = value;
        return true;
    }

    std::vector<MetaID> GetMetaIDs() const;

private:
    std::map<MetaID, Plugin::ValueType> items_ {};
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_META_H
