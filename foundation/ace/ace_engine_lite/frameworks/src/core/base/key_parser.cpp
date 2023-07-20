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

#include "key_parser.h"
#include <cstring>
#include "acelite_config.h"
#include "js_config.h"
#include "keys.h"
#ifdef OH_UI_PROGRESS_TOUCHABLE
#include "ace_log.h"
#endif
namespace OHOS {
namespace ACELite {
#ifdef KEYWORD
#undef KEYWORD
#endif // KEYWORD

// Replace the struct keys.h "KEYWORD" sector for init
#if (ENABLE_KEY == 1)
#define KEYWORD(enumkey, keystr) {#keystr, K_##enumkey, static_cast<uint8_t>(strlen(#enumkey))},
#else // ENABLE_KEY
#define KEYWORD(enumkey, keystr) {K_##enumkey, static_cast<uint8_t>(strlen(#enumkey))},
#endif // ENABLE_KEY

static const struct {
#if (ENABLE_KEY == 1)
    const char * const key;
#endif
    const uint16_t ID;
    const uint8_t LENGTH;
} G_KEYWORD_INFO[KEYWORDS_MAX] = {
#if (ENABLE_KEY == 1)
    {"UNKNOWN", K_UNKNOWN, static_cast<uint8_t>(strlen("UNKNOWN"))},
#else  // ENABLE_KEY
    {K_UNKNOWN, static_cast<uint8_t>(strlen("UNKNOWN"))},
#endif // ENABLE_KEY

#ifdef OHOS_ACELITE_KEYS_H
// include the keys.h again to redefine the "KEYWORD"
#undef OHOS_ACELITE_KEYS_H
#include "keys.h"
#endif // OHOS_ACELITE_KEYS_H
};
#undef KEYWORD

/**
 * @brief 解析自定义 key id。
 * 
 * @param s key
 * @param len key 长度
 * @return uint16_t 对应的在 keys.h 中定义的 keyid。当未匹配任何 keyid 时，返回 K_UNKNOWN。
 */
uint16_t ParsePrivateKeyId(const char *s, const size_t len)
{
    // 字段名称
    // 注意在 jsui 代码中如果是 a-bcd，则经 编译器 转换后对应的 key 是 aBcd
    if (!strcmp(s, "besScrollbarEnable")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_ENABLE;
    }
    if (!strcmp(s, "besScrollbarType")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_TYPE;
    }
    if (!strcmp(s, "besScrollbarSide")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_SIDE;
    }
    if (!strcmp(s, "besScrollbarSize")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_SIZE;
    }
    if (!strcmp(s, "besScrollbarBackground")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_BACKGROUND;
    }
    if (!strcmp(s, "besScrollbarForeground")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_FOREGROUND;
    }
    if (!strcmp(s, "besScrollbarAnim")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_ANIM;
    }
    if (!strcmp(s, "besScrollbarAutoHide")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_AUTO_HIDE;
    }
    if (!strcmp(s, "besScrollbarLenFactor")) { // scrollbar style 字段
        return K_BES_SCROLLBAR_LEN_FACTOR;
    }

    // 字段值
    if (!strcmp(s, "bes-list")) { // 字段值
        return K_BES_LIST;
    }

    if (!strcmp(s, "bes-left")) { // 字段值
        return K_BES_LEFT;
    }
    if (!strcmp(s, "bes-top")) { // 字段值
        return K_BES_TOP;
    }
    if (!strcmp(s, "bes-right")) { // 字段值
        return K_BES_RIGHT;
    }
    if (!strcmp(s, "bes-bottom")) { // 字段值
        return K_BES_BOTTOM;
    }

    if (!strcmp(s, "bes-arc")) { // 字段值
        return K_BES_ARC;
    }
    if (!strcmp(s, "bes-rectangle")) { // 字段值
        return K_BES_RECTANGLE;
    }
    if(!strcmp(s,"besType")){
        return K_BES_TYPE;
    }
    return K_UNKNOWN;
}

uint16_t KeyParser::ParseKeyId(const char *s, const size_t len)
{
    if (s == nullptr || len >= UINT16_MAX || len == 0) {
        return K_UNKNOWN;
    }
#ifdef OH_UI_PROGRESS_TOUCHABLE
    // HILOG_DEBUG(HILOG_MODULE_ACE, "ParseKeyId: %s", s);
#endif

    //yongxianglai@bestechnic.com: 优先匹配自定义 keyid
    const char* key = s;
    uint16_t keyId = ParsePrivateKeyId(key, len);
    if (keyId != K_UNKNOWN) return keyId;
    s = key;

    /*
     * The string was compared should be the same as the js define,
     * with xxx-yyy instead the xxx_yyy,
     * but the enum type use K_xxx_yyy for Id return
     */
    switch (*s++) {
            // clang-format off
        case 'a':
            if (!strcmp(s, "lignItems")) {
                return K_ALIGN_ITEMS;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "nalog-clock")) {
                return K_ANALOG_CLOCK;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            if (!strcmp(s, "nimationDelay")) {
                return K_ANIMATION_DELAY;
            }
            if (!strcmp(s, "nimationDuration")) {
                return K_ANIMATION_DURATION;
            }
            if (!strcmp(s, "nimationFillMode")) {
                return K_ANIMATION_FILL_MODE;
            }
            if (!strcmp(s, "nimationIterationCount")) {
                return K_ANIMATION_ITERATION_COUNT;
            }
            if (!strcmp(s, "nimationName")) {
                return K_ANIMATION_NAME;
            }
            if (!strcmp(s, "nimationTimingFunction")) {
                return K_ANIMATION_TIMING_FUNCTION;
            }
            if (!strcmp(s, "rc")) {
                return K_ARC;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "utoplay")) {
                return K_AUTOPLAY;
            }
#endif // FEATURE_COMPONENT_VIDEO
            break;
        case 'b':
            if (!strcmp(s, "ackgroundColor")) {
                return K_BACKGROUND_COLOR;
            }
            if (!strcmp(s, "ackgroundImage")) {
                return K_BACKGROUND_IMAGE;
            }
            if (!strcmp(s, "lockColor")) {
                return K_BLOCK_COLOR;
            }
            if (!strcmp(s, "orderBottomColor")) {
                return K_BORDER_BOTTOM_COLOR;
            }
            if (!strcmp(s, "orderLeftColor")) {
                return K_BORDER_LEFT_COLOR;
            }
            if (!strcmp(s, "orderRightColor")) {
                return K_BORDER_RIGHT_COLOR;
            }
            if (!strcmp(s, "orderTopColor")) {
                return K_BORDER_TOP_COLOR;
            }
            if (!strcmp(s, "orderColor")) {
                return K_BORDER_COLOR;
            }
            if (!strcmp(s, "orderRadius")) {
                return K_BORDER_RADIUS;
            }
            if (!strcmp(s, "orderBottomWidth")) {
                return K_BORDER_BOTTOM_WIDTH;
            }
            if (!strcmp(s, "orderLeftWidth")) {
                return K_BORDER_LEFT_WIDTH;
            }
            if (!strcmp(s, "orderRightWidth")) {
                return K_BORDER_RIGHT_WIDTH;
            }
            if (!strcmp(s, "orderTopWidth")) {
                return K_BORDER_TOP_WIDTH;
            }
            if (!strcmp(s, "orderWidth")) {
                return K_BORDER_WIDTH;
            }
            if (!strcmp(s, "reak")) {
                return K_BREAK;
            }
            if(!strcmp(s,"esLoop")){
                return K_BES_LOOP;
            }
            if(!strcmp(s,"esLayoutType")){
                return K_BES_LAYOUT_TYPE;
            }
            if(!strcmp(s,"esPosX")){
                return K_BES_POS_X;
            }
            if(!strcmp(s,"esPosY")){
                return K_BES_POS_Y;
            }
            if(!strcmp(s,"esHandWidth")){
                return K_BES_HAND_WIDTH;
            }
            if(!strcmp(s,"esHandHeight")){
                return K_BES_HAND_HEIGHT;
            }
            if (!strcmp(s, "esTransform")) {
                return K_BES_TRANSFORM;
            }
            if(!strcmp(s,"es-menu")){
                return K_BES_MENU;
            }
            if(!strcmp(s,"es-drag")){
                return K_BES_DRAG_VIEW;
            }
            if(!strcmp(s,"es-list-menu")){
                return K_BES_LIST_MENU_VIEW;
            }
            if(!strcmp(s,"es-slide")){
                return K_BES_SLIDE;
            }
            if(!strcmp(s,"es-drawer-layout")){
                return K_BES_DRAWER_LAYOUT;
            }
            if (!strcmp(s,"es-fisheye-menu")){
                return K_BES_FISHEYE_MENU;
            }
            if (!strcmp(s, "es")) {
                return K_BES;
            }
            if (!strcmp(s, "esPressColor")) {
                return K_BES_PRESS_COLOR;
            }
            if(!strcmp(s,"esPressImage")){
                return K_BES_PRESS_IMAGE;
            }
            if (!strcmp(s,"es-fragment"))
            {
                return K_BES_FRAGEMNT;
            }
            if (!strcmp(s,"esPath"))
            {
                return K_BES_PATH;
            }         
            break;
        case 'c':
#if (FEATURE_COMPONENT_CAMERA == 1)
            if (!strcmp(s, "amera")) {
                return K_CAMERA;
            }
#endif // FEATURE_COMPONENT_CAMERA
#if (FEATURE_COMPONENT_CANVAS == 1)
            if (!strcmp(s, "anvas")) {
                return K_CANVAS;
            }
#endif // FEATURE_COMPONENT_CANVAS
            if (!strcmp(s, "enter")) {
                return K_CENTER;
            }
            if (!strcmp(s, "enterX")) {
                return K_CENTER_X;
            }
            if (!strcmp(s, "enterY")) {
                return K_CENTER_Y;
            }
            if (!strcmp(s, "olor")) {
                return K_COLOR;
            }
            if (!strcmp(s, "olumn")) {
                return K_COLUMN;
            }
            if (!strcmp(s, "olumn-reverse")) {
                return K_COLUMN_REVERSE;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ontrols")) {
                return K_CONTROLS;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "hange")) {
                return K_CHANGE;
            }
            if (!strcmp(s, "heckbox")) {
                return K_CHECKBOX;
            }
            if (!strcmp(s, "hecked")) {
                return K_CHECKED;
            }
            if (!strcmp(s, "lick")) {
                return K_CLICK;
            }
            if (!strcmp(s, "hart")) {
                return K_CHART;
            }
            if (!strcmp(s, "lip")) {
                return K_CLIP;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "lock-hand")) {
                return K_CLOCK_HAND;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            if (!strcmp(s, "ontain")) {
                return K_CONTAIN;
            }
            if (!strcmp(s, "over")) {
                return K_COVER;
            }
            break;
        case 'd':
#ifdef OH_UI_PROGRESS_TOUCHABLE
            if (!strcmp(s, "isabled")) {
                return K_DISABLED;
            }
            if (!strcmp(s, "ragTriggerListener")) {
                return K_DRAG_TRIGGER_LISTENER;
            }
#endif
            if (!strcmp(s, "atasets")) {
                return K_DATASETS;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "ay")) {
                return K_DAY;
            }
#endif
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
            if (!strcmp(s, "ate")) {
                return K_DATE;
            }
#endif // FEATURE_COMPONENT_DATE_PICKER
            if (!strcmp(s, "irection")) {
                return K_DIRECTION;
            }
            if (!strcmp(s, "isplay")) {
                return K_DISPLAY;
            }
            if (!strcmp(s, "iv")) {
                return K_DIV;
            }
            if (!strcmp(s, "rag")) {
                return K_DRAG;
            }
            if (!strcmp(s, "uration")) {
                return K_DURATION;
            }
            break;
        case 'e':
            if (!strcmp(s, "ase-in")) {
                return K_EASE_IN;
            }
            if (!strcmp(s, "ase-in-out")) {
                return K_EASE_IN_OUT;
            }
            if (!strcmp(s, "ase-out")) {
                return K_EASE_OUT;
            }
            if (!strcmp(s, "llipsis")) {
                return K_ELLIPSIS;
            }
#if (FEATURE_COMPONENT_DATE_PICKER == 1)
            if (!strcmp(s, "nd")) {
                return K_END;
            }
#endif // FEATURE_COMPONENT_DATE_PICKER
#if ((FEATURE_COMPONENT_CAMERA == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "rror")) {
                return K_ERROR;
            }
