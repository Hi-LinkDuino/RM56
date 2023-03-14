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

#include "audio_manager.h"
#include <limits.h>
#include "audio_adapter_info_common.h"
#include "audio_hal_log.h"
#include "audio_interface_lib_capture.h"
#include "audio_interface_lib_render.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_IMPL

struct AudioAdapterDescriptor *g_localAddrAudioAdapterOut = NULL; // add for Fuzz
int g_localAdapterNum = 0; // add for Fuzz

BindServiceRenderSo g_bindServiceRender = NULL;
InterfaceLibModeRenderSo g_interfaceLibModeRender = NULL;
CloseServiceRenderSo g_closeServiceRender = NULL;

BindServiceCaptureSo g_bindServiceCapture = NULL;
InterfaceLibModeCaptureSo g_interfaceLibModeCapture = NULL;
CloseServiceCaptureSo g_closeServiceCapture = NULL;

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
PathSelGetConfToJsonObj g_pathSelGetConfToJsonObj = NULL;
PathSelAnalysisJson g_pathSelAnalysisJson = NULL;
#endif

static const char *g_captureSoPath = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_capture");
static const char *g_renderSoPath = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_render");

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
static const char *g_pathSelectSoPath = HDF_LIBRARY_FULL_PATH("libhdi_audio_path_select");
#endif

static void *g_ptrCaptureHandle = NULL;
static void *g_ptrRenderHandle = NULL;

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
static void *g_ptrPathSelHandle = NULL;
#endif

BindServiceRenderSo *AudioSoGetBindServiceRender(void)
{
    return &g_bindServiceRender;
}

InterfaceLibModeRenderSo *AudioSoGetInterfaceLibModeRender(void)
{
    return &g_interfaceLibModeRender;
}

CloseServiceRenderSo *AudioSoGetCloseServiceRender(void)
{
    return &g_closeServiceRender;
}

BindServiceCaptureSo *AudioSoGetBindServiceCapture(void)
{
    return &g_bindServiceCapture;
}

InterfaceLibModeCaptureSo *AudioSoGetInterfaceLibModeCapture(void)
{
    return &g_interfaceLibModeCapture;
}

CloseServiceCaptureSo *AudioSoGetCloseServiceCapture(void)
{
    return &g_closeServiceCapture;
}


#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
PathSelGetConfToJsonObj *AudioSoGetPathSelGetConfToJsonObj(void)
{
    return &g_pathSelGetConfToJsonObj;
}

PathSelAnalysisJson *AudioSoGetPathSelAnalysisJson(void)
{
    return &g_pathSelAnalysisJson;
}
#endif

