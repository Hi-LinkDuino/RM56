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
#include "wifi_wpa_common.h"
#include <string.h>
#include "wifi_wpa_hal.h"
#include "wifi_hal_common_func.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiHalWpaCommon"

#define HEX_TO_DEC_MOVING 4
#define DEC_MAX_SCOPE 10
#define WPA_CMD_RETURN_TIMEOUT (-2)

void GetStrKeyVal(char *src, const char *split, WpaKeyValue *out)
{
    if (src == NULL || split == NULL || out == NULL) {
        return;
    }
    char *p = strstr(src, split);
    if (p == NULL) {
        StrSafeCopy(out->key, sizeof(out->key), src);
        return;
    }
    *p = '\0';
    StrSafeCopy(out->key, sizeof(out->key), src);
    p += strlen(split);
    StrSafeCopy(out->value, sizeof(out->value), p);
    return;
}

int Hex2Dec(const char *str)
{
    if (str == NULL || strncasecmp(str, "0x", strlen("0x")) != 0) {
        return 0;
    }
    int result = 0;
    const char *tmp = str + strlen("0x");
    while (*tmp != '\0') {
        result <<= HEX_TO_DEC_MOVING;
        if (*tmp >= '0' && *tmp <= '9') {
            result += *tmp - '0';
        } else if (*tmp >= 'A' && *tmp <= 'F') {
            result += *tmp - 'A' + DEC_MAX_SCOPE;
        } else if (*tmp >= 'a' && *tmp <= 'f') {
            result += *tmp - 'a' + DEC_MAX_SCOPE;
        } else {
            result = 0;
            break;
        }
        ++tmp;
    }
    return result;
}

void TrimQuotationMark(char *str, char c)
{
    if (str == NULL) {
        return;
    }
    int len = strlen(str);
    if (len == 0) {
        return;
    }
    if (str[len - 1] == c) {
        str[len - 1] = '\0';
        --len;
    }
    if (str[0] == c) {
        for (int i = 0; i < len - 1; ++i) {
            str[i] = str[i + 1];
        }
        str[len - 1] = '\0';
    }
    return;
}

void ReleaseWpaCtrl(WpaCtrl *pCtrl)
{
    if (pCtrl == NULL) {
        return;
    }
    if (pCtrl->pSend != NULL) {
        wpa_ctrl_close(pCtrl->pSend);
        pCtrl->pSend = NULL;
    }
    if (pCtrl->pRecv != NULL) {
        wpa_ctrl_close(pCtrl->pRecv);
        pCtrl->pRecv = NULL;
    }
    return;
}

int InitWpaCtrl(WpaCtrl *pCtrl, const char *ifname)
{
    if (pCtrl == NULL || ifname == NULL) {
        return -1;
    }
    int flag = 0;
    do {
        pCtrl->pSend = wpa_ctrl_open("global");
        pCtrl->pRecv = wpa_ctrl_open("global");
        if (pCtrl->pSend == NULL || pCtrl->pRecv == NULL) {
            LOGE("open wpa control interface failed!");
            break;
        }
        if (wpa_ctrl_attach(pCtrl->pRecv) != 0) {
            LOGE("attach monitor interface failed!");
            break;
        }
        flag += 1;
    } while (0);
    if (!flag) {
        ReleaseWpaCtrl(pCtrl);
        return -1;
    }
    return 0;
}

int WpaCliCmd(const char *cmd, char *buf, size_t bufLen)
{
    if (cmd == NULL || buf == NULL || bufLen <= 0) {
        return -1;
    }
    WpaCtrl *ctrl = GetWpaCtrl();
    if (ctrl == NULL || ctrl->pSend == NULL) {
        return -1;
    }
    size_t len = bufLen - 1;
    LOGI("wpa_ctrl_request -> cmd: %{private}s", cmd);
    int ret = wpa_ctrl_request(ctrl->pSend, cmd, strlen(cmd), buf, &len, NULL);
    if (ret == WPA_CMD_RETURN_TIMEOUT) {
        LOGE("[%{private}s] command timed out.", cmd);
        return WPA_CMD_RETURN_TIMEOUT;
    } else if (ret < 0) {
        LOGE("[%{private}s] command failed.", cmd);
        return -1;
    }
    buf[len] = '\0';
    LOGI("wpa_ctrl_request -> buf: %{private}s", buf);
    if (strncmp(buf, "FAIL\n", strlen("FAIL\n")) == 0 ||
        strncmp(buf, "UNKNOWN COMMAND\n", strlen("UNKNOWN COMMAND\n")) == 0) {
        LOGE("%{private}s request success, but response %{public}s", cmd, buf);
        return -1;
    }
    return 0;
}
