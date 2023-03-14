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

#include "sys_event_service_ohos_test.h"

#include <cstdlib>
#include <semaphore.h>
#include <string>
#include <vector>

#include "event.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "isys_event_callback.h"
#include "isys_event_service.h"
#include "sys_event.h"
#include "sys_event_callback_default.h"
#include "sys_event_callback_ohos_test.h"
#include "sys_event_rule.h"
#include "sys_event_service.h"
#include "sys_event_service_ohos.h"
#include "sys_event_service_proxy.h"
#include "system_ability.h"

using namespace std;

namespace OHOS {
namespace HiviewDFX {
constexpr int SYS_EVENT_SERVICE_ID = 1203;

void SysEventServiceOhosTest::SetUpTestCase() {}

void SysEventServiceOhosTest::TearDownTestCase() {}

void SysEventServiceOhosTest::SetUp() {}

void SysEventServiceOhosTest::TearDown() {}

sptr<ISysEventCallback> callbackDefault = new SysEventCallbackDefault();

sptr<ISysEventCallback> callbackTest = new SysEventCallbackOhosTest();

static SysEventRule GetTestRule(int type, const string &domain, const string &eventName)
{
    SysEventRule rule;
    rule.ruleType = type;
    rule.domain = domain;
    rule.eventName = eventName;
    return rule;
}

static vector<SysEventRule> GetTestRules(int type, const string &domain, const string &eventName)
{
    vector<SysEventRule> rules;
    rules.push_back(GetTestRule(type, domain, eventName));
    return rules;
}

/* *
 * @tc.name: CommonTest001
 * @tc.desc: Check service is null condition.
 * @tc.type: FUNC
 * @tc.require: SR000GGSVB
 */
HWTEST_F(SysEventServiceOhosTest, CommonTest001, testing::ext::TestSize.Level3)
{
    vector<SysEventRule> rules = GetTestRules(1, "", "");
    auto ret = SysEventServiceOhos::GetInstance().AddListener(rules, callbackDefault);
    printf("add listener result is %d.\n", ret);
    ASSERT_TRUE(!ret);

    SysEventServiceOhos::GetInstance().RemoveListener(callbackDefault);
}

/* *
 * @tc.name: AddListenerTest001
 * @tc.desc: Check AddListener Function.
 * @tc.type: FUNC
 * @tc.require: SR000GGS49
 */
HWTEST_F(SysEventServiceOhosTest, AddListenerTest001, testing::ext::TestSize.Level3)
{
    vector<SysEventRule> rules = GetTestRules(1, "", "");
    SysEventService service;
    SysEventServiceOhos::GetSysEventService(&service);
    auto ret = SysEventServiceOhos::GetInstance().AddListener(rules, nullptr);
    ASSERT_TRUE(!ret);
    ret = SysEventServiceOhos::GetInstance().AddListener(rules, callbackDefault);
    ASSERT_TRUE(!ret);
    ret = SysEventServiceOhos::GetInstance().AddListener(rules, callbackTest);
    ASSERT_TRUE(!ret);
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        printf("SystemAbilityManager is nullptr.\n");
        ASSERT_TRUE(false);
    } else {
        sptr<IRemoteObject> stub = sam->CheckSystemAbility(SYS_EVENT_SERVICE_ID);
        if (stub != nullptr) {
            printf("check sys event service success.\n");
            auto proxy = new SysEventServiceProxy(stub);
            auto ret = proxy->AddListener(rules, callbackTest);
            printf("add listener result is %d.\n", ret);
            ASSERT_TRUE(ret);
            if (ret) {
                string cmdStr = R"(hisyseventdemo -d SystemTool -n SystemUI -t 2 -j {\"Test\":\"Test\"})";
                system(cmdStr.c_str());
                sleep(5);
            } else {
                printf("add listener fail.\n");
                ASSERT_TRUE(false);
            }
            ret = proxy->AddListener(rules, callbackTest);
            printf("add listener result is %d.\n", ret);
            ASSERT_TRUE(ret);
        } else {
            printf("check sys event service failed.\n");
            ASSERT_TRUE(false);
        }
    }
}

/**
 * @tc.name: RemoveListenerTest001
 * @tc.desc: Check RemoveListener Function.
 * @tc.type: FUNC
 * @tc.require: SR000GGS49
 */
HWTEST_F(SysEventServiceOhosTest, RemoveListenerTest001, testing::ext::TestSize.Level3)
{
    SysEventServiceOhos::GetInstance().RemoveListener(nullptr);
    SysEventServiceOhos::GetInstance().RemoveListener(callbackDefault);

    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        printf("SystemAbilityManager is nullptr.\n");
        ASSERT_TRUE(false);
    } else {
        sptr<IRemoteObject> stub = sam->CheckSystemAbility(SYS_EVENT_SERVICE_ID);
        if (stub != nullptr) {
            printf("check sys event service success.\n");
            auto proxy = new SysEventServiceProxy(stub);
            sptr<ISysEventCallback> callback = new SysEventCallbackOhosTest();
            auto ret = proxy->RemoveListener(callback);
            ASSERT_TRUE(!ret);
            ret = proxy->RemoveListener(callbackTest);
            ASSERT_TRUE(!ret);
            ret = proxy->RemoveListener(callbackTest);
            ASSERT_TRUE(!ret);
        } else {
            printf("check sys event service failed.\n");
            ASSERT_TRUE(false);
        }
    }
}

