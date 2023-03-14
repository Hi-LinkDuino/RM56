/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_SOFTBUS_SESSION_H
#define OHOS_DM_SOFTBUS_SESSION_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "inner_session.h"
#include "session.h"
#include "softbus_session_callback.h"

namespace OHOS {
namespace DistributedHardware {
class SoftbusSession {
public:
    static int OnSessionOpened(int sessionId, int result);
    static void OnSessionClosed(int sessionId);
    static void OnBytesReceived(int sessionId, const void *data, unsigned int dataLen);

public:
    SoftbusSession();
    ~SoftbusSession();
    int32_t RegisterSessionCallback(std::shared_ptr<ISoftbusSessionCallback> callback);
    int32_t UnRegisterSessionCallback();
    int32_t OpenAuthSession(const std::string &deviceId);
    int32_t CloseAuthSession(int32_t sessionId);
    int32_t SendData(int32_t sessionId, std::string &message);
    int32_t GetPeerDeviceId(int32_t sessionId, std::string &peerDevId);

private:
    static std::shared_ptr<ISoftbusSessionCallback> sessionCallback_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_SOFTBUS_SESSION_H
