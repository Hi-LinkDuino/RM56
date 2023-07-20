/**
 * @file bes_preset_transition_parser.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-05-12
 * @brief 预置的转场动画
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_ACELITE_BES_TRANSITION_PARSER_H
#define OHOS_ACELITE_BES_TRANSITION_PARSER_H

#include "non_copyable.h"
#include "screen.h"
#include "ace_log.h"
#include "scroll_layer.h"

namespace {
/** @brief 预置动画：卡牌 */
constexpr const char* VAL_PRESET_ANIM_DEALER = "dealer";

/** @brief 预置动画：从左到右的平移 */
constexpr const char* VAL_PRESET_ANIM_TRANSLATE_FORWARD = "translate-forward";
/** @brief 预置动画：从右到左的平移 */
constexpr const char* VAL_PRESET_ANIM_TRANSLATE_BACKWARD = "translate-backward";
/** @brief 预置动画：从上到下的平移 */
constexpr const char* VAL_PRESET_ANIM_TRANSLATE_FALL = "translate-fall";
/** @brief 预置动画：从下到上的无缝平移 */
constexpr const char* VAL_PRESET_ANIM_TRANSLATE_RISE = "translate-rise";

/** @brief 预置动画：淡入淡出 */
constexpr const char* VAL_PRESET_ANIM_FADE = "fade";

/** @brief 预置动画：中心缩放，放大 */
constexpr const char* VAL_PRESET_ANIM_ZOOM_IN = "zoom-in";
/** @brief 预置动画：中心缩放，缩小 */
constexpr const char* VAL_PRESET_ANIM_ZOOM_OUT = "zoom-out";

/** @brief 预置动画：挤入屏幕 */
constexpr const char* VAL_PRESET_ANIM_STUFF = "stuff";
/** @brief 预置动画：溢出屏幕 */
constexpr const char* VAL_PRESET_ANIM_OVERFLOW = "overflow";

/** @brief 预置动画：场景镜头（x 轴） */
constexpr const char* VAL_PRESET_ANIM_SCENE_X = "scene-x";
/** @brief 预置动画：场景镜头（y 轴） */
constexpr const char* VAL_PRESET_ANIM_SCENE_Y = "scene-y";

/** @brief 预置动画：CRT 亮屏效果 */
constexpr const char* VAL_PRESET_ANIM_CRT_ON = "crt-on";
/** @brief 预置动画：CRT 灭屏效果 */
constexpr const char* VAL_PRESET_ANIM_CRT_OFF = "crt-off";

/** @brief 预置动画：从左上角伸展 */
constexpr const char* VAL_PRESET_ANIM_EXPAND_LT = "expand-lt";
/** @brief 预置动画：从右上角伸展 */
constexpr const char* VAL_PRESET_ANIM_EXPAND_RT = "expand-rt";
/** @brief 预置动画：从左下角伸展 */
constexpr const char* VAL_PRESET_ANIM_EXPAND_LB = "expand-lb";
/** @brief 预置动画：从右下角伸展 */
constexpr const char* VAL_PRESET_ANIM_EXPAND_RB = "expand-rb";

/** @brief 预置动画：收敛到左上角 */
constexpr const char* VAL_PRESET_ANIM_SHRINK_LT = "shrink-lt";
/** @brief 预置动画：收敛到右上角 */
constexpr const char* VAL_PRESET_ANIM_SHRINK_RT = "shrink-rt";
/** @brief 预置动画：收敛到左下角 */
constexpr const char* VAL_PRESET_ANIM_SHRINK_LB = "shrink-lb";
/** @brief 预置动画：收敛到右下角 */
constexpr const char* VAL_PRESET_ANIM_SHRINK_RB = "shrink-rb";

/** @brief 预置动画：从左往右占据 */
constexpr const char* VAL_PRESET_ANIM_OCCUPIED_LR = "occupied-lr";
/** @brief 预置动画：从右往左占据 */
constexpr const char* VAL_PRESET_ANIM_OCCUPIED_RL = "occupied-rl";
/** @brief 预置动画：从上往下占据 */
constexpr const char* VAL_PRESET_ANIM_OCCUPIED_TB = "occupied-tb";
/** @brief 预置动画：从下往上占据 */
constexpr const char* VAL_PRESET_ANIM_OCCUPIED_BT = "occupied-bt";

