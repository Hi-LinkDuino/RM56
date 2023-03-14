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

#ifndef OHOS_HIVIEWDFX_PARCELABLE_VECTOR_RW_H
#define OHOS_HIVIEWDFX_PARCELABLE_VECTOR_RW_H

#include <typeinfo>

#include "parcel.h"

namespace OHOS {
namespace HiviewDFX {
// no object in parcel
constexpr int VALUE_NULL = -1;
// object exist in parcel
constexpr int VALUE_OBJECT = 1;
constexpr int RULE_COUNT_LIMIT = 512 * 1024;

template<typename T>
bool ReadVectorFromParcel(Parcel& parcel, std::vector<T>& rules)
{
    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }
    if (empty == VALUE_OBJECT) {
        int size = 0;
        if (!parcel.ReadInt32(size) || (size > RULE_COUNT_LIMIT)) {
            return false;
        }
        rules.clear();
        for (int i = 0; i < size; i++) {
            T* rulePtr = parcel.ReadParcelable<T>();
            if (rulePtr == nullptr) {
                return false;
            }
            rules.emplace_back(*rulePtr);
            delete rulePtr;
            rulePtr = nullptr;
        }
    }
    return true;
}

template<typename T>
bool WriteVectorToParcel(Parcel& parcel, const std::vector<T>& rules)
{
    if (rules.size() == 0) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        size_t size = rules.size();
        if (!parcel.WriteInt32(size)) {
            return false;
        }
        for (size_t i = 0; i < size; i++) {
            if (!parcel.WriteParcelable(&rules[i])) {
                return false;
            }
        }
    }
    return true;
}
} // namespace HiviewDFX
} // namespace OHOS

#endif // OHOS_HIVIEWDFX_PARCELABLE_VECTOR_RW_H