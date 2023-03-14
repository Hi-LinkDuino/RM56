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

#include "core/components/test/json/text_span_creator.h"

#include "base/log/log.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/test/json/text_style_creator.h"
#include "core/components/text_span/text_span_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TextSpanCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson TextSpan");
    auto data = componentJson.GetString(TEXT_DATA);
    auto textSpan = AceType::MakeRefPtr<TextSpanComponent>(data);
    if (componentJson.Contains(TEXT_STYLE) && componentJson.GetValue(TEXT_STYLE)->IsObject()) {
        auto spanStyle = TextStyleCreator::CreateFromJson(*componentJson.GetValue(TEXT_STYLE));
        textSpan->SetTextStyle(spanStyle);
    }
    auto children = factory.CreateChildComponent(componentJson);
    for (const auto& child : children) {
        textSpan->AppendChild(child);
    }
    return textSpan;
}

} // namespace OHOS::Ace