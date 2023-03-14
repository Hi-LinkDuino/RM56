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

#include "core/components/box/render_box.h"

#include <algorithm>
#include <cinttypes>
#include <cstdint>

#include "base/geometry/offset.h"
#include "base/log/event_report.h"
#include "base/utils/utils.h"
#include "core/animation/property_animatable_helper.h"
#include "core/common/clipboard/clipboard_proxy.h"
#include "core/components/box/box_component.h"
#include "core/components/box/box_component_helper.h"
#include "core/components/root/root_element.h"
#include "core/components/text_field/render_text_field.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/components_v2/list/render_list.h"
#include "core/event/axis_event.h"
#include "core/event/mouse_event.h"
#include "core/gestures/exclusive_recognizer.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pan_recognizer.h"
#include "core/gestures/parallel_recognizer.h"
#include "core/gestures/sequenced_recognizer.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t HOVER_ANIMATION_DURATION = 250;
constexpr int32_t DEFAULT_FINGERS = 1;
constexpr int32_t DEFAULT_DURATION = 150;
constexpr int32_t DEFAULT_DISTANCE = 0;

}; // namespace

void RenderBox::HandleAccessibilityFocusEvent(bool isAccessibilityFocus)
{
    LOGD("ACE: RenderAccessibilityFocus::HandleAccessibilityFocusEvent, isFocus:%{public}d", isAccessibilityFocus);
    isAccessibilityFocus_ = isAccessibilityFocus;
    std::string accessibilityEventType;
    if (isAccessibilityFocus) {
        accessibilityEventType = "accessibilityfocus";
    } else {
        accessibilityEventType = "accessibilityclearfocus";
    }
    SendAccessibilityEvent(accessibilityEventType);
    MarkNeedRender();
}

Size RenderBox::GetBorderSize() const
{
    auto context = GetContext().Upgrade();
    if (backDecoration_ && context) {
        return backDecoration_->GetBorder().GetLayoutSize(context->GetDipScale());
    }
    return Size(0.0, 0.0);
}

Offset RenderBox::GetBorderOffset() const
{
    auto context = GetContext().Upgrade();
    if (backDecoration_ && context) {
        return backDecoration_->GetBorder().GetOffset(context->GetDipScale());
    }
    return Offset(0.0, 0.0);
}

Radius RenderBox::GetBorderRadius() const
{
    if (backDecoration_) {
        return backDecoration_->GetBorder().TopLeftRadius();
    }
    return Radius();
}

void RenderBox::Update(const RefPtr<Component>& component)
{
    const RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(component);
    if (box) {
        boxComponent_ = box;
        inspectorDirection_ = box->GetInspectorDirection();
        RenderBoxBase::Update(component);
        UpdateBackDecoration(box->GetBackDecoration());
        UpdateFrontDecoration(box->GetFrontDecoration());
        hoverColorBegin_ = box->GetColor();
        animationType_ = box->GetMouseAnimationType();
        hoverAnimationType_ = animationType_;
        isZoom = animationType_ == HoverAnimationType::SCALE;
        MarkNeedLayout();

        responseRegion_ = box->GetResponseRegion();
        isResponseRegion_ = box->IsResponseRegion();

        auto tapGesture = box->GetOnClick();
        if (tapGesture) {
            onClick_ = tapGesture->CreateRecognizer(context_);
            onClick_->SetIsExternalGesture(true);
        }
        if (!box->GetRemoteMessageEvent().IsEmpty() && !tapGesture) {
            onClick_ = AceType::MakeRefPtr<ClickRecognizer>();
        }
        auto clickRecognizer = AceType::DynamicCast<ClickRecognizer>(onClick_);
        if (clickRecognizer) {
            auto weak = AceType::WeakClaim(this);
            clickRecognizer->SetRemoteMessage([weak](const ClickInfo& info) {
                auto client = weak.Upgrade();
                if (client) {
                    client->HandleRemoteMessage(info);
                }
            });
        }
        auto longPressGesture = box->GetOnLongPress();
        if (longPressGesture) {
            onLongPress_ = longPressGesture->CreateRecognizer(context_);
            onLongPress_->SetIsExternalGesture(true);
        }

        onDragStart_ = box->GetOnDragStartId();
        onDragEnter_ = box->GetOnDragEnterId();
        onDragMove_ = box->GetOnDragMoveId();
        onDragLeave_ = box->GetOnDragLeaveId();
        onDrop_ = box->GetOnDropId();
        if (onDragStart_) {
            CreateDragDropRecognizer();
        }

        if (!box->GetOnDomDragEnter().IsEmpty()) {
            onDomDragEnter_ = AceAsyncEvent<void(const DragUpdateInfo&)>::Create(box->GetOnDomDragEnter(), context_);
        }
        if (!box->GetOnDomDragOver().IsEmpty()) {
            onDomDragOver_ = AceAsyncEvent<void(const DragUpdateInfo&)>::Create(box->GetOnDomDragOver(), context_);
        }
        if (!box->GetOnDomDragLeave().IsEmpty()) {
            onDomDragLeave_ = AceAsyncEvent<void(const DragUpdateInfo&)>::Create(box->GetOnDomDragLeave(), context_);
        }
        if (!box->GetOnDomDragDrop().IsEmpty()) {
            onDomDragDrop_ = AceAsyncEvent<void(const DragEndInfo&)>::Create(box->GetOnDomDragDrop(), context_);
        }
        if (!box->GetRemoteMessageEvent().IsEmpty()) {
            remoteMessageEvent_ = AceAsyncEvent<void(const std::shared_ptr<ClickInfo>&)>::Create(
                box->GetRemoteMessageEvent(), context_);
        }

        auto context = GetContext().Upgrade();
        if (onDragStart_ || onDrop_) {
            context->InitDragListener();
        }

        onHover_ = box->GetOnHoverId();
        onMouse_ = box->GetOnMouseId();
        onLongPressId_ = box->GetOnLongPress();

        UpdateGestureRecognizerHierarchy(box->GetGestureHierarchy());
        SetAccessibilityFocusImpl();

        if (box->HasStateAttributes()) {
            stateAttributeList_ = box->GetStateAttributes();
        }
        OnStatusStyleChanged(disabled_ ? VisualState::DISABLED : VisualState::NORMAL);

        onTouchUpId_ = box->GetOnTouchUpId();
        onTouchDownId_ = box->GetOnTouchDownId();
        onTouchMoveId_ = box->GetOnTouchMoveId();
        auto wp = AceType::WeakClaim(this);
        touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        touchRecognizer_->SetOnTouchDown([wp](const TouchEventInfo& touchInfo) {
            auto box = wp.Upgrade();
            if (!box) {
                return;
            }
            box->HandleTouchEvent(true);
            if (box->onTouchDownId_) {
                box->onTouchDownId_(touchInfo);
            }
        });
        touchRecognizer_->SetOnTouchUp([wp](const TouchEventInfo& touchInfo) {
            auto box = wp.Upgrade();
            if (!box) {
                return;
            }
            box->HandleTouchEvent(false);
            if (box->onTouchUpId_) {
                box->onTouchUpId_(touchInfo);
            }
        });
        touchRecognizer_->SetOnTouchMove(onTouchMoveId_);
    }
    // In each update, the extensions will be updated with new one.
    if (eventExtensions_ && eventExtensions_->HasOnAreaChangeExtension()) {
        auto inspector = inspector_.Upgrade();
        if (inspector) {
            auto area = inspector->GetCurrentRectAndOrigin();
            eventExtensions_->GetOnAreaChangeExtension()->SetBase(area.first, area.second);
        }
    }
}

