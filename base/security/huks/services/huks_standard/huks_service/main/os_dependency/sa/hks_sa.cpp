/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_sa.h"

#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "hks_client_service.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_ipc_service.h"

#ifdef SUPPORT_COMMON_EVENT
#include <pthread.h>
#include <unistd.h>

#include "hks_event_observer.h"
#endif

namespace OHOS {
namespace Security {
namespace Hks {
REGISTER_SYSTEM_ABILITY_BY_ID(HksService, SA_ID_KEYSTORE_SERVICE, true);

std::mutex HksService::instanceLock;
sptr<HksService> HksService::instance;
const uint32_t UID_ROOT = 0;
const uint32_t UID_SYSTEM = 1000;
const uint32_t MAX_MALLOC_LEN = 1 * 1024 * 1024; /* max malloc size 1 MB */
#ifdef SUPPORT_COMMON_EVENT
const uint32_t MAX_DELAY_TIMES = 100;
const uint32_t DELAY_INTERVAL = 200000; /* delay 200ms waiting for system event */
#endif

using HksIpcHandlerFuncProc = void (*)(const struct HksBlob *msg, const uint8_t *context);

using HksIpcThreeStageHandlerFuncProc = void (*)(const struct HksBlob *msg, struct HksBlob *outData,
    const uint8_t *context);

enum HksMessage {
    HKS_MSG_BASE = 0x3a400,

    HKS_MSG_GEN_KEY = HKS_MSG_BASE,
    HKS_MSG_IMPORT_KEY,
    HKS_MSG_EXPORT_PUBLIC_KEY,
    HKS_MSG_DELETE_KEY,
    HKS_MSG_GET_KEY_PARAMSET,
    HKS_MSG_KEY_EXIST,
    HKS_MSG_GENERATE_RANDOM,
    HKS_MSG_SIGN,
    HKS_MSG_VERIFY,
    HKS_MSG_ENCRYPT,
    HKS_MSG_DECRYPT,
    HKS_MSG_AGREE_KEY,
    HKS_MSG_DERIVE_KEY,
    HKS_MSG_MAC,
    HKS_MSG_GET_KEY_INFO_LIST,
    HKS_MSG_ATTEST_KEY,
    HKS_MSG_GET_CERTIFICATE_CHAIN,
    HKS_MSG_WRAP_KEY,
    HKS_MSG_UNWRAP_KEY,
    HKS_MSG_PROVISION,
    HKS_MSG_PROVISION_VERIFY,
    HKS_MSG_EXPORT_TRUST_CERTS,
    HKS_MSG_INIT,
    HKS_MSG_UPDATE,
    HKS_MSG_FINISH,
    HKS_MSG_ABORT,

