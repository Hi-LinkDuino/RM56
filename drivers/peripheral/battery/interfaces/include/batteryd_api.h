/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BATTERYD_API_H
#define BATTERYD_API_H

#include <cstdlib>
#include <string>

enum BatterydPluggedType {
    PLUGGED_TYPE_NONE = 0,
    PLUGGED_TYPE_AC,
    PLUGGED_TYPE_USB,
    PLUGGED_TYPE_WIRELESS,
    PLUGGED_TYPE_BUTT
};

// Keep it same as the inner kit battery_info.h
struct BatterydInfo {
    int32_t capacity_;
    int32_t voltage_;
    int32_t temperature_;
    int32_t healthState_;
    int32_t pluggedType_;
    int32_t pluggedMaxCurrent_;
    int32_t pluggedMaxVoltage_;
    int32_t chargeState_;
    int32_t chargeCounter_;
    int32_t totalEnergy_;
    int32_t curAverage_;
    int32_t curNow_;
    int32_t remainEnergy_;
    int8_t present_;
    std::string technology_;
};

#endif // BATTERYD_API_H
