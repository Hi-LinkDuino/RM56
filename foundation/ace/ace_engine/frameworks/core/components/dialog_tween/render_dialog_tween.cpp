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

#include "core/components/dialog_tween/render_dialog_tween.h"

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/log/event_report.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components/stack/stack_element.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {
namespace {

// Using UX spec: Constrain max height within 4/5 of screen height.
constexpr double DIALOG_HEIGHT_RATIO = 0.8;
constexpr double DIALOG_HEIGHT_RATIO_FOR_LANDSCAPE = 0.9;
constexpr double DIALOG_HEIGHT_RATIO_FOR_CAR = 0.95;
constexpr double DIALOG_DRAG_RATIO = 0.5;
constexpr int32_t POSITIVE_SUCCESS_ID = 0;
constexpr int32_t NEGATIVE_SUCCESS_ID = 1;
constexpr int32_t NEUTRAL_SUCCESS_ID = 2;

} // namespace

RenderDialogTween::RenderDialogTween()
{
    clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
        auto dialog = weak.Upgrade();
        if (dialog) {
            dialog->HandleClick(info.GetLocalLocation());
        }
    });
    dragDetector_ = AceType::MakeRefPtr<DragRecognizer>(Axis::FREE);
    dragDetector_->SetOnDragStart([weak = WeakClaim(this)](const DragStartInfo& startInfo) {
        auto dialog = weak.Upgrade();
        if (dialog) {
            dialog->dragStart_ = startInfo.GetLocalLocation().GetX();
            dialog->dragY_ = startInfo.GetLocalLocation().GetY();
            dialog->dragX_ = startInfo.GetLocalLocation().GetX();
            dialog->lastPositionX_ = startInfo.GetLocalLocation().GetX();
            dialog->lastPositionY_ = startInfo.GetLocalLocation().GetY();
            dialog->init_ = false;
            if (dialog->maskDragRegion_.ContainsInRegion(dialog->dragX_, dialog->dragY_)) {
                dialog->isDraging_ = true;
            } else {
                dialog->isDraging_ = false;
            }
        }
    });
    dragDetector_->SetOnDragEnd([weak = WeakClaim(this)](const DragEndInfo& endInfo) {
        auto dialog = weak.Upgrade();
        if (dialog) {
            double dragEnd = endInfo.GetLocalLocation().GetX();
            if (!dialog->isDraging_ &&
                GreatOrEqual(dragEnd - dialog->dragStart_, dialog->GetLayoutSize().Width() * DIALOG_DRAG_RATIO)) {
                dialog->PopDialog();
            }
            dialog->lastPositionX_ = endInfo.GetLocalLocation().GetX();
            dialog->lastPositionY_ = endInfo.GetLocalLocation().GetY();
        }
    });
    dragDetector_->SetOnDragUpdate([weak = WeakClaim(this)](const DragUpdateInfo& info) {
        auto dialog = weak.Upgrade();
        if (dialog) {
            if (dialog->isDragable_ && dialog->isDraging_) {
                dialog->HandleDragUpdate(info.GetLocalLocation());
            }
        }
    });
    init_ = true;
}

RenderDialogTween::~RenderDialogTween()
{
    if (!popDialog_ && onStatusChanged_) {
        onStatusChanged_(false);
        popDialog_ = true;
    }
    auto dialogTweenComponent = weakDialogTweenComponent_.Upgrade();
    if (dialogTweenComponent) {
        RemoveBackendEvent(dialogTweenComponent);
    }

    auto accessibilityNode = accessibilityNode_.Upgrade();
    if (accessibilityNode) {
        accessibilityNode->ClearRect();
    }
}

RefPtr<RenderNode> RenderDialogTween::Create()
{
    return AceType::MakeRefPtr<RenderDialogTween>();
}