void RenderBox::HandleTouchEvent(bool isTouchDown)
{
    if (isTouchDown) {
        OnStatusStyleChanged(VisualState::PRESSED);
    } else {
        OnStatusStyleChanged(VisualState::NORMAL);
    }
}

void RenderBox::SetAccessibilityFocusImpl()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }
    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionAccessibilityFocusImpl([weakPtr](bool isFocus) {
        auto accessibilityFocus = weakPtr.Upgrade();
        if (accessibilityFocus) {
            accessibilityFocus->HandleAccessibilityFocusEvent(isFocus);
        }
    });
}

void RenderBox::CreateDragDropRecognizer()
{
    if (dragDropGesture_) {
        return;
    }

    auto context = GetContext();
    auto longPressRecognizer =
        AceType::MakeRefPtr<OHOS::Ace::LongPressRecognizer>(context, DEFAULT_DURATION, DEFAULT_FINGERS, false);
    PanDirection panDirection;
    auto panRecognizer =
        AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, DEFAULT_FINGERS, panDirection, DEFAULT_DISTANCE);
    panRecognizer->SetOnActionStart(std::bind(&RenderBox::PanOnActionStart, this, std::placeholders::_1));
    panRecognizer->SetOnActionUpdate(std::bind(&RenderBox::PanOnActionUpdate, this, std::placeholders::_1));
    panRecognizer->SetOnActionEnd(std::bind(&RenderBox::PanOnActionEnd, this, std::placeholders::_1));
    panRecognizer->SetOnActionCancel([weakRenderBox = AceType::WeakClaim<RenderBox>(this), context = context_]() {
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGE("Context is null.");
            return;
        }

        auto renderBox = weakRenderBox.Upgrade();
        if (!renderBox) {
            LOGE("RenderBox is null.");
            return;
        }

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        if (renderBox->isDragRenderBox_) {
            renderBox->isDragRenderBox_ = false;
        }

        if (renderBox->dragWindow_) {
            renderBox->dragWindow_->Destory();
            renderBox->dragWindow_ = nullptr;
        }
#endif
        if (renderBox->hasDragItem_) {
            auto stackElement = pipelineContext->GetLastStack();
            stackElement->PopComponent();
            renderBox->hasDragItem_ = false;
        }
        renderBox->SetPreTargetRenderBox(nullptr);
    });

    std::vector<RefPtr<GestureRecognizer>> recognizers { longPressRecognizer, panRecognizer };
    dragDropGesture_ = AceType::MakeRefPtr<OHOS::Ace::SequencedRecognizer>(GetContext(), recognizers);
    dragDropGesture_->SetIsExternalGesture(true);
}

void RenderBox::AddDataToClipboard(const RefPtr<PipelineContext>& context, const std::string& extraInfo)
{
    auto seleItemSizeStr = JsonUtil::Create(true);
    seleItemSizeStr->Put("width", selectedItemSize_.Width());
    seleItemSizeStr->Put("height", selectedItemSize_.Height());
    seleItemSizeStr->Put("selectedIndex", selectedIndex_);
    seleItemSizeStr->Put("customDragInfo", extraInfo.c_str());

    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
    clipboard->SetData(seleItemSizeStr->ToString());
}

DragItemInfo RenderBox::GenerateDragItemInfo(const RefPtr<PipelineContext>& context, const GestureEvent& info)
{
    RefPtr<DragEvent> event = AceType::MakeRefPtr<DragEvent>();
    event->SetX(context->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetX(), DimensionUnit::PX)));
    event->SetY(context->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetY(), DimensionUnit::PX)));
    selectedItemSize_ = GetLayoutSize();
    auto extraParams = JsonUtil::Create(true);
    SetSelectedIndex(info);
    if (selectedIndex_  != DEFAULT_INDEX) {
        extraParams->Put("selectedIndex", selectedIndex_);
    }

    return onDragStart_(event, extraParams->ToString());
}

void RenderBox::PanOnActionStart(const GestureEvent& info)
{
    if (onDragStart_) {
        auto pipelineContext = context_.Upgrade();
        if (!pipelineContext) {
            LOGE("Context is null.");
            return;
        }

        auto dragItemInfo = GenerateDragItemInfo(pipelineContext, info);
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        if (dragItemInfo.pixelMap) {
            auto wp = AceType::WeakClaim(this);
            auto initRenderNode = wp.Upgrade();
            if (!initRenderNode) {
                LOGE("initRenderNode is null.");
                return;
            }

            isDragRenderBox_ = true;
            pipelineContext->SetInitRenderNode(initRenderNode);

            AddDataToClipboard(pipelineContext, dragItemInfo.extraInfo);
            if (!dragWindow_) {
                auto rect = pipelineContext->GetCurrentWindowRect();
                dragWindow_ = DragWindow::CreateDragWindow("APP_DRAG_WINDOW",
                    static_cast<int32_t>(info.GetGlobalPoint().GetX()) + rect.Left(),
                    static_cast<int32_t>(info.GetGlobalPoint().GetY()) + rect.Top(),
                    dragItemInfo.pixelMap->GetWidth(), dragItemInfo.pixelMap->GetHeight());
                dragWindow_->SetOffset(rect.Left(), rect.Top());
                dragWindow_->DrawPixelMap(dragItemInfo.pixelMap);
            }
            return;
        }
#endif
        if (!dragItemInfo.customComponent) {
            LOGW("the drag custom component is null");
            return;
        }
        hasDragItem_ = true;
        auto positionedComponent = AceType::MakeRefPtr<PositionedComponent>(dragItemInfo.customComponent);
        positionedComponent->SetTop(Dimension(GetGlobalOffset().GetY()));
        positionedComponent->SetLeft(Dimension(GetGlobalOffset().GetX()));
        SetLocalPoint(info.GetGlobalPoint() - GetGlobalOffset());
        auto updatePosition = [renderBox = AceType::Claim(this)](
                                  const std::function<void(const Dimension&, const Dimension&)>& func) {
            if (!renderBox) {
                return;
            }
            renderBox->SetUpdateBuilderFuncId(func);
        };
        positionedComponent->SetUpdatePositionFuncId(updatePosition);
        auto stackElement = pipelineContext->GetLastStack();
        stackElement->PushComponent(positionedComponent);
    }
}

void RenderBox::PanOnActionUpdate(const GestureEvent& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        if (isDragRenderBox_ && dragWindow_) {
            int32_t x = static_cast<int32_t>(info.GetGlobalPoint().GetX());
            int32_t y = static_cast<int32_t>(info.GetGlobalPoint().GetY());
            if (dragWindow_) {
                dragWindow_->MoveTo(x, y);
            }
            return;
        }
#endif
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("Context is null.");
        return;
    }

    RefPtr<DragEvent> event = AceType::MakeRefPtr<DragEvent>();
    event->SetX(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetX(), DimensionUnit::PX)));
    event->SetY(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetY(), DimensionUnit::PX)));

    Offset offset = info.GetGlobalPoint() - GetLocalPoint();
    if (GetUpdateBuilderFuncId()) {
        GetUpdateBuilderFuncId()(Dimension(offset.GetX()), Dimension(offset.GetY()));
    }

    auto extraParams = JsonUtil::Create(true);
    auto targetRenderBox = FindTargetRenderBox(pipelineContext, info);
    auto preTargetRenderBox = GetPreTargetRenderBox();
    if (preTargetRenderBox == targetRenderBox) {
        if (targetRenderBox && targetRenderBox->GetOnDragMove()) {
            SetInsertIndex(targetRenderBox, info);
            if (insertIndex_ != DEFAULT_INDEX) {
                (targetRenderBox->GetOnDragMove())(event, extraParams->ToString());
                return;
            }
            if (targetRenderBox != initialRenderBox_) {
                extraParams->Put("selectedIndex", -1);
            } else {
                extraParams->Put("selectedIndex", selectedIndex_);
            }
            extraParams->Put("insertIndex", insertIndex_);
            (targetRenderBox->GetOnDragMove())(event, extraParams->ToString());
        }
        return;
    }
    if (preTargetRenderBox && preTargetRenderBox->GetOnDragLeave()) {
        (preTargetRenderBox->GetOnDragLeave())(event, extraParams->ToString());
    }
    if (targetRenderBox && targetRenderBox->GetOnDragEnter()) {
        (targetRenderBox->GetOnDragEnter())(event, extraParams->ToString());
    }
    SetPreTargetRenderBox(targetRenderBox);
}

