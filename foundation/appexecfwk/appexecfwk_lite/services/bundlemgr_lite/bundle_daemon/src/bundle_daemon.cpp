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

#include "bundle_daemon.h"

#include <string>

#include "bundle_daemon_log.h"
#include "liteipc_adapter.h"
#include "ohos_errno.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "serializer.h"

namespace OHOS {
namespace {
constexpr int STACK_SIZE = 0x800;
constexpr int QUEUE_SIZE = 20;
constexpr pid_t BMS_UID = 7;
}
constexpr InvokeFunc BundleDaemon::invokeFuncs[];

static BundleDaemonFeature g_defaultApi = {
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = BundleDaemon::Invoke,
    IPROXY_END,
};

static void Init()
{
    SamgrLite *samgrLite = SAMGR_GetInstance();
    if (samgrLite == nullptr) {
        PRINTE("BundleDaemon", "get samgr is nullptr");
        return;
    }
    BOOL result = samgrLite->RegisterService(&BundleDaemon::GetInstance());
    if (!result) {
        PRINTE("BundleDaemon", "register bundle_daemon service fail");
        return;
    }
    result = samgrLite->RegisterDefaultFeatureApi(BDS_SERVICE, GET_IUNKNOWN(g_defaultApi));
    PRINTE("BundleDaemon", "register default feature api %{public}s", result ? "success" : "fail");
}
SYSEX_SERVICE_INIT(Init);

BundleDaemon::BundleDaemon()
{
    this->Service::GetName = BundleDaemon::GetServiceName;
    this->Service::Initialize = BundleDaemon::ServiceInitialize;
    this->Service::MessageHandle = BundleDaemon::ServiceMessageHandle;
    this->Service::GetTaskConfig = BundleDaemon::GetServiceTaskConfig;
}


BundleDaemon::~BundleDaemon()
{
    delete bundleMsClient_;
}

const char *BundleDaemon::GetServiceName(Service *service)
{
    (void)service;
    return BDS_SERVICE;
}

BOOL BundleDaemon::ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    BundleDaemon *bundleDaemon = static_cast<BundleDaemon *>(service);
    bundleDaemon->identity_ = identity;
    return TRUE;
}

TaskConfig BundleDaemon::GetServiceTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}

BOOL BundleDaemon::ServiceMessageHandle(Service *service, Request *request)
{
    if (request == nullptr) {
        return FALSE;
    }
    return TRUE;
}

int32_t BundleDaemon::Invoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    PRINTI("BundleDaemon", "bundle_daemon invoke start %{public}d", funcId);
    if (origin == nullptr || req == nullptr) {
        PRINTE("BundleDaemon", "invalid param");
        return EC_INVALID;
    }
    // check permission
    pid_t uid = GetCallingUid(origin);
    if (uid != BMS_UID) {
        PRINTE("BundleDaemon", "permission denied");
        return EC_PERMISSION;
    }
    if (funcId == REGISTER_CALLBACK) {
#ifdef __LINUX__
        return EC_SUCCESS;
#else
        return RegisterCallbackInvoke(req);
#endif
    }
#ifndef __LINUX__
    if (BundleDaemon::GetInstance().bundleMsClient_ == nullptr) {
        PRINTE("BundleDaemon", "bundleMsClient is nullptr");
        return EC_NOINIT;
    }
#endif
    int32_t ret = EC_COMMU;
    if (funcId >= EXTRACT_HAP && funcId < BDS_CMD_END) {
        ret = (BundleDaemon::invokeFuncs[funcId])(req);
    }
#ifdef __LINUX__
    IpcIoPushInt32(reply, ret);
    return ret;
#else
    return BundleDaemon::GetInstance().bundleMsClient_->SendReply(ret);
#endif
}

int32_t BundleDaemon::RegisterCallbackInvoke(IpcIo *req)
{
    SvcIdentity *svcIdentity = IpcIoPopSvc(req);
    if (svcIdentity == nullptr) {
        return EC_INVALID;
    }
    BundleDaemon::GetInstance().bundleMsClient_ = new BundleMsClient(*svcIdentity);
    return BundleDaemon::GetInstance().bundleMsClient_->SendReply(EC_SUCCESS);
}

static int32_t ObtainStringFromIpc(IpcIo *req, std::string &firstStr, std::string &secondStr)
{
#ifdef __LINUX__
    size_t length = 0;
    std::string innerStr = reinterpret_cast<char *>(IpcIoPopString(req, &length));
#else
    BuffPtr *buff = IpcIoPopDataBuff(req);
    if (buff == nullptr || buff->buffSz == 0) {
        return EC_INVALID;
    }
    std::string innerStr = reinterpret_cast<char *>(buff->buff);
#endif
    uint32_t len = IpcIoPopUint16(req);
    if (innerStr.length() <= len) {
#ifndef __LINUX__
        FreeBuffer(nullptr, buff->buff);
#endif
        return EC_INVALID;
    }
    firstStr = innerStr.substr(0, len);
    secondStr = innerStr.substr(len);
#ifndef __LINUX__
    FreeBuffer(nullptr, buff->buff);
#endif
    return EC_SUCCESS;
}

