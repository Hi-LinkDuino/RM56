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

#include "frameworks/bridge/js_frontend/engine/quickjs/badge_bridge.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

void BadgeBridge::ParseBadgeConfig(JSContext* ctx, JSValue valObject)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val) || JS_IsNumber(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            // static linear map must be sorted by key.
            static const LinearMapNode<void (*)(const char*, BadgeBridge&)> badgeConfigOperators[] = {
                { DOM_BADGE_COLOR,
                    [](const char* valStr, BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.badgeColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_CIRCLE_SIZE,
                    [](const char* valStr, BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.badgeSize = { StringToDimension(valStr), true };
                    } },
                { DOM_BADGE_TEXT_COLOR,
                    [](const char* valStr, BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.textColor = { Color::FromString(valStr), true };
                    } },
                { DOM_BADGE_TEXT_FONT_SIZE,
                    [](const char* valStr, BadgeBridge& badgeBridge) {
                        badgeBridge.badgeConfig_.textSize = { StringToDimension(valStr), true };
                    } }
            };
            auto operatorIter = BinarySearchFindIndex(badgeConfigOperators, ArraySize(badgeConfigOperators), key);
            if (operatorIter != -1) {
                badgeConfigOperators[operatorIter].value(valStr, *this);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

} // namespace OHOS::Ace::Framework