    /* new cmd type must be added before HKS_MSG_MAX */
    HKS_MSG_MAX,
};

struct HksIpcEntryPoint {
    enum HksMessage msgId;
    HksIpcHandlerFuncProc handler;
};

struct HksIpcThreeStagePoint {
    enum HksMessage msgId;
    HksIpcThreeStageHandlerFuncProc handler;
};

static struct HksIpcThreeStagePoint g_hksIpcThreeStageHandler[] = {
    { HKS_MSG_INIT, HksIpcServiceInit },
    { HKS_MSG_UPDATE, HksIpcServiceUpdate },
    { HKS_MSG_FINISH, HksIpcServiceFinish },
    { HKS_MSG_ABORT, HksIpcServiceAbort },
};

static struct HksIpcEntryPoint g_hksIpcMessageHandler[] = {
    { HKS_MSG_GEN_KEY, HksIpcServiceGenerateKey },
    { HKS_MSG_IMPORT_KEY, HksIpcServiceImportKey },
    { HKS_MSG_EXPORT_PUBLIC_KEY, HksIpcServiceExportPublicKey },
    { HKS_MSG_DELETE_KEY, HksIpcServiceDeleteKey },
    { HKS_MSG_GET_KEY_PARAMSET, HksIpcServiceGetKeyParamSet },
    { HKS_MSG_KEY_EXIST, HksIpcServiceKeyExist },
    { HKS_MSG_GENERATE_RANDOM, HksIpcServiceGenerateRandom },
    { HKS_MSG_SIGN, HksIpcServiceSign },
    { HKS_MSG_VERIFY, HksIpcServiceVerify },
    { HKS_MSG_ENCRYPT, HksIpcServiceEncrypt },
    { HKS_MSG_DECRYPT, HksIpcServiceDecrypt },
    { HKS_MSG_AGREE_KEY, HksIpcServiceAgreeKey },
    { HKS_MSG_DERIVE_KEY, HksIpcServiceDeriveKey },
    { HKS_MSG_MAC, HksIpcServiceMac },
    { HKS_MSG_GET_KEY_INFO_LIST, HksIpcServiceGetKeyInfoList },
    { HKS_MSG_ATTEST_KEY, HksIpcServiceAttestKey },
    { HKS_MSG_GET_CERTIFICATE_CHAIN, HksIpcServiceGetCertificateChain },
    { HKS_MSG_WRAP_KEY, HksIpcServiceWrapKey },
    { HKS_MSG_UNWRAP_KEY, HksIpcServiceUnwrapKey },
    { HKS_MSG_PROVISION, HksIpcServiceProvision },
    { HKS_MSG_PROVISION_VERIFY, HksIpcServiceProvisionVerify },
    { HKS_MSG_EXPORT_TRUST_CERTS, HksIpcServiceExportTrustCerts },
};

#ifdef SUPPORT_COMMON_EVENT
static void SubscribEvent()
{
    for (uint32_t i = 0; i < MAX_DELAY_TIMES; ++i) {
        if (SystemEventObserver::SubscribeSystemEvent()) {
            HKS_LOG_I("subscribe system event success, i = %u", i);
            return;
        } else {
            HKS_LOG_E("subscribe system event failed %u times", i);
            usleep(DELAY_INTERVAL);
        }
    }
    HKS_LOG_E("subscribe system event failed");
    return;
}

static void HksSubscribeSystemEvent()
{
    pthread_t subscribeThread;
    if ((pthread_create(&subscribeThread, nullptr, (void *(*)(void *))SubscribEvent, nullptr)) == -1) {
        HKS_LOG_E("create thread failed");
        return;
    }

    HKS_LOG_I("create thread success");
}
#endif

static inline bool IsInvalidLength(uint32_t length)
{
    return (length == 0) || (length > MAX_MALLOC_LEN);
}

static int32_t ProcessMessage(uint32_t code, uint32_t outSize, const struct HksBlob &srcData, MessageParcel &reply)
{
    uint32_t size = sizeof(g_hksIpcMessageHandler) / sizeof(g_hksIpcMessageHandler[0]);
    for (uint32_t i = 0; i < size; ++i) {
        if (code == g_hksIpcMessageHandler[i].msgId) {
            g_hksIpcMessageHandler[i].handler((const struct HksBlob *)&srcData, (const uint8_t *)&reply);
            return NO_ERROR;
        }
    }

    size = sizeof(g_hksIpcThreeStageHandler) / sizeof(g_hksIpcThreeStageHandler[0]);
    for (uint32_t i = 0; i < size; ++i) {
        if (code == g_hksIpcThreeStageHandler[i].msgId) {
            struct HksBlob outData = { 0, nullptr };
            if (outSize != 0) {
                outData.size = outSize;
                if (outData.size > MAX_MALLOC_LEN) {
                    HKS_LOG_E("outData size is invalid, size:%u", outData.size);
                    return HW_SYSTEM_ERROR;
                }
                outData.data = (uint8_t *)HksMalloc(outData.size);
                if (outData.data == nullptr) {
                    HKS_LOG_E("Malloc outData failed.");
                    return HW_SYSTEM_ERROR;
                }
            }
            g_hksIpcThreeStageHandler[i].handler((const struct HksBlob *)&srcData, &outData, (const uint8_t *)&reply);
            HKS_FREE_BLOB(outData);
            break;
        }
    }
    return NO_ERROR;
}

HksService::HksService(int saId, bool runOnCreate = true)
    : SystemAbility(saId, runOnCreate), registerToService_(false), runningState_(STATE_NOT_START)
{
    HKS_LOG_D("HksService");
}

HksService::~HksService()
{
    HKS_LOG_D("~HksService");
}

sptr<HksService> HksService::GetInstance()
{
    std::lock_guard<std::mutex> autoLock(instanceLock);
    if (instance == nullptr) {
        instance = new (std::nothrow) HksService(SA_ID_KEYSTORE_SERVICE, true);
    }

    return instance;
}

bool HksService::Init()
{
    HKS_LOG_I("HksService::Init Ready to init");

    if (!registerToService_) {
        sptr<HksService> ptrInstance = HksService::GetInstance();
        if (ptrInstance == nullptr) {
            HKS_LOG_E("HksService::Init GetInstance Failed");
            return false;
        }
        if (!Publish(ptrInstance)) {
            HKS_LOG_E("HksService::Init Publish Failed");
            return false;
        }
        HKS_LOG_I("HksService::Init Publish service success");
        registerToService_ = true;
    }

    int32_t ret = HksServiceInitialize();
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Init hks service failed!");
        return false;
    }

#ifdef SUPPORT_COMMON_EVENT
    HksSubscribeSystemEvent();
#endif

