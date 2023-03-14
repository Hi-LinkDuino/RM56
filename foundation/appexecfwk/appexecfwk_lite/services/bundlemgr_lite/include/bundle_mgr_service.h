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

#ifndef OHOS_BUNDLEMGRSERVICE_SERVICE_H
#define OHOS_BUNDLEMGRSERVICE_SERVICE_H

#include "service.h"

#include "nocopyable.h"

namespace OHOS {
const unsigned int BMS_INSTALL_MSG = 100;
const unsigned int BMS_UNINSTALL_MSG = 101;
const unsigned int BMS_SCAN_PACKAGE_MSG = 102;
const unsigned int BMS_REGISTER_CALLBACK_MSG = 103;

class BundleMgrService : public Service {
public:
    static BundleMgrService *GetInstance()
    {
        static BundleMgrService instance;
        return &instance;
    }
    ~BundleMgrService() = default;
    Identity *GetIdentity();

private:
    BundleMgrService();
    static const char *GetServiceName(Service *service);
    static BOOL ServiceInitialize(Service *service, Identity identity);
    static TaskConfig GetServiceTaskConfig(Service *service);
    static BOOL ServiceMessageHandle(Service *service, Request *request);

private:
    Identity identity_;
    DISALLOW_COPY_AND_MOVE(BundleMgrService);
};
} // namespace OHOS
#endif // OHOS_BUNDLEMGRSERVICE_SERVICE_H
