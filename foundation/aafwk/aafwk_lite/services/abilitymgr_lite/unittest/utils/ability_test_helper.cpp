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

#include "ability_test_helper.h"

#include <ability_kit_command.h>
#include <ability_manager.h>
#include <ability_service_interface.h>
#include <appexecfwk_errors.h>
#include <bundle_manager.h>
#include <cstring>
#include <ctime>
#include <ohos_errno.h>
#include <samgr_lite.h>
#include <semaphore.h>
#include <want_utils.h>

namespace OHOS {
    constexpr uint32_t WAIT_TIMEOUT = 30;
    constexpr char ABILITY_STATE[] = "Ability State: [";
    constexpr char NO_ABILITY[] = "Ability not found";
    constexpr char SLICE_STACK[] = "\n   [";
    constexpr char SLICE_STATE[] = "] State: [";

    static sem_t g_sem;
    static bool g_result = false;
    static std::string g_resultString;

    SvcIdentity AbilityTestHelper::identity_ = {};
    IClientProxy *AbilityTestHelper::proxy_ = nullptr;

    void AbilityTestHelper::Initialize()
    {
        if (RegisterIpcCallback(AbilityCallback, 0, IPC_WAIT_FOREVER, &identity_, nullptr) != 0) {
            printf("registerIpcCallback failed\n");
            exit(-1);
        }
        proxy_ = GetAbilityInnerFeature();
        if (proxy_ == nullptr) {
            exit(-1);
        }
        sleep(1);
    }

    void AbilityTestHelper::UnInitialize()
    {
        UnregisterIpcCallback(identity_);
        sleep(1);
    }

    void AbilityTestHelper::InstallCallback(const uint8_t resultCode, const void *resultMessage)
    {
        std::string strMessage = reinterpret_cast<const char *>(resultMessage);
        if (!strMessage.empty()) {
            printf("install resultMessage is %s\n", strMessage.c_str());
        }

        g_result = (resultCode == ERR_OK);
        SemPost();
    }

    void AbilityTestHelper::UninstallCallback(const uint8_t resultCode, const void *resultMessage)
    {
        std::string strMessage = reinterpret_cast<const char *>(resultMessage);
        if (!strMessage.empty()) {
            printf("[INFO] [AbilityTestHelper] uninstall resultMessage is %s\n", strMessage.c_str());
        }
        
        g_result = (resultCode == ERR_OK);
        SemPost();
    }

    int32_t AbilityTestHelper::AbilityCallback(const IpcContext* context, void *ipcMsg, IpcIo *data, void *arg)
    {
        if (ipcMsg == nullptr) {
            printf("ams call back error, ipcMsg is null\n");
            return -1;
        }
        
        uint32_t funcId = 0;
        GetCode(ipcMsg, &funcId);
        uint32_t flag = 0;
        GetFlag(ipcMsg, &flag);
        if (flag == LITEIPC_FLAG_ONEWAY) {
            FreeBuffer(nullptr, ipcMsg);
        }
        switch (funcId)
        {
            case SCHEDULER_APP_INIT: {
                ElementName element = {};
                DeserializeElement(&element, data);
                int ret = IpcIoPopInt32(data);
                printf("ams call back, start %s.%s ret = %d\n", element.bundleName, element.abilityName, ret);
                ClearElement(&element);
                g_result = (ret == EC_SUCCESS);
                break;
            }
            case SCHEDULER_DUMP_ABILITY: {
                BuffPtr *buff = IpcIoPopDataBuff(data);
                if ((buff == nullptr) || (buff->buff == nullptr))
                {
                    printf("ams call back error, buff is empty\n");
                    return false;
                }
                g_resultString = static_cast<char *>(buff->buff);
                FreeBuffer(nullptr, buff->buff);
                break;
            }
            default: {
                printf("ams call back error, funcId: %u\n", funcId);
                break;
            }
        }

        SemPost();
        return 0;
    }

    bool AbilityTestHelper::TestInstall(const std::string &hap)
    {
        InstallParam installParam = {
            .installLocation = 1,
            .keepData = false
        };
        if (!Install(hap.c_str(), &installParam, InstallCallback)) {
            printf("[ERROR] [AbilityTestHelper] Install hap failed!\n");
            exit(-1);
        }
        g_result = true;
        SemWait();
        return g_result;
    }