/** @brief 预置动画：驱逐（从左到右） */
constexpr const char* VAL_PRESET_ANIM_EXPULSION_LR = "expulsion-lr";
/** @brief 预置动画：驱逐（从右往左） */
constexpr const char* VAL_PRESET_ANIM_EXPULSION_RL = "expulsion-rl";
}

namespace OHOS {
namespace ACELite {
class BesTransitionParser final : public MemoryHeap {

public:
    ACE_DISALLOW_COPY_AND_MOVE(BesTransitionParser);
    ~BesTransitionParser() {}

    /**
     * @brief 解析预置转场动画
     * 
     * @param isEnterAnim 目标是否为入场动画
     * @param pageTransParam 页面转场参数集
     * @return 
     */
    static PageTransitionParam* ParsePresetTransitionAnim(const char* const presetAnim, bool isEnterAnim, PageTransitionParam* pageTransParam)
    {
        if (presetAnim == nullptr || pageTransParam == nullptr) {
            if (pageTransParam != nullptr) {
                delete pageTransParam;
                pageTransParam = nullptr;
            }
            return nullptr;
        }

        BesAnimFade fade = isEnterAnim ? BesAnimFade::FADE_IN : BesAnimFade::FADE_OUT;
        Screen &screen = Screen::GetInstance();

        //scene
        if (!strcmp(presetAnim, VAL_PRESET_ANIM_SCENE_X)) {
            return GenerateScene(true, isEnterAnim, pageTransParam, fade);
        } 
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_SCENE_Y)) {
            return GenerateScene(false, isEnterAnim, pageTransParam, fade);
        }

        //dealer
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_DEALER)) {
            return GenerateDealer(isEnterAnim, pageTransParam);
        }

        //translate
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_TRANSLATE_FORWARD)) {
            return GenerateTranslation(pageTransParam, PRESET_ANIM_TRANSLATE_X,
                        isEnterAnim ? -screen.GetWidth() : 0, isEnterAnim ? 0 : screen.GetWidth(), fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_TRANSLATE_BACKWARD)) {
            return GenerateTranslation(pageTransParam, PRESET_ANIM_TRANSLATE_X,
                        isEnterAnim ? screen.GetWidth() : 0, isEnterAnim ? 0 : -screen.GetWidth(), fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_TRANSLATE_RISE)) {
            return GenerateTranslation(pageTransParam, PRESET_ANIM_TRANSLATE_Y,
                        isEnterAnim ? -screen.GetHeight() : 0, isEnterAnim ? 0 : screen.GetHeight(), fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_TRANSLATE_FALL)) {
            return GenerateTranslation(pageTransParam, PRESET_ANIM_TRANSLATE_Y,
                        isEnterAnim ? screen.GetHeight() : 0, isEnterAnim ? 0 : -screen.GetHeight(), fade);
        }

        //fade
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_FADE)) {
            return GenerateFade(isEnterAnim, pageTransParam);
        }

        //zoom
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_ZOOM_IN)) {
            return GenerateZoomIn(isEnterAnim, pageTransParam, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_ZOOM_OUT)) {
            return GenerateZoomOut(isEnterAnim, pageTransParam, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_STUFF)) {
            return GenerateOverflowIn(isEnterAnim, pageTransParam, fade);
        } else if (!strcmp(presetAnim, VAL_PRESET_ANIM_OVERFLOW)) {
            return GenerateOverflowOut(isEnterAnim, pageTransParam, fade);
        }

        //expand
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPAND_LT)) {
            return GenerateExpand(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_TOP, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPAND_RT)) {
            return GenerateExpand(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_TOP, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPAND_LB)) {
            return GenerateExpand(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_BOTTOM,fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPAND_RB)) {
            return GenerateExpand(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_BOTTOM, fade);
        }

        //shrink
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_SHRINK_LT)) {
            return GenerateShrink(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_TOP, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_SHRINK_RT)) {
            return GenerateShrink(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_TOP, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_SHRINK_LB)) {
            return GenerateShrink(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_BOTTOM, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_SHRINK_RB)) {
            return GenerateShrink(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_BOTTOM, fade);
        }

        //occupy
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_OCCUPIED_LR)) {
            return GenerateOccupied(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_CENTER, BesAnimPivot::RIGHT_CENTER);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_OCCUPIED_RL)) {
            return GenerateOccupied(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_CENTER, BesAnimPivot::LEFT_CENTER);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_OCCUPIED_TB)) {
            return GenerateOccupied(isEnterAnim, pageTransParam, BesAnimPivot::TOP_CENTER, BesAnimPivot::BOTTOM_CENTER);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_OCCUPIED_BT)) {
            return GenerateOccupied(isEnterAnim, pageTransParam, BesAnimPivot::BOTTOM_CENTER, BesAnimPivot::TOP_CENTER);
        }

        //expulsion
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPULSION_LR)) {
            return GenerateExpulsion(isEnterAnim, pageTransParam, BesAnimPivot::LEFT_CENTER, BesAnimPivot::RIGHT_CENTER);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_EXPULSION_RL)) {
            return GenerateExpulsion(isEnterAnim, pageTransParam, BesAnimPivot::RIGHT_CENTER, BesAnimPivot::LEFT_CENTER);
        }

        //CRT style
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_CRT_ON)) {
            return GenerateCrtOn(isEnterAnim, pageTransParam, fade);
        }
        else if (!strcmp(presetAnim, VAL_PRESET_ANIM_CRT_OFF)) {
            return GenerateCrtOff(isEnterAnim, pageTransParam, fade);
        }

        else {
            delete pageTransParam;
            pageTransParam = nullptr;
            return nullptr;
        }
    }