void RenderDialogTween::Update(const RefPtr<Component>& component)
{
    const RefPtr<DialogTweenComponent> dialog = AceType::DynamicCast<DialogTweenComponent>(component);
    if (!dialog) {
        LOGE("RenderDialogTween update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    weakDialogTweenComponent_ = dialog;
    autoCancel_ = dialog->GetAutoCancel();
    onSuccess_ = AceAsyncEvent<void(int32_t)>::Create(dialog->GetOnSuccessId(), context_);
    onCancel_ = AceAsyncEvent<void()>::Create(dialog->GetOnCancelId(), context_);
    onComplete_ = AceAsyncEvent<void()>::Create(dialog->GetOnCompleteId(), context_);
    onStatusChanged_ = dialog->GetOnStatusChanged();
    animator_ = dialog->GetParentAnimator();
    composedId_ = dialog->GetComposedId();
    dialogId_ = dialog->GetDialogId();
    customDialogId_ = dialog->GetCustomDialogId();
    data_ = dialog->GetData();
    isLimit_ = dialog->GetDialogLimit();
    isSetMargin_ = dialog->IsSetMargin();
    isDragable_ = dialog->IsDragable();
    if (isSetMargin_) {
        margin_ = dialog->GetMargin();
    }
    alignment_ = dialog->GetAlignment();
    offset_ = dialog->GetOffset();
    gridCount_ = dialog->GetGridCount();

    if (dialog->IsMenu()) {
        auto menuSuccessId = dialog->GetMenuSuccessId();
        for (size_t index = 0; index < menuSuccessId.size(); ++index) {
            const auto context = GetContext().Upgrade();
            if (context && context->GetIsDeclarative()) {
                BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
                    menuSuccessId[index], [weak = WeakClaim(this), index](const ClickInfo& info) {
                        auto dialog = weak.Upgrade();
                        dialog->CallOnSuccess(index);
                    });
            } else {
                BackEndEventManager<void()>::GetInstance().BindBackendEvent(
                    menuSuccessId[index], [weak = WeakClaim(this), index]() {
                        auto dialog = weak.Upgrade();
                        dialog->CallOnSuccess(index);
                    });
            }
        }
    }
    BindButtonEvent(dialog);
    MarkNeedLayout();
}

void RenderDialogTween::BindButtonEvent(const RefPtr<DialogTweenComponent>& dialog)
{
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
            dialog->GetOnPositiveSuccessId(), [weak = WeakClaim(this)](const ClickInfo& info) {
                auto dialog = weak.Upgrade();
                dialog->CallOnSuccess(POSITIVE_SUCCESS_ID);
            });

        BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
            dialog->GetOnNegativeSuccessId(), [weak = WeakClaim(this)](const ClickInfo& info) {
                auto dialog = weak.Upgrade();
                dialog->CallOnSuccess(NEGATIVE_SUCCESS_ID);
            });

        BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
            dialog->GetOnNeutralSuccessId(), [weak = WeakClaim(this)](const ClickInfo& info) {
                auto dialog = weak.Upgrade();
                dialog->CallOnSuccess(NEUTRAL_SUCCESS_ID);
            });
        return;
    }

    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        dialog->GetOnPositiveSuccessId(), [weak = WeakClaim(this)]() {
            auto dialog = weak.Upgrade();
            dialog->CallOnSuccess(POSITIVE_SUCCESS_ID);
        });

    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        dialog->GetOnNegativeSuccessId(), [weak = WeakClaim(this)]() {
            auto dialog = weak.Upgrade();
            dialog->CallOnSuccess(NEGATIVE_SUCCESS_ID);
        });

    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        dialog->GetOnNeutralSuccessId(), [weak = WeakClaim(this)]() {
            auto dialog = weak.Upgrade();
            dialog->CallOnSuccess(NEUTRAL_SUCCESS_ID);
        });
}

void RenderDialogTween::HandleDragUpdate(const Offset& currentPoint)
{
    dragX_ = currentPoint.GetX();
    dragY_ = currentPoint.GetY();
    MarkNeedLayout();
}

