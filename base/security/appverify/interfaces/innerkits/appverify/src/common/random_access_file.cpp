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

#include "common/random_access_file.h"

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "securec.h"

#include "common/hap_verify_log.h"

namespace OHOS {
namespace Security {
namespace Verify {
const int RandomAccessFile::FILE_OPEN_FAIL_ERROR_NUM = -1;
int RandomAccessFile::memoryPageSize = sysconf(_SC_PAGESIZE);

RandomAccessFile::RandomAccessFile()
    : fd(FILE_OPEN_FAIL_ERROR_NUM), fileLength(0)
{
}

RandomAccessFile::~RandomAccessFile()
{
    if (fd != FILE_OPEN_FAIL_ERROR_NUM) {
        close(fd);
    }
}

bool RandomAccessFile::Init(const std::string& filePath)
{
    fd = open(filePath.c_str(), O_RDONLY);
    if (fd == FILE_OPEN_FAIL_ERROR_NUM) {
        return false;
    }

    if (memoryPageSize <= 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "getting pagesize failed: %{public}d", memoryPageSize);
        return false;
    }

    fileLength = lseek(fd, 0, SEEK_END);
    if (fileLength < 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "getting fileLength failed: %{public}lld", fileLength);
        return false;
    }
    return true;
}

long long RandomAccessFile::GetLength() const
{
    return fileLength;
}

long long RandomAccessFile::DoMMap(int bufCapacity, long long offset, MmapInfo& mmapInfo)
{
    mmapInfo.mapAddr = reinterpret_cast<char*>(MAP_FAILED);
    if (fd == FILE_OPEN_FAIL_ERROR_NUM) {
        return FILE_IS_CLOSE;
    }
    if (offset < 0 || offset > fileLength - bufCapacity) {
        return READ_OFFSET_OUT_OF_RANGE;
    }
    mmapInfo.mmapPosition = (offset / memoryPageSize) * memoryPageSize;
    mmapInfo.readMoreLen = static_cast<int>(offset - mmapInfo.mmapPosition);
    mmapInfo.mmapSize = bufCapacity + mmapInfo.readMoreLen;
    mmapInfo.mapAddr = reinterpret_cast<char*>(mmap(nullptr, mmapInfo.mmapSize, PROT_READ,
        MAP_SHARED | MAP_POPULATE, fd, mmapInfo.mmapPosition));
    if (mmapInfo.mapAddr == MAP_FAILED) {
        HAPVERIFY_LOG_ERROR(LABEL, "MAP_FAILED: %{public}d", errno);
        return MMAP_FAILED;
    }
    return 0;
}

long long RandomAccessFile::ReadFileFullyFromOffset(char buf[], long long offset, int bufCapacity)
{
    if (buf == nullptr) {
        return DEST_BUFFER_IS_NULL;
    }

    MmapInfo mmapInfo;
    long long ret = DoMMap(bufCapacity, offset, mmapInfo);
    if (ret < 0) {
        return ret;
    }

    if (memcpy_s(buf, bufCapacity, mmapInfo.mapAddr + mmapInfo.readMoreLen,
        mmapInfo.mmapSize - mmapInfo.readMoreLen) != EOK) {
        munmap(mmapInfo.mapAddr, mmapInfo.mmapSize);
        return MMAP_COPY_FAILED;
    }
    munmap(mmapInfo.mapAddr, mmapInfo.mmapSize);
    return bufCapacity;
}

long long RandomAccessFile::ReadFileFullyFromOffset(HapByteBuffer& buffer, long long offset)
{
    if (!buffer.HasRemaining()) {
        return DEST_BUFFER_IS_NULL;
    }

    MmapInfo mmapInfo;
    int bufCapacity = buffer.GetCapacity();
    long long ret = DoMMap(bufCapacity, offset, mmapInfo);
    if (ret < 0) {
        return ret;
    }

    buffer.PutData(0, mmapInfo.mapAddr + mmapInfo.readMoreLen, bufCapacity);
    munmap(mmapInfo.mapAddr, mmapInfo.mmapSize);
    return bufCapacity;
}

bool RandomAccessFile::ReadFileFromOffsetAndDigestUpdate(const DigestParameter& digestParam,
    int chunkSize, long long offset)
{
    MmapInfo mmapInfo;
    long long ret = DoMMap(chunkSize, offset, mmapInfo);
    if (ret < 0) {
        HAPVERIFY_LOG_ERROR(LABEL, "DoMMap failed: %{public}lld", ret);
        return false;
    }

    unsigned char* content = reinterpret_cast<unsigned char*>(mmapInfo.mapAddr + mmapInfo.readMoreLen);
    bool res = HapVerifyOpensslUtils::DigestUpdate(digestParam, content, chunkSize);
    munmap(mmapInfo.mapAddr, mmapInfo.mmapSize);
    return res;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
