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

#ifndef AUDIO_COMMON_TEST_H
#define AUDIO_COMMON_TEST_H

#include "audio_internal.h"
#include "audio_interface_lib_common.h"

namespace comfun {
const float GAIN_MIN = 0;
const float GAIN_MAX = 15;
const int PORTNUM = 1;
const int AUDIO_CHANNELCOUNT = 2;
const int AUDIO_PORT_ID = 2; // portId
const int AUDIO_SAMPLE_RATE_48K = 48000;
const int DEEP_BUFFER_RENDER_PERIOD_SIZE = 4096;
const int INT_32_MAX = 0x7fffffff;
const int DEFAULT_RENDER_SAMPLING_RATE = 48000;
const int DEEP_BUFFER_RENDER_PERIOD_COUNT = 8;

int32_t InitPort(struct AudioPort &portIndex);
int32_t InitHwRender(struct AudioHwRender &hwRender,
    const struct AudioDeviceDescriptor &desc, const struct AudioSampleAttributes &attrs);
int32_t InitHwCapture(struct AudioHwCapture &hwCapture, const struct AudioDeviceDescriptor &desc,
    const struct AudioSampleAttributes &attrs);
int32_t InitAttrs(struct AudioSampleAttributes &attrs);
int32_t InitDevDesc(struct AudioDeviceDescriptor &devDesc);
int32_t InitAttrsCapture(struct AudioSampleAttributes &attrs);
int32_t InitDevDescCapture(struct AudioDeviceDescriptor &devDesc);
int32_t AudioRenderCallbackUtTest(enum AudioCallbackType type, void *reserved, void *cookie);
}
#endif