#endif // ((FEATURE_COMPONENT_CAMERA == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "xpand")) {
                return K_EXPAND;
            }
            break;
        case 'f':
            if (!strcmp(s, "alse")) {
                return K_FALSE;
            }
            if (!strcmp(s, "ill")) {
                return K_FILL;
            }
            if (!strcmp(s, "illmode")) {
                return K_FILL_MODE;
            }
            if (!strcmp(s, "ixedsize")) {
                return K_FIXED_SIZE;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "inish")) {
                return K_FINISH;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "itOriginalSize")) {
                return K_FIT_ORIGINAL_SIZE;
            }
            if (!strcmp(s, "lex-end")) {
                return K_FLEX_END;
            }
            if (!strcmp(s, "lex-start")) {
                return K_FLEX_START;
            }
            if (!strcmp(s, "lexDirection")) {
                return K_FLEX_DIRECTION;
            }
            if (!strcmp(s, "lexWrap")) {
                return K_FLEX_WRAP;
            }
            if (!strcmp(s, "ontSize")) {
                return K_FONT_SIZE;
            }
            if (!strcmp(s, "ontFamily")) {
                return K_FONT_FAMILY;
            }
            if (!strcmp(s, "orwards")) {
                return K_FORWARDS;
            }
            break;
        case 'h':
            if (!strcmp(s, "eight")) {
                return K_HEIGHT;
            }
            if (!strcmp(s, "xRotateFrom")) {
                return K_HX_ROTATE_FROM;
            }
            if (!strcmp(s, "xRotateTo")) {
                return K_HX_ROTATE_TO;
            }
