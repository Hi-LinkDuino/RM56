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

#include "core/components/text/render_text.h"

#include "base/geometry/size.h"
#include "base/log/dump_log.h"
#include "core/common/font_manager.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/inspector/utils.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderText::~RenderText()
{
    auto context = context_.Upgrade();
    if (context) {
        context->RemoveFontNode(AceType::WeakClaim(this));
        auto fontManager = context->GetFontManager();
        if (fontManager) {
            fontManager->UnRegisterCallback(AceType::WeakClaim(this));
            fontManager->RemoveVariationNode(WeakClaim(this));
        }
    }
}

void RenderText::Update(const RefPtr<Component>& component)
{
    text_ = AceType::DynamicCast<TextComponent>(component);
    CheckIfNeedMeasure();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("the context is nullptr in text update");
        return;
    }
    // Register callback for fonts.
    auto callback = [weakText = AceType::WeakClaim(this)] {
        auto text = weakText.Upgrade();
        if (text) {
            text->isCallbackCalled_ = true;
            text->MarkNeedLayout();
        }
    };

    auto fontManager = context->GetFontManager();
    if (fontManager) {
        for (const auto& familyName : textStyle_.GetFontFamilies()) {
            fontManager->RegisterCallback(AceType::WeakClaim(this), familyName, callback);
        }
        fontManager->AddVariationNode(WeakClaim(this));
    }
    if (isFocus_) {
        textStyle_.SetTextColor(focusColor_);
    }

    if (textStyle_.IsAllowScale() || textStyle_.GetFontSize().Unit() == DimensionUnit::FP) {
        context->AddFontNode(AceType::WeakClaim(this));
    }
}

void RenderText::OnPaintFinish()
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    UpdateAccessibilityText();
#endif
}

void RenderText::UpdateAccessibilityText()
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto viewScale = context->GetViewScale();
    if (NearZero(viewScale)) {
        return;
    }
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    if (GetChildren().empty()) {
        if (text_) {
            accessibilityNode->SetText(text_->GetData());
        }
    } else {
        std::string accessibilityText;
        for (const auto& child : GetChildren()) {
            auto renderTextSpan = AceType::DynamicCast<RenderTextSpan>(child);
            if (renderTextSpan) {
                accessibilityText += renderTextSpan->GetSpanData();
            }
        }
        accessibilityNode->SetText(accessibilityText);
    }
    if (!accessibilityNode->GetVisible()) { // Set 0 to item when whole outside of view port.
        accessibilityNode->SetWidth(0.0);
        accessibilityNode->SetHeight(0.0);
        accessibilityNode->SetTop(0.0);
        accessibilityNode->SetLeft(0.0);
        return;
    }
    if (accessibilityNode->IsValidRect()) {
        return; // Rect already clamp by viewport, no need to set again.
    }
    Size size = GetLayoutSize();
    Offset globalOffset = GetGlobalOffset();
    PositionInfo positionInfo = { (size.Width()) * viewScale, (size.Height()) * viewScale,
        (globalOffset.GetX()) * viewScale, (globalOffset.GetY()) * viewScale };
    accessibilityNode->SetPositionInfo(positionInfo);
    accessibilityNode->SetIsMultiLine(GetTextLines() > 1);
}

void RenderText::PerformLayout()
{
    auto pipelineContext = GetContext().Upgrade();
    if ((textStyle_.IsAllowScale() || textStyle_.GetFontSize().Unit() == DimensionUnit::FP) && pipelineContext &&
        !NearEqual(fontScale_, pipelineContext->GetFontScale())) {
        needMeasure_ = true;
        fontScale_ = pipelineContext->GetFontScale();
    }
    if (pipelineContext) {
        UpdateIfChanged(dipScale_, pipelineContext->GetDipScale());
    }
    Size size = Measure();
    SetLayoutSize(GetLayoutParam().Constrain(size));
    for (const auto& spanChild : GetChildren()) {
        if (spanChild) {
            const auto& param = GetLayoutParam();
            spanChild->Layout(param);
        }
    }
}