void RenderBox::PanOnActionEnd(const GestureEvent& info)
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        LOGE("Context is null.");
        return;
    }
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        if (isDragRenderBox_) {
            isDragRenderBox_ = false;

            if (GetOnDrop()) {
                RefPtr<DragEvent> event = AceType::MakeRefPtr<DragEvent>();
                RefPtr<PasteData> pasteData = AceType::MakeRefPtr<PasteData>();
                event->SetPasteData(pasteData);
                event->SetX(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetX(), DimensionUnit::PX)));
                event->SetY(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetY(), DimensionUnit::PX)));

                auto extraParams = JsonUtil::Create(true);
                extraParams->Put("selectedIndex", selectedIndex_);
                extraParams->Put("insertIndex", insertIndex_);
                (GetOnDrop())(event, extraParams->ToString());
                pipelineContext->SetInitRenderNode(nullptr);
            }
        }

        if (dragWindow_) {
            dragWindow_->Destory();
            dragWindow_ = nullptr;
            return;
        }
#endif
    RefPtr<DragEvent> event = AceType::MakeRefPtr<DragEvent>();
    RefPtr<PasteData> pasteData = AceType::MakeRefPtr<PasteData>();
    event->SetPasteData(pasteData);
    event->SetX(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetX(), DimensionUnit::PX)));
    event->SetY(pipelineContext->ConvertPxToVp(Dimension(info.GetGlobalPoint().GetY(), DimensionUnit::PX)));

    Offset offset = info.GetGlobalPoint() - GetLocalPoint();
    if (GetUpdateBuilderFuncId()) {
        GetUpdateBuilderFuncId()(Dimension(offset.GetX()), Dimension(offset.GetY()));
    }
    if (hasDragItem_) {
        auto stackElement = pipelineContext->GetLastStack();
        stackElement->PopComponent();
    }
    hasDragItem_ = false;

    ACE_DCHECK(GetPreTargetRenderBox() == FindTargetRenderNode<RenderBox>(pipelineContext, info));
    auto targetRenderBox = GetPreTargetRenderBox();
    if (!targetRenderBox) {
        return;
    }
    if (targetRenderBox->GetOnDrop()) {
        auto extraParams = JsonUtil::Create(true);
        SetInsertIndex(targetRenderBox, info);
        if (insertIndex_ == DEFAULT_INDEX) {
            (targetRenderBox->GetOnDrop())(event, extraParams->ToString());
            SetPreTargetRenderBox(nullptr);
            return;
        }
        if (targetRenderBox != initialRenderBox_) {
            extraParams->Put("selectedIndex", -1);
        } else {
            extraParams->Put("selectedIndex", selectedIndex_);
        }
        extraParams->Put("insertIndex", insertIndex_);
        (targetRenderBox->GetOnDrop())(event, extraParams->ToString());
    }
    SetPreTargetRenderBox(nullptr);
}

void RenderBox::SetSelectedIndex(const GestureEvent& info)
{
    auto renderList = FindTargetRenderNode<V2::RenderList>(context_.Upgrade(), info);
    if (renderList) {
        selectedIndex_ = renderList->CalculateSelectedIndex(renderList, info, selectedItemSize_);
        initialRenderBox_ = FindTargetRenderBox(context_.Upgrade(), info);
    }
}

void RenderBox::SetInsertIndex(const RefPtr<RenderBox>& targetRenderBox, const GestureEvent& info)
{
    auto renderList = targetRenderBox->FindTargetRenderNode<V2::RenderList>(context_.Upgrade(), info);
    if (renderList) {
        insertIndex_ = renderList->CalculateInsertIndex(renderList, info, selectedItemSize_);
    }
}

RefPtr<RenderBox> RenderBox::FindTargetRenderBox(const RefPtr<PipelineContext> context, const GestureEvent& info)
{
    if (!context) {
        return nullptr;
    }

    auto pageRenderNode = context->GetLastPageRender();
    if (!pageRenderNode) {
        return nullptr;
    }
    auto offset = context->GetStageRect().GetOffset();
    auto targetRenderNode = pageRenderNode->FindDropChild(info.GetGlobalPoint(), info.GetGlobalPoint() - offset);
    if (!targetRenderNode) {
        return nullptr;
    }
    return AceType::DynamicCast<RenderBox>(targetRenderNode);
}

