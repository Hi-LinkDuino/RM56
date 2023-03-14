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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_ACE_TYPE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_ACE_TYPE_H

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/memory/type_info_base.h"

/**
 * Example:
 *     class BaseA : public virtual AceType {
 *         DECLARE_ACE_TYPE(BaseA, AceType);
 *     };
 *
 *     class BaseB : public virtual AceType {
 *         DECLARE_ACE_TYPE(BaseB, AceType);
 *     };
 *
 *     class BaseC : public BaseA, public BaseB {
 *         DECLARE_ACE_TYPE(BaseC, BaseA, BaseB);
 *     };
 */
// Integrate it into class declaration to support 'DynamicCast'.
#define DECLARE_ACE_TYPE(...) DECLARE_RELATIONSHIP_OF_CLASSES(__VA_ARGS__)

namespace OHOS::Ace {

// Inherit 'AceType' to manager pointers using 'RefPtr', 'WeakPtr' and 'AceType::DynamicCast'.
class AceType : public virtual TypeInfoBase, public virtual Referenced {
    DECLARE_ACE_TYPE(AceType, TypeInfoBase);

public:
    ~AceType() override = default;

    template<class T>
    static T* DynamicCast(AceType* rawPtr)
    {
        return TypeInfoHelper::DynamicCast<T>(rawPtr);
    }
    template<class T>
    static const T* DynamicCast(const AceType* rawPtr)
    {
        return TypeInfoHelper::DynamicCast<T>(rawPtr);
    }
    template<class T, class O>
    static RefPtr<T> DynamicCast(const RefPtr<O>& ptr)
    {
        return Claim(DynamicCast<T>(RawPtr(ptr)));
    }
    template<class T, class O>
    static WeakPtr<T> DynamicCast(const WeakPtr<O>& weak)
    {
        return WeakClaim(DynamicCast<T>(RawPtr(weak.Upgrade())));
    }

    // Get type info by instance.
    static AceType::IdType TypeId(const AceType* rawPtr)
    {
        return TypeInfoHelper::TypeId(rawPtr);
    }
    template<class T>
    static AceType::IdType TypeId(const RefPtr<T>& ptr)
    {
        return TypeId(AceType::RawPtr(ptr));
    }
    static AceType::IdType TypeId(const AceType& instance)
    {
        return TypeInfoHelper::TypeId(instance);
    }
    static const char* TypeName(const AceType* rawPtr)
    {
        return TypeInfoHelper::TypeName(rawPtr);
    }
    template<class T>
    static const char* TypeName(const RefPtr<T>& ptr)
    {
        return TypeName(AceType::RawPtr(ptr));
    }
    static const char* TypeName(const AceType& instance)
    {
        return TypeInfoHelper::TypeName(instance);
    }

    // Get type info by type itself.
    template<class T>
    static AceType::IdType TypeId()
    {
        return TypeInfoHelper::TypeId<T>();
    }
    template<class T>
    static const char* TypeName()
    {
        return TypeInfoHelper::TypeName<T>();
    }

    // Check whether instance is the specified type
    template<class T>
    static bool InstanceOf(const AceType* rawPtr)
    {
        return TypeInfoHelper::InstanceOf<T>(rawPtr);
    }
    template<class T>
    static bool InstanceOf(const AceType& instance)
    {
        return TypeInfoHelper::InstanceOf<T>(&instance);
    }
    template<class T, class O>
    static bool InstanceOf(const RefPtr<O>& ptr)
    {
        return TypeInfoHelper::InstanceOf<T>(RawPtr(ptr));
    }

protected:
    AceType() = default;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_ACE_TYPE_H
