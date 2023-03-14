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

#ifndef HISTREAMER_PLUGIN_CORE_DEMUXER_H
#define HISTREAMER_PLUGIN_CORE_DEMUXER_H

#include <memory>

#include "base.h"
#include "common/plugin_buffer.h"
#include "core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Plugin {
using AllocatorHelper = Allocator;

struct MediaInfoHelper {
    Meta globalMeta;
    std::vector<Meta> trackMeta;
};

struct DataSourceHelper {
    virtual ~DataSourceHelper() = default;
    virtual Status ReadAt(int64_t offset, std::shared_ptr<Buffer> &buffer, size_t expectedLen) = 0;
    virtual Status GetSize(size_t &size) = 0;
    virtual Seekable GetSeekable() = 0;
};

struct DemuxerPlugin;

class Demuxer : public Base {
public:
    Demuxer(const Demuxer &) = delete;
    Demuxer operator=(const Demuxer &) = delete;
    ~Demuxer() override = default;

    Status SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode);

    Status SetDataSource(const std::shared_ptr<DataSourceHelper> &source);

    Status GetMediaInfo(MediaInfoHelper &mediaInfo);

    Status ReadFrame(Buffer &info, int32_t timeOutMs);

private:
    friend class PluginManager;

    Demuxer(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<DemuxerPlugin> plugin);

private:
    std::shared_ptr<DemuxerPlugin> demuxer_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_DEMUXER_H
