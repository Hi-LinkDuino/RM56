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

#ifndef HISTREAMER_TYPE_FINDER_H
#define HISTREAMER_TYPE_FINDER_H

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include "osal/thread/task.h"
#include "pipeline/core/type_define.h"
#include "plugin/interface/plugin_base.h"
#include "plugin/core/plugin_manager.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class TypeFinder : public std::enable_shared_from_this<TypeFinder>, public Plugin::DataSourceHelper {
public:
    TypeFinder();

    ~TypeFinder() override;

    void Init(std::string uriSuffix, size_t mediaDataSize, std::function<bool(uint64_t, size_t)> checkRange,
              std::function<bool(uint64_t, size_t, AVBufferPtr&)> peekRange);

    std::string FindMediaType();

    void FindMediaTypeAsync(std::function<void(std::string)> typeFound);

    Plugin::Status ReadAt(int64_t offset, std::shared_ptr<Plugin::Buffer>& buffer, size_t expectedLen) override;

    Plugin::Status GetSize(size_t& size) override;

    Plugin::Seekable GetSeekable() override;

private:
    void DoTask();

    std::string SniffMediaType();

    std::string GuessMediaType() const;

    bool IsOffsetValid(int64_t offset) const;

    bool IsSniffNeeded(std::string suffix);

    bool GetPlugins();

    void SortPlugins(const std::string& uriSuffix);

    bool sniffNeeded_;
    std::string uriSuffix_;
    size_t mediaDataSize_;
    std::string pluginName_;
    std::vector<std::shared_ptr<Plugin::PluginInfo>> plugins_;
    std::atomic<bool> pluginRegistryChanged_;
    std::shared_ptr<OSAL::Task> task_;
    std::function<bool(uint64_t, size_t)> checkRange_;
    std::function<bool(uint64_t, size_t, AVBufferPtr&)> peekRange_;
    std::function<void(std::string)> typeFound_;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_TYPE_FINDER_H
