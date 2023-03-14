/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HILOG_COMPRESS_H
#define HILOG_COMPRESS_H

#include "hilog_common.h"
#include <iostream>
#ifdef USING_ZSTD_COMPRESS
#define ZSTD_STATIC_LINKING_ONLY
#include "include/common.h"
#include "zstd.h"
#endif
#include <zlib.h>

namespace OHOS {
namespace HiviewDFX {
using LogPersisterBuffer = struct {
    char content[MAX_PERSISTER_BUFFER_SIZE];
    uint32_t offset;
};

class LogCompress {
public:
    LogCompress() = default;
    virtual ~LogCompress() = default;
    virtual int Compress(const LogPersisterBuffer &inBuffer, LogPersisterBuffer &compressBuffer) = 0;
};

class NoneCompress : public LogCompress {
public:
    int Compress(const LogPersisterBuffer &inBuffer, LogPersisterBuffer &compressBuffer) override;
};

class ZlibCompress : public LogCompress {
public:
    int Compress(const LogPersisterBuffer &inBuffer, LogPersisterBuffer &compressBuffer) override;
private:
    static const uint16_t CHUNK = 16384;
    char buffIn[CHUNK] = {0};
    char buffOut[CHUNK] = {0};

    z_stream cStream;
};

class ZstdCompress : public LogCompress {
public:
    int Compress(const LogPersisterBuffer &inBuffer, LogPersisterBuffer &compressBuffer) override;
private:
#ifdef USING_ZSTD_COMPRESS
    static const uint16_t CHUNK = 16384;
    char buffIn[CHUNK] = {0};
    char buffOut[CHUNK] = {0};
    ZSTD_CCtx* cctx;
#endif
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* HILOG_COMPRESS_H */
