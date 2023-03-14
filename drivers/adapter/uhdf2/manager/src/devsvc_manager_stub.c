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

#include "devsvc_manager_stub.h"
#include "device_token_proxy.h"
#include "devmgr_service_stub.h"
#include "devsvc_listener_holder.h"
#include "devsvc_manager_proxy.h"
#include "hdf_cstring.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "hdf_slist.h"
#include "osal_mem.h"

#define HDF_LOG_TAG devsvc_manager_stub

static struct HdfDeviceObject *ObtainServiceObject(
    struct DevSvcManagerStub *stub, const char *name, struct HdfRemoteService *service)
{
    struct HdfDeviceObject *serviceObject = OsalMemCalloc(sizeof(struct HdfDeviceObject));
    if (serviceObject == NULL) {
        return NULL;
    }
    serviceObject->priv = (void *)HdfStringCopy(name);
    if (serviceObject->priv == NULL) {
        OsalMemFree(serviceObject);
        return NULL;
    }
    serviceObject->service = (struct IDeviceIoService *)service;
    service->target = (struct HdfObject *)serviceObject;

    HdfRemoteServiceAddDeathRecipient(service, &stub->recipient);
    return serviceObject;
}

static void ReleaseServiceObject(struct DevSvcManagerStub *stub, struct HdfDeviceObject *serviceObject)
{
    if (serviceObject == NULL) {
        return;
    }
    if (serviceObject->priv == NULL) {
        HDF_LOGW("release service object has empty name, may broken object");
        return;
    }
    struct HdfRemoteService *serviceRemote = (struct HdfRemoteService *)serviceObject->service;
    HdfRemoteServiceRemoveDeathRecipient(serviceRemote, &stub->recipient);
    HdfRemoteServiceRecycle((struct HdfRemoteService *)serviceObject->service);
    OsalMemFree(serviceObject->priv);
    serviceObject->priv = NULL;
    serviceObject->service = NULL;
    OsalMemFree(serviceObject);
}

static int32_t DevSvcManagerStubAddService(struct IDevSvcManager *super, struct HdfSBuf *data)
{
    int ret = HDF_FAILURE;
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%{public}s failed, name is null", __func__);
        return ret;
    }
    uint16_t devClass = DEVICE_CLASS_DEFAULT;
    if (!HdfSbufReadUint16(data, &devClass)) {
        HDF_LOGE("%{public}s failed, devClass invalid", __func__);
        return ret;
    }

    struct HdfRemoteService *service = HdfSbufReadRemoteService(data);
    if (service == NULL) {
        HDF_LOGE("%{public}s failed, service is null", __func__);
        return ret;
    }
    const char *servInfo = HdfSbufReadString(data);
    struct HdfDeviceObject *serviceObject = ObtainServiceObject(stub, name, service);
    if (serviceObject == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    struct HdfDeviceObject *oldServiceObject = super->GetObject(super, name);
    ret = super->AddService(super, name, devClass, serviceObject, servInfo);
    if (ret != HDF_SUCCESS) {
        ReleaseServiceObject(stub, serviceObject);
    } else {
        ReleaseServiceObject(stub, oldServiceObject);
    }
    HDF_LOGI("add service %{public}s, %{public}d", name, ret);
    return ret;
}

static int32_t DevSvcManagerStubUpdateService(struct IDevSvcManager *super, struct HdfSBuf *data)
{
    int ret = HDF_FAILURE;
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%{public}s failed, name is null", __func__);
        return ret;
    }
    uint16_t devClass = DEVICE_CLASS_DEFAULT;
    if (!HdfSbufReadUint16(data, &devClass)) {
        HDF_LOGE("%{public}s failed, devClass invalid", __func__);
        return ret;
    }

    struct HdfRemoteService *service = HdfSbufReadRemoteService(data);
    if (service == NULL) {
        HDF_LOGE("%{public}s failed, remote service is null", __func__);
        return ret;
    }
    const char *servInfo = HdfSbufReadString(data);

    struct HdfDeviceObject *oldServiceObject = super->GetObject(super, name);
    if (oldServiceObject == NULL) {
        HDF_LOGE("update service %{public}s not exist", name);
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    struct HdfDeviceObject *serviceObject = ObtainServiceObject(stub, name, service);
    if (serviceObject == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = super->UpdateService(super, name, devClass, serviceObject, servInfo);
    if (ret != HDF_SUCCESS) {
        ReleaseServiceObject(stub, serviceObject);
    } else {
        ReleaseServiceObject(stub, oldServiceObject);
    }
    HDF_LOGI("update service %{public}s, %{public}d", name, ret);
    return ret;
}

static int32_t DevSvcManagerStubGetService(struct IDevSvcManager *super, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int ret = HDF_FAILURE;
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%{public}s failed, name is null", __func__);
        return ret;
    }
    struct HdfRemoteService *remoteService = (struct HdfRemoteService *)super->GetService(super, name);
    if (remoteService != NULL) {
        ret = HDF_SUCCESS;
        HdfSbufWriteRemoteService(reply, remoteService);
        HDF_LOGI("service %{public}s found", name);
    } else {
        HDF_LOGE("service %{public}s not found", name);
    }

    return ret;
}