void RenderDialogTween::CallOnSuccess(int32_t successType)
{
    if (onStatusChanged_) {
        onStatusChanged_(false);
        popDialog_ = true;
    }
    const auto context = context_.Upgrade();
    if (!context) {
        LOGE("the context is null");
        return;
    }
    const auto& lastStack = context->GetLastStack();
    if (!lastStack) {
        LOGE("the lastStack is null");
        return;
    }
    if (animator_) {
        animator_->AddStopListener([successType, lastStack, weak = WeakClaim(this), dialogId = dialogId_] {
            auto dialog = weak.Upgrade();
            if (!dialog) {
                return;
            }
            lastStack->PopDialog(dialogId);
            if (dialog->onSuccess_) {
                dialog->onSuccess_(successType);
            }
            if (dialog->onComplete_) {
                dialog->onComplete_();
            }
        });
        animator_->Play();
    } else {
        lastStack->PopDialog(dialogId_);
        if (onSuccess_) {
            onSuccess_(successType);
        }
        if (onComplete_) {
            onComplete_();
        }
    }
    RemoveAccessibilityNode();
}

double RenderDialogTween::GetMaxWidthBasedOnGridType(
    const RefPtr<GridColumnInfo>& info, GridSizeType type, DeviceType deviceType)
{
    if (gridCount_ > 0) {
        return info->GetWidth(std::min(gridCount_, GridSystemManager::GetInstance().GetCurrentGridInfo().maxColumns));
    }

    if (deviceType == DeviceType::WATCH) {
        if (type == GridSizeType::SM) {
            return info->GetWidth(3);
        } else if (type == GridSizeType::MD) {
            return info->GetWidth(4);
        } else if (type == GridSizeType::LG) {
            return info->GetWidth(5);
        } else {
            LOGI("GetMaxWidthBasedOnGridType is undefined");
            return info->GetWidth(5);
        }
    } else if (deviceType == DeviceType::PHONE) {
        if (type == GridSizeType::SM) {
            return info->GetWidth(4);
        } else if (type == GridSizeType::MD) {
            return info->GetWidth(5);
        } else if (type == GridSizeType::LG) {
            return info->GetWidth(6);
        } else {
            LOGI("GetMaxWidthBasedOnGridType is undefined");
            return info->GetWidth(6);
        }
    } else if (deviceType == DeviceType::CAR) {
        if (type == GridSizeType::SM) {
            return info->GetWidth(4);
        } else if (type == GridSizeType::MD) {
            return info->GetWidth(6);
        } else if (type == GridSizeType::LG) {
            return info->GetWidth(8);
        } else {
            LOGI("GetMaxWidthBasedOnGridType is undefined");
            return info->GetWidth(8);
        }
    } else {
        if (type == GridSizeType::SM) {
            return info->GetWidth(2);
        } else if (type == GridSizeType::MD) {
            return info->GetWidth(3);
        } else if (type == GridSizeType::LG) {
            return info->GetWidth(4);
        } else {
            LOGI("GetMaxWidthBasedOnGridType is undefined");
            return info->GetWidth(4);
        }
    }
}

void RenderDialogTween::PerformLayout()
{
    LayoutParam innerLayout = GetLayoutParam();
    auto maxSize = innerLayout.GetMaxSize();
    // If max size is INFINITE, use viewport of parent.
    if (maxSize == Size(std::numeric_limits<double>::max(), std::numeric_limits<double>::max())) {
        auto parent = GetParent().Upgrade();
        if (parent) {
            maxSize = parent->GetChildViewPort();
        }
    }
    innerLayout.SetMaxSize(maxSize);
    ComputeInnerLayoutParam(innerLayout);
    if (GetChildren().empty()) {
        SetLayoutSize(maxSize);
        return;
    }
    const auto& child = GetChildren().front();
    child->Layout(innerLayout);
    auto childSize = child->GetLayoutSize();
    topLeftPoint_ = ComputeChildPosition(childSize);
    child->SetPosition(topLeftPoint_);
    UpdateTouchRegion(topLeftPoint_, maxSize, childSize);
    SetLayoutSize(maxSize);
    lastPositionX_ = dragX_;
    lastPositionY_ = dragY_;
}

