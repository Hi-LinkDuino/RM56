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

#include "core/components_v2/inspector/qrcode_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const QrcodeComposedElement&)>> CREATE_JSON_MAP {
    { "value", [](const QrcodeComposedElement& inspector) { return inspector.GetValue(); } },
    { "color", [](const QrcodeComposedElement& inspector) { return inspector.GetQrcodeColor(); } }
};

} // namespace

void QrcodeComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("value: ").append(GetValue()));
}

std::unique_ptr<JsonValue> QrcodeComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string QrcodeComposedElement::GetValue() const
{
    auto renderQrcode = GetRenderQrcode();
    auto value = renderQrcode ? renderQrcode->GetValue() : "";
    return value;
}

std::string QrcodeComposedElement::GetQrcodeColor() const
{
    auto renderQrcode = GetRenderQrcode();
    auto color = renderQrcode ? renderQrcode->GetQrcodeColor() : Color::BLACK;
    return ConvertColorToString(color);
}

std::string QrcodeComposedElement::GetBackgroundColor() const
{
    auto renderQrcode = GetRenderQrcode();
    auto color = renderQrcode ? renderQrcode->GetBackgroundColor() : Color::WHITE;
    return ConvertColorToString(color);
}

RefPtr<RenderQrcode> QrcodeComposedElement::GetRenderQrcode() const
{
    auto node = GetInspectorNode(QrcodeElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderQrcode>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2