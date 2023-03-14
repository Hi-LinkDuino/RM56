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

#include "wifi_hal_base_interface.h"
#include "wifi_hal_sta_interface.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_chip_interface.h"
#include "wifi_hal_p2p_struct.h"
#include "wifi_hal_callback.h"
#include <time.h>
#include "securec.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiMockHalService"

/* --------- wifi_hal_base_interface.h   begin -------------*/
WifiErrorNo GetName(char *ifname, int32_t size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetType(int32_t *type)
{
    return WIFI_HAL_SUCCESS;
}

/* --------- wifi_hal_base_interface.h   end -------------*/

/* --------- wifi_hal_sta_interface.h   begin -------------*/
WifiErrorNo Start(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo Stop(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo ForceStop(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartSupplicant(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopSupplicant(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo ConnectSupplicant(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DisconnectSupplicant(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RequestToSupplicant(const unsigned char *buf, int32_t bufSize)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartScan(const ScanSettings *settings)
{
    WifiHalCbNotifyScanEnd(0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetScanInfos(ScanInfo *results, int *size)
{
    if (results == NULL || size == NULL || *size == 0) {
        return WIFI_HAL_SUCCESS;
    }
    if (strcpy_s(results[0].ssid, sizeof(results[0].ssid), "OHOS_testAp") != EOK ||
        strcpy_s(results[0].bssid, sizeof(results[0].bssid), "00:00:00:00:00:00") != EOK ||
        strcpy_s(results[0].flags, sizeof(results[0].flags), "CMCC") != EOK) {
        return WIFI_HAL_FAILED;
    }
    results[0].freq = 2412;
    results[0].infoElems = (ScanInfoElem *)calloc(1, sizeof(ScanInfoElem));
    if (results[0].infoElems == NULL) {
        return WIFI_HAL_FAILED;
    }
    results[0].infoElems[0].id = 0;
    results[0].infoElems[0].content = (char *)calloc(64, sizeof(char));
    if (results[0].infoElems[0].content == NULL) {
        free(results[0].infoElems);
        return WIFI_HAL_FAILED;
    }
    if (strcpy_s(results[0].infoElems[0].content, 64, "hello") != EOK) {
        free(results[0].infoElems[0].content);
        free(results[0].infoElems);
        return WIFI_HAL_FAILED;
    }
    results[0].infoElems[0].size = strlen(results[0].infoElems[0].content);
    results[0].ieSize = 1;
    results[0].timestamp = time(NULL);
    results[0].ant = NETWORK_PRIVATE;
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartPnoScan(const PnoScanSettings *settings)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopPnoScan(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo Connect(int networkId)
{
    WifiHalCbNotifyConnectChanged(0, 0, "00:00:00:00:00:00");
    WifiHalCbNotifyWrongKey(0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo Reconnect(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo Reassociate(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo Disconnect(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetPowerSave(int enable)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetStaCapabilities(int32_t *capabilities)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetDeviceMacAddress(unsigned char *mac, int *lenMac)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetFrequencies(int32_t band, int *frequencies, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetAssocMacAddr(const unsigned char *mac, int lenMac)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetScanningMacAddress(const unsigned char *mac, int lenMac)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DeauthLastRoamingBssid(const unsigned char *mac, int lenMac)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetSupportFeature(long *feature)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RunCmd(const char *ifname, int32_t cmdid, const unsigned char *buf, int32_t bufSize)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetWifiTxPower(int32_t power)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RemoveNetwork(int networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo AddNetwork(int *networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo EnableNetwork(int networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DisableNetwork(int networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetNetwork(int networkId, const SetNetworkConfig *confs, int size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SaveNetworkConfig(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartWpsPbcMode(const WifiWpsParam *param)
{
    WifiHalCbNotifyWpaStateChange(0);
    WifiHalCbNotifyWpsOverlap(0);
    WifiHalCbNotifyWpsTimeOut(0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartWpsPinMode(const WifiWpsParam *param, int *pinCode)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopWps(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetRoamingCapabilities(WifiRoamCapability *capability)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetRoamConfig(char **blocklist, int blocksize, char **trustlist, int trustsize)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo WpaSetCountryCode(const char *countryCode)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo WpaGetCountryCode(char *countryCode, int codeSize)
{
    if (countryCode == NULL) {
        return WIFI_HAL_FAILED;
    }
    for (int i = 0; i < codeSize && i < 2; ++i) {
        countryCode[i] = 'C';
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo WpaGetNetWork(GetNetworkConfig *conf)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo WpaAutoConnect(int enable)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo WpaBlocklistClear(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetNetworkList(WifiNetworkInfo *infos, int *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetConnectSignalInfo(const char *endBssid, WpaSignalInfo *info)
{
    return WIFI_HAL_SUCCESS;
}

/* --------- wifi_hal_sta_interface.h   end -------------*/

/* --------- wifi_hal_ap_interface.h   begin -------------*/
WifiErrorNo StartSoftAp(void)
{
    WifiHalCbApState("AP-ENABLED");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartHostapd(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StartHostapdHal(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopSoftAp(void)
{
    WifiHalCbApState("AP-DISABLED");
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopHostapd(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo StopHostapdHal(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetStaInfos(char *infos, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetCountryCode(const char *code)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetHostapdConfig(HostapdConfig *config)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetMacFilter(const unsigned char *mac, int lenMac)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DelMacFilter(const unsigned char *mac, int lenMac)
{
    char buff[1024] = {0};
    if (snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "AP-STA-CONNECTED %s", mac) < 0) {
        return WIFI_HAL_FAILED;
    }
    WifiHalCbStaJoin(buff);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo DisassociateSta(const unsigned char *mac, int lenMac)
{
    char buff[1024] = {0};
    if (snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "AP-STA-DISCONNECTED %s", mac) < 0) {
        return WIFI_HAL_FAILED;
    }
    WifiHalCbStaJoin(buff);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetValidFrequenciesForBand(int32_t band, int *frequencies, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

/* --------- wifi_hal_ap_interface.h   end -------------*/

/* --------- wifi_hal_chip_interface.h   begin -------------*/
WifiErrorNo GetWifiChip(uint8_t id, WifiChip *chip)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetWifiChipIds(uint8_t *ids, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetChipId(int32_t *id)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo CreateIface(int32_t type, WifiIface *iface)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetIface(const char *ifname, WifiIface *iface)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetIfaceNames(int32_t type, char *ifaces, int32_t size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RemoveIface(const char *ifname)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetCapabilities(uint32_t *capabilities)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetSupportedComboModes(int32_t *modes, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo ConfigComboModes(int32_t mode)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo GetComboModes(int32_t *id)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo RequestFirmwareDebugDump(unsigned char *bytes, int32_t *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo SetPowerMode(uint8_t mode)
{
    return WIFI_HAL_SUCCESS;
}

/* --------- wifi_hal_chip_interface.h   end -------------*/
/* --------- wifi_hal_p2p_interface.h    begin -----------*/
WifiErrorNo P2pStart(void)
{
    WifiP2pHalCbNotifyConnectSupplicant(0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStop(void)
{
    P2pHalCbConnectSupplicantFailed();
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pForceStop(void)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetRandomMac(int enable)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetDeviceName(const char *name)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetSsidPostfixName(const char *postfixName)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsDeviceType(const char *type)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsConfigMethods(const char *config)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetDeviceAddress(char *deviceAddress, int size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pFlush()
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pFlushService()
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSaveConfig()
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetupWpsPbc(const char *groupIfc, const char *address)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetupWpsPin(const char *groupIfc, const char *address, const char *pin, char *result, int resultLen)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveNetwork(int networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pListNetworks(P2pNetworkList *infoList)
{
    if (infoList == NULL) {
        return WIFI_HAL_FAILED;
    }
    infoList->infos = (P2pNetworkInfo *)calloc(1, sizeof(P2pNetworkInfo));
    if (infoList->infos == NULL) {
        return WIFI_HAL_FAILED;
    }
    infoList->infoNum = 1;
    infoList->infos[0].id = 0;
    if (strcpy_s(infoList->infos[0].ssid, sizeof(infoList->infos[0].ssid), "p2p_test") != EOK ||
        strcpy_s(infoList->infos[0].bssid, sizeof(infoList->infos[0].bssid), "00:00:00:00:00:00") != EOK ||
        strcpy_s(infoList->infos[0].flags, sizeof(infoList->infos[0].flags), "P2P-PERSISTENT") != EOK) {
        free(infoList->infos);
        infoList->infos = NULL;
        return WIFI_HAL_FAILED;
    }
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetGroupMaxIdle(const char *groupIfc, int time)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetPowerSave(const char *groupIfc, int enable)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWfdEnable(int enable)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWfdDeviceConfig(const char *config)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStartFind(int timeout)
{
    P2pDeviceInfo device;
    if (memset_s(&device, sizeof(device), 0, sizeof(device)) != EOK ||
        strcpy_s(device.srcAddress, sizeof(device.srcAddress), "00:00:00:00:00:00") != EOK ||
        strcpy_s(device.p2pDeviceAddress, sizeof(device.p2pDeviceAddress), "01:00:00:00:00:00") != EOK ||
        strcpy_s(device.deviceName, sizeof(device.deviceName), "p2p_test") != EOK ||
        strcpy_s(device.wfdDeviceInfo, sizeof(device.wfdDeviceInfo), "0x000000000000") != EOK) {
        return WIFI_HAL_FAILED;
    }
    device.wfdLength = strlen(device.wfdDeviceInfo);
    P2pHalCbDeviceFound(&device);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pStopFind()
{
    P2pHalCbDeviceLost("00:00:00:00:00:00");
    P2pHalCbFindStopped();
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetExtListen(int enable, int period, int interval)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetListenChannel(int channel, int regClass)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pConnect(const P2pConnectInfo *info)
{
    P2pHalCbStaConnectState("00:00:00:00:00:00", 1);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pCancelConnect()
{
    P2pHalCbStaConnectState("00:00:00:00:00:00", 0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pProvisionDiscovery(const char *peerBssid, int mode)
{
    P2pHalCbGoNegotiationRequest(peerBssid, 10086);
    P2pHalCbGoNegotiationSuccess();
    P2pHalCbGoNegotiationFailure(1);
    P2pHalCbProvisionDiscoveryPbcRequest("00:00:00:00:00:00");
    P2pHalCbProvisionDiscoveryPbcResponse("10:00:00:00:00:00");
    P2pHalCbProvisionDiscoveryEnterPin("20:00:00:00:00:00");
    P2pHalCbProvisionDiscoveryShowPin("30:00:00:00:00:00", "12345678");
    P2pHalCbProvisionDiscoveryFailure();
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddGroup(int isPersistent, int networkId)
{
    P2pHalCbGroupFormationSuccess();
    P2pHalCbGroupFormationFailure("unknown");
    P2pGroupInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK ||
        strcpy_s(info.groupIfName, sizeof(info.groupIfName), "p2p-dev-wlan1") != EOK ||
        strcpy_s(info.ssid, sizeof(info.ssid), "p2p_honor_suffix") != EOK ||
        strcpy_s(info.passphrase, sizeof(info.passphrase), "12345678") != EOK ||
        strcpy_s(info.goDeviceAddress, sizeof(info.goDeviceAddress), "01:00:00:00:00:00") != EOK) {
        return WIFI_HAL_FAILED;
    }
    P2pHalCbGroupStarted(&info);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveGroup(const char *interface)
{
    P2pHalCbGroupRemoved(interface, 0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pInvite(int persisitent, const char *peerBssid, const char *goBssid, const char *ifname)
{
    P2pInvitationInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK ||
        strcpy_s(info.srcAddress, sizeof(info.srcAddress), "00:00:00:00:00:00") != EOK ||
        strcpy_s(info.goDeviceAddress,
            sizeof(info.goDeviceAddress),
            ((goBssid == NULL) ? "00:00:00:00:00:00" : goBssid)) != EOK ||
        strcpy_s(info.bssid, sizeof(info.bssid), ((peerBssid == NULL) ? "00:00:00:00:00:00" : peerBssid)) != EOK) {
        return WIFI_HAL_FAILED;
    }
    P2pHalCbInvitationReceived(&info);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pReinvoke(int networkId, const char *bssid)
{
    P2pHalCbInvitationResult(bssid, 0);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetGroupCapability(const char *bssid, int *cap)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddService(const P2pServiceInfo *info)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRemoveService(const P2pServiceInfo *info)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pReqServiceDiscovery(const char *bssid, const char *msg)
{
    P2pServDiscReqInfo reqInfo;
    if (memset_s(&reqInfo, sizeof(reqInfo), 0, sizeof(reqInfo)) != EOK ||
        strcpy_s(reqInfo.mac, sizeof(reqInfo.mac), "00:00:00:00:00:00") != EOK) {
        return WIFI_HAL_FAILED;
    }
    P2pHalCbServDiscReq(&reqInfo);
    P2pServDiscRespInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK ||
        strcpy_s(info.srcAddress, sizeof(info.srcAddress), "00:00:00:00:00:00") != EOK) {
        return WIFI_HAL_FAILED;
    }
    info.tlvs = (char *)calloc(256, sizeof(char));
    if (info.tlvs == NULL || strcpy_s(info.tlvs, 256, "0123456789abcdefABCDEF") != EOK) {
        free(info.tlvs);
        return WIFI_HAL_FAILED;
    }
    P2pHalCbServiceDiscoveryResponse(&info);
    free(info.tlvs);
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pCancelServiceDiscovery(const char *id)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetMiracastType(int type)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetPersistentReconnect(int mode)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pRespServerDiscovery(P2pServDiscReqInfo *info)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetServDiscExternal(int mode)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetWpsSecondaryDeviceType(const char *type)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetPeer(const char *bssid, P2pDeviceInfo *peerInfo)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetFrequencies(int band, int *frequencies, int *size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pSetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pGetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pAddNetwork(int *networkId)
{
    return WIFI_HAL_SUCCESS;
}

WifiErrorNo P2pHid2dConnect(Hid2dConnectInfo *info)
{
    return WIFI_HAL_SUCCESS;
}
/*--------------- wifi_hal_p2p_interface.h end---------------------------*/
