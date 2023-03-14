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

#ifndef OHOS_DM_HICHAIN_CONNECTOR_CALLBACK_H
#define OHOS_DM_HICHAIN_CONNECTOR_CALLBACK_H

namespace OHOS {
namespace DistributedHardware {
class IHiChainConnectorCallback {
public:
    virtual void OnGroupCreated(int64_t requestId, const std::string &groupId) = 0;
    virtual void OnMemberJoin(int64_t requestId, int32_t status) = 0;
    virtual std::string GetConnectAddr(std::string deviceId) = 0;
    virtual int32_t GetPinCode() = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_HICHAIN_CONNECTOR_CALLBACK_H