#if ((FEATURE_COMPONENT_ANALOG_CLOCK == 1) || (FEATURE_DATE_FORMAT == 1))
            if (!strcmp(s, "our")) {
                return K_HOUR;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "our12")) {
                return K_HOUR12;
            }
#endif
            break;
        case 'i':
            if (!strcmp(s, "d")) {
                return K_ID;
            }
            if (!strcmp(s, "mage")) {
                return K_IMAGE;
            }
            if (!strcmp(s, "mage-animator")) {
                return K_IMAGE_ANIMATOR;
            }
            if (!strcmp(s, "mages")) {
                return K_IMAGES;
            }
            if (!strcmp(s, "nput")) {
                return K_INPUT;
            }
            if (!strcmp(s, "ndex")) {
                return K_INDEX;
            }
            if (!strcmp(s, "temselected")) {
                return K_ITEM_SELECTED;
            }
            if (!strcmp(s, "teration")) {
                return K_ITERATION;
            }
            break;
        case 'j':
            if (!strcmp(s, "ustifyContent")) {
                return K_JUSTIFY_CONTENT;
            }
            break;
#ifdef JS_EXTRA_EVENT_SUPPORT
        case 'k':
            if (!strcmp(s, "ey")) {
                return K_KEY;
            }
            break;
#endif
        case 'l':
            if (!strcmp(s, "abels")) {
                return K_LABELS;
            }
            if (!strcmp(s, "arger")) {
                return K_LARGER;
            }
            if (!strcmp(s, "eft")) {
                return K_LEFT;
            }
            if (!strcmp(s, "etterSpacing")) {
                return K_LETTER_SPACING;
            }
            if (!strcmp(s, "ineHeight")) {
                return K_LINE_HEIGHT;
            }
            if (!strcmp(s, "ist")) {
                return K_LIST;
            }
            if (!strcmp(s, "ist-item")) {
                return K_LIST_ITEM;
            }
            if (!strcmp(s, "ongpress")) {
                return K_LONGPRESS;
            }
            if (!strcmp(s, "oop")) {
                return K_LOOP;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "ong")) {
                return K_LONG;
            }
