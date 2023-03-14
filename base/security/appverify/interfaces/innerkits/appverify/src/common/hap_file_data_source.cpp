/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "common/hap_file_data_source.h"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
HapFileDataSource::HapFileDataSource(RandomAccessFile& hapFile,
    long long offset, long long size, long long position)
    : DataSource(), hapFileRandomAccess(hapFile), fileOffset(offset), sourceSize(size), sourcePosition(position)
{
}

HapFileDataSource::~HapFileDataSource()
{
}

bool HapFileDataSource::HasRemaining() const
{
    return sourcePosition < sourceSize;
}

long long HapFileDataSource::Remaining() const
{
    return sourceSize - sourcePosition;
}

void HapFileDataSource::Reset()
{
    sourcePosition = 0;
}

bool HapFileDataSource::ReadDataAndDigestUpdate(const DigestParameter& digestParam, int chunkSize)
{
    if (!hapFileRandomAccess.ReadFileFromOffsetAndDigestUpdate(digestParam, chunkSize, fileOffset + sourcePosition)) {
        HAPVERIFY_LOG_ERROR(LABEL, "ReadFileFromOffsetAndDigestUpdate failed");
        return false;
    }
    sourcePosition += chunkSize;
    return true;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