void RenderDialogTween::ComputeInnerLayoutParam(LayoutParam& innerLayout)
{
    auto maxSize = innerLayout.GetMaxSize();
    // Set different layout param for different devices
    auto gridSizeType = GridSystemManager::GetInstance().GetCurrentSize();
    RefPtr<GridColumnInfo> columnInfo;
    if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::CAR_DIALOG);
    } else {
        columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::DIALOG);
    }
    columnInfo->GetParent()->BuildColumnWidth();
    auto width = GetMaxWidthBasedOnGridType(columnInfo, gridSizeType, SystemProperties::GetDeviceType());
    if (!isLimit_) {
        innerLayout.SetMinSize(Size(0.0, 0.0));
        innerLayout.SetMaxSize(Size(maxSize.Width(), maxSize.Height()));
    } else if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        innerLayout.SetMinSize(Size(width, 0.0));
        innerLayout.SetMaxSize(Size(width, maxSize.Height()));
    } else if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        if (SystemProperties::GetDevcieOrientation() == DeviceOrientation::LANDSCAPE) {
            innerLayout.SetMinSize(Size(width, 0.0));
            innerLayout.SetMaxSize(Size(width, maxSize.Height() * DIALOG_HEIGHT_RATIO_FOR_LANDSCAPE));
        } else {
            innerLayout.SetMinSize(Size(width, 0.0));
            innerLayout.SetMaxSize(Size(width, maxSize.Height() * DIALOG_HEIGHT_RATIO));
        }
    } else if (SystemProperties::GetDeviceType() == DeviceType::CAR) {
        innerLayout.SetMinSize(Size(width, 0.0));
        innerLayout.SetMaxSize(Size(width, maxSize.Height() * DIALOG_HEIGHT_RATIO_FOR_CAR));
    } else {
        innerLayout.SetMinSize(Size(width, 0.0));
        innerLayout.SetMaxSize(Size(width, maxSize.Height() * DIALOG_HEIGHT_RATIO));
    }
}

Offset RenderDialogTween::ComputeChildPosition(const Size& childSize) const
{
    Offset topLeftPoint;
    auto context = context_.Upgrade();
    auto theme = GetTheme<DialogTheme>();
    if (!theme || !context) {
        return topLeftPoint;
    }

    auto maxSize = GetLayoutParam().GetMaxSize();
    // If max size is INFINITE, use viewport of parent.
    if (maxSize == Size(std::numeric_limits<double>::max(), std::numeric_limits<double>::max())) {
        auto parent = GetParent().Upgrade();
        if (parent) {
            maxSize = parent->GetChildViewPort();
        }
    }
    if (isDraging_) {
        topLeftPoint =
            Offset(topLeftPoint_.GetX() + (dragX_ - lastPositionX_), topLeftPoint_.GetY() + (dragY_ - lastPositionY_));
        return topLeftPoint;
    }

    Offset dialogOffset =
        Offset(NormalizePercentToPx(offset_.GetX(), false, true), NormalizePercentToPx(offset_.GetY(), true, true));
    if (offset_.GetX().Unit() == DimensionUnit::PERCENT) {
        dialogOffset.SetX(offset_.GetX().Value() * childSize.Width());
    } else {
        dialogOffset.SetX(NormalizeToPx(offset_.GetX()));
    }
    if (offset_.GetY().Unit() == DimensionUnit::PERCENT) {
        dialogOffset.SetY(offset_.GetY().Value() * childSize.Height());
    } else {
        dialogOffset.SetY(NormalizeToPx(offset_.GetY()));
    }
    if (init_) {
        if (SetAlignmentSwitch(maxSize, childSize, topLeftPoint)) {
            return topLeftPoint + dialogOffset;
        }

        if (context->GetIsDeclarative()) {
            topLeftPoint = Offset(maxSize.Width() - childSize.Width(), maxSize.Height() - childSize.Height()) / 2.0;
            return topLeftPoint + dialogOffset;
        }

        // Set different positions for different devices
        if (SystemProperties::GetDeviceType() == DeviceType::PHONE &&
            SystemProperties::GetDevcieOrientation() == DeviceOrientation::PORTRAIT) {
            topLeftPoint = Offset((maxSize.Width() - childSize.Width()) / 2.0,
                maxSize.Height() - childSize.Height() - (isSetMargin_ ? 0.0 : NormalizeToPx(theme->GetMarginBottom())));
        } else {
            topLeftPoint =
                Offset((maxSize.Width() - childSize.Width()) / 2.0, (maxSize.Height() - childSize.Height()) / 2.0);
        }
    }
    return topLeftPoint + dialogOffset;
}

