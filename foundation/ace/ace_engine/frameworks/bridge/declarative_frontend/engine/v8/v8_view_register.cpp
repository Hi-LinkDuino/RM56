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

#include "base/i18n/localization.h"
#include "base/log/log.h"
#include "bridge/declarative_frontend/interfaces/profiler/js_profiler.h"
#include "bridge/declarative_frontend/jsview/js_canvas_image_data.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "frameworks/bridge/declarative_frontend/engine/js_object_template.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/frontend_delegate_declarative.h"
#include "frameworks/bridge/declarative_frontend/jsview/action_sheet/js_action_sheet.h"
#include "frameworks/bridge/declarative_frontend/jsview/dialog/js_alert_dialog.h"
#include "frameworks/bridge/declarative_frontend/jsview/dialog/js_custom_dialog_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_ability_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_ability_component_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_animator.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_badge.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_blank.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_button.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_calendar.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_calendar_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas_gradient.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_canvas_path.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_checkbox.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_checkboxgroup.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_clipboard.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_hyperlink.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_offscreen_rendering_context.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_path2d.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_render_image.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_rendering_context_settings.h"
#ifndef WEARABLE_PRODUCT
#include "frameworks/bridge/declarative_frontend/jsview/js_camera.h"
#endif
#include "frameworks/bridge/declarative_frontend/jsview/js_circle.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_column.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_column_split.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_counter.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_data_panel.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_datepicker.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_divider.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_ellipse.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_environment.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_flex_impl.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_foreach.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#ifndef WEARABLE_PRODUCT
#include "frameworks/bridge/declarative_frontend/jsview/js_form.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_richtext.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_web.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_web_controller.h"
#endif
#endif
#include "frameworks/bridge/declarative_frontend/jsview/js_gauge.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_gesture.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_grid.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_grid_container.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_grid_item.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_if_else.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_image.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_image_animator.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_indexer.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_lazy_foreach.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_line.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_list.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_list_item.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_loading_progress.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_marquee.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_navigation.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_navigator.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_pan_handler.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_path.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_pattern_lock.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_persistent.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_polygon.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_polyline.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_progress.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_slider.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_textpicker.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_toggle.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "frameworks/bridge/declarative_frontend/jsview/js_qrcode.h"
#endif
#include "frameworks/bridge/declarative_frontend/jsview/js_page_transition.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_radio.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_rect.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_refresh.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_row.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_row_split.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_scroll.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_scroller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_search.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_select.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_shape.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_shape_abstract.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_sheet.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_side_bar.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_sliding_panel.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_span.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_stack.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_stepper.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_stepper_item.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_swiper.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_tab_content.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_tabs.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_tabs_controller.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_text.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_text_clock.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_textarea.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_textinput.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_texttimer.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_touch_handler.h"
#ifndef WEARABLE_PRODUCT
#include "frameworks/bridge/declarative_frontend/jsview/js_piece.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_rating.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_video.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_video_controller.h"
#endif
#include "frameworks/bridge/declarative_frontend/jsview/js_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_context.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_stack_processor.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent_controller.h"
#endif
#include "frameworks/bridge/declarative_frontend/jsview/menu/js_context_menu.h"
#include "frameworks/bridge/declarative_frontend/jsview/scroll_bar/js_scroll_bar.h"
#include "frameworks/bridge/declarative_frontend/sharedata/js_share_data.h"

