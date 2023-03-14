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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_input_bridge.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

void GetInputOption(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, InputOption& inputOption)
{
    if (!valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> properties;
    int32_t len = 0;
    if (!valObject->GetPropertyNames(runtime, properties, len)) {
        LOGE("Get property names failed when get input option.");
        return;
    }
    for (int32_t i = 0; i < len; i++) {
        const auto& key = properties->GetElement(runtime, i);
        std::string keyStr = key->ToString(runtime);
        if (keyStr.empty()) {
            continue;
        }
        const auto& val = valObject->GetProperty(runtime, key);
        if (val->IsNumber(runtime) || val->IsBoolean(runtime) || val->IsString(runtime)) {
            std::string valStr = val->ToString(runtime);
            if (valStr.empty()) {
                continue;
            }
            if (strcmp(keyStr.c_str(), DOM_INPUT_OPTION_ICON) == 0) {
                inputOption.image = valStr;
            } else if (strcmp(keyStr.c_str(), DOM_INPUT_OPTION_CONTENT) == 0) {
                inputOption.text = valStr;
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace

void JsiInputBridge::ParseInputOptions(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valArray)
{
    shared_ptr<JsValue> properties;
    int32_t len = 0;
    if (!valArray->GetPropertyNames(runtime, properties, len)) {
        LOGE("Get property names failed when parse input options.");
        return;
    }
    for (int32_t i = 0; i < len; ++i) {
        const auto& itemKey = properties->GetElement(runtime, i);
        const auto& itemVal = valArray->GetProperty(runtime, itemKey);
        InputOption inputOption;
        if (itemVal->IsObject(runtime)) {
            GetInputOption(runtime, itemVal, inputOption);
            inputOptions_.push_back(inputOption);
        }
    }
}

} // namespace OHOS::Ace::Framework
