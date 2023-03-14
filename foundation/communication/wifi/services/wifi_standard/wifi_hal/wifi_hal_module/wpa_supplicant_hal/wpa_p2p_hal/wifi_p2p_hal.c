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

#include "wifi_p2p_hal.h"
#include "wifi_wpa_common.h"
#include "utils/common.h" /* request for printf_decode to decode wpa's returned ssid info */
#include "wifi_log.h"
#include "securec.h"
#include "wifi_hal_callback.h"
#include "wifi_hal_struct.h"
#include "wifi_hal_common_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiWpaP2pHal"

#define P2P_REPLY_BUF_LENGTH 4096
#define P2P_REPLY_BUF_SMALL_LENGTH 64
#define P2P_CMD_BUF_LENGTH 256
#define P2P_PIN_CODE_LEN 8
#define P2P_REMOVE_ALL_NETWORK_CMD (-1)
#define P2P_CONNECT_JOIN_LEN 20
#define P2P_CONNECT_MODE_LEN 16
#define P2P_CONNECT_PERSISTENT_LEN 16
#define NETWORKS_LIST_ONE 1
#define NETWORKS_LIST_TWO 2
#define CMD_INT_MAX_LEN 10
#define P2P_GROUP_CONFIG_FIELD_NAME_LENGTH 32

WifiWpaP2pInterface *g_wpaP2pInterface = NULL;
WifiWpaP2pGroupInterface *g_wpaP2pGroupInterface = NULL; /* list head */

typedef struct P2pWpaNetworkField {
    P2pGroupConfigType field;
    char fieldName[P2P_GROUP_CONFIG_FIELD_NAME_LENGTH];
    int flag; /* 0 need add "" 1 no need */
} P2pWpaNetworkField;

static const P2pWpaNetworkField g_p2pWpaNetworkFields[] = {
    {GROUP_CONFIG_SSID, "ssid", 0},
    {GROUP_CONFIG_BSSID, "bssid", 1},
    {GROUP_CONFIG_PSK, "psk", 1},
    {GROUP_CONFIG_PROTO, "proto", 1},
    {GROUP_CONFIG_KEY_MGMT, "key_mgmt", 1},
    {GROUP_CONFIG_PAIRWISE, "pairwise", 1},
    {GROUP_CONFIG_AUTH_ALG, "auth_alg", 1},
    {GROUP_CONFIG_MODE, "mode", 1},
    {GROUP_CONFIG_DISABLED, "disabled", 1}
};

