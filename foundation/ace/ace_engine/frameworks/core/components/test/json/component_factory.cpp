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

#include "core/components/test/json/component_factory.h"

#include <map>

#include "base/log/log.h"
#include "base/utils/singleton.h"
#include "core/components/test/json/align_creator.h"
#include "core/components/test/json/box_creator.h"
#include "core/components/test/json/button_creator.h"
#include "core/components/test/json/checkable_creator.h"
#include "core/components/test/json/clip_creator.h"
#include "core/components/test/json/compose_creator.h"
#include "core/components/test/json/dialog_creator.h"
#include "core/components/test/json/display_creator.h"
#include "core/components/test/json/divider_creator.h"
#include "core/components/test/json/flex_creator.h"
#include "core/components/test/json/flex_item_creator.h"
#include "core/components/test/json/focusable_creator.h"
#include "core/components/test/json/gesture_listener_creator.h"
#include "core/components/test/json/grid_creator.h"
#include "core/components/test/json/grid_item_creator.h"
#include "core/components/test/json/grid_layout_creator.h"
#include "core/components/test/json/grid_layout_item_creator.h"
#include "core/components/test/json/image_creator.h"
#include "core/components/test/json/list_creator.h"
#include "core/components/test/json/list_item_creator.h"
#include "core/components/test/json/loading_progress_creator.h"
#include "core/components/test/json/padding_creator.h"
#include "core/components/test/json/positioned_creator.h"
#include "core/components/test/json/progress_creator.h"
#include "core/components/test/json/scroll_creator.h"
#include "core/components/test/json/slider_creator.h"
#include "core/components/test/json/stack_creator.h"
#include "core/components/test/json/swiper_creator.h"
#include "core/components/test/json/text_creator.h"
#include "core/components/test/json/text_span_creator.h"
#include "core/components/test/json/touch_listener_creator.h"
#include "core/components/test/json/transform_creator.h"
#include "core/components/test/json/tween_creator.h"
#include "core/components/test/json/wrap_creator.h"
#include "core/pipeline/base/constants.h"

#ifndef WEARABLE_PRODUCT
#include "core/components/test/json/marquee_creator.h"
#include "core/components/test/json/popup_creator.h"
#include "core/components/test/json/rating_creator.h"
#include "core/components/test/json/select_creator.h"
#include "core/components/test/json/tab_bar_creator.h"
#include "core/components/test/json/tab_content_creator.h"
#include "core/components/test/json/text_field_creator.h"
#include "core/components/test/json/texture_creator.h"
#include "core/components/test/json/video_creator.h"
#endif

namespace OHOS::Ace {
namespace {

class JsonComponentFactoryImpl : public JsonComponentFactory {
public:
    using CreateComponentFunc = std::function<RefPtr<Component>(const JsonValue&, const JsonComponentFactory&)>;
    using FactoryMap = std::map<std::string, CreateComponentFunc>;

