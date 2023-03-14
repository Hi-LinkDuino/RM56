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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_SPRING_CHAIN_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_SPRING_CHAIN_ITEM_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class SpringChainItem : public AceType {
    DECLARE_ACE_TYPE(SpringChainItem, AceType);

public:
    explicit SpringChainItem(int32_t index) : index_(index) {}
    ~SpringChainItem() override = default;

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

    double Value() const
    {
        return value_;
    }

    void SetVelocity(double velocity)
    {
        velocity_ = velocity;
    }

    double Velocity() const
    {
        return velocity_;
    }

private:
    int32_t index_ = 0;
    double value_ = 0.0;
    double velocity_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_SPRING_CHAIN_ITEM_H
