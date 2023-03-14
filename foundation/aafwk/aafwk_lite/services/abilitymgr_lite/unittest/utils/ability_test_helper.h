/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_TEST_HELPER_H
#define OHOS_ABILITY_TEST_HELPER_H

#include <ability_state.h>
#include <iproxy_client.h>
#include <list>
#include <memory>
#include <string>
#include <want.h>
#include "liteipc_adapter.h"
namespace OHOS {
    struct SliceRecord {
        std::string name;
        State state;
    };

    class AbilityTestHelper {
    public:
        AbilityTestHelper() = delete;
        ~AbilityTestHelper() = delete;
        static void Initialize();
        static void UnInitialize();

        static void InstallCallback(const uint8_t resultCode, const void *resultMessage);
        static void UninstallCallback(const uint8_t resultCode, const void *resultMessage);
        static int32_t AbilityCallback(const IpcContext* context, void *ipcMsg, IpcIo *data, void *arg);
        static bool TestInstall(const std::string &hap);
        static bool TestUnInstall(const std::string &bundleName);
        static bool TestStartAbility(const Want &want);
        static bool TestTerminateApp(const std::string &bundleName);
        static State GetAbilityState(const ElementName &elementName);
        static std::list<std::shared_ptr<SliceRecord>> GetSliceStack(const ElementName &elementName);

    private:
        static IClientProxy *GetAbilityInnerFeature();
        static void TestDumpAbility(const ElementName &elementName);
        static void SemWait();
        static void SemPost();

        static SvcIdentity identity_;
        static IClientProxy *proxy_;
    };
}

#endif // OHOS_ABILITY_TEST_HELPER_H