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

#ifndef AUDIO_COMMON_H
#define AUDIO_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "securec.h"
namespace OHOS::HDI::Audio_Bluetooth {
enum {INFO};
#define FILE_NAME_LEN 256
#define TIME_LEN 32

void AudioMemFree(void **ppMem);
int32_t AudioGetSysTime(char *s, int32_t len);
int32_t CheckAttrFormat(AudioFormat param);
int32_t CheckAttrSamplingRate(uint32_t param);
int32_t AudioCheckParaAttr(const struct AudioSampleAttributes *attrs);
int32_t TimeToAudioTimeStamp(uint64_t bufferFrameSize, struct AudioTimeStamp *time, uint32_t sampleRate);
}
#endif
