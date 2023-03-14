/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "p2p.h"
#include "message/message_router.h"
#include "message/sidecar.h"
#include "wifi_base.h"
#include "wifi_mac80211_ops.h"
#include "hdf_wlan_services.h"
#include "hdf_wlan_utils.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

#ifdef __cplusplus
extern "C" {
#endif
static int32_t RemainOnChannel(struct NetDevice *netdev, WifiOnChannel *onChannel)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, RemainOnChannel);
    return chipDriver->p2pOps->RemainOnChannel(netdev, onChannel);
}

static int32_t  WifiCmdRemainOnChannel(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    WifiOnChannel wifiOnChannel = {0};

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &(wifiOnChannel.freq))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "freq");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &(wifiOnChannel.duration))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "duration");
        return HDF_FAILURE;
    }
    ret = RemainOnChannel(netdev, &wifiOnChannel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to remain on channel,%d", __func__, ret);
    }
    return ret;
}

static int32_t ProbeReqReport(struct NetDevice *netdev, int32_t report)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, ProbeReqReport);
    return chipDriver->p2pOps->ProbeReqReport(netdev, report);
}

static int32_t WifiCmdProbeReqReport(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    int32_t report;

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reqData, &(report))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "report");
        return HDF_FAILURE;
    }

    ret = ProbeReqReport(netdev, report);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to probe req report,%d", __func__, ret);
    }
    return ret;
}

static int32_t CancelRemainOnChannel(struct NetDevice *netdev)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, CancelRemainOnChannel);
    return chipDriver->p2pOps->CancelRemainOnChannel(netdev);
}

static int32_t WifiCmdCancelRemainOnChannel(const RequestContext *context, struct HdfSBuf *reqData,
    struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }

    ret = CancelRemainOnChannel(netdev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to cancel remain on channel,%d", __func__, ret);
    }
    return ret;
}

static int32_t AddIf(struct NetDevice *netdev, WifiIfAdd *ifAdd)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, AddIf);
    return chipDriver->p2pOps->AddIf(netdev, ifAdd);
}

static int32_t WifiCmdAddIf(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    WifiIfAdd ifAdd = {0};

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadUint8(reqData, &(ifAdd.type))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "type");
        return HDF_FAILURE;
    }

    ret = AddIf(netdev, &ifAdd);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to cancel remain on channel,%d", __func__, ret);
    }
    return ret;
}

static int32_t RemoveIf(struct NetDevice *netdev, WifiIfRemove *ifRemove)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, RemoveIf);
    return chipDriver->p2pOps->RemoveIf(netdev, ifRemove);
}

static int32_t WifiCmdRemoveIf(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    WifiIfRemove *ifRemove = NULL;
    uint32_t dataSize;
    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }

    dataSize = 0;
    if (!HdfSbufReadBuffer(reqData, (const void **)&(ifRemove), &dataSize) || dataSize != sizeof(WifiIfRemove)) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    ret = RemoveIf(netdev, ifRemove);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to remove interface,%d", __func__, ret);
    }
    return ret;
}

static int32_t SetApWpsP2pIe(struct NetDevice *netdev, WifiAppIe *appIe)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, SetApWpsP2pIe);
    return chipDriver->p2pOps->SetApWpsP2pIe(netdev, appIe);
}

static int32_t WifiCmdSetApWpsP2pIe(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    WifiAppIe appIe = {0};

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadUint32(reqData, &(appIe.ieLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ieLen");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(reqData, &(appIe.appIeType))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "appIeType");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reqData, (const void**)&(appIe.ie), &(appIe.ieLen))) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "appIeType");
        return HDF_FAILURE;
    }
    ret = SetApWpsP2pIe(netdev, &appIe);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to setapwpsp2pie,%d", __func__, ret);
    }
    return ret;
}

int32_t GetDriverFlag (struct NetDevice *netdev, WifiGetDrvFlags **params)
{
    struct HdfChipDriver *chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad net device found!", __func__);
        return HDF_FAILURE;
    }

    RETURN_IF_CHIPOPS_NOT_IMPLEMENT(chipDriver->p2pOps, GetDriverFlag);
    return chipDriver->p2pOps->GetDriverFlag(netdev, params);
}

static int32_t WifiCmdGetDriverFlag(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    const char *ifName = NULL;
    WifiGetDrvFlags *params = NULL;

    (void)context;
    if (reqData == NULL || rspData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ifName = HdfSbufReadString(reqData);
    if (ifName == NULL) {
        HDF_LOGE("%s: %s!ParamName=%s", __func__, ERROR_DESC_READ_REQ_FAILED, "ifName");
        return HDF_FAILURE;
    }
    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGE("%s: netdev not found!ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }

    ret = GetDriverFlag(netdev, &params);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to getdriverflag,%d", __func__, ret);
    }

    if (!HdfSbufWriteUint64(rspData, params->drvFlags)) {
        HDF_LOGE("%s:%s!", __func__, ERROR_DESC_WRITE_RSP_FAILED);
        ret = HDF_ERR_IO;
    }

    HDF_LOGE("WifiCmdGetDriverFlag:%llx", params->drvFlags);
    return ret;
}

static struct MessageDef g_wifiP2pFeatureCmds[] = {
    DUEMessage(CMD_P2P_PROBE_REQ_REPORT, WifiCmdProbeReqReport, 0),
    DUEMessage(CMD_P2P_REMAIN_ON_CHANNEL, WifiCmdRemainOnChannel, 0),
    DUEMessage(CMD_P2P_CANCEL_REMAIN_ON_CHANNEL, WifiCmdCancelRemainOnChannel, 0),
    DUEMessage(CMD_P2P_ADD_IF, WifiCmdAddIf, 0),
    DUEMessage(CMD_P2P_REMOVE_IF, WifiCmdRemoveIf, 0),
    DUEMessage(CMD_P2P_SET_AP_WPS_P2P_IE, WifiCmdSetApWpsP2pIe, 0),
    DUEMessage(CMD_P2P_GET_DRIVER_FLAGS, WifiCmdGetDriverFlag, 0),
};
ServiceDefine(P2PService, P2P_SERVICE_ID, g_wifiP2pFeatureCmds);

static Service *g_p2pService = NULL;

int32_t P2pInit(struct WifiFeature *feature)
{
    (void)feature;
    if (g_p2pService == NULL) {
        ServiceCfg cfg = {
            .dispatcherId = DEFAULT_DISPATCHER_ID
        };
        g_p2pService = CreateService(P2PService, &cfg);
        if (g_p2pService == NULL) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t P2pDeinit(struct WifiFeature *feature)
{
    (void)feature;
    if (g_p2pService != NULL && g_p2pService->Destroy != NULL) {
        g_p2pService->Destroy(g_p2pService);
        g_p2pService = NULL;
    }
    return HDF_SUCCESS;
}

struct WifiFeature g_wifiP2PFeature = {
    .name = "p2p",
    .init = P2pInit,
    .deInit = P2pDeinit
};

struct WifiFeature *GetWifiP2pFeature(void)
{
    return &g_wifiP2PFeature;
}

#ifdef __cplusplus
}
#endif
