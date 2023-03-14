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

#include "core/animation/animatable_properties.h"

#include "core/animation/animatable_data.h"
#include "core/animation/property_animation.h"
#include "core/components/box/render_box.h"
#include "core/components/display/render_display.h"
#include "core/components/positioned/render_positioned.h"

namespace OHOS::Ace {

void AnimatableProperties::UpdatePropAnimation(const PropAnimationMap& propAnimations)
{
    if (propAnimations.empty()) {
        return;
    }
    for (const auto& [type, animation] : propAnimations) {
        auto weakAnimation = AceType::WeakClaim(AceType::RawPtr(animation));
        switch (type) {
            case AnimatableType::PROPERTY_WIDTH: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto width = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && width) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPropWidth());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropWidth(width->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_HEIGHT: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto height = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && height) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPropHeight());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropHeight(height->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BG_COLOR: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto color = AceType::DynamicCast<AnimatableData<Color>>(data);
                        if (animatableProperties && color) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init =
                                    AceType::MakeRefPtr<AnimatableData<Color>>(animatableProperties->GetPropBgColor());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropBgColor(color->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_OPACITY: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto opacity = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && opacity) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init =
                                    AceType::MakeRefPtr<AnimatableData<float>>(animatableProperties->GetPropOpacity());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropOpacity(opacity->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_MARGIN_LEFT:
            case AnimatableType::PROPERTY_MARGIN_TOP:
            case AnimatableType::PROPERTY_MARGIN_RIGHT:
            case AnimatableType::PROPERTY_MARGIN_BOTTOM: {
                auto setter = DimensionHelper(&Edge::SetLeft, &Edge::Left);
                if (type == AnimatableType::PROPERTY_MARGIN_TOP) {
                    setter = DimensionHelper(&Edge::SetTop, &Edge::Top);
                } else if (type == AnimatableType::PROPERTY_MARGIN_RIGHT) {
                    setter = DimensionHelper(&Edge::SetRight, &Edge::Right);
                } else if (type == AnimatableType::PROPERTY_MARGIN_BOTTOM) {
                    setter = DimensionHelper(&Edge::SetBottom, &Edge::Bottom);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto margin = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && margin) {
                            if (animation && animation->GetInit() == nullptr) {
                                const Dimension& dimension = animatableProperties->GetMargin(setter);
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(dimension);
                                animation->SetInit(init);
                            }
                            animatableProperties->SetMargin(AnimatableDimension(margin->GetValue()), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_PADDING_LEFT:
            case AnimatableType::PROPERTY_PADDING_TOP:
            case AnimatableType::PROPERTY_PADDING_RIGHT:
            case AnimatableType::PROPERTY_PADDING_BOTTOM: {
                auto setter = DimensionHelper(&Edge::SetLeft, &Edge::Left);
                if (type == AnimatableType::PROPERTY_PADDING_TOP) {
                    setter = DimensionHelper(&Edge::SetTop, &Edge::Top);
                } else if (type == AnimatableType::PROPERTY_PADDING_RIGHT) {
                    setter = DimensionHelper(&Edge::SetRight, &Edge::Right);
                } else if (type == AnimatableType::PROPERTY_PADDING_BOTTOM) {
                    setter = DimensionHelper(&Edge::SetBottom, &Edge::Bottom);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto padding = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && padding) {
                            if (animation && animation->GetInit() == nullptr) {
                                const Dimension& dimension = animatableProperties->GetPadding(setter);
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(dimension);
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPadding(AnimatableDimension(padding->GetValue()), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BACKGROUND_POSITION: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto position = AceType::DynamicCast<AnimatableData<BackgroundImagePosition>>(data);
                        if (animatableProperties && position) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<BackgroundImagePosition>>(
                                    animatableProperties->GetPropBackgroundImagePosition());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropBackgroundPosition(position->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BACKGROUND_SIZE: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto size = AceType::DynamicCast<AnimatableData<BackgroundImageSize>>(data);
                        if (animatableProperties && size) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<BackgroundImageSize>>(
                                    animatableProperties->GetPropBackgroundImageSize());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropBackgroundSize(size->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_FILTER_BLUR: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto radius = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && radius) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<float>>(
                                    animatableProperties->GetBlurRadius().Value());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBlurRadius(
                                AnimatableDimension(radius->GetValue(), DimensionUnit::PX));
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BACKDROP_FILTER_BLUR: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto radius = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && radius) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<float>>(
                                    animatableProperties->GetBackdropRadius().Value());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBackdropRadius(
                                AnimatableDimension(radius->GetValue(), DimensionUnit::PX));
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_WINDOW_FILTER_BLUR: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto progress = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && progress) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<float>>(
                                    animatableProperties->GetWindowBlurProgress());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetWindowBlurProgress(progress->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BOX_SHADOW: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto shadow = AceType::DynamicCast<AnimatableData<Shadow>>(data);
                        if (animatableProperties && shadow) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init =
                                    AceType::MakeRefPtr<AnimatableData<Shadow>>(animatableProperties->GetPropShadow());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPropShadow(shadow->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BORDER_LEFT_WIDTH:
            case AnimatableType::PROPERTY_BORDER_TOP_WIDTH:
            case AnimatableType::PROPERTY_BORDER_RIGHT_WIDTH:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_WIDTH: {
                auto setter = BorderEdgeHelper(&Border::SetLeftEdge, &Border::Left);
                if (type == AnimatableType::PROPERTY_BORDER_TOP_WIDTH) {
                    setter = BorderEdgeHelper(&Border::SetTopEdge, &Border::Top);
                } else if (type == AnimatableType::PROPERTY_BORDER_RIGHT_WIDTH) {
                    setter = BorderEdgeHelper(&Border::SetRightEdge, &Border::Right);
                } else if (type == AnimatableType::PROPERTY_BORDER_BOTTOM_WIDTH) {
                    setter = BorderEdgeHelper(&Border::SetBottomEdge, &Border::Bottom);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto width = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && width) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<float>>(
                                    animatableProperties->GetBorderWidth(setter));
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBorderWidth(width->GetValue(), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BORDER_LEFT_COLOR:
            case AnimatableType::PROPERTY_BORDER_TOP_COLOR:
            case AnimatableType::PROPERTY_BORDER_RIGHT_COLOR:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_COLOR: {
                auto setter = BorderEdgeHelper(&Border::SetLeftEdge, &Border::Left);
                if (type == AnimatableType::PROPERTY_BORDER_TOP_COLOR) {
                    setter = BorderEdgeHelper(&Border::SetTopEdge, &Border::Top);
                } else if (type == AnimatableType::PROPERTY_BORDER_RIGHT_COLOR) {
                    setter = BorderEdgeHelper(&Border::SetRightEdge, &Border::Right);
                } else if (type == AnimatableType::PROPERTY_BORDER_BOTTOM_COLOR) {
                    setter = BorderEdgeHelper(&Border::SetBottomEdge, &Border::Bottom);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto color = AceType::DynamicCast<AnimatableData<Color>>(data);
                        if (animatableProperties && color) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Color>>(
                                    animatableProperties->GetBorderColor(setter));
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBorderColor(color->GetValue(), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BORDER_LEFT_STYLE:
            case AnimatableType::PROPERTY_BORDER_TOP_STYLE:
            case AnimatableType::PROPERTY_BORDER_RIGHT_STYLE:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_STYLE: {
                auto setter = BorderEdgeHelper(&Border::SetLeftEdge, &Border::Left);
                if (type == AnimatableType::PROPERTY_BORDER_TOP_STYLE) {
                    setter = BorderEdgeHelper(&Border::SetTopEdge, &Border::Top);
                } else if (type == AnimatableType::PROPERTY_BORDER_RIGHT_STYLE) {
                    setter = BorderEdgeHelper(&Border::SetRightEdge, &Border::Right);
                } else if (type == AnimatableType::PROPERTY_BORDER_BOTTOM_STYLE) {
                    setter = BorderEdgeHelper(&Border::SetBottomEdge, &Border::Bottom);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto borderStyle = AceType::DynamicCast<AnimatableData<BorderStyle>>(data);
                        if (animatableProperties && borderStyle) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<BorderStyle>>(
                                    animatableProperties->GetBorderStyle(setter));
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBorderStyle(borderStyle->GetValue(), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_BORDER_TOP_LEFT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_TOP_RIGHT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_LEFT_RADIUS:
            case AnimatableType::PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS: {
                auto setter = BorderRadiusHelper(&Border::SetTopLeftRadius, &Border::TopLeftRadius);
                if (type == AnimatableType::PROPERTY_BORDER_TOP_RIGHT_RADIUS) {
                    setter = BorderRadiusHelper(&Border::SetTopRightRadius, &Border::TopRightRadius);
                } else if (type == AnimatableType::PROPERTY_BORDER_BOTTOM_LEFT_RADIUS) {
                    setter = BorderRadiusHelper(&Border::SetBottomLeftRadius, &Border::BottomLeftRadius);
                } else if (type == AnimatableType::PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS) {
                    setter = BorderRadiusHelper(&Border::SetBottomRightRadius, &Border::BottomRightRadius);
                }
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), setter = setter, weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto radius = AceType::DynamicCast<AnimatableData<float>>(data);
                        if (animatableProperties && radius) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<float>>(
                                    animatableProperties->GetBorderRadius(setter));
                                animation->SetInit(init);
                            }
                            animatableProperties->SetBorderRadius(radius->GetValue(), setter);
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_POSITION_LEFT: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto left = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && left) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPositionLeft());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPositionLeft(left->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_POSITION_TOP: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto top = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && top) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPositionTop());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPositionTop(top->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_POSITION_RIGHT: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto right = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && right) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPositionRight());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPositionRight(right->GetValue());
                        }
                    });
                break;
            }
            case AnimatableType::PROPERTY_POSITION_BOTTOM: {
                animation->AddListener(
                    [weak = AceType::WeakClaim(this), weakAnimation](const RefPtr<Animatable>& data) {
                        auto animation = weakAnimation.Upgrade();
                        auto animatableProperties = weak.Upgrade();
                        auto bottom = AceType::DynamicCast<AnimatableData<Dimension>>(data);
                        if (animatableProperties && bottom) {
                            if (animation && animation->GetInit() == nullptr) {
                                auto init = AceType::MakeRefPtr<AnimatableData<Dimension>>(
                                    animatableProperties->GetPositionBottom());
                                animation->SetInit(init);
                            }
                            animatableProperties->SetPositionBottom(bottom->GetValue());
                        }
                    });
                break;
            }
            default:
                break;
        }
    }
}

void AnimatableProperties::SetPropWidth(const Dimension& width)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetWidth(width);
}

Dimension AnimatableProperties::GetPropWidth() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetWidthDimension();
}

void AnimatableProperties::SetPropHeight(const Dimension& height)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetHeight(height);
}

Dimension AnimatableProperties::GetPropHeight() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetHeightDimension();
}

void AnimatableProperties::SetPropBgColor(const Color& color)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetColor(color, true);
}

Color AnimatableProperties::GetPropBgColor() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetColor();
}

void AnimatableProperties::SetPropOpacity(float opacity)
{
    auto renderNode = AceType::DynamicCast<RenderDisplay>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->UpdateOpacity(UINT8_MAX * opacity);
}

float AnimatableProperties::GetPropOpacity() const
{
    auto renderNode = AceType::DynamicCast<RenderDisplay>(this);
    if (renderNode == nullptr) {
        return 1.0f;
    }
    return renderNode->GetOpacity() / (UINT8_MAX * 1.0f);
}

void AnimatableProperties::SetPropShadow(const Shadow& shadow)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetShadow(shadow);
}

