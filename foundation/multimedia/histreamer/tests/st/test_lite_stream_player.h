/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifdef OHOS_LITE

#include <thread>
#include <chrono>

#include "testngpp/testngpp.hpp"


int StartLiteStreamPlayer(std::string dataPath);

// @fixture(tags=audio_play_fast)
FIXTURE(TestLiteStreamPlayerFast)
{
    DATA_PROVIDER(musicUrls, 1,
    DATA_GROUP(std::string(RESOURCE_DIR "/WAV/wav_stream")));

    // @test(data="musicUrls")
    PTEST((std::string url), Test play stream source automatically stop)
    {
        ASSERT_EQ(0, StartLiteStreamPlayer(url));
    }
};
#endif
