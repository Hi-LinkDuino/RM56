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
#ifndef RECORDER_LITE_TEST_H
#define RECORDER_LITE_TEST_H

#include "gtest/gtest.h"
#include "recorder.h"

using OHOS::Media::RecorderCallback;
namespace OHOS {
class RecoderLiteTest : public testing::Test {
public:
    // SetUpTestCase: before all testcasee
    static void SetUpTestCase(void);
    // TearDownTestCase: after all testcase
    static void TearDownTestCase(void);
    // SetUp
    void SetUp(void);
    // TearDown
    void TearDown(void);

    void OnError(const int32_t errorType, const int32_t errorCode);

    void OnInfo(const int32_t type, const int32_t extra);
};

namespace Media {
class TestVideoRecorderCallback : public RecorderCallback {
public:
    void OnError(int32_t errorType, int32_t errorCode);

    void OnInfo(int32_t type, int32_t extra);
};
}
}
#endif // RECORDER_LITE_TEST_H
