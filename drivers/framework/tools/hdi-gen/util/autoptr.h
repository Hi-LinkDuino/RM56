/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_AUTOPTR_H
#define OHOS_HDI_AUTOPTR_H

namespace OHOS {
namespace HDI {
template <class T>
class AutoPtr {
public:
    inline AutoPtr() : mPtr(nullptr) {}

    AutoPtr(T *other);

    AutoPtr(const AutoPtr<T> &other);

    AutoPtr(AutoPtr<T> &&other);

    ~AutoPtr();

    AutoPtr &operator=(T *other);

    AutoPtr &operator=(const AutoPtr<T> &other);

    AutoPtr &operator=(AutoPtr<T> &&other);

    void MoveTo(T **other);

    inline operator T *() const;

    inline T **operator&();

    inline T *operator->() const;

    inline T &operator*() const;

    inline T *Get() const;

    inline bool operator==(T *other) const;

    inline bool operator==(const AutoPtr<T> &other) const;

    inline bool operator!=(T *other) const;

    inline bool operator!=(const AutoPtr<T> &other) const;

    inline bool operator>(T *other) const;

    inline bool operator>(const AutoPtr<T> &other) const;

    inline bool operator<(T *other) const;

    inline bool operator<(const AutoPtr<T> &other) const;

    inline bool operator<=(T *other) const;

    inline bool operator<=(const AutoPtr<T> &other) const;

    inline bool operator>=(T *other) const;

    inline bool operator>=(const AutoPtr<T> &other) const;

private:
    T *mPtr;
};

template <class T>
AutoPtr<T>::AutoPtr(T *other) : mPtr(other)
{
    if (mPtr != nullptr) {
        mPtr->AddRef();
    }
}

template <class T>
AutoPtr<T>::AutoPtr(const AutoPtr<T> &other) : mPtr(other.mPtr)
{
    if (mPtr != nullptr) {
        mPtr->AddRef();
    }
}

template <class T>
AutoPtr<T>::AutoPtr(AutoPtr<T> &&other) : mPtr(other.mPtr)
{
    other.mPtr = nullptr;
}

template <class T>
AutoPtr<T>::~AutoPtr()
{
    if (mPtr != nullptr) {
        mPtr->Release();
    }
}

template <class T>
AutoPtr<T> &AutoPtr<T>::operator=(T *other)
{
    if (mPtr == other)
        return *this;

    if (other != nullptr) {
        other->AddRef();
    }
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other;
    return *this;
}

template <class T>
AutoPtr<T> &AutoPtr<T>::operator=(const AutoPtr<T> &other)
{
    if (mPtr == other.mPtr)
        return *this;

    if (other.mPtr != nullptr) {
        other.mPtr->AddRef();
    }
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other.mPtr;
    return *this;
}

template <class T>
AutoPtr<T> &AutoPtr<T>::operator=(AutoPtr<T> &&other)
{
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other.mPtr;
    other.mPtr = nullptr;
    return *this;
}

template <class T>
void AutoPtr<T>::MoveTo(T **other)
{
    if (other != nullptr) {
        *other = mPtr;
        mPtr = nullptr;
    }
}

template <class T>
AutoPtr<T>::operator T *() const
{
    return mPtr;
}

template <class T>
T **AutoPtr<T>::operator&()
{
    return &mPtr;
}

template <class T>
T *AutoPtr<T>::operator->() const
{
    return mPtr;
}

template <class T>
T &AutoPtr<T>::operator*() const
{
    return *mPtr;
}

template <class T>
T *AutoPtr<T>::Get() const
{
    return mPtr;
}

template <class T>
bool AutoPtr<T>::operator==(T *other) const
{
    return mPtr == other;
}

template <class T>
bool AutoPtr<T>::operator==(const AutoPtr<T> &other) const
{
    return mPtr == other.mPtr;
}

template <class T>
bool AutoPtr<T>::operator!=(T *other) const
{
    return mPtr != other;
}

template <class T>
bool AutoPtr<T>::operator!=(const AutoPtr<T> &other) const
{
    return mPtr != other.mPtr;
}

template <class T>
bool AutoPtr<T>::operator>(T *other) const
{
    return mPtr > other;
}

template <class T>
bool AutoPtr<T>::operator>(const AutoPtr<T> &other) const
{
    return mPtr > other.mPtr;
}

template <class T>
bool AutoPtr<T>::operator<(T *other) const
{
    return mPtr < other;
}

template <class T>
bool AutoPtr<T>::operator<(const AutoPtr<T> &other) const
{
    return mPtr < other.mPtr;
}

template <class T>
bool AutoPtr<T>::operator<=(T *other) const
{
    return mPtr <= other;
}

template <class T>
bool AutoPtr<T>::operator<=(const AutoPtr<T> &other) const
{
    return mPtr <= other.mPtr;
}

template <class T>
bool AutoPtr<T>::operator>=(T *other) const
{
    return mPtr >= other;
}

template <class T>
bool AutoPtr<T>::operator>=(const AutoPtr<T> &other) const
{
    return mPtr >= other.mPtr;
}
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_AUTOPTR_H