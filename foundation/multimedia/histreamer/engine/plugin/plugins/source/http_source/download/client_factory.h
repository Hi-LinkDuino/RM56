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

#ifndef HISTREAMER_CLIENT_FACTORY_H
#define HISTREAMER_CLIENT_FACTORY_H

#include <memory>
#include <string>
#include "network_client.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class ClientFactory {
public:
    ClientFactory(RxHeader headCallback, RxBody bodyCallback, void *userParam);
    ~ClientFactory();

    std::shared_ptr<NetworkClient> CreateClient(const std::string& url);

private:
    RxHeader rxHeader_;
    RxBody rxBody_;
    void *userParam_;
    std::shared_ptr<NetworkClient> httpClient_;
    std::shared_ptr<NetworkClient> httpsClient_;
};
}
}
}
}
#endif