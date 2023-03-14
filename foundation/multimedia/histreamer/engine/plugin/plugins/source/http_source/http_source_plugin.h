/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifndef HISTREAMER_HTTP_SOURCE_PLUGIN_H
#define HISTREAMER_HTTP_SOURCE_PLUGIN_H

#include <memory>
#include "media_downloader.h"
#include "plugin/common/plugin_types.h"
#include "plugin/interface/source_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class HttpCurlClient;
class HttpSourcePlugin : public SourcePlugin {
public:
    explicit HttpSourcePlugin(std::string name) noexcept;
    ~HttpSourcePlugin() override;
    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status Start() override;
    Status Stop() override;
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    Status SetSource(std::shared_ptr<MediaSource> source) override;
    Status Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen) override;
    Status GetSize(size_t &size) override;
    Seekable GetSeekable() override;
    Status SeekTo(uint64_t offset) override;

private:
    void CloseUri();

    std::shared_ptr<HttpCurlClient> client_;
    uint32_t bufferSize_;
    uint32_t waterline_;
    Callback* callback_ {};
    std::shared_ptr<MediaDownloader> downloader_;
    OSAL::Mutex mutex_ {};
    bool delayReady {true};
};
} // namespace HttpPluginLite
} // namespace Plugin 
} // namespace Media 
} // namespace OHOS
#endif