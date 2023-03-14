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

#include <fcntl.h>
#include <limits.h>
#include <securec.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "devhost_service_proxy.h"
#include "device_token_proxy.h"
#include "devmgr_query_device.h"
#include "devsvc_manager.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"
#include "osal_sysevent.h"

#include "devmgr_service_stub.h"

#define HDF_LOG_TAG devmgr_service_stub

static int32_t DevmgrServiceStubDispatchAttachDevice(struct IDevmgrService *devmgrSvc, struct HdfSBuf *data)
{
    uint32_t deviceId;
    if (!HdfSbufReadUint32(data, &deviceId)) {
        HDF_LOGE("%{public}s:failed to get host id and device id", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfDevTokenProxy *tokenClnt = HdfDevTokenProxyObtain(NULL);
    if (tokenClnt == NULL) {
        return HDF_FAILURE;
    }
    tokenClnt->super.devid = deviceId;
    return devmgrSvc->AttachDevice(devmgrSvc, &tokenClnt->super);
}

static int32_t DevmgrServiceStubDispatchDetachDevice(struct IDevmgrService *devmgrSvc, struct HdfSBuf *data)
{
    uint32_t deviceId;
    if (!HdfSbufReadUint32(data, &deviceId)) {
        HDF_LOGE("%{public}s:failed to get host id and device id", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return devmgrSvc->DetachDevice(devmgrSvc, deviceId);
}

static int32_t DevmgrServiceStubDispatchLoadDevice(struct IDevmgrService *devmgrSvc, struct HdfSBuf *data)
{
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        HDF_LOGE("%{public}s:service name is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("%{public}s:load service %{public}s", __func__, serviceName);
    return devmgrSvc->LoadDevice(devmgrSvc, serviceName);
}

static int32_t DevmgrServiceStubDispatchUnloadDevice(struct IDevmgrService *devmgrSvc, struct HdfSBuf *data)
{
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        HDF_LOGE("%{public}s:service name is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("%{public}s:unload service %{public}s", __func__, serviceName);
    return devmgrSvc->UnloadDevice(devmgrSvc, serviceName);
}

int32_t DevmgrServiceStubDispatch(struct HdfRemoteService *stub, int code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_FAILURE;
    struct DevmgrServiceStub *serviceStub = (struct DevmgrServiceStub *)stub;
    if (serviceStub == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct IDevmgrService *super = (struct IDevmgrService *)&serviceStub->super;
    if (!HdfRemoteServiceCheckInterfaceToken(serviceStub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token, code=%{public}d", __func__, code);
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t hostId = 0;
    switch (code) {
        case DEVMGR_SERVICE_ATTACH_DEVICE_HOST:
            if (!HdfSbufReadUint32(data, &hostId)) {
                HDF_LOGE("invalid host id");
                return HDF_FAILURE;
            }
            struct HdfRemoteService *service = HdfSbufReadRemoteService(data);
            struct IDevHostService *hostIf = DevHostServiceProxyObtain(hostId, service);
            ret = super->AttachDeviceHost(super, hostId, hostIf);
            break;
        case DEVMGR_SERVICE_ATTACH_DEVICE:
            ret = DevmgrServiceStubDispatchAttachDevice(super, data);
            break;
        case DEVMGR_SERVICE_DETACH_DEVICE:
            ret = DevmgrServiceStubDispatchDetachDevice(super, data);
            break;
        case DEVMGR_SERVICE_LOAD_DEVICE:
            ret = DevmgrServiceStubDispatchLoadDevice(super, data);
            break;
        case DEVMGR_SERVICE_UNLOAD_DEVICE:
            ret = DevmgrServiceStubDispatchUnloadDevice(super, data);
            break;
        case DEVMGR_SERVICE_QUERY_DEVICE:
            ret = DevFillQueryDeviceInfo(super, data, reply);
            break;
        default:
            break;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s devmgr service stub dispach failed, cmd id is %{public}d, ret = %{public}d", __func__,
            code, ret);
        HdfSbufWriteInt32(reply, ret);
    }

    return ret;
}

static void RemoveModule(const char *module)
{
    uint32_t flags = O_NONBLOCK | O_EXCL;
    if (syscall(__NR_delete_module, module, flags) != 0) {
        HDF_LOGE("failed to remove module %{public}s", module);
    }
}

static int32_t InstallModule(const char *module)
{
    HDF_LOGI("try to install module %{public}s", module);

    int fd = open(module, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        HDF_LOGE("module %{public}s is invalid", module);
        return HDF_ERR_BAD_FD;
    }
    int32_t ret = syscall(SYS_finit_module, fd, "", 0);
    if (ret != 0) {
        HDF_LOGE("failed to install module %{public}s", module);
    }

    close(fd);
    return ret;
}

static int32_t MakeModulePath(char *buffer, const char *moduleName)
{
    char temp[PATH_MAX] = {0};
    if (sprintf_s(temp, PATH_MAX, "%s/%s.ko", HDF_MODULE_DIR, moduleName) <= 0) {
        HDF_LOGI("driver module path sprintf failed: %{public}s", moduleName);
        return HDF_FAILURE;
    }
    HDF_LOGI("driver module file: %{public}s", temp);

    char *path = realpath(temp, buffer);
    if (path == NULL || strncmp(path, HDF_MODULE_DIR, strlen(HDF_MODULE_DIR)) != 0) {
        HDF_LOGE("driver module file is invalid: %{public}s", temp);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t ModuleSysEventHandle(
    struct HdfSysEventNotifyNode *self, uint64_t eventClass, uint32_t event, const char *content)
{
    if (self == NULL || (eventClass & HDF_SYSEVENT_CLASS_MODULE) == 0 || content == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("handle driver module: %{public}s", content);
    char modulePath[PATH_MAX] = {0};
    int32_t ret = MakeModulePath(modulePath, content);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    switch (event) {
        case KEVENT_MODULE_INSTALL:
            ret = InstallModule(modulePath);
            break;
        case KEVENT_MODULE_REMOVE:
            RemoveModule(modulePath);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

static int32_t DriverModuleLoadHelperInit(void)
{
    static struct HdfSysEventNotifyNode sysEventNotify = {
        .callback = ModuleSysEventHandle,
    };

    int32_t ret = HdfSysEventNotifyRegister(&sysEventNotify, HDF_SYSEVENT_CLASS_MODULE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("ModuleLoadHelper:failed to register module event listener");
    }

    return ret;
}

static struct HdfRemoteDispatcher g_devmgrDispatcher = {
    .Dispatch = DevmgrServiceStubDispatch,
};

int DevmgrServiceStubStartService(struct IDevmgrService *inst)
{
    struct DevmgrServiceStub *fullService = (struct DevmgrServiceStub *)inst;
    int status;
    if (fullService == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct IDevSvcManager *serviceManager = DevSvcManagerGetInstance();
    if (serviceManager == NULL) {
        HDF_LOGI("Start service failed, fullService is null");
        return HDF_ERR_INVALID_OBJECT;
    }

    struct HdfRemoteService *remoteService = HdfRemoteServiceObtain((struct HdfObject *)inst, &g_devmgrDispatcher);
    if (remoteService == NULL) {
        HDF_LOGI("failed to start devmgr, remoteService obtain err");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(remoteService, "HDI.IDeviceManager.V1_0")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remoteService);
        return HDF_FAILURE;
    }
    struct HdfDeviceObject *deviceObject = OsalMemCalloc(sizeof(struct HdfDeviceObject));
    if (deviceObject == NULL) {
        HdfRemoteServiceRecycle(remoteService);
        return HDF_ERR_MALLOC_FAIL;
    }
    deviceObject->service = (struct IDeviceIoService *)remoteService;
    status = DevSvcManagerAddService(serviceManager, DEVICE_MANAGER_SERVICE, DEVICE_CLASS_DEFAULT, deviceObject, NULL);
    if (status != HDF_SUCCESS) {
        HdfRemoteServiceRecycle(remoteService);
        OsalMemFree(deviceObject);
        return status;
    }
    fullService->remote = remoteService;

    (void)DriverModuleLoadHelperInit();
    status = DevmgrServiceStartService((struct IDevmgrService *)&fullService->super);
    if (status != HDF_SUCCESS) {
        HdfRemoteServiceRecycle(remoteService);
        OsalMemFree(deviceObject);
        return status;
    }
    return DevSvcManagerStartService();
}

static void DevmgrServiceStubConstruct(struct DevmgrServiceStub *inst)
{
    struct IDevmgrService *pvtbl = (struct IDevmgrService *)inst;

    DevmgrServiceFullConstruct(&inst->super);
    pvtbl->StartService = DevmgrServiceStubStartService;
    inst->remote = NULL;
    OsalMutexInit(&inst->devmgrStubMutx);
}

struct HdfObject *DevmgrServiceStubCreate(void)
{
    static struct DevmgrServiceStub *instance = NULL;
    if (instance == NULL) {
        instance = (struct DevmgrServiceStub *)OsalMemCalloc(sizeof(struct DevmgrServiceStub));
        if (instance == NULL) {
            HDF_LOGE("Creating devmgr service stub failed, alloc mem error");
            return NULL;
        }
        DevmgrServiceStubConstruct(instance);
    }
    return (struct HdfObject *)instance;
}

void DevmgrServiceStubRelease(struct HdfObject *object)
{
    struct DevmgrServiceStub *instance = (struct DevmgrServiceStub *)object;
    if (instance != NULL) {
        if (instance->remote != NULL) {
            HdfRemoteServiceRecycle(instance->remote);
            instance->remote = NULL;
        }
        OsalMutexDestroy(&instance->devmgrStubMutx);
        OsalMemFree(instance);
    }
}