void RenderBox::UpdateBackDecoration(const RefPtr<Decoration>& newDecoration)
{
    if (!newDecoration) {
        backDecoration_ = newDecoration;
        OnAttachContext();
        return;
    }

    if (!backDecoration_) {
        LOGD("backDecoration_ is null.");
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    OnAttachContext();

    backDecoration_->SetAnimationColor(newDecoration->GetAnimationColor());
    backDecoration_->SetArcBackground(newDecoration->GetArcBackground());
    backDecoration_->SetBlurRadius(newDecoration->GetBlurRadius());
    backDecoration_->SetBorder(newDecoration->GetBorder());
    backDecoration_->SetGradient(newDecoration->GetGradient(), context_, [weak = WeakClaim(this)] {
        auto renderBox = weak.Upgrade();
        if (renderBox) {
            renderBox->OnAnimationCallback();
        }
    });
    backDecoration_->SetGradientBorderImage(newDecoration->GetGradientBorderImage());
    backDecoration_->SetHasBorderImageSource(newDecoration->GetHasBorderImageSource());
    backDecoration_->SetHasBorderImageSlice(newDecoration->GetHasBorderImageSlice());
    backDecoration_->SetHasBorderImageWidth(newDecoration->GetHasBorderImageWidth());
    backDecoration_->SetHasBorderImageOutset(newDecoration->GetHasBorderImageOutset());
    backDecoration_->SetHasBorderImageRepeat(newDecoration->GetHasBorderImageRepeat());
    backDecoration_->SetHasBorderImageGradient(newDecoration->GetHasBorderImageGradient());
    backDecoration_->SetImage(newDecoration->GetImage());
    backDecoration_->SetBorderImage(newDecoration->GetBorderImage());
    backDecoration_->SetPadding(newDecoration->GetPadding());
    backDecoration_->SetWindowBlurProgress(newDecoration->GetWindowBlurProgress());
    backDecoration_->SetWindowBlurStyle(newDecoration->GetWindowBlurStyle());
    backDecoration_->SetShadows(newDecoration->GetShadows());
    backDecoration_->SetGrayScale(newDecoration->GetGrayScale());
    backDecoration_->SetBrightness(newDecoration->GetBrightness());
    backDecoration_->SetContrast(newDecoration->GetContrast());
    backDecoration_->SetSaturate(newDecoration->GetSaturate());
    backDecoration_->SetInvert(newDecoration->GetInvert());
    backDecoration_->SetColorBlend(newDecoration->GetColorBlend());
    backDecoration_->SetSepia(newDecoration->GetSepia());
    backDecoration_->SetBackgroundColor(newDecoration->GetBackgroundColor());
}

void RenderBox::UpdateFrontDecoration(const RefPtr<Decoration>& newDecoration)
{
    if (!newDecoration) {
        frontDecoration_ = newDecoration;
        return;
    }

    if (!frontDecoration_) {
        LOGD("frontDecoration_ is null.");
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    frontDecoration_->SetBlurRadius(newDecoration->GetBlurRadius());
    frontDecoration_->SetBorder(newDecoration->GetBorder());
    frontDecoration_->SetImage(newDecoration->GetImage());
    frontDecoration_->SetShadows(newDecoration->GetShadows());
    frontDecoration_->SetBackgroundColor(newDecoration->GetBackgroundColor());
    frontDecoration_->SetGrayScale(newDecoration->GetGrayScale());
    frontDecoration_->SetBrightness(newDecoration->GetBrightness());
    frontDecoration_->SetContrast(newDecoration->GetContrast());
    frontDecoration_->SetSaturate(newDecoration->GetSaturate());
    frontDecoration_->SetInvert(newDecoration->GetInvert());
    frontDecoration_->SetColorBlend(newDecoration->GetColorBlend());
    frontDecoration_->SetSepia(newDecoration->GetSepia());
    frontDecoration_->SetHueRotate(newDecoration->GetHueRotate());
}

void RenderBox::UpdateStyleFromRenderNode(PropertyAnimatableType type)
{
    // Operator map for styles
    static const std::unordered_map<PropertyAnimatableType, void (*)(RenderBox&)> operators = {
        // Set width and height
        { PropertyAnimatableType::PROPERTY_WIDTH,
            [](RenderBox& node) {
                auto box = node.boxComponent_.Upgrade();
                if (box) {
                    box->SetWidth(node.width_);
                }
            } },
        { PropertyAnimatableType::PROPERTY_HEIGHT,
            [](RenderBox& node) {
                auto box = node.boxComponent_.Upgrade();
                if (box) {
                    box->SetHeight(node.height_);
                }
            } },
        { PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR,
            [](RenderBox& node) {
                auto box = node.boxComponent_.Upgrade();
                if (box) {
                    box->SetColor(node.GetColor());
                }
            } },
    };
    auto operatorIter = operators.find(type);
    if (operatorIter != operators.end()) {
        operatorIter->second(*this);
    }
}

void RenderBox::OnPaintFinish()
{
    if (eventExtensions_ && eventExtensions_->HasOnAreaChangeExtension()) {
        auto inspector = inspector_.Upgrade();
        if (inspector) {
            auto area = inspector->GetCurrentRectAndOrigin();
            eventExtensions_->GetOnAreaChangeExtension()->UpdateArea(area.first, area.second);
        }
    }
    auto node = GetAccessibilityNode().Upgrade();
    if (!node) {
        return;
    }
    if (!node->GetVisible()) { // Set 0 to item when whole outside of view port.
        node->SetWidth(0.0);
        node->SetHeight(0.0);
        node->SetTop(0.0);
        node->SetLeft(0.0);
        return;
    }
    if (node->IsValidRect()) {
        return; // Rect already clamp by viewport, no need to set again.
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto viewScale = context->GetViewScale();
    if (NearZero(viewScale)) {
        LOGE("Get viewScale is zero.");
        EventReport::SendRenderException(RenderExcepType::VIEW_SCALE_ERR);
        return;
    }
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    Size size = GetPaintSize() * viewScale;
    Offset globalOffset = (GetGlobalOffsetExternal() + margin_.GetOffset()) * viewScale;
    node->SetMarginSize(margin_.GetLayoutSize() * viewScale);
    node->SetWidth(size.Width());
    node->SetHeight(size.Height());
    node->SetLeft(globalOffset.GetX());
    node->SetTop(globalOffset.GetY());
#else
    Size size = paintSize_;
    Offset globalOffset = GetGlobalOffset();
    globalOffset.SetX(globalOffset.GetX() + margin_.LeftPx());
    globalOffset.SetY(globalOffset.GetY() + margin_.TopPx());
    if (node->IsAnimationNode()) {
        CalculateScale(node, globalOffset, size);
        CalculateRotate(node, globalOffset, size);
        CalculateTranslate(node, globalOffset, size);
    }
    size = size * viewScale;
    globalOffset = globalOffset * viewScale;
    node->SetWidth(size.Width());
    node->SetHeight(size.Height());
    node->SetLeft(globalOffset.GetX());
    node->SetTop(globalOffset.GetY());
#endif
}

Offset RenderBox::GetGlobalOffsetExternal() const
{
    auto renderNode = GetParent().Upgrade();
    auto offset = renderNode ? GetPosition() + renderNode->GetGlobalOffsetExternal() : GetPosition();
    offset += alignOffset_;
    return offset;
}

Offset RenderBox::GetGlobalOffset() const
{
    auto renderNode = GetParent().Upgrade();
    auto offset = renderNode ? GetPosition() + renderNode->GetGlobalOffset() : GetPosition();
    offset += alignOffset_;
    return offset;
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void RenderBox::CalculateScale(RefPtr<AccessibilityNode> node, Offset& globalOffset, Size& size)
{
    double scaleFactor = node->GetScale();
    Offset scaleCenter = node->GetScaleCenter();
    if (!NearEqual(scaleFactor, 1.0)) {
        if (NearEqual(scaleFactor, 0.0)) {
            scaleFactor = 0.01;
        }
        // parent and children are scaled by the center point of parent.
        auto currentOffset = globalOffset;
        auto currentSize = size;
        auto boxCenter =
            Offset(currentOffset.GetX() + currentSize.Width() / 2.0, currentOffset.GetY() + currentSize.Height() / 2.0);
        if (boxCenter == scaleCenter) {
            globalOffset = Offset(currentSize.Width() * (1 - scaleFactor) / 2.0 + currentOffset.GetX(),
                currentSize.Height() * (1 - scaleFactor) / 2.0 + currentOffset.GetY());
        } else {
            auto center = scaleCenter;
            globalOffset = Offset(scaleFactor * currentOffset.GetX() + (1 - scaleFactor) * center.GetX(),
                scaleFactor * currentOffset.GetY() + (1 - scaleFactor) * center.GetY());
        }
        size = size * scaleFactor;
    }
}

void RenderBox::CalculateRotate(RefPtr<AccessibilityNode> node, Offset& globalOffset, Size& size)
{
    double angle = node->GetRotateAngle();
    if (!NearEqual(angle, 0.0)) {
        Point leftTop;
        Point rightTop;
        Point leftBottom;
        Point rightBottom;
        Point center = Point(node->GetScaleCenter().GetX(), node->GetScaleCenter().GetY());
        leftTop.SetX(globalOffset.GetX());
        leftTop.SetY(globalOffset.GetY());

        rightTop.SetX(globalOffset.GetX() + size.Width());
        rightTop.SetY(globalOffset.GetY());

        leftBottom.SetX(globalOffset.GetX());
        leftBottom.SetY(globalOffset.GetY() + size.Height());

        rightBottom.SetX(globalOffset.GetX() + size.Width());
        rightBottom.SetY(globalOffset.GetY() + size.Height());
        const double pi = std::acos(-1);
        double RotateAngle = angle * pi / 180;

        leftTop.Rotate(center, RotateAngle);
        rightTop.Rotate(center, RotateAngle);
        leftBottom.Rotate(center, RotateAngle);
        rightBottom.Rotate(center, RotateAngle);

        double min_X = std::min({ leftTop.GetX(), rightTop.GetX(), leftBottom.GetX(), rightBottom.GetX() });
        double max_X = std::max({ leftTop.GetX(), rightTop.GetX(), leftBottom.GetX(), rightBottom.GetX() });
        double min_Y = std::min({ leftTop.GetY(), rightTop.GetY(), leftBottom.GetY(), rightBottom.GetY() });
        double max_Y = std::max({ leftTop.GetY(), rightTop.GetY(), leftBottom.GetY(), rightBottom.GetY() });
        globalOffset.SetX(min_X);
        globalOffset.SetY(min_Y);
        size.SetWidth(max_X - min_X);
        size.SetHeight(max_Y - min_Y);
    }
}

void RenderBox::CalculateTranslate(RefPtr<AccessibilityNode> node, Offset& globalOffset, Size& size)
{
    // calculate translate
    Offset translateOffset = node->GetTranslateOffset();
    globalOffset = globalOffset + translateOffset;
}
#endif

void RenderBox::SetBackgroundPosition(const BackgroundImagePosition& position)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
    if (!backgroundImage) {
        // Suppress error logs when do animation.
        LOGD("set background position failed. no background image.");
        return;
    }
    if (backgroundImage->GetImagePosition() == position) {
        return;
    }
    backgroundImage->SetImagePosition(position);
    if (renderImage_) {
        renderImage_->SetBgImagePosition(backgroundImage->GetImagePosition());
    }
    MarkNeedLayout();
}

void RenderBox::ClearRenderObject()
{
    RenderBoxBase::ClearRenderObject();
    renderImage_ = nullptr;
    backDecoration_ = nullptr;
    frontDecoration_ = nullptr;
    controllerEnter_ = nullptr;
    controllerExit_ = nullptr;
    colorAnimationEnter_ = nullptr;
    colorAnimationExit_ = nullptr;
    animationType_ = HoverAnimationType::NONE;
    hoverColor_ = Color::TRANSPARENT;
    for (size_t i = 0; i < recognizers_.size(); i++) {
        recognizers_[i] = nullptr;
    }

    dragDropGesture_ = nullptr;
    preTargetRenderBox_ = nullptr;
    initialRenderBox_ = nullptr;
    updateBuilder_ = nullptr;
    onDragStart_ = nullptr;
    onDragEnter_ = nullptr;
    onDragMove_ = nullptr;
    onDragLeave_ = nullptr;
    onDrop_ = nullptr;
    onClick_ = nullptr;
    onLongPress_ = nullptr;
}

BackgroundImagePosition RenderBox::GetBackgroundPosition() const
{
    if (backDecoration_ == nullptr) {
        return BackgroundImagePosition();
    }
    RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
    if (!backgroundImage) {
        LOGE("get background position failed. no background image.");
        return BackgroundImagePosition();
    }
    return backgroundImage->GetImagePosition();
}

void RenderBox::SetBackgroundSize(const BackgroundImageSize& size)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
    if (!backgroundImage) {
        // Suppress error logs when do animation.
        LOGE("set background size failed. no background image.");
        return;
    }
    if (backgroundImage->GetImageSize() == size) {
        return;
    }
    backgroundImage->SetImageSize(size);
    if (renderImage_) {
        // x direction
        renderImage_->SetBgImageSize(size.GetSizeTypeX(), size.GetSizeValueX(), true);
        // y direction
        renderImage_->SetBgImageSize(size.GetSizeTypeY(), size.GetSizeValueY(), false);
    }
    MarkNeedLayout();
}

BackgroundImageSize RenderBox::GetBackgroundSize() const
{
    if (backDecoration_ == nullptr) {
        return BackgroundImageSize();
    }
    RefPtr<BackgroundImage> backgroundImage = backDecoration_->GetImage();
    if (!backgroundImage) {
        LOGE("get background size failed. no background image.");
        return BackgroundImageSize();
    }
    return backgroundImage->GetImageSize();
}

void RenderBox::OnMouseHoverEnterAnimation()
{
    // stop the exit animation being played.
    ResetController(controllerExit_);
    if (!controllerEnter_) {
        controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (animationType_ == HoverAnimationType::OPACITY) {
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        CreateColorAnimation(colorAnimationEnter_, hoverColor_, Color::FromRGBO(0, 0, 0, 0.05));
        colorAnimationEnter_->SetCurve(Curves::FRICTION);
    }
    controllerEnter_->AddInterpolator(colorAnimationEnter_);
    controllerEnter_->SetDuration(HOVER_ANIMATION_DURATION);
    controllerEnter_->Play();
    controllerEnter_->SetFillMode(FillMode::FORWARDS);
}

void RenderBox::OnMouseHoverExitAnimation()
{
    // stop the enter animation being played.
    ResetController(controllerEnter_);
    if (!controllerExit_) {
        controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (animationType_ == HoverAnimationType::OPACITY) {
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        // The exit animation plays from the current background color.
        CreateColorAnimation(colorAnimationExit_, hoverColor_, Color::FromRGBO(0, 0, 0, 0.0));
        if (hoverColor_ == Color::FromRGBO(0, 0, 0, 0.05)) {
            colorAnimationExit_->SetCurve(Curves::FRICTION);
        } else {
            colorAnimationExit_->SetCurve(Curves::FAST_OUT_SLOW_IN);
        }
    }
    controllerExit_->AddInterpolator(colorAnimationExit_);
    controllerExit_->SetDuration(HOVER_ANIMATION_DURATION);
    controllerExit_->Play();
    controllerExit_->SetFillMode(FillMode::FORWARDS);
}

WeakPtr<RenderNode> RenderBox::CheckHoverNode()
{
    return AceType::WeakClaim<RenderNode>(this);
}

void RenderBox::CreateFloatAnimation(RefPtr<KeyframeAnimation<float>>& floatAnimation, float beginValue, float endValue)
{
    if (!floatAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<float>>(1.0, endValue);
    floatAnimation->AddKeyframe(keyframeBegin);
    floatAnimation->AddKeyframe(keyframeEnd);
    floatAnimation->AddListener([weakBox = AceType::WeakClaim(this)](float value) {
        auto box = weakBox.Upgrade();
        if (box) {
            box->scale_ = value;
            box->MarkNeedRender();
        }
    });
}

void RenderBox::CreateColorAnimation(
    RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue, const Color& endValue)
{
    if (!colorAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0, endValue);
    colorAnimation->AddKeyframe(keyframeBegin);
    colorAnimation->AddKeyframe(keyframeEnd);
    if (!backDecoration_) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    colorAnimation->AddListener([weakBox = AceType::WeakClaim(this)](const Color& value) {
        auto box = weakBox.Upgrade();
        if (!box) {
            return;
        }
        box->hoverColor_ = value;
        if (box->GetBackDecoration()) {
            LOGD("RenderBox::CreateColorAnimation box->hoverColor_ = %{public}x", box->hoverColor_.GetValue());
            box->GetBackDecoration()->SetBackgroundColor(box->hoverColor_);
            box->GetBackDecoration()->SetAnimationColor(box->hoverColor_);
        }
        box->MarkNeedRender();
    });
}

void RenderBox::AnimateMouseHoverEnter()
{
    MouseHoverEnterTest();
}

void RenderBox::MouseHoverEnterTest()
{
    LOGD("RenderBox::MouseHoverEnterTest in. animationType_ = %{public}d", animationType_);
    ResetController(controllerExit_);
    if (!controllerEnter_) {
        controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (animationType_ == HoverAnimationType::SCALE) {
        if (!scaleAnimationEnter_) {
            scaleAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        }
        CreateFloatAnimation(scaleAnimationEnter_, 1.0, 1.05);
        controllerEnter_->ClearInterpolators();
        controllerEnter_->AddInterpolator(scaleAnimationEnter_);
        isHoveredScale = true;
    } else if (animationType_ == HoverAnimationType::BOARD) {
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        if (!colorAnimationEnter_) {
            colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        }
        CreateColorAnimation(colorAnimationEnter_, hoverColorBegin_, Color::FromRGBO(0, 0, 0, 0.05));
        controllerEnter_->ClearInterpolators();
        controllerEnter_->AddInterpolator(colorAnimationEnter_);
        isHoveredBoard = true;
    } else {
        return;
    }
    controllerEnter_->SetDuration(HOVER_ANIMATION_DURATION);
    controllerEnter_->SetFillMode(FillMode::FORWARDS);
    controllerEnter_->Play();
}

void RenderBox::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void RenderBox::AnimateMouseHoverExit()
{
    MouseHoverExitTest();
}

void RenderBox::MouseHoverExitTest()
{
    LOGD("RenderBox::MouseHoverExitTest in. animationType_ = %{public}d", animationType_);
    ResetController(controllerEnter_);
    if (!controllerExit_) {
        controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (animationType_ == HoverAnimationType::SCALE) {
        scaleAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        auto begin = scale_;
        CreateFloatAnimation(scaleAnimationExit_, begin, 1.0);
        controllerExit_->ClearInterpolators();
        controllerExit_->AddInterpolator(scaleAnimationExit_);
        isHoveredScale = false;
    } else if (animationType_ == HoverAnimationType::BOARD) {
        if (!backDecoration_) {
            backDecoration_ = AceType::MakeRefPtr<Decoration>();
        }
        if (!colorAnimationExit_) {
            colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        }
        LOGD("MouseHoverExitTest hoverColor_.GetValue() = %{public}x, hoverColorBegin_.GetValue() = %{public}x",
            hoverColor_.GetValue(), hoverColorBegin_.GetValue());
        CreateColorAnimation(colorAnimationExit_, hoverColor_, hoverColorBegin_);
        controllerExit_->ClearInterpolators();
        controllerExit_->AddInterpolator(colorAnimationExit_);
        isHoveredBoard = false;
    } else {
        return;
    }
    controllerExit_->SetDuration(HOVER_ANIMATION_DURATION);
    controllerExit_->Play();
    controllerExit_->SetFillMode(FillMode::FORWARDS);
}

void RenderBox::HandleMouseHoverEvent(MouseState mouseState)
{
    std::string accessibilityEventType;
    if (mouseState == MouseState::HOVER) {
        accessibilityEventType = "mousehoverenter";
    } else {
        accessibilityEventType = "mousehoverexit";
    }
    SendAccessibilityEvent(accessibilityEventType);

    if (onHover_) {
        onHover_(mouseState == MouseState::HOVER);
    }
}

void RenderBox::StopMouseHoverAnimation()
{
    if (controllerExit_) {
        if (!controllerExit_->IsStopped()) {
            controllerExit_->Stop();
        }
        controllerExit_->ClearInterpolators();
    }
}

bool RenderBox::HandleMouseEvent(const MouseEvent& event)
{
    if (!onMouse_) {
        return false;
    }
    
    MouseInfo info;
    info.SetButton(event.button);
    info.SetAction(event.action);
    info.SetGlobalLocation(event.GetOffset());
    info.SetLocalLocation(event.GetOffset() - Offset(GetCoordinatePoint().GetX(), GetCoordinatePoint().GetY()));
    info.SetScreenLocation(event.GetScreenOffset());
    info.SetTimeStamp(event.time);
    info.SetDeviceId(event.deviceId);
    info.SetSourceDevice(event.sourceType);
    LOGI("RenderBox::HandleMouseEvent: Do mouse callback with mouse event{ Global(%{public}f,%{public}f), "
         "Local(%{public}f,%{public}f)}, Button(%{public}d), Action(%{public}d), Time(%{public}lld), "
         "DeviceId(%{public}" PRId64 ", SourceType(%{public}d) }. Return: %{public}d",
        info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY(), info.GetLocalLocation().GetX(),
        info.GetLocalLocation().GetY(), info.GetButton(), info.GetAction(),
        info.GetTimeStamp().time_since_epoch().count(), info.GetDeviceId(), info.GetSourceDevice(),
        info.IsStopPropagation());
    onMouse_(info);
    return info.IsStopPropagation();
}

ColorPropertyAnimatable::SetterMap RenderBox::GetColorPropertySetterMap()
{
    ColorPropertyAnimatable::SetterMap map;
    auto weak = AceType::WeakClaim(this);
    const RefPtr<RenderTextField> renderTextField = AceType::DynamicCast<RenderTextField>(GetFirstChild());
    if (renderTextField) {
        WeakPtr<RenderTextField> textWeak = renderTextField;
        map[PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR] = [textWeak](Color value) {
            auto renderTextField = textWeak.Upgrade();
            if (!renderTextField) {
                return;
            }
            renderTextField->SetColor(value);
        };
    } else {
        map[PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR] = [weak](Color value) {
            auto box = weak.Upgrade();
            if (!box) {
                return;
            }
            box->SetColor(value, true);
        };
    }
    map[PropertyAnimatableType::PROPERTY_FRONT_DECORATION_COLOR] = [weak](Color value) {
        auto box = weak.Upgrade();
        if (!box) {
            return;
        }
        box->SetColor(value, false);
    };
    return map;
}

ColorPropertyAnimatable::GetterMap RenderBox::GetColorPropertyGetterMap()
{
    ColorPropertyAnimatable::GetterMap map;
    auto weak = AceType::WeakClaim(this);
    map[PropertyAnimatableType::PROPERTY_FRONT_DECORATION_COLOR] = [weak]() -> Color {
        auto box = weak.Upgrade();
        if (!box) {
            return Color();
        }
        auto frontDecoration = box->GetFrontDecoration();
        if (frontDecoration) {
            return frontDecoration->GetBackgroundColor();
        }
        return Color::TRANSPARENT;
    };
    const RefPtr<RenderTextField> renderTextField = AceType::DynamicCast<RenderTextField>(GetFirstChild());
    if (renderTextField) {
        WeakPtr<RenderTextField> textWeak = renderTextField;
        map[PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR] = [textWeak]() -> Color {
            auto renderTextField = textWeak.Upgrade();
            if (!renderTextField) {
                return Color();
            }
            return renderTextField->GetColor();
        };
    } else {
        map[PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR] = [weak]() -> Color {
            auto box = weak.Upgrade();
            if (!box) {
                return Color();
            }
            return box->GetColor();
        };
    }
    return map;
}

void RenderBox::SetShadow(const Shadow& value)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }

    auto shadows = backDecoration_->GetShadows();
    Shadow shadow;

    if (!shadows.empty()) {
        shadow = shadows.front();
    }

    if (shadow != value) {
        backDecoration_->ClearAllShadow();
        backDecoration_->AddShadow(value);
        MarkNeedLayout();
    }
}

Shadow RenderBox::GetShadow() const
{
    if (backDecoration_ != nullptr) {
        const auto& shadows = backDecoration_->GetShadows();
        if (!shadows.empty()) {
            return shadows.front();
        }
    }
    return {};
}

void RenderBox::SetGrayScale(double scale)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }

    double _scale = frontDecoration_->GetGrayScale().Value();

    if (!NearEqual(_scale, scale)) {
        frontDecoration_->SetGrayScale(Dimension(_scale));
        MarkNeedRender();
    }
}

double RenderBox::GetGrayScale(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetGrayScale().Value();
    }

    return 0.0;
}

void RenderBox::SetBrightness(double ness)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }

    double brightness = frontDecoration_->GetBrightness().Value();

    if (!NearEqual(brightness, ness)) {
        frontDecoration_->SetBrightness(Dimension(brightness));
        MarkNeedRender();
    }
}

