/**
 * @file duckweed_module.cpp
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-22
 * @brief 
 * @since 1.0
 * @version 1.0
 */

#include "bestechnic/duckweed_module.h"

#if (BES_FEATURE_DUCKWEED == 1)
#include <cstdint>
#include <string.h>
#include "js_fwk_common.h"
#include "components/root_view.h"
#include "gfx_utils/graphic_math.h"

namespace {
/** @brief 文本字体大小的最小值 */
const int16_t TEXT_SIZE_MIN = 12;
/** @brief 显示时长的最小值 */
const int16_t DURATION_MIN_MS = 100;
/** @brief 显示时长的最大值 */
const int16_t DURATION_MAX_MS = 30000;
/** @brief 淡入淡出时间最小值，单位为毫秒 */
const int16_t FADE_IN_OUT_MIN_MS = 100;
/** @brief 淡入淡出时间最大值，单位为毫秒 */
const int16_t FADE_IN_OUT_MAX_MS = 5000;

/** @brief FIELD_SIDE 字段值，顶部位置（水平方向剧中） */
const char * const VAL_TOP = "top";
/** @brief FIELD_SIDE 字段值，中间位置 */
const char * const VAL_CENTER = "center";
/** @brief FIELD_SIDE 字段值，底部位置（水平居中） */
const char * const VAL_BOTTOM = "bottom";

/** @brief 文本样式值，普通样式 */
const char * const VAL_NORMAL = "normal";
/** @brief 文本样式值，加粗 */
const char * const VAL_BOLD = "bold";
/** @brief 文本样式值，斜体 */
const char * const VAL_ITALIC = "italic";
/** @brief 文本样式值，加粗 & 斜体 */
const char * const VAL_BOLD_ITALIC = "bold_italic";

/** @brief 消息显示时长字段，值为整数，单位为毫秒 */
const char * const FIELD_DURATION = "duration";
/** @brief 消息显示位置字段，可取值为 VAL_TOP | VAL_CENTER | VAL_BOTTOM */
const char * const FIELD_SIDE = "side";
/** @brief 消息背景颜色字段，值类型为十六进制表示的 ARGB 颜色字符串 */
const char * const FIELD_BACKGROUND = "background";
/** @brief 消息文字字体大小字段，值类型为整数，单位为 pixel */
const char * const FIELD_TEXT_SIZE = "textSize";
/** @brief 消息文字颜色字段，值类型为十六进制表示的 ARGB 颜色字符串 */
const char * const FIELD_TEXT_COLOR = "textColor";
/** @brief 消息淡入淡出时长字段，值为包含两个数值元素的数组类型，元素数值单位为 pixel */
const char * const FIELD_FADE_IN_OUT_TIMES = "fadeInOutTimes";
/** @brief 消息内容的边距大小字段，值为非负整数，对应数值单位为 pixel */
const char * const FIELD_PADDING = "padding";
/** @brief 富文本样式字段，值为一个二维数组，每个数组元素包含 3 个值，分别为 样式（string）、起始位置（number），结束位置（number） */
const char * const FIELD_TEXT_STYLE = "textStyle";
} //namespace

namespace OHOS {
namespace ACELite {

//OHOS::ACELite::DuckweedModule public

jerry_value_t DuckweedModule::Show(const jerry_value_t func,
                const jerry_value_t context,
                const jerry_value_t *args,
                const jerry_length_t argsNum)
{
    if (argsNum < 1) return UNDEFINED;

    DuckweedModule* duckweedModule = GetInstance();

    //如果第一个参数不是字符串，则视为无效的 duckweed 消息
    if (!jerry_value_is_string(args[0])) return UNDEFINED;

    uint16_t len = 0;
    char* str = MallocStringOf(args[0], &len);
    SpannableString content(str);

    DuckweedOption option;
    if (argsNum > 1) {
        ParseOption(args[1], option, &content);
    }

    uint32_t ret = UNDEFINED;
    uint32_t id = RootView::GetInstance()->AddDuckweed(&content, option);
    if (id != UNDEFINED) {
        ret = jerry_create_number(id);
    }

    ACE_FREE(str);

    return ret;
}

jerry_value_t DuckweedModule::Hide(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t *args,
                                const jerry_length_t argsNum)
{
    uint32_t id = UNDEFINED;
    if (argsNum > 0) {
        //消息 id
        id = IntegerOf(args[0]);
    }

    if (id == UNDEFINED) {
        RootView::GetInstance()->ClearDuckweed();
    } else {
        RootView::GetInstance()->RemoveDuckweed(id);
    }

    return UNDEFINED;
}

void DuckweedModule::Init()
{
    const char * const FUNC_SHOW_DUCKWEED = "showDuckweed";
    const char * const FUNC_HIDE_DOCKWEED = "hideDuckweed";

    CreateNamedFunction(FUNC_SHOW_DUCKWEED, Show);
    CreateNamedFunction(FUNC_HIDE_DOCKWEED, Hide);
}

//OHOS::ACELite::DuckweedModule private

void DuckweedModule::ParseOption(jerry_value_t obj, DuckweedOption &option, SpannableString *content)
{
    uint16_t len = 0;
    uint8_t alpha = 0;
    uint32_t color = 0;

    //显示时长
    int16_t duration = JerryGetIntegerProperty(obj, FIELD_DURATION);
    if (duration > 0) {
        duration = MATH_MAX(DURATION_MIN_MS, duration);
        option.duration = MATH_MIN(DURATION_MAX_MS, duration);
    }

    //位置
    char *side = JerryMallocStringProperty(obj, FIELD_SIDE, len);
    if (side != nullptr) {
        if (!strcmp(side, VAL_TOP)) {
            option.side = DuckweedSide::TOP;
        } else if (!strcmp(side, VAL_CENTER)) {
            option.side = DuckweedSide::CENTER;
        } else if (!strcmp(side, VAL_BOTTOM)) {
            option.side = DuckweedSide::BOTTOM;
        }
        ACE_FREE(side);
    }

    //背景色
    char *bgColor = JerryMallocStringProperty(obj, FIELD_BACKGROUND, len);
    if (bgColor != nullptr) {
        if (IsHexColor(bgColor) && ParseHexColor(bgColor, color, alpha)) {
            option.bgColor = color | 0xFF000000; //alpha 位必须置为 0xFF
            option.bgOpacity = alpha;
        }
        ACE_FREE(bgColor);
    }

    //字体颜色
    char *textColor = JerryMallocStringProperty(obj, FIELD_TEXT_COLOR, len);
    if (textColor != nullptr) {
        if (IsHexColor(textColor) && ParseHexColor(textColor, color, alpha)) {
            option.textColor = color | 0xFF000000; //alpha 位必须置为 0xFF
            option.textOpacity = alpha;
        }
        ACE_FREE(textColor);
    }

    //字体大小
    int16_t textSize = JerryGetIntegerProperty(obj, FIELD_TEXT_SIZE);
    if (textSize > 0) {
        option.textSize = MATH_MAX(TEXT_SIZE_MIN, textSize);
    }

    //淡入淡出时间
    jerry_value_t fadeInOutField = jerryx_get_property_str(obj, FIELD_FADE_IN_OUT_TIMES);
    if (jerry_value_is_array(fadeInOutField)) {
        uint16_t valueCount = jerry_get_array_length(fadeInOutField);
        if (valueCount > 0) {
            jerry_value_t fadeIn = jerry_get_property_by_index(fadeInOutField, 0);
            int16_t fadeInTime = MATH_MAX(FADE_IN_OUT_MIN_MS, IntegerOf(fadeIn));
            option.fadeInOut[0] = MATH_MIN(FADE_IN_OUT_MAX_MS, fadeInTime);
            jerry_release_value(fadeIn);
        }
        if (valueCount > 1) {
            jerry_value_t fadeOut = jerry_get_property_by_index(fadeInOutField, 1);
            int16_t fadeOutTime = MATH_MAX(FADE_IN_OUT_MIN_MS, IntegerOf(fadeOut));
            option.fadeInOut[1] = MATH_MIN(FADE_IN_OUT_MAX_MS, fadeOutTime);
            jerry_release_value(fadeOut);
        }
    }
    jerry_release_value(fadeInOutField);

    //内容边距
    int16_t padding = JerryGetIntegerProperty(obj, FIELD_PADDING);
    if (padding > 0) {
        option.padding = padding;
    }

    //富文本配置
    jerry_value_t textStyleField = jerryx_get_property_str(obj, FIELD_TEXT_STYLE);

    if (jerry_value_is_array(textStyleField)) {
        uint16_t len = 0;
        uint16_t styleSize = jerry_get_array_length(textStyleField);
        for (uint16_t idx = 0; idx < styleSize; ++idx) {
            jerry_value_t styleField = jerry_get_property_by_index(textStyleField, idx);
            if (!jerry_value_is_array(styleField)) continue;
            jerry_value_t nameField = UNDEFINED;
            if (jerry_get_array_length(styleField) >= 3) {
                nameField = jerry_get_property_by_index(styleField, 0);
            }
            jerry_release_value(styleField);

            if (IS_UNDEFINED(nameField) || !jerry_value_is_string(nameField)) continue;

            char *name = MallocStringOf(nameField, &len);
            jerry_release_value(nameField);
            if (name == nullptr) continue;
            TextStyle style;
            if (!strcmp(name, VAL_BOLD)) {
                style = TEXT_STYLE_BOLD;
            } else if (!strcmp(name, VAL_ITALIC)) {
                style = TEXT_STYLE_ITALIC;
            } else if (!strcmp(name, VAL_BOLD_ITALIC)) {
                style = TEXT_STYLE_BOLD_ITALIC;
            } else {
                style = TEXT_STYLE_NORMAL;
            }
            ACE_FREE(name);

            jerry_value_t startIndexField = jerry_get_property_by_index(styleField, 1);
            jerry_value_t endIndexField = jerry_get_property_by_index(styleField, 2);
            uint16_t startIndex = MATH_MAX(IntegerOf(startIndexField), 0);
            uint16_t endIndex = MATH_MAX(IntegerOf(endIndexField), 0);
            //不得超过文本长度
            uint16_t textLen = static_cast<uint16_t>(strlen(content->text_) + 1);
            endIndex = MATH_MIN(endIndex, textLen);
            ReleaseJerryValue(startIndexField, endIndexField, VA_ARG_END_FLAG);
            if (startIndex >= endIndex) continue;

            content->SetTextStyle(style, startIndex, endIndex);
        }
    }
}

} // namespace ACELite
} // namespace OHOS
#endif //BES_FEATURE_DUCKWEED