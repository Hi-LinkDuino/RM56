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

#include "bridge/declarative_frontend/jsview/js_environment.h"

#include "base/memory/referenced.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/common/environment/environment_proxy.h"
#include "frameworks/base/i18n/localization.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_container_base.h"

namespace OHOS::Ace::Framework {

void JSEnvironment::JSBind(BindingTarget globalObj)
{
    JSClass<JSEnvironment>::Declare("EnvironmentSetting");
    JSClass<JSEnvironment>::CustomMethod("getAccessibilityEnabled", &JSEnvironment::GetAccessibilityEnabled);
    JSClass<JSEnvironment>::CustomMethod("getColorMode", &JSEnvironment::GetColorMode);
    JSClass<JSEnvironment>::CustomMethod("getFontScale", &JSEnvironment::GetFontScale);
    JSClass<JSEnvironment>::CustomMethod("getFontWeightScale", &JSEnvironment::GetFontWeightScale);
    JSClass<JSEnvironment>::CustomMethod("getLayoutDirection", &JSEnvironment::GetLayoutDirection);
    JSClass<JSEnvironment>::CustomMethod("getLanguageCode", &JSEnvironment::GetLanguageCode);
    JSClass<JSEnvironment>::CustomMethod("onValueChanged", &JSEnvironment::onChange);
    JSClass<JSEnvironment>::Bind(globalObj, JSEnvironment::ConstructorCallback, JSEnvironment::DestructorCallback);
}

void JSEnvironment::ConstructorCallback(const JSCallbackInfo& args)
{
    auto environment = Referenced::MakeRefPtr<JSEnvironment>();
    environment->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(environment));
}

void JSEnvironment::DestructorCallback(JSEnvironment* environment)
{
    if (environment != nullptr) {
        environment->DecRefCount();
    }
}

void JSEnvironment::GetAccessibilityEnabled(const JSCallbackInfo& args)
{
    std::string value;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    value = "false";
#else
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto executor = container->GetTaskExecutor();
    value = EnvironmentProxy::GetInstance()->GetEnvironment(executor)->GetAccessibilityEnabled();
#endif
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto returnValue = JSVal(ToJSValue(value));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

void JSEnvironment::GetColorMode(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto colorMode = SystemProperties::GetColorMode();
    auto returnValue = JSVal(ToJSValue(static_cast<int32_t>(colorMode)));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

void JSEnvironment::onChange(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
}

void JSEnvironment::GetFontScale(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return;
    }
    auto context = container->GetPipelineContext();
    auto returnValue = JSVal(ToJSValue(context->GetFontScale()));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

void JSEnvironment::GetFontWeightScale(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto weightScale = SystemProperties::GetFontWeightScale();
    auto returnValue = JSVal(ToJSValue(weightScale));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

void JSEnvironment::GetLayoutDirection(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto isRTL = AceApplicationInfo::GetInstance().IsRightToLeft();
    auto value = isRTL ? 0 : 1;
    auto returnValue = JSVal(ToJSValue(value));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

void JSEnvironment::GetLanguageCode(const JSCallbackInfo& args)
{
    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(args.GetExecutionContext());
    auto location = Localization::GetInstance();
    auto language = location->GetLanguage();
    auto returnValue = JSVal(ToJSValue(language));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    args.SetReturnValue(returnPtr);
}

} // namespace OHOS::Ace::Framework
