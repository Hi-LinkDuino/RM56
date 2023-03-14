/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_FILE_H
#define OHOS_HDI_FILE_H

#include <cstdio>

#include "util/string.h"

namespace OHOS {
namespace HDI {
class File {
public:
    File(const String &path, unsigned int mode);

    ~File();

    inline bool IsValid() const
    {
        return fd_ != nullptr;
    }

    inline String GetPath() const
    {
        return path_;
    }

    char GetChar();

    char PeekChar();

    bool IsEof() const;

    inline int GetCharLineNumber() const
    {
        return lineNo_;
    }

    inline int GetCharColumnNumber() const
    {
        return columnNo_;
    }

    bool ReadData(void *data, size_t size) const;

    bool WriteData(const void *data, size_t size) const;

    void Flush();

    bool Reset();

    bool Skip(long size);

    void Close();

    static bool CreateParentDir(const String &path);

    static String AdapterPath(const String &path);

    size_t GetHashKey();

    static constexpr unsigned int READ = 0x1;
    static constexpr unsigned int WRITE = 0x2;
    static constexpr unsigned int APPEND = 0x4;

#ifndef __MINGW32__
    static constexpr char pathSeparator = '/';
#else
    static constexpr char pathSeparator = '\\';
#endif

private:
    int Read();

    bool CheckValid(const String &path);

    static constexpr int BUFFER_SIZE = 1024;

    char buffer_[BUFFER_SIZE] = {0};
    size_t size_ = 0;
    size_t position_ = 0;
    size_t columnNo_ = 1;
    size_t lineNo_ = 1;
    bool isEof_ = false;
    bool isError_ = false;

    FILE *fd_ = nullptr;
    String path_;
    unsigned int mode_ = 0;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_FILE_H