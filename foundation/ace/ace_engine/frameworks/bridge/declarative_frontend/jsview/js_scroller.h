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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "core/components/scroll/scroll_controller_interface.h"
#include "core/components/scroll_bar/scroll_bar_proxy.h"

namespace OHOS::Ace::Framework {

class JSScroller final : public Referenced {
public:
    JSScroller() = default;
    ~JSScroller() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSScroller* scroller);

    void ScrollTo(const JSCallbackInfo& args);
    void ScrollEdge(const JSCallbackInfo& args);
    void ScrollPage(const JSCallbackInfo& args);
    void CurrentOffset(const JSCallbackInfo& args);
    void ScrollToIndex(const JSCallbackInfo& args);

    const RefPtr<ScrollController>& GetController() const
    {
        return controller_;
    }

    void SetController(const RefPtr<ScrollController>& controller)
    {
        controller_ = controller;
    }

    void SetScrollBarProxy(const RefPtr<ScrollBarProxy>& proxy)
    {
        scrollBarProxy_ = proxy;
    }

    const RefPtr<ScrollBarProxy>& GetScrollBarProxy()
    {
        return scrollBarProxy_;
    }

private:
    RefPtr<ScrollController> controller_;
    RefPtr<ScrollBarProxy> scrollBarProxy_;

    ACE_DISALLOW_COPY_AND_MOVE(JSScroller);
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_SCROLLER_H