#endif
            break;
        case 'm':
            if (!strcmp(s, "argin")) {
                return K_MARGIN;
            }
            if (!strcmp(s, "arginBottom")) {
                return K_MARGIN_BOTTOM;
            }
            if (!strcmp(s, "arginLeft")) {
                return K_MARGIN_LEFT;
            }
            if (!strcmp(s, "arginRight")) {
                return K_MARGIN_RIGHT;
            }
            if (!strcmp(s, "arginTop")) {
                return K_MARGIN_TOP;
            }
            if (!strcmp(s, "arquee")) {
                return K_MARQUEE;
            }
            if (!strcmp(s, "ax")) {
                return K_MAX;
            }
            if (!strcmp(s, "in")) {
                return K_MIN;
            }
            if (!strcmp(s, "ini")) {
                return K_MINI;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "uted")) {
                return K_MUTED;
            }
#endif // FEATURE_COMPONENT_VIDEO
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "inimumFractionDigits")) {
                return K_MINIMUMFRACTIONDIGITS;
            }
            if (!strcmp(s, "aximumFractionDigits")) {
                return K_MAXIMUMFRACTIONDIGITS;
            }
#endif
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "inute")) {
                return K_MINUTE;
            }
            if (!strcmp(s, "onth")) {
                return K_MONTH;
            }
#endif
            break;
        case 'n':
            if (!strcmp(s, "ame")) {
                return K_NAME;
            }
            if (!strcmp(s, "one")) {
                return K_NONE;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "umeric")) {
                return K_NUMERIC;
            }
#endif
            break;
        case 'o':
            if (!strcmp(s, "bjectFit")) {
                return K_OBJECT_FIT;
            }
            if (!strcmp(s, "pacity")) {
                return K_OPACITY;
            }
            if (!strcmp(s, "ptions")) {
                return K_OPTIONS;
            }
            break;
        case 'p':
            if (!strcmp(s, "adding")) {
                return K_PADDING;
            }
            if (!strcmp(s, "addingBottom")) {
                return K_PADDING_BOTTOM;
            }
            if (!strcmp(s, "addingLeft")) {
                return K_PADDING_LEFT;
            }
            if (!strcmp(s, "addingRight")) {
                return K_PADDING_RIGHT;
            }
            if (!strcmp(s, "addingTop")) {
                return K_PADDING_TOP;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ause")) {
                return K_PAUSE;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "ercent")) {
                return K_PERCENT;
            }
            if (!strcmp(s, "icker-view")) {
                return K_PICKER_VIEW;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "ivotX")) {
                return K_PIVOT_X;
            }
            if (!strcmp(s, "ivotY")) {
                return K_PIVOT_Y;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "repared")) {
                return K_PREPARED;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "rogress")) {
                return K_PROGRESS;
            }
            break;
#if (FEATURE_COMPONENT_QRCODE == 1)
        case 'q':
            if (!strcmp(s, "rcode")) {
                return K_QRCODE;
            }
            break;