bool RenderDialogTween::SetAlignmentSwitch(const Size& maxSize, const Size& childSize, Offset& topLeftPoint) const
{
    // If alignment is setted, compute position with alignment and offset.
    if (alignment_ != DialogAlignment::DEFAULT) {
        switch (alignment_) {
            case DialogAlignment::TOP:
                topLeftPoint = Offset((maxSize.Width() - childSize.Width()) / 2.0, 0.0);
                break;
            case DialogAlignment::CENTER:
                topLeftPoint = Offset(maxSize.Width() - childSize.Width(), maxSize.Height() - childSize.Height()) / 2.0;
                break;
            case DialogAlignment::BOTTOM:
                topLeftPoint =
                    Offset((maxSize.Width() - childSize.Width()) / 2.0, maxSize.Height() - childSize.Height());
                break;
            case DialogAlignment::TOP_START:
                topLeftPoint = Offset(0.0, 0.0);
                break;
            case DialogAlignment::TOP_END:
                topLeftPoint = Offset(maxSize.Width() - childSize.Width(), 0.0);
                break;
            case DialogAlignment::CENTER_START:
                topLeftPoint = Offset(0.0, maxSize.Height() - childSize.Height()) / 2.0;
                break;
            case DialogAlignment::CENTER_END:
                topLeftPoint =
                    Offset(maxSize.Width() - childSize.Width(), (maxSize.Height() - childSize.Height()) / 2.0);
                break;
            case DialogAlignment::BOTTOM_START:
                topLeftPoint = Offset(0.0, maxSize.Height() - childSize.Height());
                break;
            case DialogAlignment::BOTTOM_END:
                topLeftPoint = Offset(maxSize.Width() - childSize.Width(), maxSize.Height() - childSize.Height());
                break;
            default:
                topLeftPoint = Offset(maxSize.Width() - childSize.Width(), maxSize.Height() - childSize.Height()) / 2.0;
                break;
        }
        return true;
    }
    return false;
}

void RenderDialogTween::UpdateTouchRegion(const Offset& topLeftPoint, const Size& maxSize, const Size& childSize)
{
    double left = margin_.Left().Unit() == DimensionUnit::PERCENT ? margin_.Left().Value() * maxSize.Width()
                                                                  : NormalizeToPx(margin_.Left());
    double top = margin_.Top().Unit() == DimensionUnit::PERCENT ? margin_.Top().Value() * maxSize.Height()
                                                                : NormalizeToPx(margin_.Top());
    Offset touchTopLeft = topLeftPoint + (isSetMargin_ ? Offset(left, top) : Offset(0.0, 0.0));

    double right = margin_.Right().Unit() == DimensionUnit::PERCENT ? margin_.Right().Value() * maxSize.Width()
                                                                    : NormalizeToPx(margin_.Right());
    double bottom = margin_.Bottom().Unit() == DimensionUnit::PERCENT ? margin_.Bottom().Value() * maxSize.Height()
                                                                      : NormalizeToPx(margin_.Bottom());
    Offset touchBottomRight = topLeftPoint + childSize - (isSetMargin_ ? Offset(right, bottom) : Offset(0.0, 0.0));

    Offset dragBottomRight = touchBottomRight;
    dragBottomRight.SetY(touchTopLeft.GetY() + DRAG_BAR_HEIGHT);

    maskTouchRegion_ = TouchRegion(touchTopLeft, touchBottomRight);
    LOGD("top: %{public}lf, bottom:%{public}lf, left: %{public}lf, right:%{public}lf isSetMargin:%{public}d",
        touchTopLeft.GetY(), touchBottomRight.GetY(), touchTopLeft.GetX(), touchBottomRight.GetX(), isSetMargin_);
    maskDragRegion_ = TouchRegion(touchTopLeft, dragBottomRight);
}

