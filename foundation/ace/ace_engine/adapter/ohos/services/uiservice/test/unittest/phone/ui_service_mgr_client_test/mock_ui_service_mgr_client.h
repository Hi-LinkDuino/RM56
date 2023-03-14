
/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef MOCK_FOUNDATION_AAFWK_MOCK_DATAOBS_MGR_CLIENT_H
#define MOCK_FOUNDATION_AAFWK_MOCK_DATAOBS_MGR_CLIENT_H

#include <gmock/gmock.h>
#include <map>
#include <memory>
#define protected public
#define private public
#include "mock_ui_mgr_service.h"
#include "semaphore_ex.h"
#include "system_ability.h"
#include "ui_mgr_service.h"
#include "ui_service_mgr_client.h"

namespace OHOS {
namespace Ace {
class MockUIServiceMgrClient : public UIServiceMgrClient {
public:
    static std::shared_ptr<UIServiceMgrClient> GetInstance();
    ErrCode Connect();
};

std::shared_ptr<UIServiceMgrClient> MockUIServiceMgrClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::shared_ptr<UIServiceMgrClient> client = std::make_shared<MockUIServiceMgrClient>();
        if (client != nullptr) {
            ((MockUIServiceMgrClient *)client.get())->Connect();
        }
        instance_ = client;
    }
    return instance_;
}

ErrCode MockUIServiceMgrClient::Connect()
{
    if (remoteObject_ == nullptr) {
        sptr<IRemoteObject> mockUIServiceManager = (new (std::nothrow) MockUIMgrService());
        if (mockUIServiceManager != nullptr) {
            ((MockUIMgrService *)mockUIServiceManager.GetRefPtr())->OnStart();
        }

        remoteObject_ = mockUIServiceManager;
    }
    return ERR_OK;
}
}  // namespace Ace
}  // namespace OHOS
#endif  // MOCK_FOUNDATION_AAFWK_MOCK_DATAOBS_MGR_CLIENT_H