private:
    /**
     * @brief 生成转场动画：场景切换
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateScene(bool isX, bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        param->Clone(PRESET_ANIM_SCALE);
        int16_t *values = param->animValues;
        if (isEnterAnim) {
            values[0] = 0;
            values[1] = 100;
            param->animDelay = 250;
            param->animDuration = 480;
            param->forceBackground = true;
        } else {
            values[0] = 100;
            values[1] = 0;
            param->animDuration = 230;
        }
        BesTransitionImpl::SetScaleXYEnable(values[0], isX, !isX);
        BesTransitionImpl::SetScaleXYEnable(values[1], isX, !isX);
        param->animPivot = BesAnimPivot::CENTER;
        param->animFade = fade;

        return param;
    }

    /**
     * @brief 生成转场动画：CRT 灭屏效果
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateCrtOff(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_OPACITY);
            values[0] = OPA_OPAQUE - 1;
            values[1] = OPA_OPAQUE;
            param->forceBackground = true;
        } else {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 100;
            values[1] = 1;
            param->animPivot = BesAnimPivot::CENTER;
            BesTransitionImpl::SetScaleXYEnable(values[0], false, true);
            BesTransitionImpl::SetScaleXYEnable(values[1], false, true);
            param->animFade = fade;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：CRT 亮屏效果
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateCrtOn(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_SCALE);
            int16_t *values = param->animValues;
            values[0] = 1;
            values[1] = 100;
            param->animPivot = BesAnimPivot::CENTER;
            BesTransitionImpl::SetScaleXYEnable(values[0], false, true);
            BesTransitionImpl::SetScaleXYEnable(values[1], false, true);
            param->animFade = fade;
        } else {
            delete param;
            param = nullptr;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：溢出屏幕
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateOverflowOut(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_OPACITY);
            values[0] = OPA_OPAQUE - 1;
            values[1] = OPA_OPAQUE;
            param->forceBackground = true;
        } else {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 100;
            values[1] = 300;
            param->animFade = fade;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：适应屏幕
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateOverflowIn(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_SCALE);
            int16_t *values = param->animValues;
            values[0] = 200;
            values[1] = 100;
            param->animFade = fade;
        } else {
            delete param;
            param = nullptr;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：中心缩小
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateZoomOut(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_OPACITY);
            values[0] = OPA_OPAQUE - 1;
            values[1] = OPA_OPAQUE;
            param->forceBackground = true;
        } else {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 100;
            values[1] = 1;
            param->animPivot = BesAnimPivot::CENTER;
            param->animFade = fade;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：中心放大
     * @param isEnterAnim
     * @param param 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateZoomIn(bool isEnterAnim, PageTransitionParam* param, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_SCALE);
            int16_t *values = param->animValues;
            values[0] = 1;
            values[1] = 100;
            param->animPivot = BesAnimPivot::CENTER;
            param->animFade = fade;
        } else {
            delete param;
            param = nullptr;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：驱逐
     * @param isEnterAnim
     * @param param 
     * @param enterPivot
     * @param exitPivot 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateExpulsion(
        bool isEnterAnim, PageTransitionParam* param, BesAnimPivot enterPivot, BesAnimPivot exitPivot)
    {
        if (param == nullptr) return nullptr;

        param->Clone(PRESET_ANIM_SCALE);

        int16_t *values = param->animValues; 
        if (isEnterAnim) {
            values[0] = 15;
            values[1] = 100;
            param->animPivot = enterPivot;
        } else {
            values[0] = 100;
            values[1] = 15;
            param->animPivot = exitPivot;
        }
        BesTransitionImpl::SetScaleXYEnable(param->animValues[0], true, true);
        BesTransitionImpl::SetScaleXYEnable(param->animValues[1], true, true);

        return param;
    }

    /**
     * @brief 生成转场动画：占据
     * 
     * @param isEnterAnim 
     * @param param 
     * @param pivot 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateOccupied(
        bool isEnterAnim, PageTransitionParam* param, BesAnimPivot enterPivot, BesAnimPivot exitPivot)
    {
       if (param == nullptr) return nullptr;

        param->Clone(PRESET_ANIM_SCALE);

        int16_t *values = param->animValues; 
        if (isEnterAnim) {
            values[0] = 5;
            values[1] = 100;
            param->animPivot = enterPivot;
        } else {
            values[0] = 100;
            values[1] = 5;
            param->animPivot = exitPivot;
        }

        bool xEnable = false, yEnable = false;
        switch (enterPivot) {
            case BesAnimPivot::LEFT_CENTER:
            case BesAnimPivot::RIGHT_CENTER:
                xEnable = true;
                break;
            case BesAnimPivot::TOP_CENTER:
            case BesAnimPivot::BOTTOM_CENTER:
                yEnable = true;
                break;
        }
        BesTransitionImpl::SetScaleXYEnable(param->animValues[0], xEnable, yEnable);
        BesTransitionImpl::SetScaleXYEnable(param->animValues[1], xEnable, yEnable);

        return param;
    }

    /**
     * @brief 生成转场动画：淡入淡出
     * @param param 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateFade(bool isEnterAnim, PageTransitionParam* param)
    {
        param->Clone(PRESET_ANIM_OPACITY);
        int16_t *values = param->animValues;
        values[0] = isEnterAnim ? (OPA_OPAQUE / 4) : OPA_OPAQUE;
        values[1] = isEnterAnim ? OPA_OPAQUE : OPA_TRANSPARENT;
        param->forceBackground = isEnterAnim;
        return param;
    }

    /**
     * @brief 生成转场动画：平移
     * @param param 
     * @param preset 
     * @param startValue 
     * @param endValue 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateTranslation(PageTransitionParam* param, PageTransitionParam& preset, 
                            int16_t startValue, int16_t endValue, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        param->Clone(preset);
        int16_t *values = param->animValues;
        values[0] = startValue;
        values[1] = endValue;
        param->animFade = fade;

        return param;
    }

    /**
     * @brief 生成转场动画：从角落扩张
     * @param isEnterAnim 
     * @param param 
     * @param pivot 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateExpand(
        bool isEnterAnim, PageTransitionParam* param, BesAnimPivot pivot, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 5;
            values[1] = 100;
            param->animPivot = pivot;
            param->animFade = fade;
        } else {
            delete param;
            param = nullptr;
        }
        return param;
    }

    /**
     * @brief 生成转场动画：缩回角落
     * @param isEnterAnim 
     * @param param 
     * @param pivot 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateShrink(
        bool isEnterAnim, PageTransitionParam* param, BesAnimPivot pivot, BesAnimFade fade = BesAnimFade::FADE_NONE)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_OPACITY);
            values[0] = OPA_OPAQUE / 4;
            values[1] = OPA_OPAQUE;
            param->forceBackground = true;
        } else {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 100;
            values[1] = 5;
            param->animPivot = pivot;
            param->animFade = fade;
        }
        return param;
    }

     /**
     * @brief 生成转场动画：卡牌
     * @param isEnterAnim 
     * @param param 
     * @param pivot 
     * @param fade 
     * @return PageTransitionParam* 
     */
    static PageTransitionParam* GenerateDealer(bool isEnterAnim, PageTransitionParam* param)
    {
        if (param == nullptr) return nullptr;

        int16_t *values = param->animValues;
        if (isEnterAnim) {
            param->Clone(PRESET_ANIM_TRANSLATE_X);
            values[0] = -Screen::GetInstance().GetWidth();
            values[1] = 0;
        } else {
            param->Clone(PRESET_ANIM_SCALE);
            values[0] = 100;
            values[1] = 85;
        }
        return param;
    }
};
}
}
#endif //OHOS_ACELITE_BES_TRANSITION_PARSER_H