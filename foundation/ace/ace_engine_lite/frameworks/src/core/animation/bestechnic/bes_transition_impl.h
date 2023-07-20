/**
 * @file bes_transition_impl.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-14
 * @brief 以 transition_impl.h 为基础的改造
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_ACELITE_BES_TRANSITION_IMPL_H
#define OHOS_ACELITE_BES_TRANSITION_IMPL_H
#include <ctime>

#include "animator.h"
#include "js_fwk_common.h"
#include "non_copyable.h"

#include "transition_impl.h"

namespace {
/** @brief 缩放比率 */
static constexpr int16_t SCALE_PERCENTAGE = 100;
}

namespace OHOS {
namespace ACELite {

/**
 * @author yongxianglai@bestechnic.com
 * @brief 动画基准位置，以此位置作为中心点。
 * 等效值（0 为 启用位，1 为禁止位）：
 * center:  1111
 * left:     1101
 * top:      1110
 * right:   0111
 * bottom:  1011
 */
enum BesAnimPivot: uint8_t {
    /** @brief 中心 */
    CENTER = 0xF,
    /** @brief 左上 */
    LEFT_TOP = 0xC,
    /** @brief 右上 */
    RIGHT_TOP = 0x6,
    /** @brief 左下 */
    LEFT_BOTTOM = 0x9,
    /** @brief 右下 */
    RIGHT_BOTTOM = 0x3,
    /** @brief 左中 */
    LEFT_CENTER = 0xD,
    /** @brief 右中 */
    RIGHT_CENTER = 0x7,
    /** @brief 顶部中点 */
    TOP_CENTER = 0xE,
    /** @brief 底部中点 */
    BOTTOM_CENTER = 0xB
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 动画淡入淡出类型枚举
 */
enum BesAnimFade: int8_t {
    /** @brief 淡出 */
    FADE_OUT = -1,
    /** @brief 无 */
    FADE_NONE,
    /** @brief 淡入 */
    FADE_IN
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 动画返回码定义
 */
enum BesAnimatorCode: int8_t {
    /** @brief 要求立即到达动画起始值 */
    REQ_START,
    /** @brief 要求动画立即到达终点值 */
    REQ_ENDED,
    /** @brief 要求立即中止动画 */
    REQ_STOP,
    /** @brief 未知返回码 */
    CODE_UNDEFINE
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 转场动画过程中抛出的异常
 */
class BesAnimationException : public MemoryHeap {
public:
    BesAnimationException(int8_t* code, char* message = nullptr): code_(code), message_(message) {};

    virtual ~BesAnimationException() {};

    /**
     * @brief 获取错误码
     * @return 动画错误码，0 为未知错误，其他错误码定义见 AnimatorErrCode
     */
    const int8_t GetCode() const {
        return *code_;
    };

    /**
     * @brief 获取异常描述消息
     * @return 异常描述
     */
    const char* GetMessage() const {
        return message_;
    }

private:
    /** @brief 返回码，取值见 AnimatorCode */
    int8_t* code_;

    /** @brief 描述信息 */
    char* message_;
};

/**
 * @brief: animation callback implement.
 *         supported parameters can refer to struct TransitionParams
 */
class BesTransitionImpl : public AnimatorCallback {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BesTransitionImpl);

    BesTransitionImpl(TransitionParams& params, UIView* view, BesAnimPivot pivot = BesAnimPivot::CENTER, BesAnimFade fadeType = BesAnimFade::FADE_NONE);

    BesTransitionImpl() = delete;

    virtual ~BesTransitionImpl();

    /**
     * @brief: must call this to do initialization after create TransitionImpl instance
     */
    void Init();

    /**
     * brief: start the animation
     */
    void Start();

    static int8_t GetNumIterations(const char* iterations);

    static bool IsEndWith(const char* src, const char *end);

    static void SetScaleXYEnable(int16_t& value, bool xEnable, bool yEnable);

    void Stop() const;

    /**
     * @brief 设置外部事件回调
     * 
     * @param callback 
     */
   void SetAnimatorCallback(AnimatorCallback* callback);

private:
    /**
     * @brief 动画每执行一帧，就调用一次此函数
     * 
     * @param view 
     */
    void Callback(UIView* view) override;

    /**
     * @brief 在动画结束时回调
     * 
     * @param view 
     */
    void OnStop(UIView& view) override;

    bool RepeatAnimator();
    void ResetRepeatParam();
    void RecordViewStatus();
    void RecoveryViewStatus(Rect invalidatedAreaBefore) const;
    void InitTransitionParams();
    void InitTransitionParamsStyle();
    void InitTransitionParamsTransform();
    void InitTransitionParamsEasing();
    void GetRGB(const uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b) const;
    int16_t GetNextFrameValue(int16_t from, int16_t to, int32_t elapsedTime) const;
    void SetTransformSrcPosition(int16_t from, int16_t to);
    void RotateAroundCenterPoint(int16_t angle);
    /**
     * @brief 缩放
     * @param valueX X 轴上的缩放比率百分值
     * @param valueY Y 轴上的缩放比率百分值
     */
    void Scale(int16_t valueX, int16_t valueY);
    void Perform(int32_t elapsedTime);
    void PerformTransitionBgColorLinear(int32_t elapsedTime);
    void PerformTransition(int16_t from,
                           int16_t to,
                           TransitionType transitionType,
                           int16_t& updateAttrValue,
                           int32_t elapsedTime);

    void SetAnimFade(int32_t elapsedTime);

    /**
     * @brief 获取缩放在 X / Y 轴的可用状态
     * 
     * @param value
     * @param xEnable
     * @param yEnable
     */
    void GetScaleXYEnable(int16_t value, bool& xEnable, bool& yEnable);

    /**
     * @brief 获取缩放百分值
     * 
     * @param value 
     * @return Vector2<float> 
     */
    int16_t GetScaleRatio(int16_t value);

    /**
     * @brief 解析动画基准值
     * 
     * @param pivot 基准类型
     * @param pivotX 待基准的 x 轴坐标
     * @param pivotY 待基准的 y 轴坐标
     */
    void ParseAnimPivot(BesAnimPivot pivot, float& pivotX, float& pivotY);

    UIView* view_;
    TransitionParams& params_;
    int8_t oriIteration_;
    Animator* animator_;
    Vector2<float> pivot_;
    int16_t xSrc_;
    int16_t ySrc_;
    int16_t rotateSrc_;
    int16_t widthSrc_;
    int16_t heightSrc_;
    int16_t opacitySrc_;
    int16_t scaleSrc_;
    ViewStatus viewStatus_;
    bool isTransformSrcSet_ = false;
    bool timeArrivaled_ = false;
    bool easingType_[EasingType::EEND] = {0};
    bool isTransitionSet_[GeneralType::END] = {0};
    OptionsFill fill_ = OptionsFill::FNONE;
    TransformType transformType_ = TransformType::NONE;

    /* used for background-color */
    uint8_t rSrc_ = 0; // used to record the last time red value
    uint8_t gSrc_ = 0;
    uint8_t bSrc_ = 0;
    uint8_t rTo_ = 0;
    uint8_t gTo_ = 0;
    uint8_t bTo_ = 0;
    int8_t count_ = 1;             // used to record the current number of times of updating the bg-color
    int8_t steps_ = 1;             // total target number of times of updating the bg-color (during_/INTERVAL)
    int32_t bgcolorTimeSrc_ = 0;       // used to record the last time bg-color
    const static int8_t ITERATIONS_INFINITY = -1;
    const static int16_t INTERVAL = 150; // update the bg-color every INTERVAL (ms)

    /**
     * @brief 动画的事件回调，以允许外部跟踪事件
     * 其中的 Callback(UIView*) 函数支持抛出异常数据类型为 BesAnimatorException 的异常，以在转场动画过程中做一些处理，比如立即结束动画，中止动画等。
     * 示例：
     *     void AnimatorCallback::Callback(UIView* view)
     *     {
     *         //...
     *         if (...) {
     *             //这个异常对象将由内部销毁，不需要调用者处理
     *             throw new BesAnimatorException(BesAnimatorCode::REQ_TERNIMATION);
     *         }
     *         //...
     *     }
     */
    AnimatorCallback* extCallback_ = nullptr;

    /** @brief 动画淡入淡出类型 */
    BesAnimFade fadeType_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_BES_TRANSITION_IMPL_H
