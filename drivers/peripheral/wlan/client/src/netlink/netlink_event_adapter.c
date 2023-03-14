/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <poll.h>
#include <sys/types.h>
#include <securec.h>
#include <unistd.h>
#include <errno.h>
#include <net/if.h>
#include <linux/nl80211.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>

#include "hilog/log.h"
#include "../wifi_common_cmd.h"
#include "netlink_adapter.h"

#define OUI_QCA 0x001374

#define LISTEN_FD_NUMS 2
#define EVENT_SOCKET_INDEX 0
#define CTRL_SOCKET_INDEX 1
#define CTRL_SOCKET_WRITE_SIDE 0
#define CTRL_SOCKET_READ_SIDE 1

#define BUFSIZE 1024
#define POLLTIMEOUT 1000
#define SIGNALLEVELCONFFICIENT 100

static inline uint32_t BitLeftShift(uint8_t x)
{
    return 1U << x;
}

#define SCAN_QUAL_INVALID      BitLeftShift(0)
#define SCAN_NOISE_INVALID     BitLeftShift(1)
#define SCAN_LEVEL_INVALID     BitLeftShift(2)
#define SCAN_LEVEL_DBM         BitLeftShift(3)
#define SCAN_ASSOCIATED        BitLeftShift(5)

#define SUCCESS_STATUS 0
#define WLAN_ATTR_SCAN_COOKIE 7
#define WLAN_ATTR_SCAN_STATUS 8
#define WLAN_ATTR_SCAN_MAX 11
#define SCAN_STATUS_MAX 2
#define NL80211_SCAN_DONE 107

static int g_familyId = 0;

static int HandleCtrlEvent(int fd)
{
    int ret;
    char buf[BUFSIZE];

    ret = TEMP_FAILURE_RETRY(read(fd, buf, sizeof(buf)));
    if (ret < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Read after POLL returned %zd, error no = %d (%s)",
            __FUNCTION__, ret, errno, strerror(errno));
    }

    return ret;
}

static int NoSeqCheck(struct nl_msg *msg, void *arg)
{
    (void)msg;
    return NL_OK;
}

static void QcaWifiEventScanDoneProcess(const char *ifName, struct nlattr *data, size_t len)
{
    struct nlattr *attr[WLAN_ATTR_SCAN_MAX + 1];
    uint32_t status;

    if (nla_parse(attr, WLAN_ATTR_SCAN_MAX, data, len, NULL) ||
        attr[WLAN_ATTR_SCAN_STATUS] ||
        !attr[WLAN_ATTR_SCAN_COOKIE]) {
        return;
    }

    status = nla_get_u8(attr[WLAN_ATTR_SCAN_STATUS]);
    if (status >= SCAN_STATUS_MAX) {
        HILOG_ERROR(LOG_DOMAIN, "%s: invalid status", __func__);
        return;
    }

    WifiEventReport(ifName, WIFI_EVENT_SCAN_DONE, &status);
}

static void WifiEventVendorProcess(const char *ifName, struct nlattr **attr)
{
    uint32_t vendor_id;
    uint32_t subcmd;
    uint8_t *data = NULL;
    size_t len;

    if (attr[NL80211_ATTR_VENDOR_ID] == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: failed to get vendor id", __FUNCTION__);
        return;
    }
    if (attr[NL80211_ATTR_VENDOR_SUBCMD] == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: failed to get vendor subcmd", __FUNCTION__);
        return;
    }

    vendor_id = nla_get_u32(attr[NL80211_ATTR_VENDOR_ID]);
    subcmd = nla_get_u32(attr[NL80211_ATTR_VENDOR_SUBCMD]);
    if (vendor_id != OUI_QCA || subcmd != NL80211_SCAN_DONE) {
        HILOG_ERROR(LOG_DOMAIN, "%s: unsupported vendor event", __FUNCTION__);
        return;
    }

    if (attr[NL80211_ATTR_VENDOR_DATA] == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get vendor data fail", __FUNCTION__);
        return;
    }
    data = nla_data(attr[NL80211_ATTR_VENDOR_DATA]);
    len = nla_len(attr[NL80211_ATTR_VENDOR_DATA]);

    QcaWifiEventScanDoneProcess(ifName, (struct nlattr *)data, len);
}

