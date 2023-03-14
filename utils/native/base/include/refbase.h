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

#ifndef UTILS_BASE_REFBASE_H
#define UTILS_BASE_REFBASE_H

#include <atomic>
#include <functional>

namespace OHOS {

#define INITIAL_PRIMARY_VALUE (1 << 28)

class RefBase;

class RefCounter {
public:
    using RefPtrCallback = std::function<void()>;
    friend class RefBase;

    RefCounter();

    explicit RefCounter(RefCounter *counter);

    RefCounter &operator=(const RefCounter &counter);

    virtual ~RefCounter();

    void SetCallback(const RefPtrCallback& callback);

    void RemoveCallback();

    int GetRefCount();

    void IncRefCount();

    void DecRefCount();

    bool IsRefPtrValid();

    int IncStrongRefCount(const void *objectId);

    int DecStrongRefCount(const void *objectId);

    int GetStrongRefCount();

    int IncWeakRefCount(const void *objectId);

    int DecWeakRefCount(const void *objectId);

    int GetWeakRefCount();

    void SetAttemptAcquire();

    bool IsAttemptAcquireSet();

    void ClearAttemptAcquire();

    bool AttemptIncStrongRef(const void *objectId, int &outCount);

    // Only for IPC use.
    bool AttemptIncStrong(const void *objectId);

    bool IsLifeTimeExtended();

    void ExtendObjectLifetime();

private:
    std::atomic<int> atomicStrong_;
    std::atomic<int> atomicWeak_;
    std::atomic<int> atomicRefCount_;
    std::atomic<unsigned int> atomicFlags_;
    std::atomic<int> atomicAttempt_;
    RefPtrCallback callback_ = nullptr;
    static constexpr unsigned int FLAG_EXTEND_LIFE_TIME = 0x00000002;
};

class WeakRefCounter {
public:
    WeakRefCounter(RefCounter *base, void *cookie);

    virtual ~WeakRefCounter();

    void *GetRefPtr();

    void IncWeakRefCount(const void *objectId);

    void DecWeakRefCount(const void *objectId);

    bool AttemptIncStrongRef(const void *objectId);

private:
    std::atomic<int> atomicWeak_;
    RefCounter *refCounter_ = nullptr;
    void *cookie_ = nullptr;
};

class RefBase {
public:
    RefBase();

    RefBase(const RefBase &refbase);

    RefBase &operator=(const RefBase &refbase);

    RefBase(RefBase &&refbase) noexcept;

    RefBase &operator=(RefBase &&refbase) noexcept;

    virtual ~RefBase();

    virtual void RefPtrCallback();

    void ExtendObjectLifetime();

    void IncStrongRef(const void *objectId);

    void DecStrongRef(const void *objectId);

    int GetSptrRefCount();

    WeakRefCounter *CreateWeakRef(void *cookie);

    void IncWeakRef(const void *objectId);

    void DecWeakRef(const void *objectId);

    int GetWptrRefCount();

    bool AttemptAcquire(const void *objectId);

    bool AttemptIncStrongRef(const void *objectId);

    // Only for IPC use.
    bool AttemptIncStrong(const void *objectId);

    bool IsAttemptAcquireSet();

    bool IsExtendLifeTimeSet();

    virtual void OnFirstStrongRef(const void *objectId);

    virtual void OnLastStrongRef(const void *objectId);

    virtual void OnLastWeakRef(const void *objectId);

    virtual bool OnAttemptPromoted(const void *objectId);

private:
    RefCounter *refs_ = nullptr;
};

template <typename T>
class wptr;

template <typename T>

class sptr {
    friend class wptr<T>;

public:
    sptr();

    ~sptr();

    sptr(T *other);

    sptr(const sptr<T> &other);

    sptr(sptr<T> &&other);

    sptr<T> &operator=(sptr<T> &&other);

    template <typename O>
    sptr(const sptr<O> &other);

    inline sptr(WeakRefCounter *p, bool force);

    inline T *GetRefPtr() const
    {
        return refs_;
    }

    inline void ForceSetRefPtr(T *other);

    void clear();

    inline operator T *() const
    {
        return refs_;
    }

    inline T &operator*() const
    {
        return *refs_;
    }

    inline T *operator->() const
    {
        return refs_;
    }

    inline bool operator!() const
    {
        return refs_ == nullptr;
    }

    sptr<T> &operator=(T *other);

