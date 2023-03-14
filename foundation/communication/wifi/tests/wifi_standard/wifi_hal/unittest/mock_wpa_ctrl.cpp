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
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "mock_wpa_ctrl.h"
#include "wifi_hal_common_func.h"

static std::vector<int> gPipeFd;
static int gUsePipeFdNum = 0;
const int OncePipeFdNum = 2;
const int MaxExactCmdLength = 32;

static std::map<std::string, std::string> gSupportedCmd;

void MockSetWpaExpectCmdResponse(const std::string &cmd, const std::string &response)
{
    gSupportedCmd[cmd] = response;
}

void MockEraseSupportedCmd(const std::string &cmd)
{
    gSupportedCmd.erase(cmd);
}

void MockInitGlobalCmd(void)
{
    gSupportedCmd["INTERFACE_ADD"] = "OK";
    gSupportedCmd["INTERFACE_REMOVE"] = "OK";
}

void MockInitStaSupportedCmd(void)
{
    gSupportedCmd["STATUS"] = "OK";
    gSupportedCmd["ADD_NETWORK"] = "OK";
    gSupportedCmd["RECONNECT"] = "OK";
    gSupportedCmd["REASSOCIATE"] = "OK";
    gSupportedCmd["DISCONNECT"] = "OK";
    gSupportedCmd["SAVE_CONFIG"] = "OK";
    gSupportedCmd["SET_NETWORK"] = "OK";
    gSupportedCmd["ENABLE_NETWORK"] = "OK";
    gSupportedCmd["SELECT_NETWORK"] = "OK";
    gSupportedCmd["DISABLE_NETWORK"] = "OK";
    gSupportedCmd["REMOVE_NETWORK"] = "OK";
    gSupportedCmd["GET_NETWORK"] = "OK";
    gSupportedCmd["WPS_PBC"] = "OK";
    gSupportedCmd["WPS_PIN"] = "OK";
    gSupportedCmd["WPS_CANCEL"] = "OK";
    gSupportedCmd["SET"] = "OK";
    gSupportedCmd["GET"] = "OK";
    gSupportedCmd["STA_AUTOCONNECT"] = "OK";
    gSupportedCmd["RECONFIGURE"] = "OK";
    gSupportedCmd["LIST_NETWORKS"] = "OK";
    gSupportedCmd["SIGNAL_POLL"] = "OK";
    return;
}

void MockInitApSupportedCmd(void)
{
    gSupportedCmd["ENABLE"] = "OK";
    gSupportedCmd["SET"] = "OK";
    gSupportedCmd["STATUS"] = "OK";
    gSupportedCmd["DISABLE"] = "OK";
    gSupportedCmd["DENY_ACL"] = "OK";
    gSupportedCmd["STA-FIRST"] = "OK";
    gSupportedCmd["STA-NEXT"] = "OK";
    gSupportedCmd["RELOAD"] = "OK";
    gSupportedCmd["DEAUTHENTICATE"] = "OK";
    gSupportedCmd["DISASSOCIATE"] = "OK";
    return;
}

void MockInitP2pSupportedCmd(void)
{
    gSupportedCmd["P2P_STOP_FIND"] = "OK";
    gSupportedCmd["P2P_FIND"] = "OK";
    gSupportedCmd["SET"] = "OK";
    gSupportedCmd["STATUS"] = "OK";
    gSupportedCmd["P2P_FLUSH"] = "OK";
    gSupportedCmd["P2P_SERVICE_FLUSH"] = "OK";
    gSupportedCmd["P2P_PROV_DISC"] = "OK";
    gSupportedCmd["P2P_GROUP_ADD"] = "OK";
    gSupportedCmd["SAVE_CONFIG"] = "OK";
    gSupportedCmd["LIST_NETWORKS"] = "OK";
    gSupportedCmd["P2P_PEER"] = "OK\n";
    gSupportedCmd["P2P_SERV_DISC_RESP"] = "OK";
    gSupportedCmd["P2P_SERV_DISC_EXTERNAL"] = "OK";
    gSupportedCmd["DRIVER"] = "OK";
    gSupportedCmd["P2P_SERV_DISC_CANCEL_REQ"] = "OK";
    gSupportedCmd["P2P_SERV_DISC_REQ"] = "OK";
    gSupportedCmd["P2P_SERVICE_ADD"] = "OK";
    gSupportedCmd["P2P_SERVICE_DEL"] = "OK";
    gSupportedCmd["P2P_INVITE"] = "OK";
    gSupportedCmd["P2P_CANCEL"] = "OK";
    gSupportedCmd["P2P_SET"] = "OK";
    gSupportedCmd["P2P_EXT_LISTEN"] = "OK";
    gSupportedCmd["WFD_SUBELEM_SET"] = "OK";
    gSupportedCmd["REMOVE_NETWORK"] = "OK";
    gSupportedCmd["P2P_GROUP_REMOVE"] = "OK";
    gSupportedCmd["WPS_PBC"] = "OK";
    gSupportedCmd["WPS_PIN"] = "OK";
    gSupportedCmd["P2P_CONNECT"] = "OK";
    return;
}

