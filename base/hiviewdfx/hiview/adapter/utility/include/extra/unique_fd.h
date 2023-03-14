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

#ifndef UNIQUE_FD_H
#define UNIQUE_FD_H
#ifndef _WIN32
#include <unistd.h>
#endif // !_WIN32

namespace OHOS {
class DefaultDeleter {
public:
    static void Close(int fd)
    {
        if (fd >= 0) {
#ifdef _WIN32
            Close(fd);
#else
            close(fd);
#endif
        }
    }
};

template<typename Deleter>
class UniqueFdAddDeletor;
template<typename Deleter>
bool operator==(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);
template<typename Deleter>
bool operator!=(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);
template<typename Deleter>
bool operator>=(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);
template<typename Deleter>
bool operator>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);
template<typename Deleter>
bool operator<=(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);
template<typename Deleter>
bool operator<(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

template<typename Deleter = DefaultDeleter>
class UniqueFdAddDeletor final {
    friend bool operator==<Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

    friend bool operator!=<Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

    friend bool operator>=<Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

    friend bool operator><Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

    friend bool operator<=<Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

    friend bool operator< <Deleter>(const int& lhs, const UniqueFdAddDeletor<Deleter>& rhs);

public:
    explicit UniqueFdAddDeletor(const int& value)
        : fd_(value)
    {
    }
    UniqueFdAddDeletor()
        : fd_(-1)
    {
    }
    ~UniqueFdAddDeletor()
    {
        Reset(-1);
    }

    // get fd out
    int Release()
    {
        int tmp = fd_;
        fd_ = -1;
        return tmp;
    }

    // this is dangerous, when you use it , you should know it, donot operator on the ret
    operator int() const
    {
        return Get();
    } // NOLINT
    // this is dangerous, when you use it , you should know it, donot operator on the ret
    int Get() const
    {
        return fd_;
    }

    // we need move fd from one to another
    UniqueFdAddDeletor(UniqueFdAddDeletor && rhs)
    {
        int rhsfd = rhs.Release();
        fd_ = rhsfd;
    }

    UniqueFdAddDeletor& operator=(UniqueFdAddDeletor && rhs)
    {
        int rhsfd = rhs.Release();
        Reset(rhsfd);
        return *this;
    }

    bool operator==(const int& rhs) const
    {
        return fd_ == rhs;
    }

    bool operator!=(const int& rhs) const
    {
        return !(fd_ == rhs);
    }
    bool operator>=(const int& rhs) const
    {
        return fd_ >= rhs;
    }

    bool operator>(const int& rhs) const
    {
        return fd_ > rhs;
    }

    bool operator<=(const int& rhs) const
    {
        return fd_ <= rhs;
    }

    bool operator<(const int& rhs) const
    {
        return fd_ < rhs;
    }

private:
    int fd_ = -1;

    void Reset(int newValue)
    {
        if (fd_ >= 0) {
            Deleter::Close(fd_);
        }
        fd_ = newValue;
    }

    // disallow copy ctor and copy assign
    UniqueFdAddDeletor(const UniqueFdAddDeletor& rhs) = delete;
    UniqueFdAddDeletor& operator=(const UniqueFdAddDeletor& rhs) = delete;
};

template<typename Deleter = DefaultDeleter>
bool operator==(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return lhs == uniqueFd.fd_;
}

template<typename Deleter = DefaultDeleter>
bool operator!=(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return !(lhs == uniqueFd.fd_);
}

template<typename Deleter = DefaultDeleter>
bool operator>=(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return lhs >= uniqueFd.fd_;
}

template<typename Deleter = DefaultDeleter>
bool operator>(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return lhs > uniqueFd.fd_;
}

template<typename Deleter = DefaultDeleter>
bool operator<=(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return lhs <= uniqueFd.fd_;
}

template<typename Deleter = DefaultDeleter>
bool operator<(const int& lhs, const UniqueFdAddDeletor<Deleter>& uniqueFd)
{
    return lhs < uniqueFd.fd_;
}

using UniqueFd = UniqueFdAddDeletor<DefaultDeleter>;
} // namespace OHOS
#endif
