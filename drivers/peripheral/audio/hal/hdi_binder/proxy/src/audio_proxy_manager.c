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
#include <servmgr_hdi.h>
#include "audio_hal_log.h"
#include "audio_proxy_common.h"
#include "audio_proxy_internal.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_PROXY

#define HDI_SERVER_NAME  "audio_hdi_service"
#define CONFIG_FRAME_SIZE      (1024 * 2 * 1)
#define FRAME_SIZE              1024
#define CONFIG_FRAME_COUNT     ((8000 * 2 * 1 + (CONFIG_FRAME_SIZE - 1)) / CONFIG_FRAME_SIZE)

static struct AudioAdapterDescriptor *g_localAudioProxyAdapterAddrOut = NULL; // add for Fuzz
int g_localAudioProxyAdapterNum = 0; // add for Fuzz
static struct AudioProxyManager g_localAudioProxyMgr = {0}; // serverManager

int32_t AudioProxyManagerGetAllAdapters(struct AudioProxyManager *manager,
                                        struct AudioAdapterDescriptor **descs,
                                        int *size)
{
    LOG_FUN_INFO();
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    int32_t ret;
    if ((&g_localAudioProxyMgr) != manager || manager == NULL ||
        manager->remote == NULL || descs == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(manager->remote, data)) {
        LOG_FUN_ERR("write interface token failed");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioAdaptersForUser(descs, size);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        LOG_FUN_ERR("AudioAdaptersForUser FAIL!");
        return AUDIO_HAL_ERR_NOTREADY; // Failed to read sound card configuration file
    }
    /* add for Fuzz. */
    if (*descs && size && (*size) > 0) {
        g_localAudioProxyAdapterAddrOut  = *descs;
        g_localAudioProxyAdapterNum = *size;
    } else {
        LOG_FUN_ERR("Get AudioAdapterDescriptor Failed");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INVALID_OBJECT;
    }
    ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_GET_ALL_ADAPTER, data, reply);
    if (ret != AUDIO_HAL_SUCCESS) {
        AudioProxyBufReplyRecycle(data, reply);
        LOG_FUN_ERR("Failed to send service call!");
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t LoadAdapterPrepareParameters(struct HdfRemoteService * remoteObj, const struct AudioAdapterDescriptor *desc,
    struct HdfSBuf **data, struct HdfSBuf **reply)
{
    if (desc == NULL || desc->adapterName == NULL || desc->ports == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    bool descFlag = false;
    if (g_localAudioProxyAdapterNum <= 0 || g_localAudioProxyAdapterNum > SUPPORT_ADAPTER_NUM_MAX) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (g_localAudioProxyAdapterAddrOut != NULL) { // add for Fuzz
        for (int index = 0; index < g_localAudioProxyAdapterNum; index++) {
            if (&g_localAudioProxyAdapterAddrOut[index] == desc) {
                descFlag = true;
                break;
            }
        }
        if (!descFlag) {
            LOG_FUN_ERR("The proxy desc address passed in is invalid");
            return AUDIO_HAL_ERR_INVALID_OBJECT;
        }
    }
    if (AudioAdapterExist(desc->adapterName)) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessSBuf(data, reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(remoteObj, *data)) {
        LOG_FUN_ERR("write interface token failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    // adapterName
    if (!HdfSbufWriteString(*data, desc->adapterName)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    // dir
    uint32_t tempDir = (uint32_t)desc->ports->dir;
    if (!HdfSbufWriteUint32(*data, tempDir)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyManagerLoadAdapter(struct AudioProxyManager *manager, const struct AudioAdapterDescriptor *desc,
                                     struct AudioAdapter **adapter)
{
    LOG_FUN_INFO();
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if ((&g_localAudioProxyMgr) != manager || manager == NULL || manager->remote == NULL || desc == NULL ||
        adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = LoadAdapterPrepareParameters(manager->remote, desc, &data, &reply);
    if (ret < 0) {
        LOG_FUN_ERR("LoadAdapterPrepareParameters Fail");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)calloc(1, sizeof(struct AudioHwAdapter));
    if (hwAdapter == NULL) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwAdapter->common.InitAllPorts = AudioProxyAdapterInitAllPorts;
    hwAdapter->common.CreateRender = AudioProxyAdapterCreateRender;
    hwAdapter->common.DestroyRender = AudioProxyAdapterDestroyRender;
    hwAdapter->common.CreateCapture = AudioProxyAdapterCreateCapture;
    hwAdapter->common.DestroyCapture = AudioProxyAdapterDestroyCapture;
    hwAdapter->common.GetPortCapability = AudioProxyAdapterGetPortCapability;
    hwAdapter->common.SetPassthroughMode = AudioProxyAdapterSetPassthroughMode;
    hwAdapter->common.GetPassthroughMode = AudioProxyAdapterGetPassthroughMode;
    hwAdapter->adapterDescriptor = *desc;
    hwAdapter->proxyRemoteHandle = manager->remote; // get dispatch Server
    ret = AudioAddAdapterAddrToList((AudioHandle)((&hwAdapter->common)), desc);
    if (ret < 0) { // add for Fuzz test
        LOG_FUN_ERR("The proxy AudioProxyAdapterAddrGet check Failed");
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwAdapter);
        return ret;
    }
    ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_LOAD_ADAPTER, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to send service call!!");
        AudioProxyBufReplyRecycle(data, reply);
        if (AudioDelAdapterAddrFromList((AudioHandle)(&hwAdapter->common))) {
            LOG_FUN_ERR("The proxy Adapter or proxyRender not in MgrList");
        }
        AudioMemFree((void **)&hwAdapter);
        return ret;
    }
    *adapter = &hwAdapter->common;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

void AudioProxyManagerUnloadAdapter(struct AudioProxyManager *manager, struct AudioAdapter *adapter)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy Adapter address passed in is invalid");
        return;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    const char *adapterName = NULL;
    int32_t i = 0;
    int32_t portNum;
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if ((&g_localAudioProxyMgr) != manager || manager == NULL || manager->remote == NULL || adapter == NULL) {
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
        if (!HdfRemoteServiceWriteInterfaceToken(manager->remote, data)) {
            LOG_FUN_ERR("write interface token failed");
            AudioProxyBufReplyRecycle(data, reply);
            return;
        }
        adapterName = hwAdapter->adapterDescriptor.adapterName;
        if (HdfSbufWriteString(data, adapterName)) {
            int32_t ret = AudioProxyDispatchCall(manager->remote, AUDIO_HDI_MGR_UNLOAD_ADAPTER, data, reply);
            if (ret < 0) {
                LOG_FUN_ERR("Send Server fail!");
            }
        }
        AudioProxyBufReplyRecycle(data, reply);
    }
    if (AudioDelAdapterAddrFromList((AudioHandle)adapter) < 0) {
        LOG_FUN_ERR("The proxy Adapter or proxyRender not in MgrList");
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
    LOG_FUN_INFO();
    static bool audioProxyAdapterAddrMgrFlag = false;
    if (audioProxyAdapterAddrMgrFlag) {
        return (&g_localAudioProxyMgr);
    }
    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        LOG_FUN_ERR("HDIServiceManagerGet failed!");
        return NULL;
    }

    struct HdfRemoteService *remote = serviceMgr->GetService(serviceMgr, HDI_SERVER_NAME);
    if (remote == NULL) {
        LOG_FUN_ERR("Remote GetService failed!");
        HDIServiceManagerRelease(serviceMgr);
        return NULL;
    }

    if (!HdfRemoteServiceSetInterfaceDesc(remote, "ohos.hdi.audio_service")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }

    HDIServiceManagerRelease(serviceMgr);
    (void)memset_s(&g_localAudioProxyMgr, sizeof(struct AudioProxyManager), 0, sizeof(struct AudioProxyManager));
    ProxyMgrConstruct(&g_localAudioProxyMgr);
    g_localAudioProxyMgr.remote = remote;
    AudioAdapterAddrMgrInit(); // memset for Fuzz
    audioProxyAdapterAddrMgrFlag = true;
    return (&g_localAudioProxyMgr);
}

