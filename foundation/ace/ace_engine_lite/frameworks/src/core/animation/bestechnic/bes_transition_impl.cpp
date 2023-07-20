/**
 * @file bes_transition_impl.cpp
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-14
 * @brief 以 transition_impl.cpp 为基础的改造，为页面转场等功能提供支持。支持向外部回调各种动画事件
 * @since 1.0
 * @version 1.0
 */

#include "bestechnic/bes_transition_impl.h"
#include <stdlib.h>
#include "ace_log.h"
#include "ace_mem_base.h"
#include "easing_equation.h"
#include "root_view.h"
#include "securec.h"

#include "screen.h"

namespace OHOS {
namespace ACELite {

//OHOS::ACELite::BesTransitionImpl public

BesTransitionImpl::BesTransitionImpl(TransitionParams& params, UIView* view, BesAnimPivot pivot, BesAnimFade fadeType)
    : view_(view),
          params_(params),
          oriIteration_(1),
          animator_(nullptr),
          xSrc_(0), ySrc_(0),
          rotateSrc_(0),
          widthSrc_(0), heightSrc_(0),
          opacitySrc_(0), bgcolorTimeSrc_(0),
          fadeType_(fadeType)
{
    ParseAnimPivot(pivot, pivot_.x_, pivot_.y_);
}

BesTransitionImpl::~BesTransitionImpl()
{
    Stop();

    if (animator_ != nullptr) {
        delete (animator_);
        animator_ = nullptr;
    }
}

void BesTransitionImpl::Init()
{
    if (animator_ != nullptr) {
        return;
    }
    animator_ = new Animator(this, view_, 0, true);
    if (animator_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animator create failed");
        return;
    }
}

void BesTransitionImpl::Start()
{
    if (animator_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "animator not initial or has been started");
        return;
    }
    uint8_t state = animator_->GetState();
    if (((state == Animator::STOP) || (state == Animator::PAUSE)) && (params_.during > 0)) {
        RecordViewStatus();
        InitTransitionParams();
        animator_->Start();
    }
}

int8_t BesTransitionImpl::GetNumIterations(const char *iterations)
{
    int8_t min = 1;
    int8_t max = 127;
    if (iterations == nullptr) {
        return min;
    }
    if (!strcmp(iterations, "infinite")) {
        return BesTransitionImpl::ITERATIONS_INFINITY;
    }
    long value = strtol(iterations, nullptr, DEC);
    if ((value < min) || (value > max)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "animation iterations should set between 1 and 127");
        return min;
    }
    return (int8_t)value;
}

bool BesTransitionImpl::IsEndWith(const char *src, const char *end)
{
    if ((src == nullptr) || (end == nullptr)) {
        return false;
    }

    size_t strLen = strlen(src);
    size_t endLen = strlen(end);
    if ((strLen < endLen) || (strLen == 0) || (endLen == 0)) {
        return false;
    }

    while (endLen > 0) {
        if (src[strLen - 1] != end[endLen - 1]) {
            return false;
        }
        endLen--;
        strLen--;
    }
    return true;
}

void BesTransitionImpl::Stop() const
{
    if (animator_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "animator not initial or is not running");
        return;
    }
    uint8_t state = animator_->GetState();
    if (state == Animator::START || state == Animator::RUNNING) {
        animator_->Stop();
    }
}

void BesTransitionImpl::SetAnimatorCallback(AnimatorCallback* callback)
{
    extCallback_ = callback;
}

//OHOS::ACELite::BesTransitionImpl private

void BesTransitionImpl::Callback(UIView *view)
{
    //外部优先级更高
    if (extCallback_ != nullptr) {
        try {
            extCallback_->Callback(view);
        }
        catch (BesAnimationException* e) {
            if (e == nullptr) return;

            int8_t code = e->GetCode();
            switch (code) {
                case BesAnimatorCode::REQ_START:
                    HILOG_ERROR(HILOG_MODULE_ACE, "Error! Restart animator is not supported.");
                    break;
                case BesAnimatorCode::REQ_ENDED:
                    Perform(params_.during);
                    return;
                case BesAnimatorCode::REQ_STOP:
                    timeArrivaled_ = true;
                    Stop();
                    return;
            }
            delete e;
        }
    }

    (void)(view);
    if (animator_ == nullptr) {
        return;
    }
    int32_t elapsedTime = animator_->GetRunTime() - params_.delay; // animation execution time
    if (elapsedTime < 0 && params_.delay > 0) {
        //即使在延时，也产生回调，因为 动画确实正在进行中
        extCallback_->Callback(view);
    }
    if (elapsedTime <= 0) {
        return;
    }
    if (elapsedTime >= params_.during) {
        timeArrivaled_ = true;
    }

    Perform(elapsedTime);
}

