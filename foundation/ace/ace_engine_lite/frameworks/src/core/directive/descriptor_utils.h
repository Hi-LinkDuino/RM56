/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_DESCRIPTOR_UTILS_H
#define OHOS_ACELITE_DESCRIPTOR_UTILS_H

#include "component.h"
#include "non_copyable.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
class DescriptorUtils final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DescriptorUtils);
    ~DescriptorUtils() {}

    /**
     * @brief Create a descriptor object of if directive, which like { if: true, getter, render }.
     * @param getter The getter function.
     * @param render The render function.
     * @return The descriptor object of if directive.
     */
    static JSValue CreateIfDescriptor(JSValue getter, JSValue render);

    /**
     * @brief Create a descriptor object of for directive, which like { for: true, getter, render }.
     * @param Getter the getter function.
     * @param Render the render function.
     * @return The descriptor object of for directive.
     */
    static JSValue CreateForDescriptor(JSValue getter, JSValue render);

    /**
     * @brief Create a descriptor watcher.
     * @param element The parent element of descriptor object.
     * @param descriptor The descriptor object.
     * @return The descriptor watcher instance.
     */
    static JSValue CreateDescriptorWatcher(JSValue element, JSValue descriptor);

    /**
     * @brief Check whether the descriptor object is a if directive descriptor or not?
     * @param descriptor The descriptor object to be checked.
     * @return The check result.
     */
    static bool IsIfDescriptor(JSValue descriptor);

    /**
     * @brief Check whether the descriptor object is a for directive descriptor or not?
     * @param descriptor The descriptor object to be checked.
     * @return The check result.
     */
    static bool IsForDescriptor(JSValue descriptor);

    /**
     * @brief Check whether the if directive descriptor object can be shown or not?
     * @param descriptor The if directive descriptor object to be checked.
     * @return The check result.
     */
    static bool IsIfDescriptorShown(JSValue descriptor);

    /**
     * @brief Rendering the if directive descriptor object.
     * The rendered result will be set to the rendered property of descriptor object,
     * like { if: true, rendered: {}, getter, render }.
     * @param descriptor The if directive descriptor object to be rendered.
     * @return  The rendered result which will be an element.
     */
    static JSValue RenderIfDescriptor(JSValue descriptor);

    /**
     * @brief Rendering the for directive descriptor object.
     * The rendered result will be set to the rendered property of descriptor object,
     * like { for: true, rendered: [...], getter, render }.
     * @param descriptor The for directive descriptor object to be rendered.
     * @return  The rendered result which will be an element array.
     */
    static JSValue RenderForDescriptor(JSValue descriptor);

    /**
     * @brief Get the rendered result of directive descriptor.
     * @param descriptor The directive descriptor.
     * @return The rendered result.
     */
    static JSValue GetDescriptorRendered(JSValue descriptor);

    /**
     * @brief Delete the rendered result of if directive descriptor object.
     * @param descriptor The if directive descriptor.
     */
    static void DelIfDescriptorRendered(JSValue descriptor);

    /**
     * @brief Delete the rendered result of for directive descriptor object.
     * @param descriptor The for directive descriptor.
     */
    static void DelForDescriptorRendered(JSValue descriptor);

    /**
     * @brief Release a collection of descriptor object or element.
     * @param descriptorOrElements The collection of descriptor object or element to be released.
     */
    static void ReleaseDescriptorOrElements(JSValue descriptorOrElements);

    /**
     * @brief Release a descriptor object or element.
     * @param descriptorOrElement The descriptor object of element to be released.
     */
    static void ReleaseDescriptorOrElement(JSValue descriptorOrElement);

    /**
     * @brief Release a for directive descriptor object.
     * @param descriptor The for directive descriptor object to be released.
     */
    static void ReleaseForDescriptor(JSValue descriptor);

    /**
     * @brief Release a if descriptor object.
     * @param descriptor The if directive descriptor object to be released.
     */
    static void ReleaseIfDescriptor(JSValue descriptor);

    /**
     * @brief Release an element.
     * @param element The element to be released.
     */
    static void ReleaseElement(JSValue element);

    /**
     * @brief rendered property name
     */
    static const char * const DESCRIPTOR_ATTR_RENDERED;

    /**
     * @brief getter property name
     */
    static const char * const DESCRIPTOR_ATTR_GETTER;

    /**
     * @brief render property name
     */
    static const char * const DESCRIPTOR_ATTR_RENDER;

    /**
     * @brief element property name
     */
    static const char * const WATCHER_OPTION_ELEMENT;

    /**
     * @brief options property name
     */
    static const char * const WATCHER_OPTION_DESCRIPTOR;

private:
    /**
     * @brief Create a directive descriptor object.
     * @param type The type of directive descriptor object, which is if or for?
     * @param getter The getter function.
     * @param render The render function.
     * @return The directive descriptor object.
     */
    static JSValue CreateDescriptor(const char * const type, JSValue getter, JSValue render);

    /**
     * @brief Check the type of a directive descriptor object.
     * @param type The type of directive descriptor object.
     * @param descriptor The directive descriptor object to be checked.
     * @return The check result.
     */
    static bool IsDescriptor(const char * const type, JSValue descriptor);

    /**
     * @brief if property name
     */
    static const char * const DESCRIPTOR_ATTR_IF;

    /**
     * @brief for property name
     */
    static const char * const DESCRIPTOR_ATTR_FOR;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_DESCRIPTOR_UTILS_H
