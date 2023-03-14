/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_CAMERA_ABILITY_IMPL_H
#define OHOS_CAMERA_ABILITY_IMPL_H

#include "camera_ability.h"

namespace OHOS {
namespace Media {
class CameraAbilityImpl : public CameraAbility {
public:
    CameraAbilityImpl() {}
    ~CameraAbilityImpl() = default;
    std::list<int32_t> GetSupportedFormats() const;
    std::list<CameraPicSize> GetSupportedSizes(int format) const;

    template<typename T> int32_t SetParameterRange(uint32_t key, std::list<T> rangeList);
    template<typename T> std::list<T> GetParameterRange(uint32_t key) const;

private:
    void SetSupportParameterRange(uint32_t key, std::list<CameraPicSize> &rangeList);
    std::list<CameraPicSize> GetSupportParameterRange(uint32_t key) const;
    bool IsParameterSupport(uint32_t key) const;
    std::map<uint32_t, std::list<CameraPicSize>> SizeMap_;
    std::set<uint32_t> supportProperties_;
};
} // namespace Media
} // namespace OHOS

#endif // OHOS_CAMERA_ABILITY_IMPL_H
