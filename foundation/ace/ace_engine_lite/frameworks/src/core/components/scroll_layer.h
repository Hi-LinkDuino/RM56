/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_SCROLL_LAYER_H
#define OHOS_ACELITE_SCROLL_LAYER_H

#include "component.h"
#include "js_fwk_common.h"
#include "non_copyable.h"
#include "ui_scroll_view.h"

#if (BES_FRATURE_PAGE_TRANSITION == 1)
#include "bestechnic/bes_transition_impl.h"

/** @brief 动画默认持续时长，单位为毫秒 */
static constexpr uint16_t BES_ANIM_DURATION_DEF = 350;
/** @brief 动画名称：X 轴平移 */
static constexpr char* BES_ANIM_TRANSLATE_X = "translateX";
/** @brief 动画名称：Y 轴平移 */
static constexpr char* BES_ANIM_TRANSLATE_Y = "translateY";
/** @brief 动画名称：透明度。包含起始透明度和终止透明度这 2 个动画数值。数值范围为 [0, 255] */
static constexpr char* BES_ANIM_OPACITY = "opacity";
/** 
 * @brief 动画名称：缩放。包含 起始缩放百分值，终止缩放百分值，x 轴缩放开关值，y 轴缩放开关值 这 4 个动画数值。
 *             缩放百分值取值范围为 (0, 1000], 缩放开关值可设置 0 或 1，0 表示禁用，1 表示启用。
 *             X，Y 缩放开关被依次存储在 起始值的高 1 位和高 2 位
 */
static constexpr char* BES_ANIM_SCALE = "scale";
/** @brief 动画名称，旋转 */
static constexpr char* BES_ANIM_ROTATION = "rotation";
#endif

namespace OHOS {
namespace ACELite {

#if (BES_FRATURE_PAGE_TRANSITION == 1)
/**
 * @author yongxianglai@bestechnic.com
 * @brief 页面转场参数
 */
struct PageTransitionParam {

    PageTransitionParam() = default;

    PageTransitionParam(char* name, uint16_t duration, EasingType easingType, BesAnimFade fade = BesAnimFade::FADE_NONE)
        : animName(name),
            animDuration(duration),
            animInterpolation(easingType),
            animFade(fade) {}

    /** @brief 重置 */
    void Reset()
    {
        forceBackground = false;
        animName = nullptr;
        animDuration = BES_ANIM_DURATION_DEF;
        memset(animValues, 0, sizeof(animValues));
        animInterpolation = EasingType::LINEAR;
        animFade = BesAnimFade::FADE_NONE;
        animPivot = BesAnimPivot::CENTER;
        animDelay = 0;
    }

    /**
     * @brief 克隆目标中的所有数据
     * @param param  目标
     */
    void Clone(PageTransitionParam& param)
    {
        forceBackground = param.forceBackground;
        animName = param.animName;
        animDuration = param.animDuration;
        memcpy(animValues, param.animValues, sizeof(animValues));
        animInterpolation = param.animInterpolation;
        animFade = param.animFade;
        animPivot = param.animPivot;
        animDelay = param.animDelay;
    }

    /** 是否强制目标转场动作处于后方位置 */
    bool forceBackground = false;

    /**
     * @brief 动画名称，预置以下动画：
     * BES_ANIM_TRANSLATE_X：X 轴方向平移动画，数值类型为 int16_t
     * BES_ANIM_TRANSLATE_Y：Y 轴方向平移动画，数值类型为 int16_t
     * BES_ANIM_OPACITY：透明度变换动画，数值类型为 uint8_t
     * BES_ANIM_SCALE：缩放动画，数值类型为 int16_t
     */
    char* animName;

    /**
     * @brief 动画数值，目前仅支持设置 2 个动画数值，数值的意义由对应的动画（animName）字段决定。
     */
    int16_t animValues[2] = { 0, 0 };

    /** @brief 动画持续时长，单位为毫秒 */
    uint16_t animDuration = BES_ANIM_DURATION_DEF;

    /** @brief 动画插值器 */
    EasingType animInterpolation = EasingType::LINEAR;

    /** @brief 动画淡入淡出类型 */
    BesAnimFade animFade = BesAnimFade::FADE_NONE;

    /** @brief 动画基准点（中心） */
    BesAnimPivot animPivot = BesAnimPivot::CENTER;

    /** @brief 动画延迟时间，单位为毫秒 */
    uint16_t animDelay = 0;
};

static PageTransitionParam PRESET_ANIM_TRANSLATE_X(BES_ANIM_TRANSLATE_X, BES_ANIM_DURATION_DEF, EasingType::LINEAR);
static PageTransitionParam PRESET_ANIM_TRANSLATE_Y(BES_ANIM_TRANSLATE_Y, BES_ANIM_DURATION_DEF, EasingType::LINEAR);
static PageTransitionParam PRESET_ANIM_OPACITY(BES_ANIM_OPACITY, BES_ANIM_DURATION_DEF, EasingType::LINEAR);
static PageTransitionParam PRESET_ANIM_SCALE(BES_ANIM_SCALE, BES_ANIM_DURATION_DEF, EasingType::LINEAR);
#endif //BES_FRATURE_PAGE_TRANSITION

class ScrollLayer final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ScrollLayer);
    ScrollLayer();
    ~ScrollLayer();
    void AppendScrollLayer(Component *rootComponent);
    void Hide() const;
    void Show(bool backgroundInPageTrans = false) const;
    void DetachFromRootView() const;

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    /**
     * @brief 开始页面转场
     */
    void StartPageTransition(const PageTransitionParam* param, AnimatorCallback* callback);

    /**
     * @brief 立即停止页面转场
     */
    void StopPageTransition();

    /** 
     * @brief  获取顶级 view（UIScrollView）
     * @return 
     */
    UIView& GetLayerRootView()
    {
        return *scroll_;
    }
#endif

private:
    UIScrollView *AddScrollLayer(UIView &view) const;
    UIScrollView *scroll_ = nullptr;
    UIView *pageRootView_ = nullptr;

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    /**
     * @brief 解析外部的页面转场参数
     * @param params 外部指定的页面转场参数
     */
    bool ParsePageTransParam(const PageTransitionParam& param);

    /** @brief 页面转场参数 */
    TransitionParams* pageTransParams_ = nullptr;

    /** @brief 页面转场实现 */
    BesTransitionImpl* pageTransImpl_ = nullptr;
#endif
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_SCROLL_LAYER_H