double RenderBox::GetBrightness(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetBrightness().Value();
    }
    return 0.0;
}

void RenderBox::SetContrast(double trast)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    double contrast = frontDecoration_->GetContrast().Value();
    if (!NearEqual(contrast, trast)) {
        frontDecoration_->SetContrast(Dimension(contrast));
        MarkNeedRender();
    }
}

double RenderBox::GetContrast(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetContrast().Value();
    }
    return 0.0;
}

void RenderBox::SetColorBlend(const Color& color)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    if (!NearEqual(frontDecoration_->GetColorBlend().GetValue(), color.GetValue())) {
        frontDecoration_->SetColorBlend(color);
        MarkNeedRender();
    }
}

Color RenderBox::GetColorBlend() const
{
    if (frontDecoration_) {
        return frontDecoration_->GetColorBlend();
    }
    return {};
}

void RenderBox::SetSaturate(double rate)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    double saturate = frontDecoration_->GetSaturate().Value();
    if (!NearEqual(saturate, rate)) {
        frontDecoration_->SetSaturate(Dimension(saturate));
        MarkNeedRender();
    }
}

double RenderBox::GetSaturate(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetSaturate().Value();
    }
    return 0.0;
}

void RenderBox::SetSepia(double pia)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    double pias = frontDecoration_->GetSepia().Value();
    if (!NearEqual(pias, pia)) {
        frontDecoration_->SetSepia(Dimension(pias));
        MarkNeedRender();
    }
}