namespace OHOS::Ace::Framework {

void JsLoadDocument(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("Load Document");
    if (args.Length() != 1) {
        LOGE("The arg is wrong, it is supposed to have 1 arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("loadDocument, context is empty!");
        return;
    }

    v8::Local<v8::Object> obj = args[0]->ToObject(context).ToLocalChecked();
    JSView* view = static_cast<JSView*>(obj->GetAlignedPointerFromInternalField(0));

    auto page = V8DeclarativeEngineInstance::GetStagingPage(isolate);
    V8DeclarativeEngineInstance::PersistRootViewHandle(isolate, obj);

    LOGD("Load Document setting root view");
    auto rootComponent = view->CreateComponent();
    std::list<RefPtr<Component>> stackChildren;
    stackChildren.emplace_back(rootComponent);
    auto rootStackComponent = AceType::MakeRefPtr<StackComponent>(
        Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, stackChildren);
    rootStackComponent->SetMainStackSize(MainStackSize::MAX);
    auto rootComposed = AceType::MakeRefPtr<ComposedComponent>("0", "root");
    rootComposed->SetChild(rootStackComponent);
    page->SetRootComponent(rootComposed);

    page->SetPageTransition(view->BuildPageTransitionComponent());
    // We are done, tell to the JSAgePage
    page->SetPageCreated();
    page->SetDeclarativeOnPageAppearCallback([view]() { view->FireOnShow(); });
    page->SetDeclarativeOnPageDisAppearCallback([view]() { view->FireOnHide(); });
    page->SetDeclarativeOnBackPressCallback([view]() { return view->FireOnBackPress(); });
    page->SetDeclarativeOnPageRefreshCallback([view]() { view->MarkNeedUpdate(); });
}

void JsDumpMemoryStats(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("dumpMemoryStats: Not Implemented for V8. UnSupported");
}

void JsGetI18nResource(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1 && args.Length() != 2) {
        LOGE("The arg is wrong, must have one or two arguments");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsGetI18nResource, context is empty!");
        return;
    }

    v8::String::Utf8Value targetStringValue(isolate, args[0]);
    if (!(*targetStringValue)) {
        return;
    }

    std::string targetString(*targetStringValue);
    std::vector<std::string> splitStr;
    StringUtils::SplitStr(targetString, ".", splitStr);
    if (splitStr.size() != 2) {
        return;
    }

    auto targetStringKey = splitStr[0];
    auto targetStringKeyValue = splitStr[1];
    auto resultStrJson = V8DeclarativeEngineInstance::GetI18nStringResource(targetStringKey, targetStringKeyValue);
    auto resultStr = resultStrJson->GetString();
    if (args.Length() != 1) {
        if (args[1]->IsArray()) {
            v8::Local<v8::Array> arrayVal = v8::Local<v8::Array>::Cast(args[1]);
            uint32_t len = arrayVal->Length();
            std::vector<std::string> arrayResult;
            for (uint32_t i = 0; i < len; i++) {
                v8::Local<v8::Value> value = arrayVal->Get(context, i).ToLocalChecked();
                v8::String::Utf8Value valueStr(isolate, value);
                if (!(*targetStringValue)) {
                    arrayResult.emplace_back(std::string());
                    continue;
                }
                std::string valueString(*valueStr);
                arrayResult.emplace_back(valueString);
            }
            ReplacePlaceHolderArray(resultStr, arrayResult);
        } else if (args[1]->IsObject()) {
            v8::Local<v8::String> value = v8::JSON::Stringify(context, args[1]).ToLocalChecked();
            v8::String::Utf8Value argsJsStr(isolate, value);
            std::string valueString(*argsJsStr);
            if ((*argsJsStr)) {
                std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);
                ReplacePlaceHolder(resultStr, argsPtr);
            }
        } else if (args[1]->IsNumber()) {
            v8::Local<v8::String> value = v8::JSON::Stringify(context, args[1]).ToLocalChecked();
            v8::String::Utf8Value argsJsStr(isolate, value);
            std::string valueString(*argsJsStr);
            auto count = StringToDouble(valueString);
            auto pluralChoice = Localization::GetInstance()->PluralRulesFormat(count);
            if (!pluralChoice.empty()) {
                resultStr = ParserPluralResource(resultStrJson, pluralChoice, valueString);
            }
        }
    }

    if (!resultStr.empty()) {
        v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, resultStr.c_str()).ToLocalChecked();
        args.GetReturnValue().Set(result);
    } else {
        args.GetReturnValue().SetNull();
    }
}

void JsGetMediaResource(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        LOGE("The arg is wrong, must have one argument");
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("JsGetMediaResource, context is empty!");
        return;
    }

    v8::String::Utf8Value targetMediaFileJsName(isolate, args[0]);
    if (!(*targetMediaFileJsName)) {
        return;
    }

    std::string targetMediaFileName(*targetMediaFileJsName);
    std::string filePath = V8DeclarativeEngineInstance::GetMediaResource(targetMediaFileName);
    if (!filePath.empty()) {
        v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked();
        args.GetReturnValue().Set(result);
    } else {
        args.GetReturnValue().SetNull();
    }
}