    bool AbilityTestHelper::TestUnInstall(const std::string &bundleName)
    {
        InstallParam installParam = {
            .installLocation = 1,
            .keepData = false
        };
        bool ret = Uninstall(bundleName.c_str(), &installParam, UninstallCallback);
        SemWait();
        return ret;
    }

    bool AbilityTestHelper::TestStartAbility(const Want &want)
    {
        SetWantSvcIdentity(const_cast<Want *>(&want), identity_);
        int32_t ret = StartAbility(&want);
        g_result = (ERR_OK == ret);
        SemWait();
        sleep(1);
        return g_result;
    }

    bool AbilityTestHelper::TestTerminateApp(const std::string &bundleName)
    {
        IpcIo req;
        char data[IPC_IO_DATA_MAX];
        IpcIoInit(&req, data, IPC_IO_DATA_MAX, 0);
        IpcIoPushString(&req, bundleName.c_str());
        int32_t ret = proxy_->Invoke(proxy_, TERMINATE_APP, &req, nullptr, nullptr);
        sleep(2);
        return ret == EC_SUCCESS;
    }

    State AbilityTestHelper::GetAbilityState(const ElementName &elementName)
    {
        TestDumpAbility(elementName);

        auto position = g_resultString.find(ABILITY_STATE);
        if (position != std::string::npos) {
            return static_cast<State>(g_resultString[position + strlen(ABILITY_STATE)] - '0');
        }

        if (g_resultString.find(NO_ABILITY) != std::string::npos) {
            return STATE_INITIAL;
        }
        printf("[ERROR] [AbilityTestHelper] Failed to GetAbilityState\n");
        return STATE_UNINITIALIZED;
    }

    std::list<std::shared_ptr<SliceRecord>> AbilityTestHelper::GetSliceStack(const ElementName &elementName)
    {
        TestDumpAbility(elementName);
        std::list<std::shared_ptr<SliceRecord>> sliceList;
        std::string::size_type begin;
        std::string::size_type end = 0;

        while (((begin = g_resultString.find(SLICE_STACK, end)) != std::string::npos) &&
            ((end = g_resultString.find(SLICE_STATE, begin)) != std::string::npos)) {
            auto record = std::make_shared<SliceRecord>();
            record->name = g_resultString.substr(begin + strlen(SLICE_STACK), end);
            record->state = static_cast<State>(g_resultString[end + strlen(SLICE_STATE)] - '0');
            sliceList.push_back(record);
        }

        return sliceList;
    }

    IClientProxy *AbilityTestHelper::GetAbilityInnerFeature()
    {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_INNER_FEATURE);
        if (iUnknown == nullptr) {
            printf("ams inner unknown is null\n");
            return nullptr;
        }
        IClientProxy *innerProxy = nullptr;
        (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&innerProxy);
        if (innerProxy == nullptr) {
            printf("ams inner feature is null\n");
            return nullptr;
        }
        return innerProxy;
    }

    void AbilityTestHelper::TestDumpAbility(const ElementName &elementName)
    {
        IpcIo req;
        char data[IPC_IO_DATA_MAX];
        IpcIoInit(&req, data, IPC_IO_DATA_MAX, 2);
        Want want = {};
        SetWantElement(&want, elementName);
        SetWantSvcIdentity(&want, identity_);
        if (!SerializeWant(&req, &want)) {
            printf("SerializeWant failed\n");
            ClearWant(&want);
            exit(-1);
        }
        ClearWant(&want);
        proxy_->Invoke(proxy_, DUMP_ABILITY, &req, nullptr, nullptr);
        SemWait();

        printf("[Dump]\n%s\n", g_resultString.c_str());
    }

    void AbilityTestHelper::SemWait()
    {
        printf("waiting callback\n");
        sem_init(&g_sem, 0, 0);
        struct timespec ts = {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += WAIT_TIMEOUT;
        sem_timedwait(&g_sem, &ts);
    }

    void AbilityTestHelper::SemPost()
    {
        printf("receive callback\n");
        sem_post(&g_sem);
    }
}

