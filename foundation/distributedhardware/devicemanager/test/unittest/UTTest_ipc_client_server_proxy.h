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

#ifndef OHOS_IPC_CLIENT_SERVER_PROXY_TEST_H
#define OHOS_IPC_CLIENT_SERVER_PROXY_TEST_H

#include <gtest/gtest.h>
#include <refbase.h>

#include "ipc_remote_broker.h"
#include "ipc_client_server_proxy.h"
#include "iremote_proxy.h"
namespace OHOS {
namespace DistributedHardware {
class IpcClientServerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
} // namespace DistributedHardware
} // namespace OHOS

#endif //  OHOS_IPC_CLIENT_SERVER_PROXY_TEST_H
