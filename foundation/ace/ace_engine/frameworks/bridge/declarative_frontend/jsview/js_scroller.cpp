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

#include "bridge/declarative_frontend/jsview/js_scroller.h"

#include "base/geometry/axis.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "core/animation/curves.h"
#include "core/components/common/layout/align_declaration.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Axis DIRECTION_TABLE[] = { Axis::VERTICAL, Axis::HORIZONTAL };

constexpr AlignDeclaration::Edge EDGE_TABLE[] = {
    AlignDeclaration::Edge::TOP,
    AlignDeclaration::Edge::CENTER,
    AlignDeclaration::Edge::BOTTOM,
    AlignDeclaration::Edge::BASELINE,
    AlignDeclaration::Edge::START,
    AlignDeclaration::Edge::MIDDLE,
    AlignDeclaration::Edge::END,
};

// corresponding to EDGE_TABLE[]
constexpr ScrollEdgeType EDGE_TYPE_TABLE[] = { ScrollEdgeType::SCROLL_TOP, ScrollEdgeType::SCROLL_NONE,
    ScrollEdgeType::SCROLL_BOTTOM, ScrollEdgeType::SCROLL_NONE, ScrollEdgeType::SCROLL_TOP, ScrollEdgeType::SCROLL_NONE,
    ScrollEdgeType::SCROLL_BOTTOM };

const LinearMapNode<RefPtr<Curve>> CURVE_MAP[] = {
    { "ease", Curves::EASE },
    { "ease-in", Curves::EASE_IN },
    { "ease-in-out", Curves::EASE_IN_OUT },
    { "ease-out", Curves::EASE_OUT },
    { "friction", Curves::FRICTION },
    { "linear", Curves::LINEAR },
};

} // namespace

void JSScroller::JSBind(BindingTarget globalObj)
{
    JSClass<JSScroller>::Declare("Scroller");
    JSClass<JSScroller>::CustomMethod("scrollTo", &JSScroller::ScrollTo);
    JSClass<JSScroller>::CustomMethod("scrollEdge", &JSScroller::ScrollEdge);
    JSClass<JSScroller>::CustomMethod("scrollPage", &JSScroller::ScrollPage);
    JSClass<JSScroller>::CustomMethod("currentOffset", &JSScroller::CurrentOffset);
    JSClass<JSScroller>::CustomMethod("scrollToIndex", &JSScroller::ScrollToIndex);
    JSClass<JSScroller>::Bind(globalObj, JSScroller::Constructor, JSScroller::Destructor);
}

void JSScroller::Constructor(const JSCallbackInfo& args)
{
    auto scroller = Referenced::MakeRefPtr<JSScroller>();
    scroller->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(scroller));
}

void JSScroller::Destructor(JSScroller* scroller)
{
    if (scroller != nullptr) {
        scroller->DecRefCount();
    }
}

void JSScroller::ScrollTo(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    Dimension xOffset;
    Dimension yOffset;
    if (!ConvertFromJSValue(obj->GetProperty("xOffset"), xOffset) ||
        !ConvertFromJSValue(obj->GetProperty("yOffset"), yOffset)) {
        LOGW("Failed to parse param 'xOffset' or 'yOffset'");
        return;
    }

    double duration = 0.0;
    RefPtr<Curve> curve = Curves::EASE;
    auto animationValue = obj->GetProperty("animation");
    if (animationValue->IsObject()) {
        auto animationObj = JSRef<JSObject>::Cast(animationValue);
        ConvertFromJSValue(animationObj->GetProperty("duration"), duration);

        std::string curveName;
        if (ConvertFromJSValue(animationObj->GetProperty("curve"), curveName)) {
            auto index = BinarySearchFindIndex(CURVE_MAP, ArraySize(CURVE_MAP), curveName.c_str());
            if (index >= 0) {
                curve = CURVE_MAP[index].value;
            }
        }
    }

    if (GreatNotEqual(duration, 0.0)) {
        LOGD("ScrollTo(%lf, %lf, %lf)", xOffset.Value(), yOffset.Value(), duration);
    } else {
        LOGD("ScrollTo(%lf, %lf)", xOffset.Value(), yOffset.Value());
    }
    if (!controller_) {
        LOGE("controller_ is nullptr");
        return;
    }
    auto direction = controller_->GetScrollDirection();
    auto position = direction == Axis::VERTICAL ? yOffset : xOffset;
    controller_->AnimateTo(position, duration, curve);
}

void JSScroller::ScrollEdge(const JSCallbackInfo& args)
{
    AlignDeclaration::Edge edge = AlignDeclaration::Edge::AUTO;
    if (args.Length() < 1 || !ConvertFromJSValue(args[0], EDGE_TABLE, edge)) {
        LOGW("Invalid params");
        return;
    }
    if (!controller_) {
        LOGE("controller_ is nullptr");
        return;
    }
    LOGD("ScrollEdge(%{public}d)", static_cast<int32_t>(edge));
    ScrollEdgeType edgeType = EDGE_TYPE_TABLE[static_cast<int32_t>(edge)];
    controller_->ScrollToEdge(edgeType, true);
}

void JSScroller::ScrollToIndex(const JSCallbackInfo& args)
{
    int32_t index = 0;
    if (args.Length() < 1 || !ConvertFromJSValue(args[0], index)) {
        LOGW("Invalid params");
        return;
    }
    if (!controller_) {
        LOGE("controller_ is nullptr");
        return;
    }
    controller_->JumpTo(index, SCROLL_FROM_JUMP);
}

void JSScroller::ScrollPage(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("Invalid params");
        return;
    }

    auto obj = JSRef<JSObject>::Cast(args[0]);
    bool next = true;
    if (!ConvertFromJSValue(obj->GetProperty("next"), next)) {
        LOGW("Failed to parse param 'next'");
        return;
    }

    Axis direction = Axis::NONE;
    ConvertFromJSValue(obj->GetProperty("direction"), DIRECTION_TABLE, direction);
    if (!controller_) {
        LOGE("controller_ is nullptr");
        return;
    }
    LOGD("ScrollPage(%{public}s, %{public}d)", next ? "true" : "false", static_cast<int32_t>(direction));
    controller_->ScrollPage(!next, true);
}

void JSScroller::CurrentOffset(const JSCallbackInfo& args)
{
    LOGD("CurrentOffset()");
    if (!controller_) {
        LOGE("controller_ is nullptr");
        return;
    }
    auto retObj = JSRef<JSObject>::New();
    auto offset = controller_->GetCurrentOffset();
    retObj->SetProperty("xOffset", offset.GetX());
    retObj->SetProperty("yOffset", offset.GetY());
    args.SetReturnValue(retObj);
}

} // namespace OHOS::Ace::Framework
