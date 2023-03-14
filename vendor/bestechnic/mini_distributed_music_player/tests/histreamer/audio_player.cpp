/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include <unistd.h>
#include "histreamer/hiplayer.h"

using namespace OHOS::Media;

std::string GetMusicUri()
{
    std::string uri = "/data/bbg.mp3";
    return uri;
}

int AudioPlayerStart(void)
{
    auto player = OHOS::Media::CreateHiPlayer();
    player->Init();
    OHOS::Media::Source source(GetMusicUri());
    player->SetSource(source);
    player->SetLoop(true);
    player->Play();
    while(1) {
        sleep(1);
    }
    return 0;
}