double RenderBox::GetSepia(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetSepia().Value();
    }
    return 0.0;
}

void RenderBox::SetInvert(double invert)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    double inverts = frontDecoration_->GetInvert().Value();
    if (!NearEqual(inverts, invert)) {
        frontDecoration_->SetInvert(Dimension(inverts));
        MarkNeedRender();
    }
}

double RenderBox::GetInvert(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetInvert().Value();
    }
    return 0.0;
}

void RenderBox::SetHueRotate(float deg)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    float degs = frontDecoration_->GetHueRotate();
    if (!NearEqual(degs, deg)) {
        frontDecoration_->SetHueRotate(degs);
        MarkNeedRender();
    }
}

float RenderBox::GetHueRotate(void) const
{
    if (frontDecoration_ != nullptr) {
        return frontDecoration_->GetHueRotate();
    }
    return 0.0;
}

void RenderBox::SetBorderWidth(double width, const BorderEdgeHelper& helper)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    Border border = backDecoration_->GetBorder();
    if (helper.Set(width, &border)) {
        backDecoration_->SetBorder(border);
        MarkNeedLayout();
    }
}

double RenderBox::GetBorderWidth(const BorderEdgeHelper& helper) const
{
    if (backDecoration_ != nullptr) {
        return helper.Get(backDecoration_->GetBorder()).GetWidth().Value();
    }
    return 0.0;
}

