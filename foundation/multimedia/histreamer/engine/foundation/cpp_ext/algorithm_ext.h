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

#ifndef HISTREAMER_FOUNDATION_CPP_EXT_ALGO_EXT_H
#define HISTREAMER_FOUNDATION_CPP_EXT_ALGO_EXT_H
namespace OHOS {
namespace Media {
namespace CppExt {
template<class Iterator, class UnaryPredicate>
inline bool AnyOf(Iterator first, Iterator last, UnaryPredicate p) // implementation of std::any_of used on c++11
{
    while (first != last) {
        if (p(*first)) {
            return true;
        }
        ++first;
    }
    return false;
}
} // CppExt
} // Media
} // OHOS
#endif // HISTREAMER_FOUNDATION_CPP_EXT_ALGO_EXT_H
