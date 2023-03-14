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

#include "frameworks/bridge/declarative_frontend/jsview/js_grid.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "core/common/ace_application_info.h"
#include "core/components_v2/grid/render_grid_scroll.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_scroller.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

const std::vector<DisplayMode> DISPLAY_MODE = { DisplayMode::OFF, DisplayMode::AUTO, DisplayMode::ON };
const std::vector<EdgeEffect> EDGE_EFFECT = { EdgeEffect::SPRING, EdgeEffect::FADE, EdgeEffect::NONE };
const std::vector<FlexDirection> LAYOUT_DIRECTION = { FlexDirection::ROW, FlexDirection::COLUMN,
    FlexDirection::ROW_REVERSE, FlexDirection::COLUMN_REVERSE };

} // namespace

void JSGrid::Create(const JSCallbackInfo& info)
{
    LOGD("Create component: Grid");
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;

    RefPtr<OHOS::Ace::GridLayoutComponent> gridComponent = AceType::MakeRefPtr<GridLayoutComponent>(componentChildren);
    gridComponent->SetDeclarative();
    gridComponent->SetNeedShrink(true);
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSScroller* jsScroller = JSRef<JSObject>::Cast(info[0])->Unwrap<JSScroller>();
        if (jsScroller) {
            auto positionController = AceType::MakeRefPtr<V2::GridPositionController>();
            jsScroller->SetController(positionController);
            gridComponent->SetController(positionController);

            // Init scroll bar proxy.
            auto proxy = jsScroller->GetScrollBarProxy();
            if (!proxy) {
                proxy = AceType::MakeRefPtr<ScrollBarProxy>();
                jsScroller->SetScrollBarProxy(proxy);
            }
            gridComponent->SetScrollBarProxy(proxy);
        }
    }
    ViewStackProcessor::GetInstance()->Push(gridComponent);
}

void JSGrid::SetColumnsTemplate(const std::string& value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetColumnsArgs(value);
    }
}

void JSGrid::SetRowsTemplate(const std::string& value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetRowsArgs(value);
    }
}

void JSGrid::SetColumnsGap(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension colGap;
    if (!ParseJsDimensionVp(info[0], colGap)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetColumnGap(colGap);
    }
}

void JSGrid::SetRowsGap(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension rowGap;
    if (!ParseJsDimensionVp(info[0], rowGap)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetRowGap(rowGap);
    }
}

void JSGrid::JsGridHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    JSViewAbstract::JsHeight(info);
    Dimension height;
    if (!ParseJsDimensionVp(info[0], height)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid && height.IsValid()) {
        grid->SetNeedShrink(false);
    }
}


void JSGrid::JsOnScrollIndex(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        auto onScrolled = EventMarker(
            [execCtx = info.GetExecutionContext(), func = JSRef<JSFunc>::Cast(info[0])](const BaseEventInfo* event) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<V2::GridEventInfo>(event);
                if (!eventInfo) {
                    return;
                }
                auto params = ConvertToJSValues(eventInfo->GetScrollIndex());
                func->Call(JSRef<JSObject>(), params.size(), params.data());
            });

        auto grid = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (grid) {
            grid->SetScrolledEvent(onScrolled);
        }
    }
}

