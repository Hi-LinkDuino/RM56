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
#ifndef HAP_BYTEBUFFER_H
#define HAP_BYTEBUFFER_H

#include <memory>
#include <string>

#include "common/export_define.h"

namespace OHOS {
namespace Security {
namespace Verify {
enum ReadFileErrorCode {
    DEST_BUFFER_IS_NULL = -1,
    FILE_IS_CLOSE = -2,
    MMAP_COPY_FAILED = -3,
    READ_OFFSET_OUT_OF_RANGE = -4,
    MMAP_FAILED = -5,
    MMAP_PARAM_INVALID = -6,
};

class HapByteBuffer {
public:
    DLL_EXPORT HapByteBuffer();
    DLL_EXPORT explicit HapByteBuffer(int bufferCapacity);
    DLL_EXPORT HapByteBuffer(const HapByteBuffer& other);
    DLL_EXPORT ~HapByteBuffer();
    DLL_EXPORT HapByteBuffer& operator=(const HapByteBuffer& other);
    DLL_EXPORT bool GetInt64(long long& value);
    DLL_EXPORT bool GetInt64(int index, long long& value);
    DLL_EXPORT bool GetUInt32(int index, unsigned int& value);
    DLL_EXPORT bool GetInt32(int& value);
    DLL_EXPORT bool GetInt32(int index, int& value);
    DLL_EXPORT bool GetUInt16(int index, unsigned short& value);
    DLL_EXPORT void PutInt32(int offset, int value);
    DLL_EXPORT void PutByte(int offset, char value);
    DLL_EXPORT void PutData(int offset, const char data[], int len);
    DLL_EXPORT int GetCapacity() const;
    DLL_EXPORT int GetPosition() const;
    DLL_EXPORT int GetLimit() const;
    DLL_EXPORT const char* GetBufferPtr() const;
    DLL_EXPORT void SetPosition(int pos);
    DLL_EXPORT void SetLimit(int lim);
    DLL_EXPORT void SetCapacity(int cap);
    DLL_EXPORT void Slice();
    DLL_EXPORT int Remaining() const;
    DLL_EXPORT bool HasRemaining() const;
    DLL_EXPORT bool CopyPartialBuffer(const HapByteBuffer& other, int len);
    DLL_EXPORT void Clear();
    DLL_EXPORT bool IsEqual(const HapByteBuffer& other);
    DLL_EXPORT bool IsEqual(const std::string& other);

private:
    void Init(int bufferCapacity);
    bool CheckInputForGettingData(int index, int dataLen);

private:
    static const int MAX_PRINT_LENGTH;
    static const int HEX_PRINT_LENGTH;
    std::unique_ptr<char[]> buffer;
    int position;
    int limit;
    int capacity;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_BYTEBUFFER_H
