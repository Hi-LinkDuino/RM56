/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifndef HISTREAMER_FOUNDATION_CPP_EXT_MEMORY_EXT_H
#define HISTREAMER_FOUNDATION_CPP_EXT_MEMORY_EXT_H
namespace OHOS {
namespace Media {
namespace CppExt {
template<class T> struct _unique_ptr_if {
    typedef std::unique_ptr<T> _single_object;
};

template<class T> struct _unique_ptr_if<T[]> {
    typedef std::unique_ptr<T[]> _unknown_bound_array;
};

template<class T, size_t N> struct _unique_ptr_if<T[N]> {
    typedef void _known_bound_array; // not supported
};

template<class T, class... U>
typename _unique_ptr_if<T>::_single_object make_unique(U&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<U>(params)...));
}

template<class T>
typename _unique_ptr_if<T>::_unknown_bound_array make_unique(size_t n)
{
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _unique_ptr_if<T>::_known_bound_array make_unique(Args&&...) = delete;
} // CppExt
} // Media
} // OHOS
#endif // HISTREAMER_FOUNDATION_CPP_EXT_MEMORY_EXT_H