bool RenderText::TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
    TouchTestResult& result)
{
    if (GetDisableTouchEvent() || disabled_) {
        return false;
    }

    // Since the paintRect is relative to parent, use parent local point to perform touch test.
    if (!InTouchRectList(parentLocalPoint, GetTouchRectList())) {
        return false;
    }

    // Reset flag firstly.
    needClickDetector_ = false;
    needLongPressDetector_ = false;
    needTouchDetector_ = false;
    // Calculates the local point location in this node.
    const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
    Offset localOffset = Offset(localPoint.GetX(), localPoint.GetY());
    // If span of touch position has click event, need add click detector.
    if (!GetEventMarker(GetTouchPosition(localOffset), GestureType::CLICK).IsEmpty() ||
        !GetEventMarker(GetTouchPosition(localOffset), GestureType::REMOTE_MESSAGE).IsEmpty()) {
        needClickDetector_ = true;
    }
    // If span of touch position has long press event, need add long press detector.
    if (!GetEventMarker(GetTouchPosition(localOffset), GestureType::LONG_PRESS).IsEmpty()) {
        needLongPressDetector_ = true;
    }
    // If span of touch position has touch event, need add touch detector.
    if (!GetEventMarker(GetTouchPosition(localOffset), GestureType::TOUCH_START).IsEmpty() ||
        !GetEventMarker(GetTouchPosition(localOffset), GestureType::TOUCH_MOVE).IsEmpty() ||
        !GetEventMarker(GetTouchPosition(localOffset), GestureType::TOUCH_END).IsEmpty() ||
        !GetEventMarker(GetTouchPosition(localOffset), GestureType::TOUCH_CANCEL).IsEmpty()) {
        needTouchDetector_ = true;
    }
    if (!needClickDetector_ && !needLongPressDetector_ && !needTouchDetector_) {
        return false;
    }

    auto beforeSize = result.size();
    if (touchable_) {
        // Calculates the coordinate offset in this node.
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
    }
    auto endSize = result.size();
    return beforeSize != endSize;
}

void RenderText::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (GetChildren().empty()) {
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (needTouchDetector_) {
        if (!rawRecognizer_) {
            rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
            rawRecognizer_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
                auto text = weak.Upgrade();
                if (text && !info.GetTouches().empty()) {
                    text->HandleTouchEvent(GestureType::TOUCH_START,
                        info.GetTouches().front().GetLocalLocation() - text->GetGlobalOffset());
                }
            });

            rawRecognizer_->SetOnTouchMove([weak = WeakClaim(this)](const TouchEventInfo& info) {
                auto text = weak.Upgrade();
                if (text && !info.GetTouches().empty()) {
                    text->HandleTouchEvent(GestureType::TOUCH_MOVE,
                        info.GetTouches().front().GetLocalLocation() - text->GetGlobalOffset());
                }
            });

            rawRecognizer_->SetOnTouchUp([weak = WeakClaim(this)](const TouchEventInfo& info) {
                auto text = weak.Upgrade();
                if (text && !info.GetTouches().empty()) {
                    text->HandleTouchEvent(
                        GestureType::TOUCH_END, info.GetTouches().front().GetLocalLocation() - text->GetGlobalOffset());
                }
            });

            rawRecognizer_->SetOnTouchCancel([weak = WeakClaim(this)](const TouchEventInfo& info) {
                auto text = weak.Upgrade();
                if (text && !info.GetTouches().empty()) {
                    text->HandleTouchEvent(GestureType::TOUCH_CANCEL,
                        info.GetTouches().front().GetLocalLocation() - text->GetGlobalOffset());
                }
            });
        }
        rawRecognizer_->SetTouchRestrict(touchRestrict);
        rawRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(rawRecognizer_);
        needTouchDetector_ = false;
    }

    if (needClickDetector_) {
        if (!clickDetector_) {
            clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
            clickDetector_->SetOnClick([weak = WeakClaim(this)](const ClickInfo& info) {
                auto text = weak.Upgrade();
                if (text) {
                    text->HandleClick(info);
                }
            });
            clickDetector_->SetRemoteMessage([weak = WeakClaim(this)](const ClickInfo& info) {
                auto text = weak.Upgrade();
                if (text) {
                    text->HandleRemoteMessage(info);
                }
            });
        }
        clickDetector_->SetCoordinateOffset(coordinateOffset);
        clickDetector_->SetTouchRestrict(touchRestrict);
        clickDetector_->SetIsExternalGesture(true);
        result.emplace_back(clickDetector_);
        needClickDetector_ = false;
    }

    if (needLongPressDetector_) {
        if (!longPressRecognizer_) {
            longPressRecognizer_ = AceType::MakeRefPtr<LongPressRecognizer>(context_);
            longPressRecognizer_->SetOnLongPress([weak = WeakClaim(this)](const LongPressInfo& info) {
                auto text = weak.Upgrade();
                if (text) {
                    text->HandleLongPress(info.GetLocalLocation());
                }
            });
        }
        longPressRecognizer_->SetCoordinateOffset(coordinateOffset);
        longPressRecognizer_->SetTouchRestrict(touchRestrict);
        result.emplace_back(longPressRecognizer_);
        needLongPressDetector_ = false;
    }
}

