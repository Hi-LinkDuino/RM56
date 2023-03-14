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

#include "frameworks/bridge/declarative_frontend/jsview/js_web_controller.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_webview_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {
namespace {
void ParseWebViewValueToJsValue(
    std::shared_ptr<WebJSValue> value, std::vector<JSRef<JSVal>>& argv)
{
    auto type = value->GetType();
    switch (type) {
        case WebJSValue::Type::INTEGER:
            argv.push_back(JSRef<JSVal>::Make(ToJSValue(value->GetInt())));
            break;
        case WebJSValue::Type::DOUBLE: {
            argv.push_back(JSRef<JSVal>::Make(ToJSValue(value->GetDouble())));
            break;
        }
        case WebJSValue::Type::BOOLEAN:
            argv.push_back(JSRef<JSVal>::Make(ToJSValue(value->GetBoolean())));
            break;
        case WebJSValue::Type::STRING:
            argv.push_back(JSRef<JSVal>::Make(ToJSValue(value->GetString())));
            break;
        case WebJSValue::Type::NONE:
            break;
        default:
            LOGW("WebJavaScriptResultCallBack: jsvalue type[%{public}d] not support!", (int)type);
            break;
    }
}

std::shared_ptr<WebJSValue> ParseValue(
    const JSRef<JSVal>& resultValue, std::shared_ptr<WebJSValue> webviewValue)
{
    webviewValue->error_ = static_cast<int>(WebJavaScriptBridgeError::NO_ERROR);
    if (resultValue->IsBoolean()) {
        webviewValue->SetType(WebJSValue::Type::BOOLEAN);
        webviewValue->SetBoolean(resultValue->ToBoolean());
    } else if (resultValue->IsNull()) {
        webviewValue->SetType(WebJSValue::Type::NONE);
    } else if (resultValue->IsString()) {
        webviewValue->SetType(WebJSValue::Type::STRING);
        webviewValue->SetString(resultValue->ToString());
    } else if (resultValue->IsNumber()) {
        webviewValue->SetType(WebJSValue::Type::DOUBLE);
        webviewValue->SetInt(resultValue->ToNumber<double>());
    } else if (resultValue->IsArray() || resultValue->IsObject() || resultValue->IsUndefined()) {
        webviewValue->SetType(WebJSValue::Type::NONE);
        webviewValue->error_ = static_cast<int>(WebJavaScriptBridgeError::OBJECT_IS_GONE);
    }
    return webviewValue;
}
}

JSWebController::JSWebController()
{
    instanceId_ = Container::CurrentId();
}


std::shared_ptr<WebJSValue> JSWebController::GetJavaScriptResult(
    const std::string& objectName,
    const std::string& objectMethod,
    const std::vector<std::shared_ptr<WebJSValue>>& args)
{
    std::vector<JSRef<JSVal>> argv = {};
    std::shared_ptr<WebJSValue> jsResult =
        std::make_shared<WebJSValue>(WebJSValue::Type::NONE);
    if (objectorMap_.find(objectName) == objectorMap_.end()) {
        return jsResult;
    }
    auto jsObject = objectorMap_[objectName];
    if (jsObject->IsEmpty()) {
        return jsResult;
    }
    for (std::shared_ptr<WebJSValue> input: args) {
        ParseWebViewValueToJsValue(input, argv);
    }
    JSRef<JSFunc> func = JSRef<JSFunc>::Cast(jsObject->GetProperty(objectMethod.c_str()));
    if (func->IsEmpty()) {
        LOGE("%{public}s not found or is not a function!", objectMethod.c_str());
        jsResult->error_ = static_cast<int>(WebJavaScriptBridgeError::OBJECT_IS_GONE);
        return jsResult;
    }
    JSRef<JSVal> result = argv.empty() ? func->Call(jsObject, 0, {}) : func->Call(jsObject, argv.size(), &argv[0]);
    return ParseValue(result, jsResult);
}

