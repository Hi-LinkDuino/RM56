/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_UTILS_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_UTILS_H
#include <chrono>
#include <ctime>
#include <ratio>
#include <string>
#include <errno.h>
#include <stddef.h>
#include <functional>

#include "event_handler.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {

using CALLBACK = std::function<void(int)>;

#if !defined(DISALLOW_ASSIGN)
// Put this in the declarations for a class to be unassignable.
#define DISALLOW_ASSIGN(TypeName) void operator=(const TypeName &) = delete
#endif

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// We define this macro conditionally as it may be defined by another libraries.
#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &);            \
    void operator = (const TypeName &)
#endif

using PlatformFile = int;
using char16 = wchar_t;
using string16 = std::wstring;

// zip Error
enum ErrorCode {
    ERROR_CODE_OK = 0,
    ERROR_CODE_STREAM_END = 1,
    ERROR_CODE_NEED_DICT = 2,
    ERROR_CODE_ERRNO = -1,
    ERROR_CODE_STREAM_ERROR = -2,
    ERROR_CODE_DATA_ERROR = -3,
    ERROR_CODE_MEM_ERROR = -4,
    ERROR_CODE_BUF_ERROR = -5,
    ERROR_CODE_VERSION_ERROR = -6
};

// Constant definitions related to zlib Library
enum FlushType {
    FLUSH_TYPE_NO_FLUSH = 0,
    FLUSH_TYPE_PARTIAL_FLUSH = 1,
    FLUSH_TYPE_SYNC_FLUSH = 2,
    FLUSH_TYPE_FULL_FLUSH = 3,
    FLUSH_TYPE_FINISH = 4,
    FLUSH_TYPE_BLOCK = 5,
    FLUSH_TYPE_TREES = 6
};
using FLUSH_TYPE = enum FlushType;

// Compress Level
enum CompressLevel {
    COMPRESS_LEVEL_NO_COMPRESSION = 0,
    COMPRESS_LEVEL_BEST_SPEED = 1,
    COMPRESS_LEVEL_BEST_COMPRESSION = 9,
    COMPRESS_LEVEL_DEFAULT_COMPRESSION = -1
};
using COMPRESS_LEVEL = enum CompressLevel;

// Compress Strategy
enum CompressStrategy {
    COMPRESS_STRATEGY_DEFAULT_STRATEGY = 0,
    COMPRESS_STRATEGY_FILTERED = 1,
    COMPRESS_STRATEGY_HUFFMAN_ONLY = 2,
    COMPRESS_STRATEGY_RLE = 3,
    COMPRESS_STRATEGY_FIXED = 4
};
using COMPRESS_STRATEGY = enum CompressStrategy;

// Memory Level
enum MemoryLevel { MEM_LEVEL_MIN_MEMLEVEL = 1, MEM_LEVEL_DEFAULT_MEMLEVEL = 8, MEM_LEVEL_MAX_MEMLEVEL = 9 };
using MEMORY_LEVEL = enum MemoryLevel;

// Compression Options
struct Options {
    FLUSH_TYPE flush;
    FLUSH_TYPE finishFlush;
    int chunkSize;          // Buffer size, > 64
    COMPRESS_LEVEL level;   // Compression level. The compression level is a number from 0 to 9. 0 has the fastest
                            // compression speed (compression process), 9 has the slowest compression speed, the largest
                            // compression rate, and 0 does not compress
    MEMORY_LEVEL memLevel;  // Internal compression status, how much memory should be allocated
    COMPRESS_STRATEGY strategy;  // CompressStrategy

    // default constructor
    Options()
    {
        CompressDefaultValue();
    }

    // compression default options
    void CompressDefaultValue()
    {
        flush = FLUSH_TYPE_NO_FLUSH;
        finishFlush = FLUSH_TYPE_SYNC_FLUSH;
        chunkSize = 128;
        level = COMPRESS_LEVEL_DEFAULT_COMPRESSION;
        memLevel = MEM_LEVEL_DEFAULT_MEMLEVEL;
        strategy = COMPRESS_STRATEGY_DEFAULT_STRATEGY;
    }
};
using OPTIONS = struct Options;

constexpr PlatformFile kInvalidPlatformFile = -1;

struct tm *GetCurrentSystemTime(void);
bool StartsWith(const std::string &str, const std::string &searchFor);
bool EndsWith(const std::string &str, const std::string &searchFor);
void PostTask(const OHOS::AppExecFwk::InnerEvent::Callback &callback);
bool FilePathCheckValid(const std::string &str);
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_UTILS_H