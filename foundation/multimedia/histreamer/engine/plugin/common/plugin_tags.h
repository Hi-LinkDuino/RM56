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

#ifndef HISTREAMER_PLUGIN_COMMON_TAGS_H
#define HISTREAMER_PLUGIN_COMMON_TAGS_H

#include <map> // NOLINT
#include <string>
#include <vector> // NOLINT
#include "any.h" // NOLINT

namespace OHOS {
namespace Media {
namespace Plugin {
enum struct TagSection : uint8_t {
    REGULAR = 1,
    MEDIA = 2,
    AUDIO_UNIVERSAL = 3,
    AUDIO_SPECIFIC = 4,
    VIDEO_UNIVERSAL = 5,
    VIDEO_SPECIFIC = 6,
    MAX_SECTION = 64
};

enum struct AudioFormat : uint8_t {
    MPEG = 1,
    AAC,
};

enum struct VideoFormat : uint8_t {
    UNKNOWN = 0,
    H264 = 1,
    MPEG4 = 2,
};

#define MAKE_AUDIO_SPECIFIC_START(format) (SECTION_AUDIO_SPECIFIC_START | (static_cast<uint8_t>(format) << 8U))

#define MAKE_VIDEO_SPECIFIC_START(format) (SECTION_VIDEO_SPECIFIC_START | (static_cast<uint8_t>(format) << 8U))

/**
 * @brief Tag is a key-value pair used to settings or transfer information.
 *
 * The "key" member：An uint32_t index, defined as an enumerated type.
 *   Tag Index consisting of the following fragments:
 *   - reserved field
 *   - vendor extensions
 *   - section (regular, audio, video ...)
 *   - addition index
 *
 *   layout:
 *          +----------+---------+--------+----------------+
 *          | reserved | section | vendor | addition index |
 *          +----------+---------+--------+----------------+
 *    bit:   31 ... 22  21 ... 16    15    14 ............ 0
 *
 * The "value" member: Different tag have different value types,
 *                     which is defined in the plug-in interface.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct Tag : uint32_t {
    INVALID = 0,
    SECTION_REGULAR_START = static_cast<uint8_t>(TagSection::REGULAR) << 16U,                 // regular tag
    SECTION_MEDIA_START = static_cast<uint8_t>(TagSection::MEDIA) << 16U,                     // media tag
    SECTION_AUDIO_UNIVERSAL_START = static_cast<uint8_t>(TagSection::AUDIO_UNIVERSAL) << 16U, // audio universal tag
    SECTION_AUDIO_SPECIFIC_START = static_cast<uint8_t>(TagSection::AUDIO_SPECIFIC) << 16U,   // audio specific tag
    SECTION_VIDEO_UNIVERSAL_START = static_cast<uint8_t>(TagSection::VIDEO_UNIVERSAL) << 16U, // video universal tag
    SECTION_VIDEO_SPECIFIC_START = static_cast<uint8_t>(TagSection::VIDEO_SPECIFIC) << 16U,   // video specific tag

    /* -------------------- regular tag -------------------- */
    MIME = SECTION_REGULAR_START + 1, ///< std::string
    TRACK_ID,                         ///< uint32_t, track id
    REQUIRED_OUT_BUFFER_CNT,          ///< uint32_t, required buffer count of plugin; read only tag
    BUFFER_ALLOCATOR,                 ///< Allocator, allocator to alloc buffers
    BUFFERING_SIZE,                   ///< uint32_t, download buffer size
    WATERLINE_HIGH,                   ///< uint32_t, high waterline
    WATERLINE_LOW,                    ///< uint32_t, low waterline
    SRC_INPUT_TYPE,                   ///< @see SrcInputType
    BITS_PER_CODED_SAMPLE,            ///< uint32_t, bits per coded sample
    APP_TOKEN_ID,                     ///< uint32_t, app token id
    APP_UID,                          ///< int32_t, app uid