void JSWebController::JSBind(BindingTarget globalObj)
{
    JSClass<JSWebController>::Declare("WebController");
    JSClass<JSWebController>::CustomMethod("loadUrl", &JSWebController::LoadUrl);
    JSClass<JSWebController>::CustomMethod("runJavaScript", &JSWebController::ExecuteTypeScript);
    JSClass<JSWebController>::CustomMethod("refresh", &JSWebController::Refresh);
    JSClass<JSWebController>::CustomMethod("stop", &JSWebController::StopLoading);
    JSClass<JSWebController>::CustomMethod("getHitTest", &JSWebController::GetHitTestResult);
    JSClass<JSWebController>::CustomMethod("registerJavaScriptProxy", &JSWebController::AddJavascriptInterface);
    JSClass<JSWebController>::CustomMethod("deleteJavaScriptRegister", &JSWebController::RemoveJavascriptInterface);
    JSClass<JSWebController>::CustomMethod("onInactive", &JSWebController::OnInactive);
    JSClass<JSWebController>::CustomMethod("onActive", &JSWebController::OnActive);
    JSClass<JSWebController>::CustomMethod("zoom", &JSWebController::Zoom);
    JSClass<JSWebController>::CustomMethod("requestFocus", &JSWebController::RequestFocus);
    JSClass<JSWebController>::CustomMethod("loadData", &JSWebController::LoadDataWithBaseUrl);
    JSClass<JSWebController>::CustomMethod("backward", &JSWebController::Backward);
    JSClass<JSWebController>::CustomMethod("forward", &JSWebController::Forward);
    JSClass<JSWebController>::CustomMethod("accessStep", &JSWebController::AccessStep);
    JSClass<JSWebController>::CustomMethod("accessForward", &JSWebController::AccessForward);
    JSClass<JSWebController>::CustomMethod("accessBackward", &JSWebController::AccessBackward);
    JSClass<JSWebController>::Bind(globalObj, JSWebController::Constructor, JSWebController::Destructor);
}

void JSWebController::Constructor(const JSCallbackInfo& args)
{
    auto webController = Referenced::MakeRefPtr<JSWebController>();
    webController->IncRefCount();
    RefPtr<WebController> controller = AceType::MakeRefPtr<WebController>();
    webController->SetController(controller);
    args.SetReturnValue(Referenced::RawPtr(webController));
}

void JSWebController::Destructor(JSWebController* webController)
{
    if (webController != nullptr) {
        webController->DecRefCount();
    }
}

void JSWebController::Reload() const
{
    if (webController_) {
        webController_->Reload();
    }
}

void JSWebController::LoadUrl(const JSCallbackInfo& args)
{
    ContainerScope scope(instanceId_);
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("invalid url params");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    JSRef<JSVal> valUrl = obj->GetProperty("url");
    std::string url;
    if (valUrl->IsObject()) {
        // same as src process of JSWeb::Create
        std::string webSrc;
        if (!JSViewAbstract::ParseJsMedia(valUrl, webSrc)) {
            LOGE("JSWebController failed to parse url object");
            return;
        }
        auto np = webSrc.find_first_of("/");
        url = (np == std::string::npos) ? webSrc : webSrc.erase(np, 1);
    } else if (!ConvertFromJSValue(valUrl, url)) {
        LOGW("can't find url.");
        return;
    }

    JSRef<JSVal> headers = obj->GetProperty("headers");
    std::map<std::string, std::string> httpHeaders;
    if (headers->IsArray()) {
        JSRef<JSArray> array = JSRef<JSArray>::Cast(headers);
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> jsValue = array->GetValueAt(i);
            if (!jsValue->IsObject()) {
                continue;
            }
            JSRef<JSObject> obj = JSRef<JSObject>::Cast(jsValue);
            std::string key;
            if (!ConvertFromJSValue(obj->GetProperty("headerKey"), key)) {
                LOGW("can't find key at index %{public}d of additionalHttpHeaders, so skip it.", i);
                continue;
            }
            std::string value;
            if (!ConvertFromJSValue(obj->GetProperty("headerValue"), value)) {
                LOGW("can't find value at index %{public}d of additionalHttpHeaders, so skip it.", i);
                continue;
            }
            httpHeaders[key] = value;
        }
    }
    if (webController_) {
        webController_->LoadUrl(url, httpHeaders);
    }
    LOGI("JSWebController load url:%{public}s, httpHeaders:%{public}d", url.c_str(), (int)httpHeaders.size());
}

void JSWebController::ExecuteTypeScript(const JSCallbackInfo& args)
{
    LOGI("JSWebController excute typescript");
    ContainerScope scope(instanceId_);
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("invalid excute params");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    std::string script;
    if (!ConvertFromJSValue(obj->GetProperty("script"), script)) {
        LOGW("can't find script.");
        return;
    }
    JSRef<JSVal> tsCallback = obj->GetProperty("callback");
    std::function<void(std::string)> callback = nullptr;
    if (tsCallback->IsFunction()) {
        auto jsCallback =
            AceType::MakeRefPtr<JsWebViewFunction>(JSRef<JSFunc>::Cast(tsCallback));
        callback = [execCtx = args.GetExecutionContext(), func = std::move(jsCallback)](std::string result) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("ExecuteTypeScript CallBack");
            LOGI("About to call ExecuteTypeScript callback method on js");
            func->Execute(result);
        };
    }
    if (webController_) {
        webController_->ExecuteTypeScript(script, std::move(callback));
    }
}