void BesTransitionImpl::OnStop(UIView& view)
{
    if (extCallback_ == nullptr) return;
    extCallback_->OnStop(view);
}

void BesTransitionImpl::InitTransitionParams()
{
    InitTransitionParamsStyle();
    InitTransitionParamsTransform();
    InitTransitionParamsEasing();
}

void BesTransitionImpl::InitTransitionParamsStyle()
{
    params_.delay = (params_.delay <= 0) ? 0 : params_.delay;
    params_.during = (params_.during <= 0) ? 0 : params_.during;
    oriIteration_ = params_.iterations;

    if (params_.fill == OptionsFill::FORWARDS) {
        fill_ = OptionsFill::FORWARDS;
    }

    if (params_.height_from >= 0 && params_.height_to >= 0) {
        isTransitionSet_[GeneralType::IS_HEIGHT_TRANSITION_SET] = true;
        heightSrc_ = params_.height_from;
    }
    if (params_.width_from >= 0 && params_.width_to >= 0) {
        isTransitionSet_[GeneralType::IS_WIDTH_TRANSITION_SET] = true;
        widthSrc_ = params_.width_from;
    }
    if (params_.opacity_from >= 0 && params_.opacity_to >= 0) {
        isTransitionSet_[GeneralType::IS_OPACITY_TRANSITION_SET] = true;
        opacitySrc_ = (params_.opacity_from <= OPA_OPAQUE) ? params_.opacity_from : OPA_OPAQUE;
        params_.opacity_to = (params_.opacity_to <= OPA_OPAQUE) ? params_.opacity_to : OPA_OPAQUE;
    }
    const uint32_t rgbValueMax = 0xFFFFFF;
    if ((params_.background_color_from <= rgbValueMax) && (params_.background_color_to <= rgbValueMax)) {
        isTransitionSet_[GeneralType::IS_BACKGROUND_COLOR_TRANSITION_SET] = true;
        GetRGB(params_.background_color_from, rSrc_, gSrc_, bSrc_);
        GetRGB(params_.background_color_to, rTo_, gTo_, bTo_);
        steps_ = params_.during / INTERVAL;
    }
}

void BesTransitionImpl::InitTransitionParamsTransform()
{
    if (params_.transformType == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "transformType not set");
        return;
    }
    if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_X)) {
        transformType_ = TransformType::TRANSLATE_X;
        xSrc_ = viewStatus_.x + params_.transform_from;
    } else if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_Y)) {
        transformType_ = TransformType::TRANSLATE_Y;
        ySrc_ = viewStatus_.y + params_.transform_from;
    } else if (!strcmp(params_.transformType, TRANSITION_ROTATE)) {
        transformType_ = TransformType::ROTATE;
        rotateSrc_ = params_.transform_from;
    } else if(!strcmp(params_.transformType,TRANSITION_SCALE)){
        transformType_ = TransformType::SCALE;
        scaleSrc_ = params_.transform_from;
    }
}

void BesTransitionImpl::InitTransitionParamsEasing()
{
    switch (params_.easing) {
        case EasingType::EASE_IN:
            easingType_[EasingType::EASE_IN] = true;
            break;
        case EasingType::EASE_OUT:
            easingType_[EasingType::EASE_OUT] = true;
            break;
        case EasingType::EASE_IN_OUT:
            easingType_[EasingType::EASE_IN_OUT] = true;
            break;
        default:
            easingType_[EasingType::LINEAR] = true;
            break;
    }
}

