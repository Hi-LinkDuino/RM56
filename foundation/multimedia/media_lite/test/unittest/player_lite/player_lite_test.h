/*
 * Copyright (C) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef PLAYER_LITE_TEST_H
#define PLAYER_LITE_TEST_H

#include "gtest/gtest.h"
#include "player.h"

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;
using OHOS::Media::SourceType;
using OHOS::Media::PlayerCallback;

using namespace std;
using namespace OHOS::Media;
using namespace testing::ext;

const int FILE_PATH_LEN = 2048;

using TestSample = struct TagTestSample {
    std::shared_ptr<Player> adaptr;
    char filePath[FILE_PATH_LEN];
};

static TagTestSample g_tagTestSample;
static Surface *g_surface = Surface::CreateSurface();
} // namespace OHOS
#endif // PLAYER_LITE_TEST_H