static void DoGetScanResult(struct nlattr *attr[], int len, WifiScanResult *scanResult)
{
    if (attr[NL80211_BSS_INFORMATION_ELEMENTS]) {
        scanResult->ie = nla_data(attr[NL80211_BSS_INFORMATION_ELEMENTS]);
        scanResult->ieLen = nla_len(attr[NL80211_BSS_INFORMATION_ELEMENTS]);
    }
    if (attr[NL80211_BSS_BEACON_IES]) {
        scanResult->beaconIe = nla_data(attr[NL80211_BSS_BEACON_IES]);
        scanResult->beaconIeLen = nla_len(attr[NL80211_BSS_BEACON_IES]);
    }
    if (attr[NL80211_BSS_BSSID]) {
        scanResult->bssid = nla_data(attr[NL80211_BSS_BSSID]);
    }
    if (attr[NL80211_BSS_FREQUENCY]) {
        scanResult->freq = nla_get_u32(attr[NL80211_BSS_FREQUENCY]);
    }
    if (attr[NL80211_BSS_BEACON_INTERVAL]) {
        scanResult->beaconInt = nla_get_u16(attr[NL80211_BSS_BEACON_INTERVAL]);
    }
    if (attr[NL80211_BSS_CAPABILITY]) {
        scanResult->caps = nla_get_u16(attr[NL80211_BSS_CAPABILITY]);
    }
    if (attr[NL80211_BSS_SIGNAL_MBM]) {
        scanResult->level = nla_get_u32(attr[NL80211_BSS_SIGNAL_MBM]) / SIGNALLEVELCONFFICIENT; /* mBm to dBm */
        scanResult->flags |= SCAN_LEVEL_DBM | SCAN_QUAL_INVALID;
    } else if (attr[NL80211_BSS_SIGNAL_UNSPEC]) {
        scanResult->level = nla_get_u8(attr[NL80211_BSS_SIGNAL_UNSPEC]);
        scanResult->flags |= SCAN_QUAL_INVALID;
    } else {
        scanResult->flags |= SCAN_LEVEL_INVALID | SCAN_QUAL_INVALID;
    }
    if (attr[NL80211_BSS_SEEN_MS_AGO]) {
        scanResult->age = nla_get_u32(attr[NL80211_BSS_SEEN_MS_AGO]);
    }
}

static int32_t WifiGetScanResultHandler(struct nl_msg *msg, void *arg)
{
    WifiScanResult scanResult;
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    const char *ifName = (const char *)arg;
    struct nlattr *attr[NL80211_ATTR_MAX + 1], *bssAttr[NL80211_BSS_MAX + 1];
    static struct nla_policy bssPolicy[NL80211_BSS_MAX + 1];
    memset_s(bssPolicy, sizeof(bssPolicy), 0, sizeof(bssPolicy));
    bssPolicy[NL80211_BSS_FREQUENCY].type = NLA_U32;
    bssPolicy[NL80211_BSS_TSF].type = NLA_U64;
    bssPolicy[NL80211_BSS_BEACON_INTERVAL].type = NLA_U16;
    bssPolicy[NL80211_BSS_CAPABILITY].type = NLA_U16;
    bssPolicy[NL80211_BSS_SIGNAL_MBM].type = NLA_U32;
    bssPolicy[NL80211_BSS_SIGNAL_UNSPEC].type = NLA_U8;
    bssPolicy[NL80211_BSS_STATUS].type = NLA_U32;
    bssPolicy[NL80211_BSS_SEEN_MS_AGO].type = NLA_U32;

    if (ifName == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: ifName is null", __func__);
        return NL_SKIP;
    }

    nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0), genlmsg_attrlen(hdr, 0), NULL);
    if (!attr[NL80211_ATTR_BSS]) {
        HILOG_ERROR(LOG_DOMAIN, "%s: bss info missing", __func__);
        return NL_SKIP;
    }
    if (nla_parse_nested(bssAttr, NL80211_BSS_MAX, attr[NL80211_ATTR_BSS], bssPolicy)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: failed to parse nested attributes", __func__);
        return NL_SKIP;
    }

    DoGetScanResult(bssAttr, NL80211_BSS_MAX + 1, &scanResult);

    WifiEventReport(ifName, WIFI_EVENT_SCAN_RESULT, &scanResult);
    return NL_SKIP;
}

static void WifiEventScanResultProcess(const char *ifName)
{
    int32_t ret;
    uint32_t ifaceId = if_nametoindex(ifName);
    struct nl_msg *msg = nlmsg_alloc();

    genlmsg_put(msg, 0, 0, g_familyId, 0, NLM_F_DUMP, NL80211_CMD_GET_SCAN, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    ret = SendCmdSync(msg, WifiGetScanResultHandler, (void *)ifName);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __func__);
    }

    nlmsg_free(msg);
}