void BesTransitionImpl::GetRGB(const uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) const
{
    r = uint8_t((color & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
    g = uint8_t((color & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
    b = uint8_t((color & TEXT_BLUE_COLOR_MASK));
}

int16_t BesTransitionImpl::GetNextFrameValue(int16_t from, int16_t to, int32_t elapsedTime) const
{
    int16_t nextPoint;
    if (easingType_[EasingType::EASE_IN]) {
        nextPoint = EasingEquation::CubicEaseIn(from, to, elapsedTime, params_.during);
    } else if (easingType_[EasingType::EASE_OUT]) {
        nextPoint = EasingEquation::CubicEaseOut(from, to, elapsedTime, params_.during);
    } else if (easingType_[EasingType::EASE_IN_OUT]) {
        nextPoint = EasingEquation::CubicEaseInOut(from, to, elapsedTime, params_.during);
    } else {
        nextPoint = EasingEquation::LinearEaseNone(from, to, elapsedTime, params_.during);
    }

    return nextPoint;
}

void BesTransitionImpl::SetTransformSrcPosition(int16_t from, int16_t to)
{
    if (params_.transformType == nullptr) {
        return;
    }
    if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_X)) {
        view_->SetPosition(xSrc_, viewStatus_.y);
        view_->GetParent()->Invalidate();
    } else if (!strcmp(params_.transformType, TRANSITION_TRANSFORM_Y)) {
        view_->SetPosition(viewStatus_.x, ySrc_);
        view_->GetParent()->Invalidate();
    } else if (!strcmp(params_.transformType, TRANSITION_ROTATE)) {
        RotateAroundCenterPoint(rotateSrc_);
    }
    else {
        // do nothing
    }
}

void BesTransitionImpl::RotateAroundCenterPoint(int16_t angle){
    TransformMap transMap(view_->GetOrigRect());
    const int circleRate = 360;
    angle = angle % circleRate;
    transMap.Rotate((angle), pivot_);
    view_->SetTransformMap(transMap);
}

void BesTransitionImpl::Scale(int16_t valueX, int16_t valueY) {
    float scaleX = valueX < 1 ? 0.0f : (float)valueX / SCALE_PERCENTAGE;
    float scaleY = valueY < 1 ? 0.0f : (float)valueY / SCALE_PERCENTAGE;
    TransformMap transMap(view_->GetOrigRect());
    Vector2<float> pivotVector(pivot_.x_, pivot_.y_);
    Vector2<float> scaleVector(scaleX, scaleY);
    transMap.Scale(scaleVector, pivotVector);
    view_->SetTransformMap(transMap);
}

void BesTransitionImpl::Perform(int32_t elapsedTime)
{
    if (timeArrivaled_) {
        if (!RepeatAnimator()) {
            this->Stop();
        }
    }

    if (!isTransformSrcSet_) {
        SetTransformSrcPosition(params_.transform_from, params_.transform_to);
        isTransformSrcSet_ = true;
    }

    if (transformType_ == TransformType::TRANSLATE_X) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TTRANSLATE_X, xSrc_,
                          elapsedTime);
    } else if (transformType_ == TransformType::TRANSLATE_Y) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TTRANSLATE_Y, ySrc_,
                          elapsedTime);
    } else if (transformType_ == TransformType::ROTATE) {
        PerformTransition(params_.transform_from, params_.transform_to, TransitionType::TROTATE, rotateSrc_,
                          elapsedTime);
    }else if(transformType_ == TransformType::SCALE){
        PerformTransition(params_.scale_from, params_.scale_to, TransitionType::TSCALE, scaleSrc_,
                          elapsedTime);
    }

    if (isTransitionSet_[GeneralType::IS_HEIGHT_TRANSITION_SET]) {
        PerformTransition(params_.height_from, params_.height_to, TransitionType::HEIGHT, heightSrc_, elapsedTime);
    }
    if (isTransitionSet_[GeneralType::IS_WIDTH_TRANSITION_SET]) {
        PerformTransition(params_.width_from, params_.width_to, TransitionType::WIDTH, widthSrc_, elapsedTime);
    }

    if (isTransitionSet_[GeneralType::IS_OPACITY_TRANSITION_SET]) {
        PerformTransition(params_.opacity_from, params_.opacity_to, TransitionType::OPACITY, opacitySrc_, elapsedTime);
    }

    if (isTransitionSet_[GeneralType::IS_BACKGROUND_COLOR_TRANSITION_SET]) {
        PerformTransitionBgColorLinear(elapsedTime);
    }

    if ((fill_ == OptionsFill::FNONE) && timeArrivaled_) {
        RecoveryViewStatus(view_->GetRect());
    }

    if (timeArrivaled_) {
        ResetRepeatParam();
    }
}

void BesTransitionImpl::PerformTransitionBgColorLinear(int32_t elapsedTime)
{
    if (timeArrivaled_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "time arrived");
        view_->SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(rTo_, gTo_, bTo_).full);
        return;
    }
    if ((steps_ != 0) && (count_ <= steps_) && (elapsedTime <= params_.during)) {
        if ((elapsedTime - bgcolorTimeSrc_) > INTERVAL) {
            ColorType color = Color::GetColorFromRGB(rSrc_ + (rTo_ - rSrc_) * count_ / steps_,
                                                     gSrc_ + (gTo_ - gSrc_) * count_ / steps_,
                                                     bSrc_ + (bTo_ - bSrc_) * count_ / steps_);
            view_->SetStyle(STYLE_BACKGROUND_COLOR, color.full);
            view_->Invalidate();
            bgcolorTimeSrc_ = elapsedTime;
            count_++;
        }
    }
}