#endif // FEATURE_COMPONENT_QRCODE
        case 'r':
            if (!strcmp(s, "adio")) {
                return K_RADIO;
            }
            if (!strcmp(s, "adius")) {
                return K_RADIUS;
            }
            if (!strcmp(s, "ange")) {
                return K_RANGE;
            }
            if (!strcmp(s, "ef")) {
                return K_REF;
            }
            if (!strcmp(s, "everse")) {
                return K_REVERSE;
            }
            if (!strcmp(s, "ight")) {
                return K_RIGHT;
            }
            if (!strcmp(s, "otate")) {
                return K_ROTATE;
            }
            if (!strcmp(s, "ow")) {
                return K_ROW;
            }
            if (!strcmp(s, "ow-reverse")) {
                return K_ROW_REVERSE;
            }
            if (!strcmp(s,"adiusDirection"))
            {
                return K_RADIUS_DIRECTION;
            }
            break;
        case 's':
            if (!strcmp(s, "cale-down")) {
                return K_SCALE_DOWN;
            }
            if (!strcmp(s, "crollamount")) {
                return K_SCROLLAMOUNT;
            }
            if (!strcmp(s, "crolldelay")) {
                return K_SCROLLDELAY;
            }
            if (!strcmp(s, "crollend")) {
                return K_SCROLLEND;
            }
            if (!strcmp(s, "crollstart")) {
                return K_SCROLLSTART;
            }
            if (!strcmp(s, "crolltop")) {
                return K_SCROLLTOP;
            }
            if (!strcmp(s, "crollbottom")) {
                return K_SCROLLBOTTOM;
            }
            if(!strcmp(s,"cale")){
                return K_SCALE;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "ec")) {
                return K_SEC;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "eeked")) {
                return K_SEEKED;
            }
            if (!strcmp(s, "eeking")) {
                return K_SEEKING;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "elected")) {
                return K_SELECTED;
            }
            if (!strcmp(s, "electedColor")) {
                return K_SELECTED_COLOR;
            }
            if (!strcmp(s, "electedFontFamily")) {
                return K_SELECTED_FONT_FAMILY;
            }
            if (!strcmp(s, "electedFontSize")) {
                return K_SELECTED_FONT_SIZE;
            }
            if (!strcmp(s, "how")) {
                return K_SHOW;
            }
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "hort")) {
                return K_SHORT;
            }
#endif
            if (!strcmp(s, "lider")) {
                return K_SLIDER;
            }
            if (!strcmp(s, "pace-around")) {
                return K_SPACE_AROUND;
            }
            if (!strcmp(s, "pace-between")) {
                return K_SPACE_BETWEEN;
            }
            if (!strcmp(s, "pace-evenly")) {
                return K_SPACE_EVENLY;
            }
            if (!strcmp(s, "rc")) {
                return K_SRC;
            }
            if (!strcmp(s, "tack")) {
                return K_STACK;
            }
            if (!strcmp(s, "top")) {
                return K_STOP;
            }
#if ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "tart")) {
                return K_START;
            }
#endif // ((FEATURE_COMPONENT_DATE_PICKER == 1) || (FEATURE_COMPONENT_VIDEO == 1))
            if (!strcmp(s, "tartAngle")) {
                return K_START_ANGLE;
            }
            if (!strcmp(s, "tandard")) {
                return K_STANDARD;
            }
            if (!strcmp(s, "trokeWidth")) {
                return K_STROKE_WIDTH;
            }
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "tyle")) {
                return K_STYLE;
            }
#endif // FEATURE_NUMBER_FORMAT
            if (!strcmp(s, "wipe")) {
                return K_SWIPE;
            }
            if (!strcmp(s, "wiper")) {
                return K_SWIPER;
            }
            if (!strcmp(s, "witch")) {
                return K_SWITCH;
            }
            break;
        case 't':
#if (FEATURE_COMPONENT_TABS == 1)
            if (!strcmp(s, "ab-bar")) {
                return K_TAB_BAR;
            }
            if (!strcmp(s, "ab-content")) {
                return K_TAB_CONTENT;
            }
            if (!strcmp(s, "abs")) {
                return K_TABS;
            }