void JsGetInspectorNodes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    if (!delegate || !(*delegate)) {
        LOGE("JsGetInspectorNodes, delegate is null!");
        args.GetReturnValue().SetNull();
        return;
    }
    auto declarativeDelegate = AceType::DynamicCast<FrontendDelegateDeclarative>(*delegate);
    if (!declarativeDelegate) {
        LOGE("declarativeDelegate, delegate is null!");
        args.GetReturnValue().SetNull();
        return;
    }
    auto accessibilityManager = declarativeDelegate->GetJSAccessibilityManager();
    auto nodeInfos = accessibilityManager->DumpComposedElementsToJson();
    v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, nodeInfos->ToString().c_str()).ToLocalChecked();
    auto context = isolate->GetCurrentContext();
    args.GetReturnValue().Set(v8::JSON::Parse(context, result).ToLocalChecked());
}

void JsGetInspectorNodeById(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetNull();
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    if (!delegate || !(*delegate)) {
        LOGE("JsGetInspectorNodes, delegate is null!");
        args.GetReturnValue().SetNull();
        return;
    }
    auto declarativeDelegate = AceType::DynamicCast<FrontendDelegateDeclarative>(*delegate);
    if (!declarativeDelegate) {
        LOGE("declarativeDelegate is null!");
        args.GetReturnValue().SetNull();
        return;
    }
    auto accessibilityManager = declarativeDelegate->GetJSAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("accessibilityManager is null!");
        args.GetReturnValue().SetNull();
        return;
    }
    int32_t value = args[0]->Int32Value(context).ToChecked();
    auto nodeInfo = accessibilityManager->DumpComposedElementToJson(value);
    v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, nodeInfo->ToString().c_str()).ToLocalChecked();
    args.GetReturnValue().Set(v8::JSON::Parse(context, result).ToLocalChecked());
}

void Vp2Px(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    double density = SystemProperties::GetResolution();
    double vpValue = args[0]->NumberValue(context).ToChecked();
    double pxValue = vpValue * density;

    int32_t result = GreatOrEqual(pxValue, 0) ? (pxValue + 0.5) : (pxValue - 0.5);
    v8::Local<v8::Number> pxV8Value = v8::Number::New(isolate, result);
    args.GetReturnValue().Set(pxV8Value);
}

void Px2Vp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    double density = SystemProperties::GetResolution();
    if (NearZero(density)) {
        LOGE("The density cannot be zero");
        args.GetReturnValue().SetUndefined();
        return;
    }

    double pxValue = args[0]->NumberValue(context).ToChecked();
    double vpValue = pxValue / density;
    v8::Local<v8::Number> vpV8Value = v8::Number::New(isolate, vpValue);
    args.GetReturnValue().Set(vpV8Value);
}

void Fp2Px(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    double density = SystemProperties::GetResolution();
    double fpValue = args[0]->NumberValue(context).ToChecked();

    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    double fontScale = 1.0;
    if (pipelineContext) {
        fontScale = pipelineContext->GetFontScale();
    }
    double pxValue = fpValue * density * fontScale;

    int32_t result = GreatOrEqual(pxValue, 0) ? (pxValue + 0.5) : (pxValue - 0.5);
    v8::Local<v8::Number> pxV8Value = v8::Number::New(isolate, result);
    args.GetReturnValue().Set(pxV8Value);
}

void Px2Fp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    double density = SystemProperties::GetResolution();
    if (NearZero(density)) {
        LOGE("The density cannot be zero");
        args.GetReturnValue().SetUndefined();
        return;
    }
    double pxValue = args[0]->NumberValue(context).ToChecked();

    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    double fontScale = 1.0;
    if (pipelineContext) {
        fontScale = pipelineContext->GetFontScale();
    }
    double ratio = density * fontScale;
    double fpValue = pxValue / ratio;

    v8::Local<v8::Number> fpV8Value = v8::Number::New(isolate, fpValue);
    args.GetReturnValue().Set(fpV8Value);
}