    sptr<T> &operator=(const sptr<T> &other);

    sptr<T> &operator=(const wptr<T> &other);

    template <typename O>
    sptr<T> &operator=(const sptr<O> &other);

    bool operator==(const T *other) const;

    inline bool operator!=(const T *other) const
    {
        return !operator==(other);
    }

    bool operator==(const wptr<T> &other) const;

    inline bool operator!=(const wptr<T> &other) const
    {
        return !operator==(other);
    }

    bool operator==(const sptr<T> &other) const;

    inline bool operator!=(const sptr<T> &other) const
    {
        return !operator==(other);
    }

private:
    T *refs_ = nullptr;
};

template <typename T>
inline void sptr<T>::ForceSetRefPtr(T *other)
{
    refs_ = other;
}

template <typename T>
inline sptr<T>::sptr()
{
    refs_ = nullptr;
}

template <typename T>
inline sptr<T>::sptr(T *other)
{
    refs_ = other;
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
inline sptr<T>::sptr(const sptr<T> &other)
{
    refs_ = other.GetRefPtr();
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
sptr<T>::sptr(sptr<T> &&other)
{
    refs_ = other.GetRefPtr();
    other.ForceSetRefPtr(nullptr);
}

template <typename T>
sptr<T> &sptr<T>::operator=(sptr<T> &&other)
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
    refs_ = other.GetRefPtr();
    other.ForceSetRefPtr(nullptr);
    return *this;
}

template <typename T>
template <typename O>
sptr<T>::sptr(const sptr<O> &other) : refs_(other.GetRefPtr())
{
    if (refs_ != nullptr) {
        refs_->IncStrongRef(this);
    }
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(T *other)
{
    if (other != nullptr) {
        other->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = other;
    return *this;
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(const sptr<T> &other)
{
    T *otherRef(other.GetRefPtr());
    if (otherRef != nullptr) {
        otherRef->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = otherRef;
    return *this;
}

template <typename T>
inline sptr<T> &sptr<T>::operator=(const wptr<T> &other)
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
    if ((other != nullptr) && other.AttemptIncStrongRef(this)) {
        refs_ = other.GetRefPtr();
    } else {
        refs_ = nullptr;
    }

    return *this;
}

template <typename T>
template <typename O>
sptr<T> &sptr<T>::operator=(const sptr<O> &other)
{
    T *otherRef(other.GetRefPtr());
    if (otherRef != nullptr) {
        otherRef->IncStrongRef(this);
    }

    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }

    refs_ = otherRef;
    return *this;
}

template <typename T>
inline bool sptr<T>::operator==(const T *other) const
{
    return other == refs_;
}

template <typename T>
inline bool sptr<T>::operator==(const wptr<T> &other) const
{
    return refs_ == other.GetRefPtr();
}

template <typename T>
inline bool sptr<T>::operator==(const sptr<T> &other) const
{
    return refs_ == other.GetRefPtr();
}

template<typename T>
void sptr<T>::clear()
{
    if (refs_) {
        refs_->DecStrongRef(this);
        refs_ = 0;
    }
}

template <typename T>
inline sptr<T>::~sptr()
{
    if (refs_ != nullptr) {
        refs_->DecStrongRef(this);
    }
}

template <typename T>
inline sptr<T>::sptr(WeakRefCounter *p, bool /* force */)
{
    if ((p != nullptr) && p->AttemptIncStrongRef(this)) {
        refs_ = reinterpret_cast<T *>(p->GetRefPtr());
    } else {
        refs_ = nullptr;
    }
}

template <typename T>
class wptr {
    template <typename O>
    friend class wptr;

public:
    wptr();

    wptr(T *other);

    wptr(const wptr<T> &other);

    wptr(const sptr<T> &other);

    template <typename O>
    wptr(const wptr<O> &other);

    template <typename O>
    wptr(const sptr<O> &other);

    wptr<T> &operator=(T *other);

    template <typename O>
    wptr<T> &operator=(O *other);

    wptr<T> &operator=(const wptr<T> &other);

    wptr<T> &operator=(const sptr<T> &other);

    template <typename O>
    wptr<T> &operator=(const wptr<O> &other);

    template <typename O>
    wptr<T> &operator=(const sptr<O> &other);

    inline T *operator*() const
    {
        return *refs_;
    }
    inline T *operator->() const
    {
        return reinterpret_cast<T *>(refs_->GetRefPtr());
    }

    bool operator==(const T *other) const;

    inline bool operator!=(const T *other) const
    {
        return !operator==(other);
    };

    bool operator==(const wptr<T> &other) const;

    inline bool operator!=(const wptr<T> &other) const
    {
        return !operator==(other);
    }

    bool operator==(const sptr<T> &other) const;

    inline bool operator!=(const sptr<T> &other) const
    {
        return !operator==(other);
    }

    T *GetRefPtr() const;

    inline bool AttemptIncStrongRef(const void *objectId) const
    {
        return refs_->AttemptIncStrongRef(objectId);
    }

    const sptr<T> promote() const;

    ~wptr();

private:
    WeakRefCounter *refs_ = nullptr;
};

template <typename T>
inline T *wptr<T>::GetRefPtr() const
{
    return (refs_ != nullptr) ? reinterpret_cast<T *>(refs_->GetRefPtr()) : nullptr;
}

template <typename T>
wptr<T>::wptr()
{
    refs_ = nullptr;
}

template <typename T>
wptr<T>::wptr(T *other)
{
    if (other != nullptr) {
        refs_ = other->CreateWeakRef(other);
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    } else {
        refs_ = nullptr;
    }
}

template <typename T>
wptr<T>::wptr(const wptr<T> &other)
{
    refs_ = other.refs_;
    if (refs_ != nullptr) {
        refs_->IncWeakRefCount(this);
    }
}

template <typename T>
wptr<T>::wptr(const sptr<T> &other)
{
    if (other.GetRefPtr() != nullptr) {
        refs_ = other->CreateWeakRef(other.GetRefPtr());
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    }
}

template <typename T>
template <typename O>
wptr<T>::wptr(const wptr<O> &other)
{
    refs_ = other.refs_;
    if (refs_ != nullptr) {
        refs_->IncWeakRefCount(this);
    }
}

template <typename T>
template <typename O>
wptr<T>::wptr(const sptr<O> &other)
{
    if (other.GetRefPtr() != nullptr) {
        refs_ = other->CreateWeakRef(other.GetRefPtr());
        if (refs_ != nullptr) {
            refs_->IncWeakRefCount(this);
        }
    }
}

template <typename T>
wptr<T> &wptr<T>::operator=(T *other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other != nullptr) {
        newWeakRef = other->CreateWeakRef(other);
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(O *other)
{
    T *object = reinterpret_cast<T *>(other);
    WeakRefCounter *newWeakRef = nullptr;
    if (object != nullptr) {
        newWeakRef = object->CreateWeakRef(object);
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
inline wptr<T> &wptr<T>::operator=(const wptr<T> &other)
{
    if (other.refs_ != nullptr) {
        other.refs_->IncWeakRefCount(this);
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = other.refs_;
    return *this;
}

template <typename T>
inline wptr<T> &wptr<T>::operator=(const sptr<T> &other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other.GetRefPtr() != nullptr) {
        newWeakRef = other->CreateWeakRef(other.GetRefPtr());
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(const wptr<O> &other)
{
    if (other.refs_ != nullptr) {
        other.refs_->IncWeakRefCount(this);
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = other.refs_;
    return *this;
}

template <typename T>
template <typename O>
wptr<T> &wptr<T>::operator=(const sptr<O> &other)
{
    WeakRefCounter *newWeakRef = nullptr;
    if (other.GetRefPtr() != nullptr) {
        newWeakRef = other->CreateWeakRef(other->GetRefPtr());
        if (newWeakRef != nullptr) {
            newWeakRef->IncWeakRefCount(this);
        }
    }

    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }

    refs_ = newWeakRef;
    return *this;
}

template <typename T>
inline bool wptr<T>::operator==(const T *other) const
{
    return GetRefPtr() == other;
}

template <typename T>
inline bool wptr<T>::operator==(const wptr<T> &other) const
{
    return GetRefPtr() == other.GetRefPtr();
}

template <typename T>
inline bool wptr<T>::operator==(const sptr<T> &other) const
{
    return GetRefPtr() == other.GetRefPtr();
}

template <typename T>
inline const sptr<T> wptr<T>::promote() const
{
    return sptr<T>(refs_, true);
}

template <typename T>
inline wptr<T>::~wptr()
{
    if (refs_ != nullptr) {
        refs_->DecWeakRefCount(this);
    }
}

} // namespace OHOS

#endif