void RenderBox::SetBorderColor(const Color& color, const BorderEdgeHelper& helper)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    Border border = backDecoration_->GetBorder();
    if (helper.Set(color, &border)) {
        backDecoration_->SetBorder(border);
        MarkNeedLayout();
    }
}

Color RenderBox::GetBorderColor(const BorderEdgeHelper& helper) const
{
    if (backDecoration_) {
        return helper.Get(backDecoration_->GetBorder()).GetColor();
    }
    return {};
}

void RenderBox::SetBorderStyle(BorderStyle borderStyle, const BorderEdgeHelper& helper)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    Border border = backDecoration_->GetBorder();
    if (helper.Set(borderStyle, &border)) {
        backDecoration_->SetBorder(border);
        MarkNeedLayout();
    }
}

BorderStyle RenderBox::GetBorderStyle(const BorderEdgeHelper& helper) const
{
    if (backDecoration_) {
        return helper.Get(backDecoration_->GetBorder()).GetBorderStyle();
    }
    return BorderStyle::NONE;
}

void RenderBox::SetBorderRadius(double radius, const BorderRadiusHelper& helper)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    Border border = backDecoration_->GetBorder();
    if (helper.Set(radius, &border)) {
        backDecoration_->SetBorder(border);
        MarkNeedLayout();
    }
}

double RenderBox::GetBorderRadius(const BorderRadiusHelper& helper) const
{
    if (backDecoration_) {
        return helper.Get(backDecoration_->GetBorder());
    }
    return 0.0;
}

void RenderBox::SetBlurRadius(const AnimatableDimension& radius)
{
    if (frontDecoration_ == nullptr) {
        frontDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    if (!NearEqual(frontDecoration_->GetBlurRadius().Value(), radius.Value())) {
        frontDecoration_->SetBlurRadius(radius);
        MarkNeedRender();
    }
}

AnimatableDimension RenderBox::GetBlurRadius() const
{
    if (frontDecoration_) {
        return frontDecoration_->GetBlurRadius();
    }
    return AnimatableDimension(0.0, DimensionUnit::PX);
}

void RenderBox::SetBackdropRadius(const AnimatableDimension& radius)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    if (!NearEqual(backDecoration_->GetBlurRadius().Value(), radius.Value())) {
        backDecoration_->SetBlurRadius(radius);
        MarkNeedRender();
    }
}

AnimatableDimension RenderBox::GetBackdropRadius() const
{
    if (backDecoration_) {
        return backDecoration_->GetBlurRadius();
    }
    return AnimatableDimension(0.0, DimensionUnit::PX);
}

void RenderBox::SetWindowBlurProgress(double progress)
{
    if (backDecoration_ == nullptr) {
        backDecoration_ = AceType::MakeRefPtr<Decoration>();
    }
    if (!NearEqual(backDecoration_->GetWindowBlurProgress(), progress)) {
        backDecoration_->SetWindowBlurProgress(progress);
        MarkNeedRender();
    }
}

double RenderBox::GetWindowBlurProgress() const
{
    if (backDecoration_) {
        return backDecoration_->GetWindowBlurProgress();
    }
    return 0.0;
}

bool RenderBox::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (recognizerHierarchy_.empty()) {
        return RenderBoxBase::TouchTest(globalPoint, parentLocalPoint, touchRestrict, result);
    }

    TouchTestResult innerResult;
    bool parentResult = RenderBoxBase::TouchTest(globalPoint, parentLocalPoint, touchRestrict, innerResult);
    if (!parentResult) {
        Point transformPoint = GetTransformPoint(parentLocalPoint);
        if (!InTouchRectList(transformPoint, GetTouchRectList())) {
            return false;
        }
    }

    std::vector<RefPtr<GestureRecognizer>> innerRecognizers;
    const auto coordinateOffset = Offset(GetCoordinatePoint().GetX(), GetCoordinatePoint().GetY());

    for (auto const& eventTarget : innerResult) {
        auto recognizer = AceType::DynamicCast<GestureRecognizer>(eventTarget);
        if (recognizer) {
            recognizer->SetCoordinateOffset(coordinateOffset);
            innerRecognizers.push_back(std::move(recognizer));
        } else {
            result.push_back(eventTarget);
        }
    }

    OnTouchTestHierarchy(coordinateOffset, touchRestrict, innerRecognizers, result);

    return parentResult;
}