void RenderDialogTween::OnPaintFinish()
{
    InitAccessibilityEventListener();
    if (onStatusChanged_) {
        onStatusChanged_(true);
    }
}

void RenderDialogTween::HandleClick(const Offset& clickPosition)
{
    if (autoCancel_ && !maskTouchRegion_.ContainsInRegion(clickPosition.GetX(), clickPosition.GetY())) {
        PopDialog();
    }
}

void RenderDialogTween::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    clickDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickDetector_);
    if (isDragable_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
}

bool RenderDialogTween::PopDialog()
{
    if (onStatusChanged_) {
        onStatusChanged_(false);
        popDialog_ = true;
    }
    const auto context = context_.Upgrade();
    if (!context) {
        LOGE("the context is null");
        return false;
    }
    const auto& lastStack = context->GetLastStack();
    if (!lastStack) {
        LOGE("the lastStack is null");
        return false;
    }
    if (animator_) {
        animator_->AddStopListener([lastStack, weak = AceType::WeakClaim(this), dialogId = dialogId_] {
            auto dialog = weak.Upgrade();
            if (!dialog) {
                return;
            }
            lastStack->PopDialog(dialogId);
            if (dialog->onCancel_) {
                dialog->onCancel_();
            }
            if (dialog->onComplete_) {
                dialog->onComplete_();
            }
        });
        animator_->Play();
    } else {
        lastStack->PopDialog(dialogId_);
        if (onCancel_) {
            onCancel_();
        }
        if (onComplete_) {
            onComplete_();
        }
    }
    RemoveAccessibilityNode();
    return true;
}

void RenderDialogTween::SetAnimator(const RefPtr<Animator>& animator)
{
    animator_ = animator;
}

void RenderDialogTween::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetFocusableState(true);
    accessibilityNode->SetText(data_);

    accessibilityNode->AddSupportAction(AceAction::CUSTOM_ACTION);
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->AddSupportAction(AceAction::ACTION_FOCUS);
    accessibilityNode->AddSupportAction(AceAction::ACTION_LONG_CLICK);
    accessibilityNode->AddSupportAction(AceAction::GLOBAL_ACTION_BACK);
    accessibilityNode->SetLongClickableState(true);

    accessibilityNode->SetActionLongClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& dialogTween = weakPtr.Upgrade();
        if (dialogTween) {
            dialogTween->PopDialog();
        }
    });

    // RenderDialogTween's size is 0*0, use parent's rect,
    // or no child on dialog can get focused by click in accessibility mode
    auto parent = accessibilityNode->GetParentNode();
    if (parent) {
        accessibilityNode->SetRect(parent->GetRect());
    }
}

void RenderDialogTween::RemoveAccessibilityNode()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    const auto& accessibilityManager = context->GetAccessibilityManager();
    if (accessibilityManager) {
        // no new accessibility node is created with DialogTween in JS app
        if (context->GetIsDeclarative()) {
            accessibilityManager->RemoveAccessibilityNodeById(composedId_);
        }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        auto node = accessibilityManager->GetAccessibilityNodeById(customDialogId_);
        accessibilityManager->ClearNodeRectInfo(node, true);
#endif
    }
}

void RenderDialogTween::RemoveBackendEvent(const RefPtr<DialogTweenComponent>& component)
{
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().RemoveBackEndEvent(
            component->GetOnPositiveSuccessId());
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().RemoveBackEndEvent(
            component->GetOnNegativeSuccessId());
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().RemoveBackEndEvent(
            component->GetOnNeutralSuccessId());
    } else {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(component->GetOnPositiveSuccessId());
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(component->GetOnNegativeSuccessId());
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(component->GetOnNeutralSuccessId());
    }
    BackEndEventManager<void(int32_t)>::GetInstance().RemoveBackEndEvent(component->GetOnSuccessId());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(component->GetOnCancelId());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(component->GetOnCompleteId());
}

bool RenderDialogTween::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button != MouseButton::NONE_BUTTON && event.button != MouseButton::LEFT_BUTTON &&
        event.action == MouseAction::PRESS) {
        HandleClick({ event.x, event.y });
    }
    return true;
}

} // namespace OHOS::Ace
