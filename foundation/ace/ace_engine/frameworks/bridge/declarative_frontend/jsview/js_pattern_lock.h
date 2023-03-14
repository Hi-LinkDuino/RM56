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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_PATTERN_LOCK_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_PATTERN_LOCK_H

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "core/components_v2/pattern_lock/pattern_lock_component.h"
#include "core/components_v2/pattern_lock/pattern_lock_theme.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {
class JSPatternLock : public JSViewAbstract {
public:
    static void Create(const JSCallbackInfo& info);
    static void JSBind(BindingTarget globalObj);
    static void SetSelectedColor(const JSCallbackInfo& info);
    static void SetActiveColor(const JSCallbackInfo& info);
    static void SetPathColor(const JSCallbackInfo& info);
    static void SetRegularColor(const JSCallbackInfo& info);
    static void SetCircleRadius(const JSCallbackInfo& info);
    static void SetSideLength(const JSCallbackInfo& info);
    static void SetPathStrokeWidth(const JSCallbackInfo& info);
    static void SetAutoReset(bool isAutoReset);

protected:
    static void SetPatternComplete(const JSCallbackInfo& info);

private:
    static void SetDefaultTheme(OHOS::Ace::RefPtr<OHOS::Ace::V2::PatternLockComponent>& patternLock);
    static JSRef<JSArray> ChoosePointToJSValue(std::vector<int> input);
};

class JSPatternLockController final : public Referenced {
public:
    JSPatternLockController() = default;
    ~JSPatternLockController() override = default;
    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSPatternLockController* controller);
    void Reset(const JSCallbackInfo& args);
    void SetController(const RefPtr<V2::PatternLockController>& controller)
    {
        controller_ = controller;
    }

private:
    RefPtr<V2::PatternLockController> controller_;
    ACE_DISALLOW_COPY_AND_MOVE(JSPatternLockController);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_PATTERN_LOCK_H