Shadow AnimatableProperties::GetPropShadow() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetShadow();
}

void AnimatableProperties::SetPropBackgroundPosition(const BackgroundImagePosition& position)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBackgroundPosition(position);
}

BackgroundImagePosition AnimatableProperties::GetPropBackgroundImagePosition() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetBackgroundPosition();
}

void AnimatableProperties::SetPropBackgroundSize(const BackgroundImageSize& size)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBackgroundSize(size);
}

BackgroundImageSize AnimatableProperties::GetPropBackgroundImageSize() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetBackgroundSize();
}

void AnimatableProperties::SetPadding(const AnimatableDimension& value, const DimensionHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetPadding(value, helper);
}

AnimatableDimension AnimatableProperties::GetPadding(const DimensionHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetPadding(helper);
}

void AnimatableProperties::SetMargin(const AnimatableDimension& value, const DimensionHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetMargin(value, helper);
}

AnimatableDimension AnimatableProperties::GetMargin(const DimensionHelper& helper) const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetMargin(helper);
}

void AnimatableProperties::SetBorderWidth(float value, const BorderEdgeHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBorderWidth(value, helper);
}

float AnimatableProperties::GetBorderWidth(const BorderEdgeHelper& helper) const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return 0.0f;
    }
    return renderNode->GetBorderWidth(helper);
}

