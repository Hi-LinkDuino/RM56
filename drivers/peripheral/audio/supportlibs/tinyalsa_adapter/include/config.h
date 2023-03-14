/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "alsa_audio.h"

#define DEVICES_0 0
struct RouteCfgInfo {
    const char *controlName;   // name of control.
    const char *stringVal;  // value of control, which type is stream.
    const int32_t intVal[2];    // left and right value of control, which type are int32_t.
};

struct PathRoute {
    const int32_t sndCard;
    const int32_t devices;
    const struct RouteCfgInfo *controls;
    const unsigned ctlsNums;
};

struct PathRouteCfgTable {
    const struct PathRoute speakNormal;
    const struct PathRoute headphoneNormal;
    const struct PathRoute HdmiNormal;
    const struct PathRoute speakerHeadphoneNormal;
    const struct PathRoute mainMicCapture;
    const struct PathRoute handsFreeMicCapture;
    const struct PathRoute playbackOff;
    const struct PathRoute captureOff;
};

struct TinyalsaSndCardCfg {
    const char *sndCardName;
    struct PathRouteCfgTable *pathRouteMap;
};

struct PathRouteCfgTable* GetHdmiConfigTable(void);
struct PathRouteCfgTable* GetDefaultConfigTable(void);

#endif // _CONFIG_H_