    HKS_LOG_I("HksService::Init success.");
    return true;
}

bool HksService::CanRequest() const
{
    auto callingUid = IPCSkeleton::GetCallingUid();
    return (callingUid == UID_ROOT) || (callingUid == UID_SYSTEM);
}

int HksService::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    // this is the temporary version which comments the descriptor check
    std::u16string descriptor = HksService::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HKS_LOG_E("descriptor is diff.");
        return HW_SYSTEM_ERROR;
    }

    HKS_LOG_I("OnRemoteRequest code:%d", code);
    uint32_t outSize = (uint32_t)data.ReadUint32();

    struct HksBlob srcData = { 0, nullptr };
    srcData.size = (uint32_t)data.ReadUint32();
    if (IsInvalidLength(srcData.size)) {
        HKS_LOG_E("srcData size is invalid, size:%u", srcData.size);
        return HW_SYSTEM_ERROR;
    }

    srcData.data = (uint8_t *)HksMalloc(srcData.size);
    if (srcData.data == nullptr) {
        HKS_LOG_E("Malloc srcData failed.");
        return HW_SYSTEM_ERROR;
    }

    const uint8_t *pdata = data.ReadBuffer((size_t)srcData.size);
    if (pdata == nullptr) {
        HKS_FREE_BLOB(srcData);
        return HKS_ERROR_BAD_STATE;
    }
    if (memcpy_s(srcData.data, srcData.size, pdata, srcData.size) != EOK) {
        HKS_LOG_E("copy remote data failed!");
        HKS_FREE_BLOB(srcData);
        return HKS_ERROR_BAD_STATE;
    }

    if (ProcessMessage(code, outSize, srcData, reply) != NO_ERROR) {
        HKS_LOG_E("process message!");
        HKS_FREE_BLOB(srcData);
        return HKS_ERROR_BAD_STATE;
    }

    HKS_FREE_BLOB(srcData);
    return NO_ERROR;
}

void HksService::OnStart()
{
    HKS_LOG_I("HksService OnStart");

    if (runningState_ == STATE_RUNNING) {
        HKS_LOG_I("HksService has already Started");
        return;
    }

    if (!Init()) {
        HKS_LOG_E("Failed to init HksService");
        return;
    }

    runningState_ = STATE_RUNNING;
    HKS_LOG_I("HksService start success.");
}

void HksService::OnStop()
{
    HKS_LOG_I("HksService Service OnStop");
    runningState_ = STATE_NOT_START;
    registerToService_ = false;
}
} // namespace Hks
} // namespace Security
} // namespace OHOS
