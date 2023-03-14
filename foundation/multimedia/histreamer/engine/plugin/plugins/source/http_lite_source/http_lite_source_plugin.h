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
#ifndef HISTREAMER_HTTP_SOURCE_PLUGIN_H
#define HISTREAMER_HTTP_SOURCE_PLUGIN_H

#include "http_lite_manager.h"
#include "plugin/common/plugin_types.h"
#include "plugin/interface/source_plugin.h"
#include "foundation/osal/thread/scoped_lock.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpLitePlugin {
class HttpSourceAllocator : public Allocator {
public:
    HttpSourceAllocator() = default;
    ~HttpSourceAllocator() override = default;

    void *Alloc(size_t size) override;
    void Free(void *ptr) override;
};

class HttpSourcePlugin : public SourcePlugin {
public:
    explicit HttpSourcePlugin(const std::string name) noexcept;
    ~HttpSourcePlugin();
    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status Stop() override;
    Status GetParameter(Tag tag, ValueType &value) override;
    Status SetParameter(Tag tag, const ValueType &value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    Status SetSource(std::shared_ptr<MediaSource> source) override;
    Status Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen) override;
    Status GetSize(size_t &size) override;
    Seekable GetSeekable() override;
    Status SeekTo(uint64_t offset) override;
    static void OnError(int httpError, int localError, void *param, int support_retry);
    Status OnHttpEvent(void *priv, int errorType, int32_t errorCode);

private:
    std::string url_;
    std::string certFile_;
    bool needExit_ {false};
    bool isStream_ {false};
    uint32_t bufferSize_ {0};
    unsigned int position_ {0};
    uint32_t waterline_ {0};
    size_t   fileSize_ {0};
    Callback* callback_ {};
    std::shared_ptr<HttpLiteManager> httpHandle_;
    std::shared_ptr<HttpSourceAllocator> mAllocator_;
    mutable OSAL::Mutex httpMutex_ {};
    Status OpenUri(std::string &url);
    void CloseUri();
};
} // namespace HttpLitePlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif