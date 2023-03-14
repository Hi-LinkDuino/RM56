# 轻设备图形子系统更新说明

202106：
摘要：
1. 新增DFX维测能力：UIKit支持显示控件轮廓
2. 移除AmimanatorManager对外接口，封装到Animator类
3. 增多后端框架：支持对接不同芯片
4. 组件UIScrollView/UIList新增进度条功能
5. UISlider接口及功能变更
6. 点阵字体解耦：字库工具更新
7. 支持UICheckBox/UIRadioButton/UIToggleButton动效
8. UIButton支持点击动效
9. 支持A4、A8、LUT4、LUT8图片格式，ColorMode新增TSC6枚举类型
10. 支持多语言对齐，Style中lineHeight_替换lineSpace_,STYLE_LINE_HEIGHT替换STYLE_LINE_SPACE.

详情：
1. 新增DFX维测能力：UIKit支持显示控件轮廓
    代码路径: graphic_ui/interfaces/kits/dfx/ui_view_bounds.h；
    
    使用方式：调用 SetShowState 控制是否显示边框

2. 移除AmimanatorManager对外接口，封装到Animator类

   直接使用Animator Start和Stop接口，不需要手动去调用AmimanatorManager的Add/Remove添加Animator类。
   
3. 新增多后端框架：支持对接不同芯片

   抽象出一套绘制接口作为南向API，新芯片显示硬件加速能力需要实现这套接口。

   代码路径：/foundation/graphic/ui/interfaces/innerkits/engines/gfx/gfx_engine_manager.h

4. 组件UIScrollView/UIList新增进度条功能，接口变更如下：

    | 删除接口                                            |
    | --------------------------------------------------- |
    | void UIScrollView::SetScrollbarWidth(uint8_t width) |
    | void UIScrollView::RefreshScrollBar()               |

    | 新增接口：                                                |
    | --------------------------------------------------------- |
    | void UIAbstractScroll::SetXScrollBarVisible(bool visible) |
    | void UIAbstractScroll::SetYScrollBarVisible(bool visible) |

5. UISlider接口及功能修改如下：

    | 删除接口：                                                   |
    | ------------------------------------------------------------ |
    | void SetImage(const ImageInfo* backgroundImage, const ImageInfo* foregroundImage, const ImageInfo* knobImage) |
    | void SetImage(const char* backgroundImage, const char* foregroundImage, const char* knobImage); |
    | void SetSliderColor(const ColorType backgroundColor, const ColorType foregroundColor, const ColorType knobColor) |
    | void SetSliderRadius(int16_t backgroundRadius, int16_t foregroundRadius, int16_t knobRadius) |

    | 新增接口：                                                   |
    | ------------------------------------------------------------ |
    | void SetImage(**const** ImageInfo* backgroundImage, **const** ImageInfo* foregroundImage) |
    | void SetKnobImage(const ImageInfo* knobImage)                |
    | void SetImage(const char* backgroundImage, const char* foregroundImage) |
    | void SetKnobImage(const char* knobImage)                     |
    | void SetSliderColor(const ColorType backgroundColor, const ColorType foregroundColor) |
    | void SetKnobColor(const ColorType knobColor)                 |
    | void SetSliderRadius(int16_t backgroundRadius, int16_t foregroundRadius) |
    | void SetKnobRadius(int16_t knobRadius)                       |

    | 功能变更接口                     | 原功能                                                       | 现功能    |
    | -------------------------------- | ------------------------------------------------------------ | --------- |
    | int16_t UISlider::GetKnobWidth() | 如果未调用UISlider::SetKnobWidth(), GetKnobWidth()默认值为progressHeight_ / progressWidth_ | 默认值为0 |

6. 点阵字体解耦：字库工具更新

   （1）需要使用新的字库工具生成字体bin文件

   （2）在字体引擎初始化时需要调用BitmapFontInit接口来设置点阵字体相关资源。

7. 支持UICheckBox/UIRadioButton/UIToggleButton动效

   （1）支持UICheckBox软件绘制和动效，默认为图片绘制，可以调用SetImages接口取消默认图片。

   （2）支持UIRadioButton和UIToggleButton动效，默认软件绘制。

8. UIButton支持点击动效，默认开启，不需要开发者适配。

9. 支持A4、A8、LUT4、LUT8图片格式，ColorMode新增TSC枚举类型

    (1) ColorMode新增TSC6枚举 类型。

    (2) UIImageView成员变量colorFormat由4位修改为8位。

    (3) 合入时间2021.6.17

10. 支持多语言对齐，Style中lineHeight_替换lineSpace_，STYLE_LINE_HEIGHT替换STYLE_LINE_SPACE.

    (1) lineHeight_：为两行文字baseline之间的距离。

    (2) 使用方法：调用SetStyle(STYLE_LINE_HEIGHT, value)。

    (3) 合入时间2021.6.24