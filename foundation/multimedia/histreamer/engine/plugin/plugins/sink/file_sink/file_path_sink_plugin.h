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
#ifndef HISTREAMER_FILE_PATH_SINK_PLUGIN_H
#define HISTREAMER_FILE_PATH_SINK_PLUGIN_H

#include <cstdio>
#include "plugin/common/media_sink.h"
#include "plugin/interface/output_sink_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FileSink {
class FilePathSinkPlugin : public OutputSinkPlugin {
public:
    explicit FilePathSinkPlugin(std::string name);
    ~FilePathSinkPlugin() override;

    Status Stop() override;

    // file path sink
    Status SetSink(const MediaSink& sink) override;
    Seekable GetSeekable()  override;
    Status SeekTo(uint64_t offset)  override;
    Status Write(const std::shared_ptr<Buffer>& buffer) override;
    Status Flush() override;
    Status Reset() override;
private:
    Status OpenFile();
    void CloseFile();

    std::string fileName_ {};
    std::string tmpFileName_ {};
    std::FILE* fp_;
    Seekable seekable_;
};
}
}
}
}
#endif // HISTREAMER_FILE_PATH_SINK_PLUGIN_H