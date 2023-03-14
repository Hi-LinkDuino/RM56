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

#include "../include/player_factory.h"

#include "histreamer/hiplayer.h"
#include "media_log.h"
#include "parameter.h"
#include "player_impl.h"

namespace OHOS {
namespace Media {
std::shared_ptr<PlayerInterface> PlayerFactory::CreatePlayer(PlayerId playerId)
{
    std::shared_ptr<PlayerInterface> player = nullptr;
    switch (playerId) {
        case PlayerId::HISTREAMER:
            player = CreateHiPlayer();
            break;
        case PlayerId::PLAYER_LITE:
        default:
            player = std::shared_ptr<PlayerInterface>(new (std::nothrow) PlayerImpl());
            break;
    }
    return player;
}

std::shared_ptr<PlayerInterface> PlayerFactory::CreatePlayer()
{
    MEDIA_INFO_LOG("create player");
    char useHistreamer [10] = {0}; // 10 for system parameter usage
    auto res = GetParameter("debug.media_service.histreamer", "0", useHistreamer,
        10); // 10 for system parameter usage
    if (res == 1 && useHistreamer[0] == '1') {
        MEDIA_INFO_LOG("enable histreamer");
        return CreateHiPlayer();
    } else {
        MEDIA_INFO_LOG("enable liteplayer");
        return std::shared_ptr<PlayerInterface>(new (std::nothrow) PlayerImpl());
    }
}
} // namespace Media
} // namespace OHOS