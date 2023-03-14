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
#include <iostream>
#include "dm_adapter_manager.h"
#include "dm_config_manager.h"
#include "UTTest_dm_adapter_manager.h"

namespace OHOS {
namespace DistributedHardware {
void DmAdapterManagerTest::SetUp()
{
}
void DmAdapterManagerTest::TearDown()
{
}
void DmAdapterManagerTest::SetUpTestCase()
{
}
void DmAdapterManagerTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<DmAdapterManager> dmAdapterManager =
    std::make_shared<DmAdapterManager>();

/**
 * @tc.name: DmAdapterManager::GetDecisionAdapter_001
 * @tc.desc: 1 set soName to null
 *           2 call DmAdapterManager::GetDecisionAdapter_001 with soName = nullptr
 * @tc.type: FUNC
 * @tc.require:AR000GHSJK
 */
HWTEST_F(DmAdapterManagerTest, GetDecisionAdapter_001, testing::ext::TestSize.Level0)
{
    std::string soName = "123";
    soName.clear();
    std::shared_ptr<IDecisionAdapter> iDecisionAdapter = dmAdapterManager->GetDecisionAdapter(soName);
    EXPECT_EQ(nullptr, iDecisionAdapter);
}

/**
 * @tc.name: DmAdapterManager::GetProfileAdapter_001
 * @tc.desc: 1 set soName to null
 *           2 callDmAdapterManager::GetProfileAdapter_001 with soName = nullptr
 * @tc.type: FUNC
 * @tc.require:AR000GHSJK
 */
HWTEST_F(DmAdapterManagerTest, GetProfileAdapter_001, testing::ext::TestSize.Level0)
{
    std::string soName = "123";
    soName.clear();
    std::shared_ptr<IProfileAdapter> iProfileAdapter = dmAdapterManager->GetProfileAdapter(soName);
    EXPECT_EQ(nullptr, iProfileAdapter);
}

/**
 * @tc.name: DmAdapterManager::GetCryptoAdapter_001
 * @tc.desc: 1 set soName to null
 *           2 callDmAdapterManager::GetCryptoAdapter_001 with soName = nullptr
 * @tc.type: FUNC
 * @tc.require:AR000GHSJK
 */
HWTEST_F(DmAdapterManagerTest, GetCryptoAdapter_001, testing::ext::TestSize.Level0)
{
    std::string soName = "123";
    soName.clear();
    std::shared_ptr<ICryptoAdapter> iCryptoAdapter = dmAdapterManager->GetCryptoAdapter(soName);
    EXPECT_EQ(nullptr, iCryptoAdapter);
}
}
}
}