void RenderText::FireEvent(const EventMarker& marker)
{
    if (marker.IsEmpty()) {
        return;
    }

    auto func = AceAsyncEvent<void()>::Create(marker, context_);
    if (func) {
        func();
    }
}

void RenderText::HandleTouchEvent(GestureType type, const Offset& touchPosition)
{
    if (type == GestureType::TOUCH_START) {
        touchStartPosition_ = GetTouchPosition(touchPosition);
    }

    auto eventMarker = GetEventMarker(touchStartPosition_, type);
    if (!eventMarker.IsEmpty()) {
        FireEvent(eventMarker);
        return;
    }

    // If span has not touch event, use touch event of text.
    if (!text_) {
        return;
    }
    auto declaration = text_->GetDeclaration();
    if (!declaration) {
        return;
    }
    auto& rawEvent = static_cast<CommonRawEvent&>(declaration->GetEvent(EventTag::COMMON_RAW_EVENT));
    if (!rawEvent.IsValid()) {
        return;
    }

    switch (type) {
        case GestureType::TOUCH_START:
            eventMarker = rawEvent.touchStart.eventMarker;
            break;
        case GestureType::TOUCH_MOVE:
            eventMarker = rawEvent.touchMove.eventMarker;
            break;
        case GestureType::TOUCH_END:
            eventMarker = rawEvent.touchEnd.eventMarker;
            break;
        case GestureType::TOUCH_CANCEL:
            eventMarker = rawEvent.touchCancel.eventMarker;
            break;
        default:
            break;
    }
    FireEvent(eventMarker);
}

void RenderText::HandleClick(const ClickInfo& info)
{
    auto clickPosition = info.GetLocalLocation();
    auto clickMarker = GetEventMarker(GetTouchPosition(clickPosition), GestureType::CLICK);
    // If span has not click event, use click event of text.
    if (text_ && clickMarker.IsEmpty()) {
        auto declaration = text_->GetDeclaration();
        if (declaration) {
            auto& gestureEvent =
                static_cast<CommonGestureEvent&>(declaration->GetEvent(EventTag::COMMON_GESTURE_EVENT));
            if (gestureEvent.IsValid() && !gestureEvent.click.eventMarker.IsEmpty()) {
                clickMarker = gestureEvent.click.eventMarker;
            }
        }
    }

    auto onClick = AceAsyncEvent<void(const ClickInfo&)>::Create(clickMarker, context_);
    if (onClick) {
        onClick(info);
    }
}

void RenderText::HandleRemoteMessage(const ClickInfo& info)
{
    auto clickPosition = info.GetLocalLocation();
    auto clickMarker = GetEventMarker(GetTouchPosition(clickPosition), GestureType::REMOTE_MESSAGE);
    // If span has not click event, use click event of text.
    if (text_ && clickMarker.IsEmpty()) {
        auto declaration = text_->GetDeclaration();
        if (declaration) {
            auto& gestureEvent =
                static_cast<CommonGestureEvent&>(declaration->GetEvent(EventTag::COMMON_REMOTE_MESSAGE_GRESURE_EVENT));
            if (gestureEvent.IsValid() && !gestureEvent.click.eventMarker.IsEmpty()) {
                clickMarker = gestureEvent.click.eventMarker;
            }
        }
    }

    auto remoteMessage = AceAsyncEvent<void(const ClickInfo&)>::Create(clickMarker, context_);
    if (remoteMessage) {
        remoteMessage(info);
    }
}

