/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef AUDIOLITETEST_H
#define AUDIOLITETEST_H

#include <sys/time.h>

#include "gtest/gtest.h"
#include "audio_capturer.h"


namespace OHOS {
const int RET_SUCCESS = 0;
const int RET_FAILURE = -1;

struct OHOS::Audio::AudioCapturerInfo audioCapInfo;

class AudioliteTest : public testing::Test {
public:
    // SetUpTestCase: before all testcasee
    static void SetUpTestCase(void);
    // TearDownTestCase: after all testcase
    static void TearDownTestCase(void);
    // SetUp
    void SetUp(void);
    // TearDown
    void TearDown(void);
};
} // namespace OHOS
#endif // AUDIOLITETEST_H
