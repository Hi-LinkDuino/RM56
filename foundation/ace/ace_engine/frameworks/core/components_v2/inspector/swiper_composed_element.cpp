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

#include "core/components_v2/inspector/swiper_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const SwiperComposedElement&)>> CREATE_JSON_MAP {
    { "index", [](const SwiperComposedElement& inspector) { return inspector.GetCurrentIndex(); } },
    { "autoPlay", [](const SwiperComposedElement& inspector) { return inspector.GetAutoPlay(); } },
    { "interval", [](const SwiperComposedElement& inspector) { return inspector.GetInterval(); } },
    { "indicator", [](const SwiperComposedElement& inspector) { return inspector.GetIndicator(); } },
    { "loop", [](const SwiperComposedElement& inspector) { return inspector.GetLoop(); } },
    { "duration", [](const SwiperComposedElement& inspector) { return inspector.GetDuration(); } },
    { "vertical", [](const SwiperComposedElement& inspector) { return inspector.GetVertical(); } },
    { "disableSwipe", [](const SwiperComposedElement& inspector) { return inspector.GetDisableSwipe(); } },
    { "itemSpace", [](const SwiperComposedElement& inspector) { return inspector.GetItemSpace(); } },
    { "curve", [](const SwiperComposedElement& inspector) { return inspector.GetCurve(); } },
    { "indicatorStyle", [](const SwiperComposedElement& inspector) { return inspector.GetIndicatorStyle(); } }
};

} // namespace

void SwiperComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("index: ").append(GetCurrentIndex()));
    DumpLog::GetInstance().AddDesc(std::string("autoPlay: ").append(GetAutoPlay()));
    DumpLog::GetInstance().AddDesc(std::string("interval: ").append(GetInterval()));
    DumpLog::GetInstance().AddDesc(std::string("indicator: ").append(GetIndicator()));
    DumpLog::GetInstance().AddDesc(std::string("loop: ").append(GetLoop()));
    DumpLog::GetInstance().AddDesc(std::string("duration: ").append(GetDuration()));
    DumpLog::GetInstance().AddDesc(std::string("vertical: ").append(GetVertical()));
    DumpLog::GetInstance().AddDesc(std::string("disableSwipe: ").append(GetDisableSwipe()));
    DumpLog::GetInstance().AddDesc(std::string("itemSpace: ").append(GetItemSpace()));
    DumpLog::GetInstance().AddDesc(std::string("curve: ").append(GetCurve()));
    DumpLog::GetInstance().AddDesc(std::string("selectedColor: ").append(GetSelectedColor()));
    DumpLog::GetInstance().AddDesc(std::string("color: ").append(GetColor()));
    DumpLog::GetInstance().AddDesc(std::string("left: ").append(GetSwiperLeft()));
    DumpLog::GetInstance().AddDesc(std::string("top: ").append(GetSwiperTop()));
    DumpLog::GetInstance().AddDesc(std::string("right: ").append(GetSwiperRight()));
    DumpLog::GetInstance().AddDesc(std::string("bottom: ").append(GetSwiperBottom()));
    DumpLog::GetInstance().AddDesc(std::string("size: ").append(GetSwiperSize()));
}

std::unique_ptr<JsonValue> SwiperComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string SwiperComposedElement::GetIndicatorStyle() const
{
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("left", GetSwiperLeft().c_str());
    jsonValue->Put("top", GetSwiperTop().c_str());
    jsonValue->Put("right", GetSwiperRight().c_str());
    jsonValue->Put("bottom", GetSwiperBottom().c_str());
    jsonValue->Put("size", GetSwiperSize().c_str());
    jsonValue->Put("selectedColor", GetSelectedColor().c_str());
    jsonValue->Put("color", GetColor().c_str());
    return jsonValue->ToString();
}

std::string SwiperComposedElement::GetCurrentIndex() const
{
    auto renderSwiper = GetRenderSwiper();
    auto currentIndex = renderSwiper ? renderSwiper->GetCurrentIndex() : 0;
    return std::to_string(currentIndex);
}

std::string SwiperComposedElement::GetAutoPlay() const
{
    auto renderSwiper = GetRenderSwiper();
    auto autoPlay = renderSwiper ? renderSwiper->GetAutoPlay() : false;
    return ConvertBoolToString(autoPlay);
}

