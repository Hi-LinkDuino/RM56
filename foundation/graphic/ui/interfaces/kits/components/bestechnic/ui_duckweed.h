/**
 * @file ui_duckweed.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-23
 * @brief Duckweed 消息
 * @since 1.0
 * @version 1.0
 */

#if (BES_FEATURE_DUCKWEED == 1)
#ifndef GRAPHIC_LITE_UI_DUCKWEED_H
#define GRAPHIC_LITE_UI_DUCKWEED_H

#include "gfx_utils/rect.h"
#include "gfx_utils/style.h"
#include "gfx_utils/graphic_buffer.h"
#include "common/screen.h"
#include "common/text.h"

namespace OHOS {

/** @brief 默认的淡入时间，单位为毫秒 */
const int16_t DEF_FADE_IN_MS = 300;
/** @brief 默认的淡出时间，单位为毫秒 */
const int16_t DEF_FADE_OUT_MS = 600;
/** @brief 文本内容在消息中与边缘的默认间隙大小，单位为 pixel */
const int16_t DEF_TEXT_PADDING = 10;

/**
 * @brief 屏幕位置
 */
enum DuckweedSide: uint8_t {
    /** @brief 顶部（水平居中） */
    TOP,
    /** @brief 中间 */
    CENTER,
    /** @brief 底部（水平居中） */
    BOTTOM
};

/**
 * @brief duckweed 可更新的数据 
 */
struct DuckweedParam {
    DuckweedParam() = delete;

    DuckweedParam(uint8_t opacity) : opacity(opacity) {}

     ~DuckweedParam() = default;

    /** @brief 不透明度 */
    uint8_t opacity;
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief duckweed 消息配置
 */
struct DuckweedOption {
    DuckweedOption()
        : duration(2000),
          side(DuckweedSide::CENTER),
          bgColor(Color::ColorTo32(Color::Black())),
          bgOpacity(204),
          textColor(Color::ColorTo32(Color::White())),
          textOpacity(OPA_OPAQUE),
          textSize(30),
          padding(DEF_TEXT_PADDING)
    {
        fadeInOut[0] = DEF_FADE_IN_MS;
        fadeInOut[1] = DEF_FADE_OUT_MS;
    }

    ~DuckweedOption() = default;

    /**
     * @brief 克隆对象
     * 
     * @param src 克隆源
     */
    void Clone(const DuckweedOption& src)
    {
        duration = src.duration;
        side = src.side;
        bgColor = src.bgColor;
        bgOpacity = src.bgOpacity;
        textColor = src.textColor;
        textOpacity = src.textOpacity;
        textSize = src.textSize;
        memcpy(fadeInOut, src.fadeInOut, sizeof(fadeInOut));
        padding = src.padding;
    }

    /** 显示时长，单位为毫秒 */
    unsigned int duration;
    /** @brief 位置 */
    DuckweedSide side;
    /** 背景 rgb 颜色 */
    unsigned int bgColor;
    /** 背景不透明度, 0~255 */
    unsigned char bgOpacity;
    /** @brief 文字 rgb 颜色 */
    unsigned int textColor;
    /** @brief 文字不透明度，0~255 */
    unsigned char textOpacity;
    /** @brief 文字大小，单位为 pixel */
    unsigned char textSize;
    /** @brief 淡入淡出时间设置，值为包含两个元素的数组，依次表示淡入和淡出时长，单位为毫秒 */
    uint16_t fadeInOut[2];
    /** @brief 内容边距 */
    uint16_t padding;
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief duckweed 消息实现
 */
class UIDuckweed final : public HeapBase {
public:
    UIDuckweed();

    virtual ~UIDuckweed();

    /**
     * @brief 获取 Duckweed 消息的显示区域
     * 
     * @return const Rect& 
     */
    const Rect& GetRect();

    const DuckweedOption& GetOption();

    /**
     * @brief 设置 Duckweed 消息的文本内容（富文本）
     * 
     * @param content 
     */
    void SetContent(const SpannableString* content);

    /**
     * @brief 设置 duckweed 消息的各种参数
     * 
     * @param option 
     */
    void SetOption(const DuckweedOption& option);

    /**
     * @brief 重新评估 duckweed。在每一次参数的变更后，应调用此函数，以应用各种参数的最新数据
     */
    virtual void Evaluate();

    /**
     * @brief 设置不透明度
     * 
     * @param opacity 不透明度，取值范围为 [0, 255]，数值越大透明度越低
     */
    virtual void SetOpacity(uint8_t opacity);

    /**
     * @brief 绘制
     * 
     * @param gfxDstBuffer 
     * @param invalidatedArea 
     */
    virtual void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);

protected:
    /** @brief 绘制文本内容的 UI 组件 */
    Text* uiText_ = nullptr;

    /** @brief 消息显示的区域 */
    Rect rect_;

    /** @brief 文本内容（富文本支持） */
    SpannableString* content_ = nullptr;

    /** @brief 消息样式参数 */
    DuckweedOption option_;

    /** @brief 当前的不透明度 */
    uint8_t opacity_;

    /** @brief 消息背景样式 */
    Style* backgroundStyle_ = nullptr;

    /** @brief 消息前景（文本内容）样式 */
    Style* foregroundStyle_ = nullptr;

private:
    /**  @brief 初始化文本 UI 组件 */
    void InitUIText();

    /**
     * @brief 根据消息内容匹配消息的高度
     * 
     * @param width 当前消息宽度
     * @param height 当前消息高度
     * @param textWidth 文本内容宽度
     * @param textHeight 文本内容高度
     */
    void MatchHeightForContent(const uint16_t& width, uint16_t& height, uint16_t textWidth, const uint16_t& textHeight);

    /**
     * @brief 计算显示区域
     * 
     * @param rect 最大可显示区域
     */
    void CalculatorRect(Rect& rect);

    void DrawBackground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect);
    void DrawForeground(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, uint8_t backgroundOpa, Rect& rect);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_DUCKWEED_H
#endif //BES_FEATURE_DUCKWEED
