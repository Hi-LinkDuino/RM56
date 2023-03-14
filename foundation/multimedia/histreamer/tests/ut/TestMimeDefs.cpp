/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#define private public
#define protected public
#define UNIT_TEST 1

#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Test {
TEST(TestDefs, check_mime_defs)
{
    ASSERT_STREQ(MEDIA_MIME_AUDIO_MPEG, "audio/mpeg");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_FLAC, "audio/flac");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_RAW, "audio/raw");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_APE, "audio/ape");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_WAV, "audio/wav");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AAC, "audio/aac");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AAC_ADTS, "audio/aac-adts");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_VORBIS, "audio/vorbis");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_OPUS, "audio/opus");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AC3, "audio/ac3");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_EAC3, "audio/eac3");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_EAC3_JOC, "audio/eac3-joc");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AC4, "audio/ac4");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_WMA, "audio/x-ms-wma");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AMR_NB, "audio/3gpp");
    ASSERT_STREQ(MEDIA_MIME_AUDIO_AMR_WB, "audio/amr-wb");
}
} // namespace Test
} // namespace Media
} // namespace OHOS