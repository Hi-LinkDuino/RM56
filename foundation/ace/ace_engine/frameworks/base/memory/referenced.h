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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REFERENCED_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REFERENCED_H

#include <string>

#include "base/memory/memory_monitor.h"
#include "base/memory/ref_counter.h"
#include "base/utils/macros.h"

#define ACE_REMOVE(...)

namespace OHOS::Ace {

template<class T>
class RefPtr;
template<class T>
class WeakPtr;

// Inherit this class to use 'RefPtr' and 'WeakPtr' to manage pointer of instance.
class Referenced {
public:
    // Use raw pointer to construct 'RefPtr' and 'WeakPtr'.
    template<class T>
    static RefPtr<T> Claim(T* rawPtr)
    {
#ifdef ACE_MEMORY_MONITOR
        MemoryMonitor::GetInstance().Update(rawPtr, static_cast<Referenced*>(rawPtr));
#endif
        return RefPtr<T>(rawPtr);
    }
    template<class T>
    static WeakPtr<T> WeakClaim(T* rawPtr)
    {
        return WeakPtr<T>(rawPtr);
    }

    // 'Referenced::MakeRefPtr' is used to create new instance inherited from 'Referenced',
    // and manager its pointer using 'RefPtr'.
    template<class T, class... Args>
    static RefPtr<T> MakeRefPtr(Args&&... args)
    {
        return Claim(new T(std::forward<Args>(args)...));
    }

    // Get raw pointer from 'RefPtr'.
    template<class T>
    static T* RawPtr(const RefPtr<T>& ptr)
    {
        return ptr.rawPtr_;
    }

    int32_t IncRefCount()
    {
        return refCounter_->IncStrongRef();
    }
    int32_t DecRefCount()
    {
        int32_t refCount = refCounter_->DecStrongRef();
        if (refCount == 0 && MaybeRelease()) {
            // Release this instance, while its strong reference have reduced to zero.
            delete this;
        }
        return refCount;
    }
#ifdef ACE_MEMORY_MONITOR
    int32_t RefCount() const
    {
        return refCounter_->StrongRefCount();
    }
#endif

protected:
    explicit Referenced(bool threadSafe = true)
        : refCounter_(threadSafe ? ThreadSafeRef::Create() : ThreadUnsafeRef::Create())
    {
#ifdef ACE_MEMORY_MONITOR
        MemoryMonitor::GetInstance().Add(this);
#endif
    }

    virtual ~Referenced()
    {
        // Decrease weak reference count held by 'Referenced' itself.
        refCounter_->DecWeakRef();
        refCounter_ = nullptr;
#ifdef ACE_MEMORY_MONITOR
        MemoryMonitor::GetInstance().Remove(this);
#endif
    }

    virtual bool MaybeRelease()
    {
        return true;
    }

private:
    template<class T>
    friend class RefPtr;
    template<class T>
    friend class WeakPtr;

    RefCounter* refCounter_ { nullptr };

    ACE_DISALLOW_COPY_AND_MOVE(Referenced);
};

// Use reference count to manager instance inherited from 'Referenced'.
// Implicit conversion is necessary in some cases, so remove 'explicit' from construct function.
template<class T>
class RefPtr final {
public:
    // Basic constructors.
    RefPtr() = default;
    ACE_REMOVE(explicit) RefPtr(std::nullptr_t) {}
    // Basic copy and move constructors.
    ACE_REMOVE(explicit) RefPtr(const RefPtr& other) : RefPtr(other.rawPtr_) {}
    ACE_REMOVE(explicit) RefPtr(RefPtr&& other) : rawPtr_(other.rawPtr_)
    {
        other.rawPtr_ = nullptr;
    }
    // Construct instance by other 'RefPtr' that inherited from type 'T'.
    template<class O>
    ACE_REMOVE(explicit) RefPtr(const RefPtr<O>& other) : RefPtr(other.rawPtr_) {}
    template<class O>
    ACE_REMOVE(explicit) RefPtr(RefPtr<O>&& other) : rawPtr_(other.rawPtr_)
    {
        other.rawPtr_ = nullptr;
    }

    ~RefPtr()
    {
        if (rawPtr_ != nullptr) {
            // Decrease strong reference count.
            rawPtr_->DecRefCount();
            rawPtr_ = nullptr;
        }
    }

    void Swap(RefPtr& other)
    {
        std::swap(rawPtr_, other.rawPtr_);
    }
    void Swap(RefPtr&& other)
    {
        Swap(other);
    }
    void Reset()
    {
        Swap(RefPtr());
    }

    T* operator->() const
    {
        return rawPtr_;
    }
    T& operator*() const
    {
        ACE_DCHECK(rawPtr_ != nullptr);
        return *rawPtr_;
    }
    operator bool() const
    {
        return rawPtr_ != nullptr;
    }

    // Use 'Swap' to implement overloaded operator '='.
    // Construct a temporary 'RefPtr' by different parameters to increase strong reference count of the new instance,
    // swap with 'this', and then decrease strong reference of the old instance while destroying the temporary 'RefPtr'.
    RefPtr& operator=(const RefPtr& other)
    {
        if (this != &other) {
            Swap(RefPtr(other));
        }
        return *this;
    }
    RefPtr& operator=(RefPtr&& other)
    {
        if (this != &other) {
            Swap(RefPtr(std::move(other)));
        }
        return *this;
    }

    template<class O>
    RefPtr& operator=(O* rawPtr)
    {
        Swap(RefPtr(rawPtr));
        return *this;
    }
    template<class O>
    RefPtr& operator=(const RefPtr<O>& other)
    {
        Swap(RefPtr(other));
        return *this;
    }
    template<class O>
    RefPtr& operator=(RefPtr<O>&& other)
    {
        Swap(RefPtr(std::move(other)));
        return *this;
    }

    // Comparing pointer of reference counter to implement Overloaded operator '==' and '!='.
    template<class O>
    bool operator==(const O* rawPtr) const
    {
        if (rawPtr_ == nullptr) {
            return rawPtr == nullptr;
        }
        return rawPtr != nullptr && rawPtr_->refCounter_ == rawPtr->refCounter_;
    }
    template<class O>
    bool operator!=(const O* rawPtr) const
    {
        return !operator==(rawPtr);
    }

    template<class O>
    bool operator==(const RefPtr<O>& other) const
    {
        return *this == other.rawPtr_;
    }
    template<class O>
    bool operator!=(const RefPtr<O>& other) const
    {
        return !operator==(other);
    }

    template<class O>
    bool operator==(const WeakPtr<O>& weak) const
    {
        return weak == *this;
    }
    template<class O>
    bool operator!=(const WeakPtr<O>& weak) const
    {
        return !operator==(weak);
    }

    bool operator==(std::nullptr_t) const
    {
        return rawPtr_ == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return rawPtr_ != nullptr;
    }

    // Overload '<' is useful for 'std::map', 'std::set' and so on.
    template<class O>
    bool operator<(const RefPtr<O>& other) const
    {
        if (rawPtr_ == nullptr) {
            return other.rawPtr_ != nullptr;
        }
        return other.rawPtr_ != nullptr && rawPtr_->refCounter_ < other.rawPtr_->refCounter_;
    }

private:
    // Construct instance by raw pointer.
    // 'WeakPtr' may construct 'RefPtr' without increasing its strong reference count,
    // because strong reference count is already increased in 'WeakPtr' while upgrading.
    // In that case, 'forceIncRef' should be 'false'.
    explicit RefPtr(T* rawPtr, bool forceIncRef = true) : rawPtr_(rawPtr)
    {
        if (rawPtr_ != nullptr && forceIncRef) {
            // Increase strong reference count for holding instance.
            rawPtr_->IncRefCount();
        }
    }

    friend class Referenced;
    template<class O>
    friend class RefPtr;
    template<class O>
    friend class WeakPtr;

    T* rawPtr_ { nullptr };
};

// Working with 'RefPtr' to resolve 'circular reference'.
// Implicit conversion is necessary in some cases, so remove 'explicit' from construct function.
template<class T>
class WeakPtr final {
public:
    // Basic constructors.
    WeakPtr() = default;
    ACE_REMOVE(explicit) WeakPtr(std::nullptr_t) {}
    // Basic copy and move constructors.
    ACE_REMOVE(explicit) WeakPtr(const WeakPtr& other) : WeakPtr(other.unsafeRawPtr_, other.refCounter_) {}
    ACE_REMOVE(explicit) WeakPtr(WeakPtr&& other) : unsafeRawPtr_(other.unsafeRawPtr_), refCounter_(other.refCounter_)
    {
        other.unsafeRawPtr_ = nullptr;
        other.refCounter_ = nullptr;
    }
    // Construct instance by other 'WeakPtr' that inherited from type 'T'.
    template<class O>
    ACE_REMOVE(explicit) WeakPtr(const WeakPtr<O>& other) : WeakPtr(other.unsafeRawPtr_, other.refCounter_) {}
    template<class O>
    ACE_REMOVE(explicit) WeakPtr(WeakPtr<O>&& other)
        : unsafeRawPtr_(other.unsafeRawPtr_), refCounter_(other.refCounter_)
    {
        other.unsafeRawPtr_ = nullptr;
        other.refCounter_ = nullptr;
    }
    // Construct instance by 'RefPtr' that inherited from type 'T' or 'T' itself.
    template<class O>
    ACE_REMOVE(explicit) WeakPtr(const RefPtr<O>& other) : WeakPtr(other.rawPtr_) {}

    ~WeakPtr()
    {
        // Decrease weak reference count while releasing reference counter.
        if (refCounter_ != nullptr) {
            refCounter_->DecWeakRef();
            refCounter_ = nullptr;
            unsafeRawPtr_ = nullptr;
        }
    }

    void Swap(WeakPtr& other)
    {
        std::swap(unsafeRawPtr_, other.unsafeRawPtr_);
        std::swap(refCounter_, other.refCounter_);
    }
    void Swap(WeakPtr&& other)
    {
        Swap(other);
    }
    void Reset()
    {
        Swap(WeakPtr());
    }

    RefPtr<T> Upgrade() const
    {
        // A 'WeakPtr' could upgrade to 'RefPtr' if this instance is still alive.
        return refCounter_ != nullptr && refCounter_->TryIncStrongRef() > 0 ? RefPtr<T>(unsafeRawPtr_, false) : nullptr;
    }
    bool Invalid() const
    {
        return refCounter_ == nullptr || refCounter_->StrongRefCount() == 0;
    }

    // Use 'Swap' to implement overloaded operator '=', just like 'RefPtr'.
    WeakPtr& operator=(const WeakPtr& other)
    {
        if (this != &other) {
            Swap(WeakPtr(other));
        }
        return *this;
    }
    WeakPtr& operator=(WeakPtr&& other)
    {
        if (this != &other) {
            Swap(WeakPtr(std::move(other)));
        }
        return *this;
    }

    template<class O>
    WeakPtr& operator=(O* rawPtr)
    {
        Swap(WeakPtr(rawPtr));
        return *this;
    }
    template<class O>
    WeakPtr& operator=(const WeakPtr<O>& other)
    {
        Swap(WeakPtr(other));
        return *this;
    }
    template<class O>
    WeakPtr& operator=(WeakPtr<O>&& other)
    {
        Swap(WeakPtr(std::move(other)));
        return *this;
    }
    template<class O>
    WeakPtr& operator=(const RefPtr<O>& other)
    {
        Swap(WeakPtr(other.rawPtr_));
        return *this;
    }

    // Comparing pointer of reference counter to implement Overloaded operator '==' and '!=', just like 'RefPtr'.
    template<class O>
    bool operator==(const O* rawPtr) const
    {
        if (refCounter_ == nullptr) {
            return rawPtr == nullptr;
        }
        return rawPtr != nullptr && refCounter_ == rawPtr->refCounter_;
    }
    template<class O>
    bool operator!=(const O* rawPtr) const
    {
        return !operator==(rawPtr);
    }

    template<class O>
    bool operator==(const WeakPtr<O>& other) const
    {
        return refCounter_ == other.refCounter_;
    }
    template<class O>
    bool operator!=(const WeakPtr<O>& other) const
    {
        return !operator==(other);
    }

    template<class O>
    bool operator==(const RefPtr<O>& strong) const
    {
        return strong.rawPtr_ != nullptr ? strong.rawPtr_->refCounter_ == refCounter_ : refCounter_ == nullptr;
    }
    template<class O>
    bool operator!=(const RefPtr<O>& strong) const
    {
        return !operator==(strong);
    }

    // Overload '<' is useful for 'std::map', 'std::set' and so on, just like 'RefPtr'.
    template<class O>
    bool operator<(const WeakPtr<O>& other) const
    {
        return refCounter_ < other.refCounter_;
    }

private:
    // Construct instance by raw pointer.
    explicit WeakPtr(T* rawPtr) : WeakPtr(rawPtr, rawPtr != nullptr ? rawPtr->refCounter_ : nullptr) {}
    template<class O>
    WeakPtr(O* rawPtr, RefCounter* aceRef) : unsafeRawPtr_(rawPtr), refCounter_(aceRef)
    {
        if (refCounter_) {
            refCounter_->IncWeakRef();
        }
    }

    friend class Referenced;
    template<class O>
    friend class WeakPtr;

    // Notice: Raw pointer of instance is kept, but NEVER use it except succeed to upgrade to 'RefPtr'.
    T* unsafeRawPtr_ { nullptr };
    RefCounter* refCounter_ { nullptr };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_REFERENCED_H
