/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "wifi_supplicant_hal.h"
#include <poll.h>
#include <pthread.h>
#include "wifi_hal_callback.h"
#include "securec.h"
#include "wifi_log.h"
#include "wifi_wpa_common.h"
#include "utils/common.h" /* request for printf_decode to decode wpa's returned ssid info */
#include "wifi_hal_common_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiWpaStaHal"

#define FAIL_BUSY 2
#define SCAN_INFO_NONE 0
#define SCAN_INFO_ONE 1
#define SCAN_INFO_TWO 2
#define SCAN_INFO_THREE 3
#define FAIL_PBC_OVERLAP_RETUEN 3
#define CMD_BUFFER_SIZE 1024
#define REPLY_BUF_LENGTH 4096
#define REPLY_BUF_SMALL_LENGTH 64
#define CMD_FREQ_MAX_LEN 8

const int QUOTATION_MARKS_FLAG_YES = 0;
const int QUOTATION_MARKS_FLAG_NO = 1;

WifiWpaStaInterface *g_wpaStaInterface = NULL;

static WpaSsidField g_wpaSsidFields[] = {
    {DEVICE_CONFIG_SSID, "ssid", QUOTATION_MARKS_FLAG_YES},
    {DEVICE_CONFIG_PSK, "psk", QUOTATION_MARKS_FLAG_YES},
    {DEVICE_CONFIG_KEYMGMT, "key_mgmt", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_PRIORITY, "priority", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_SCAN_SSID, "scan_ssid", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_EAP, "eap", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_IDENTITY, "identity", QUOTATION_MARKS_FLAG_YES},
    {DEVICE_CONFIG_PASSWORD, "password", QUOTATION_MARKS_FLAG_YES},
    {DEVICE_CONFIG_BSSID, "bssid", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_AUTH_ALGORITHMS, "auth_alg", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_WEP_KEY_IDX, "wep_tx_keyidx", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_WEP_KEY_0, "wep_key0", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_WEP_KEY_1, "wep_key1", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_WEP_KEY_2, "wep_key2", QUOTATION_MARKS_FLAG_NO},
    {DEVICE_CONFIG_WEP_KEY_3, "wep_key3", QUOTATION_MARKS_FLAG_NO}
};

static int WpaCliCmdStatus(WifiWpaStaInterface *this, struct WpaHalCmdStatus *pcmd)
{
    if (this == NULL || pcmd == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s STATUS", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (WpaCliCmd(cmd, buf, REPLY_BUF_LENGTH) != 0) {
        free(buf);
        return -1;
    }
    char *savedPtr = NULL;
    char *key = strtok_r(buf, "=", &savedPtr);
    while (key != NULL) {
        char *value = strtok_r(NULL, "\n", &savedPtr);
        if (strcmp(key, "bssid") == 0) {
            StrSafeCopy(pcmd->bssid, sizeof(pcmd->bssid), value);
        } else if (strcmp(key, "freq") == 0) {
            pcmd->freq = atoi(value);
        } else if (strcmp(key, "ssid") == 0) {
            StrSafeCopy(pcmd->ssid, sizeof(pcmd->ssid), value);
            printf_decode((u8 *)pcmd->ssid, sizeof(pcmd->ssid), pcmd->ssid);
        } else if (strcmp(key, "id") == 0) {
            pcmd->id = atoi(value);
        } else if (strcmp(key, "key_mgmt") == 0) {
            StrSafeCopy(pcmd->keyMgmt, sizeof(pcmd->keyMgmt), value);
        } else if (strcmp(key, "address") == 0) {
            StrSafeCopy(pcmd->address, sizeof(pcmd->address), value);
        }

        key = strtok_r(NULL, "=", &savedPtr);
    }
    free(buf);
    if (strcmp(pcmd->address, "") == 0) {
        return -1;
    }
    if (strcmp(pcmd->bssid, "") == 0) {
        return 1;
    }
    return 0;
}

static int WpaCliCmdAddNetworks(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s ADD_NETWORK", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        return -1;
    }
    return atoi(buf);
}