void MockWpaCallback(struct wpa_ctrl *send, const char *message)
{
    if (send == NULL || message == NULL) {
        return;
    }
    write(send->s, message, strlen(message));
    return;
}

static void SetNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return;
    }
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    return;
}

struct wpa_ctrl *wpa_ctrl_open(const char *ctrl_path)
{
    (void)ctrl_path;
    struct wpa_ctrl *ctrl = (struct wpa_ctrl *)calloc(1, sizeof(struct wpa_ctrl));
    if (ctrl == NULL) {
        return NULL;
    }
    if (gUsePipeFdNum % OncePipeFdNum == 0) {
        int pipeFd[OncePipeFdNum] = {0};
        pipe(pipeFd);
        SetNonBlock(pipeFd[0]);
        gPipeFd.push_back(pipeFd[0]);
        gPipeFd.push_back(pipeFd[1]);
    }
    if (gUsePipeFdNum % OncePipeFdNum == 0) {
        ctrl->s = gPipeFd[gUsePipeFdNum + 1];
    } else {
        ctrl->s = gPipeFd[gUsePipeFdNum - 1];
    }
    ++gUsePipeFdNum;
    return ctrl;
}

struct wpa_ctrl * wpa_ctrl_open2(const char *ctrl_path, const char *cli_path)
{
    (void)cli_path;
    return wpa_ctrl_open(ctrl_path);
}

void wpa_ctrl_close(struct wpa_ctrl *ctrl)
{
    if (ctrl == NULL) {
        return;
    }
    close(ctrl->s);
    for (auto iter = gPipeFd.begin(); iter != gPipeFd.end(); ++iter) {
        if (*iter == ctrl->s) {
            gPipeFd.erase(iter);
            break;
        }
    }
    --gUsePipeFdNum;
    free(ctrl);
    return;
}

static void GetExactCmd(const char *cmd, char *out, int out_len)
{
    if (cmd == NULL || out == NULL) {
        return;
    }
    int i = 0;
    int j = 0;
    if (strncmp(cmd, "IFNAME=", strlen("IFNAME=")) == 0) {
        const char *p = strchr(cmd, ' ');
        if (p == NULL) {
            return;
        }
        j = p - cmd + 1;
    }
    while (i < out_len - 1 && cmd[i + j] != '\0' && cmd[i + j] != ' ') {
        out[i] = cmd[i + j];
        ++i;
    }
    out[i] = '\0';
    return;
}

static int CheckInputCmdArgs(const char *cmd)
{
    (void)cmd;
    return 0;
}

int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len, char *reply, size_t *reply_len,
    void (*msg_cb)(char *msg, size_t len))
{
    if (ctrl == NULL || cmd == NULL || reply == NULL || reply_len == NULL) {
        return -1;
    }
    (void)cmd_len;
    (void)msg_cb;
    char exactCmd[MaxExactCmdLength] = {0};
    GetExactCmd(cmd, exactCmd, MaxExactCmdLength);
    std::map<std::string, std::string>::iterator iter = gSupportedCmd.find(std::string(exactCmd));
    if (iter == gSupportedCmd.end()) {
        StrSafeCopy(reply, *reply_len, "UNKNOWN COMMAND\n");
        *reply_len = strlen(reply);
        return -1;
    }
    if (CheckInputCmdArgs(cmd) < 0) {
        StrSafeCopy(reply, *reply_len, "FAIL\n");
    } else {
        StrSafeCopy(reply, *reply_len, iter->second.c_str());
    }
    *reply_len = strlen(reply);
    return 0;
}

int wpa_ctrl_attach(struct wpa_ctrl *ctrl)
{
    (void)ctrl;
    return 0;
}

int wpa_ctrl_detach(struct wpa_ctrl *ctrl)
{
    (void)ctrl;
    return 0;
}

int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, size_t *reply_len)
{
    size_t pos = 0;
    while (pos < *reply_len) {
        int ret = read(ctrl->s, reply + pos, 1);
        if (ret <= 0) {
            break;
        }
        if (reply[pos] == '\n') {
            break;
        }
        ++pos;
    }
    *reply_len = pos;
    reply[pos] = '\0';
    return 0;
}

int wpa_ctrl_pending(struct wpa_ctrl *ctrl)
{
    (void)ctrl;
    return 0;
}

int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl)
{
    if (ctrl != NULL) {
        return ctrl->s;
    }
    return -1;
}

size_t printf_decode(u8 *buf, size_t maxlen, const char *str)
{
    return 0;
}