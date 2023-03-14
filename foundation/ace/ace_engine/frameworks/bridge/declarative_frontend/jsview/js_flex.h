/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_H

#include <list>

#include "base/log/ace_trace.h"
#include "core/components/common/layout/constants.h"
#include "core/components/flex/flex_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {

class JSFlex : public JSContainerBase {
public:
    static void SetFillParent();
    static void SetWrapContent();

    /**
     * JS JustifyContent function definition:
     * Main axis alignment format of the current row of the flex container. The options are as follows:
     * flexstart: The project is at the beginning of the container.
     * flexend: The project is at the end of the container.
     * center: The project is located in the center of the container.
     * spacebetween: The project is located in a container with a blank space between lines.
     * spacearound: The item is located in a container with blank space before, between, and after each line.
     */
    static void SetJustifyContent(int32_t value);

    /**
     * JS AlignItems function definition:
     * Alignment format of the cross axis of the current row of the flex container. The options are as follows:
     *	stretch: The elastic element is stretched to the same height or width as the container in the cross axis
     *  direction.
     * flexstart: The element is aligned to the start point of the cross axis.
     * flexend: The element is aligned to the end of the cross axis.
     * center: The element is centered on the cross axis.
     */
    static void SetAlignItems(int32_t value);

    static void SetAlignContent(int32_t value);

    static void SetWrap(bool value);

    static void JsHeight(const JSCallbackInfo& info);

    static void JsWidth(const JSCallbackInfo& info);

    static void JsSize(const JSCallbackInfo& info);

private:
    static void SetWidth(const JSRef<JSVal>& jsValue);

    static void SetHeight(const JSRef<JSVal>& jsValue);
}; // JSFlex class

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_FLEX_H
