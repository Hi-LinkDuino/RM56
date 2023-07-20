/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_JS_FWK_COMMON_H
#define OHOS_ACELITE_JS_FWK_COMMON_H

#include "handler.h"
#include "js_config.h"
#include "non_copyable.h"
#ifdef JS_EXTRA_EVENT_SUPPORT
#include "root_view.h"
#endif
#include "graphic_config.h"
#include "wrapper/js.h"
namespace OHOS {
namespace ACELite {
struct Watcher : public MemoryHeap {
    ACE_DISALLOW_COPY_AND_MOVE(Watcher);
    Watcher() : watcher(jerry_create_undefined()), next(nullptr) {}
    jerry_value_t watcher;
    struct Watcher *next;
};

/*
 * Remove parameters, variables, or expressions compiling warning.
 */
#undef UNUSED // incase others define the same macro
#define UNUSED(a) (void)(a)

#ifndef UNDEFINED
#define UNDEFINED jerry_create_undefined()
#endif

#ifndef IS_UNDEFINED
#define IS_UNDEFINED(v) jerry_value_is_undefined(v)
#endif

#ifndef IS_ERROR_VALUE
#define IS_ERROR_VALUE(v) jerry_value_is_error(v)
#endif

#ifndef ACE_FREE
#define ACE_FREE(pointer)     \
    if (pointer != nullptr) { \
        ace_free(pointer);    \
        pointer = nullptr;    \
    }
#endif // ACE_FREE

#ifndef ACE_DELETE
#define ACE_DELETE(pointer)   \
    if (pointer != nullptr) { \
        delete pointer;       \
        pointer = nullptr;    \
    }
#endif // ACE_DELETE

void ThrowError();

#if IS_ENABLED(JS_PROFILER)
#ifndef LOG_PROFILER_TRACE
#define LOG_PROFILER(format, ...) printf(format "\n", ##__VA_ARGS__)
#define LOG_PROFILER_TRACE(format, ...) printf("[PERFORMANCE]:" format "\n", ##__VA_ARGS__)
#endif
#else
#ifndef LOG_PROFILER_TRACE
#define LOG_PROFILER(format, ...)
#define LOG_PROFILER_TRACE(format, ...)
#endif
#endif

constexpr uint16_t PATH_LENGTH_MAX = 1024;
constexpr uint8_t NAME_LENGTH_MAX = 255;
constexpr uint8_t DEVICE_ID_LENGTH_MAX = 65;
constexpr uint16_t FILE_CONTENT_LENGTH_MAX = 1024 * 48;

// hex code
constexpr uint8_t DEC = 10;
constexpr uint8_t HEX = 16;

constexpr int SIZE_EXPRESSION_PLACE_HOLDER = 4;
constexpr int IDX_EXPRESSSION_STRING = 2;
constexpr int ARG_LENGTH_WATCHER = 3;
constexpr int ARG_LENGTH_WATCHER_CALLBACK = 3;

constexpr char ATTR_ROOT[] = "$root"; // global.$root represents for abilitySlice

constexpr uint32_t RGB_COLOR_VALUE_MAX = 0x1000000;
constexpr uint32_t TEXT_RED_COLOR_MASK = 0xff0000;
constexpr uint32_t TEXT_GREEN_COLOR_MASK = 0x00ff00;
constexpr uint32_t TEXT_BLUE_COLOR_MASK = 0x0000ff;
constexpr int RED_COLOR_START_BIT = 16;
constexpr int GREEN_COLOR_START_BIT = 8;
constexpr char ATTR_SRC[] = "src"; // image-animator
constexpr char ATTR_TITLE[] = "title"; //bes-menu-list
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
constexpr char CLOCK_HAND_IS_IMAGE[] = "isImage";
constexpr char COMMON_STYLE_OPACITY[] = "opacity";
constexpr char COMMON_STYLE_COLOR[] = "color";
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
constexpr uint8_t DEFAULT_FONT_SIZE = 30;
#if (FEATURE_COMPONENT_CANVAS == 1)
constexpr uint8_t DEFAULT_FONT_LETTERSPACE = 2;
#endif // FEATURE_COMPONENT_CANVAS
constexpr char DEFAULT_FONT_FAMILY[] = DEFAULT_VECTOR_FONT_FILENAME;

// common attributes
constexpr char ATTR_WIDTH[] = "width";   // tab-bar | image-animator
constexpr char ATTR_HEIGHT[] = "height"; // tab-bar | image-animator
constexpr char ATTR_LEFT[] = "left";     // image-animator
constexpr char ATTR_TOP[] = "top";       // image-animator
constexpr char ATTR_TYPE[] = "type";
constexpr char COMMON_STYLE_BACKGROUND_COLOR[] = "backgroundColor";
constexpr char ARG_WATCH_EL[] = "el";
constexpr char ARG_WATCH_ATTR[] = "attr";
constexpr char ATTR_ATTRS[] = "attrs";
constexpr char ATTR_FREEZE[] = "freeze";
constexpr char ABILITY_LIFECYCLE_CALLBACK_ON_CREATE[] = "onCreate";
constexpr char ABILITY_LIFECYCLE_CALLBACK_ON_DESTROY[] = "onDestroy";
constexpr char BACK_PRESSED_NAME[] = "$backPress";
constexpr char ROUTER_PAGE_URI[] = "uri";
constexpr char ROUTER_PAGE_PARAMS[] = "params";
constexpr char ROUTER_PAGE_PATH[] = "path";
constexpr char ROUTER_PAGE[] = "$page";
constexpr char ATTR_REFS[] = "$refs";
constexpr char ATTR_SUCCESS[] = "success";
constexpr char ATTR_FAIL[] = "fail";
constexpr char ATTR_COMPLETE[] = "complete";

// chart style
constexpr uint8_t POINT_SIZE = 5;

// animation style
constexpr char ANIMATION_VALUE_SEP[] = ",";
constexpr char ATTR_APP[] = "$app";
constexpr char ATTR_CHILDREN[] = "children";
constexpr char ATTR_RENDER[] = "$render";
constexpr char ATTR_WATCH[] = "$watch";
constexpr char DESCRIPTOR_ATTR_FOR[] = "for";
constexpr char DESCRIPTOR_ATTR_NODES[] = "nodes";
constexpr char DESCRIPTOR_ATTR_RENDER[] = "render";
constexpr char DESCRIPTOR_ATTR_GETTER[] = "getter";
constexpr char TRANSITION_ROTATE[] = "rotate";
constexpr char TRANSITION_SCALE[] = "scale";
constexpr char TRANSITION_TRANSFORM_X[] = "translateX";
constexpr char TRANSITION_TRANSFORM_Y[] = "translateY";
constexpr char CONSTRUCTOR_VIEW_MODEL[] = "ViewModel";
constexpr char CONSTRUCTOR_ABILITY_SLICE[] = "AbilitySlice";

#if (FEATURE_ROTATION_API == 1)
constexpr char FUNC_ROTATION_NAME[] = "rotation";
constexpr char ATTR_NAME_FOCUS[] = "focus";
#endif // FEATURE_ROTATION_API

// get screen horizontal resolution
uint16_t GetHorizontalResolution();
// get screen vertical resolution
uint16_t GetVerticalResolution();

// global functions
void JerrySetNamedProperty(jerry_value_t object, const char * const name, jerry_value_t propValue);
void JerrySetNumberProperty(jerry_value_t object, const char * const name, double value);
void JerrySetStringProperty(jerry_value_t object, const char * const name, const char * const value);
void JerrySetStringProperty(jerry_value_t object, const char * const name, const char * const value, uint32_t length);
// value returned should be freed by caller when it's not nullptr and won't be used any more
char *JerryMallocStringProperty(const jerry_value_t object, const char * const name, uint16_t &length);
int16_t JerryGetIntegerProperty(jerry_value_t object, const char * const name);
/**
 * @brief try to parse bool property from one JS object, caller should check the returned result before using outValue.
 * @param object the given object
 * @param name the target property name
 * @param outValue the target property value
 * @return true if the parse successfully
 */
bool JerryGetBoolProperty(jerry_value_t object, const char * const name, bool &outValue);
void JerrySetFuncProperty(jerry_value_t object, const char * const name, jerry_external_handler_t handler);
char *MallocStringOf(jerry_value_t source);
// convert one jerry string value to char*, and return the string length
char *MallocStringOf(jerry_value_t source, uint16_t *strLength);
int16_t IntegerOf(jerry_value_t source);
bool BoolOf(jerry_value_t source);

// relocate file name to full path of the current app path
char *RelocateFilePath(const char *appRootPath, const char *subPath, const char *fileName);

// relocate file name to appRootPath/dirA/dirB/fileName
// e.x. /system/app/73709738-2d9d-4947-ac63-9858dcae7ccb/pages/index/right.png
char *RelocateFilePathRelative(const char * const appRootPath, const char * const resFileName);

// returned value must be freed by caller
char *RelocateResourceFilePath(const char * const appRootPath, const char * const resFileName);
// returned value must be freed by caller
char *RelocateJSSourceFilePath(const char * const appRootPath, const char * const srcFileName);
// read from src file, return value must be freed by caller
char *ReadJSFile(const char * const appPath, const char * const jsFileName);
// read the given file and return the whole file content
char *ReadJSFile(const char * const appPath, const char * const jsFileName, uint32_t &fileSize);
// read the given file and return the whole file content
char *ReadFile(const char * const fullPath, uint32_t &fileSize, const bool binary);
// check whether the given file exists
bool IsFileExisted(const char * const filePath);
// Whether file existed and return the file size. 0 represents file not existed.
int32_t GetFileSize(const char * const filePath);

/**
 * @brief give out the path string from an url pattern, "url(common/logo.png)".
 *
 * @param url the given url
 *
 * @return the url itself, "common/logo.png"
 */
char *CreatePathStrFromUrl(const char * const url);
/**
 * @brief Call jerry_has_property and return the result
 *
 * @param container the target JS object to check
 * @param property the property string value
 */
bool JerryHasProperty(const jerry_value_t container, const jerry_value_t property);
/**
 * @brief Call jerry_has_own_property and return the result
 *
 * @param container the target JS object to check
 * @param property the property string value
 */
bool HasOwnProperty(const jerry_value_t container, const jerry_value_t property);
// watcher callback function
jerry_value_t WatcherCallbackFunc(const jerry_value_t func,
                                  const jerry_value_t context,
                                  const jerry_value_t *args,
                                  const jerry_length_t argsLength);
// print out error information from jerry value
void PrintErrorMessage(const jerry_value_t errorValue);
// wrapper function for jerry_call_function
jerry_value_t CallJSFunction(const jerry_value_t func,          /* function object to call */
                             const jerry_value_t context,       /* object for 'this' binding */
                             const jerry_value_t args[],        /* function's call arguments */
                             const jerry_size_t argsCount);     /* number of the arguments */
void CallJSFunctionAutoRelease(const jerry_value_t funcObj,     /* function object to call */
                               const jerry_value_t thisVal,     /* object for 'this' binding */
                               const jerry_value_t args[],      /* function's call arguments */
                               const jerry_size_t argc);        /* number of the arguments */
jerry_value_t CallJSFunctionOnRoot(const jerry_value_t funcObj, /* function object to call */
                                   const jerry_value_t args[],  /* function's call arguments */
                                   const jerry_size_t argc);    /* number of the arguments */
jerry_value_t CallJSWatcher(jerry_value_t arg1,
                            jerry_value_t (*watcherCB)(const jerry_value_t func,
                                                       const jerry_value_t context,
                                                       const jerry_value_t *args,
                                                       const jerry_length_t argsLength),
                            jerry_value_t arg3);
void ReleaseJerryValue(jerry_value_t value, ...);
void InsertWatcherCommon(Watcher *&head, const jerry_value_t watcher);
void ClearWatchersCommon(Watcher *&head);
jerry_value_t ListForWatcherCallbackFunc(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t *args,
                                         const jerry_length_t argsLength);
#ifdef JS_EXTRA_EVENT_SUPPORT
jerry_value_t *ConvertBaseEventInfo(const Event &event, const uint16_t id);
bool CallBaseEvent(const jerry_value_t func, const Event &event, const uint16_t id);
jerry_value_t *ConvertDragEventInfo(const DragEvent &event, const uint16_t id);
jerry_value_t *ConvertKeyEventInfo(const KeyEvent &event);
void ClearEventListener(const jerry_value_t args[], uint8_t argc);
#endif
JSValue GetRootAbilitySlice();
JSValue CallWithRootAbilitySlice(JSValue func, JSValue args[], JSSize size);
JSValue CallWithRootAbilitySlice(JSValue func);
JSValue CreateWatcher(JSValue getter, JSHandler handler, JSValue options);

void ExpandImagePathMem(char *&imagePath, const int16_t dotPos, const int16_t suffixLen, const int16_t imagePathLen);
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
void CureImagePath(char *&imagePath);
#endif // OHOS_ACELITE_PRODUCT_WATCH
const char *ParseImageSrc(jerry_value_t source);

/**
 * @brief ParseKeyIdFromJSString parse key ID from JS value
 * @param str the input JS string value
 * @return the key ID
 */
uint16_t ParseKeyIdFromJSString(const jerry_value_t str);

constexpr char PATH_PREFIX[] = ".";
constexpr char PATH_DEFAULT[] = "/";

constexpr char PREFIX_HEX_COLOR[] = "#";
constexpr char PREFIX_RGB_COLOR[] = "rgb";
constexpr char PREFIX_RGBA_COLOR[] = "rgba";
constexpr uint8_t ALPHA_MAX = 255;
constexpr char BRACE_OPEN = '(';
constexpr char BRACE_CLOSE = ')';
constexpr char DELIMITER[] = ",";
constexpr char RESOURCE_SEPARATOR = '/';

bool StartWith(const char *sequence, const char *prefix);
bool IsHexColor(const char * const target);
bool IsRgbColor(const char * const target);
bool IsRgbaColor(const char * const target);
bool ParseHexColor(const char * const source, uint32_t &color, uint8_t &alpha);
bool ParseRgbaColor(const char * const source, uint32_t &color, uint8_t &alpha);
bool ParseColor(const char * const source, uint32_t &color, uint8_t &alpha);
bool CopyFontFamily(char *&destination, const char * const fontFamily, uint32_t fontFamilyNameLen = 0);

constexpr int16_t BUTT_VALUE = 0;
constexpr int16_t SQUARE_VALUE = 1;
constexpr int16_t ROUND_VALUE = 2;
constexpr int16_t LINEJOIN_MITER_VALUE = 0;
constexpr int16_t LINEJOIN_ROUND_VALUE = 1;
constexpr int16_t LINEJOIN_BEVEL_VALUE = 2;

#if (defined(_WIN32) || defined(_WIN64))
constexpr char PATH_SEPARATOR = '\\';
constexpr char PATH_RELATIVE_WHOLE[] = "\\..\\";
constexpr char PATH_RELATIVE_PART[] = "..\\";
constexpr char SRC_SUB_FOLDER_NAME[] = "";
constexpr char JS_INDEX_FILE_PATH[] = "pages\\index\\index";
#else
constexpr char PATH_SEPARATOR = '/';
constexpr char PATH_RELATIVE_WHOLE[] = "/../";
constexpr char PATH_RELATIVE_PART[] = "../";
constexpr char SRC_SUB_FOLDER_NAME[] = "";
constexpr char JS_INDEX_FILE_PATH[] = "pages/index/index";
#endif

// for error definition
enum {
    FAILED = -1,
    SUCCESS = 0,
    ERROR_BASE = 0x10000,
    ERROR_INPUT_PARAMETER,
    ERROR_MALLOC,
    ERROR_SECURE_USE // use secure function error
};

constexpr int VA_ARG_END_FLAG = -1;
constexpr int INVALID_PIXEL_VALUE = -1;

// switch on random path
#ifndef JS_PAGE_SPECIFIC
#define JS_PAGE_SPECIFIC 0
#endif

#if JS_PAGE_SPECIFIC
constexpr char servicePrefix[] = "pages";
constexpr char serviceSuffix[] = "index";

struct JSPageSpecific : public MemoryHeap {
    char *jsIndexFilePath; // page path
    JSPageSpecific() : jsIndexFilePath(nullptr) {}
};

#endif // JS_PAGE_SPECIFIC

#if (BES_FRATURE_PAGE_TRANSITION == 1)
/** @brief 页面路由字段：页面转场配置 */
constexpr const char BES_FIELD_PAGE_TRANSITION[] = "besTransition";
/** @brief 页面转场字段：预置动画 */
constexpr const char BES_FIELD_PRESET_ANIM[] = "animator";
/** @brief 页面转场字段：转场强调（层级关系），被强调的转场动作将位于顶层 */
constexpr const char BES_FIELD_ANIM_PERFER[] = "perfer";
/** @brief 页面转场类型字段：入场 */
constexpr const char BES_FIELD_ENTER[] = "enter";
/** @brief 页面转场类型字段：退场 */
constexpr const char BES_FIELD_EXIT[] = "exit";
/** @brief 页面转场动画字段：动画名称 */
constexpr const char BES_FIELD_ANIM_NAME[] = "animName";
/** @brief 页面转场动画字段：动画数值列表 */
constexpr const char BES_FIELD_ANIM_VALUES[] = "animValues";
/** @brief 页面转场动画字段：动画持续时间 */
constexpr const char BES_FIELD_ANIM_DURATION[] = "animDuration";
/** @brief 页面转场动画字段：动画时间插值器 */
constexpr const char BES_FIELD_ANIM_INTERPOLATION[] = "animInterpolation";
/** @brief 页面转场动画字段：动画淡入淡出类型 */
constexpr const char BES_FIELD_ANIM_FADE[] = "animFade";
/** @brief 页面转场动画字段：动画基准点（中心） */
constexpr const char BES_FIELD_ANIM_PIVOT[] = "animPivot";
/** @brief 页面转场动画字段：动画开始的延迟时长 */
constexpr const char BES_FIELD_ANIM_DELAY[] = "animDelay";
#endif
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_JS_FWK_COMMON_H
