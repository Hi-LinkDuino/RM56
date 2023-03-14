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
 
#ifndef MINIMP4_DEMUXER_PLUGIN_H
#define MINIMP4_DEMUXER_PLUGIN_H

#include <memory>
#include <string>
#include <vector>
#include <set>

#include "interface/demuxer_plugin.h"
#include "minimp4.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp4 {
class MiniMP4DemuxerPlugin : public DemuxerPlugin {
public:
    explicit MiniMP4DemuxerPlugin(std::string name);
    ~MiniMP4DemuxerPlugin() override;
    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status Stop() override;
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    Status SetDataSource(const std::shared_ptr<DataSource>& source) override;
    Status GetMediaInfo(MediaInfo& mediaInfo) override;
    Status ReadFrame(Buffer& info, int32_t timeOutMs) override;
    Status SeekTo(int32_t trackId, int64_t timeStampUs, SeekMode mode) override;
    size_t GetTrackCount() override;
    Status SelectTrack(int32_t trackId) override;
    Status UnselectTrack(int32_t trackId) override;
    Status GetSelectedTracks(std::vector<int32_t>& trackIds) override;
    size_t GetFileSize();
    std::shared_ptr<DataSource> GetInputBuffer();
    Status AudioAdapterForDecoder();
    Status DoReadFromSource(uint32_t ioNeedReadSize);
    Status GetDataFromSource();
private:
    void FillADTSHead(std::shared_ptr<Memory> &data, unsigned int frameSize);
    static int ReadCallback(int64_t offset, void *buffer, size_t size, void *token);
    struct IOContext {
        std::shared_ptr<DataSource> dataSource {nullptr};
        int64_t offset {0};
        bool eos {false};
    };
    IOContext ioContext_;
    std::shared_ptr<Callback> callback_ {nullptr};
    MP4D_demux_t miniMP4_;
    size_t fileSize_;
    unsigned int sampleIndex_;
    std::unique_ptr<MediaInfo> mediaInfo_;
    unsigned char *inIoBuffer_;
    unsigned int  ioDataRemainSize_;
    int           inIoBufferSize_;
};
} // namespace Minimp4
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif

