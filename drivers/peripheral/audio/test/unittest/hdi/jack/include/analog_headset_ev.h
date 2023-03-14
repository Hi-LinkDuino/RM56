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

#ifndef ANALOG_HEADSET_EV_H
#define ANALOG_HEADSET_EV_H

#define HEADSET_JACK_INDEX     3   // 0, 1=touch, 2=no response, 3=jack, 4 not found
#define INDEV_TYPE_HEADSET     4
#define KEY_JACK_HEADSET       20  // EXTCON_JACK_MICROPHONE(20)
#define KEY_JACK_HEADPHONE     21  // EXTCON_JACK_HEADPHONE(21)
#define KEY_JACK_HOOK          226 // KEY_MEDIA (226)

enum JackType {
    JACK_TYPE_HEADSET = 0,
    JACK_TYPE_HEADPHONE,
    JACK_TYPE_LINEOUT,
    JACK_TYPE_UNKNOWN = 10,
};

enum JackInsertStatus {
    JACK_STATUS_IN = 0,
    JACK_STATUS_OUT,
};

enum JackState {
    JACK_STATE_HEADSET_OUT = 0,
    JACK_STATE_HEADPHONE_OUT,
    JACK_STATE_HEADSET_IN,
    JACK_STATE_HEADPHONE_IN,
    JACK_STATE_UNKNOWN,
};

struct JackNotifyInfo {
    uint16_t jackType;
    bool jackStatus;
    uint16_t jackState;
};

#endif