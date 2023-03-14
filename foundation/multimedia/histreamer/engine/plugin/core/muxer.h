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

#ifndef HISTREAMER_PLUGIN_CORE_MUXER_H
#define HISTREAMER_PLUGIN_CORE_MUXER_H

#include "base.h"
#include "common/plugin_buffer.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct MuxerPlugin;
struct DataSinkHelper {
    virtual ~DataSinkHelper() = default;
    virtual Status WriteAt(int64_t offset, const std::shared_ptr<Buffer>& buffer) = 0;
};
class Muxer : public Base {
public:
    Muxer(const Muxer &) = delete;
    Muxer operator=(const Muxer &) = delete;
    ~Muxer() override = default;
    Status SetTrackParameter(uint32_t trackId, Plugin::Tag tag, const Plugin::ValueType& value);
    Status GetTrackParameter(uint32_t trackId, Plugin::Tag tag, Plugin::ValueType& value);
    Status AddTrack(uint32_t& trackId);
    Status SetDataSink(const std::shared_ptr<DataSinkHelper>& dataSink);
    Status WriteHeader();
    Status WriteFrame(const std::shared_ptr<Buffer>& buffer);
    Status WriteTrailer();
private:
    friend class PluginManager;

    Muxer(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<MuxerPlugin> plugin);

private:
    std::shared_ptr<MuxerPlugin> muxer_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_MUXER_H
