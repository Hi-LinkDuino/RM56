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
#ifndef HISTREAMER_PLUGIN_TYPE_INFO_EXT_H
#define HISTREAMER_PLUGIN_TYPE_INFO_EXT_H

#include <cstring>
#include <memory>
#include <typeinfo>

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * This function is used to compare two type_info. Besides the basic compare using operator == of type_info,
 * we also consider types with the same names are the same types.
 * @param t1 type
 * @param t2 type
 * @return true if t1 and t2 are the same type. otherwise, false.
 */
inline bool IsSameType(const std::type_info& t1, const std::type_info& t2) noexcept
{
    if (t1 == t2) {
        return true;
    }
    auto t1Length = strlen(t1.name());
    if (t1Length == strlen(t2.name()) && strncmp(t1.name(), t2.name(), t1Length) == 0) {
        return true;
    }
    return false;
}

/**
 * This function is used to reinterpret cast one shared_ptr into another shared_ptr.
 * @tparam T type
 * @tparam U type
 * @param ptr pointer
 * @return cast result
 */
template<typename T, typename U>
inline std::shared_ptr<T> ReinterpretPointerCast(const std::shared_ptr<U>& ptr) noexcept
{
    return std::shared_ptr<T>(ptr, reinterpret_cast<T*>(ptr.get()));
}
} // Plugin
} // Media
} // OHOS

#endif // HISTREAMER_PLUGIN_TYPE_INFO_EXT_H