void JSWebController::LoadDataWithBaseUrl(const JSCallbackInfo& args)
{
    ContainerScope scope(instanceId_);
    if (args.Length() >= 1 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);

        std::string data;
        if (!ConvertFromJSValue(obj->GetProperty("data"), data)) {
            return;
        }

        std::string mimeType;
        if (!ConvertFromJSValue(obj->GetProperty("mimeType"), mimeType)) {
            return;
        }

        std::string encoding;
        if (!ConvertFromJSValue(obj->GetProperty("encoding"), encoding)) {
            return;
        }

        std::string baseUrl;
        std::string historyUrl;
        ConvertFromJSValue(obj->GetProperty("baseUrl"), baseUrl);
        ConvertFromJSValue(obj->GetProperty("historyUrl"), historyUrl);
        if (webController_) {
            webController_->LoadDataWithBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
        }
    }
}

void JSWebController::Backward(const JSCallbackInfo& args)
{
    LOGI("JSWebController Start backward.");
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->Backward();
    }
}

void JSWebController::Forward(const JSCallbackInfo& args)
{
    LOGI("JSWebController Start forward.");
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->Forward();
    }
}

void JSWebController::AccessStep(const JSCallbackInfo& args)
{
    LOGI("JSWebController start accessStep.");
    ContainerScope scope(instanceId_);
    int32_t step = 0;
    if (args.Length() < 1 || !ConvertFromJSValue(args[0], step)) {
        LOGE("AccessStep parameter is invalid.");
        return;
    }
    if (webController_) {
        auto canAccess = webController_->AccessStep(step);
        auto jsVal = JSVal(ToJSValue(canAccess));
        auto returnValue = JSRef<JSVal>::Make(jsVal);
        args.SetReturnValue(returnValue);
    }
}

void JSWebController::AccessBackward(const JSCallbackInfo& args)
{
    LOGI("JSWebController start accessBackward.");
    ContainerScope scope(instanceId_);
    if (webController_) {
        auto canAccess = webController_->AccessBackward();
        auto jsVal = JSVal(ToJSValue(canAccess));
        auto returnValue = JSRef<JSVal>::Make(jsVal);
        args.SetReturnValue(returnValue);
    }
}

void JSWebController::AccessForward(const JSCallbackInfo& args)
{
    LOGI("JSWebController start accessForward.");
    ContainerScope scope(instanceId_);
    if (webController_) {
        auto canAccess = webController_->AccessForward();
        auto jsVal = JSVal(ToJSValue(canAccess));
        auto returnValue = JSRef<JSVal>::Make(jsVal);
        args.SetReturnValue(returnValue);
    }
}

void JSWebController::Refresh(const JSCallbackInfo& args)
{
    LOGI("JSWebController Refresh");
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->Refresh();
    }
}

void JSWebController::StopLoading(const JSCallbackInfo& args)
{
    LOGI("JSWebController StopLoading");
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->StopLoading();
    }
}

void JSWebController::GetHitTestResult(const JSCallbackInfo& args)
{
    LOGI("JSWebController get his test result");
    ContainerScope scope(instanceId_);
    if (webController_) {
        int result = webController_->GetHitTestResult();
        args.SetReturnValue(JSRef<JSVal>::Make(ToJSValue(result)));
    }
}

void JSWebController::SetJavascriptCallBackImpl()
{
    if (!webController_ || jsRegisterCallBackInit_) {
        return;
    }

    LOGI("JSWebController set webview javascript CallBack");
    jsRegisterCallBackInit_ = true;
    WebController::JavaScriptCallBackImpl callback =
        [weak = WeakClaim(this)](
        const std::string& objectName, const std::string& objectMethod,
        const std::vector<std::shared_ptr<WebJSValue>>& args) {
        auto jsWebController = weak.Upgrade();
        if (jsWebController == nullptr) {
            return std::make_shared<WebJSValue>(WebJSValue::Type::NONE);
        }
        return jsWebController->GetJavaScriptResult(objectName, objectMethod, args);
    };
    webController_->SetJavaScriptCallBackImpl(std::move(callback));
}