int32_t BundleDaemon::ExtractHapInvoke(IpcIo *req)
{
    std::string hapPath = "";
    std::string codePath = "";
    int32_t ret = ObtainStringFromIpc(req, hapPath, codePath);
    if (ret != EC_SUCCESS) {
        return ret;
    }
    return BundleDaemon::GetInstance().handler_.ExtractHap(hapPath.c_str(), codePath.c_str());
}

int32_t BundleDaemon::RenameFileInvoke(IpcIo *req)
{
    std::string oldFile = "";
    std::string newFile = "";
    int32_t ret = ObtainStringFromIpc(req, oldFile, newFile);
    if (ret != EC_SUCCESS) {
        return ret;
    }
    return BundleDaemon::GetInstance().handler_.RenameFile(oldFile.c_str(), newFile.c_str());
}

int32_t BundleDaemon::CreatePermissionInvoke(IpcIo *req)
{
    return BundleDaemon::GetInstance().handler_.CreatePermissionDir();
}

int32_t BundleDaemon::CreateDataDirectoryInvoke(IpcIo *req)
{
    size_t len = 0;
    const char *dataPath = reinterpret_cast<char *>(IpcIoPopString(req, &len));
    if (dataPath == nullptr || len == 0) {
        return EC_INVALID;
    }
    int32_t uid = IpcIoPopInt32(req);
    int32_t gid = IpcIoPopInt32(req);
    bool isChown = IpcIoPopBool(req);

    return BundleDaemon::GetInstance().handler_.CreateDataDirectory(dataPath, uid, gid, isChown);
}

int32_t BundleDaemon::StoreContentToFileInvoke(IpcIo *req)
{
    size_t len = 0;
    const char *path = reinterpret_cast<char *>(IpcIoPopString(req, &len));
    if (path == nullptr || len == 0) {
        return EC_INVALID;
    }
#ifdef __LINUX__
    size_t buffLen = 0;
    const char *buff = reinterpret_cast<char *>(IpcIoPopString(req, &buffLen));
    if (buff == nullptr || buffLen == 0) {
        return EC_INVALID;
    }
    int32_t ret = BundleDaemon::GetInstance().handler_.StoreContentToFile(path, buff, buffLen);
#else
    BuffPtr *buffPtr = IpcIoPopDataBuff(req);
    if (buffPtr == nullptr || buffPtr->buffSz == 0) {
        return EC_INVALID;
    }
    char *buff = reinterpret_cast<char *>(buffPtr->buff);
    if (buff == nullptr) {
        return EC_INVALID;
    }

    int32_t ret = BundleDaemon::GetInstance().handler_.StoreContentToFile(path, buff, buffPtr->buffSz);
    FreeBuffer(nullptr, buffPtr->buff);
#endif
    return ret;
}

int32_t BundleDaemon::MoveFileInvoke(IpcIo *req)
{
    size_t len = 0;
    const char *oldFile = reinterpret_cast<char *>(IpcIoPopString(req, &len));
    if (oldFile == nullptr || len == 0) {
        return EC_INVALID;
    }
    const char *newFile = reinterpret_cast<char *>(IpcIoPopString(req, &len));
    if (newFile == nullptr || len == 0) {
        return EC_INVALID;
    }
    return BundleDaemon::GetInstance().handler_.MoveFile(oldFile, newFile);
}

int32_t BundleDaemon::RemoveFileInvoke(IpcIo *req)
{
    size_t len = 0;
    const char *path = reinterpret_cast<char *>(IpcIoPopString(req, &len));
    if (path == nullptr || len == 0) {
        return EC_INVALID;
    }
    return BundleDaemon::GetInstance().handler_.RemoveFile(path);
}

int32_t BundleDaemon::RemoveInstallDirectoryInvoke(IpcIo *req)
{
    std::string codePath = "";
    std::string dataPath = "";
    int32_t ret = ObtainStringFromIpc(req, codePath, dataPath);
    if (ret != EC_SUCCESS) {
        return ret;
    }
    bool keepData = IpcIoPopBool(req);
    return BundleDaemon::GetInstance().handler_.RemoveInstallDirectory(codePath.c_str(), dataPath.c_str(), keepData);
}
} // OHOS
