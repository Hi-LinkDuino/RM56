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

#ifndef HISTREAMER_UTDEMUXERTEST2_H
#define HISTREAMER_UTDEMUXERTEST2_H


#include "plugin/interface/demuxer_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
class UtDemuxerTest2 : public DemuxerPlugin {
public:
    explicit UtDemuxerTest2(std::string name) : DemuxerPlugin(std::move(name))
    {
    }

    ~UtDemuxerTest2() override = default;

    Status SetDataSource(const std::shared_ptr<DataSource> &source) override;

    Status GetMediaInfo(MediaInfo &mediaInfo) override;

    size_t GetTrackCount() override;

    Status SelectTrack(int32_t trackId) override;

    Status UnselectTrack(int32_t trackId) override;

    Status GetSelectedTracks(std::vector<int32_t> &trackIds) override;

    Status ReadFrame(Buffer &buffer, int32_t timeOutMs) override;

    Status SeekTo(int32_t trackId, int64_t timeStampUs, SeekMode mode) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_UTDEMUXERTEST2_H
