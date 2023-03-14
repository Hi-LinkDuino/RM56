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

#ifndef MULTIMEDIA_PLAYER_FACTORY_H
#define MULTIMEDIA_PLAYER_FACTORY_H

#include <memory>

#include "player_interface.h"

namespace OHOS {
namespace Media {
enum class PlayerId {
    HISTREAMER,
    PLAYER_LITE
};

class PlayerFactory {
public:
  static std::shared_ptr<PlayerInterface> CreatePlayer(PlayerId playerId);
  static std::shared_ptr<PlayerInterface> CreatePlayer();
};
} // namespace Media
} // namespace OHOS
#endif // MULTIMEDIA_PLAYER_FACTORY_H
