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

#ifndef HISTREAMER_NETWORK_CLIENT_H
#define HISTREAMER_NETWORK_CLIENT_H

#include <string>
#include <common/plugin_event.h>
#include "common/plugin_types.h"
#include "network_typs.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class NetworkClient {
public:
    virtual ~NetworkClient() = default;
    virtual Status Init() = 0;
    virtual Status Open(const std::string& url) = 0;
    virtual Status RequestData(long startPos, int len, NetworkServerErrorCode& serverCode,
                               NetworkClientErrorCode& clientCode) = 0;
    virtual Status Close() = 0;
    virtual Status Deinit() = 0;
};
}
}
}
}


#endif