void BesTransitionImpl::PerformTransition(int16_t from,
                                       int16_t to,
                                       TransitionType transitionType,
                                       int16_t &updateAttrValue,
                                       int32_t elapsedTime)
{
    int16_t startVal = from;
    int16_t endVal = to;
    if (transitionType == TransitionType::TSCALE) {
        startVal = GetScaleRatio(from);
        endVal = GetScaleRatio(to);
    }

    if (timeArrivaled_) {
        updateAttrValue = endVal;
    } else {
        if (elapsedTime == 0) {
            updateAttrValue = startVal;
        } else {
            int16_t prefetchedValue = 0;
            prefetchedValue = GetNextFrameValue(startVal, endVal, elapsedTime);
            int16_t diffDistance = prefetchedValue - updateAttrValue;
            if (((diffDistance < 1) && (diffDistance > -1)) || (elapsedTime > params_.during)) {
                return;
            }
            updateAttrValue = static_cast<int16_t>(prefetchedValue);
        }
    }

    Rect invalidatedArea = view_->GetRect();
    switch (transitionType) {
        case TransitionType::TTRANSLATE_X: {
            SetAnimFade(elapsedTime);
            view_->SetPosition((updateAttrValue + viewStatus_.x), viewStatus_.y);
            break;
        }
        case TransitionType::TTRANSLATE_Y: {
            SetAnimFade(elapsedTime);
            view_->SetPosition(viewStatus_.x, (updateAttrValue + viewStatus_.y));
            break;
        }
        case TransitionType::TROTATE: {
            RotateAroundCenterPoint(updateAttrValue);
            break;
        }
        case TransitionType::TSCALE: {
            SetAnimFade(elapsedTime);
            bool xEnable = true, yEnable = true;
            GetScaleXYEnable(from, xEnable, yEnable);
            Scale(xEnable ? updateAttrValue : SCALE_PERCENTAGE, yEnable ? updateAttrValue : SCALE_PERCENTAGE);
            break;
        }
        case TransitionType::HEIGHT:
            view_->SetHeight(updateAttrValue);
            break;
        case TransitionType::WIDTH:
            view_->SetWidth(updateAttrValue);
            break;
        case TransitionType::OPACITY: {
            SetAnimFade(elapsedTime);
            //透明度的支持
            int16_t opa = MATH_MAX(updateAttrValue, OPA_TRANSPARENT);
            opa = MATH_MIN(updateAttrValue, OPA_OPAQUE);
            view_->SetOpaScale((uint8_t)opa);

            // double rate = (double)opa / OPA_OPAQUE;
            // view_->SetStyle(STYLE_BACKGROUND_OPA, static_cast<int64_t>(viewStatus_.rectOpacity * rate));
            // view_->SetStyle(STYLE_IMAGE_OPA, static_cast<int64_t>(viewStatus_.imageOpacity * rate));
            // view_->SetStyle(STYLE_LINE_OPA, static_cast<int64_t>(viewStatus_.lineOpacity * rate));
            // view_->SetStyle(STYLE_TEXT_OPA, static_cast<int64_t>(viewStatus_.textOpacity * rate));
            break;
        }
        default:
            HILOG_INFO(HILOG_MODULE_ACE, "animation nothing to do.");
            break;
    }

    invalidatedArea.Join(invalidatedArea, view_->GetRect());
    view_->InvalidateRect(invalidatedArea);
}

bool BesTransitionImpl::RepeatAnimator()
{
    if (oriIteration_ != BesTransitionImpl::ITERATIONS_INFINITY) {
        oriIteration_--;
        if (oriIteration_ == 0) {
            oriIteration_ = params_.iterations;
            return false;
        }
    }
    HILOG_DEBUG(HILOG_MODULE_ACE, "repeat");
    return true;
}

void BesTransitionImpl::ResetRepeatParam()
{
    if (animator_ != nullptr) {
        animator_->SetRunTime(0); // reset animation executing time
    }
    timeArrivaled_ = false; // reset timeArrivaled flag
    bgcolorTimeSrc_ = 0;    // reset bg-color last update time
    count_ = 1;             // reset bg-color update number of times
}

