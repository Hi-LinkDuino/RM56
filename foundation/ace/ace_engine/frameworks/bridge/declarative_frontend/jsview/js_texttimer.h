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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTTIMER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTTIMER_H

#include "bridge/declarative_frontend/jsview/js_container_base.h"
#include "core/components/texttimer/texttimer_controller.h"

namespace OHOS::Ace::Framework {
class JSTextTimer : public JSViewAbstract {
public:
    static void JSBind(BindingTarget globalObj);
    static void Create(const JSCallbackInfo& info);

protected:
    static void SetFormat(const JSCallbackInfo& info);
    static void SetTextColor(const JSCallbackInfo& info);
    static void SetFontSize(const JSCallbackInfo& info);
    static void SetFontWeight(const std::string& value);
    static void SetFontStyle(int32_t value);
    static void SetFontFamily(const JSCallbackInfo& info);
    static void OnTimer(const JSCallbackInfo& info);
};

class JSTextTimerController final : public Referenced {
public:
    JSTextTimerController() = default;
    ~JSTextTimerController() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& info);
    static void Destructor(JSTextTimerController* scroller);

    void Start(const JSCallbackInfo& info);
    void Pause(const JSCallbackInfo& info);
    void Reset(const JSCallbackInfo& info);

    void SetController(const RefPtr<TextTimerController>& controller)
    {
        controller_ = controller;
    }

    RefPtr<TextTimerController> GetController() const
    {
        return controller_;
    }

private:
    RefPtr<TextTimerController> controller_;

    ACE_DISALLOW_COPY_AND_MOVE(JSTextTimerController);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TEXTTIMER_H
