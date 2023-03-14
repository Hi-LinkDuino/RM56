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

#ifndef HISTREAMER_PLUGIN_TYPES_H
#define HISTREAMER_PLUGIN_TYPES_H

#include <cstdint> // NOLINT: using int32_t in this file

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @enum Plugin running state.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct State : int32_t {
    CREATED = 0,     ///< Indicates the status of the plugin when it is constructed.
                     ///< The plug-in will not be restored in the entire life cycle.
    INITIALIZED = 1, ///< Plugin global resource initialization completion status.
    PREPARED = 2,    ///< Status of parameters required for plugin running.
    RUNNING = 3,     ///< The system enters the running state after call start().
    PAUSED = 4,      ///< Plugin temporarily stops processing data. This state is optional.
    DESTROYED = -1,  ///< Plugin destruction state. In this state, all resources are released.
    INVALID = -2,    ///< An error occurs in any state and the plugin enters the invalid state.
};

/**
 * @enum Enumerates types of Seek Mode.
 *
 * @brief Seek modes, Options that SeekTo() behaviour.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct SeekMode : uint32_t {
    SEEK_NEXT_SYNC = 0,     ///> sync to keyframes after the time point.
    SEEK_PREVIOUS_SYNC,     ///> sync to keyframes before the time point.
    SEEK_CLOSEST_SYNC,      ///> sync to closest keyframes.
    SEEK_CLOSEST,           ///> seek to frames closest the time point.
};

/**
 * @enum Seekable Status.
 *
 * @since 1.0
 * @version 1.0
 */
enum class Seekable : int32_t {
    INVALID = -1,
    UNSEEKABLE = 0,
    SEEKABLE = 1
};

/**
 * @enum Api Return Status.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct Status : int32_t {
    END_OF_STREAM = 1,         ///< Read source when end of stream
    OK = 0,                    ///< The execution result is correct.
    NO_ERROR = OK,             ///< Same as Status::OK
    ERROR_UNKNOWN = -1,        ///< An unknown error occurred.
    ERROR_PLUGIN_ALREADY_EXISTS = -2, ///< The plugin already exists, usually occurs when in plugin registered.
    ERROR_INCOMPATIBLE_VERSION =
        -3,                         ///< Incompatible version, may occur during plugin registration or function calling.
    ERROR_NO_MEMORY = -4,           ///< The system memory is insufficient.
    ERROR_WRONG_STATE = -5,         ///< The function is called in an invalid state.
    ERROR_UNIMPLEMENTED = -6,       ///< This method or interface is not implemented.
    ERROR_INVALID_PARAMETER = -7,   ///< The plugin does not support this parameter.
    ERROR_INVALID_DATA = -8,        ///< The value is not in the valid range.
    ERROR_MISMATCHED_TYPE = -9,     ///< Mismatched data type
    ERROR_TIMED_OUT = -10,          ///< Operation timeout.
    ERROR_UNSUPPORTED_FORMAT = -11, ///< The plugin not support this format/name.
    ERROR_NOT_ENOUGH_DATA = -12,    ///< Not enough data when read from source.
    ERROR_NOT_EXISTED = -13,        ///< Source is not existed.
    ERROR_AGAIN = -14,              ///< Operation is not available right now, should try again later.
    ERROR_PERMISSION_DENIED = -15,  ///< Permission denied.
    ERROR_NULL_POINTER = -16,       ///< Null pointer.
    ERROR_INVALID_OPERATION = -17,  ///< Invalid operation.
    ERROR_CLIENT = -18,             ///< Http client error
    ERROR_SERVER = -19,             ///< Http server error
    ERROR_DELAY_READY = -20,        ///< Delay ready event
};

/**
 * @enum Plugin Type.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct PluginType : int32_t {
    INVALID_TYPE = -1, ///< Invalid plugin
    SOURCE = 1,        ///< reference SourcePlugin
    DEMUXER,           ///< reference DemuxerPlugin
    CODEC,             ///< reference CodecPlugin
    AUDIO_SINK,        ///< reference AudioSinkPlugin
    VIDEO_SINK,        ///< reference VideoSinkPlugin
    MUXER,             ///< reference MuxerPlugin
    OUTPUT_SINK,       ///< reference OutputSinkPlugin
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_TYPES_H
