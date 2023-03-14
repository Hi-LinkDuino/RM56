/**
 * @file ui_bes_arc_scroll_bar.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-01-18
 * @brief 
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_BES_ARC_SCROLL_BAR_H
#define GRAPHIC_LITE_UI_BES_ARC_SCROLL_BAR_H

#include "components/ui_abstract_scroll_bar.h"

namespace {
constexpr uint16_t RIGHT_SIDE_START_ANGLE_IN_DEGREE = 0;
constexpr uint16_t RIGHT_SIDE_END_ANGLE_IN_DEGREE = 180;

constexpr uint16_t BOTTOM_SIDE_START_ANGLE_IN_DEGREE = 90;
constexpr uint16_t BOTTOM_SIDE_END_ANGLE_IN_DEGREE = 270;

constexpr uint16_t LEFT_SIDE_START_ANGLE_IN_DEGREE = 180;
constexpr uint16_t LEFT_SIDE_END_ANGLE_IN_DEGREE = 360;

constexpr uint16_t TOP_SIDE_START_ANGLE_IN_DEGREE = 270;
constexpr uint16_t TOP_SIDE_END_ANGLE_IN_DEGREE = 450;

constexpr uint16_t SCROLL_BAR_MIN_ARC = 10;
} // namespace

namespace OHOS {

/**
 * @brief 弧形滚动条的位置
 */
enum BesArcScrollBarSide: uint8_t {
    BES_ARC_SCROLL_BAR_RIGHT_SIDE = ScrollBarSide::SCROLL_BAR_RIGHT_SIDE,
    BES_ARC_SCROLL_BAR_LEFT_SIDE = ScrollBarSide::SCROLL_BAR_LEFT_SIDE,
    BES_ARC_SCROLL_BAR_TOP_SIDE,
    BES_ARC_SCROLL_BAR_BOTTOM_SIDE,
    BES_ARC_SCROLL_BAR_INVALID_SIDE = 255
};

/**
 * @brief 自定义的弧形滚动条
 */
class UIBesArcScrollBar : public UIAbstractScrollBar {
public:
    UIBesArcScrollBar();

    virtual ~UIBesArcScrollBar() = default;

    /**
     * @brief 设置滚动条的相关参数
     *
     * @param x 滚动条弧线所在圆的圆心 x 坐标
     * @param y 滚动条弧线所在圆的圆心 y 坐标
     * @param width 滚动条弧线的大小（竖向滚动条的大小表现为宽度）。注意，此数值不允许超过 滚动条所在圆的半径（radius_）
     * @param radius 滚动条弧线所在圆的半径
     */
    virtual void SetPosition(int16_t x, int16_t y, int16_t width, int16_t radius) override;

    virtual void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa) override;

    /**
     * @brief 设置滚动条的弧线起始/结束角度
     *
     * @param startAngle
     * @param endAngle
     */
    void SetScrollBarAngle(uint16_t startAngle, uint16_t endAngle);

    /**
     * @brief 设置滚动条的滑块样式
     *
     * @param key 字段名称，取值见 style.h 中的 STYLE_XXX 枚举值
     * @param value 字段值，数据类型由字段决定
     *
     * @see STYLE_LINE_COLOR
     * @see STYLE_LINE_OPA
     */
    void SetScrollBarForeground(uint8_t key, int64_t value);

    /**
     * @brief 设置滚动条的背景样式
     *
     * @param key 字段名称，取值见 style.h 中的 STYLE_XXX 枚举值
     * @param value 字段值，数据类型由字段决定
     *
     * @see STYLE_LINE_COLOR
     * @see STYLE_LINE_OPA
     */
    void SetScrollBarBackground(uint8_t key, int64_t value);

protected:
    virtual void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa);
    virtual void DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa);
    int16_t radius_;
    int16_t width_;
    uint16_t startAngle_;
    uint16_t endAngle_;
    Point center_;

private:
    /** 滚动条的位置，取值见 {@link ArcScrollBarSide} */
    uint8_t side_;
};

} // GRAPHIC_LITE_UI_BES_ARC_SCROLL_BAR_H

#endif