static int WpaCliCmdReconnect(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s RECONNECT", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdReassociate(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s REASSOCIATE", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdDisconnect(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s DISCONNECT", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdSaveConfig(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SAVE_CONFIG", this->ifname) < 0) {
        LOGE("snprintf error");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int CalcQuotationMarksFlag(int pos, const char value[WIFI_NETWORK_CONFIG_VALUE_LENGTH])
{
    int flag = g_wpaSsidFields[pos].flag;
    const int HEX_PSK_MAX_LEN = 64;
    int len = strlen(value);
    /* if the psk length is 64, it's hex format and don't need quotation marks */
    if (pos == DEVICE_CONFIG_PSK && len >= HEX_PSK_MAX_LEN) {
        flag = QUOTATION_MARKS_FLAG_NO;
    }
    if (pos == DEVICE_CONFIG_WEP_KEY_0 ||
        pos == DEVICE_CONFIG_WEP_KEY_1 ||
        pos == DEVICE_CONFIG_WEP_KEY_2 ||
        pos == DEVICE_CONFIG_WEP_KEY_3) {
        const int WEP_KEY_LEN1 = 5;
        const int WEP_KEY_LEN2 = 13;
        const int WEP_KEY_LEN3 = 16;
        /* For wep key, ASCII format need quotation marks, hex format is not required */
        if (len == WEP_KEY_LEN1 || len == WEP_KEY_LEN2 || len == WEP_KEY_LEN3) {
            flag = QUOTATION_MARKS_FLAG_YES;
        }
    }
    return flag;
}

static int WpaCliCmdSetNetwork(WifiWpaStaInterface *this, const struct WpaSetNetworkArgv *argv)
{
    if (this == NULL || argv == NULL) {
        return -1;
    }
    int pos = -1;
    for (unsigned i = 0; i < sizeof(g_wpaSsidFields) / sizeof(g_wpaSsidFields[0]); ++i) {
        if (g_wpaSsidFields[i].field == argv->param) {
            pos = i;
            break;
        }
    }
    if (pos < 0) {
        LOGE("unsupported param: %{public}d", argv->param);
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    int res;
    if (CalcQuotationMarksFlag(pos, argv->value) == QUOTATION_MARKS_FLAG_YES) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET_NETWORK %d %s \"%s\"", this->ifname,
            argv->id, g_wpaSsidFields[pos].fieldName, argv->value);
    } else {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET_NETWORK %d %s %s", this->ifname,
            argv->id, g_wpaSsidFields[pos].fieldName, argv->value);
    }
    if (res < 0) {
        LOGE("Internal error, set request message failed!");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdEnableNetwork(WifiWpaStaInterface *this, int networkId)
{
    if (this == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s ENABLE_NETWORK %d", this->ifname, networkId) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdSelectNetwork(WifiWpaStaInterface *this, int networkId)
{
    if (this == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SELECT_NETWORK %d", this->ifname, networkId) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdDisableNetwork(WifiWpaStaInterface *this, int networkId)
{
    if (this == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s DISABLE_NETWORK %d", this->ifname, networkId) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdRemoveNetwork(WifiWpaStaInterface *this, int networkId)
{
    if (this == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    int res = 0;
    if (networkId == -1) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s REMOVE_NETWORK all", this->ifname);
    } else if (networkId >= 0) {
        res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s REMOVE_NETWORK %d", this->ifname, networkId);
    } else {
        return -1;
    }
    if (res < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdGetNetwork(
    WifiWpaStaInterface *this, const struct WpaGetNetworkArgv *argv, char *pcmd, unsigned size)
{
    if (this == NULL || argv == NULL || pcmd == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s GET_NETWORK %d %s", this->ifname, argv->id,
        argv->param) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (WpaCliCmd(cmd, buf, REPLY_BUF_LENGTH) != 0) {
        free(buf);
        return -1;
    }
    if (strncpy_s(pcmd, size, buf, strlen(buf)) != EOK) {
        LOGE("copy set get_network result failed!");
        free(buf);
        return -1;
    }
    free(buf);
    return 0;
}

static int WpaCliCmdWpsPbc(WifiWpaStaInterface *this, const struct WpaWpsPbcArgv *wpspbc)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    int pos = 0;
    int res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PBC", this->ifname);
    if (res < 0) {
        LOGE("snprintf err");
        return -1;
    }
    pos += res;
    if (wpspbc != NULL) {
        res = 0; /* reset res value */
        if (wpspbc->anyFlag == 1) {
            res = snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " %s", "any");
        } else if (strlen(wpspbc->bssid) > 0) {
            res = snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " %s", wpspbc->bssid);
        }
        if (res < 0) {
            LOGE("snprintf err");
            return -1;
        }
        pos += res;
        if (wpspbc->multiAp > 0) { /* The value of ap needs to be determined. The value is greater than 0. */
            res = snprintf_s(
                cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " multi_ap=%d", wpspbc->multiAp);
            if (res < 0) {
                LOGE("snprintf err");
                return -1;
            }
        }
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("wps_pbc return failed!");
        return -1;
    }
    if (strncmp(buf, "FAIL-PBC-OVERLAP", strlen("FAIL-PBC-OVERLAP")) == 0) {
        LOGE("wps_pbc success, but result err: buf =%{public}s", buf);
        return FAIL_PBC_OVERLAP_RETUEN; /* Add a new enumerated value. */
    }
    return 0;
}

static int WpaCliCmdWpsPin(WifiWpaStaInterface *this, const struct WpaWpsPinArgv *wpspin, int *pincode)
{
    if (this == NULL || wpspin == NULL || pincode == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    int pos = 0;
    int res = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_PIN", this->ifname);
    if (res < 0) {
        LOGE("snprintf err");
        return -1;
    }
    pos += res;
    if (strlen(wpspin->bssid) > 0) {
        res = snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " %s", wpspin->bssid);
    } else {
        res = snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " any");
    }
    if (res < 0) {
        LOGE("snprintf err");
        return -1;
    }
    pos += res;
    if (strlen(wpspin->pinCode) > 0) {
        res = snprintf_s(cmd + pos, sizeof(cmd) - pos, sizeof(cmd) - pos - 1, " %s", wpspin->pinCode);
        if (res < 0) {
            LOGE("snprintf err");
            return -1;
        }
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("wps_pin return failed!");
        return -1;
    }
    *pincode = atoi(buf);
    return 0;
}

static int WpaCliCmdWpsCancel(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s WPS_CANCEL", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdPowerSave(WifiWpaStaInterface *this, int enable)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    int ret;
    if (enable) {
        ret = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET PS 1", this->ifname);
    } else {
        ret = snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET PS 0", this->ifname);
    }
    if (ret < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdSetCountryCode(WifiWpaStaInterface *this, const char *countryCode)
{
    if (this == NULL || countryCode == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SET country %s", this->ifname, countryCode) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdGetCountryCode(WifiWpaStaInterface *this, char *countryCode, int codeSize)
{
    if (this == NULL || countryCode == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s GET country", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    if (WpaCliCmd(cmd, buf, sizeof(buf)) != 0) {
        LOGE("get countrycode failed");
        return -1;
    }
    if (strncpy_s(countryCode, codeSize, buf, strlen(buf)) != EOK) {
        LOGE("copy set country code failed!");
        return -1;
    }
    return 0;
}

static int WpaCliCmdSetAutoConnect(WifiWpaStaInterface *this, int enable)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s STA_AUTOCONNECT %d", this->ifname, enable) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdWpaBlockListClear(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s BL%cCKLIST clear", this->ifname, 'A') < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static void ListNetworkProcess(WifiNetworkInfo *pcmd, char *tmpBuf, int bufLeng)
{
    int start = 0; /* start pos */
    int end = 0;   /* end pos */
    int i = 0;
    while (end < bufLeng) {
        if (tmpBuf[end] != '\t') {
            ++end;
            continue;
        }
        tmpBuf[end] = '\0';
        if (i == SCAN_INFO_NONE) {
            pcmd->id = atoi(tmpBuf);
        } else if (i == SCAN_INFO_ONE) {
            if (strcpy_s(pcmd->ssid, sizeof(pcmd->ssid), tmpBuf + start) != EOK) {
                break;
            }
            printf_decode((u8 *)pcmd->ssid, sizeof(pcmd->ssid), pcmd->ssid);
        } else if (i == SCAN_INFO_TWO) {
            if (strcpy_s(pcmd->bssid, sizeof(pcmd->bssid), tmpBuf + start) != EOK) {
                break;
            }
            start = end + 1;
            if (strcpy_s(pcmd->flags, sizeof(pcmd->flags), tmpBuf + start) != EOK) {
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

static int WpaCliCmdListNetworks(WifiWpaStaInterface *this, WifiNetworkInfo *pcmd, int *size)
{
    if (this == NULL || pcmd == NULL || size == NULL || *size <= 0) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s LIST_NETWORKS", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (WpaCliCmd(cmd, buf, REPLY_BUF_LENGTH) != 0) {
        free(buf);
        return -1;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, "\n", &savedPtr); /* skip first line */
    token = strtok_r(NULL, "\n", &savedPtr);
    int j = 0;

    while (token != NULL) {
        if (j >= *size) {
            *size = j;
            LOGW("list_networks full!");
            free(buf);
            return 0;
        }
        int length = strlen(token);
        if (length <= 0) {
            break;
        }
        ListNetworkProcess(pcmd + j, token, length);
        token = strtok_r(NULL, "\n", &savedPtr);
        j++;
    }
    *size = j;
    if (*size <= 0) {
        LOGW("list_networks empty!");
    }
    free(buf);
    return 0;
}

static int ConcatScanSetting(const ScanSettings *settings, char *buff, int len)
{
    if (settings == NULL) {
        return 0;
    }
    int pos = 0;
    int res;
    int i;
    for (i = 0; i < settings->freqSize; ++i) {
        if (i == 0) {
            res = snprintf_s(buff + pos, len - pos, len - pos - 1, "%s", " freq=");
            if (res < 0) {
                LOGE("snprintf error");
                return -1;
            }
            pos += res;
        }
        if (i != (settings->freqSize - 1)) {
            res = snprintf_s(buff + pos, len - pos, len - pos - 1, "%d,", settings->freqs[i]);
        } else {
            res = snprintf_s(buff + pos, len - pos, len - pos - 1, "%d;", settings->freqs[i]);
        }
        if (res < 0) {
            LOGE("snprintf error");
            return -1;
        }
        pos += res;
    }
    for (i = 0; i < settings->hiddenSsidSize; ++i) {
        res = snprintf_s(buff + pos, len - pos, len - pos - 1, " ssid ");
        if (res < 0) {
            LOGE("snprintf error");
            return -1;
        }
        pos += res;
        char *p = settings->hiddenSsid[i];
        while (*p) {
            res = snprintf_s(buff + pos, len - pos, len - pos - 1, "%02x", *p);
            if (res < 0) {
                LOGE("snprintf error");
                return -1;
            }
            pos += res;
            p++;
        }
    }
    return 0;
}

static int WpaCliCmdBssFlush(WifiWpaStaInterface *this)
{
    if (this == NULL) {
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s BSS_FLUSH 0", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    return WpaCliCmd(cmd, buf, sizeof(buf));
}

static int WpaCliCmdScan(WifiWpaStaInterface *this, const ScanSettings *settings)
{
    if (this == NULL) {
        return -1;
    }

    /* Invalidate expired scan results */
    WpaCliCmdBssFlush(this);
    unsigned len = CMD_BUFFER_SIZE;
    if (settings != NULL) {
        unsigned expectLen = strlen("IFNAME=") + strlen(this->ifname) + 1 + strlen("SCAN");
        if (settings->freqSize > 0) {
            expectLen += strlen(" freq=") + (CMD_FREQ_MAX_LEN + 1) * settings->freqSize;
        }
        for (int i = 0; i < settings->hiddenSsidSize; ++i) {
            unsigned ssidLen = strlen(settings->hiddenSsid[i]);
            expectLen += strlen(" ssid ") + (ssidLen << 1); /* double size, convert to hex */
        }
        if (expectLen >= len) {
            len = expectLen + 1;
        }
    }
    char *pcmd = (char *)calloc(len, sizeof(char));
    if (pcmd == NULL) {
        return -1;
    }
    int pos = 0;
    int res = snprintf_s(pcmd, len, len - 1, "IFNAME=%s SCAN", this->ifname);
    if (res < 0) {
        LOGE("snprintf error");
        free(pcmd);
        return -1;
    }
    pos += res;
    if (settings != NULL && ConcatScanSetting(settings, pcmd + pos, len - pos) < 0) {
        LOGE("snprintf scan settings error");
        free(pcmd);
        return -1;
    }
    char buf[REPLY_BUF_SMALL_LENGTH] = {0};
    if (WpaCliCmd(pcmd, buf, sizeof(buf)) != 0) {
        free(pcmd);
        return -1;
    }
    free(pcmd);
    if (strncmp(buf, "FAIL-BUSY", strlen("FAIL-BUSY")) == 0) {
        return FAIL_BUSY;
    }
    return 0;
}

static int DelScanInfoLine(ScanInfo *pcmd, char *srcBuf, int length)
{
    int i = 0;
    int start = 0;
    int end = 0;
    int fail = 0;
    while (end < length) {
        if (srcBuf[end] != '\t') {
            ++end;
            continue;
        }
        srcBuf[end] = '\0';
        if (i == 0) {
            if (strcpy_s(pcmd->bssid, sizeof(pcmd->bssid), srcBuf + start) != EOK) {
                fail = 1;
                break;
            }
        } else if (i == 1) {
            pcmd->freq = atoi(srcBuf + start);
        } else if (i == SCAN_INFO_TWO) {
            pcmd->siglv = atoi(srcBuf + start);
        } else if (i == SCAN_INFO_THREE) {
            if (strcpy_s(pcmd->flags, sizeof(pcmd->flags), srcBuf + start) != EOK) {
                fail = 1;
                break;
            }
            start = end + 1;
            if (strcpy_s(pcmd->ssid, sizeof(pcmd->ssid), srcBuf + start) != EOK) {
                fail = 1;
                break;
            }
            printf_decode((u8 *)pcmd->ssid, sizeof(pcmd->ssid), pcmd->ssid);
            start = length;
            break;
        }
        ++i;
        ++end;
        start = end;
    }
    if (fail == 0 && start < length) {
        if (strcpy_s(pcmd->flags, sizeof(pcmd->flags), srcBuf + start) != EOK) {
            fail = 1;
        }
    }
    return fail;
}

static int WpaCliCmdScanInfo(WifiWpaStaInterface *this, ScanInfo *pcmd, int *size)
{
    if (this == NULL || pcmd == NULL || size == NULL || *size <= 0) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SCAN_RESULTS", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (WpaCliCmd(cmd, buf, REPLY_BUF_LENGTH) != 0) {
        free(buf);
        return -1;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, "\n", &savedPtr); /* skip first line */
    token = strtok_r(NULL, "\n", &savedPtr);
    int j = 0;
    while (token != NULL) {
        if (j >= *size) {
            *size = j;
            LOGE("get scan info full!");
            free(buf);
            return 0;
        }
        int length = strlen(token);
        if (length <= 0) {
            break;
        }
        if (DelScanInfoLine(&pcmd[j], token, length)) {
            LOGE("parse scan result line failed!");
            break;
        }
        token = strtok_r(NULL, "\n", &savedPtr);
        j++;
    }
    *size = j;
    free(buf);
    return 0;
}

static int WpaCliCmdGetSignalInfo(WifiWpaStaInterface *this, WpaSignalInfo *info)
{
    if (this == NULL || info == NULL) {
        return -1;
    }
    char cmd[CMD_BUFFER_SIZE] = {0};
    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "IFNAME=%s SIGNAL_POLL", this->ifname) < 0) {
        LOGE("snprintf err");
        return -1;
    }
    char *buf = (char *)calloc(REPLY_BUF_LENGTH, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (WpaCliCmd(cmd, buf, REPLY_BUF_LENGTH) != 0) {
        free(buf);
        return -1;
    }
    char *savedPtr = NULL;
    char *token = strtok_r(buf, "=", &savedPtr);
    while (token != NULL) {
        if (strcmp(token, "RSSI") == 0) {
            token = strtok_r(NULL, "\n", &savedPtr);
            info->signal = atoi(token);
        } else if (strcmp(token, "LINKSPEED") == 0) {
            token = strtok_r(NULL, "\n", &savedPtr);
            info->txrate = atoi(token);
        } else if (strcmp(token, "NOISE") == 0) {
            token = strtok_r(NULL, "\n", &savedPtr);
            info->noise = atoi(token);
        } else if (strcmp(token, "FREQUENCY") == 0) {
            token = strtok_r(NULL, "\n", &savedPtr);
            info->frequency = atoi(token);
        } else {
            token = strtok_r(NULL, "\n", &savedPtr);
        }
        token = strtok_r(NULL, "=", &savedPtr);
    }
    free(buf);
    return 0;
}

WifiWpaStaInterface *GetWifiStaInterface(int staNo)
{
    char *name;
    if (staNo == 0) {
        name = "wlan0";
    } else {
        name = "wlan2";
    }
    WifiWpaStaInterface *p = g_wpaStaInterface;
    while (p != NULL) {
        if (strcmp(p->ifname, name) == 0) {
            return p;
        }
        p = p->next;
    }
    p = (WifiWpaStaInterface *)calloc(1, sizeof(WifiWpaStaInterface));
    if (p == NULL) {
        return NULL;
    }
    StrSafeCopy(p->ifname, sizeof(p->ifname), name);
    p->staNo = staNo;
    p->wpaCliCmdStatus = WpaCliCmdStatus;
    p->wpaCliCmdAddNetworks = WpaCliCmdAddNetworks;
    p->wpaCliCmdReconnect = WpaCliCmdReconnect;
    p->wpaCliCmdReassociate = WpaCliCmdReassociate;
    p->wpaCliCmdDisconnect = WpaCliCmdDisconnect;
    p->wpaCliCmdSaveConfig = WpaCliCmdSaveConfig;
    p->wpaCliCmdSetNetwork = WpaCliCmdSetNetwork;
    p->wpaCliCmdEnableNetwork = WpaCliCmdEnableNetwork;
    p->wpaCliCmdSelectNetwork = WpaCliCmdSelectNetwork;
    p->wpaCliCmdDisableNetwork = WpaCliCmdDisableNetwork;
    p->wpaCliCmdRemoveNetwork = WpaCliCmdRemoveNetwork;
    p->wpaCliCmdGetNetwork = WpaCliCmdGetNetwork;
    p->wpaCliCmdWpsPbc = WpaCliCmdWpsPbc;
    p->wpaCliCmdWpsPin = WpaCliCmdWpsPin;
    p->wpaCliCmdWpsCancel = WpaCliCmdWpsCancel;
    p->wpaCliCmdPowerSave = WpaCliCmdPowerSave;
    p->wpaCliCmdSetCountryCode = WpaCliCmdSetCountryCode;
    p->wpaCliCmdGetCountryCode = WpaCliCmdGetCountryCode;
    p->wpaCliCmdSetAutoConnect = WpaCliCmdSetAutoConnect;
    p->wpaCliCmdWpaBlockListClear = WpaCliCmdWpaBlockListClear;
    p->wpaCliCmdListNetworks = WpaCliCmdListNetworks;
    p->wpaCliCmdScan = WpaCliCmdScan;
    p->wpaCliCmdScanInfo = WpaCliCmdScanInfo;
    p->wpaCliCmdGetSignalInfo = WpaCliCmdGetSignalInfo;
    p->next = g_wpaStaInterface;
    g_wpaStaInterface = p;

    return p;
}

void ReleaseWifiStaInterface(int staNo)
{
    char *name;
    if (staNo == 0) {
        name = "wlan0";
    } else {
        name = "wlan2";
    }
    WifiWpaStaInterface *p = g_wpaStaInterface;
    WifiWpaStaInterface *prev = NULL;
    while (p != NULL) {
        if (strcmp(p->ifname, name) == 0) {
            break;
        }
        prev = p;
        p = p->next;
    }
    if (p == NULL) {
        return;
    }
    if (prev == NULL) {
        g_wpaStaInterface = p->next;
    } else {
        prev->next = p->next;
    }
    free(p);
    return;
}

WifiWpaStaInterface *TraversalWifiStaInterface(void)
{
    return g_wpaStaInterface;
}

int GetStaInterfaceNo(const char *ifName)
{
    WifiWpaStaInterface *p = g_wpaStaInterface;
    while (p != NULL) {
        if (strcmp(p->ifname, ifName) == 0) {
            return p->staNo;
        }
        p = p->next;
    }
    return -1;
}
