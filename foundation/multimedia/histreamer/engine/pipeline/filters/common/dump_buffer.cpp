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
#define HST_LOG_TAG "DumpBuffer"

#include "dump_buffer.h"
#include <cstdio>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <dirent.h>
#include "foundation/log.h"
#include "foundation/osal/filesystem/file_system.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
// Specify the dump file dir in BUILD.gn
#ifndef DUMP_FILE_DIR
#define DUMP_FILE_DIR "/data/local/tmp"
#endif

std::string GetDumpFileDir()
{
    auto rootDir = std::string(DUMP_FILE_DIR);
    return rootDir.empty() ? "histreamer_dump_files/" : rootDir + "/histreamer_dump_files/";
}

void DumpBufferToFile(const std::string& fileName, const std::shared_ptr<Plugin::Buffer>& buffer)
{
    size_t bufferSize = buffer->GetMemory()->GetSize();
    FALSE_RETURN(bufferSize != 0);

    std::string filePath = GetDumpFileDir() + fileName;
    auto filePtr = fopen(filePath.c_str(), "ab+");
    FALSE_RETURN_MSG(filePtr != nullptr, "Open file(" PUBLIC_LOG_S ") failed(" PUBLIC_LOG_S ").", filePath.c_str(),
                     strerror(errno));
    (void)fwrite(reinterpret_cast<const char*>(buffer->GetMemory()->GetReadOnlyData()),
                 1, bufferSize, filePtr);
    (void)fclose(filePtr);
}

void PrepareDumpDir()
{
    std::string dumpDir = GetDumpFileDir();
    const char* fileDir = dumpDir.c_str();
    if (access(fileDir, 0) == 0) { // 目录存在
        OSAL::FileSystem::RemoveFilesInDir(fileDir);
    } else {
        (void)OSAL::FileSystem::MakeMultipleDir(fileDir);
    }
}

void DumpBufferToLog(const char* desc, const std::shared_ptr<Plugin::Buffer>& buffer, uint64_t offset, size_t dumpSize)
{
    FALSE_RETURN_MSG(buffer && (!buffer->IsEmpty()),  PUBLIC_LOG_S " Buffer(null or empty)", desc);
    size_t bufferSize = buffer->GetMemory()->GetSize();
    size_t realDumpSize = std::min(dumpSize, bufferSize);
    realDumpSize = std::min(realDumpSize, static_cast<size_t>(DUMP_BUFFER2LOG_SIZE)); // max DUMP_BUFFER2LOG_SIZE bytes
    char tmpStr[2 * DUMP_BUFFER2LOG_SIZE + 10] = {0}; // 字符串长度是打印的buffer长度的2倍 + 1 (字符串结束符)
    char* dstPtr = tmpStr;
    int len;
    const uint8_t* p = buffer->GetMemory()->GetReadOnlyData();
    for (size_t i = 0; i < realDumpSize; i++) {
        len = snprintf_s(dstPtr, 3, 2, "%02x", p[i]); // max write 3 bytes, string len 2
        FALSE_RETURN_MSG(len > 0 && len <= 2, "snprintf_s returned unexpected value " PUBLIC_LOG_D32, len); // max len 2
        dstPtr += len;
    }
    MEDIA_LOG_I(PUBLIC_LOG_S " Buffer(offset " PUBLIC_LOG_D64 ", size " PUBLIC_LOG_ZU ", capacity "
        PUBLIC_LOG_ZU ") : " PUBLIC_LOG_S, desc, offset, bufferSize, buffer->GetMemory()->GetCapacity(), tmpStr);
}
} // Pipeline
} // Media
} // OHOS