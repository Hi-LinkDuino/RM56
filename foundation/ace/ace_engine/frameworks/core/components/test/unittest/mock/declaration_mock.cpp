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

#include "core/components/declaration/common/declaration.h"

namespace OHOS::Ace {

Declaration::Declaration() = default;
Declaration::~Declaration() = default;
void Declaration::SetAttr(const std::vector<std::pair<std::string, std::string>>& attrs) {}
void Declaration::SetStyle(const std::vector<std::pair<std::string, std::string>>& styles) {}
void Declaration::AddEvent(int32_t pageId, const std::string& eventId, const std::vector<std::string>& events) {}
void Declaration::CallMethod(const std::string& method, const std::string& args) {}
void Declaration::OnRequestFocus(bool shouldFocus) {}
void Declaration::OnScrollBy(double dx, double dy, bool isSmooth) {}
void Declaration::SetShowAttr(const std::string& showValue) {}
void Declaration::SetCurrentStyle(const std::pair<std::string, std::string>& style) {}
void Declaration::BindPipelineContext(const WeakPtr<PipelineContext>& pipelineContext) {}
void Declaration::ResetDefaultStyles() {}
void Declaration::InitCommonAttribute() {}
void Declaration::InitCommonStyle() {}
void Declaration::InitCommonEvent() {}
void Declaration::InitCommonMethod() {}
void Declaration::AddCommonAttribute(AttributeTag tag) {}
void Declaration::AddCommonStyle(StyleTag tag) {}
void Declaration::AddCommonEvent(EventTag tag) {}
void Declaration::AddCommonMethod(MethodTag tag) {}
Attribute& Declaration::GetAttribute(AttributeTag tag) const
{
    auto it = attributes_.find(tag);
    if (it != attributes_.end()) {
        return *(it->second);
    } else {
        static Attribute errAttribute { .tag = AttributeTag::UNKNOWN };
        return errAttribute;
    }
}
Style& Declaration::GetStyle(StyleTag tag) const
{
    auto it = styles_.find(tag);
    if (it != styles_.end()) {
        return *(it->second);
    } else {
        static Style errStyle { .tag = StyleTag::UNKNOWN };
        return errStyle;
    }
}
Event& Declaration::GetEvent(EventTag tag) const
{
    auto it = events_.find(tag);
    if (it != events_.end()) {
        return *(it->second);
    } else {
        static Event errEvent { .tag = EventTag::UNKNOWN };
        return errEvent;
    }
}
Method& Declaration::GetMethod(MethodTag tag) const
{
    auto it = methods_.find(tag);
    if (it != methods_.end()) {
        return *(it->second);
    } else {
        static Method errMethod { .tag = MethodTag::UNKNOWN };
        return errMethod;
    }
}

void Declaration::AddSpecializedAttribute(std::shared_ptr<Attribute>&& specializedAttribute)
{
    attributes_.try_emplace(AttributeTag::SPECIALIZED_ATTR, std::move(specializedAttribute));
}
void Declaration::AddSpecializedStyle(std::shared_ptr<Style>&& specializedStyle)
{
    styles_.try_emplace(StyleTag::SPECIALIZED_STYLE, std::move(specializedStyle));
}
void Declaration::AddSpecializedEvent(std::shared_ptr<Event>&& specializedEvent)
{
    events_.try_emplace(EventTag::SPECIALIZED_EVENT, std::move(specializedEvent));
}
void Declaration::AddSpecializedMethod(std::shared_ptr<Method>&& specializedMethod)
{
    methods_.try_emplace(MethodTag::SPECIALIZED_METHOD, std::move(specializedMethod));
}

void Declaration::SetBackgroundImagePosition(const std::string& value, Declaration& declaration) {}
void Declaration::SetBackgroundImageSize(const std::string& value, Declaration& declaration) {}
void Declaration::SetPaddingOverall(const std::string& value, Declaration& declaration) {}
void Declaration::SetMarginOverall(const std::string& value, Declaration& declaration) {}
void Declaration::SetBorderOverall(const std::string& value, Declaration& declaration) {}
void Declaration::SetBorderWidthForFourEdges(const std::string& value, Declaration& declaration) {}
void Declaration::SetBorderColorForFourEdges(const std::string& value, Declaration& declaration) {}
void Declaration::SetBorderStyleForFourEdges(const std::string& value, Declaration& declaration) {}
void Declaration::SetBackground(const std::string& value, Declaration& declaration) {}
void Declaration::SetGradientType(const std::string& gradientType, Declaration& declaration) {}
void Declaration::SetGradientDirections(const std::unique_ptr<JsonValue>& gradientDirections, Declaration& declaration)
{}
void Declaration::SetGradientColor(const std::unique_ptr<JsonValue>& gradientColorValues, Declaration& declaration) {}
void Declaration::SetTransform(const std::string& value, Declaration& declaration) {}

std::string Declaration::GetTransformJsonValue(const std::string& value)
{
    return "";
}
std::string Declaration::GetTransformType(const std::unique_ptr<JsonValue>& transformJson)
{
    return "";
}
std::string Declaration::GetTransformTypeValue(const std::unique_ptr<JsonValue>& transformJson)
{
    return "";
}

RefPtr<ThemeManager> Declaration::GetThemeManager() const
{
    return nullptr;
}
RefPtr<ThemeConstants> Declaration::GetThemeConstants() const
{
    return nullptr;
}
Color Declaration::ParseColor(const std::string& value, uint32_t maskAlpha) const
{
    return Color();
}
double Declaration::ParseDouble(const std::string& value) const
{
    return 0.0;
}
Dimension Declaration::ParseDimension(const std::string& value, bool useVp) const
{
    return Dimension();
}
Dimension Declaration::ParseLineHeight(const std::string& value) const
{
    return Dimension();
}
std::vector<std::string> Declaration::ParseFontFamilies(const std::string& value) const
{
    std::vector<std::string> fontfamilyes;
    return fontfamilyes;
}
std::vector<Dimension> Declaration::ParsePreferFontSizes(const std::string& value) const
{
    std::vector<Dimension> fontSizes;
    return fontSizes;
}

std::string Declaration::ParseImageSrc(const std::string& imgSrc) const
{
    return imgSrc;
}

bool Declaration::IsRightToLeft() const
{
    return false;
}

void Declaration::Init()
{
    InitSpecialized();
}

} // namespace OHOS::Ace