void RenderText::HandleLongPress(const Offset& longPressPosition)
{
    auto longPressMarker = GetEventMarker(GetTouchPosition(longPressPosition), GestureType::LONG_PRESS);
    // If span has not long press event, use long press event of text.
    if (text_ && longPressMarker.IsEmpty()) {
        auto declaration = text_->GetDeclaration();
        if (declaration) {
            auto& gestureEvent =
                static_cast<CommonGestureEvent&>(declaration->GetEvent(EventTag::COMMON_GESTURE_EVENT));
            if (gestureEvent.IsValid() && !gestureEvent.longPress.eventMarker.IsEmpty()) {
                longPressMarker = gestureEvent.longPress.eventMarker;
            }
        }
    }

    FireEvent(longPressMarker);
}

EventMarker RenderText::GetEventMarker(int32_t position, GestureType type)
{
    if (touchRegions_.empty()) {
        return EventMarker();
    }
    for (const auto& region : touchRegions_) {
        if (position < region.first) {
            auto markerIter = region.second.find(type);
            if (markerIter != region.second.end()) {
                return markerIter->second;
            }
            return EventMarker();
        }
    }
    return EventMarker();
}

void RenderText::OnStatusChanged(OHOS::Ace::RenderStatus renderStatus)
{
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        return;
    }

    if (renderStatus == RenderStatus::FOCUS) {
        textStyle_.SetTextColor(focusColor_);
        isFocus_ = true;
    } else {
        textStyle_.SetTextColor(lostFocusColor_);
        isFocus_ = false;
    }
    needMeasure_ = true;
    Measure();
    MarkNeedRender();
}

void RenderText::CheckIfNeedMeasure()
{
    if (!text_) {
        return;
    }

    if (text_->IsChanged()) {
        needMeasure_ = true;
    }
    UpdateIfChanged(textDirection_, text_->GetTextDirection());
    UpdateIfChanged(textStyle_, text_->GetTextStyle());
    UpdateIfChanged(focusColor_, text_->GetFocusColor());
    UpdateIfChanged(lostFocusColor_, textStyle_.GetTextColor());
    UpdateIfChanged(maxLines_, textStyle_.GetMaxLines());
    if (needMeasure_) {
        MarkNeedLayout();
    }
}

std::string RenderText::GetTextData() const
{
    return text_ ? text_->GetData() : "";
}

void RenderText::SetTextData(const std::string& textData)
{
    if (text_) {
        text_->SetData(textData);
    }
}

void RenderText::ClearRenderObject()
{
    RenderNode::ClearRenderObject();
    LOGD("TextNode ClearRenderObject");
    text_.Reset();
    textStyle_ = TextStyle();
    textDirection_ = TextDirection::LTR;
    focusColor_ = Color();
    lostFocusColor_ = Color();
    fontScale_ = 1.0;
    dipScale_ = 1.0;
    isFocus_ = false;
    needMeasure_ = true;
    isCallbackCalled_ = false;
}

Size RenderText::GetContentSize()
{
    if (textStyle_.GetAdaptTextSize()) {
        return Size();
    }
    // Make sure text's height is not clipped, width is not guard.
    auto measuredSize = Measure();
    if (textStyle_.GetTextOverflow() != TextOverflow::NONE || maxLines_ > 1) {
        measuredSize.SetWidth(0.0);
    }
    return measuredSize;
}

RefPtr<Component> RenderText::GetComponent()
{
    return text_;
}

void RenderText::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("FontColor: ").append(textStyle_.GetTextColor().ColorToString()));
    DumpLog::GetInstance().AddDesc(std::string("FontSize: ").append(textStyle_.GetFontSize().ToString()));
    DumpLog::GetInstance().AddDesc(
        std::string("FontStyle: ").append(V2::ConvertWrapFontStyleToStirng(textStyle_.GetFontStyle())));
    DumpLog::GetInstance().AddDesc(
        std::string("FontWeight: ").append(V2::ConvertWrapFontWeightToStirng(textStyle_.GetFontWeight())));
    std::string fontFamilies;
    for (const auto& family : textStyle_.GetFontFamilies()) {
        fontFamilies += family;
        fontFamilies += ",";
    }
    DumpLog::GetInstance().AddDesc(std::string("FontFamily: ").append(fontFamilies));
}

} // namespace OHOS::Ace
