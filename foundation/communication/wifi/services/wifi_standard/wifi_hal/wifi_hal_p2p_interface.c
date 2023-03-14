/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "wifi_hal_p2p_interface.h"
#include "wifi_log.h"
#include "wifi_wpa_hal.h"
#include "wifi_p2p_hal.h"
#include "wifi_hal_callback.h"
#include "wifi_hal_module_manage.h"
#include "wifi_hal_common_func.h"
#include "wifi_hal_adapter.h"
#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalP2pInterface"

const int P2P_CONNECT_DELAY_TIME = 100000;
const char *g_wpaSupplicantP2p = "wpa_supplicant";
const char *g_systemCmdWpaP2pStart = "wpa_supplicant -iglan0 -g/data/misc/wifi/sockets/wpa";
static int g_p2pSupplicantConnectEvent = 0;

static WifiErrorNo P2pStartSupplicant(void)
{
    LOGD("Start p2p supplicant");
    const char *p2pConf = "/data/misc/wifi/wpa_supplicant/p2p_supplicant.conf";
    if ((access(p2pConf, F_OK)) != -1) {
        LOGD("p2p configure file %{public}s is exist.", p2pConf);
    } else {
        char szcpCmd[BUFF_SIZE] = {0};
        const char *cpP2pConfCmd = "cp /system/etc/wifi/p2p_supplicant.conf /data/misc/wifi/wpa_supplicant/";
        int iRet = snprintf_s(szcpCmd, sizeof(szcpCmd), sizeof(szcpCmd) - 1, "%s", cpP2pConfCmd);
        if (iRet < 0) {
            return WIFI_HAL_FAILED;
        }

        ExcuteCmd(szcpCmd);
    }

    ModuleManageRetCode ret = StartModule(g_wpaSupplicantP2p, g_systemCmdWpaP2pStart);
    if (ret == MM_SUCCESS) {
        return WIFI_HAL_SUCCESS;
    }
    LOGE("start wpa_supplicant failed!");
    return WIFI_HAL_FAILED;
}

static WifiErrorNo P2pConnectSupplicant(void)
{
    LOGD("Ready to connect p2p_wpa_supplicant.");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    return WIFI_HAL_SUCCESS;
}

static WifiErrorNo P2pStopSupplicant(void)
{
    ModuleManageRetCode ret = StopModule(g_wpaSupplicantP2p);
    if (ret == MM_FAILED) {
        LOGE("stop p2p_wpa_supplicant failed!");
        return WIFI_HAL_FAILED;
    }
    if (ret == MM_SUCCESS) {
        ReleaseWpaGlobalInterface();
    }
    return WIFI_HAL_SUCCESS;
}

static WifiErrorNo P2pDisconnectSupplicant(void)
{
    LOGD("Ready to disconnect p2p_wpa_supplicant.");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    LOGD("Disconnect p2p_wpa_supplicant finish!");
    return WIFI_HAL_SUCCESS;
}

static WifiErrorNo StopP2pWpaAndWpaHal(void)
{
    if (P2pDisconnectSupplicant() != WIFI_HAL_SUCCESS) {
        LOGE("p2p_wpa_s hal already stop!");
    }
    WifiWpaInterface *pWpaInterface = GetWifiWapGlobalInterface();
    if (pWpaInterface != NULL) {
#ifdef PRODUCT_RK
        pWpaInterface->wpaCliRemoveIface(pWpaInterface, "p2p-wlan0-0");
#else
        pWpaInterface->wpaCliRemoveIface(pWpaInterface, "p2p0");
#endif
    }
    if (P2pStopSupplicant() != WIFI_HAL_SUCCESS) {
        LOGE("p2p_wpa_supplicant stop failed!");
        return WIFI_HAL_FAILED;
    }
    LOGD("p2p_wpa_supplicant stop success");
    RelesaeWpaP2pInterface();
    return WIFI_HAL_SUCCESS;
}

