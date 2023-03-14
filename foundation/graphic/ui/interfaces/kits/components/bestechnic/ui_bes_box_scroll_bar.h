/**
 * @file ui_bes_box_scroll_bar.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-02
 * @brief 对 UIBoxScrollBar 增强的矩形滚动条，支持设置部分样式
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_BES_BOX_SCROLL_BAR_H
#define GRAPHIC_LITE_UI_BES_BOX_SCROLL_BAR_H

#include "components/ui_abstract_scroll_bar.h"

namespace OHOS {
class UIBesBoxScrollBar : public UIAbstractScrollBar {
public:

    UIBesBoxScrollBar() = delete;

    /**
     * @brief Construct a new UIBesBoxScrollBar object
     *
     * @param orientation 滚动条的方向，取值为 BOX_ORIENTATION_VER | BOX_ORIENTATION_HOR
     */
    explicit UIBesBoxScrollBar(bool isVertical);

    virtual ~UIBesBoxScrollBar() = default;

    /**
     * @brief 设置滚动条的相关参数
     *
     * @param x 滚动条起始 X 坐标
     * @param y 滚动条弧线起始 Y 坐标
     * @param width 滚动条宽度
     * @param height 滚动条高度
     */
    virtual void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height) override;

    virtual void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa) override;

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
    Rect backgroundRect_;

    virtual void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect);

    virtual void DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect);

private:
    /** @brief 滚动条是否为 Y 轴方向 */
    bool isVertical_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_BES_BOX_SCROLL_BAR_H