static void DoProcessEvent(const char *ifName, int cmd, struct nlattr **attr)
{
    switch (cmd) {
        case NL80211_CMD_VENDOR:
            WifiEventVendorProcess(ifName, attr);
            break;
        case NL80211_CMD_NEW_SCAN_RESULTS:
            WifiEventScanResultProcess(ifName);
            break;
        default:
            HILOG_INFO(LOG_DOMAIN, "%s: not supported cmd, cmd = %d\n", __FUNCTION__, cmd);
            break;
    }
}

static int32_t ProcessEvent(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *attr[NL80211_ATTR_MAX + 1];
    struct NetworkInfoResult networkInfo;
    uint32_t ifidx = -1;
    uint32_t i;
    int ret;

    nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);

    if (attr[NL80211_ATTR_IFINDEX]) {
        ifidx = nla_get_u32(attr[NL80211_ATTR_IFINDEX]);
    }

    ret = GetUsableNetworkInfo(&networkInfo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get usable network information failed", __FUNCTION__);
        return NL_SKIP;
    }

    for (i = 0; i < networkInfo.nums; i++) {
        if (ifidx == if_nametoindex(networkInfo.infos[i].name)) {
            DoProcessEvent(networkInfo.infos[i].name, hdr->cmd, attr);
            return NL_SKIP;
        }
    }

    return NL_SKIP;
}

static struct nl_cb *CreateCb(void)
{
    struct nl_cb *cb;

    cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (cb == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: alloc cb failed", __FUNCTION__);
        return NULL;
    }

    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, NoSeqCheck, NULL);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, ProcessEvent, NULL);

    return cb;
}

static int HandleEvent(struct nl_sock *sock)
{
    int ret;
    struct nl_cb *cb = CreateCb();
    if (cb == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Ceate cb failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    ret = nl_recvmsgs(sock, cb);

    nl_cb_put(cb);
    cb = NULL;
    return ret;
}

void *EventThread(void *para)
{
    struct nl_sock *eventSock = NULL;
    struct pollfd pollFds[LISTEN_FD_NUMS] = {0};
    struct WifiThreadParam *threadParam = NULL;
    int ctrlSocks[2];
    int ret;
    enum ThreadStatus *status = NULL;

    if (para == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: para is null", __FUNCTION__);
        return NULL;
    } else {
        threadParam = (struct WifiThreadParam *)para;
        eventSock = threadParam->eventSock;
        g_familyId = threadParam->familyId;
        status = threadParam->status;
        *status = THREAD_RUN;
    }

    pollFds[EVENT_SOCKET_INDEX].fd = nl_socket_get_fd(eventSock);
    pollFds[EVENT_SOCKET_INDEX].events = POLLIN | POLLERR;

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, ctrlSocks);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail socketpair", __FUNCTION__);
        *status = THREAD_STOP;
        return NULL;
    }
    pollFds[CTRL_SOCKET_INDEX].fd = ctrlSocks[CTRL_SOCKET_READ_SIDE];
    pollFds[CTRL_SOCKET_INDEX].events = POLLIN | POLLERR;

    while (*status == THREAD_RUN) {
        ret = TEMP_FAILURE_RETRY(poll(pollFds, LISTEN_FD_NUMS, POLLTIMEOUT));
        if (ret < 0) {
            HILOG_ERROR(LOG_DOMAIN, "%s: fail poll", __FUNCTION__);
            break;
        } else if (pollFds[EVENT_SOCKET_INDEX].revents & POLLERR) {
            HILOG_ERROR(LOG_DOMAIN, "%s: event socket get POLLERR event", __FUNCTION__);
            break;
        } else if (pollFds[EVENT_SOCKET_INDEX].revents & POLLIN) {
            if (HandleEvent(eventSock) != RET_CODE_SUCCESS) {
                break;
            }
        } else if (pollFds[CTRL_SOCKET_INDEX].revents & POLLERR) {
            HILOG_ERROR(LOG_DOMAIN, "%s: ctrl socket get POLLERR event", __FUNCTION__);
            break;
        } else if (pollFds[CTRL_SOCKET_INDEX].revents & POLLIN) {
            if (HandleCtrlEvent(pollFds[CTRL_SOCKET_INDEX].fd) != RET_CODE_SUCCESS) {
                break;
            }
        }
    }

    close(pollFds[CTRL_SOCKET_INDEX].fd);
    *status = THREAD_STOP;
    return NULL;
}
