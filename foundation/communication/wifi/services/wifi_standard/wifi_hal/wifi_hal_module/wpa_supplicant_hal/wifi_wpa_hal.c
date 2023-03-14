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

#include "wifi_wpa_hal.h"
#include <unistd.h>
#include <poll.h>
#include "securec.h"
#include "wifi_wpa_common.h"
#include "utils/common.h" /* request for printf_decode to decode wpa's returned ssid info */
#include "wifi_hal_common_func.h"
#include "wifi_hal_callback.h"
#include "wifi_hal_struct.h"
#include "wifi_hal_p2p_struct.h"
#include "wifi_p2p_hal.h"

#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiWpaHal"

#define WPA_TRY_CONNECT_TIMES 20
#define WPA_TRY_CONNECT_SLEEP_TIME (100 * 1000) /* 100ms */
#define WPA_CMD_BUF_LEN 256
#define WPA_CMD_REPLY_BUF_SMALL_LEN 64
#define P2P_SERVICE_INFO_FIRST_SECTION 1
#define P2P_SERVICE_INFO_SECOND_SECTION 2
#define P2P_SERVICE_INFO_THIRD_SECTION 3
#define P2P_SERVICE_DISC_REQ_ONE 1
#define P2P_SERVICE_DISC_REQ_TWO 2
#define P2P_SERVICE_DISC_REQ_THREE 3
#define P2P_SERVICE_DISC_REQ_FOUR 4
#define P2P_SERVICE_DISC_REQ_FIVE 5
#define WPA_CB_SCAN_FAILED 1
#define WPA_CB_SCAN_OVER_OK 2
#define WPA_CB_CONNECTED 1
#define WPA_CB_DISCONNECTED 2
#define WPS_EVENT_PBC_OVERLAP "WPS-OVERLAP-DETECTED PBC session overlap"
#define REPLY_BUF_LENGTH 4096
#define CONNECTION_FULL_STATUS 17
#define CONNECTION_REJECT_STATUS 37

static WifiWpaInterface *g_wpaInterface = NULL;

