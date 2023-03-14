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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_badge_bridge.h"

namespace OHOS::Ace::Framework {

void JsiBadgeBridge::ParseBadgeConfig(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject)
{
    if (!runtime) {
        LOGE("runtime is null");
        return;
    }
    if (!valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> value = valObject->GetProperty(runtime, key);
        if (!value) {
            LOGW("value is null. Ignoring!");
            continue;
        }
        if (value->IsString(runtime) || value->IsNumber(runtime)) {
            std::string valStr = value->ToString(runtime);
            // static linear map must be sorted by key.
            static const LinearMapNode<void (*)(std::string&, JsiBadgeBridge&)> badgeConfigOperators[] = {
                { DOM_BADGE_COLOR,
                    [](std::string& valStr, JsiBadgeBridge& jsiBadgeBridge) {
                        jsiBadgeBridge.badgeConfig_.badgeColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_CIRCLE_SIZE,
                    [](std::string& valStr, JsiBadgeBridge& jsiBadgeBridge) {
                        jsiBadgeBridge.badgeConfig_.badgeSize = { StringUtils::StringToDimension(valStr), true };
                    } },
                { DOM_BADGE_TEXT_COLOR,
                    [](std::string& valStr, JsiBadgeBridge& jsiBadgeBridge) {
                        jsiBadgeBridge.badgeConfig_.textColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_TEXT_FONT_SIZE,
                    [](std::string& valStr, JsiBadgeBridge& jsiBadgeBridge) {
                        jsiBadgeBridge.badgeConfig_.textSize = { StringUtils::StringToDimension(valStr), true };
                    } }
            };
            auto operatorIter =
                BinarySearchFindIndex(badgeConfigOperators, ArraySize(badgeConfigOperators), keyStr.c_str());
            if (operatorIter != -1) {
                badgeConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace OHOS::Ace::Framework