std::string SwiperComposedElement::GetInterval() const
{
    auto renderSwiper = GetRenderSwiper();
    auto interval = renderSwiper ? renderSwiper->GetAutoPlayInterval() : 0;
    return std::to_string(interval);
}

std::string SwiperComposedElement::GetIndicator() const
{
    auto renderSwiper = GetRenderSwiper();
    auto indicator = renderSwiper ? renderSwiper->IsShowIndicator() : true;
    return ConvertBoolToString(indicator);
}

std::string SwiperComposedElement::GetLoop() const
{
    auto renderSwiper = GetRenderSwiper();
    auto loop = renderSwiper ? renderSwiper->GetLoop() : true;
    return ConvertBoolToString(loop);
}

std::string SwiperComposedElement::GetDuration() const
{
    auto renderSwiper = GetRenderSwiper();
    auto duration = renderSwiper ? renderSwiper->GetDuration() : 0;
    return std::to_string(duration);
}

std::string SwiperComposedElement::GetVertical() const
{
    auto renderSwiper = GetRenderSwiper();
    auto isVertical = renderSwiper ? renderSwiper->IsVertical() : false;
    return ConvertBoolToString(isVertical);
}

std::string SwiperComposedElement::GetDisableSwipe() const
{
    auto renderSwiper = GetRenderSwiper();
    auto disable = renderSwiper ? renderSwiper->GetDisableSwipe() : false;
    return ConvertBoolToString(disable);
}

std::string SwiperComposedElement::GetItemSpace() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto itemspace = renderSwiper->GetItemSpace();
        return itemspace.ToString().c_str();
    }
    return "0";
}

std::string SwiperComposedElement::GetCurve() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto curve = renderSwiper->GetCurveRender();
        return curve;
    }
    return "";
}

std::string SwiperComposedElement::GetSelectedColor() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto selectedColor = renderSwiper->GetSelectedColor();
        return selectedColor.ColorToString();
    }
    return "";
}

std::string SwiperComposedElement::GetColor() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto Color = renderSwiper->GetColor();
        return Color.ColorToString();
    }
    return "";
}

std::string SwiperComposedElement::GetSwiperLeft() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto left = renderSwiper->GetSwiperLeft();
        return left.ToString();
    }
    return "";
}

std::string SwiperComposedElement::GetSwiperTop() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto top = renderSwiper->GetSwiperTop();
        return top.ToString();
    }
    return "";
}

std::string SwiperComposedElement::GetSwiperRight() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto right = renderSwiper->GetSwiperRight();
        return right.ToString();
    }
    return "";
}

std::string SwiperComposedElement::GetSwiperBottom() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto bottom = renderSwiper->GetSwiperBottom();
        return bottom.ToString();
    }
    return "";
}

std::string SwiperComposedElement::GetSwiperSize() const
{
    auto renderSwiper = GetRenderSwiper();
    if (renderSwiper) {
        auto size = renderSwiper->GetSwiperSize();
        return size.ToString();
    }
    return "";
}

RefPtr<RenderSwiper> SwiperComposedElement::GetRenderSwiper() const
{
    auto node = GetInspectorNode(SwiperElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderSwiper>(node);
    }
    return nullptr;
}

void SwiperComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto swiperElement = GetContentElement<SwiperElement>(SwiperElement::TypeId());
    if (!swiperElement) {
        LOGE("get GetSwiperElement failed");
        return;
    }
    swiperElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    swiperElement->MarkDirty();
}

void SwiperComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto swiperElement = GetContentElement<SwiperElement>(SwiperElement::TypeId());
    if (!swiperElement) {
        LOGE("get GetSwiperElement failed");
        return;
    }
    auto child = swiperElement->GetChildBySlot(slot);
    swiperElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    swiperElement->MarkDirty();
}

void SwiperComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto swiperElement = GetContentElement<SwiperElement>(SwiperElement::TypeId());
    if (!swiperElement) {
        LOGE("get GetSwiperElement failed");
        return;
    }
    auto child = swiperElement->GetChildBySlot(slot);
    swiperElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    swiperElement->MarkDirty();
}

} // namespace OHOS::Ace::V2