int32_t InitCaptureSoHandle(const char *captureSoPath)
{
    if (captureSoPath == NULL) {
        LOG_FUN_ERR("captureSoPath is NULL");
        return HDF_FAILURE;
    }
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(captureSoPath, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    if (g_ptrCaptureHandle == NULL) {
        char *cPathBuf = pathBuf;
        g_ptrCaptureHandle = dlopen(cPathBuf, RTLD_LAZY);
        if (g_ptrCaptureHandle == NULL) {
            LOG_FUN_ERR("open lib capture so fail, reason:%s", dlerror());
            return HDF_FAILURE;
        }
        g_bindServiceCapture = dlsym(g_ptrCaptureHandle, "AudioBindServiceCapture");
        g_interfaceLibModeCapture = dlsym(g_ptrCaptureHandle, "AudioInterfaceLibModeCapture");
        g_closeServiceCapture = dlsym(g_ptrCaptureHandle, "AudioCloseServiceCapture");
        if (g_bindServiceCapture == NULL || g_interfaceLibModeCapture == NULL || g_closeServiceCapture == NULL) {
            LOG_FUN_ERR("lib capture so func not found!");
            AudioDlClose(&g_ptrCaptureHandle);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t InitRenderSoHandle(const char *renderSoPath)
{
    if (renderSoPath == NULL) {
        LOG_FUN_ERR("renderSoPath is NULL");
        return HDF_FAILURE;
    }
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(renderSoPath, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    if (g_ptrRenderHandle == NULL) {
        char *cPathBuf = pathBuf;
        g_ptrRenderHandle = dlopen(cPathBuf, RTLD_LAZY);
        if (g_ptrRenderHandle == NULL) {
            LOG_FUN_ERR("open lib render so fail, reason:%s", dlerror());
            return HDF_FAILURE;
        }
        g_bindServiceRender = dlsym(g_ptrRenderHandle, "AudioBindServiceRender");
        g_interfaceLibModeRender = dlsym(g_ptrRenderHandle, "AudioInterfaceLibModeRender");
        g_closeServiceRender = dlsym(g_ptrRenderHandle, "AudioCloseServiceRender");
        if (g_bindServiceRender == NULL || g_interfaceLibModeRender == NULL || g_closeServiceRender == NULL) {
            LOG_FUN_ERR("lib render so func not found!");
            AudioDlClose(&g_ptrRenderHandle);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
int32_t InitPathSelectSoHandle(const char *pathSelectSoPath)
{
    if (pathSelectSoPath == NULL) {
        LOG_FUN_ERR("pathSelectSoPath is NULL");
        return HDF_FAILURE;
    }
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(pathSelectSoPath, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    if (g_ptrPathSelHandle == NULL) {
        char *cPathBuf = pathBuf;
        g_ptrPathSelHandle = dlopen(cPathBuf, RTLD_LAZY);
        if (g_ptrPathSelHandle == NULL) {
            LOG_FUN_ERR("open lib PathSelct so fail, reason:%s", dlerror());
            return HDF_FAILURE;
        }
        g_pathSelGetConfToJsonObj = dlsym(g_ptrPathSelHandle, "AudioPathSelGetConfToJsonObj");
        g_pathSelAnalysisJson = dlsym(g_ptrPathSelHandle, "AudioPathSelAnalysisJson");
        if (g_pathSelGetConfToJsonObj == NULL || g_pathSelAnalysisJson == NULL) {
            LOG_FUN_ERR("lib PathSelct so func not found!");
            AudioDlClose(&g_ptrPathSelHandle);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}
#endif

int32_t AudioManagerGetAllAdapters(struct AudioManager *manager,
                                   struct AudioAdapterDescriptor **descs,
                                   int *size)
{
    LOG_FUN_INFO();
    if (manager == NULL || descs == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = AudioAdaptersForUser(descs, size);
    if (ret < 0) {
        LOG_FUN_ERR("AudioAdaptersForUser FAIL!");
        return AUDIO_HAL_ERR_NOTREADY; // Failed to read sound card configuration file
    }
    if (g_captureSoPath == NULL || g_renderSoPath == NULL) {
        LOG_FUN_ERR("sopath is error");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (*descs != NULL && size != NULL && (*size) > 0) { // Fuzz test
        g_localAddrAudioAdapterOut  = *descs;
        g_localAdapterNum = *size;
    } else {
        LOG_FUN_ERR("Get AudioAdapterDescriptor Failed");
        return AUDIO_HAL_ERR_INVALID_OBJECT;
    }
    ret = InitCaptureSoHandle(g_captureSoPath);
    if (ret < 0) {
        LOG_FUN_ERR("InitCaptureSoHandle FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = InitRenderSoHandle(g_renderSoPath);
    if (ret < 0) {
        LOG_FUN_ERR("InitRenderSoHandle FAIL!");
        AudioDlClose(&g_ptrCaptureHandle);
        return AUDIO_HAL_ERR_INTERNAL;
    }
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    ret = InitPathSelectSoHandle(g_pathSelectSoPath);
    if (ret < 0 || g_pathSelGetConfToJsonObj == NULL) {
        LOG_FUN_ERR("InitPathSelectSoHandle FAIL!");
        AudioDlClose(&g_ptrRenderHandle);
        AudioDlClose(&g_ptrCaptureHandle);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = g_pathSelGetConfToJsonObj();
    if (ret < 0) {
        LOG_FUN_ERR("g_pathSelGetConfToJsonObj FAIL!");
        AudioDlClose(&g_ptrRenderHandle);
        AudioDlClose(&g_ptrCaptureHandle);
        AudioDlClose(&g_ptrPathSelHandle);
        return AUDIO_HAL_ERR_INTERNAL;
    }
#endif
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioManagerLoadAdapter(struct AudioManager *manager, const struct AudioAdapterDescriptor *desc,
                                struct AudioAdapter **adapter)
{
    LOG_FUN_INFO();
    if (manager == NULL || desc == NULL || desc->adapterName == NULL || desc->ports == NULL || adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    bool descFlag = false;
    if (g_localAdapterNum <= 0 || g_localAdapterNum > SUPPORT_ADAPTER_NUM_MAX) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (g_localAddrAudioAdapterOut != NULL) { // Fuzz test
        for (int index = 0; index < g_localAdapterNum; index++) {
            if (&g_localAddrAudioAdapterOut[index] == desc) {
                descFlag = true;
                break;
            }
        }
        if (!descFlag) {
            LOG_FUN_ERR("The desc address passed in is invalid");
            return AUDIO_HAL_ERR_INVALID_OBJECT;
        }
    }
    LOGV("%s: adapter name %s", __func__, desc->adapterName);
    if (AudioAdapterExist(desc->adapterName)) {
        LOGE("%s: not supported this adapter %s", __func__, desc->adapterName);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)calloc(1, sizeof(*hwAdapter));
    if (hwAdapter == NULL) {
        LOGE("%s: calloc AudioHwAdapter failed", __func__);
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwAdapter->common.InitAllPorts = AudioAdapterInitAllPorts;
    hwAdapter->common.CreateRender = AudioAdapterCreateRender;
    hwAdapter->common.DestroyRender = AudioAdapterDestroyRender;
    hwAdapter->common.CreateCapture = AudioAdapterCreateCapture;
    hwAdapter->common.DestroyCapture = AudioAdapterDestroyCapture;
    hwAdapter->common.GetPortCapability = AudioAdapterGetPortCapability;
    hwAdapter->common.SetPassthroughMode = AudioAdapterSetPassthroughMode;
    hwAdapter->common.GetPassthroughMode = AudioAdapterGetPassthroughMode;
    hwAdapter->adapterDescriptor = *desc;
    hwAdapter->adapterMgrRenderFlag = 0; // The adapterMgrRenderFlag init is zero
    hwAdapter->adapterMgrCaptureFlag = 0; // The adapterMgrCaptureFlag init is zero
    int32_t ret = AudioAddAdapterAddrToList((AudioHandle)(&hwAdapter->common), desc);
    if (ret < 0) { // add for Fuzz
        LOG_FUN_ERR("AudioAdapterAddrGet check Failed");
        AudioMemFree((void **)&hwAdapter);
        return ret;
    }
    *adapter = &hwAdapter->common;
    return AUDIO_HAL_SUCCESS;
}

void AudioManagerUnloadAdapter(struct AudioManager *manager, struct AudioAdapter *adapter)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (manager == NULL || hwAdapter == NULL) {
        return;
    }
    if (hwAdapter->portCapabilitys != NULL) {
        int32_t portNum = hwAdapter->adapterDescriptor.portNum;
        int32_t i = 0;
        while (i < portNum) {
            if (&hwAdapter->portCapabilitys[i] != NULL) {
                AudioMemFree((void **)&hwAdapter->portCapabilitys[i].capability.subPorts);
            }
            i++;
        }
        AudioMemFree((void **)&hwAdapter->portCapabilitys);
    }
    if (AudioDelAdapterAddrFromList((AudioHandle)adapter)) {
        LOG_FUN_ERR("adapter or render not in MgrList");
    }
    AudioMemFree((void **)&adapter);
}

static struct AudioManager g_audioManagerFuncs = {
    .GetAllAdapters = AudioManagerGetAllAdapters,
    .LoadAdapter = AudioManagerLoadAdapter,
    .UnloadAdapter = AudioManagerUnloadAdapter,
};

struct AudioManager *GetAudioManagerFuncs(void)
{
    static bool audioAdapterAddrMgrFlag = false;
    if (!audioAdapterAddrMgrFlag) {
        AudioAdapterAddrMgrInit(); // memset for Fuzz
        audioAdapterAddrMgrFlag = true;
    }
    return &g_audioManagerFuncs;
}