static void DealP2pFindInfo(char *buf)
{
    if (buf == NULL || strlen(buf) < WIFI_MAC_LENGTH) {
        return;
    }
    P2pDeviceInfo info = {0};
    if (strncpy_s(info.srcAddress, sizeof(info.srcAddress), buf, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    buf += WIFI_MAC_LENGTH + 1;
    char *savedPtr = NULL;
    char *s = strtok_r(buf, " ", &savedPtr);
    while (s != NULL) {
        WpaKeyValue retMsg = INIT_WPA_KEY_VALUE;
        GetStrKeyVal(s, "=", &retMsg);
        if (strncmp(retMsg.key, "p2p_dev_addr", strlen("p2p_dev_addr")) == 0) {
            StrSafeCopy(info.p2pDeviceAddress, sizeof(info.p2pDeviceAddress), retMsg.value);
        } else if (strncmp(retMsg.key, "pri_dev_type", strlen("pri_dev_type")) == 0) {
            StrSafeCopy(info.primaryDeviceType, sizeof(info.primaryDeviceType), retMsg.value);
        } else if (strncmp(retMsg.key, "config_methods", strlen("config_methods")) == 0) {
            info.configMethods = Hex2Dec(retMsg.value);
        } else if (strncmp(retMsg.key, "dev_capab", strlen("dev_capab")) == 0) {
            info.deviceCapabilities = Hex2Dec(retMsg.value);
        } else if (strncmp(retMsg.key, "group_capab", strlen("group_capab")) == 0) {
            info.groupCapabilities = Hex2Dec(retMsg.value);
        } else if (strncmp(retMsg.key, "wfd_dev_info", strlen("wfd_dev_info")) == 0) {
            if (strlen(retMsg.value) != strlen("0x000000000000")) {
                LOGD("Unexpect wfd device info, it's return 6 uint8 array convert to hex string!");
            } else {
                StrSafeCopy(info.wfdDeviceInfo, sizeof(info.wfdDeviceInfo), retMsg.value);
                info.wfdLength = strlen(info.wfdDeviceInfo);
            }
        } else if (strncmp(retMsg.key, "name", strlen("name")) == 0) {
            unsigned len = strlen(retMsg.value);
            if (len < sizeof(retMsg.value) - 1 && retMsg.value[len - 1] != '\'') { /* special deal: name='xxx xxx' */
                s = strtok_r(NULL, "\'", &savedPtr);
                retMsg.value[len++] = ' ';
                StrSafeCopy(retMsg.value + len, sizeof(retMsg.value) - len, s);
            }
            TrimQuotationMark(retMsg.value, '\'');
            StrSafeCopy(info.deviceName, sizeof(info.deviceName), retMsg.value);
        }
        s = strtok_r(NULL, " ", &savedPtr);
    }
    P2pHalCbDeviceFound(&info);
    return;
}

static void DealP2pGoNegRequest(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    if (strncpy_s(macAddr, sizeof(macAddr), buf, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    const char *passId = strstr(buf, "dev_passwd_id=");
    if (passId == NULL) {
        LOGD("Not find dev_passwd_id");
        return;
    }
    short passwordId = atoi(passId + strlen("dev_passwd_id="));
    P2pHalCbGoNegotiationRequest(macAddr, passwordId);
    return;
}

static void DealGroupStartInfo(char *buf)
{
    if (buf == NULL) {
        return;
    }
    P2pGroupInfo conf = {0};
    if (memset_s(&conf, sizeof(conf), 0, sizeof(conf)) != EOK) {
        return;
    }
    if (strstr(buf, "[PERSISTENT]") != NULL) {
        conf.isPersistent = 1;
    }

    char *savedPtr = NULL;
    char *token = strtok_r(buf, " ", &savedPtr);
    while (token != NULL) {
        WpaKeyValue retMsg = INIT_WPA_KEY_VALUE;
        GetStrKeyVal(token, "=", &retMsg);
        if (strncmp(retMsg.key, "GO", strlen("GO")) == 0) {
            conf.isGo = 1;
        } else if (strncmp(retMsg.key, "freq", strlen("freq")) == 0) {
            conf.frequency = atoi(retMsg.value);
        } else if (strncmp(retMsg.key, "go_dev_addr", strlen("go_dev_addr")) == 0) {
            StrSafeCopy(conf.goDeviceAddress, sizeof(conf.goDeviceAddress), retMsg.value);
        } else if (strncmp(retMsg.key, "p2p-", strlen("p2p-")) == 0) {
            StrSafeCopy(conf.groupIfName, sizeof(conf.groupIfName), retMsg.key);
        } else if (strncmp(retMsg.key, "psk", strlen("psk")) == 0) {
            StrSafeCopy(conf.psk, sizeof(conf.psk), retMsg.value);
        } else if (strncmp(retMsg.key, "ssid", strlen("ssid")) == 0 ||
                   strncmp(retMsg.key, "passphrase", strlen("passphrase")) == 0) {
            unsigned len = strlen(retMsg.value);
            if (len < sizeof(retMsg.value) - 1 && retMsg.value[len - 1] != '\"') {
                token = strtok_r(NULL, "\"", &savedPtr);
                retMsg.value[len++] = ' ';
                StrSafeCopy(retMsg.value + len, sizeof(retMsg.value) - len, token);
            }
            TrimQuotationMark(retMsg.value, '\"');
            if (strncmp(retMsg.key, "ssid", strlen("ssid")) == 0) {
                StrSafeCopy(conf.ssid, sizeof(conf.ssid), retMsg.value);
                printf_decode((u8 *)conf.ssid, sizeof(conf.ssid), conf.ssid);
            } else {
                StrSafeCopy(conf.passphrase, sizeof(conf.passphrase), retMsg.value);
            }
        }
        token = strtok_r(NULL, " ", &savedPtr);
    }
    P2pHalCbGroupStarted(&conf);
    return;
}

static void DealServiceDiscRespEvent(char *buf)
{
    if (buf == NULL) {
        return;
    }
    P2pServDiscRespInfo info = {0};
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, " ", &savedPtr);
    int index = 0;
    while (token != NULL) {
        if (index == P2P_SERVICE_INFO_FIRST_SECTION) {
            if (strncpy_s(info.srcAddress, sizeof(info.srcAddress), token, strlen(token)) != EOK) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
        } else if (index == P2P_SERVICE_INFO_SECOND_SECTION) {
            info.updateIndicator = atoi(token);
        } else if (index == P2P_SERVICE_INFO_THIRD_SECTION) {
            unsigned len = strlen(token) + 1;
            if (len == 0) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
            if (info.tlvs != NULL) {
                free(info.tlvs);
                info.tlvs = NULL;
            }
            info.tlvs = (char *)calloc(len, sizeof(char));
            if (info.tlvs == NULL || strncpy_s(info.tlvs, len, token, len - 1) != EOK) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
        }
        index++;
        token = strtok_r(NULL, " ", &savedPtr);
    }
    P2pHalCbServiceDiscoveryResponse(&info);
    free(info.tlvs);
    info.tlvs = NULL;
    return;
}

static void DealP2pGroupRemove(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    char groupIfname[WIFI_P2P_GROUP_IFNAME_LENGTH + 1] = {0};
    const char *pos = strstr(buf, " ");
    if (pos == NULL || pos - buf > WIFI_P2P_GROUP_IFNAME_LENGTH) {
        LOGD("pos is %{public}s", ((pos == NULL) ? "NULL" : "bigger than ifname length"));
        return;
    }
    if (strncpy_s(groupIfname, sizeof(groupIfname), buf, pos - buf) != EOK) {
        return;
    }
    int flag = 0;
    if (strstr(buf, "GO") != NULL) {
        flag = 1;
    }
    P2pHalCbGroupRemoved(groupIfname, flag);
    ReleaseWpaP2pGroupInterface(groupIfname); /* remove group interface */
    return;
}

static void DealP2pConnectChanged(const char *buf, int type)
{
    if (buf == NULL) {
        return;
    }
    char devAddr[WIFI_MAC_LENGTH + 1] = {0};
    const char *pos = strstr(buf, "p2p_dev_addr=");
    if (pos == NULL) {
        return;
    }
    if (strncpy_s(devAddr, sizeof(devAddr), pos + strlen("p2p_dev_addr="), WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbStaConnectState(devAddr, type);
    return;
}

static void DealDeviceLostEvent(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    const char *peeraddr = strstr(buf, "p2p_dev_addr=");
    if (peeraddr == NULL) {
        return;
    }
    peeraddr += strlen("p2p_dev_addr=");
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    if (strncpy_s(macAddr, sizeof(macAddr), peeraddr, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbDeviceLost(macAddr);
    return;
}

static void DealInvitationReceived(char *buf, int type)
{
    if (buf == NULL) {
        return;
    }
    P2pInvitationInfo info = {0};
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    info.type = type;
    char *savedPtr = NULL;
    char *token = strtok_r(buf, " ", &savedPtr);
    while (token != NULL) {
        WpaKeyValue retMsg = INIT_WPA_KEY_VALUE;
        GetStrKeyVal(token, "=", &retMsg);
        if (strncmp(retMsg.key, "sa", strlen("sa")) == 0) {
            StrSafeCopy(info.srcAddress, sizeof(info.srcAddress), retMsg.value);
        } else if (strncmp(retMsg.key, "persistent", strlen("persistent")) == 0) {
            info.persistentNetworkId = atoi(retMsg.value);
        } else if (strncmp(retMsg.key, "freq", strlen("freq")) == 0) {
            info.operatingFrequency = atoi(retMsg.value);
        } else if (strncmp(retMsg.key, "go_dev_addr", strlen("go_dev_addr")) == 0) {
            StrSafeCopy(info.goDeviceAddress, sizeof(info.goDeviceAddress), retMsg.value);
        } else if (strncmp(retMsg.key, "bssid", strlen("bssid")) == 0) {
            StrSafeCopy(info.bssid, sizeof(info.bssid), retMsg.value);
        }
        token = strtok_r(NULL, " ", &savedPtr);
    }
    P2pHalCbInvitationReceived(&info);
    return;
}

static void DealInvitationResultEvent(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    const char *sta = strstr(buf, "status=");
    if (sta == NULL) {
        return;
    }
    int status = atoi(sta + strlen("status="));
    const char *bssidpos = strstr(sta, " ");
    if (bssidpos == NULL) {
        return;
    }
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    if (strncpy_s(macAddr, sizeof(macAddr), bssidpos + 1, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbInvitationResult(macAddr, status);
    return;
}

static void DealP2pGoNegotiationFailure(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    const char *sta = strstr(buf, "status=");
    if (sta == NULL) {
        return;
    }
    int status = atoi(sta + strlen("status="));
    P2pHalCbGoNegotiationFailure(status);
    return;
}

static void DealGroupFormationFailureEvent(const char *buf)
{
    if (buf == NULL) {
        return;
    }
    char reason[WIFI_P2P_GROUP_IFNAME_LENGTH + 1] = {0};
    char *reapos = strstr(buf, "reason=");
    if (reapos != NULL) {
        if (strncpy_s(reason, sizeof(reason), reapos + strlen("reason="), WIFI_P2P_GROUP_IFNAME_LENGTH) != EOK) {
            return;
        }
    }
    P2pHalCbGroupFormationFailure(reason);
    return;
}

static void DealProvDiscPbcReqEvent(const char *buf, unsigned long length)
{
    if (buf == NULL || length < strlen(P2P_EVENT_PROV_DISC_PBC_REQ) + WIFI_MAC_LENGTH) {
        return;
    }
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    const char *pos = buf + strlen(P2P_EVENT_PROV_DISC_PBC_REQ);
    if (strncpy_s(macAddr, sizeof(macAddr), pos, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbProvisionDiscoveryPbcRequest(macAddr);
    return;
}

static void DealProDiscPbcRespEvent(const char *buf, unsigned long length)
{
    if (buf == NULL || length < strlen(P2P_EVENT_PROV_DISC_PBC_RESP) + WIFI_MAC_LENGTH) {
        return;
    }
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    const char *pos = buf + strlen(P2P_EVENT_PROV_DISC_PBC_RESP);
    if (strncpy_s(macAddr, sizeof(macAddr), pos, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbProvisionDiscoveryPbcResponse(macAddr);
    return;
}

static void DealProDiscEnterPinEvent(const char *buf, unsigned long length)
{
    if (buf == NULL || length < strlen(P2P_EVENT_PROV_DISC_ENTER_PIN) + WIFI_MAC_LENGTH) {
        return;
    }
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    const char *pos = buf + strlen(P2P_EVENT_PROV_DISC_ENTER_PIN);
    if (strncpy_s(macAddr, sizeof(macAddr), pos, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    P2pHalCbProvisionDiscoveryEnterPin(macAddr);
    return;
}

static void DealProvDiscShowPinEvent(const char *buf, unsigned long length)
{
    if (buf == NULL || length < strlen(P2P_EVENT_PROV_DISC_SHOW_PIN) + WIFI_MAC_LENGTH + 1 + WIFI_PIN_CODE_LENGTH) {
        return;
    }
    const char *p = buf + strlen(P2P_EVENT_PROV_DISC_SHOW_PIN);
    char macAddr[WIFI_MAC_LENGTH + 1] = {0};
    char pinCode[WIFI_PIN_CODE_LENGTH + 1] = {0};
    if (strncpy_s(macAddr, sizeof(macAddr), p, WIFI_MAC_LENGTH) != EOK) {
        return;
    }
    p += WIFI_MAC_LENGTH + 1;
    if (strncpy_s(pinCode, sizeof(pinCode), p, WIFI_PIN_CODE_LENGTH) != EOK) {
        return;
    }
    P2pHalCbProvisionDiscoveryShowPin(macAddr, pinCode);
    return;
}

static void DealP2pServDiscReqEvent(char *buf)
{
    if (buf == NULL) {
        return;
    }
    P2pServDiscReqInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, " ", &savedPtr);
    int index = 0;
    while (token != NULL && index <= P2P_SERVICE_DISC_REQ_FIVE) {
        if (index == P2P_SERVICE_DISC_REQ_ONE) {
            info.freq = atoi(token);
        } else if (index == P2P_SERVICE_DISC_REQ_TWO) {
            if (strncpy_s(info.mac, sizeof(info.mac), token, strlen(token)) != EOK) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
        } else if (index == P2P_SERVICE_DISC_REQ_THREE) {
            info.dialogToken = atoi(token);
        } else if (index == P2P_SERVICE_DISC_REQ_FOUR) {
            info.updateIndic = atoi(token);
        } else if (index == P2P_SERVICE_DISC_REQ_FIVE) {
            unsigned len = strlen(token) + 1;
            if (len == 0) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
            if (info.tlvs != NULL) {
                free(info.tlvs);
                info.tlvs = NULL;
            }
            info.tlvs = (char *)calloc(len, sizeof(char));
            if (info.tlvs == NULL || strncpy_s(info.tlvs, len, token, len - 1) != EOK) {
                free(info.tlvs);
                info.tlvs = NULL;
                return;
            }
        }
        token = strtok_r(NULL, " ", &savedPtr);
        index++;
    }
    P2pHalCbServDiscReq(&info);
    free(info.tlvs);
    info.tlvs = NULL;
    return;
}

static int DealWpaP2pCallBackSubFun(char *p)
{
    if (p == NULL) {
        return -1;
    }
    if (strncmp(p, P2P_EVENT_DEVICE_FOUND, strlen(P2P_EVENT_DEVICE_FOUND)) == 0) {
        DealP2pFindInfo(p + strlen(P2P_EVENT_DEVICE_FOUND));
    } else if (strncmp(p, P2P_EVENT_DEVICE_LOST, strlen(P2P_EVENT_DEVICE_LOST)) == 0) {
        DealDeviceLostEvent(p);
    } else if (strncmp(p, P2P_EVENT_GO_NEG_REQUEST, strlen(P2P_EVENT_GO_NEG_REQUEST)) == 0) {
        DealP2pGoNegRequest(p + strlen(P2P_EVENT_GO_NEG_REQUEST));
    } else if (strncmp(p, P2P_EVENT_GO_NEG_SUCCESS, strlen(P2P_EVENT_GO_NEG_SUCCESS)) == 0) {
        P2pHalCbGoNegotiationSuccess();
    } else if (strncmp(p, P2P_EVENT_GO_NEG_FAILURE, strlen(P2P_EVENT_GO_NEG_FAILURE)) == 0) {
        DealP2pGoNegotiationFailure(p);
    } else if (strncmp(p, P2P_EVENT_INVITATION_RECEIVED, strlen(P2P_EVENT_INVITATION_RECEIVED)) == 0) {
        DealInvitationReceived(p, 0);
    } else if (strncmp(p, P2P_EVENT_INVITATION_ACCEPTED, strlen(P2P_EVENT_INVITATION_ACCEPTED)) == 0) {
        DealInvitationReceived(p, 1);
    } else if (strncmp(p, P2P_EVENT_INVITATION_RESULT, strlen(P2P_EVENT_INVITATION_RESULT)) == 0) {
        DealInvitationResultEvent(p);
    } else if (strncmp(p, P2P_EVENT_GROUP_FORMATION_SUCCESS, strlen(P2P_EVENT_GROUP_FORMATION_SUCCESS)) == 0) {
        P2pHalCbGroupFormationSuccess();
    } else if (strncmp(p, P2P_EVENT_GROUP_FORMATION_FAILURE, strlen(P2P_EVENT_GROUP_FORMATION_FAILURE)) == 0) {
        DealGroupFormationFailureEvent(p);
    } else if (strncmp(p, P2P_EVENT_GROUP_STARTED, strlen(P2P_EVENT_GROUP_STARTED)) == 0) {
        DealGroupStartInfo(p);
    } else if (strncmp(p, P2P_EVENT_GROUP_REMOVED, strlen(P2P_EVENT_GROUP_REMOVED)) == 0) {
        DealP2pGroupRemove(p + strlen(P2P_EVENT_GROUP_REMOVED));
    } else {
        return 1;
    }
    return 0;
}

static int WpaP2pCallBackFunc(char *p)
{
    if (p == NULL) {
        LOGI("recv notify message is NULL");
        return -1;
    }
    if (strncmp(p, P2P_EVENT_PROV_DISC_PBC_REQ, strlen(P2P_EVENT_PROV_DISC_PBC_REQ)) == 0) {
        DealProvDiscPbcReqEvent(p, strlen(p));
    } else if (strncmp(p, P2P_EVENT_PROV_DISC_PBC_RESP, strlen(P2P_EVENT_PROV_DISC_PBC_RESP)) == 0) {
        DealProDiscPbcRespEvent(p, strlen(p));
    } else if (strncmp(p, P2P_EVENT_PROV_DISC_ENTER_PIN, strlen(P2P_EVENT_PROV_DISC_ENTER_PIN)) == 0) {
        DealProDiscEnterPinEvent(p, strlen(p));
    } else if (strncmp(p, P2P_EVENT_PROV_DISC_SHOW_PIN, strlen(P2P_EVENT_PROV_DISC_SHOW_PIN)) == 0) {
        DealProvDiscShowPinEvent(p, strlen(p));
    } else if (strncmp(p, P2P_EVENT_FIND_STOPPED, strlen(P2P_EVENT_FIND_STOPPED)) == 0) {
        P2pHalCbFindStopped();
    } else if (strncmp(p, P2P_EVENT_SERV_DISC_RESP, strlen(P2P_EVENT_SERV_DISC_RESP)) == 0) {
        DealServiceDiscRespEvent(p);
    } else if (strncmp(p, P2P_EVENT_PROV_DISC_FAILURE, strlen(P2P_EVENT_PROV_DISC_FAILURE)) == 0) {
        P2pHalCbProvisionDiscoveryFailure();
    } else if (strncmp(p, AP_STA_DISCONNECTED, strlen(AP_STA_DISCONNECTED)) == 0) {
        DealP2pConnectChanged(p, 0);
    } else if (strncmp(p, AP_STA_CONNECTED, strlen(AP_STA_CONNECTED)) == 0) {
        DealP2pConnectChanged(p, 1);
    } else if (strncmp(p, P2P_EVENT_SERV_DISC_REQ, strlen(P2P_EVENT_SERV_DISC_REQ)) == 0) {
        DealP2pServDiscReqEvent(p);
    } else {
        if (DealWpaP2pCallBackSubFun(p) != 0) {
            return 1;
        }
    }
    return 0;
}

static void WpaCallBackFuncTwo(const char *p)
{
    if (p == NULL) {
        LOGI("recv notify message is NULL");
        return;
    }
    if (strncmp(p, WPA_EVENT_STATE_CHANGE, strlen(WPA_EVENT_STATE_CHANGE)) == 0) { /* wpa-state change */
        char *pstate = strstr(p, "state=");
        if (pstate != NULL) {
            pstate += strlen("state=");
            WifiHalCbNotifyWpaStateChange(atoi(pstate));
        }
    } else if (strncmp(p, WPA_EVENT_TEMP_DISABLED, strlen(WPA_EVENT_TEMP_DISABLED)) == 0) { /* Wrong Key */
        if (strstr(p, "reason=WRONG_KEY") != NULL) {
            WifiHalCbNotifyWrongKey(1);
        }
    } else if (strncmp(p, WPS_EVENT_PBC_OVERLAP, strlen(WPS_EVENT_PBC_OVERLAP)) == 0) { /* wps_pbc_overlap */
        WifiHalCbNotifyWpsOverlap(1);
    } else if (strncmp(p, WPS_EVENT_TIMEOUT, strlen(WPS_EVENT_TIMEOUT)) == 0) {
        WifiHalCbNotifyWpsTimeOut(1);
    } else if (strncmp(p, WPA_EVENT_AUTH_REJECT, strlen(WPA_EVENT_AUTH_REJECT)) == 0) {
        char *connectionStatus = strstr(p, "status_code=");
        if (connectionStatus != NULL) {
            connectionStatus += strlen("status_code=");
            int status = atoi(connectionStatus);
            if (status == CONNECTION_FULL_STATUS) {
                WifiHalCbNotifyConnectionFull(status);
            } else if (status == CONNECTION_REJECT_STATUS) {
                WifiHalCbNotifyConnectionReject(status);
            }
        }
    }
    return;
}

static void WpaCallBackFunc(const char *p)
{
    if (p == NULL) {
        LOGI("recv notify message is NULL");
        return;
    }
    if (strncmp(p, WPA_EVENT_SCAN_RESULTS, strlen(WPA_EVENT_SCAN_RESULTS)) == 0) {
        WifiHalCbNotifyScanEnd(WPA_CB_SCAN_OVER_OK);
    } else if (strncmp(p, WPA_EVENT_SCAN_FAILED, strlen(WPA_EVENT_SCAN_FAILED)) == 0) {
        WifiHalCbNotifyScanEnd(WPA_CB_SCAN_FAILED);
    } else if (strncmp(p, WPA_EVENT_CONNECTED, strlen(WPA_EVENT_CONNECTED)) == 0) { /* Connection notification */
        char *pid = strstr(p, "id=");
        char *pMacPos = strstr(p, "Connection to ");
        if (pid == NULL || pMacPos == NULL) {
            return;
        }
        pid += strlen("id=");
        pMacPos += strlen("Connection to ");
        int id = atoi(pid);
        if (id < 0) {
            id = -1;
        }
        WifiHalCbNotifyConnectChanged(WPA_CB_CONNECTED, id, pMacPos);
    } else if (strncmp(p, WPA_EVENT_DISCONNECTED, strlen(WPA_EVENT_DISCONNECTED)) == 0) { /* Disconnection */
        char *pBssid = strstr(p, "bssid=");
        if (pBssid == NULL) {
            return;
        }
        pBssid += strlen("bssid=");
        WifiHalCbNotifyConnectChanged(WPA_CB_DISCONNECTED, -1, pBssid);
    } else {
        WpaCallBackFuncTwo(p);
    }
    return;
}

static int MyWpaCtrlPending(struct wpa_ctrl *ctrl)
{
    if (ctrl == NULL) {
        return -1;
    }
    struct pollfd pfd;
    if (memset_s(&pfd, sizeof(pfd), 0, sizeof(pfd)) != EOK) {
        return -1;
    }
    pfd.fd = wpa_ctrl_get_fd(ctrl);
    pfd.events = POLLIN;
    int ret = poll(&pfd, 1, 100); /* 100 ms */
    if (ret < 0) {
        LOGE("poll failed!");
        return -1;
    }
    if (ret == 0) {
        return 0;
    }
    return 1;
}

static void *WpaReceiveCallback(void *arg)
{
    if (arg == NULL) {
        return NULL;
    }
    WifiWpaInterface *pWpa = arg;
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        LOGE("In wpa deal receive message thread, failed to calloc buff!");
        return NULL;
    }
    while (pWpa->threadRunFlag) {
        int ret = MyWpaCtrlPending(pWpa->wpaCtrl.pRecv);
        if (ret < 0) {
            LOGE("thread get event message failed!");
            break;
        } else if (ret == 0) {
            continue;
        }
        if (memset_s(buf, REPLY_BUF_LENGTH, 0, REPLY_BUF_LENGTH) != EOK) {
            LOGE("thread clear buffer cache failed!");
            break;
        }
        size_t len = REPLY_BUF_LENGTH - 1;
        ret = wpa_ctrl_recv(pWpa->wpaCtrl.pRecv, buf, &len);
        if (ret < 0) {
            LOGE("thread read event message failed!");
            break;
        }
        if (len <= 0) {
            continue;
        }
        /* Message format: IFACE=wlan0 <priority>EventType params... */
        char *p = strchr(buf, '>');
        if (p == NULL) {
            p = buf;
        } else {
            p++;
        }
        if (strncmp(p, WPA_EVENT_TERMINATING, strlen(WPA_EVENT_TERMINATING)) == 0) {
            break;
        }
        if (WpaP2pCallBackFunc(p) == 0) {
            continue;
        }
        WpaCallBackFunc(p);
    }
    free(buf);
    buf = NULL;
    LOGI("=====================thread exist=======================");
    return NULL;
}

static int WpaCliConnect(WifiWpaInterface *p)
{
    if (p == NULL) {
        return -1;
    }
    if (p->wpaCtrl.pSend != NULL) {
        return 0;
    }
    int count = WPA_TRY_CONNECT_TIMES;
    while (count-- > 0) {
        int ret = InitWpaCtrl(&p->wpaCtrl, "/data/misc/wifi/sockets/wpa");
        if (ret == 0) {
            LOGD("Global wpa interface connect successfully!");
            break;
        } else {
            LOGD("Init wpaCtrl failed: %{public}d", ret);
        }
        usleep(WPA_TRY_CONNECT_SLEEP_TIME);
    }
    if (count <= 0) {
        return -1;
    }
    p->threadRunFlag = 1;
    if (pthread_create(&p->tid, NULL, WpaReceiveCallback, p) != 0) {
        p->threadRunFlag = 0;
        ReleaseWpaCtrl(&p->wpaCtrl);
        LOGE("Create monitor thread failed!");
        return -1;
    }
    return 0;
}

static void WpaCliClose(WifiWpaInterface *p)
{
    if (p->tid != 0) {
        p->threadRunFlag = 0;
        pthread_join(p->tid, NULL);
        p->tid = 0;
    }
    ReleaseWpaCtrl(&p->wpaCtrl);
    return;
}

static int WpaCliAddIface(WifiWpaInterface *p, const AddInterfaceArgv *argv)
{
    if (p == NULL || argv == NULL) {
        return -1;
    }
    WpaIfaceInfo *info = p->ifaces;
    while (info != NULL) {
        if (strcmp(info->name, argv->name) == 0) {
            info->num += 1;
            return 0;
        }
        info = info->next;
    }
    info = (WpaIfaceInfo *)calloc(1, sizeof(WpaIfaceInfo));
    if (info == NULL) {
        return -1;
    }
    StrSafeCopy(info->name, sizeof(info->name), argv->name);
    char cmd[WPA_CMD_BUF_LEN] = {0};
    char buf[WPA_CMD_REPLY_BUF_SMALL_LEN] = {0};
    LOGI("WpaCliAddIface CMD: %{public}s", cmd);
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "INTERFACE_ADD %s\t%s", argv->name, argv->confName) < 0 ||
        WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        free(info);
        info = NULL;
        LOGI("WpaCliAddIface buf: %{public}s", buf);
        return -1;
    }
    info->num += 1;
    info->next = p->ifaces;
    p->ifaces = info;
    return 0;
}

static int WpaCliRemoveIface(WifiWpaInterface *p, const char *name)
{
    if (p == NULL || name == NULL) {
        return -1;
    }
    WpaIfaceInfo *prev = NULL;
    WpaIfaceInfo *info = p->ifaces;
    while (info != NULL) {
        if (strcmp(info->name, name) == 0) {
            info->num -= 1;
            break;
        }
        prev = info;
        info = info->next;
    }
    if (info == NULL || info->num > 0) {
        return 0;
    }
    char cmd[WPA_CMD_BUF_LEN] = {0};
    char buf[WPA_CMD_REPLY_BUF_SMALL_LEN] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "INTERFACE_REMOVE %s", name) < 0 ||
        WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        info->num += 1;
        return -1;
    }
    if (prev == NULL) {
        p->ifaces = info->next;
    } else {
        prev->next = info->next;
    }
    free(info);
    info = NULL;
    return 0;
}

WifiWpaInterface *GetWifiWapGlobalInterface(void)
{
    if (g_wpaInterface != NULL) {
        return g_wpaInterface;
    }
    g_wpaInterface = (WifiWpaInterface *)calloc(1, sizeof(WifiWpaInterface));
    if (g_wpaInterface == NULL) {
        LOGE("Failed to create wpa interface!");
        return NULL;
    }
    g_wpaInterface->wpaCliConnect = WpaCliConnect;
    g_wpaInterface->wpaCliClose = WpaCliClose;
    g_wpaInterface->wpaCliAddIface = WpaCliAddIface;
    g_wpaInterface->wpaCliRemoveIface = WpaCliRemoveIface;
    return g_wpaInterface;
}

void ReleaseWpaGlobalInterface(void)
{
    if (g_wpaInterface == NULL) {
        return;
    }
    WpaIfaceInfo *p = g_wpaInterface->ifaces;
    while (p != NULL) {
        WpaCliRemoveIface(g_wpaInterface, p->name);
        WpaIfaceInfo *q = p->next;
        free(p);
        p = q;
    }
    WpaCliClose(g_wpaInterface);
    free(g_wpaInterface);
    g_wpaInterface = NULL;
}

WpaCtrl *GetWpaCtrl(void)
{
    if (g_wpaInterface == NULL) {
        return NULL;
    }
    return &g_wpaInterface->wpaCtrl;
}

int ExcuteCmd(const char *szCmd)
{
    int ret = system(szCmd);
    if (ret == -1) {
        LOGE("system cmd %{public}s failed!", szCmd);
    } else {
        if (WIFEXITED(ret)) {
            if (WEXITSTATUS(ret) == 0) {
                return 0;
            }
            LOGE("system cmd %{public}s failed, return status %{public}d", szCmd, WEXITSTATUS(ret));
        } else {
            LOGE("system cmd %{public}s failed", szCmd);
        }
    }

    return -1;
}
