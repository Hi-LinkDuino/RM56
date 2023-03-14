/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_STATE_ATTRUBUTES_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_STATE_ATTRUBUTES_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

constexpr int VISUAL_STATE_COUNT = 5; // Count of valid enums below (without UNDEFINED)

enum class VisualState {
    NORMAL = 0,
    FOCUSED,
    PRESSED,
    DISABLED,
    HOVER,
    NOTSET,
};

template<class AttributeID>
class StateAttributeBase : public virtual AceType {
    DECLARE_ACE_TYPE(StateAttributeBase<AttributeID>, AceType);
public:
    explicit StateAttributeBase(AttributeID id) : id_(id) {}
    virtual ~StateAttributeBase() {}
    AttributeID id_;
};

template<class AttributeID, class Attribute>
class StateAttributeValue : public StateAttributeBase<AttributeID> {
    DECLARE_ACE_TYPE(StateAttributeValue, StateAttributeBase<AttributeID>);
public:
    StateAttributeValue(AttributeID id, Attribute value)
        : StateAttributeBase<AttributeID>(id), value_(value)
    {}
    virtual ~StateAttributeValue() {}
    Attribute value_;
};

template<class AttributeID>
class StateAttributes : public Referenced {
public:
    bool HasAttribute(AttributeID attribute, VisualState state)
    {
        int stateIdx = static_cast<int>(state);
        for (auto attrItem : stateAttrs_[stateIdx]) {
            if (attrItem->id_ == attribute) {
                return true;
            }
        }
        return false;
    }

    template<class AttributeType>
    void AddAttribute(AttributeID attribute, AttributeType value, VisualState state)
    {
        int stateIdx = static_cast<int>(state);
        stateAttrs_[stateIdx].push_back(
            MakeRefPtr<StateAttributeValue<AttributeID, AttributeType>>(attribute, value));
    }

    const std::vector<RefPtr<StateAttributeBase<AttributeID>>>& GetAttributesForState(VisualState state)
    {
        int stateIdx = static_cast<int>(state);
        return stateAttrs_[stateIdx];
    }

private:
    // stateAttrs_ is an array with every entry pointing to
    // vector of Attributes for one of the states.
    // array[0] -> vector of attributes for state "normal"
    // array[1] -> vector of attributes for state "focuse"
    // ...
    std::array<std::vector<RefPtr<StateAttributeBase<AttributeID>>>, VISUAL_STATE_COUNT> stateAttrs_;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COMMON_STATE_ATTRUBUTES_H