/**
 * @tc.name: OnSysEventTest001
 * @tc.desc: Check OnSysEvent Function.
 * @tc.type: FUNC
 * @tc.require: SR000GGS49
 */
HWTEST_F(SysEventServiceOhosTest, OnSysEventTest001, testing::ext::TestSize.Level3)
{
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        printf("SystemAbilityManager is nullptr.\n");
        ASSERT_TRUE(false);
    } else {
        sptr<IRemoteObject> stub = sam->CheckSystemAbility(SYS_EVENT_SERVICE_ID);
        if (stub != nullptr) {
            printf("check sys event service success.\n");
            auto proxy = new SysEventServiceProxy(stub);
            vector<SysEventRule> rules;
            SysEventRule rule0 = GetTestRule(0, "", "");
            SysEventRule rule1 = GetTestRule(1, "Test", "Test");
            SysEventRule rule2 = GetTestRule(2, "Test", "Test");
            SysEventRule rule3 = GetTestRule(3, "", "[0-9]*");
            rules.push_back(rule0);
            rules.push_back(rule1);
            rules.push_back(rule2);
            rules.push_back(rule3);
            auto ret = proxy->AddListener(rules, callbackTest);
            sleep(5);
            if (ret) {
                string cmdStr = R"(hisyseventdemo -d SystemTool -n SystemUI -t 2 -j {\"Test\":\"Test\"})";
                system(cmdStr.c_str());
                sleep(5);
            } else {
                printf("add listener fail.\n");
                ASSERT_TRUE(false);
            }
        } else {
            printf("check sys event service failed.\n");
            ASSERT_TRUE(false);
        }
    }
}

/**
 * @tc.name: StartService001
 * @tc.desc: Check StartService Function.
 * @tc.type: FUNC
 * @tc.require: SR000GGSVA
 */
HWTEST_F(SysEventServiceOhosTest, StartService001, testing::ext::TestSize.Level3)
{
    SysEventService service;
    auto notifyFunc = [] (std::shared_ptr<Event> event) -> void {};
    SysEventServiceOhos::StartService(&service, notifyFunc);
    system("stop hiview");
    system("start hiview");
}

/**
 * @tc.name: SetDebugModeTest
 * @tc.desc: Check SetDebugMode Function.
 * @tc.type: FUNC
 * @tc.require: SR000GGSVA
 */
HWTEST_F(SysEventServiceOhosTest, SetDebugModeTest, testing::ext::TestSize.Level3)
{
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        printf("SystemAbilityManager is nullptr.\n");
        ASSERT_TRUE(false);
    } else {
        sptr<IRemoteObject> stub = sam->CheckSystemAbility(SYS_EVENT_SERVICE_ID);
        if (stub != nullptr) {
            printf("check sys event service success.\n");
            auto proxy = new SysEventServiceProxy(stub);
            bool result = proxy->SetDebugMode(callbackTest, true);
            printf("SetDebugMode result is %d.\n", result);
            ASSERT_TRUE(result == true);
        } else {
            printf("check sys event service failed.\n");
            ASSERT_TRUE(false);
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS