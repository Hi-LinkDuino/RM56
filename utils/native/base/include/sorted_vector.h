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

#ifndef UTILS_BASE_SORTED_VECTOR_H
#define UTILS_BASE_SORTED_VECTOR_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <sys/types.h>
#include <vector>

namespace OHOS {

template <class TYPE, bool AllowDuplicate = true>
class SortedVector {

public:
    using value_type = TYPE;
    using size_type = std::size_t;
    using iterator = typename std::vector<TYPE>::iterator;
    using const_iterator = typename std::vector<TYPE>::const_iterator;

    // cont and dest

    SortedVector();

    SortedVector(const SortedVector<TYPE, false>& rhs);

    SortedVector(const SortedVector<TYPE, true>& rhs);

    SortedVector(const std::vector<TYPE>& orivect);

    virtual ~SortedVector() {}
    // copy operator
    SortedVector<TYPE, AllowDuplicate>& operator=(const SortedVector<TYPE, false>& rhs);
    SortedVector<TYPE, AllowDuplicate>& operator=(const SortedVector<TYPE, true>& rhs);

    inline void Clear() { vec_.clear(); }
    inline size_t Size() const { return vec_.size(); }
    inline bool IsEmpty() const { return vec_.empty(); }
    inline size_t Capacity() const { return vec_.capacity(); }

    ssize_t SetCapcity(size_t size)
    {
        if (size < vec_.capacity()) {
            return CAPCITY_NOT_CHANGED;
        }

        vec_.reserve(size);
        return size;
    }

    // Cstyle access
    // when use it , you should make sure it sorted~!
    inline const TYPE* Array() const { return vec_.data(); }
    TYPE* EditArray() { return vec_.data(); }

    ssize_t IndexOf(const TYPE& item) const;
    size_t OrderOf(const TYPE& item) const;

    // accessors
    inline const TYPE& operator[](size_t index) const { return vec_[index]; }

    const TYPE& Back() const { return vec_.back(); }
    const TYPE& Front() const { return vec_.front(); }
    void PopBack() { return vec_.pop_back(); }

    const TYPE& MirrorItemAt(ssize_t index) const
    {
        if (index < 0) {
            return *(vec_.end() + index);
        }
        return *(vec_.begin() + index);
    };

    // modify the array
    ssize_t Add(const TYPE& item);
    TYPE& EditItemAt(size_t index)
    {
        return vec_[index];
    }

    // merge a vector into this one
    size_t Merge(const std::vector<TYPE>& invec);
    size_t Merge(const SortedVector<TYPE, AllowDuplicate>& sortedVector);

    // erase an item at index
    iterator Erase(size_t index)
    {
        if (index >= vec_.size()) {
            return vec_.end();
        }
        return vec_.erase(vec_.begin() + index);
    }

    iterator Begin()
    {
        return vec_.begin();
    }

    const_iterator Begin() const
    {
        return vec_.begin();
    }

    iterator End()
    {
        return vec_.end();
    }

    const_iterator End() const
    {
        return vec_.end();
    }

    static const ssize_t NOT_FOUND = -1;
    static const ssize_t ADD_FAIL = -1;
    static const ssize_t CAPCITY_NOT_CHANGED = -1;

private:
    std::vector<TYPE> vec_;
};

template <class TYPE, bool AllowDuplicate>
inline SortedVector<TYPE, AllowDuplicate>::SortedVector()
    : vec_() {}

template <class TYPE, bool AllowDuplicate>
SortedVector<TYPE, AllowDuplicate>::SortedVector(const SortedVector<TYPE, false>& rhs)
{
    // this class: AllowDuplicate or Not AllowDuplicate same type
    std::copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
}

template <class TYPE, bool AllowDuplicate>
SortedVector<TYPE, AllowDuplicate>::SortedVector(const SortedVector<TYPE, true>& rhs)
{

    if (AllowDuplicate) {
        std::copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
    } else {
        // AllowDuplicate to Not AllowDuplicate
        std::unique_copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
    }
}

// copy operator
template <class TYPE, bool AllowDuplicate>
SortedVector<TYPE, AllowDuplicate>& SortedVector<TYPE, AllowDuplicate>::operator=(const SortedVector<TYPE, false>& rhs)
{
    // this class: AllowDuplicate or Not AllowDuplicate same type
    vec_.clear();
    std::copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
    return *this;
}

// copy operator
template <class TYPE, bool AllowDuplicate>
SortedVector<TYPE, AllowDuplicate>& SortedVector<TYPE, AllowDuplicate>::operator=(const SortedVector<TYPE, true>& rhs)
{
    vec_.clear();

    if (AllowDuplicate) {
        std::copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
    } else {
        // AllowDuplicate to Not AllowDuplicate
        std::unique_copy(rhs.Begin(), rhs.End(), std::back_inserter(vec_));
    }

    return *this;
}

template <class TYPE, bool AllowDuplicate>
ssize_t SortedVector<TYPE, AllowDuplicate>::IndexOf(const TYPE& item) const
{
    if (vec_.empty()) {
        return NOT_FOUND;
    }

    auto it = std::lower_bound(std::begin(vec_), std::end(vec_), item);
    if (it == vec_.end() || !(*it == item)) {
        return NOT_FOUND;
    }
    return it - vec_.begin();
}

template <class TYPE, bool AllowDuplicate>
size_t SortedVector<TYPE, AllowDuplicate>::OrderOf(const TYPE& item) const
{
    auto it = std::upper_bound(vec_.begin(), vec_.end(), item);
    return it - vec_.begin();
}

template <class TYPE, bool AllowDuplicate>
ssize_t SortedVector<TYPE, AllowDuplicate>::Add(const TYPE& item)
{
    ssize_t index = IndexOf(item);
    if (index != NOT_FOUND && !AllowDuplicate) {
        return ADD_FAIL;
    }

    auto it = std::upper_bound(vec_.begin(), vec_.end(), item);
    it = vec_.insert(it, item);
    return it - vec_.begin();
}

template <class TYPE, bool AllowDuplicate>
SortedVector<TYPE, AllowDuplicate>::SortedVector(const std::vector<TYPE>& invec)
{
    if (invec.empty()) {
        return;
    }

    std::vector<TYPE> newvector(invec);
    std::sort(newvector.begin(), newvector.end());
    if (AllowDuplicate) {
        vec_.swap(newvector);
    } else {
        std::unique_copy(newvector.begin(), newvector.end(), std::back_inserter(vec_));
    }
}

template <class TYPE, bool AllowDuplicate>
size_t SortedVector<TYPE, AllowDuplicate>::Merge(const std::vector<TYPE>& invec)
{
    SortedVector<TYPE, AllowDuplicate> sortedVector(invec);
    Merge(sortedVector);
    return vec_.size();
}

template <class TYPE, bool AllowDuplicate>
size_t SortedVector<TYPE, AllowDuplicate>::Merge(const SortedVector<TYPE, AllowDuplicate>& sortedVector)
{
    std::vector<TYPE> newVec;
    std::merge(vec_.begin(), vec_.end(), sortedVector.Begin(), sortedVector.End(), std::back_inserter(newVec));
    if (!AllowDuplicate) {
        vec_.clear();
        std::unique_copy(newVec.begin(), newVec.end(), std::back_inserter(vec_));
    } else {
        vec_.swap(newVec);
    }
    return vec_.size();
}

} // namespace OHOS
#endif