void RenderBox::OnTouchTestHierarchy(const Offset& coordinateOffset, const TouchRestrict& touchRestrict,
    const std::vector<RefPtr<GestureRecognizer>>& innerRecognizers, TouchTestResult& result)
{
    RefPtr<GestureRecognizer> current;
    if (innerRecognizers.size() == 1) {
        current = innerRecognizers[0];
    } else if (innerRecognizers.size() > 1) {
        current = AceType::MakeRefPtr<ExclusiveRecognizer>(innerRecognizers);
        current->SetCoordinateOffset(coordinateOffset);
    }

    for (auto const& level : recognizerHierarchy_) {
        GesturePriority priority = level.first;
        auto recognizers = level.second;

        if (recognizers.empty()) {
            continue;
        }

        for (auto& recognizer : recognizers) {
            recognizer->SetCoordinateOffset(coordinateOffset);
        }

        if (priority == GesturePriority::Parallel) {
            if (current) {
                recognizers.push_back(current);
            }

            if (recognizers.size() > 1) {
                current = AceType::MakeRefPtr<ParallelRecognizer>(std::move(recognizers));
                current->SetCoordinateOffset(coordinateOffset);
            } else if (recognizers.size() == 1) {
                current = recognizers[0];
            }
        } else {
            if (current) {
                if (priority == GesturePriority::Low) {
                    recognizers.insert(recognizers.begin(), current);
                } else {
                    recognizers.push_back(current);
                }
            }

            if (recognizers.size() > 1) {
                current = AceType::MakeRefPtr<ExclusiveRecognizer>(std::move(recognizers));
                current->SetCoordinateOffset(coordinateOffset);
            } else if (recognizers.size() == 1) {
                current = recognizers[0];
            }
        }
    }
    result.push_back(std::move(current));
}

void RenderBox::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (onClick_) {
        onClick_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(onClick_);
    }
    if (onLongPress_) {
        onLongPress_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(onLongPress_);
    }
    if (dragDropGesture_) {
        dragDropGesture_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDropGesture_);
    }
    if (touchRecognizer_) {
        touchRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(touchRecognizer_);
    }
}

void RenderBox::UpdateGestureRecognizerHierarchy(
    const std::vector<std::pair<GesturePriority, std::vector<RefPtr<Gesture>>>>& hierarchy)
{
    bool success = hierarchy.size() == recognizerHierarchy_.size();

    if (success) {
        for (size_t i = 0; i < hierarchy.size(); ++i) {
            if (hierarchy[i].first != recognizerHierarchy_[i].first ||
                hierarchy[i].second.size() != recognizerHierarchy_[i].second.size()) {
                success = false;
                break;
            }

            for (size_t j = 0; j < hierarchy[i].second.size(); ++j) {
                auto newRecognizer = hierarchy[i].second[j]->CreateRecognizer(context_);

                success = success && recognizerHierarchy_[i].second[j]->ReconcileFrom(newRecognizer);
            }
        }
    }

    if (!success) {
        recognizerHierarchy_.clear();
        for (auto const& level : hierarchy) {
            recognizerHierarchy_.emplace_back(
                level.first,
                std::vector<RefPtr<GestureRecognizer>>()
                );

            for (auto const& gesture : level.second) {
                auto recognizer = gesture->CreateRecognizer(context_);
                recognizer->SetIsExternalGesture(true);
                recognizerHierarchy_.back().second.push_back(std::move(recognizer));
            }
        }
    }
}

void RenderBox::HandleRemoteMessage(const ClickInfo& clickInfo)
{
    if (remoteMessageEvent_) {
        remoteMessageEvent_(std::make_shared<ClickInfo>(clickInfo));
    }
}

void RenderBox::OnStatusStyleChanged(const VisualState state)
{
    RenderBoxBase::OnStatusStyleChanged(state);

    if (stateAttributeList_ == nullptr) {
        return;
    }

    LOGD("state %{public}d  attr count %{public}d", state,
        static_cast<int32_t>(stateAttributeList_->GetAttributesForState(state).size()));
    bool updated = false;
    for (auto& attribute : stateAttributeList_->GetAttributesForState(state)) {
        updated = true;
        switch (attribute->id_) {
            case BoxStateAttribute::COLOR: {
                LOGD("Setting COLOR for state %{public}d", attribute->id_);
                auto colorState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableColor>>(attribute);
                GetBackDecoration()->SetBackgroundColor(colorState->value_);
            } break;

            case BoxStateAttribute::BORDER_COLOR: {
                LOGD("Setting BORDER_COLOR for state %{public}d", attribute->id_);
                auto colorState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableColor>>(attribute);
                BoxComponentHelper::SetBorderColor(GetBackDecoration(), colorState->value_);
            } break;

            case BoxStateAttribute::BORDER_RADIUS: {
                LOGD("Setting BORDER_RADIUS for state %{public}d", attribute->id_);
                auto radiusState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableDimension>>(attribute);
                BoxComponentHelper::SetBorderRadius(GetBackDecoration(), radiusState->value_);
            } break;

            case BoxStateAttribute::BORDER_STYLE: {
                LOGD("Setting BORDER_STYLE for state %{public}d", attribute->id_);
                auto attributeStateValue =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, BorderStyle>>(attribute);
                BoxComponentHelper::SetBorderStyle(GetBackDecoration(), attributeStateValue->value_);
            } break;

            case BoxStateAttribute::BORDER_WIDTH: {
                auto widthState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableDimension>>(attribute);
                LOGD("Setting BORDER_WIDTH for state %{public}d to %{public}lf",
                    state, widthState->value_.Value());
                BoxComponentHelper::SetBorderWidth(GetBackDecoration(), widthState->value_);
            } break;

            case BoxStateAttribute::HEIGHT: {
                auto valueState = AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, Dimension>>(attribute);
                LOGD("Setting BORDER_WIDTH for state %{public}d to %{public}lf", attribute->id_,
                    valueState->value_.Value());
                height_ = valueState->value_;
            } break;

            case BoxStateAttribute::WIDTH: {
                auto valueState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableDimension>>(attribute);
                LOGD("Setting BORDER_WIDTH for state %{public}d to %{public}lf",
                    state, valueState->value_.Value());
                width_ = valueState->value_;
            } break;

            case BoxStateAttribute::ASPECT_RATIO: {
                LOGD("Setting ASPECT Ration state %{public}d", attribute->id_);
                auto valueState =
                    AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, AnimatableDimension>>(attribute);
                SetAspectRatio(valueState->value_);
            } break;

            case BoxStateAttribute::BORDER: {
                // We replace support for border object with updates to border components:
                // color, style, width, radius
                // The reason - developer does not have to provide all border properties
                // when border is set.
                // See JSViewAbstract::JsBorder for details
            } break;

            case BoxStateAttribute::GRADIENT: {
                auto gradientState = AceType::DynamicCast<StateAttributeValue<BoxStateAttribute, Gradient>>(attribute);
                LOGD("Setting Gradient state %{public}d", state);
                GetBackDecoration()->SetGradient(gradientState->value_);
            } break;
            default:
                break;
        }
    }
    if (updated) {
        MarkNeedLayout();
    }
};

} // namespace OHOS::Ace