void Lpx2Px(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    auto frontend = pipelineContext->GetFrontend();
    auto windowConfig = frontend->GetWindowConfig();
    double lpxValue = args[0]->NumberValue(context).ToChecked();
    double pxValue = lpxValue * windowConfig.designWidthScale;

    int32_t result = GreatOrEqual(pxValue, 0) ? (pxValue + 0.5) : (pxValue - 0.5);
    v8::Local<v8::Number> pxV8Value = v8::Number::New(isolate, result);
    args.GetReturnValue().Set(pxV8Value);
}

void Px2Lpx(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        LOGE("The arg is wrong, must have one argument");
        args.GetReturnValue().SetUndefined();
        return;
    }
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    auto frontend = pipelineContext->GetFrontend();
    auto windowConfig = frontend->GetWindowConfig();
    double pxValue = args[0]->NumberValue(context).ToChecked();
    double lpxValue = pxValue / windowConfig.designWidthScale;

    v8::Local<v8::Number> lpxV8Value = v8::Number::New(isolate, lpxValue);
    args.GetReturnValue().Set(lpxV8Value);
}

void SetAppBackgroundColor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    if (args.Length() < 1 || !args[0]->IsString()) {
        LOGE("The arg is wrong, must have one argument");
        return;
    }
    v8::String::Utf8Value backgroundColorUtf8Str(isolate, args[0]);
    if (!(*backgroundColorUtf8Str)) {
        return;
    }
    std::string backgroundColorStr(*backgroundColorUtf8Str);
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (pipelineContext) {
        pipelineContext->SetRootBgColor(Color::FromString(backgroundColorStr));
    }
}

static const std::unordered_map<std::string, std::function<void(BindingTarget)>> bindFuncs = {
    {"Flex", JSFlexImpl::JSBind},
    {"Text", JSText::JSBind},
    {"Animator", JSAnimator::JSBind},
    {"SpringProp", JSAnimator::JSBind},
    {"SpringMotion", JSAnimator::JSBind},
    {"ScrollMotion", JSAnimator::JSBind},
    {"Animator", JSAnimator::JSBind},
    {"Span", JSSpan::JSBind},
    {"Button", JSButton::JSBind},
    {"Canvas", JSCanvas::JSBind},
    {"LazyForEach", JSLazyForEach::JSBind},
    {"List", JSList::JSBind},
    {"ListItem", JSListItem::JSBind},
    {"LoadingProgress", JSLoadingProgress::JSBind},
    {"Image", JSImage::JSBind},
    {"ImageAnimator", JSImageAnimator::JSBind},
    {"Counter", JSCounter::JSBind},
    {"Progress", JSProgress::JSBind},
    {"Column", JSColumn::JSBind},
    {"Row", JSRow::JSBind},
    {"Grid", JSGrid::JSBind},
    {"GridItem", JSGridItem::JSBind},
    {"GridContainer", JSGridContainer::JSBind},
    {"Slider", JSSlider::JSBind},
    {"Stack", JSStack::JSBind},
    {"Stepper", JSStepper::JSBind},
    {"StepperItem", JSStepperItem::JSBind},
    {"ForEach", JSForEach::JSBind},
    {"Divider", JSDivider::JSBind},
    {"Swiper", JSSwiper::JSBind},
    {"Panel", JSSlidingPanel::JSBind},
    {"Navigation", JSNavigation::JSBind},
    {"Navigator", JSNavigator::JSBind},
    {"ColumnSplit", JSColumnSplit::JSBind},
    {"If", JSIfElse::JSBind},
    {"Scroll", JSScroll::JSBind},
    {"ScrollBar", JSScrollBar::JSBind},
    {"Toggle", JSToggle::JSBind},
    {"Blank", JSBlank::JSBind},
    {"Calendar", JSCalendar::JSBind},
    {"Rect", JSRect::JSBind},
    {"Refresh", JSRefresh::JSBind},
    {"Shape", JSShape::JSBind},
    {"Path", JSPath::JSBind},
    {"PatternLock", JSPatternLock::JSBind},
    {"PatternLockController", JSPatternLockController::JSBind},
    {"Circle", JSCircle::JSBind},
    {"Line", JSLine::JSBind},
    {"Polygon", JSPolygon::JSBind},
    {"Polyline", JSPolyline::JSBind},
    {"Ellipse", JSEllipse::JSBind},
    {"Tabs", JSTabs::JSBind},
    {"TabContent", JSTabContent::JSBind},
    {"TextPicker", JSTextPicker::JSBind},
    {"TextPickerDialog", JSTextPickerDialog::JSBind},
    {"DatePicker", JSDatePicker::JSBind},
    {"TimePicker", JSTimePicker::JSBind},
    {"DatePickerDialog", JSDatePickerDialog::JSBind},
    {"TimePickerDialog", JSTimePickerDialog::JSBind},
    {"PageTransitionEnter", JSPageTransition::JSBind},
    {"PageTransitionExit", JSPageTransition::JSBind},
    {"RowSplit", JSRowSplit::JSBind},
    {"ColumnSplit", JSColumnSplit::JSBind},
    {"AlphabetIndexer", JSIndexer::JSBind},
    {"Hyperlink", JSHyperlink::JSBind},
    {"Radio", JSRadio::JSBind},
    {"ActionSheet", JSActionSheet::JSBind},
    {"AlertDialog", JSAlertDialog::JSBind},
    {"ContextMenu", JSContextMenu::JSBind },
    {"AbilityComponent", JSAbilityComponent::JSBind},
    {"TextArea", JSTextArea::JSBind},
    {"TextInput", JSTextInput::JSBind},
    {"TextClock", JSTextClock::JSBind},
    {"SideBarContainer", JSSideBar::JSBind},
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    {"QRCode", JSQRCode::JSBind},
#ifdef FORM_SUPPORTED
    {"FormComponent", JSForm::JSBind},
#endif
#endif
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM) and !defined(OHOS_STANDARD_SYSTEM)
    {"RichText", JSRichText::JSBind},
    {"Web", JSWeb::JSBind},
    {"WebController", JSWebController::JSBind},