    /* -------------------- media tag -------------------- */
    MEDIA_TITLE = SECTION_MEDIA_START + 1, ///< string
    MEDIA_ARTIST,                          ///< std::string, artist
    MEDIA_LYRICIST,                        ///< std::string, lyricist
    MEDIA_ALBUM,                           ///< std::string, album
    MEDIA_ALBUM_ARTIST,                    ///< std::string, album artist
    MEDIA_DATE,                            ///< std::string, media date, format：YYYY-MM-DD
    MEDIA_COMMENT,                         ///< std::string, comment
    MEDIA_GENRE,                           ///< std::string, genre
    MEDIA_COPYRIGHT,                       ///< std::string, copyright
    MEDIA_LANGUAGE,                        ///< std::string, language
    MEDIA_DESCRIPTION,                     ///< std::string, description
    MEDIA_LYRICS,                          ///< std::string, cyrics
    MEDIA_DURATION,                        ///< int64_t, duration based on {@link HST_TIME_BASE}
    MEDIA_FILE_SIZE,                       ///< uint64_t, file size
    MEDIA_BITRATE,                         ///< int64_t, bite rate
    MEDIA_FILE_EXTENSION,                  ///< std::string, file extension
    MEDIA_CODEC_CONFIG,                    ///< std::vector<uint8_t>, codec config. e.g. AudioSpecificConfig for mp4
    MEDIA_POSITION,                        ///< uint64_t : The byte position within media stream/file
    MEDIA_START_TIME,                      ///< int64_t: The start time of one track
    MEDIA_SEEKABLE,                        ///< enum Seekable: Seekable status of the media

    /* -------------------- audio universal tag -------------------- */
    AUDIO_CHANNELS = SECTION_AUDIO_UNIVERSAL_START + 1, ///< uint32_t, stream channel num
    AUDIO_CHANNEL_LAYOUT,                               ///< @see AudioChannelLayout, stream channel layout
    AUDIO_SAMPLE_RATE,                                  ///< uint32_t, sample rate
    AUDIO_SAMPLE_FORMAT,                                ///< @see AudioSampleFormat
    AUDIO_SAMPLE_PER_FRAME,                             ///< uint32_t, sample per frame
    AUDIO_OUTPUT_CHANNELS,                              ///< uint32_t, sink output channel num
    AUDIO_OUTPUT_CHANNEL_LAYOUT,                        ///< @see AudioChannelLayout, sink output channel layout

    /* -------------------- audio specific tag -------------------- */
    AUDIO_SPECIFIC_MPEG_START = MAKE_AUDIO_SPECIFIC_START(AudioFormat::MPEG),
    AUDIO_MPEG_VERSION, ///< uint32_t, mpeg version
    AUDIO_MPEG_LAYER,   ///< uint32_t, mpeg layer

    AUDIO_SPECIFIC_AAC_START = MAKE_AUDIO_SPECIFIC_START(AudioFormat::AAC),
    AUDIO_AAC_PROFILE,       ///< @see AudioAacProfile
    AUDIO_AAC_LEVEL,         ///< uint32_t, acc level
    AUDIO_AAC_STREAM_FORMAT, ///< @see AudioAacStreamFormat

    /* -------------------- video universal tag -------------------- */
    VIDEO_WIDTH = SECTION_VIDEO_UNIVERSAL_START + 1, ///< uint32_t, video width
    VIDEO_HEIGHT,                                    ///< uint32_t, video height
    VIDEO_PIXEL_FORMAT,                              ///< @see VideoPixelFormat
    VIDEO_FRAME_RATE,                                ///< uint32_t, video frame rate
    VIDEO_SURFACE,                                   ///< @see class Surface
    VIDEO_MAX_SURFACE_NUM,                           ///< uint32_t, max video surface num
    VIDEO_CAPTURE_RATE,                              ///< double, video capture rate

    /* -------------------- video specific tag -------------------- */
    VIDEO_SPECIFIC_H264_START = MAKE_VIDEO_SPECIFIC_START(VideoFormat::H264),
    VIDEO_H264_PROFILE,      ///< @see VideoH264Profile
    VIDEO_H264_LEVEL,        ///< uint32_t, h264 level
};
/**
 * @enum Media protocol type.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct ProtocolType : uint32_t {
    UNKNOWN, ///< Unknown protocol
    FILE,    ///< File protocol, uri prefix: "file://"
    FD,      ///< File descriptor protocol, uri prefix: "fd://"
    STREAM,  ///< Stream protocol, uri prefix: "stream://"
    HTTP,    ///< Http protocol, uri prefix: "http://"
    HTTPS,   ///< Https protocol, uri prefix: "https://"
    HLS,     ///< Http live streaming protocol, uri prefix: "https://" or "https://" or "file://", suffix: ".m3u8"
    DASH,    ///< Dynamic adaptive streaming over Http protocol, uri prefix: "https://" or "https://", suffix: ".mpd"
    RTSP,    ///< Real time streaming protocol, uri prefix: "rtsp://"
    RTP,     ///< Real-time transport protocol, uri prefix: "rtp://"
    RTMP,    ///< RTMP protocol, uri prefix: "rtmp://"
    FTP,     ///< FTP protocol, uri prefix: "ftp://"
    UDP,     ///< User datagram protocol, uri prefix: "udp://"
};

using ValueType = Any;

/**
 * The tag content is stored in key-value format.
 */
using CodecConfig = std::vector<uint8_t>;
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_COMMON_TAGS_H
