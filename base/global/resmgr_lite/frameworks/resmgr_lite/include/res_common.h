/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_RES_COMMON_H
#define OHOS_RES_COMMON_H

namespace OHOS {
namespace Global {
namespace Resource {
// DIRECTION
static const char *VERTICAL = "vertical";
static const char *HORIZONTAL = "horizontal";

// DEVICETYPE
static const char *PHONE_STR = "phone";
static const char *TABLET_STR = "tablet";
static const char *CAR_STR = "car";
static const char *PAD_STR = "pad";
static const char *TV_STR = "tv";
static const char *WEARABLE_STR = "wearable";

// ScreenDensity
static const char *RE_120_STR = "sdpi";
static const char *RE_160_STR = "mdpi";
static const char *RE_240_STR = "ldpi";
static const char *RE_320_STR = "xldpi";
static const char *RE_480_STR = "xxldpi";
static const char *RE_640_STR = "xxxldpi";

// the type of qualifiers
typedef enum KeyType {
    LANGUAGES       = 0,
    REGION          = 1,
    SCREEN_DENSITY  = 2,
    DIRECTION       = 3,
    DEVICETYPE      = 4,
    SCRIPT          = 5,
    NIGHTMODE       = 6, // DARK = 0, LIGHT = 1
    UNKNOWN         = 7,
    MCC             = 8,
    MNC             = 9,
    KEY_TYPE_MAX,
} KeyType;

// the type of resources
typedef enum ResType {
    VALUES    = 0,
    ANIMATION = 1,
    DRAWABLE  = 2,
    LAYOUT    = 3,
    MENU      = 4,
    MIPMAP    = 5,
    RAW       = 6,
    XML       = 7,

    INTEGER          = 8,
    STRING           = 9,
    STRINGARRAY      = 10,
    INTARRAY         = 11,
    BOOLEAN          = 12,
    DIMEN            = 13,
    COLOR            = 14,
    ID               = 15,
    THEME            = 16,
    PLURALS          = 17,
    FLOAT            = 18,
    MEDIA            = 19,
    PROF             = 20,
    SVG              = 21,
    PATTERN          = 22,
    MAX_RES_TYPE     = 23,
} ResType;

enum DeviceType {
    DEVICE_NOT_SET  = -1,
    DEVICE_PHONE    = 0,
    DEVICE_TABLET   = 1,
    DEVICE_CAR      = 2,
    DEVICE_PAD      = 3,
    DEVICE_TV       = 4,
    DEVICE_WEARABLE = 6,
};

enum NightMode {
    DARK     = 0,
    LIGHT    = 1,
};

enum ScreenDensity {
    SCREEN_DENSITY_NOT_SET  = 0,
    SCREEN_DENSITY_SDPI     = 120,
    SCREEN_DENSITY_MDPI     = 160,
    SCREEN_DENSITY_LDPI     = 240,
    SCREEN_DENSITY_XLDPI    = 320,
    SCREEN_DENSITY_XXLDPI   = 480,
    SCREEN_DENSITY_XXXLDPI  = 640,
};

enum Direction {
    DIRECTION_NOT_SET = -1,
    DIRECTION_VERTICAL = 0,
    DIRECTION_HORIZONTAL = 1
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif // OHOS_RES_COMMON_H