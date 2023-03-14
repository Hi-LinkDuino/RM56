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

#ifndef HISTREAMER_PIPELINE_DUMP_BUFFER_H
#define HISTREAMER_PIPELINE_DUMP_BUFFER_H

#include "plugin/common/plugin_buffer.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
// Modify DUMP_BUFFER2FILE_ENABLE to 1 to dump buffer to file.
#define DUMP_BUFFER2FILE_ENABLE 0

// Modify DUMP_BUFFER2LOG_ENABLE to 1 to dump buffer first 10 bytes to log.
#define DUMP_BUFFER2LOG_ENABLE 0

#if DUMP_BUFFER2FILE_ENABLE
#define DUMP_BUFFER2FILE(fileName, buffer) OHOS::Media::Pipeline::DumpBufferToFile(fileName, buffer)
#define DUMP_BUFFER2FILE_PREPARE() OHOS::Media::Pipeline::PrepareDumpDir()
#else
#define DUMP_BUFFER2FILE(fileName, buffer)
#define DUMP_BUFFER2FILE_PREPARE()
#endif

#define DUMP_BUFFER2LOG_SIZE 10 // Dump first 10 bytes of buffer.
#if DUMP_BUFFER2LOG_ENABLE
#define DUMP_BUFFER2LOG(desc, buffer, offset) \
    OHOS::Media::Pipeline::DumpBufferToLog(desc, buffer, offset, DUMP_BUFFER2LOG_SIZE)
#else
#define DUMP_BUFFER2LOG(desc, buffer, offset)
#endif

void DumpBufferToFile(const std::string& fileName, const std::shared_ptr<Plugin::Buffer>& buffer);
void PrepareDumpDir();
void DumpBufferToLog(const char* desc, const std::shared_ptr<Plugin::Buffer>& buffer, uint64_t offset, size_t size);
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_DUMP_BUFFER_H
