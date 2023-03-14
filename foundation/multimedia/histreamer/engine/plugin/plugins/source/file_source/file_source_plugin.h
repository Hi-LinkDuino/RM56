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

#ifndef MEDIA_PIPELINE_FILE_SOURCE_PLUGIN_H
#define MEDIA_PIPELINE_FILE_SOURCE_PLUGIN_H

#include <cstdio>
#include "plugin/common/plugin_types.h"
#include "plugin/interface/source_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FileSource {
class FileSourceAllocator : public Allocator {
public:
    FileSourceAllocator() = default;
    ~FileSourceAllocator() override = default;

    void* Alloc(size_t size) override;
    void Free(void* ptr) override; // NOLINT: void*
};

class FileSourcePlugin : public SourcePlugin {
public:
    explicit FileSourcePlugin(std::string name);
    ~FileSourcePlugin() override;

    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    Status SetSource(std::shared_ptr<MediaSource> source) override;
    Status Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen) override;
    Status GetSize(size_t& size) override;
    Seekable GetSeekable() override;
    Status SeekTo(uint64_t offset) override;

private:
    std::string fileName_ {};
    std::FILE* fp_;
    size_t fileSize_;
    Seekable seekable_;
    uint64_t position_;
    std::shared_ptr<FileSourceAllocator> mAllocator_ {nullptr};

    Status ParseFileName(const std::string& uri);
    Status CheckFileStat();
    Status OpenFile();
    void CloseFile();
};
} // namespace FileSource
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // MEDIA_PIPELINE_FILE_SOURCE_PLUGIN_H
