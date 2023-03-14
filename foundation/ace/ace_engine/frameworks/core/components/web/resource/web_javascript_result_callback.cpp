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

#include "core/components/web/resource/web_javascript_result_callback.h"

#include "base/log/log.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components/web/resource/web_javascript_value.h"

namespace OHOS::Ace {
using namespace OHOS::Ace::Framework;
using namespace OHOS::NWeb;

std::vector<std::shared_ptr<WebJSValue>> GetWebJSValue(const std::vector<std::shared_ptr<NWebValue>>& args)
{
    std::vector<std::shared_ptr<WebJSValue>> webJSValues;
    for (auto value : args) {
        if (value == nullptr) {
            continue;
        }
        NWebValue::Type type = value->GetType();
        auto webJsValue = std::make_shared<WebJSValue>(WebJSValue::Type::NONE);
        switch (type) {
            case NWebValue::Type::INTEGER:
                webJsValue->SetType(WebJSValue::Type::INTEGER);
                webJsValue->SetInt(value->GetInt());
                webJSValues.push_back(webJsValue);
                break;
            case NWebValue::Type::DOUBLE:
                webJsValue->SetType(WebJSValue::Type::DOUBLE);
                webJsValue->SetDouble(value->GetDouble());
                webJSValues.push_back(webJsValue);
                break;
            case NWebValue::Type::BOOLEAN:
                webJsValue->SetType(WebJSValue::Type::BOOLEAN);
                webJsValue->SetBoolean(value->GetBoolean());
                webJSValues.push_back(webJsValue);
                break;
            case NWebValue::Type::STRING:
                webJsValue->SetType(WebJSValue::Type::STRING);
                webJsValue->SetString(value->GetString());
                webJSValues.push_back(webJsValue);
                break;
            case NWebValue::Type::NONE:
                break;
            default:
                LOGI("WebJavaScriptResultCallBack: jsvalue type not support!");
                break;
        }
    }
    return webJSValues;
}

std::shared_ptr<NWebValue> GetWebViewValue(const std::shared_ptr<WebJSValue>& webJSValue)
{
    std::shared_ptr<NWebValue> webViewValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    WebJSValue::Type type = webJSValue->GetType();
    switch (type) {
        case WebJSValue::Type::INTEGER:
            webViewValue->SetType(NWebValue::Type::INTEGER);
            webViewValue->SetInt(webJSValue->GetInt());
            break;
        case WebJSValue::Type::DOUBLE:
            webViewValue->SetType(NWebValue::Type::DOUBLE);
            webViewValue->SetDouble(webJSValue->GetDouble());
            break;
        case WebJSValue::Type::BOOLEAN:
            webViewValue->SetType(NWebValue::Type::BOOLEAN);
            webViewValue->SetBoolean(webJSValue->GetBoolean());
            break;
        case WebJSValue::Type::STRING:
            webViewValue->SetType(NWebValue::Type::STRING);
            webViewValue->SetString(webJSValue->GetString());
            break;
        case WebJSValue::Type::NONE:
            break;
        default:
            LOGI("WebJavaScriptResultCallBack: jsvalue type not support!");
            break;
    }
    return webViewValue;
}

std::shared_ptr<NWebValue> WebJavaScriptResultCallBack::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& object_name)
{
    LOGI("GetJavaScriptResult");
    ContainerScope scope(instanceId_);
    std::shared_ptr<WebJSValue> result;
    auto jsArgs = GetWebJSValue(args);

    auto task = Container::CurrentTaskExecutor();
    if (task == nullptr) {
        LOGW("can't get task executor");
        return std::make_shared<NWebValue>(NWebValue::Type::NONE);
    }

    task->PostSyncTask(
        [webJSCallBack = this, object_name, method, jsArgs, &result] {
            if (webJSCallBack->javaScriptCallBackImpl_) {
                result = webJSCallBack->javaScriptCallBackImpl_(object_name, method, jsArgs);
            }
        },
        OHOS::Ace::TaskExecutor::TaskType::JS);

    return GetWebViewValue(result);
}
} // namespace OHOS::Ace