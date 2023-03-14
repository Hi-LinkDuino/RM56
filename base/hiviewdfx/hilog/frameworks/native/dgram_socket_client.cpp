/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "dgram_socket_client.h"

#include <unistd.h>

namespace OHOS {
namespace HiviewDFX {
int DgramSocketClient::CheckSocket()
{
    if (fdHandler >= 0) {
        return fdHandler;
    }

    int fd = GenerateFD();
    if (fd < 0) {
        return fd;
    }

    int ret;
    int defaultValue = -1;
    fdHandler.compare_exchange_strong(defaultValue, fd);
    if (defaultValue != -1) {
        ret = defaultValue;
        close(fd);
    } else {
        ret = fd;
        setHandler(fd);
        Connect();
    }

    return ret;
}
} // namespace HiviewDFX
} // namespace OHOS