#endif
#ifndef WEARABLE_PRODUCT
    {"Camera", JSCamera::JSBind},
    {"Piece", JSPiece::JSBind},
    {"Rating", JSRating::JSBind},
    {"Video", JSVideo::JSBind},
#endif
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    {"XComponent", JSXComponent::JSBind},
    {"XComponentController", JSXComponentController::JSBind},
#endif
    {"DataPanel", JSDataPanel::JSBind},
    {"Badge", JSBadge::JSBind},
    {"Gauge", JSGauge::JSBind},
    {"Marquee", JSMarquee::JSBind},
    {"Gesture", JSGesture::JSBind},
    {"TapGesture", JSGesture::JSBind},
    {"LongPressGesture", JSGesture::JSBind},
    {"PanGesture", JSGesture::JSBind},
    {"SwipeGesture", JSGesture::JSBind},
    {"PinchGesture", JSGesture::JSBind},
    {"RotationGesture", JSGesture::JSBind},
    {"GestureGroup", JSGesture::JSBind},
    {"PanGestureOption", JSPanGestureOption::JSBind},
    {"CustomDialogController", JSCustomDialogController::JSBind},
    {"Scroller", JSScroller::JSBind},
    {"SwiperController", JSSwiperController::JSBind},
    {"TabsController", JSTabsController::JSBind},
    {"CalendarController", JSCalendarController::JSBind},
    {"CanvasRenderingContext2D", JSRenderingContext::JSBind},
    {"OffscreenCanvasRenderingContext2D", JSOffscreenRenderingContext::JSBind},
    {"CanvasGradient", JSCanvasGradient::JSBind},
    {"ImageBitmap", JSRenderImage::JSBind},
    {"ImageData", JSCanvasImageData::JSBind},
    {"Path2D", JSPath2D::JSBind},
    {"RenderingContextSettings", JSRenderingContextSettings::JSBind},
    {"AbilityController", JSAbilityComponentController::JSBind},
    {"VideoController", JSVideoController::JSBind},
    {"Search", JSSearch::JSBind},
    {"Select", JSSelect::JSBind},
    {"Sheet", JSSheet::JSBind},
    {"TextTimer", JSTextTimer::JSBind},
    {"SearchController", JSSearchController::JSBind },
    {"TextAreaController", JSTextAreaController::JSBind },
    {"TextInputController", JSTextInputController::JSBind},
    {"TextTimerController", JSTextTimerController::JSBind},
    {"TextClockController", JSTextClockController::JSBind},
    {"Checkbox", JSCheckbox::JSBind},
    {"CheckboxGroup", JSCheckboxGroup::JSBind}
};