void JSGrid::JSBind(BindingTarget globalObj)
{
    LOGD("JSGrid:V8Bind");
    JSClass<JSGrid>::Declare("Grid");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSGrid>::StaticMethod("create", &JSGrid::Create, opt);
    JSClass<JSGrid>::StaticMethod("columnsTemplate", &JSGrid::SetColumnsTemplate, opt);
    JSClass<JSGrid>::StaticMethod("rowsTemplate", &JSGrid::SetRowsTemplate, opt);
    JSClass<JSGrid>::StaticMethod("columnsGap", &JSGrid::SetColumnsGap, opt);
    JSClass<JSGrid>::StaticMethod("rowsGap", &JSGrid::SetRowsGap, opt);
    JSClass<JSGrid>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSGrid>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSGrid>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSGrid>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSGrid>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSGrid>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSGrid>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSGrid>::StaticMethod("scrollBar", &JSGrid::SetScrollBar, opt);
    JSClass<JSGrid>::StaticMethod("scrollBarWidth", &JSGrid::SetScrollBarWidth, opt);
    JSClass<JSGrid>::StaticMethod("scrollBarColor", &JSGrid::SetScrollBarColor, opt);
    JSClass<JSGrid>::StaticMethod("onScrollIndex", &JSGrid::JsOnScrollIndex);
    JSClass<JSGrid>::StaticMethod("cachedCount", &JSGrid::SetCachedCount);
    JSClass<JSGrid>::StaticMethod("editMode", &JSGrid::SetEditMode, opt);
    JSClass<JSGrid>::StaticMethod("multiSelectable", &JSGrid::SetMultiSelectable, opt);
    JSClass<JSGrid>::StaticMethod("maxCount", &JSGrid::SetMaxCount, opt);
    JSClass<JSGrid>::StaticMethod("minCount", &JSGrid::SetMinCount, opt);
    JSClass<JSGrid>::StaticMethod("cellLength", &JSGrid::CellLength, opt);
    JSClass<JSGrid>::StaticMethod("layoutDirection", &JSGrid::SetLayoutDirection, opt);
    JSClass<JSGrid>::StaticMethod("dragAnimation", &JSGrid::SetDragAnimation, opt);
    JSClass<JSGrid>::StaticMethod("edgeEffection", &JSGrid::SetEdgeEffection, opt);
    JSClass<JSGrid>::StaticMethod("direction", &JSGrid::SetDirection, opt);
    JSClass<JSGrid>::StaticMethod("supportAnimation", &JSGrid::SetSupportAnimation, opt);
    JSClass<JSGrid>::StaticMethod("onItemDragEnter", &JSGrid::JsOnGridDragEnter);
    JSClass<JSGrid>::StaticMethod("onItemDragMove", &JSGrid::JsOnGridDragMove);
    JSClass<JSGrid>::StaticMethod("onItemDragLeave", &JSGrid::JsOnGridDragLeave);
    JSClass<JSGrid>::StaticMethod("onItemDragStart", &JSGrid::JsOnGridDragStart);
    JSClass<JSGrid>::StaticMethod("height", &JSGrid::JsGridHeight);
    JSClass<JSGrid>::StaticMethod("onItemDrop", &JSGrid::JsOnGridDrop);
    JSClass<JSGrid>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSGrid>::Inherit<JSContainerBase>();
    JSClass<JSGrid>::Inherit<JSViewAbstract>();
    JSClass<JSGrid>::Bind<>(globalObj);
}

void JSGrid::SetScrollBar(int32_t displayMode)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (!grid) {
        return;
    }
    if (displayMode >= 0 && displayMode < static_cast<int32_t>(DISPLAY_MODE.size())) {
        grid->SetScrollBar(DISPLAY_MODE[displayMode]);
    }
}

void JSGrid::SetScrollBarColor(const std::string& color)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetScrollBarColor(color);
    }
}

void JSGrid::SetScrollBarWidth(const std::string& width)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetScrollBarWidth(width);
    }
}

void JSGrid::SetCachedCount(int32_t cachedCount)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetCachedCount(cachedCount);
    }
}

void JSGrid::SetEditMode(bool editMode)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetEditMode(editMode);
    }
}

void JSGrid::SetMaxCount(int32_t maxCount)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetMaxCount(maxCount);
    }
}

void JSGrid::SetMinCount(int32_t minCount)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetMinCount(minCount);
    }
}

void JSGrid::CellLength(int32_t cellLength)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetCellLength(cellLength);
    }
}

void JSGrid::SetSupportAnimation(bool supportAnimation)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetSupportAnimation(supportAnimation);
    }
}

void JSGrid::SetDragAnimation(bool value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetDragAnimation(value);
    }
}

void JSGrid::SetEdgeEffection(int32_t value)
{
    if (value >= 0 && value < static_cast<int32_t>(EDGE_EFFECT.size())) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
        if (grid) {
            grid->SetEdgeEffection(EDGE_EFFECT[value]);
        }
    }
}