static int32_t DevSvcManagerStubRemoveService(struct IDevSvcManager *super, struct HdfSBuf *data)
{
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    const char *name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%{public}s failed, name is null", __func__);
        return HDF_FAILURE;
    }
    struct HdfDeviceObject *serviceObject = super->GetObject(super, name);
    if (serviceObject == NULL) {
        HDF_LOGE("remove service %{public}s not exist", name);
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    const char *servName = (const char *)serviceObject->priv;
    if (servName == NULL) {
        HDF_LOGE("remove service %{public}s is broken object", name);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (strcmp(name, servName) != 0) {
        HDF_LOGE("remove service %{public}s name mismatch with %{public}s", name, servName);
        return HDF_ERR_INVALID_OBJECT;
    }

    super->RemoveService(super, name);
    HDF_LOGI("service %{public}s removed", name);

    ReleaseServiceObject(stub, serviceObject);
    return HDF_SUCCESS;
}
static int32_t DevSvcManagerStubRegisterServListener(struct IDevSvcManager *super, struct HdfSBuf *data)
{
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    uint16_t listenClass = DEVICE_CLASS_DEFAULT;
    if (!HdfSbufReadUint16(data, &listenClass)) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfRemoteService *listenerRemote = HdfSbufReadRemoteService(data);
    if (listenerRemote == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct ServStatListenerHolder *listenerHolder =
        ServStatListenerHolderCreate((uintptr_t)listenerRemote, listenClass);
    if (listenerHolder == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    int ret = super->RegsterServListener(super, listenerHolder);
    if (ret != HDF_SUCCESS) {
        ServStatListenerHolderRelease(listenerHolder);
    } else {
        HDF_LOGI("register servstat listener success");
    }

    return HDF_SUCCESS;
}

static int32_t DevSvcManagerStubUnregisterServListener(struct IDevSvcManager *super, struct HdfSBuf *data)
{
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)super;
    if (!HdfRemoteServiceCheckInterfaceToken(stub->remote, data)) {
        HDF_LOGE("%{public}s: invalid interface token", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfRemoteService *listenerRemote = HdfSbufReadRemoteService(data);
    if (listenerRemote == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct ServStatListenerHolder *listenerHolder = ServStatListenerHolderGet(listenerRemote->index);
    if (listenerHolder == NULL) {
        HDF_LOGE("failed to unregister svcstat listener, unknown listener");
        HdfRemoteServiceRecycle(listenerRemote);
        return HDF_ERR_INVALID_OBJECT;
    }
    super->UnregsterServListener(super, listenerHolder);
    ServStatListenerHolderRelease(listenerHolder);
    HdfRemoteServiceRecycle(listenerRemote);
    HDF_LOGI("unregister servstat listener success");
    return HDF_SUCCESS;
}

int DevSvcManagerStubDispatch(struct HdfRemoteService *service, int code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int ret = HDF_FAILURE;
    struct DevSvcManagerStub *stub = (struct DevSvcManagerStub *)service;
    if (stub == NULL) {
        HDF_LOGE("DevSvcManagerStubDispatch failed, object is null, code is %{public}d", code);
        return ret;
    }
    struct IDevSvcManager *super = (struct IDevSvcManager *)&stub->super;
    HDF_LOGD("DevSvcManagerStubDispatch called: code=%{public}d", code);
    switch (code) {
        case DEVSVC_MANAGER_ADD_SERVICE:
            ret = DevSvcManagerStubAddService(super, data);
            break;
        case DEVSVC_MANAGER_UPDATE_SERVICE:
            ret = DevSvcManagerStubUpdateService(super, data);
            break;
        case DEVSVC_MANAGER_GET_SERVICE:
            ret = DevSvcManagerStubGetService(super, data, reply);
            break;
        case DEVSVC_MANAGER_REMOVE_SERVICE:
            ret = DevSvcManagerStubRemoveService(super, data);
            break;
        case DEVSVC_MANAGER_REGISER_SVCLISTENER:
            ret = DevSvcManagerStubRegisterServListener(super, data);
            break;
        case DEVSVC_MANAGER_UNREGISER_SVCLISTENER:
            ret = DevSvcManagerStubUnregisterServListener(super, data);
            break;
        default:
            HDF_LOGE("Unknown code : %{public}d", code);
            ret = HDF_FAILURE;
    }
    return ret;
}

void DevSvcManagerOnServiceDied(struct HdfDeathRecipient *recipient, struct HdfRemoteService *remote)
{
    struct DevSvcManagerStub *stub =
        HDF_SLIST_CONTAINER_OF(struct HdfDeathRecipient, recipient, struct DevSvcManagerStub, recipient);
    if (stub == NULL) {
        return;
    }

    struct IDevSvcManager *iSvcMgr = &stub->super.super;
    struct HdfDeviceObject *serviceObject = (struct HdfDeviceObject *)remote->target;

    if (serviceObject == NULL || serviceObject->priv == NULL) {
        HDF_LOGI("%{public}s:invalid service object", __func__);
        return;
    }

    if (iSvcMgr->GetService == NULL || iSvcMgr->RemoveService == NULL) {
        HDF_LOGI("%{public}s:invalid svcmgr object", __func__);
        return;
    }

    char *serviceName = (char *)serviceObject->priv;
    struct HdfObject *service = iSvcMgr->GetService(iSvcMgr, serviceName);
    HDF_LOGI("service %{public}s died", serviceName);

    if ((uintptr_t)service == (uintptr_t)remote) {
        HDF_LOGI("%{public}s: remove died service %{public}s", __func__, serviceName);
        iSvcMgr->RemoveService(iSvcMgr, serviceName);
    }

    ReleaseServiceObject(stub, serviceObject);
}

int DevSvcManagerStubStart(struct IDevSvcManager *svcmgr)
{
    struct DevSvcManagerStub *inst = (struct DevSvcManagerStub *)svcmgr;
    if (inst == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (inst->started) {
        return HDF_SUCCESS;
    }

    ServStatListenerHolderinit();

    static struct HdfRemoteDispatcher dispatcher = {.Dispatch = DevSvcManagerStubDispatch};
    inst->remote = HdfRemoteServiceObtain((struct HdfObject *)inst, &dispatcher);
    if (inst->remote == NULL) {
        HDF_LOGE("failed to obtain device service manager remote service");
        return HDF_ERR_MALLOC_FAIL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(inst->remote, "HDI.IServiceManager.V1_0")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    inst->recipient.OnRemoteDied = DevSvcManagerOnServiceDied;
    int ret = HdfRemoteServiceRegister(DEVICE_SERVICE_MANAGER_SA_ID, inst->remote);
    if (ret != 0) {
        HDF_LOGE("failed to publish device service manager, %{public}d", ret);
        HdfRemoteServiceRecycle(inst->remote);
        inst->remote = NULL;
    } else {
        HDF_LOGI("publish device service manager success");
        inst->started = true;
    }

    return ret;
}

static bool DevSvcManagerStubConstruct(struct DevSvcManagerStub *inst)
{
    if (inst == NULL) {
        return false;
    }
    if (!DevSvcManagerConstruct(&inst->super)) {
        HDF_LOGE("failed to construct device service manager");
        return false;
    }
    inst->super.super.StartService = DevSvcManagerStubStart;

    return true;
}

struct HdfObject *DevSvcManagerStubCreate(void)
{
    static struct DevSvcManagerStub *instance;
    if (instance != NULL) {
        return (struct HdfObject *)instance;
    }

    instance = OsalMemCalloc(sizeof(struct DevSvcManagerStub));
    if (!DevSvcManagerStubConstruct(instance)) {
        OsalMemFree(instance);
        instance = NULL;
    }

    return (struct HdfObject *)instance;
}

void DevSvcManagerStubRelease(struct HdfObject *object)
{
    struct DevmgrServiceStub *instance = (struct DevmgrServiceStub *)object;
    if (instance != NULL) {
        if (instance->remote != NULL) {
            HdfRemoteServiceRecycle(instance->remote);
            instance->remote = NULL;
        }
    }
}
