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

#include "seq_packet_socket_client.h"

#include <iostream>

namespace OHOS {
namespace HiviewDFX {
using namespace std;
SeqPacketSocketResult SeqPacketSocketClient::Init()
{
    int res = Create();
    if (res < 0) {
        cout << "socket creation result : " << res << std::endl;
        return SeqPacketSocketResult::CREATE_SOCKET_FAILED;
    }

    res = Connect();
    if (res == -1) {
        cout << "socket connect result : " << res << std::endl;
        return SeqPacketSocketResult::CONNECT_FAILED;
    }

    return SeqPacketSocketResult::CREATE_AND_CONNECTED;
}

int SeqPacketSocketClient::RecvMsg(char *buffer, unsigned int len)
{
    return Recv(buffer, len, 0);
}
} // namespace HiviewDFX
} // namespace OHOS