void AnimatableProperties::SetBorderColor(const Color& color, const BorderEdgeHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBorderColor(color, helper);
}

Color AnimatableProperties::GetBorderColor(const BorderEdgeHelper& helper) const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetBorderColor(helper);
}

void AnimatableProperties::SetBorderStyle(BorderStyle style, const BorderEdgeHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBorderStyle(style, helper);
}

BorderStyle AnimatableProperties::GetBorderStyle(const BorderEdgeHelper& helper) const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return BorderStyle::NONE;
    }
    return renderNode->GetBorderStyle(helper);
}

void AnimatableProperties::SetBorderRadius(float value, const BorderRadiusHelper& helper)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBorderRadius(value, helper);
}

float AnimatableProperties::GetBorderRadius(const BorderRadiusHelper& helper) const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return 0.0f;
    }
    return renderNode->GetBorderRadius(helper);
}

void AnimatableProperties::SetBlurRadius(const AnimatableDimension& radius)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBlurRadius(radius);
}

AnimatableDimension AnimatableProperties::GetBlurRadius() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return AnimatableDimension(0.0, DimensionUnit::PX);
    }
    return renderNode->GetBlurRadius();
}

void AnimatableProperties::SetBackdropRadius(const AnimatableDimension& radius)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBackdropRadius(radius);
}

