/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DM_PROFILE_CONNECTOR_H
#define OHOS_DM_PROFILE_CONNECTOR_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "distributed_device_profile_client.h"
#include "profile_connector_callback.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class ProfileConnector final : public OHOS::DeviceProfile::IProfileEventCallback,
    public std::enable_shared_from_this<ProfileConnector> {
public:
    ProfileConnector();
    virtual ~ProfileConnector();
    int32_t RegisterProfileCallback(const std::string &pkgName, const std::string &deviceId,
                                    IProfileConnectorCallback* callback);
    int32_t UnRegisterProfileCallback(const std::string &pkgName);
    int32_t SubscribeProfileEvents(const std::list<std::string> &serviceIds, const std::string &deviceId);
    int32_t UnSubscribeProfileEvents();
    void OnSyncCompleted(const OHOS::DeviceProfile::SyncResult &syncResults) override;

private:
    std::mutex callbackMapMutex_;
    std::map<std::string, IProfileConnectorCallback*> callbackMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_PROFILE_CONNECTOR_H