void JSGrid::SetLayoutDirection(int32_t value)
{
    if (value >= 0 && value < static_cast<int32_t>(LAYOUT_DIRECTION.size())) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
        if (grid) {
            grid->SetDirection(LAYOUT_DIRECTION[value]);
        }
    }
}

void JSGrid::SetDirection(const std::string& dir)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        bool rightToLeft = false;
        if (dir == "Ltr") {
            rightToLeft = false;
        } else if (dir == "Rtl") {
            rightToLeft = true;
        } else {
            rightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
        }
        grid->SetRightToLeft(rightToLeft);
    }
}

void JSGrid::JsOnGridDragEnter(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragEnter event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragEnterFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragEnterId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragEnterFunc)](
                                const ItemDragInfo& dragInfo) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Grid.onItemDragEnter");
        func->ItemDragEnterExecute(dragInfo);
    };
    auto component = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutComponent>());
        return;
    }
    component->SetOnGridDragEnterId(onItemDragEnterId);
}

void JSGrid::JsOnGridDragMove(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragMove event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragMoveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragMoveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragMoveFunc)](
                            const ItemDragInfo& dragInfo, int32_t itemIndex, int32_t insertIndex) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Grid.onItemDragMove");
        func->ItemDragMoveExecute(dragInfo, itemIndex, insertIndex);
    };
    auto component = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutComponent>());
        return;
    }
    component->SetOnGridDragMoveId(onItemDragMoveId);
}

void JSGrid::JsOnGridDragLeave(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragLeave event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragLeaveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragLeaveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragLeaveFunc)](
                                const ItemDragInfo& dragInfo, int32_t itemIndex) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Grid.onItemDragLeave");
        func->ItemDragLeaveExecute(dragInfo, itemIndex);
    };
    auto component = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutComponent>());
        return;
    }
    component->SetOnGridDragLeaveId(onItemDragLeaveId);
}

void JSGrid::JsOnGridDragStart(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDragStart event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDragFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDragStartId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragFunc)](
                        const ItemDragInfo& dragInfo, int32_t itemIndex) -> RefPtr<Component> {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, nullptr);
        auto ret = func->ItemDragStartExecute(dragInfo, itemIndex);
        if (!ret->IsObject()) {
            LOGE("builder param is not an object.");
            return nullptr;
        }

        auto builderObj = JSRef<JSObject>::Cast(ret);
        auto builder = builderObj->GetProperty("builder");
        if (!builder->IsFunction()) {
            LOGE("builder param is not a function.");
            return nullptr;
        }
        auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
        if (!builderFunc) {
            LOGE("builder function is null.");
            return nullptr;
        }
        // use another VSP instance while executing the builder function
        ScopedViewStackProcessor builderViewStackProcessor;
        {
            ACE_SCORING_EVENT("Grid.onItemDragStart.builder");
            builderFunc->Execute();
        }
        RefPtr<Component> customComponent = ViewStackProcessor::GetInstance()->Finish();
        if (!customComponent) {
            LOGE("Custom component is null.");
            return nullptr;
        }
        return customComponent;
    };
    auto component = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutComponent>());
        return;
    }
    component->SetOnGridDragStartId(onItemDragStartId);
}

void JSGrid::JsOnGridDrop(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("fail to bind onItemDrop event due to info is not function");
        return;
    }

    RefPtr<JsDragFunction> jsOnDropFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onItemDropId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDropFunc)](
                        const ItemDragInfo& dragInfo, int32_t itemIndex, int32_t insertIndex, bool isSuccess) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Grid.onItemDrop");
        func->ItemDropExecute(dragInfo, itemIndex, insertIndex, isSuccess);
    };
    auto component = AceType::DynamicCast<GridLayoutComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGW("Failed to get '%{public}s' in view stack", AceType::TypeName<GridLayoutComponent>());
        return;
    }
    component->SetOnGridDropId(onItemDropId);
}

void JSGrid::SetMultiSelectable(bool multiSelectable)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto grid = AceType::DynamicCast<GridLayoutComponent>(component);
    if (grid) {
        grid->SetMultiSelectable(multiSelectable);
    }
}

} // namespace OHOS::Ace::Framework
