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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_TYPE_INFO_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_TYPE_INFO_BASE_H

#include <string>

#include "base/memory/memory_monitor_def.h"

// Generate 'TypeInfo' for each classes.
// And using hash code of its name for 'TypeId'.
#define DECLARE_CLASS_TYPE_INFO(classname)                                              \
public:                                                                                 \
    static const char* TypeName()                                                       \
    {                                                                                   \
        return #classname;                                                              \
    }                                                                                   \
    static TypeInfoBase::IdType TypeId()                                                \
    {                                                                                   \
        static TypeInfoBase::IdType myTypeId = std::hash<std::string> {}(TypeName());   \
        return myTypeId;                                                                \
    }                                                                                   \
    DECLARE_CLASS_TYPE_SIZE(classname)

// Integrate it into class declaration to support 'DynamicCast'.
#define DECLARE_RELATIONSHIP_OF_CLASSES(classname, ...) DECLARE_CLASS_TYPE_INFO(classname)            \
protected:                                                                                            \
    template<class __T, class __O, class... __V>                                                      \
    uintptr_t TrySafeCastById(TypeInfoBase::IdType id) const                                          \
    {                                                                                                 \
        VERIFY_DECLARED_CLASS(__T);                                                                   \
        uintptr_t ptr = __T::SafeCastById(id);                                                        \
        return ptr != 0 ? ptr : TrySafeCastById<__O, __V...>(id);                                     \
    }                                                                                                 \
    template<class __T>                                                                               \
    uintptr_t TrySafeCastById(TypeInfoBase::IdType id) const                                          \
    {                                                                                                 \
        VERIFY_DECLARED_CLASS(__T);                                                                   \
        return __T::SafeCastById(id);                                                                 \
    }                                                                                                 \
    uintptr_t SafeCastById(TypeInfoBase::IdType id) const override                                    \
    {                                                                                                 \
        return id == TypeId() ? reinterpret_cast<uintptr_t>(this) : TrySafeCastById<__VA_ARGS__>(id); \
    }                                                                                                 \
    TypeInfoBase::IdType GetTypeId() const override                                                   \
    {                                                                                                 \
        return TypeId();                                                                              \
    }                                                                                                 \
    const char* GetTypeName() const override                                                          \
    {                                                                                                 \
        return TypeName();                                                                            \
    }                                                                                                 \
    DECLARE_CLASS_GET_TYPE_SIZE(classname)

namespace OHOS::Ace {

// Define the base class, inherit this class to support partial 'RTTI' feature.
class TypeInfoBase {
public:
    friend class TypeInfoHelper;
    virtual ~TypeInfoBase() = default;

    using IdType = std::size_t;
    DECLARE_CLASS_TYPE_INFO(TypeInfoBase);

protected:
    virtual uintptr_t SafeCastById(IdType id) const
    {
        return id == TypeId() ? reinterpret_cast<uintptr_t>(this) : 0;
    }

    virtual IdType GetTypeId() const = 0;
    virtual const char* GetTypeName() const = 0;
#ifdef ACE_MEMORY_MONITOR
    virtual size_t GetTypeSize() const = 0;
#endif // ACE_MEMORY_MONITOR
};

class TypeInfoHelper final {
public:
    TypeInfoHelper() = delete;
    ~TypeInfoHelper() = delete;

    // Convert pointer of instance inherited from 'TypeInfoBase'.
    // Notice: Result SHOULD BE checked with 'nullptr', because 'DynamicCast' might be failed.
    template<class T>
    static T* DynamicCast(TypeInfoBase* rawPtr)
    {
        VERIFY_DECLARED_CLASS(T);
        return rawPtr != nullptr ? reinterpret_cast<T*>(rawPtr->SafeCastById(T::TypeId())) : nullptr;
    }
    template<class T>
    static const T* DynamicCast(const TypeInfoBase* rawPtr)
    {
        VERIFY_DECLARED_CLASS(T);
        return rawPtr != nullptr ? reinterpret_cast<const T*>(rawPtr->SafeCastById(T::TypeId())) : nullptr;
    }

    // Get type info by instance.
    static TypeInfoBase::IdType TypeId(const TypeInfoBase* rawPtr)
    {
        return rawPtr != nullptr ? rawPtr->GetTypeId() : 0;
    }
    static TypeInfoBase::IdType TypeId(const TypeInfoBase& instance)
    {
        return TypeId(&instance);
    }
    static const char* TypeName(const TypeInfoBase* rawPtr)
    {
        return rawPtr != nullptr ? rawPtr->GetTypeName() : nullptr;
    }
    static const char* TypeName(const TypeInfoBase& instance)
    {
        return TypeName(&instance);
    }
#ifdef ACE_MEMORY_MONITOR
    static size_t TypeSize(const TypeInfoBase* rawPtr)
    {
        return rawPtr != nullptr ? rawPtr->GetTypeSize() : 0;
    }
    static size_t TypeSize(const TypeInfoBase& instance)
    {
        return TypeSize(&instance);
    }
#endif

    // Get type info by type itself.
    template<class T>
    static TypeInfoBase::IdType TypeId()
    {
        return T::TypeId();
    }
    template<class T>
    static const char* TypeName()
    {
        return T::TypeName();
    }

    // Check whether instance is the specified type
    template<class T>
    static bool InstanceOf(const TypeInfoBase* rawPtr)
    {
        return DynamicCast<T>(rawPtr) != nullptr;
    }
    template<class T>
    static bool InstanceOf(const TypeInfoBase& instance)
    {
        return DynamicCast<T>(&instance) != nullptr;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_TYPE_INFO_BASE_H
