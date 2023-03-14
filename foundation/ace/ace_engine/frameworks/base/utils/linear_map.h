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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LINEAR_MAP_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LINEAR_MAP_H

#include <algorithm>
#include <cstring>

namespace OHOS::Ace {
template<typename T>
struct LinearMapNode {
    const char* key;
    T value;
};

// the key type K must can be compared.
template<typename K, typename V>
struct LinearEnumMapNode {
    K key;
    V value;
};

// these binary search functions in linear map, so the map must be sorted by key.
template<typename T>
int64_t BinarySearchFindIndex(const LinearMapNode<T>* vec, size_t length, const char* key)
{
    auto it = std::lower_bound(
        vec, vec + length, key, [](LinearMapNode<T> lhs, const char* key) { return strcmp(lhs.key, key) < 0; });
    if (it == vec + length || strcmp(it->key, key) != 0) {
        return -1;
    } else {
        return it - vec;
    }
}

template<typename K, typename V>
int64_t BinarySearchFindIndex(const LinearEnumMapNode<K, V>* vec, size_t length, K key)
{
    auto it =
        std::lower_bound(vec, vec + length, key, [](LinearEnumMapNode<K, V> lhs, K key) { return lhs.key < key; });
    if (it == vec + length || it->key != key) {
        return -1;
    } else {
        return it - vec;
    }
}
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_LINEAR_MAP_H
