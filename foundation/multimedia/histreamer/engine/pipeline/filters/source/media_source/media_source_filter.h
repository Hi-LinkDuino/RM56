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

#ifndef MEDIA_PIPELINE_MEDIA_SOURCE_FILTER_H
#define MEDIA_PIPELINE_MEDIA_SOURCE_FILTER_H

#include <memory>
#include <string>

#include "pipeline/core/error_code.h"
#include "foundation/osal/thread/task.h"
#include "foundation/osal/utils/util.h"
#include "pipeline/core/filter_base.h"
#include "pipeline/core/type_define.h"
#include "plugin/common/media_source.h"
#include "plugin/core/plugin_manager.h"
#include "plugin/interface/source_plugin.h"
#include "utils/blocking_queue.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
using SourceType = OHOS::Media::Plugin::SourceType;
using MediaSource = OHOS::Media::Plugin::MediaSource;

class MediaSourceFilter : public FilterBase {
public:
    explicit MediaSourceFilter(const std::string& name);
    ~MediaSourceFilter() override;

    std::vector<WorkMode> GetWorkModes() override;
    ErrorCode PullData(const std::string& outPort, uint64_t offset, size_t size, AVBufferPtr& data) override;
    virtual ErrorCode SetSource(const std::shared_ptr<MediaSource>& source);
    virtual ErrorCode SetBufferSize(size_t size);
    ErrorCode Prepare() override;
    ErrorCode Start() override;
    ErrorCode Stop() override;
    void FlushStart() override;
    void FlushEnd() override;
    Plugin::Seekable GetSeekable() const;

private:
    void InitPorts() override;
    void ActivateMode();
    ErrorCode InitPlugin(const std::shared_ptr<MediaSource>& source);
    static std::string GetUriSuffix(const std::string& uri);
    ErrorCode DoNegotiate(const std::shared_ptr<MediaSource>& source);
    void ReadLoop();
    bool GetProtocolByUri();
    bool ParseProtocol(const std::shared_ptr<MediaSource>& source);
    ErrorCode CreatePlugin(const std::shared_ptr<Plugin::PluginInfo>& info, const std::string& name,
                           Plugin::PluginManager& manager);
    ErrorCode FindPlugin(const std::shared_ptr<MediaSource>& source);
    void OnEvent(const Plugin::PluginEvent &event) override;
    void ClearData();

    std::shared_ptr<OSAL::Task> taskPtr_;
    std::string protocol_;
    std::string uri_;
    Plugin::Seekable seekable_;
    uint64_t position_;
    int64_t mediaOffset_ {0}; // offset used in push mode
    size_t bufferSize_;
    std::shared_ptr<Plugin::Source> plugin_;
    std::shared_ptr<Allocator> pluginAllocator_;
    bool isPluginReady_ {false};
    bool isAboveWaterline_ {false};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif
