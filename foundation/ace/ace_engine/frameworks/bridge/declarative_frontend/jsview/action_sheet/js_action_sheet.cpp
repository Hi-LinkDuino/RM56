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

#include "frameworks/bridge/declarative_frontend/jsview/action_sheet/js_action_sheet.h"

#include <string>
#include <vector>

#include "core/common/container.h"
#include "core/components/dialog/dialog_component.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {
const DimensionOffset ACTION_SHEET_OFFSET_DEFAULT = DimensionOffset(0.0_vp, -40.0_vp);
const std::vector<DialogAlignment> DIALOG_ALIGNMENT = {
    DialogAlignment::TOP, DialogAlignment::CENTER, DialogAlignment::BOTTOM, DialogAlignment::DEFAULT,
    DialogAlignment::TOP_START, DialogAlignment::TOP_END, DialogAlignment::CENTER_START,
    DialogAlignment::CENTER_END, DialogAlignment::BOTTOM_START, DialogAlignment::BOTTOM_END
};
} // namespace

ActionSheetInfo ParseSheetInfo(const JSCallbackInfo& args, JSRef<JSVal> val)
{
    ActionSheetInfo sheetInfo;
    if (!val->IsObject()) {
        LOGW("param is not an object.");
        return sheetInfo;
    }

    auto obj = JSRef<JSObject>::Cast(val);
    auto titleVal = obj->GetProperty("title");
    std::string title;
    if (JSActionSheet::ParseJsString(titleVal, title)) {
        sheetInfo.title = title;
    }

    auto iconVal = obj->GetProperty("icon");
    std::string icon;
    if (JSActionSheet::ParseJsString(iconVal, icon)) {
        sheetInfo.icon = icon;
    }

    auto actionValue = obj->GetProperty("action");
    if (actionValue->IsFunction()) {
        auto actionFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(actionValue));
        RefPtr<Gesture> tapGesture = AceType::MakeRefPtr<TapGesture>();
        tapGesture->SetOnActionId(
            [execCtx = args.GetExecutionContext(), func = std::move(actionFunc)](const GestureEvent& info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("SheetInfo.action");
                func->Execute();
                // Close dialog when click sheet.
                auto container = Container::Current();
                if (!container) {
                    return;
                }
                auto context = container->GetPipelineContext();
                if (!context) {
                    return;
                }
                auto stack = context->GetLastStack();
                if (!stack) {
                    return;
                }
                stack->PopDialog();
            });
        sheetInfo.gesture = tapGesture;
    }
    return sheetInfo;
}

void JSActionSheet::Show(const JSCallbackInfo& args)
{
    LOGD("show ActionSheet");
    if (!args[0]->IsObject()) {
        LOGE("args is not an object, can't show ActionSheet.");
        return;
    }

    DialogProperties properties {
        .type = DialogType::ACTION_SHEET, .alignment = DialogAlignment::BOTTOM, .offset = ACTION_SHEET_OFFSET_DEFAULT
    };
    auto obj = JSRef<JSObject>::Cast(args[0]);
    // Parse title.
    auto titleValue = obj->GetProperty("title");
    std::string title;
    if (ParseJsString(titleValue, title)) {
        properties.title = title;
    }

    // Parses message.
    auto messageValue = obj->GetProperty("message");
    std::string message;
    if (ParseJsString(messageValue, message)) {
        properties.content = message;
    }

    // Parse auto autoCancel.
    auto autoCancelValue = obj->GetProperty("autoCancel");
    if (autoCancelValue->IsBoolean()) {
        properties.autoCancel = autoCancelValue->ToBoolean();
    }

    // Parse cancel.
    auto cancelValue = obj->GetProperty("cancel");
    if (cancelValue->IsFunction()) {
        auto cancelFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(cancelValue));
        EventMarker cancelId([execCtx = args.GetExecutionContext(), func = std::move(cancelFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("ActionSheet.cancel");
            func->Execute();
        });
        properties.callbacks.try_emplace("cancel", cancelId);
    }

    // Parse confirm.
    auto confirmVal = obj->GetProperty("confirm");
    if (confirmVal->IsObject()) {
        JSRef<JSObject> confirmObj = JSRef<JSObject>::Cast(confirmVal);
        JSRef<JSVal> value = confirmObj->GetProperty("value");
        std::string buttonValue;
        if (ParseJsString(value, buttonValue)) {
            ButtonInfo buttonInfo = {
                .text = buttonValue,
                .textColor = ""
            };
            properties.buttons.emplace_back(buttonInfo);
            JSRef<JSVal> actionValue = confirmObj->GetProperty("action");
            if (actionValue->IsFunction()) {
                auto actionFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(actionValue));
                EventMarker actionId([execCtx = args.GetExecutionContext(), func = std::move(actionFunc)]() {
                    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                    ACE_SCORING_EVENT("ActionSheet.confirm.action");
                    func->Execute();
                });
                properties.primaryId = actionId;
            }
        }
    }

    // Parse sheets
    auto sheetsVal = obj->GetProperty("sheets");
    if (sheetsVal->IsArray()) {
        std::vector<ActionSheetInfo> sheetsInfo;
        auto sheetsArr = JSRef<JSArray>::Cast(sheetsVal);
        for (size_t index = 0; index < sheetsArr->Length(); ++index) {
            sheetsInfo.emplace_back(ParseSheetInfo(args, sheetsArr->GetValueAt(index)));
        }
        properties.sheetsInfo = std::move(sheetsInfo);
    }

    // Parse alignment
    auto alignmentValue = obj->GetProperty("alignment");
    if (alignmentValue->IsNumber()) {
        auto alignment = alignmentValue->ToNumber<int32_t>();
        if (alignment >= 0 && alignment <= static_cast<int32_t>(DIALOG_ALIGNMENT.size())) {
            properties.alignment = DIALOG_ALIGNMENT[alignment];
        }
    }

    // Parse offset
    auto offsetValue = obj->GetProperty("offset");
    if (offsetValue->IsObject()) {
        auto offsetObj = JSRef<JSObject>::Cast(offsetValue);
        Dimension dx;
        auto dxValue = offsetObj->GetProperty("dx");
        ParseJsDimensionVp(dxValue, dx);
        Dimension dy;
        auto dyValue = offsetObj->GetProperty("dy");
        ParseJsDimensionVp(dyValue, dy);
        properties.offset = DimensionOffset(dx, dy);
    }

    // Parses gridCount.
    auto gridCountValue = obj->GetProperty("gridCount");
    if (gridCountValue->IsNumber()) {
        properties.gridCount = gridCountValue->ToNumber<int32_t>();
    }

    // Show ActionSheet.
    auto container = Container::Current();
    if (container) {
        auto context = container->GetPipelineContext();
        auto executor = container->GetTaskExecutor();
        if (executor) {
            executor->PostTask(
                [context, properties]() {
                    if (context) {
                        context->ShowDialog(properties, false, "ActionSheet");
                    }
                },
                TaskExecutor::TaskType::UI);
        }
    }
    args.SetReturnValue(args.This());
}

void JSActionSheet::JSBind(BindingTarget globalObj)
{
    JSClass<JSActionSheet>::Declare("ActionSheet");
    JSClass<JSActionSheet>::StaticMethod("show", &JSActionSheet::Show);
    JSClass<JSActionSheet>::Inherit<JSViewAbstract>();
    JSClass<JSActionSheet>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