AnimatableDimension AnimatableProperties::GetBackdropRadius() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return AnimatableDimension(0.0, DimensionUnit::PX);
    }
    return renderNode->GetBackdropRadius();
}

void AnimatableProperties::SetWindowBlurProgress(double progress)
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetWindowBlurProgress(progress);
}

double AnimatableProperties::GetWindowBlurProgress() const
{
    auto renderNode = AceType::DynamicCast<RenderBox>(this);
    if (renderNode == nullptr) {
        return 0.0f;
    }
    return renderNode->GetWindowBlurProgress();
}

void AnimatableProperties::SetPositionLeft(const Dimension& left)
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetLeft(left);
}

Dimension AnimatableProperties::GetPositionLeft() const
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetLeft();
}

void AnimatableProperties::SetPositionTop(const Dimension& top)
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetTop(top);
}

Dimension AnimatableProperties::GetPositionTop() const
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetTop();
}

void AnimatableProperties::SetPositionRight(const Dimension& right)
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetRight(right);
}

Dimension AnimatableProperties::GetPositionRight() const
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetRight();
}

void AnimatableProperties::SetPositionBottom(const Dimension& bottom)
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return;
    }
    renderNode->SetBottom(bottom);
}

Dimension AnimatableProperties::GetPositionBottom() const
{
    auto renderNode = AceType::DynamicCast<RenderNode>(this);
    if (renderNode == nullptr) {
        return {};
    }
    return renderNode->GetBottom();
}

} // namespace OHOS::Ace