void RegisterAllModule(BindingTarget globalObj)
{
    JSColumn::JSBind(globalObj);
    for (auto& iter : bindFuncs) {
        iter.second(globalObj);
    }
}

void RegisterModuleByName(BindingTarget globalObj, std::string moduleName)
{
    auto func = bindFuncs.find(moduleName);
    if (func == bindFuncs.end()) {
        LOGW("Component not exist, name: %{public}s", moduleName.c_str());
        return;
    }
    if ((*func).first == "Grid") {
        JSColumn::JSBind(globalObj);
    }

    (*func).second(globalObj);
}

void JsRegisterModules(BindingTarget globalObj, std::string modules)
{
    std::stringstream input(modules);
    std::string moduleName;
    while (std::getline(input, moduleName, ',')) {
        RegisterModuleByName(globalObj, moduleName);
    }
}

void JsRegisterViews(BindingTarget globalObj)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    globalObj->Set(v8::String::NewFromUtf8(isolate, "loadDocument").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsLoadDocument));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "dumpMemoryStats").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsDumpMemoryStats));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "$s").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsGetI18nResource));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "$m").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsGetMediaResource));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "getInspectorNodes").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsGetInspectorNodes));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "getInspectorNodeById").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, JsGetInspectorNodeById));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "vp2px").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Vp2Px));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "px2vp").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Px2Vp));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "fp2px").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Fp2Px));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "px2fp").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Px2Fp));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "lpx2px").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Lpx2Px));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "px2lpx").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Px2Lpx));
    globalObj->Set(v8::String::NewFromUtf8(isolate, "setAppBgColor").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, SetAppBackgroundColor));

    JSViewAbstract::JSBind();
    JSContainerBase::JSBind();
    JSShapeAbstract::JSBind();
    JSView::JSBind(globalObj);

    JSEnvironment::JSBind(globalObj);
    JSViewContext::JSBind(globalObj);
    JSViewStackProcessor::JSBind(globalObj);
    JSTouchHandler::JSBind(globalObj);
    JSPanHandler::JSBind(globalObj);
    JsDragFunction::JSBind(globalObj);
    JSPersistent::JSBind(globalObj);
    JSClipboard::JSBind(globalObj);

    JSProfiler::JSBind(globalObj);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    std::string jsModules;
    if (!(*delegate)->GetAssetContent("component_collection.txt", jsModules)) {
        LOGW("component list file load failed, bind all component!");
        RegisterAllModule(globalObj);
    } else {
        JsRegisterModules(globalObj, jsModules);
    }

    JSObjectTemplate toggleType;
    toggleType.Constant("Checkbox", 0);
    toggleType.Constant("Switch", 1);
    toggleType.Constant("Button", 2);

    JSObjectTemplate refreshStatus;
    refreshStatus.Constant("Inactive", 0);
    refreshStatus.Constant("Drag", 1);
    refreshStatus.Constant("OverDrag", 2);
    refreshStatus.Constant("Refresh", 3);
    refreshStatus.Constant("Done", 4);

    JSObjectTemplate mainAxisAlign;
    mainAxisAlign.Constant("Start", 1);
    mainAxisAlign.Constant("Center", 2);
    mainAxisAlign.Constant("End", 3);
    mainAxisAlign.Constant("SpaceBetween", 6);
    mainAxisAlign.Constant("SpaceAround", 7);

    JSObjectTemplate crossAxisAlign;
    crossAxisAlign.Constant("Start", 1);

    crossAxisAlign.Constant("Center", 2);
    crossAxisAlign.Constant("End", 3);
    crossAxisAlign.Constant("Stretch", 4);

    JSObjectTemplate direction;
    direction.Constant("Horizontal", 0);
    direction.Constant("Vertical", 1);

    JSObjectTemplate loadingProgressStyle;
    loadingProgressStyle.Constant("Default", 1);
    loadingProgressStyle.Constant("Circular", 2);
    loadingProgressStyle.Constant("Orbital", 3);

    JSObjectTemplate progressStyle;
    progressStyle.Constant("Linear", 0);
    progressStyle.Constant("Ring", 1);
    progressStyle.Constant("Eclipse", 2);
    progressStyle.Constant("ScaleRing", 3);
    progressStyle.Constant("Capsule", 4);

    JSObjectTemplate stackFit;
    stackFit.Constant("Keep", 0);
    stackFit.Constant("Stretch", 1);
    stackFit.Constant("Inherit", 2);
    stackFit.Constant("FirstChild", 3);

    JSObjectTemplate overflow;
    overflow.Constant("Clip", 0);
    overflow.Constant("Observable", 1);

    JSObjectTemplate alignment;
    alignment.Constant("TopLeft", 0);
    alignment.Constant("TopCenter", 1);
    alignment.Constant("TopRight", 2);
    alignment.Constant("CenterLeft", 3);
    alignment.Constant("Center", 4);
    alignment.Constant("CenterRight", 5);
    alignment.Constant("BottomLeft", 6);
    alignment.Constant("BottomCenter", 7);
    alignment.Constant("BottomRight", 8);

    JSObjectTemplate sliderStyle;
    sliderStyle.Constant("OutSet", 0);
    sliderStyle.Constant("InSet", 1);

    JSObjectTemplate sliderChangeMode;
    sliderChangeMode.Constant("Begin", 0);
    sliderChangeMode.Constant("Moving", 1);
    sliderChangeMode.Constant("End", 2);

    JSObjectTemplate pickerStyle;
    pickerStyle.Constant("Inline", 0);
    pickerStyle.Constant("Block", 1);
    pickerStyle.Constant("Fade", 2);

    JSObjectTemplate buttonType;
    buttonType.Constant("Normal", (int)ButtonType::NORMAL);
    buttonType.Constant("Capsule", (int)ButtonType::CAPSULE);
    buttonType.Constant("Circle", (int)ButtonType::CIRCLE);
    buttonType.Constant("Arc", (int)ButtonType::ARC);

    JSObjectTemplate iconPosition;
    iconPosition.Constant("Start", 0);
    iconPosition.Constant("End", 1);

    JSObjectTemplate badgePosition;
    badgePosition.Constant("RightTop", 0);
    badgePosition.Constant("Right", 1);
    badgePosition.Constant("Left", 2);

    globalObj->Set(isolate, "MainAxisAlign", *mainAxisAlign);
    globalObj->Set(isolate, "CrossAxisAlign", *crossAxisAlign);
    globalObj->Set(isolate, "Direction", *direction);
    globalObj->Set(isolate, "StackFit", *stackFit);
    globalObj->Set(isolate, "Align", *alignment);
    globalObj->Set(isolate, "Overflow", *overflow);
    globalObj->Set(isolate, "ButtonType", *buttonType);
    globalObj->Set(isolate, "LoadingProgressStyle", *loadingProgressStyle);
    globalObj->Set(isolate, "ProgressStyle", *progressStyle);
    globalObj->Set(isolate, "ToggleType", *toggleType);
    globalObj->Set(isolate, "RefreshStatus", *refreshStatus);
    globalObj->Set(isolate, "SliderStyle", *sliderStyle);
    globalObj->Set(isolate, "SliderChangeMode", *sliderChangeMode);
    globalObj->Set(isolate, "IconPosition", *iconPosition);
    globalObj->Set(isolate, "PickerStyle", *pickerStyle);
    globalObj->Set(isolate, "BadgePosition", *badgePosition);
    LOGD("View classes and jsCreateDocument, registerObservableObject functions registered.");
}

} // namespace OHOS::Ace::Framework
