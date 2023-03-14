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

#ifndef HISTREAMER_PLUGIN_INTF_DEMUXER_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_DEMUXER_PLUGIN_H

#include <memory>
#include <vector>
#include "common/plugin_caps.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief MediaInfo is a facilitate unified display of the relevant technical and
 * tag data for video and audio files.
 *
 * MediaInfo reveals information such as:
 *   - General: artist, album, author, copyright, date, duration, etc.
 *   - Tracks: such as codec, channel, bitrate, etc.
 * @see Tag
 *
 * @since 1.0
 * @version 1.0
 */
struct MediaInfo {
    TagMap general;             ///< General information
    std::vector<TagMap> tracks; ///< Media tracks, include audio, video and text
};

/**
 * @brief Data source operation interface.
 *
 * @since 1.0
 * @version 1.0
 */
struct DataSource {
    /// Destructor
    virtual ~DataSource() = default;

    /**
     * @brief Read data from data source.
     *
     * @param offset    Offset of read position
     * @param buffer    Storage of the read data
     * @param expectedLen   Expected data size to be read
     * @return  Execution status return
     *  @retval OK: Plugin ReadAt succeeded.
     *  @retval ERROR_NOT_ENOUGH_DATA: Data not enough
     *  @retval END_OF_STREAM: End of stream
     */
    virtual Status ReadAt(int64_t offset, std::shared_ptr<Buffer>& buffer, size_t expectedLen) = 0;

    /**
     * @brief Get data source size.
     *
     * @param size data source size.
     * @return  Execution status return.
     *  @retval OK: Plugin GetSize succeeded.
     */
    virtual Status GetSize(size_t& size) = 0;

    /**
     * @brief Indicates that the current data source seekable or not.
     *
     * The function is valid only after INITIALIZED state.
     *
     * @return  Seekable status
     */
    virtual Seekable GetSeekable() = 0;
};

/**
 * @brief Demuxer Plugin Interface.
 *
 * Used for audio and video media file parse.
 *
 * @since 1.0
 * @version 1.0
 */
struct DemuxerPlugin : public PluginBase {
    /// constructor
    explicit DemuxerPlugin(std::string name): PluginBase(std::move(name)) {}
    /**
     * @brief Set the data source to demuxer component.
     *
     * The function is valid only in the CREATED state.
     *
     * @param source Data source where data read from.
     * @return  Execution status return
     *  @retval OK: Plugin SetDataSource succeeded.
     */
    virtual Status SetDataSource(const std::shared_ptr<DataSource>& source) = 0;

    /**
     * @brief Get the attributes of a media file.
     *
     * The attributes contain file and stream attributes.
     * The function is valid only after INITIALIZED state.
     *
     * @param mediaInfo Indicates the pointer to the source attributes
     * @return  Execution status return
     *  @retval OK: Plugin GetMediaInfo succeeded.
     */
    virtual Status GetMediaInfo(MediaInfo& mediaInfo) = 0;

    /**
     * @brief Get the stack count from the data source.
     *
     * The function is valid only after INITIALIZED state.
     *
     * @return number of tracks.
     */
    virtual size_t GetTrackCount() = 0;

    /**
     * @brief Select a specified media track.
     *
     * The function is valid only after RUNNING state.
     *
     * @param trackId Identifies the media track. If an invalid value is passed, the default media track specified.
     * @return  Execution status return
     *  @retval OK: Plugin SelectTrack succeeded.
     */
    virtual Status SelectTrack(int32_t trackId) = 0;

    /**
     * @brief Unselect a specified media track from which the demuxer reads data frames.
     *
     * The function is valid only after RUNNING state.
     *
     * @param trackId Identifies the media track. ignore the invalid value is passed.
     * @return  Execution status return
     *  @retval OK: Plugin UnselectTrack succeeded.
     */
    virtual Status UnselectTrack(int32_t trackId) = 0;

    /**
     * @brief Get the ID of the media track selected by the demuxer for output.
     *
     * The function is valid only after RUNNING state.
     *
     * @param trackIds Identifies the array of selected media tracks.
     * @return  Execution status return
     *  @retval OK: Plugin GetSelectedTracks succeeded.
     */
    virtual Status GetSelectedTracks(std::vector<int32_t>& trackIds) = 0;

    /**
     * @brief Reads data frames.
     *
     * The function is valid only after RUNNING state.
     *
     * @param buffer Indicates the pointer to the data buffer.
     * @param timeOutMs Indicates the time required for waiting data frame read.
     * @return  Execution status return
     *  @retval OK: Plugin ReadFrame succeeded.
     *  @retval ERROR_TIMED_OUT: Operation timeout.
     */
    virtual Status ReadFrame(Buffer& buffer, int32_t timeOutMs) = 0;

    /**
     * @brief Seeks for a specified position for the demuxer.
     *
     * After being started, the demuxer seeks for a specified position to read data frames.
     *
     * The function is valid only after RUNNING state.
     *
     * @param trackId Identifies the stream in the media file.
     * @param hstTime Indicates the target position, based on {@link HST_TIME_BASE} .
     * @param mode Indicates the seek mode.
     * @return  Execution status return
     *  @retval OK: Plugin SeekTo succeeded.
     *  @retval ERROR_INVALID_DATA: The input data is invalid.
     */
    virtual Status SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode) = 0;
};

/// Demuxer plugin api major number.
#define DEMUXER_API_VERSION_MAJOR (1)

/// Demuxer plugin api minor number
#define DEMUXER_API_VERSION_MINOR (0)

/// Demuxer plugin version
#define DEMUXER_API_VERSION MAKE_VERSION(DEMUXER_API_VERSION_MAJOR, DEMUXER_API_VERSION_MINOR)

/// Demuxer sniff function
using DemuxerPluginSnifferFunc = int (*)(const std::string& name, std::shared_ptr<DataSource> dataSource);

/**
 * @brief Describes the demuxer plugin information.
 *
 * @since 1.0
 * @version 1.0
 */
struct DemuxerPluginDef : public PluginDefBase {
    std::vector<std::string> extensions;      ///< File extensions supported by demuxer
    CapabilitySet inCaps;                     ///< Plug-in input capability, For details, @see Capability.
    CapabilitySet outCaps;                    ///< Plug-in output capability, For details, @see Capability.
    PluginCreatorFunc<DemuxerPlugin> creator {nullptr}; ///< Demuxer plugin create function.
    DemuxerPluginSnifferFunc sniffer {nullptr};         ///< Demuxer plugin sniff function.
    DemuxerPluginDef()
    {
        apiVersion = DEMUXER_API_VERSION; ///< Demuxer plugin version.
        pluginType = PluginType::DEMUXER; ///< Plugin type, MUST be DEMUXER.
    }
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_DEMUXER_PLUGIN_H
