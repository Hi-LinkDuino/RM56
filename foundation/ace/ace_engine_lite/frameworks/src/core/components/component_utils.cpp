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

#include "component_utils.h"
#include "ace_log.h"
#include "component.h"
#include "descriptor_utils.h"

namespace OHOS {
namespace ACELite {
UIView* ComponentUtils::GetViewFromBindingObject(jerry_value_t value)
{
    Component* component = GetComponentFromBindingObject(value);
    return component ? component->GetComponentRootView() : nullptr;
}

Component* ComponentUtils::GetComponentFromBindingObject(jerry_value_t value)
{
    if (IS_UNDEFINED(value)) {
        return nullptr;
    }

    Component* component = nullptr;
    bool isBinded = jerry_get_object_native_pointer(value, reinterpret_cast<void**>(&component), nullptr);
    if (!isBinded || (component == nullptr)) {
        return nullptr;
    }
    return component;
}

void ComponentUtils::ReleaseComponents(Component *rootComponent)
{
    if (rootComponent == nullptr) {
        return;
    }
    // for list component, release all the list items first
    if (rootComponent->GetComponentName() != K_LIST) {
        JSValue descriptors = rootComponent->GetDescriptors();
        if (!JSUndefined::Is(descriptors)) {
            DescriptorUtils::ReleaseDescriptorOrElements(descriptors);
        }
    }
    Component *child = const_cast<Component *>(rootComponent->GetChildHead());
    if (child == nullptr) {
        // has no children, can be released directly
        rootComponent->Release();
        delete rootComponent;
        return;
    }
    while (child != nullptr) {
        ReleaseComponents(child);
        child = const_cast<Component *>(child->GetNextSibling());
    }
}

uint16_t ComponentUtils::GetComponentType(jerry_value_t options)
{
    if (IS_UNDEFINED(options)) {
        return 0;
    }
    jerry_value_t attrsPropValue = jerryx_get_property_str(options, ATTR_ATTRS);
    // create component according to attribute type
    const char * const attrType = "type";
    jerry_value_t typeValHandler = jerryx_get_property_str(attrsPropValue, attrType);
    uint16_t typeId = 0;
    if (!jerry_value_is_undefined(typeValHandler)) {
        uint16_t len = 0;
        char *type = MallocStringOf(typeValHandler, &len);
        typeId = KeyParser::ParseKeyId(type, len);
        ACE_FREE(type);
    }
    ReleaseJerryValue(typeValHandler, attrsPropValue, VA_ARG_END_FLAG);
    return typeId;
}
} // namespace ACELite
} // namespace OHOS