static P2pSupplicantErrCode WpaP2pCliCmdWpsPbc(WifiWpaP2pGroupInterface *this, const char *bssid)
{
    if (this == NULL || bssid == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PBC", this->groupIfname) < 0) {
        LOGE("snprintf error!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (strlen(bssid) > 0) {
        unsigned pos = strlen(cmd);
        if (snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " %s", bssid) < 0) {
            LOGE("snprintf err");
            return P2P_SUP_ERRCODE_FAILED;
        }
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("WPS_PBC command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (strncmp(buf, "FAIL-PBC-OVERLAP", strlen("FAIL-PBC-OVERLAP")) == 0) {
        LOGE("wps_pbc success, but result err: buf =%{public}s", buf);
        return P2P_SUP_ERRCODE_PBC_OVERLAP;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdWpsPin(WifiWpaP2pGroupInterface *this, P2pWpsPinDisplayArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if ((argv->mode != P2P_PIN_KEYPAD && argv->mode != P2P_PIN_DISPLAY) ||
        (argv->mode == P2P_PIN_KEYPAD && strlen(argv->pinCode) == 0)) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (argv->mode == P2P_PIN_KEYPAD) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PIN any %s", this->groupIfname,
            argv->pinCode);
    } else if (strlen(argv->bssid) == 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PIN any", this->groupIfname);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PIN %s", this->groupIfname, argv->bssid);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("WPS_PIN command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (argv->mode == P2P_PIN_DISPLAY) {
        if (strncpy_s(argv->pinCode, sizeof(argv->pinCode), buf, P2P_PIN_CODE_LEN) != EOK) {
            LOGE("Failed to copy return pin code!");
            return P2P_SUP_ERRCODE_FAILED;
        }
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetPowerSave(WifiWpaP2pGroupInterface *this, int enable)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SET ps %d", this->groupIfname, enable) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SET ps command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetGroupIdle(WifiWpaP2pGroupInterface *this, int timeout)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET p2p_group_idle %d", this->groupIfname,
        timeout) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET p2p_group_idle command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWpsName(WifiWpaP2pInterface *this, const char *name)
{
    if (this == NULL || name == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET device_name %s", this->ifName, name) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("set device_name command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWpsDeviceType(WifiWpaP2pInterface *this, const char *type)
{
    if (this == NULL || type == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET device_type %s", this->ifName, type) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    LOGI("Set device type CMD: %{public}s", cmd);
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("set device_type command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWpsSecDeviceType(WifiWpaP2pInterface *this, const char *type)
{
    if (this == NULL || type == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET sec_device_type %s", this->ifName, type) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("set sec_device_type command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWpsConfigMethods(WifiWpaP2pInterface *this, const char *methods)
{
    if (this == NULL || methods == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET config_methods %s", this->ifName, methods) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("set config_methods command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetSsidPostfixName(WifiWpaP2pInterface *this, const char *name)
{
    if (this == NULL || name == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SET ssid_postfix %s", this->ifName, name) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SET ssid_postfix command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdGetDeviceAddress(WifiWpaP2pInterface *this, char *devAddress, int size)
{
    if (this == NULL || devAddress == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s STATUS", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *buf = (char *)calloc(P2P_REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, P2P_REPLY_BUF_LENGTH) != 0) {
        LOGE("STATUS command failed!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *p = strstr(buf, "p2p_device_address=");
    if (p == NULL) {
        LOGE("Not find device address!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    p += strlen("p2p_device_address=");
    char *q = p;
    while (*q != '\0' && *q != '\n') {
        ++q;
    }
    if (strncpy_s(devAddress, size, p, q - p) != EOK) {
        LOGE("Failed to copy device address!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(buf);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdFlush(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_FLUSH", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_FLUSH command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdFlushService(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SERVICE_FLUSH", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERVICE_FLUSH command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdP2pStopFind(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_STOP_FIND", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_STOP_FIND command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdP2pRemoveGroup(WifiWpaP2pInterface *this, const char *groupname)
{
    if (this == NULL || groupname == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_GROUP_REMOVE %s", this->ifName, groupname) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_GROUP_REMOVE command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdP2pFound(WifiWpaP2pInterface *this, int timeout)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (timeout >= 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_FIND %d", this->ifName, timeout);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_FIND", this->ifName);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_FIND command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdRemoveNetwork(WifiWpaP2pInterface *this, int networkId)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (networkId < 0 && networkId != P2P_REMOVE_ALL_NETWORK_CMD) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (networkId == P2P_REMOVE_ALL_NETWORK_CMD) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s REMOVE_NETWORK all", this->ifName);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s REMOVE_NETWORK %d", this->ifName, networkId);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("REMOVE_NETWORK command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWfdEnable(WifiWpaP2pInterface *this, int enable)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET wifi_display %d", this->ifName, enable) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET wifi_display command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetWfdDeviceInfo(WifiWpaP2pInterface *this, const char *conf)
{
    if (this == NULL || conf == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    unsigned len = strlen(conf);
    if (len == 0) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    len += strlen("IFNAME=") + strlen(this->ifName) + 1 + strlen("WFD_SUBELEM_SET") + 1;
    char *cmd = (char *)calloc(len + 1, sizeof(char));
    if (cmd == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (snprintf_s(cmd, len + 1, len, "IFNAME=%s WFD_SUBELEM_SET %s", this->ifName, conf) < 0) {
        LOGE("snprintf err");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("WFD_SUBELEM_SET command failed!");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(cmd);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdExtListen(WifiWpaP2pInterface *this, int enable, int period, int interval)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (enable == 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_EXT_LISTEN", this->ifName);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_EXT_LISTEN %d %d", this->ifName, period,
            interval);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_EXT_LISTEN command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetListenChannel(WifiWpaP2pInterface *this, int channel, int regClass)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (regClass > 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SET listen_channel %d %d", this->ifName,
            channel, regClass);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SET listen_channel %d", this->ifName,
            channel);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SET listen_channel command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdCancelConnect(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_CANCEL", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_CANCEL command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdInvite(WifiWpaP2pInterface *this, const P2pHalInviteArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (strlen(argv->peerbssid) <= 0 || strlen(argv->gobssid) <= 0 || strlen(argv->ifname) <= 0) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_INVITE group=%s peer=%s go_dev_addr=%s",
        this->ifName, argv->ifname, argv->peerbssid, argv->gobssid) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_INVITE command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdReInvite(WifiWpaP2pInterface *this, const P2pHalReInviteArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (strlen(argv->peerbssid) <= 0) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_INVITE persistent=%d peer=%s", this->ifName,
        argv->networkId, argv->peerbssid) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_INVITE reinvite command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdServiceAdd(WifiWpaP2pInterface *this, const P2pServiceInfo *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    unsigned nameLen = strlen(argv->name);
    unsigned queryLen = strlen(argv->query);
    unsigned respLen = strlen(argv->resp);
    if ((argv->mode == 0 && nameLen == 0) || (argv->mode != 0 && (queryLen == 0 || respLen == 0))) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    unsigned cmdLen;
    if (argv->mode == 0) {
        cmdLen = strlen("P2P_SERVICE_ADD") + 1 + strlen("upnp") + 1 + CMD_INT_MAX_LEN + 1 + nameLen;
    } else {
        cmdLen = strlen("P2P_SERVICE_ADD") + 1 + strlen("bonjour") + 1 + queryLen + 1 + respLen;
    }
    cmdLen += strlen("IFNAME=") + strlen(this->ifName) + 1;
    char *cmd = (char *)calloc(cmdLen + 1, sizeof(char));
    if (cmd == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    int res;
    if (argv->mode == 0) {
        res = snprintf_s(cmd, cmdLen + 1, cmdLen, "IFNAME=%s P2P_SERVICE_ADD upnp %d %s", this->ifName, argv->version,
            argv->name);
    } else {
        res = snprintf_s(cmd, cmdLen + 1, cmdLen, "IFNAME=%s P2P_SERVICE_ADD bonjour %s %s", this->ifName, argv->query,
            argv->resp);
    }
    if (res < 0) {
        LOGE("snprintf err");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERVICE_ADD command failed!");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(cmd);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdServiceDel(WifiWpaP2pInterface *this, const P2pServiceInfo *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    unsigned nameLen = strlen(argv->name);
    unsigned queryLen = strlen(argv->query);
    if ((argv->mode == 0 && nameLen == 0) || (argv->mode == 1 && queryLen == 0)) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    unsigned cmdLen;
    if (argv->mode == 0) {
        cmdLen = strlen("P2P_SERVICE_DEL") + 1 + strlen("upnp") + 1 + CMD_INT_MAX_LEN + 1 + nameLen;
    } else {
        cmdLen = strlen("P2P_SERVICE_DEL") + 1 + strlen("bonjour") + 1 + queryLen;
    }
    cmdLen += strlen("IFNAME=") + strlen(this->ifName) + 1;
    char *cmd = (char *)calloc(cmdLen + 1, sizeof(char));
    if (cmd == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    int res;
    if (argv->mode == 0) {
        res = snprintf_s(cmd, cmdLen + 1, cmdLen, "IFNAME=%s P2P_SERVICE_DEL upnp %d %s", this->ifName, argv->version,
            argv->name);
    } else {
        res = snprintf_s(cmd, cmdLen + 1, cmdLen, "IFNAME=%s P2P_SERVICE_DEL bonjour %s", this->ifName, argv->query);
    }
    if (res < 0) {
        LOGE("snprintf err");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERVICE_DEL command failed!");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(cmd);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdServDiscReq(
    WifiWpaP2pInterface *this, const char *peerBssid, const char *tlvs, char *retSeq, unsigned size)
{
    if (this == NULL || peerBssid == NULL || tlvs == NULL || retSeq == NULL || size <= 0) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    unsigned bssidLen = strlen(peerBssid);
    unsigned tlvsLen = strlen(tlvs);
    if (bssidLen <= 0 || tlvsLen <= 0) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    unsigned cmdLen = strlen("P2P_SERV_DISC_REQ") + 1 + bssidLen + 1 + tlvsLen;
    cmdLen += strlen("IFNAME=") + strlen(this->ifName) + 1;
    char *cmd = (char *)calloc(cmdLen + 1, sizeof(char));
    if (cmd == NULL || snprintf_s(cmd, cmdLen + 1, cmdLen, "IFNAME=%s P2P_SERV_DISC_REQ %s %s", this->ifName, peerBssid,
        tlvs) < 0) {
        free(cmd);
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERV_DISC_REQ command failed!");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(cmd);
    if (strncpy_s(retSeq, size, buf, strlen(buf)) != EOK) {
        LOGE("Failed to copy response about service discovery sequence!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdServDiscCancelReq(WifiWpaP2pInterface *this, const char *val)
{
    if (this == NULL || val == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SERV_DISC_CANCEL_REQ %s", this->ifName, val) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERV_DISC_CANCEL_REQ command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdProvisionDiscovery(
    WifiWpaP2pInterface *this, const P2pProvisionDiscoveryArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (strlen(argv->peerbssid) <= 0) {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res = 0;
    if (argv->mode == HAL_WPS_METHOD_PBC) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_PROV_DISC %s pbc", this->ifName,
            argv->peerbssid);
    } else if (argv->mode == HAL_WPS_METHOD_DISPLAY) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_PROV_DISC %s display", this->ifName,
            argv->peerbssid);
    } else if (argv->mode == HAL_WPS_METHOD_KEYPAD) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_PROV_DISC %s keypad", this->ifName,
            argv->peerbssid);
    } else {
        return P2P_SUP_ERRCODE_INPUT_ERROR;
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_PROV_DISC command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdGroupAdd(WifiWpaP2pInterface *this, int isPersistent, int networkId, int freq)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    int res;
    if (isPersistent) {
        if (networkId < 0) {
            res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_GROUP_ADD persistent freq=%d",
                this->ifName, freq);
        } else {
            res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_GROUP_ADD persistent=%d freq=%d",
                this->ifName, networkId, freq);
        }
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_GROUP_ADD freq=%d", this->ifName, freq);
    }
    if (res < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_GROUP_ADD command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdStoreConfig(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SAVE_CONFIG", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SAVE_CONFIG command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static void GetHalNetworkInfos(char *buf, P2pNetworkInfo *info)
{
    if (buf == NULL || info == NULL) {
        return;
    }
    int len = strlen(buf);
    int start = 0; /* start pos */
    int end = 0;   /* end pos */
    int i = 0;
    while (end < len) {
        if (buf[end] != '\t') {
            ++end;
            continue;
        }
        buf[end] = '\0';
        if (i == 0) {
            info->id = atoi(buf);
        } else if (i == NETWORKS_LIST_ONE) {
            if (strcpy_s(info->ssid, sizeof(info->ssid), buf + start) != EOK) {
                break;
            }
            printf_decode((u8 *)info->ssid, sizeof(info->ssid), info->ssid);
        } else if (i == NETWORKS_LIST_TWO) {
            if (strcpy_s(info->bssid, sizeof(info->bssid), buf + start) != EOK) {
                break;
            }
            start = end + 1;
            if (strcpy_s(info->flags, sizeof(info->flags), buf + start) != EOK) {
                break;
            }
            break;
        }
        ++i;
        end++;
        start = end;
    }
    return;
}

static P2pSupplicantErrCode WpaP2pCliCmdNetworkList(WifiWpaP2pInterface *this, P2pNetworkList *infoList)
{
    if (this == NULL || infoList == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s LIST_NETWORKS", this->ifName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *buf = (char *)calloc(P2P_REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, P2P_REPLY_BUF_LENGTH) != 0) {
        LOGE("LIST_NETWORKS command failed!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *token = strstr(buf, "\n"); /* skip first line */
    if (token == NULL) {
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *tmpPos = token + 1;
    while ((tmpPos = strstr(tmpPos, "\n")) != NULL) {
        infoList->infoNum += 1;
        ++tmpPos;
    }
    if (infoList->infoNum <= 0) {
        free(buf);
        return P2P_SUP_ERRCODE_SUCCESS;
    }
    infoList->infos = (P2pNetworkInfo *)calloc(infoList->infoNum, sizeof(P2pNetworkInfo));
    if (infoList->infos == NULL) {
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *tmpBuf = token + 1;
    char *savedPtr = NULL;
    token = strtok_r(tmpBuf, "\n", &savedPtr);
    int index = 0;
    while (token != NULL) {
        if (index >= infoList->infoNum) {
            break;
        }
        GetHalNetworkInfos(token, &infoList->infos[index]);
        index++;
        token = strtok_r(NULL, "\n", &savedPtr);
    }
    free(buf);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdConnect(WifiWpaP2pInterface *this, P2pConnectInfo *info)
{
    if (this == NULL || info == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    char join[P2P_CONNECT_JOIN_LEN] = {0};
    char mode[P2P_CONNECT_MODE_LEN] = {0};
    char persistent[P2P_CONNECT_PERSISTENT_LEN] = {0};
    if (info->mode != 0) {
        StrSafeCopy(join, sizeof(join), " join");
    } else {
        if (snprintf_s(join, sizeof(join), sizeof(join) - 1, " go_intent=%d", info->goIntent) < 0) {
            LOGE("snprintf err");
            return P2P_SUP_ERRCODE_FAILED;
        }
    }
    if (info->provdisc == HAL_WPS_METHOD_DISPLAY) {
        StrSafeCopy(mode, sizeof(mode), " display");
    } else if (info->provdisc == HAL_WPS_METHOD_KEYPAD) {
        StrSafeCopy(mode, sizeof(mode), " keypad");
    } else if (info->provdisc == HAL_WPS_METHOD_PBC && strlen(info->pin) == 0) {
        StrSafeCopy(info->pin, sizeof(info->pin), "pbc");
    } else {
        LOGD("Mode value is invalid %{public}d", info->provdisc);
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_CONNECT %s %s%s%s%s", this->ifName,
        info->peerDevAddr, info->pin, mode, persistent, join) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_CONNECT command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (strncmp(buf, "FAIL", strlen("FAIL")) == 0) {
        LOGE("P2p connect return %{public}s", buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (info->provdisc == HAL_WPS_METHOD_DISPLAY && strcmp(info->pin, "pin") == 0) {
        if (strncpy_s(info->pin, sizeof(info->pin), buf, strlen(buf)) != EOK) {
            LOGE("Failed to copy response pin code info!");
            return P2P_SUP_ERRCODE_FAILED;
        }
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetPersistentReconnect(WifiWpaP2pInterface *this, int status)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (status != 0 && status != 1) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET persistent_reconnect %d",
        this->ifName, status) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET persistent_reconnect command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdRespServerDiscovery(WifiWpaP2pInterface *this, P2pServDiscReqInfo *info)
{
    if (this == NULL || info == NULL || info->tlvs == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    /* info.tlvs may be very large, so here we request a buffer for input the message */
    unsigned len = strlen("P2P_SERV_DISC_RESP") + 1 + CMD_INT_MAX_LEN + 1 + strlen(info->mac) + 1 + CMD_INT_MAX_LEN +
                   1 + strlen(info->tlvs) + strlen("IFNAME=") + strlen(this->ifName) + 1;
    char *cmd = (char *)calloc(len + 1, sizeof(char));
    if (cmd == NULL || snprintf_s(cmd, len + 1, len, "IFNAME=%s P2P_SERV_DISC_RESP %d %s %d %s", this->ifName,
        info->freq, info->mac, info->dialogToken, info->tlvs) < 0) {
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERV_DISC_RESP command failed!");
        free(cmd);
        return P2P_SUP_ERRCODE_FAILED;
    }
    free(cmd);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetServDiscExternal(WifiWpaP2pInterface *this, int mode)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_SERV_DISC_EXTERNAL %d", this->ifName, mode) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("P2P_SERV_DISC_EXTERNAL command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode SetP2pDeviceRandomMacAddr(WifiWpaP2pInterface *this, int mode)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET p2p_device_random_mac_addr %d", this->ifName,
        mode) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET p2p_device_random_mac_addr command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode SetP2pInterfaceRandomMacAddr(WifiWpaP2pInterface *this, int mode)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET p2p_interface_random_mac_addr %d", this->ifName,
        mode) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET p2p_interface_random_mac_addr command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode CheckDriverRandomCapcb(WifiWpaP2pInterface *this)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s DRIVER_FLAGS", this->ifName) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *buf = (char *)calloc(P2P_REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, P2P_REPLY_BUF_LENGTH) != 0) {
        LOGE("DRIVER_FLAGS command failed!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    P2pSupplicantErrCode ret;
    if (strstr(buf, "DEDICATED_P2P_DEVICE") != NULL) {
        ret = P2P_SUP_ERRCODE_SUCCESS;
    } else {
        ret = P2P_SUP_ERRCODE_FAILED;
    }
    free(buf);
    return ret;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetRandomMac(WifiWpaP2pInterface *this, int mode)
{
    if (this == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (CheckDriverRandomCapcb(this) != P2P_SUP_ERRCODE_SUCCESS) {
        LOGE("Dedicated P2P device doesn't support MAC randomization");
        return P2P_SUP_ERRCODE_INVALID;
    }
    if (SetP2pDeviceRandomMacAddr(this, mode) != P2P_SUP_ERRCODE_SUCCESS ||
        SetP2pInterfaceRandomMacAddr(this, mode) != P2P_SUP_ERRCODE_SUCCESS) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdP2pGetPeer(
    WifiWpaP2pInterface *this, const char *bssid, P2pDeviceInfo *peerInfo)
{
    if (this == NULL || bssid == NULL || peerInfo == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s P2P_PEER %s", this->ifName, bssid) < 0) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *buf = (char *)calloc(P2P_REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, P2P_REPLY_BUF_LENGTH) != 0) {
        LOGE("P2P_PEER command failed!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (strstr(buf, "\n") == NULL) {
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, "\n", &savedPtr);
    StrSafeCopy(peerInfo->p2pDeviceAddress, sizeof(peerInfo->p2pDeviceAddress), token); /* copy first line */
    while (token != NULL) {
        WpaKeyValue retMsg = INIT_WPA_KEY_VALUE;
        GetStrKeyVal(token, "=", &retMsg);
        if (strncmp(retMsg.key, "pri_dev_type", strlen("pri_dev_type")) == 0) {
            StrSafeCopy(peerInfo->primaryDeviceType, sizeof(peerInfo->primaryDeviceType), retMsg.value);
        } else if (strncmp(retMsg.key, "device_name", strlen("device_name")) == 0) {
            StrSafeCopy(peerInfo->deviceName, sizeof(peerInfo->deviceName), retMsg.value);
        } else if (strncmp(retMsg.key, "config_methods", strlen("config_methods")) == 0) {
            peerInfo->configMethods = Hex2Dec(retMsg.value);
        } else if (strncmp(retMsg.key, "dev_capab", strlen("dev_capab")) == 0) {
            peerInfo->deviceCapabilities = Hex2Dec(retMsg.value);
        } else if (strncmp(retMsg.key, "group_capab", strlen("group_capab")) == 0) {
            peerInfo->groupCapabilities = Hex2Dec(retMsg.value);
        }

        token = strtok_r(NULL, "\n", &savedPtr);
    }
    free(buf);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static int CheckValidGroupConfigField(const P2pWpaGroupConfigArgv *argv)
{
    int pos = -1;
    for (unsigned i = 0; i < sizeof(g_p2pWpaNetworkFields) / sizeof(g_p2pWpaNetworkFields[0]); ++i) {
        if (g_p2pWpaNetworkFields[i].field == argv->param) {
            pos = i;
            break;
        }
    }
    return pos;
}

static P2pSupplicantErrCode WpaP2pCliCmdSetGroupConfig(WifiWpaP2pInterface *this, const P2pWpaGroupConfigArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    int pos = CheckValidGroupConfigField(argv);
    if (pos < 0) {
        LOGE("unsupported param: %{public}d", argv->param);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    int res;
    if (g_p2pWpaNetworkFields[pos].flag == 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET_NETWORK %d %s \"%s\"", this->ifName,
            argv->id, g_p2pWpaNetworkFields[pos].fieldName, argv->value);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET_NETWORK %d %s %s", this->ifName, argv->id,
            g_p2pWpaNetworkFields[pos].fieldName, argv->value);
    }
    if (res < 0) {
        LOGE("Internal error, set request message failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("SET_NETWORK command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdGetGroupConfig(WifiWpaP2pInterface *this, P2pWpaGroupConfigArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    int pos = CheckValidGroupConfigField(argv);
    if (pos < 0) {
        LOGD("unsupported param: %{public}d", argv->param);
        return P2P_SUP_ERRCODE_FAILED;
    }
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s GET_NETWORK %d %s", this->ifName, argv->id,
        g_p2pWpaNetworkFields[pos].fieldName) < 0) {
        LOGE("snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }
    char *buf = (char *)calloc(P2P_REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        LOGE("Failed to init response buffer!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, P2P_REPLY_BUF_LENGTH) != 0) {
        LOGE("GET_NETWORK command failed!");
        free(buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    StrSafeCopy(argv->value, sizeof(argv->value), buf);
    free(buf);
    if (argv->param == GROUP_CONFIG_SSID) {
        TrimQuotationMark(argv->value, '\"');
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pCliCmdAddNetwork(WifiWpaP2pInterface *this, int *networkId)
{
    if (this == NULL || networkId == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }
    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s ADD_NETWORK", this->ifName) < 0) {
        LOGE("snprintf error");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("ADD_NETWORK command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    *networkId = atoi(buf);
    LOGD("WpaP2pCliCmdAddNetwork: buf = %{public}s networkId = %{public}d", buf, *networkId);
    return P2P_SUP_ERRCODE_SUCCESS;
}

static P2pSupplicantErrCode WpaP2pHid2dCliCmdConnect(WifiWpaP2pInterface *this, Hid2dConnectInfo *info)
{
    if (this == NULL || info == NULL) {
        return P2P_SUP_ERRCODE_INVALID;
    }

    char buf[P2P_REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[P2P_CMD_BUF_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s MAGICLINK \"%s\"\n%s\n\"%s\"\n%d", this->ifName,
        info->ssid, info->bssid, info->passphrase, info->frequency) < 0) {
        LOGE("hid2d connect snprintf err");
        return P2P_SUP_ERRCODE_FAILED;
    }

    LOGI("hid2d_connect, frequency = %{public}d", info->frequency);
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("hid2d_connect command failed!");
        return P2P_SUP_ERRCODE_FAILED;
    }
    if (strncmp(buf, "FAIL", strlen("FAIL")) == 0) {
        LOGE("P2p hid2d_connect return %{public}s", buf);
        return P2P_SUP_ERRCODE_FAILED;
    }
    return P2P_SUP_ERRCODE_SUCCESS;
}

static void InitGlobalWpaP2pFunc(void)
{
    g_wpaP2pInterface->wpaP2pCliCmdSetWpsName = WpaP2pCliCmdSetWpsName;
    g_wpaP2pInterface->wpaP2pCliCmdSetSsidPostfixName = WpaP2pCliCmdSetSsidPostfixName;
    g_wpaP2pInterface->wpaP2pCliCmdSetWpsDeviceType = WpaP2pCliCmdSetWpsDeviceType;
    g_wpaP2pInterface->wpaP2pCliCmdSetWpsSecDeviceType = WpaP2pCliCmdSetWpsSecDeviceType;
    g_wpaP2pInterface->wpaP2pCliCmdSetWpsConfigMethods = WpaP2pCliCmdSetWpsConfigMethods;
    g_wpaP2pInterface->wpaP2pCliCmdGetDeviceAddress = WpaP2pCliCmdGetDeviceAddress;
    g_wpaP2pInterface->wpaP2pCliCmdFlush = WpaP2pCliCmdFlush;
    g_wpaP2pInterface->wpaP2pCliCmdFlushService = WpaP2pCliCmdFlushService;
    g_wpaP2pInterface->wpaP2pCliCmdP2pStopFind = WpaP2pCliCmdP2pStopFind;
    g_wpaP2pInterface->wpaP2pCliCmdP2pRemoveGroup = WpaP2pCliCmdP2pRemoveGroup;
    g_wpaP2pInterface->wpaP2pCliCmdP2pFound = WpaP2pCliCmdP2pFound;
    g_wpaP2pInterface->wpaP2pCliCmdRemoveNetwork = WpaP2pCliCmdRemoveNetwork;
    g_wpaP2pInterface->wpaP2pCliCmdSetWfdEnable = WpaP2pCliCmdSetWfdEnable;
    g_wpaP2pInterface->wpaP2pCliCmdSetWfdDeviceInfo = WpaP2pCliCmdSetWfdDeviceInfo;
    g_wpaP2pInterface->wpaP2pCliCmdExtListen = WpaP2pCliCmdExtListen;
    g_wpaP2pInterface->wpaP2pCliCmdSetListenChannel = WpaP2pCliCmdSetListenChannel;
    g_wpaP2pInterface->wpaP2pCliCmdCancelConnect = WpaP2pCliCmdCancelConnect;
    g_wpaP2pInterface->wpaP2pCliCmdInvite = WpaP2pCliCmdInvite;
    g_wpaP2pInterface->wpaP2pCliCmdReInvite = WpaP2pCliCmdReInvite;
    g_wpaP2pInterface->wpaP2pCliCmdServiceAdd = WpaP2pCliCmdServiceAdd;
    g_wpaP2pInterface->wpaP2pCliCmdServiceDel = WpaP2pCliCmdServiceDel;
    g_wpaP2pInterface->wpaP2pCliCmdServDiscReq = WpaP2pCliCmdServDiscReq;
    g_wpaP2pInterface->wpaP2pCliCmdServDiscCancelReq = WpaP2pCliCmdServDiscCancelReq;
    g_wpaP2pInterface->wpaP2pCliCmdProvisionDiscovery = WpaP2pCliCmdProvisionDiscovery;
    g_wpaP2pInterface->wpaP2pCliCmdGroupAdd = WpaP2pCliCmdGroupAdd;
    g_wpaP2pInterface->wpaP2pCliCmdStoreConfig = WpaP2pCliCmdStoreConfig;
    g_wpaP2pInterface->wpaP2pCliCmdNetworkList = WpaP2pCliCmdNetworkList;
    g_wpaP2pInterface->wpaP2pCliCmdConnect = WpaP2pCliCmdConnect;
    g_wpaP2pInterface->wpaP2pCliCmdSetPersistentReconnect = WpaP2pCliCmdSetPersistentReconnect;
    g_wpaP2pInterface->wpaP2pCliCmdRespServerDiscovery = WpaP2pCliCmdRespServerDiscovery;
    g_wpaP2pInterface->wpaP2pCliCmdSetServDiscExternal = WpaP2pCliCmdSetServDiscExternal;
    g_wpaP2pInterface->wpaP2pCliCmdSetRandomMac = WpaP2pCliCmdSetRandomMac;
    g_wpaP2pInterface->wpaP2pCliCmdP2pGetPeer = WpaP2pCliCmdP2pGetPeer;
    g_wpaP2pInterface->wpaP2pCliCmdSetGroupConfig = WpaP2pCliCmdSetGroupConfig;
    g_wpaP2pInterface->wpaP2pCliCmdGetGroupConfig = WpaP2pCliCmdGetGroupConfig;
    g_wpaP2pInterface->wpaP2pCliCmdAddNetwork = WpaP2pCliCmdAddNetwork;
    g_wpaP2pInterface->wpaP2pCliCmdHid2dConnect = WpaP2pHid2dCliCmdConnect;
    return;
}

WifiWpaP2pInterface *GetWifiWapP2pInterface()
{
    if (g_wpaP2pInterface != NULL) {
        return g_wpaP2pInterface;
    }
    g_wpaP2pInterface = (WifiWpaP2pInterface *)calloc(1, sizeof(WifiWpaP2pInterface));
    if (g_wpaP2pInterface == NULL) {
        LOGE("alloc memory for p2p interface failed!");
        return NULL;
    }
#ifdef PRODUCT_RK
    StrSafeCopy(g_wpaP2pInterface->ifName, sizeof(g_wpaP2pInterface->ifName), "wlan0");
#else
    StrSafeCopy(g_wpaP2pInterface->ifName, sizeof(g_wpaP2pInterface->ifName), "p2p0");
#endif
    InitGlobalWpaP2pFunc();
    return g_wpaP2pInterface;
}

void RelesaeWpaP2pInterface(void)
{
    while (g_wpaP2pGroupInterface != NULL) {
        ReleaseWpaP2pGroupInterface(g_wpaP2pGroupInterface->groupIfname);
    }
    if (g_wpaP2pInterface != NULL) {
        free(g_wpaP2pInterface);
        g_wpaP2pInterface = NULL;
    }
}

WifiErrorNo ConvertP2pErrCode(P2pSupplicantErrCode code)
{
    switch (code) {
        case P2P_SUP_ERRCODE_SUCCESS:
            return WIFI_HAL_SUCCESS;
        case P2P_SUP_ERRCODE_PBC_OVERLAP:
            return WIFI_HAL_PBC_OVERLAP;
        case P2P_SUP_ERRCODE_INPUT_ERROR:
            return WIFI_HAL_INVALID_PARAM;
        default:
            return WIFI_HAL_FAILED;
    }
}

WifiWpaP2pGroupInterface *GetWifiWpaP2pGroupInterface(const char *groupIfc)
{
    if (groupIfc == NULL) {
        return NULL;
    }
    WifiWpaP2pGroupInterface *p = g_wpaP2pGroupInterface;
    while (p != NULL) {
        if (strcmp(p->groupIfname, groupIfc) == 0) {
            return p;
        }
        p = p->next;
    }
    p = (WifiWpaP2pGroupInterface *)calloc(1, sizeof(WifiWpaP2pGroupInterface));
    if (p == NULL) {
        return NULL;
    }
    if (strcpy_s(p->groupIfname, sizeof(p->groupIfname), groupIfc) != EOK) {
        LOGE("Failed to save group iface!");
        free(p);
        return NULL;
    }
    p->wpaP2pCliCmdWpsPbc = WpaP2pCliCmdWpsPbc;
    p->wpaP2pCliCmdWpsPin = WpaP2pCliCmdWpsPin;
    p->wpaP2pCliCmdSetPowerSave = WpaP2pCliCmdSetPowerSave;
    p->wpaP2pCliCmdSetGroupIdle = WpaP2pCliCmdSetGroupIdle;
    p->next = g_wpaP2pGroupInterface;
    g_wpaP2pGroupInterface = p;
    return p;
}

void ReleaseWpaP2pGroupInterface(const char *groupIfc)
{
    if (groupIfc == NULL) {
        return;
    }
    WifiWpaP2pGroupInterface *p = g_wpaP2pGroupInterface;
    WifiWpaP2pGroupInterface *q = p;
    while (p != NULL) {
        if (strcmp(p->groupIfname, groupIfc) == 0) {
            break;
        }
        q = p;
        p = p->next;
    }
    if (p == NULL) {
        return;
    }
    if (p == g_wpaP2pGroupInterface) {
        g_wpaP2pGroupInterface = p->next;
    } else {
        q->next = p->next;
    }
    free(p);
    return;
}