void JSWebController::AddJavascriptInterface(const JSCallbackInfo& args)
{
    LOGI("JSWebController add js interface");
    ContainerScope scope(instanceId_);
    if (args.Length() < 1 || !args[0]->IsObject()) {
        return;
    }
    if (webController_ == nullptr) {
        LOGW("JSWebController not ready");
        return;
    }
    // Init webview callback
    SetJavascriptCallBackImpl();

    // options
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    // options.name
    std::string objName;
    if (!ConvertFromJSValue(obj->GetProperty("name"), objName)) {
        return;
    }
    // options.obj
    JSRef<JSVal> jsClassObj = obj->GetProperty("object");
    if (!jsClassObj->IsObject()) {
        LOGW("JSWebController param obj is not object");
        return;
    }
    if (objectorMap_.find(objName) == objectorMap_.end()) {
        objectorMap_[objName] = JSRef<JSObject>::Cast(jsClassObj);
    }
    // options.methodList
    std::vector<std::string> methods;
    JSRef<JSVal> methodList = obj->GetProperty("methodList");
    JSRef<JSArray> array = JSRef<JSArray>::Cast(methodList);
    if (array->IsArray()) {
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> method = array->GetValueAt(i);
            if (method->IsString()) {
                methods.push_back(method->ToString());
            }
        }
    }

    webController_->AddJavascriptInterface(objName, methods);
}

void JSWebController::InitJavascriptInterface()
{
    if (!webController_) {
        LOGW("JSWebController not ready");
        return;
    }
    // Init webview callback
    SetJavascriptCallBackImpl();
    for (auto& entry : methods_) {
        webController_->AddJavascriptInterface(entry.first, entry.second);
    }
}

void JSWebController::SetJavascriptInterface(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsObject()) {
        return;
    }
    if (!webController_) {
        LOGW("JSWebController not ready");
        return;
    }
    // options
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    // options.name
    std::string objName;
    if (!ConvertFromJSValue(obj->GetProperty("name"), objName)) {
        return;
    }
    // options.obj
    JSRef<JSVal> jsClassObj = obj->GetProperty("object");
    if (!jsClassObj->IsObject()) {
        LOGW("JSWebController param obj is not object");
        return;
    }
    objectorMap_[objName] = JSRef<JSObject>::Cast(jsClassObj);
    std::vector<std::string> methods;
    methods_.clear();
    JSRef<JSVal> methodList = obj->GetProperty("methodList");
    JSRef<JSArray> array = JSRef<JSArray>::Cast(methodList);
    if (array->IsArray()) {
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> method = array->GetValueAt(i);
            if (method->IsString()) {
                methods.emplace_back(method->ToString());
            }
        }
    }
    methods_[objName] = methods;

    webController_->SetInitJavascriptInterface([weak = WeakClaim(this)]() {
        auto jsWebcontroller = weak.Upgrade();
        if (jsWebcontroller) {
            jsWebcontroller->InitJavascriptInterface();
        }
    });
}

void JSWebController::RemoveJavascriptInterface(const JSCallbackInfo& args)
{
    LOGI("JSWebController remove js interface");
    ContainerScope scope(instanceId_);
    std::string objName;
    if (args.Length() < 1 || !ConvertFromJSValue(args[0], objName)) {
        return;
    }
    if (objectorMap_.find(objName) == objectorMap_.end()) {
        return;
    }
    objectorMap_.erase(objName);
    if (webController_) {
        webController_->RemoveJavascriptInterface(objName, {});
    }
}

void JSWebController::OnInactive(const JSCallbackInfo& args)
{
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->OnInactive();
    }
}

void JSWebController::OnActive(const JSCallbackInfo& args)
{
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->OnActive();
    }
}

void JSWebController::Zoom(const JSCallbackInfo& args)
{
    ContainerScope scope(instanceId_);
    float factor = 1.0;
    if (args.Length() < 1 || !ConvertFromJSValue(args[0], factor)) {
        LOGE("Zoom parameter is invalid.");
        return;
    }
    if (webController_) {
        webController_->Zoom(factor);
    }
}

void JSWebController::RequestFocus(const JSCallbackInfo& args)
{
    LOGI("JSWebController request focus");
    ContainerScope scope(instanceId_);
    if (webController_) {
        webController_->RequestFocus();
    }
}
} // namespace OHOS::Ace::Framework