static WifiErrorNo AddP2pIface(void)
{
    WifiWpaInterface *pWpaInterface = GetWifiWapGlobalInterface();
    if (pWpaInterface == NULL) {
        LOGE("Get wpa interface failed!");
        return WIFI_HAL_FAILED;
    }
    if (pWpaInterface->wpaCliConnect(pWpaInterface) < 0) {
        LOGE("Failed to connect to wpa!");
        return WIFI_HAL_FAILED;
    }
    AddInterfaceArgv argv;
#ifdef PRODUCT_RK
    if (strcpy_s(argv.name, sizeof(argv.name), "wlan0") != EOK ||
#else
    if (strcpy_s(argv.name, sizeof(argv.name), "p2p0") != EOK ||
#endif
        strcpy_s(argv.confName, sizeof(argv.confName), "/data/misc/wifi/wpa_supplicant/p2p_supplicant.conf") != EOK) {
        return WIFI_HAL_FAILED;
    }
    if (pWpaInterface->wpaCliAddIface(pWpaInterface, &argv) < 0) {
        LOGE("Failed to add wpa iface!");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStart(void)
{
    LOGD("Ready to start P2p");
    if (P2pStartSupplicant() != WIFI_HAL_SUCCESS) {
        LOGE("p2p_wpa_supplicant start failed!");
        return WIFI_HAL_OPEN_SUPPLICANT_FAILED;
    }
    LOGD("p2p_wpa_supplicant start success!");

    if (AddP2pIface() != WIFI_HAL_SUCCESS || P2pConnectSupplicant() != WIFI_HAL_SUCCESS) {
        LOGE("SupplicantHal connect p2p_wpa_supplicant failed!");
        StopP2pWpaAndWpaHal();
        P2pHalCbConnectSupplicantFailed();
        return WIFI_HAL_CONN_SUPPLICANT_FAILED;
    }
    g_p2pSupplicantConnectEvent = 1;
    WifiP2pHalCbNotifyConnectSupplicant(g_p2pSupplicantConnectEvent);
    LOGD("SupplicantHal connect p2p_wpa_supplicant success!");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStop(void)
{
    LOGD("Ready to StopP2p wifi");
    WifiErrorNo ret = StopP2pWpaAndWpaHal();
    if (ret == WIFI_HAL_FAILED) {
        return WIFI_HAL_FAILED;
    }
    LOGD("Wifi_p2p stop successfully!");
    g_p2pSupplicantConnectEvent = 0;
    WifiP2pHalCbNotifyConnectSupplicant(g_p2pSupplicantConnectEvent);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pForceStop(void)
{
    LOGD("Ready force StopP2p wifi");
    WifiErrorNo ret = StopP2pWpaAndWpaHal();
    if (ret == WIFI_HAL_FAILED) {
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetRandomMac(int enable)
{
    LOGD("P2pSetRandomMac, enable = %{public}d", enable);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetRandomMac(pMainIfc, enable);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetRandomMac fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetDeviceName(const char *name)
{
    if (name == NULL) {
        LOGE("P2pSetDeviceName, name is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetDeviceName, name = %{public}s", name);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWpsName(pMainIfc, name);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetWpsName fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetSsidPostfixName(const char *postfixName)
{
    if (postfixName == NULL) {
        LOGE("P2pSetSsidPostfixName, name is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetSsidPostfixName, name = %{public}s", postfixName);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetSsidPostfixName(pMainIfc, postfixName);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetSsidPostfixName fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsDeviceType(const char *type)
{
    if (type == NULL) {
        LOGE("P2pSetWpsDeviceType, type is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetWpsDeviceType, type = %{public}s", type);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWpsDeviceType(pMainIfc, type);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetWpsDeviceType fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsSecondaryDeviceType(const char *type)
{
    if (type == NULL) {
        LOGD("P2pSetWpsSecondaryDeviceType, type is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetWpsSecondaryDeviceType, type = %{public}s", type);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWpsSecDeviceType(pMainIfc, type);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGD("WpaP2pCliCmdSetWpsSecDeviceType fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsConfigMethods(const char *config)
{
    if (config == NULL) {
        LOGE("P2pSetWpsConfigMethods, config is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetWpsConfigMethods, config = %{public}s", config);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWpsConfigMethods(pMainIfc, config);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetWpsConfigMethods fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetDeviceAddress(char *deviceAddress, int size)
{
    if (deviceAddress == NULL) {
        LOGE("P2pGetDeviceAddress deviceAddress is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pGetDeviceAddress");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdGetDeviceAddress(pMainIfc, deviceAddress, size);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdGetDeviceAddress fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pFlush()
{
    LOGD("P2pFlush");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdFlush(pMainIfc);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdFlush fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pFlushService()
{
    LOGD("P2pFlushService");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdFlushService(pMainIfc);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdFlushService fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSaveConfig()
{
    LOGD("P2pSaveConfig");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdStoreConfig(pMainIfc);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdStoreConfig fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetupWpsPbc(const char *groupIfc, const char *address)
{
    if (groupIfc == NULL || address == NULL) {
        LOGE("P2pSetupWpsPbc groupIfc or address is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetupWpsPbc groupIfc %{public}s address %{private}s", groupIfc, address);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    WifiWpaP2pGroupInterface *pGroupIfc = GetWifiWpaP2pGroupInterface(groupIfc);
    if (pGroupIfc == NULL) {
        return WIFI_HAL_GET_P2P_GROUP_INFACE_FAILED;
    }
    P2pSupplicantErrCode ret = pGroupIfc->wpaP2pCliCmdWpsPbc(pGroupIfc, address);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdStoreConfig fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetupWpsPin(const char *groupIfc, const char *address, const char *pin, char *result, int resultLen)
{
    if (groupIfc == NULL || address == NULL || pin == NULL || result == NULL || resultLen <= 0) {
        LOGE("P2pSetupWpsPin groupIfc, address, pin and result have NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetupWpsPin");
    P2pWpsPinDisplayArgv argv;
    if (memset_s(&argv, sizeof(argv), 0, sizeof(argv)) != EOK) {
        return WIFI_HAL_FAILED;
    }
    if (strlen(pin) > 0) {
        argv.mode = P2P_PIN_KEYPAD;
        if (strncpy_s(argv.pinCode, sizeof(argv.pinCode), pin, strlen(pin)) != EOK) {
            LOGE("Failed to init pin code, the input pin code may be invalid!");
            return WIFI_HAL_FAILED;
        }
    } else {
        argv.mode = P2P_PIN_DISPLAY;
        if (strncpy_s(argv.bssid, sizeof(argv.bssid), address, strlen(address)) != EOK) {
            LOGE("Failed to init request message, the input message may be invalid!");
            return WIFI_HAL_FAILED;
        }
    }
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    WifiWpaP2pGroupInterface *pGroupIfc = GetWifiWpaP2pGroupInterface(groupIfc);
    if (pGroupIfc == NULL) {
        return WIFI_HAL_GET_P2P_GROUP_INFACE_FAILED;
    }
    P2pSupplicantErrCode ret = pGroupIfc->wpaP2pCliCmdWpsPin(pGroupIfc, &argv);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdWpsPin fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    if (argv.mode == P2P_PIN_DISPLAY) {
        if (strncpy_s(result, resultLen, argv.pinCode, strlen(argv.pinCode)) != EOK) {
            LOGE("Failed to return pin code!");
            return WIFI_HAL_FAILED;
        }
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveNetwork(int networkId)
{
    LOGD("P2pRemoveNetwork");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdRemoveNetwork(pMainIfc, networkId);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdRemoveNetwork fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pListNetworks(P2pNetworkList *infoList)
{
    if (infoList == NULL) {
        LOGE("P2pListNetworks infoList is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pListNetworks");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdNetworkList(pMainIfc, infoList);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdNetworkList fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetGroupMaxIdle(const char *groupIfc, int time)
{
    if (groupIfc == NULL) {
        LOGE("P2pSetGroupMaxIdle, groupIfc is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetGroupMaxIdle, group %{public}s time %{public}d", groupIfc, time);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    WifiWpaP2pGroupInterface *pGroupIfc = GetWifiWpaP2pGroupInterface(groupIfc);
    if (pGroupIfc == NULL) {
        return WIFI_HAL_GET_P2P_GROUP_INFACE_FAILED;
    }
    P2pSupplicantErrCode ret = pGroupIfc->wpaP2pCliCmdSetGroupIdle(pGroupIfc, time);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetGroupIdle fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetPowerSave(const char *groupIfc, int enable)
{
    if (groupIfc == NULL) {
        LOGE("P2pSetPowerSave, groupIfc is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetPowerSave, groupIfc %{public}s, enable %{public}d", groupIfc, enable);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    WifiWpaP2pGroupInterface *pGroupIfc = GetWifiWpaP2pGroupInterface(groupIfc);
    if (pGroupIfc == NULL) {
        return WIFI_HAL_GET_P2P_GROUP_INFACE_FAILED;
    }
    P2pSupplicantErrCode ret = pGroupIfc->wpaP2pCliCmdSetPowerSave(pGroupIfc, enable);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetPowerSave fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWfdEnable(int enable)
{
    LOGD("P2pSetWfdEnable");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWfdEnable(pMainIfc, enable);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetWfdEnable fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWfdDeviceConfig(const char *config)
{
    if (config == NULL) {
        LOGE("P2pSetWfdDeviceConfig, config is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetWfdDeviceConfig");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetWfdDeviceInfo(pMainIfc, config);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetWfdDeviceInfo fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStartFind(int timeout)
{
    LOGD("P2pStartFind");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdP2pFound(pMainIfc, timeout);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdP2pFound fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStopFind()
{
    LOGD("P2pStopFind");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdP2pStopFind(pMainIfc);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdP2pStopFind fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetExtListen(int enable, int period, int interval)
{
    LOGD("P2pSetExtListen");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdExtListen(pMainIfc, enable, period, interval);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdExtListen fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetListenChannel(int channel, int regClass)
{
    LOGD("P2pSetListenChannel");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetListenChannel(pMainIfc, channel, regClass);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetListenChannel fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pConnect(P2pConnectInfo *info)
{
    if (info == NULL) {
        LOGE("P2pConnect, info is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pConnect");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdConnect(pMainIfc, info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdConnect fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pCancelConnect()
{
    LOGD("P2pCancelConnect");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdCancelConnect(pMainIfc);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdCancelConnect fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pProvisionDiscovery(const char *peerBssid, int mode)
{
    if (peerBssid == NULL) {
        LOGE("P2pProvisionDiscovery, peerBssid is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pProvisionDiscovery");
    P2pProvisionDiscoveryArgv argv;
    if (memset_s(&argv, sizeof(argv), 0, sizeof(argv)) != EOK ||
        strncpy_s(argv.peerbssid, sizeof(argv.peerbssid), peerBssid, strlen(peerBssid)) != EOK) {
        LOGE("Failed to init request message, the input message may be invalid!");
        return WIFI_HAL_FAILED;
    }
    argv.mode = mode;
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdProvisionDiscovery(pMainIfc, &argv);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdProvisionDiscovery fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddGroup(int isPersistent, int networkId, int freq)
{
    LOGD("P2pAddGroup");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdGroupAdd(pMainIfc, isPersistent, networkId, freq);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdGroupAdd fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveGroup(const char *interface)
{
    if (interface == NULL) {
        LOGE("P2pRemoveGroup, interface is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pRemoveGroup");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdP2pRemoveGroup(pMainIfc, interface);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdP2pRemoveGroup fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pInvite(int persistent, const char *peerBssid, const char *goBssid, const char *ifname)
{
    if (peerBssid == NULL || goBssid == NULL || ifname == NULL) {
        LOGE("P2pRemoveGroup, peerBssid, goBssid and ifname have NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pInvite");
    P2pHalInviteArgv argv;
    if (memset_s(&argv, sizeof(argv), 0, sizeof(argv)) != EOK ||
        strncpy_s(argv.peerbssid, sizeof(argv.peerbssid), peerBssid, strlen(peerBssid)) != EOK ||
        strncpy_s(argv.gobssid, sizeof(argv.gobssid), goBssid, strlen(goBssid)) != EOK ||
        strncpy_s(argv.ifname, sizeof(argv.ifname), ifname, strlen(ifname)) != EOK) {
        LOGE("Failed to init request message, the input message may be invalid!");
        return WIFI_HAL_FAILED;
    }
    argv.persistent = persistent;
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdInvite(pMainIfc, &argv);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdInvite fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pReinvoke(int networkId, const char *bssid)
{
    if (bssid == NULL) {
        LOGE("P2pReinvoke, bssid is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pReinvoke");
    P2pHalReInviteArgv argv;
    if (memset_s(&argv, sizeof(argv), 0, sizeof(argv)) != EOK ||
        strncpy_s(argv.peerbssid, sizeof(argv.peerbssid), bssid, strlen(bssid)) != EOK) {
        LOGE("Failed to init request message, the input message may be invalid!");
        return WIFI_HAL_FAILED;
    }
    argv.networkId = networkId;
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdReInvite(pMainIfc, &argv);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdReInvite fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetGroupCapability(const char *bssid, int *cap)
{
    if (bssid == NULL || cap == NULL) {
        LOGE("P2pReinvoke, bssid or cap is NULL");
        return WIFI_HAL_FAILED;
    }
    P2pDeviceInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        LOGE("Failed to init p2p device info");
        return WIFI_HAL_FAILED;
    }
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdP2pGetPeer(pMainIfc, bssid, &info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdP2pGetPeer fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    *cap = info.groupCapabilities;
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddService(const P2pServiceInfo *info)
{
    if (info == NULL) {
        LOGE("P2pAddService, input service info is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pAddService");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdServiceAdd(pMainIfc, info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdServiceAdd fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveService(const P2pServiceInfo *info)
{
    if (info == NULL) {
        LOGE("P2pRemoveService, input service info is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pRemoveService");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdServiceDel(pMainIfc, info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdServiceDel fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pReqServiceDiscovery(const char *bssid, const char *msg, char *disc, int size)
{
    if (bssid == NULL || msg == NULL || disc == NULL || size <= 0) {
        LOGE("P2pReqServiceDiscovery, bssid, msg and disc have NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pReqServiceDiscovery");
    char seq[WIFI_P2P_SERVER_DISCOVERY_SEQUENCE_LENGTH] = {0};
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdServDiscReq(pMainIfc, bssid, msg, seq, sizeof(seq));
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdServDiscReq fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    if (strncpy_s(disc, size, seq, strlen(seq)) != EOK) {
        LOGE("WpaP2pCliCmdServDiscReq return function result fail.");
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pCancelServiceDiscovery(const char *id)
{
    if (id == NULL) {
        LOGE("P2pReinvoke, id is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pCancelServiceDiscovery");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdServDiscCancelReq(pMainIfc, id);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdServDiscCancelReq fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetMiracastType(int type)
{
    LOGD("P2pSetMiracastType %{public}d", type);
    /* call vendor hal functions */
    WifiHalVendorInterface *pInterface = GetWifiHalVendorInterface();
    if (pInterface == NULL) {
        return WIFI_HAL_GET_VENDOR_HAL_FAILED;
    }
    HalVendorError err = pInterface->func.wifiSetMiracastMode(type);
    return ConvertErrorCode(err);
}

WifiErrorNo P2pRespServerDiscovery(P2pServDiscReqInfo *info)
{
    if (info == NULL) {
        LOGE("P2pRespServerDiscovery, info is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pRespServerDiscovery");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdRespServerDiscovery(pMainIfc, info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdRespServerDiscovery fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetServDiscExternal(int mode)
{
    LOGD("P2pSetServDiscExternal %{public}d", mode);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetServDiscExternal(pMainIfc, mode);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetServDiscExternal fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetPersistentReconnect(int mode)
{
    LOGD("P2pSetPersistentReconnect %{public}d", mode);
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetPersistentReconnect(pMainIfc, mode);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdSetPersistentReconnect fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetPeer(const char *bssid, P2pDeviceInfo *peerInfo)
{
    if (bssid == NULL || peerInfo == NULL) {
        LOGE("P2pGetPeer, bssid, peer info struct have NULL");
        return WIFI_HAL_FAILED;
    }
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdP2pGetPeer(pMainIfc, bssid, peerInfo);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdP2pGetPeer fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetFrequencies(int band, int *frequencies, int *size)
{
    if (frequencies == NULL || size == NULL) {
        LOGE("P2pGetFrequencies frequencies or size is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pGetFrequencies");
    return WIFI_HAL_NOT_SUPPORT;
}

WifiErrorNo P2pSetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    if (pConfig == NULL || size == 0) {
        LOGE("P2pSetGroupConfig() confs is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pSetGroupConfig()");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pWpaGroupConfigArgv conf;
    for (int i = 0; i < size; ++i) {
        if (memset_s(&conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
            return WIFI_HAL_FAILED;
        }
        conf.id = networkId;
        conf.param = pConfig[i].cfgParam;
        if (strncpy_s(conf.value, sizeof(conf.value), pConfig[i].cfgValue, strlen(pConfig[i].cfgValue)) != EOK) {
            return WIFI_HAL_FAILED;
        }
        P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdSetGroupConfig(pMainIfc, &conf);
        if (ret != P2P_SUP_ERRCODE_SUCCESS) {
            LOGE("wpaP2pCliCmdSetGroupConfig: Set networkId: %{public}d param %{public}d err.", conf.id, conf.param);
            return ConvertP2pErrCode(ret);
        }
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    if (pConfig == NULL || size == 0) {
        LOGE("P2pGetGroupConfig() confs is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pGetGroupConfig()");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pWpaGroupConfigArgv conf;
    for (int i = 0; i < size; ++i) {
        if (memset_s(&conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
            return WIFI_HAL_FAILED;
        }
        conf.id = networkId;
        conf.param = pConfig[i].cfgParam;
        P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdGetGroupConfig(pMainIfc, &conf);
        if (ret != P2P_SUP_ERRCODE_SUCCESS) {
            LOGE("wpaP2pCliCmdGetGroupConfig: Get NetworkId: %{public}d param %{public}d err.", conf.id, conf.param);
            return ConvertP2pErrCode(ret);
        }
        StrSafeCopy(pConfig[i].cfgValue, sizeof(pConfig[i].cfgValue), conf.value);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddNetwork(int *networkId)
{
    if (networkId == NULL) {
        LOGE("P2pAddNetwork() networkId is null");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pAddNetwork");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdAddNetwork(pMainIfc, networkId);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("WpaP2pCliCmdAddNetwork fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pHid2dConnect(Hid2dConnectInfo *info)
{
    if (info == NULL) {
        LOGE("P2pHid2dConnect, info is NULL");
        return WIFI_HAL_FAILED;
    }
    LOGD("P2pHid2dConnect");
    WifiWpaP2pInterface *pMainIfc = GetWifiWapP2pInterface();
    if (pMainIfc == NULL) {
        return WIFI_HAL_SUPPLICANT_NOT_INIT;
    }
    P2pSupplicantErrCode ret = pMainIfc->wpaP2pCliCmdHid2dConnect(pMainIfc, info);
    if (ret != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("wpaP2pHid2dCliCmdConnect fail, ret = %{public}d", ret);
        return ConvertP2pErrCode(ret);
    }
    return WIFI_HAL_SUCCESS;
}
