/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "util/file.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "securec.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
#ifdef __MINGW32__
constexpr unsigned int File::READ;
constexpr unsigned int File::WRITE;
constexpr unsigned int File::APPEND;
#endif

File::File(const String &path, unsigned int mode) : mode_(mode)
{
    if (path.IsEmpty()) {
        return;
    }

    if (mode_ & READ) {
        if (!CheckValid(path)) {
            return;
        }
        fd_ = fopen(path.string(), "r");
    } else if (mode_ & WRITE) {
        fd_ = fopen(path.string(), "w+");
    } else if (mode_ & APPEND) {
        fd_ = fopen(path.string(), "a+");
    }

    if (fd_ != nullptr) {
#ifndef __MINGW32__
        char *absolutePath = realpath(path.string(), nullptr);
        if (absolutePath != nullptr) {
            path_ = absolutePath;
            free(absolutePath);
        } else {
            path_ = path;
        }
#else
        char absolutePath[_MAX_PATH];
        _fullpath(absolutePath, path.string(), _MAX_PATH);
        path_ = absolutePath;
#endif
    }
}

File::~File()
{
    Close();
}

char File::GetChar()
{
    char c = PeekChar();

    if (position_ + 1 <= size_) {
        position_++;

        if (c != '\n') {
            columnNo_++;
        } else {
            columnNo_ = 0;
            lineNo_++;
        }
    }
    return c;
}

char File::PeekChar()
{
    if (position_ + 1 > size_) {
        int ret = Read();
        if (ret == -1) {
            isEof_ = true;
        }
    }

    return buffer_[position_];
}

bool File::IsEof() const
{
    return isEof_ || buffer_[position_] == -1;
}

int File::Read()
{
    if (isEof_ || isError_) {
        return -1;
    }

    (void)memset_s(buffer_, BUFFER_SIZE, 0, BUFFER_SIZE);
    size_t count = fread(buffer_, 1, BUFFER_SIZE - 1, fd_);
    if (count < BUFFER_SIZE - 1) {
        isError_ = ferror(fd_) != 0;
        buffer_[count] = -1;
    }
    size_ = count;
    position_ = 0;
    return (count != 0) ? count : -1;
}

bool File::ReadData(void *data, size_t size) const
{
    if (data == nullptr || size == 0) {
        return true;
    }

    if (fd_ == nullptr) {
        return false;
    }

    size_t count = fread(data, size, 1, fd_);
    return count == 1;
}

bool File::WriteData(const void *data, size_t size) const
{
    if (data == nullptr || size == 0) {
        return true;
    }

    if (fd_ == nullptr || !(mode_ & (WRITE | APPEND))) {
        return false;
    }

    size_t count = fwrite(data, size, 1, fd_);
    return count == 1;
}

void File::Flush()
{
    if ((mode_ & (WRITE | APPEND)) && fd_ != nullptr) {
        fflush(fd_);
    }
}

bool File::Reset()
{
    if (fd_ == nullptr) {
        return false;
    }

    return fseek(fd_, 0, SEEK_SET) == 0;
}

bool File::Skip(long size)
{
    if (fd_ == nullptr) {
        return false;
    }

    return fseek(fd_, size, SEEK_CUR) == 0;
}

void File::Close()
{
    if (fd_ != nullptr) {
        fclose(fd_);
        fd_ = nullptr;
    }
}

bool File::CreateParentDir(const String &path)
{
    if (!access(path.string(), F_OK | R_OK | W_OK)) {
        return true;
    }

    int pos = 1;

#ifndef __MINGW32__
    char Separator = '/';
#else
    char Separator = '\\';
#endif

    while ((pos = path.IndexOf(Separator, pos)) != -1) {
        String partPath = path.Substring(0, pos);
        struct stat st;
        if (stat(partPath.string(), &st) < 0) {
            if (errno != ENOENT) {
                return false;
            }

#ifndef __MINGW32__
            if (mkdir(partPath.string(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
#else
            if (mkdir(partPath.string()) < 0) {
#endif
                return false;
            }
        } else if (!S_ISDIR(st.st_mode)) {
            return false;
        }
        pos += 1;
    }
    return true;
}

String File::AdapterPath(const String &path)
{
#ifndef __MINGW32__
    return path;
#else
    return path.Replace('/', '\\');
#endif
}

size_t File::GetHashKey()
{
    StringBuilder fileStr;
    while (!IsEof()) {
        fileStr.Append(GetChar());
    }

    return std::hash<std::string>()(fileStr.ToString().string());
}

bool File::CheckValid(const String &path)
{
    if (access(path.string(), F_OK | R_OK | W_OK)) {
        return false;
    }

    struct stat st;
    if (stat(path.string(), &st) < 0) {
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        return false;
    }

    return true;
}
} // namespace HDI
} // namespace OHOS