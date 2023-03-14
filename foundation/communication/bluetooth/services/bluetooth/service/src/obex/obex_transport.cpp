/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "obex_transport.h"
#include "log.h"
#include "obex_utils.h"

namespace OHOS {
namespace bluetooth {
ObexServerTransport::ObexServerTransport(ObexTransportObserver &observer) : observer_(observer)
{}

void ObexTransportObserver::OnTransportConnectIncoming(ObexIncomingConnect &incomingConnect)
{
    OBEX_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
}

void ObexTransportObserver::OnTransportIncomingDisconnected(const std::string &btAddr)
{
    OBEX_LOG_INFO("Call %{public}s, %{public}s", __PRETTY_FUNCTION__, btAddr.c_str());
}
}  // namespace bluetooth
}  // namespace OHOS