void BesTransitionImpl::RecordViewStatus()
{
    viewStatus_.x = view_->GetX();
    viewStatus_.y = view_->GetY();
    viewStatus_.oriRect = view_->GetOrigRect();
    viewStatus_.height = view_->GetHeight();
    viewStatus_.width = view_->GetWidth();
    viewStatus_.background_color.full = view_->GetStyle(STYLE_BACKGROUND_COLOR);
    viewStatus_.rectOpacity = view_->GetStyle(STYLE_BACKGROUND_OPA);
    viewStatus_.imageOpacity = view_->GetStyle(STYLE_IMAGE_OPA);
    viewStatus_.lineOpacity = view_->GetStyle(STYLE_LINE_OPA);
    viewStatus_.textOpacity = view_->GetStyle(STYLE_TEXT_OPA);
}

void BesTransitionImpl::RecoveryViewStatus(Rect invalidatedAreaBefore) const
{
    view_->SetX(viewStatus_.x);
    view_->SetY(viewStatus_.y);
    view_->SetHeight(viewStatus_.height);
    view_->SetWidth(viewStatus_.width);
    TransformMap &transMap = view_->GetTransformMap();
    Polygon polygon(Rect(0, 0, 0, 0));
    transMap.SetPolygon(polygon);
    view_->SetStyle(STYLE_BACKGROUND_COLOR, viewStatus_.background_color.full);
    view_->SetStyle(STYLE_BACKGROUND_OPA, viewStatus_.rectOpacity);
    view_->SetStyle(STYLE_IMAGE_OPA, viewStatus_.imageOpacity);
    view_->SetStyle(STYLE_LINE_OPA, viewStatus_.lineOpacity);
    view_->SetStyle(STYLE_TEXT_OPA, viewStatus_.textOpacity);
    invalidatedAreaBefore.Join(invalidatedAreaBefore, view_->GetRect());
    view_->InvalidateRect(invalidatedAreaBefore);
}

void BesTransitionImpl::SetAnimFade(int32_t elapsedTime)
{
    if (fadeType_ == BesAnimFade::FADE_NONE) return;

    uint8_t opa = OPA_OPAQUE;
    float fraction = MATH_MIN((float)elapsedTime / params_.during, 1.0f);
    if (fadeType_ == BesAnimFade::FADE_IN) {
        opa = MATH_MAX((uint8_t)(opa * fraction), OPA_OPAQUE * 5 / 10);
    } else if (fadeType_ == BesAnimFade::FADE_OUT) {
        opa = MATH_MIN((uint8_t)(opa * (1.0f - fraction)), OPA_OPAQUE * 6 / 10);
    }

    view_->SetOpaScale(MATH_MIN(opa, OPA_OPAQUE));
}

void BesTransitionImpl::SetScaleXYEnable(int16_t& value, bool xEnable, bool yEnable)
{
    xEnable ^= 1;
    yEnable ^= 1;
    if (xEnable) {
        value |= xEnable << 15;
    } else {
        value &= 0x7fff;
    }

    if (yEnable) {
        value |= yEnable << 14;
    } else {
        value &= 0xbfff;
    }
}

void BesTransitionImpl::GetScaleXYEnable(int16_t value, bool& xEnable, bool& yEnable)
{
    xEnable = ((value >> 15) & 0x1) == 0;
    yEnable = ((value >> 14) & 0x1) == 0;
}

int16_t BesTransitionImpl::GetScaleRatio(int16_t value)
{
    return value & 0xfff;
}

void BesTransitionImpl::ParseAnimPivot(BesAnimPivot pivot, float& pivotX, float& pivotY)
{
    uint16_t w = Screen::GetInstance().GetWidth() - 1;
    uint16_t h = Screen::GetInstance().GetHeight() - 1;
    float half = 0.5f;
    switch (pivot) {
        case BesAnimPivot::LEFT_TOP:
            pivotX = half;
            pivotY = half;
            break;
        case BesAnimPivot::RIGHT_TOP:
            pivotX = w;
            pivotY = 0;
            break;
        case BesAnimPivot::LEFT_BOTTOM:
            pivotX = 0;
            pivotY = h;
            break;
        case BesAnimPivot::RIGHT_BOTTOM:
            pivotX = w;
            pivotY = h;
            break;
        case BesAnimPivot::LEFT_CENTER:
            pivotX = 0;
            pivotY = h * half;
            break;
        case BesAnimPivot::RIGHT_CENTER:
            pivotX = w;
            pivotY = h * half;
            break;
        case BesAnimPivot::TOP_CENTER:
            pivotX = w * half;
            pivotY = 0;
            break;
        case BesAnimPivot::BOTTOM_CENTER:
            pivotX = w * half;
            pivotY = h;
            break;
        case BesAnimPivot::CENTER:
        default:
            pivotX = w * half;
            pivotY = h * half;
            break;
    }
}
} // namespace ACELite
} // namespace OHOS
