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

#include <hdf_device_object.h>
#include "hdf_audio_server_common.h"
#include "hdf_audio_server_render.h"
#include "hdf_audio_server.h"
namespace OHOS::HDI::Audio_Bluetooth {
#define HDF_LOG_TAG hdf_audio_bluetooth_server

struct AudioAdapterDescriptor *g_descs = NULL;
struct AudioManager *g_serverManager = NULL;
/**************************public************************/
int32_t HdiServiceGetFuncs()
{
    HDF_LOGE("%{public}s: enter to", __func__);
    if (g_serverManager != NULL) {
        return AUDIO_HAL_SUCCESS;
    }
    g_serverManager = GetAudioManagerFuncs();
    if (g_serverManager == NULL) {
        HDF_LOGE("%{public}s: GetAudioManagerFuncs FAIL!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

static int32_t HdiServiceGetAllAdapter(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGE("%{public}s: enter to", __func__);
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    static bool getAdaptersFlag = true;
    struct AudioAdapterDescriptor *descs = NULL;
    struct AudioManager *manager = g_serverManager;
    int32_t size = 0;

    if (manager == NULL) {
        HDF_LOGE("%{public}s: Manager is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = manager->GetAllAdapters(manager, &descs, &size);
    if (ret < 0) {
        HDF_LOGE("%{public}s: g_manager->GetAllAdapters error", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (size > MAX_AUDIO_ADAPTER_NUM_SERVER || size == 0 || descs == NULL || ret < 0) {
        HDF_LOGE("%{public}s: size or g_descs is error", __func__);
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    g_descs = descs;
    if (getAdaptersFlag) {  // Initialize only once
        ret = AdaptersServerManageInit(descs, size);
        if (ret != AUDIO_HAL_SUCCESS) {
            HDF_LOGE("%{public}s: AdapterServerManageInit fail", __func__);

            return ret;
        }
        getAdaptersFlag = false;
    }
    HDF_LOGE("%{public}s: out", __func__);

    return AUDIO_HAL_SUCCESS;
}

int SwitchAdapter(struct AudioAdapterDescriptor *descs, const char *adapterNameCase, AudioPortDirection portFlag,
                  struct AudioPort *renderPort, const int size)
{
    if (descs == NULL || adapterNameCase == NULL || renderPort == NULL) {
        return HDF_FAILURE;
    }
    for (int index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc == NULL) {
            continue;
        }
        if (desc->adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (strcmp(desc->adapterName, adapterNameCase)) {
            continue;
        }
        for (uint32_t port = 0; port < desc->portNum; port++) {
            if (desc->ports[port].dir == portFlag) {
                *renderPort = desc->ports[port];
                HDF_LOGE("%{public}s: success!,%{public}d", __func__, portFlag);
                return index;
            }
        }
    }
    HDF_LOGE("%{public}s: out!", __func__);
    return HDF_FAILURE;
}

/* Adapter Check */

static int32_t HdiServiceLoadAdapter(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioAdapter *adapter = NULL;
    struct AudioPort renderPort;
    const char *adapterName = NULL;
    uint32_t tempDir = 0;
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = AudioAdapterCheckListExist(adapterName);
    if (ret == AUDIO_HAL_ERR_INVALID_PARAM) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (ret == AUDIO_HAL_SUCCESS) {
        HDF_LOGE("%{public}s: adapte already exist !", __func__);
        return AUDIO_HAL_SUCCESS;
    }
    if (!HdfSbufReadUint32(data, &tempDir)) {
        HDF_LOGE("%{public}s: adapter need Load!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioPortDirection port = (AudioPortDirection)tempDir;
    struct AudioManager *manager = g_serverManager;
    if (adapterName == NULL || manager == NULL || g_descs == NULL) {
        HDF_LOGE("%{public}s: Point is NULL!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int index = SwitchAdapter(g_descs, adapterName, port,
        &renderPort, ServerManageGetAdapterNum());
    if (index < 0) {
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    struct AudioAdapterDescriptor *desc = &g_descs[index];
    ret = manager->LoadAdapter(manager, desc, &adapter);
    if (ret < 0) {
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    if (adapter == NULL) {
        HDF_LOGE("%{public}s: load audio device failed", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioAdapterListAdd(adapterName, adapter)) {
        HDF_LOGE("%{public}s: AudioAdapterListAdd error!", __func__);
        manager->UnloadAdapter(manager, adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

static int32_t HdiServiceInitAllPorts(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGE("%{public}s: HdiServiceInitAllPorts", __func__);
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    const char *adapterName = NULL;
    struct AudioAdapter *adapter = NULL;
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioAdapterListGetAdapter(adapterName, &adapter)) {
        HDF_LOGE("%{public}s: AudioAdapterListGetAdapter fail", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (adapter->InitAllPorts(adapter)) {
        HDF_LOGE("%{public}s: InitAllPorts fail", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

static int32_t HdiServiceUnloadAdapter(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioAdapter *adapter = NULL;
    const char *adapterName = NULL;
    int ret;
    struct AudioManager *manager = g_serverManager;
    if (manager == NULL) {
        HDF_LOGE("%{public}s: Point is NULL!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioAdapterListDestory(adapterName, &adapter);
    if (ret == AUDIO_HAL_ERR_INTERNAL) {
        HDF_LOGE("%{public}s: Other dev Use the adapter", __func__);
        return AUDIO_HAL_SUCCESS;
    } else if (ret == AUDIO_HAL_ERR_INVALID_PARAM) {
        HDF_LOGE("%{public}s: param invalid!", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    } else {
        HDF_LOGE("%{public}s: Unload the adapter!", __func__);
    }
    if (adapter == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    manager->UnloadAdapter(manager, adapter);
    return AUDIO_HAL_SUCCESS;
}

static int32_t HdiServiceGetPortCapability(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGE("%{public}s: enter", __func__);
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioPort port;
    struct AudioPortCapability capability;
    struct AudioAdapter *adapter = NULL;
    const char *adapterName = NULL;
    uint32_t tempDir;
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &tempDir)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    port.dir = (AudioPortDirection)tempDir;
    if (!HdfSbufReadUint32(data, &port.portId)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if ((port.portName = HdfSbufReadString(data)) == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    HDF_LOGE("port.portName = %{public}s", port.portName);
    if (AudioAdapterListGetAdapter(adapterName, &adapter)) {
        HDF_LOGE("%{public}s: AudioAdapterListGetAdapter fail", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (adapter == NULL) {
        HDF_LOGE("%{public}s: HdiServiceCreatRender adapter is NULL!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = adapter->GetPortCapability(adapter, &port, &capability);
    if (ret < 0) {
        HDF_LOGE("%{public}s: ret failed", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

static int32_t HdiServiceSetPassthroughMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGE("%{public}s: enter", __func__);
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioPort port;
    AudioPortPassthroughMode mode;
    struct AudioAdapter *adapter = NULL;
    const char *adapterName = NULL;
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t tempDir;
    if (!HdfSbufReadUint32(data, &tempDir)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    port.dir = (AudioPortDirection)tempDir;
    HDF_LOGE("port.dir = %{public}d", port.dir);
    if (!HdfSbufReadUint32(data, &port.portId)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if ((port.portName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("port.portName = %{public}s", port.portName);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    HDF_LOGE("port.portName = %{public}s", port.portName);
    uint32_t tempMode = 0;
    if (!HdfSbufReadUint32(data, &tempMode)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    mode = (AudioPortPassthroughMode)tempMode;
    HDF_LOGE("%{public}s: ready in,mode = %{public}d", __func__, mode);
    if (AudioAdapterListGetAdapter(adapterName, &adapter)) {
        HDF_LOGE("%{public}s: AudioAdapterListGetAdapter fail", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (adapter == NULL) {
        HDF_LOGE("%{public}s: HdiServiceCreatRender adapter is NULL!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int ret = adapter->SetPassthroughMode(adapter, &port, mode);
    return ret;
}
static int32_t HdiServiceGetPassthroughMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    AudioPortPassthroughMode mode = PORT_PASSTHROUGH_AUTO;
    struct AudioAdapter *adapter = NULL;
    const char *adapterName = NULL;
    struct AudioPort port;
    memset_s(&port, sizeof(struct AudioPort), 0, sizeof(struct AudioPort));
    if ((adapterName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("%{public}s: adapterNameCase Is NULL", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t tempDir = port.dir;
    if (!HdfSbufReadUint32(data, &tempDir)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    port.dir = (AudioPortDirection)tempDir;
    if (!HdfSbufReadUint32(data, &port.portId)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if ((port.portName = HdfSbufReadString(data)) == NULL) {
        HDF_LOGE("port.portName = %{public}s", port.portName);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioAdapterListGetAdapter(adapterName, &adapter)) {
        HDF_LOGE("%{public}s: AudioAdapterListGetAdapter fail", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (adapter == NULL) {
        HDF_LOGE("%{public}s: adapter is NULL!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int ret = adapter->GetPassthroughMode(adapter, &port, &mode);
    if (ret < 0) {
        HDF_LOGE("%{public}s: GetPassthroughMode ret failed", __func__);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempMode = (uint32_t)mode;
    if (!HdfSbufWriteUint32(reply, tempMode)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

/*****************************end*************************/
struct HdiServiceDispatchCmdHandleList g_hdiServiceDispatchCmdHandleList[] = {
    {AUDIO_HDI_MGR_GET_FUNCS, (AudioAllfunc)HdiServiceGetFuncs},
    {AUDIO_HDI_MGR_GET_ALL_ADAPTER, HdiServiceGetAllAdapter},
    {AUDIO_HDI_MGR_LOAD_ADAPTER, HdiServiceLoadAdapter},
    {AUDIO_HDI_MGR_UNLOAD_ADAPTER, HdiServiceUnloadAdapter},
    {AUDIO_HDI_ADT_INIT_PORTS, HdiServiceInitAllPorts},
    {AUDIO_HDI_ADT_GET_PORT_CAPABILITY, HdiServiceGetPortCapability},
    {AUDIO_HDI_ADT_SET_PASS_MODE, HdiServiceSetPassthroughMode},
    {AUDIO_HDI_ADT_GET_PASS_MODE, HdiServiceGetPassthroughMode},
    {AUDIO_HDI_RENDER_CREATE_RENDER, HdiServiceCreatRender},
    {AUDIO_HDI_RENDER_DESTROY, HdiServiceRenderDestory},
    {AUDIO_HDI_RENDER_START, HdiServiceRenderStart},
    {AUDIO_HDI_RENDER_STOP, HdiServiceRenderStop},
    {AUDIO_HDI_RENDER_PAUSE, HdiServiceRenderPause},
    {AUDIO_HDI_RENDER_RESUME, HdiServiceRenderResume},
    {AUDIO_HDI_RENDER_FLUSH, HdiServiceRenderFlush},
    {AUDIO_HDI_RENDER_GET_FRAME_SIZE, HdiServiceRenderGetFrameSize},
    {AUDIO_HDI_RENDER_GET_FRAME_COUNT, HdiServiceRenderGetFrameCount},
    {AUDIO_HDI_RENDER_SET_SAMPLE_ATTR, HdiServiceRenderSetSampleAttr},
    {AUDIO_HDI_RENDER_GET_SAMPLE_ATTR, HdiServiceRenderGetSampleAttr},
    {AUDIO_HDI_RENDER_GET_CUR_CHANNEL_ID, HdiServiceRenderGetCurChannelId},
    {AUDIO_HDI_RENDER_CHECK_SCENE_CAPABILITY, HdiServiceRenderCheckSceneCapability},
    {AUDIO_HDI_RENDER_SELECT_SCENE, HdiServiceRenderSelectScene},
    {AUDIO_HDI_RENDER_GET_MUTE, HdiServiceRenderGetMute},
    {AUDIO_HDI_RENDER_SET_MUTE, HdiServiceRenderSetMute},
    {AUDIO_HDI_RENDER_SET_VOLUME, HdiServiceRenderSetVolume},
    {AUDIO_HDI_RENDER_GET_VOLUME, HdiServiceRenderGetVolume},
    {AUDIO_HDI_RENDER_GET_GAIN_THRESHOLD, HdiServiceRenderGetGainThreshold},
    {AUDIO_HDI_RENDER_GET_GAIN, HdiServiceRenderGetGain},
    {AUDIO_HDI_RENDER_SET_GAIN, HdiServiceRenderSetGain},
    {AUDIO_HDI_RENDER_GET_LATENCY, HdiServiceRenderGetLatency},
    {AUDIO_HDI_RENDER_RENDER_FRAME, HdiServiceRenderRenderFrame},
    {AUDIO_HDI_RENDER_GET_RENDER_POSITION, HdiServiceRenderGetRenderPosition},
    {AUDIO_HDI_RENDER_GET_SPEED, HdiServiceRenderGetSpeed},
    {AUDIO_HDI_RENDER_SET_SPEED, HdiServiceRenderSetSpeed},
    {AUDIO_HDI_RENDER_SET_CHANNEL_MODE, HdiServiceRenderSetChannelMode},
    {AUDIO_HDI_RENDER_GET_CHANNEL_MODE, HdiServiceRenderGetChannelMode},
    {AUDIO_HDI_RENDER_SET_EXTRA_PARAMS, HdiServiceRenderSetExtraParams},
    {AUDIO_HDI_RENDER_GET_EXTRA_PARAMS, HdiServiceRenderGetExtraParams},
    {AUDIO_HDI_RENDER_REQ_MMAP_BUFFER, HdiServiceRenderReqMmapBuffer},
    {AUDIO_HDI_RENDER_GET_MMAP_POSITION, HdiServiceRenderGetMmapPosition},
    {AUDIO_HDI_RENDER_TURN_STAND_BY_MODE, HdiServiceRenderTurnStandbyMode},
    {AUDIO_HDI_RENDER_DEV_DUMP, HdiServiceRenderDevDump},
    {AUDIO_HDI_RENDER_REG_CALLBACK, HdiServiceRenderRegCallback},
    {AUDIO_HDI_RENDER_DRAIN_BUFFER, HdiServiceRenderDrainBuffer},
};

static int32_t HdiServiceDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
                                  struct HdfSBuf *reply)
{
    unsigned int i;
    if (client == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: ControlDispatch: input para is NULL.", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    HDF_LOGE("ControlDispatch: valid cmdId = %{public}d", cmdId);

    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (cmdId > AUDIO_HDI_RENDER_DRAIN_BUFFER || cmdId < 0) {
        HDF_LOGE("ControlDispatch: invalid cmdId = %{public}d", cmdId);
        return AUDIO_HAL_ERR_INTERNAL;
    } else if (cmdId <= AUDIO_HDI_RENDER_DRAIN_BUFFER) {
        for (i = 0; i < sizeof(g_hdiServiceDispatchCmdHandleList) /
            sizeof(g_hdiServiceDispatchCmdHandleList[0]); ++i) {
            if ((cmdId == (int)(g_hdiServiceDispatchCmdHandleList[i].cmd)) &&
                (g_hdiServiceDispatchCmdHandleList[i].func != NULL)) {
                    return g_hdiServiceDispatchCmdHandleList[i].func(client, data, reply);
            }
        }
    }
    return AUDIO_HAL_ERR_INTERNAL;
}

void AudioHdiServerRelease(struct HdfDeviceObject *deviceObject)
{
    LOG_FUN_INFO();
    /* g_renderAndCaptureManage release */
    AdaptersServerManageInfomationRecycle();

    if (deviceObject == NULL) {
        HDF_LOGE("%{public}s: deviceObject is null!", __func__);
        return;
    }
    deviceObject->service = NULL;
    return;
}

int AudioHdiServerBind(struct HdfDeviceObject *deviceObject)
{
    LOG_FUN_INFO();
    if (deviceObject == NULL) {
        HDF_LOGE("%{public}s: deviceObject is null!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    static struct IDeviceIoService hdiService = {
        .Open = NULL,
        .Dispatch = HdiServiceDispatch,
        .Release = NULL,
    };
    if (HdiServiceGetFuncs()) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int ret = HdfDeviceObjectSetInterfaceDesc(deviceObject, "ohos.hdi.audio_bluetooth_service");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to set interface desc");
        return ret;
    }
    deviceObject->service = &hdiService;
    return AUDIO_HAL_SUCCESS;
}

int AudioHdiServerInit(struct HdfDeviceObject *deviceObject)
{
    LOG_FUN_INFO();
    if (deviceObject == NULL) {
        HDF_LOGE("%{public}s: deviceObject is null!", __func__);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_SUCCESS;
}

struct HdfDriverEntry g_hdiServerEntry = {
    .moduleVersion = 1,
    .moduleName = "audio_bluetooth_hdi_adapter_server",
    .Bind = AudioHdiServerBind,
    .Init = AudioHdiServerInit,
    .Release = AudioHdiServerRelease,
};

#ifdef __cplusplus
extern "C" {
#endif

HDF_INIT(g_hdiServerEntry);

#ifdef __cplusplus
}
#endif
}