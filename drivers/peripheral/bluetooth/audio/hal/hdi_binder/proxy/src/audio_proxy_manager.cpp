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
#include "audio_proxy_manager.h"
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <servmgr_hdi.h>
#include <osal_mem.h>
#include "audio_proxy_common.h"
#include "audio_proxy_internal.h"
#include "audio_adapter_info_common.h"

namespace OHOS::HDI::Audio_Bluetooth {
constexpr const char *HDI_SERVER_NAME = "audio_bluetooth_hdi_service";

int32_t AudioProxyManagerGetAllAdapters(struct AudioProxyManager *manager,
                                        struct AudioAdapterDescriptor **descs,
                                        int *size)
{
    HDF_LOGI("%{public}s, ", __func__);
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    int32_t ret;
    if (manager == NULL || manager->remote == NULL || descs == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    HDF_LOGI("%{public}s, call HdfRemoteServiceWriteInterfaceToken ", __func__);
    if (!HdfRemoteServiceWriteInterfaceToken(manager->remote, data)) {
        HDF_LOGE("%{public}s, call HdfRemoteServiceWriteInterfaceToken error ", __func__);
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioAdaptersForUser(descs, size);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        HDF_LOGE("AudioAdaptersForUser FAIL!");
        return AUDIO_HAL_ERR_NOTREADY; // Failed to read sound card configuration file
    }
    HDF_LOGI("%{public}s, call AudioProxyDispatchCall ", __func__);
    ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_GET_ALL_ADAPTER, data, reply);
    if (ret != AUDIO_HAL_SUCCESS) {
        AudioProxyBufReplyRecycle(data, reply);
        HDF_LOGE("Failed to send service call!");
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyManagerLoadAdapter(struct AudioProxyManager *manager, const struct AudioAdapterDescriptor *desc,
                                     struct AudioAdapter **adapter)
{
    HDF_LOGI("%{public}s, ", __func__);
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (manager == NULL || manager->remote == NULL || desc == NULL ||
        desc->adapterName == NULL || desc->ports == NULL || adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioAdapterExist(desc->adapterName) || (AudioProxyPreprocessSBuf(&data, &reply) < 0)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(manager->remote, data)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteString(data, desc->adapterName)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempDir = (uint32_t)desc->ports->dir;
    if (!HdfSbufWriteUint32(data, tempDir)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)calloc(1, sizeof(struct AudioHwAdapter));
    if (hwAdapter == NULL) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    int32_t ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_LOAD_ADAPTER, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwAdapter);
        return ret;
    }
    hwAdapter->common.InitAllPorts = AudioProxyAdapterInitAllPorts;
    hwAdapter->common.CreateRender = AudioProxyAdapterCreateRender;
    hwAdapter->common.DestroyRender = AudioProxyAdapterDestroyRender;
    hwAdapter->common.GetPortCapability = AudioProxyAdapterGetPortCapability;
    hwAdapter->common.SetPassthroughMode = AudioProxyAdapterSetPassthroughMode;
    hwAdapter->common.GetPassthroughMode = AudioProxyAdapterGetPassthroughMode;
    hwAdapter->adapterDescriptor = *desc;
    hwAdapter->proxyRemoteHandle = manager->remote; // get dispatch Server
    *adapter = &hwAdapter->common;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

void AudioProxyManagerUnloadAdapter(const struct AudioProxyManager *manager, const struct AudioAdapter *adapter)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    const char *adapterName = NULL;
    int32_t i = 0;
    int32_t portNum;
    const struct AudioHwAdapter *hwAdapter = reinterpret_cast<const struct AudioHwAdapter *>(adapter);
    if (manager == NULL || manager->remote == NULL || adapter == NULL) {
        return;
    }
    if (hwAdapter->portCapabilitys != NULL) {
        portNum = hwAdapter->adapterDescriptor.portNum;
        while (i < portNum) {
            if (&hwAdapter->portCapabilitys[i] != NULL) {
                AudioMemFree((void **)&hwAdapter->portCapabilitys[i].capability.subPorts);
            }
            i++;
        }
        AudioMemFree((void **)&hwAdapter->portCapabilitys);
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) == AUDIO_HAL_SUCCESS) {
        adapterName = hwAdapter->adapterDescriptor.adapterName;
        if (!HdfRemoteServiceWriteInterfaceToken(manager->remote, data)) {
            AudioProxyBufReplyRecycle(data, reply);
            return;
        }
        if (HdfSbufWriteString(data, adapterName)) {
            int32_t ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_UNLOAD_ADAPTER, data, reply);
            if (ret < 0) {
                HDF_LOGE("Send Server fail!");
            }
        }
        AudioProxyBufReplyRecycle(data, reply);
    }
    AudioMemFree((void **)&adapter);
    return;
}

static void ProxyMgrConstruct(struct AudioProxyManager *proxyMgr)
{
    proxyMgr->GetAllAdapters = AudioProxyManagerGetAllAdapters;
    proxyMgr->LoadAdapter = AudioProxyManagerLoadAdapter;
    proxyMgr->UnloadAdapter = AudioProxyManagerUnloadAdapter;
}

struct AudioProxyManager *GetAudioProxyManagerFuncs(void)
{
    HDF_LOGI("%{public}s, ", __func__);
    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        HDF_LOGE("HDIServiceManagerGet failed!");
        return NULL;
    }
    struct HdfRemoteService *remote = serviceMgr->GetService(serviceMgr, HDI_SERVER_NAME);
    if (remote == NULL) {
        HDF_LOGE("Remote GetService failed!");
        HDIServiceManagerRelease(serviceMgr);
        return NULL;
    }
    HDIServiceManagerRelease(serviceMgr);
    HDF_LOGE("HdfRemoteServiceSetInterfaceDesc");
    if (!HdfRemoteServiceSetInterfaceDesc(remote, "ohos.hdi.audio_bluetooth_service")) {
        HDF_LOGE("failed to init interface desc");
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    struct AudioProxyManager *proxyDevMgr = (AudioProxyManager *)OsalMemAlloc(sizeof(struct AudioProxyManager));
    if (proxyDevMgr == NULL) {
        HDF_LOGE("malloc failed!");
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }
    ProxyMgrConstruct(proxyDevMgr);
    proxyDevMgr->remote = remote;
    return proxyDevMgr;
}
}