    RefPtr<Component> CreateComponentFromDsl(const uint8_t* dslBuf, size_t dslLen) final;
    std::list<RefPtr<Component>> CreateComponentsFromDsl(const uint8_t* dslBuf, size_t dslLen) final;
    RefPtr<Component> CreateComponent(const JsonValue& jsonValue) const final;
    std::list<RefPtr<Component>> CreateChildComponent(const JsonValue& jsonValue) const final;
    std::list<RefPtr<Component>> CreateComponents(const JsonValue& jsonValue) const;

private:
    std::unique_ptr<JsonValue> ParseDslData(const uint8_t* dslBuf, size_t dslLen) const;
};

const JsonComponentFactoryImpl::FactoryMap FACTORY_MAP = {
    { BOX_NAME, BoxCreator::CreateFromJson },
    { COLUMN_NAME, ColumnCreator::CreateFromJson },
    { COMPOSE_NAME, ComposeCreator::CreateFromJson },
    { CHECKBOX_NAME, CheckableCreator::CreateFromJson },
    { SWITCH_NAME, CheckableCreator::CreateFromJson },
    { RADIO_NAME, CheckableCreator::CreateFromJson },
    { IMAGE_NAME, ImageCreator::CreateFromJson },
    { ROW_NAME, RowCreator::CreateFromJson },
    { TEXT_NAME, TextCreator::CreateFromJson },
    { TEXT_SPAN_NAME, TextSpanCreator::CreateFromJson },
    { LIST_NAME, ListCreator::CreateFromJson },
    { LIST_ITEM_NAME, ListItemCreator::CreateFromJson },
    { GRID_NAME, GridCreator::CreateFromJson },
    { GRID_ITEM_NAME, GridItemCreator::CreateFromJson },
    { GRID_LAYOUT_NAME, GridLayoutCreator::CreateFromJson },
    { GRID_LAYOUT_ITEM_NAME, GridLayoutItemCreator::CreateFromJson },
    { PADDING_NAME, PaddingCreator::CreateFromJson },
    { ALIGN_NAME, AlignCreator::CreateFromJson },
    { TWEEN_NAME, TweenCreator::CreateFromJson },
    { SLIDER_NAME, SliderCreator::CreateFromJson },
    { SWIPER_NAME, SwiperCreator::CreateFromJson },
    { FLEX_ITEM_NAME, FlexItemCreator::CreateFromJson },
    { BUTTON_NAME, ButtonCreator::CreateFromJson },
    { TOUCH_LISTENER_NAME, TouchListenerCreator::CreateFromJson },
    { GESTURE_LISTENER_NAME, GestureListenerCreator::CreateFromJson },
    { DISPLAY_NAME, DisplayCreator::CreateFromJson },
    { SCROLL_NAME, ScrollCreator::CreateFromJson },
    { DIALOG_NAME, DialogCreator::CreateFromJson },
    { PROGRESS_NAME, ProgressCreator::CreateFromJson },
    { STACK_NAME, StackCreator::CreateFromJson },
    { POSITIONED_NAME, PositionedCreator::CreateFromJson },
    { FOCUSABLE_NAME, FocusableCreator::CreateFromJson },
    { TRANSFORM_NAME, TransformCreator::CreateFromJson },
    { WRAP_NAME, WrapCreator::CreateFromJson },
    { LOADING_PROGRESS_NAME, LoadingProgressCreator::CreateFromJson },
    { CLIP_NAME, ClipCreator::CreateFromJson },
    { DIVIDER_NAME, DividerCreator::CreateFromJson },
#ifndef WEARABLE_PRODUCT
    { SELECT_NAME, SelectCreator::CreateFromJson },
    { TAB_BAR_NAME, TabBarCreator::CreateFromJson },
    { TAB_CONTENT_NAME, TabContentCreator::CreateFromJson },
    { POPUP_NAME, PopupCreator::CreateFromJson },
    { RATING_NAME, RatingCreator::CreateFromJson },
    { MARQUEE_NAME, MarqueeCreator::CreateFromJson },
    { VIDEO_NAME, VideoCreator::CreateFromJson },
    { TEXTURE_NAME, TextureCreator::CreateFromJson },
    { TEXT_FIELD_NAME, TextFieldCreator::CreateFromJson },
#endif
};

RefPtr<Component> JsonComponentFactoryImpl::CreateComponentFromDsl(const uint8_t* dslBuf, size_t dslLen)
{
    auto componentJson = ParseDslData(dslBuf, dslLen);
    if (componentJson == nullptr) {
        LOGE("parse dsl data failed");
        return nullptr;
    }
    return CreateComponent(*componentJson);
}

std::list<RefPtr<Component>> JsonComponentFactoryImpl::CreateComponentsFromDsl(const uint8_t* dslBuf, size_t dslLen)
{
    auto componentJson = ParseDslData(dslBuf, dslLen);
    if (componentJson == nullptr) {
        LOGE("parse dsl data failed");
        return std::list<RefPtr<Component>>();
    }
    return CreateComponents(*componentJson);
}

std::unique_ptr<JsonValue> JsonComponentFactoryImpl::ParseDslData(const uint8_t* dslBuf, size_t dslLen) const
{
    std::string componentJsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    const char* endMsg = nullptr;
    std::unique_ptr<JsonValue> componentJson = JsonUtil::ParseJsonString(componentJsonStr, &endMsg);
    if (componentJson == nullptr) {
        LOGE("parse json error %{private}s:", endMsg);
        return nullptr;
    }
    return componentJson;
}

RefPtr<Component> JsonComponentFactoryImpl::CreateComponent(const JsonValue& jsonValue) const
{
    if (!jsonValue.Contains(CLASS_NAME)) {
        LOGE("not have className field.");
        return nullptr;
    }

    auto typeValue = jsonValue.GetValue(CLASS_NAME);
    std::string type = typeValue->GetString();
    const auto& it = FACTORY_MAP.find(type);
    if (it == FACTORY_MAP.end()) {
        LOGE("can't create component for type:%{public}s", type.c_str());
        return nullptr;
    }

    const CreateComponentFunc& creator = it->second;
    return creator(jsonValue, *this);
}

std::list<RefPtr<Component>> JsonComponentFactoryImpl::CreateChildComponent(const JsonValue& jsonValue) const
{
    if (!jsonValue.Contains(CHILD)) {
        return std::list<RefPtr<Component>>();
    }

    auto childValue = jsonValue.GetValue(CHILD);
    if (!childValue) {
        return std::list<RefPtr<Component>>();
    }

    return CreateComponents(*childValue);
}

std::list<RefPtr<Component>> JsonComponentFactoryImpl::CreateComponents(const JsonValue& jsonValue) const
{
    std::list<RefPtr<Component>> children;
    if (!jsonValue.IsArray()) {
        auto child = CreateComponent(jsonValue);
        if (child) {
            children.emplace_back(child);
        }
        return children;
    }

    int32_t arraySize = jsonValue.GetArraySize();
    for (int32_t index = 0; index < arraySize; ++index) {
        auto item = jsonValue.GetArrayItem(index);
        auto child = CreateComponent(*item);
        if (child) {
            children.emplace_back(child);
        }
    }
    return children;
}

} // namespace

ComponentFactory& ComponentFactory::GetInstance()
{
    return Singleton<JsonComponentFactoryImpl>::GetInstance();
}

} // namespace OHOS::Ace