#endif // FEATURE_COMPONENT_TABS
            if (!strcmp(s, "ime")) {
                return K_TIME;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "imeupdate")) {
                return K_TIME_UPDATE;
            }
#endif // FEATURE_COMPONENT_VIDEO
            if (!strcmp(s, "op")) {
                return K_TOP;
            }
            if (!strcmp(s, "otalAngle")) {
                return K_TOTAL_ANGLE;
            }
            if (!strcmp(s, "ouchend")) {
                return K_TOUCHEND;
            }
            if (!strcmp(s, "ouchmove")) {
                return K_TOUCHMOVE;
            }
            if (!strcmp(s, "ouchstart")) {
                return K_TOUCHSTART;
            }
#ifdef JS_EXTRA_EVENT_SUPPORT
            if (!strcmp(s, "ouchcancel")) {
                return K_TOUCHCANCEL;
            }
#endif
            if (!strcmp(s, "ext")) {
                return K_TEXT;
            }
            if (!strcmp(s, "ransform")) {
                return K_TRANSFORM;
            }
            if (!strcmp(s, "ranslateX")) {
                return K_TRANSLATE_X;
            }
            if (!strcmp(s, "ranslateY")) {
                return K_TRANSLATE_Y;
            }
            if (!strcmp(s, "rue")) {
                return K_TRUE;
            }
            if (!strcmp(s, "extAlign")) {
                return K_TEXT_ALIGN;
            }
            if (!strcmp(s, "extOverflow")) {
                return K_TEXT_OVERFLOW;
            }
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            if (!strcmp(s, "ype")) {
                return K_TYPE;
            }
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            break;
        case 'u':
#if (FEATURE_NUMBER_FORMAT == 1)
            if (!strcmp(s, "seGrouping")) {
                return K_USEGROUP;
            }
#endif // FEATURE_NUMBER_FORMAT
            break;
        case 'v':
            if (!strcmp(s, "alue")) {
                return K_VALUE;
            }
            if (!strcmp(s, "ertical")) {
                return K_VERTICAL;
            }
#if (FEATURE_COMPONENT_VIDEO == 1)
            if (!strcmp(s, "ideo")) {
                return K_VIDEO;
            }
#endif // FEATURE_COMPONENT_VIDEO
            break;
        case 'w':
            if (!strcmp(s, "idth")) {
                return K_WIDTH;
            }
            if (!strcmp(s, "rap")) {
                return K_WRAP;
            }
            break;
        case '2':
#if (FEATURE_DATE_FORMAT == 1)
            if (!strcmp(s, "-digit")) {
                return K_DIGIT2;
            }
            break;
#endif
        default:
            break;
            // clang-format on
    }
    return K_UNKNOWN;
}

uint16_t KeyParser::ParseKeyId(const char * const s)
{
    if (s == nullptr) {
        return K_UNKNOWN;
    }

    size_t len = strlen(s);
    if (len >= UINT16_MAX) {
        return K_UNKNOWN;
    }
    return ParseKeyId(s, len);
}

bool KeyParser::IsKeyValid(uint16_t id)
{
    return ((id > K_UNKNOWN) && (id < KEYWORDS_MAX));
}

const char *KeyParser::GetKeyById(uint16_t id)
{
    if (!IsKeyValid(id)) {
        return "UNKNOWN";
    }
#if (ENABLE_KEY == 1)
    return G_KEYWORD_INFO[id].key;
#else
    return "UNKNOWN";
#endif // ENABLE_KEY
}

uint8_t KeyParser::GetKeyLengthById(uint16_t id)
{
    if (!IsKeyValid(id)) {
        return 0;
    }
    return G_KEYWORD_INFO[id].LENGTH;
}
} // namespace ACELite
} // namespace OHOS
