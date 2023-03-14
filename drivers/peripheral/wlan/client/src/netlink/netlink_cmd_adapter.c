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

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <securec.h>
#include <net/if.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/nl80211.h>
#include <linux/netlink.h>
#include <linux/wireless.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netlink-private/types.h>
#include <arpa/inet.h>

#include "hilog/log.h"
#include "../wifi_common_cmd.h"
#include "netlink_adapter.h"

#define VENDOR_ID 0x001A11

// vendor subcmd
#define WIFI_SUBCMD_SET_COUNTRY_CODE 0x100E
#define WIFI_SUBCMD_SET_RANDOM_MAC_OUI 0x100C

#define WAITFORMUTEX 100000
#define WAITFORTHREAD 100000
#define RETRIES 30

#define STR_WLAN0 "wlan0"
#define STR_WLAN1 "wlan1"
#define STR_P2P0 "p2p0"
#define STR_P2P0_X "p2p0-"
#define STRLEN_WLAN0 5
#define STRLEN_WLAN1 5
#define STRLEN_P2P0 4
#define STRLEN_P2P0_X 5

// vendor attr
enum AndrWifiAttr {
    ANDR_WIFI_ATTRIBUTE_NUM_FEATURE_SET,
    ANDR_WIFI_ATTRIBUTE_FEATURE_SET,
    ANDR_WIFI_ATTRIBUTE_RANDOM_MAC_OUI,
    ANDR_WIFI_ATTRIBUTE_NODFS_SET,
    WIFI_ATTRIBUTE_COUNTRY
};

struct FamilyData {
    const char *group;
    int id;
};

struct WifiHalInfo {
    struct nl_sock *cmdSock;
    struct nl_sock *eventSock;
    int familyId;

    // thread controller info
    pthread_t thread;
    enum ThreadStatus status;
    pthread_mutex_t mutex;
};

static struct WifiHalInfo g_wifiHalInfo = {0};

static struct nl_sock *OpenNetlinkSocket(void)
{
    struct nl_sock *sock = NULL;

    sock = nl_socket_alloc();
    if (sock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to alloc socket", __FUNCTION__);
        return NULL;
    }

    if (nl_connect(sock, NETLINK_GENERIC) != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to connect socket", __FUNCTION__);
        nl_socket_free(sock);
        return NULL;
    }
    return sock;
}

static void CloseNetlinkSocket(struct nl_sock *sock)
{
    if (sock != NULL) {
        nl_socket_free(sock);
    }
}

static int32_t ConnectCmdSocket(void)
{
    g_wifiHalInfo.cmdSock = OpenNetlinkSocket();
    if (g_wifiHalInfo.cmdSock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to open cmd socket", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    nl_socket_disable_seq_check(g_wifiHalInfo.cmdSock);
    // send find familyId result to Controller
    g_wifiHalInfo.familyId = genl_ctrl_resolve(g_wifiHalInfo.cmdSock, NL80211_GENL_NAME);
    if (g_wifiHalInfo.familyId < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to resolve family", __FUNCTION__);
        CloseNetlinkSocket(g_wifiHalInfo.cmdSock);
        g_wifiHalInfo.cmdSock = NULL;
        return RET_CODE_FAILURE;
    }
    HILOG_INFO(LOG_DOMAIN, "%s: family id: %d", __FUNCTION__, g_wifiHalInfo.familyId);
    return RET_CODE_SUCCESS;
}

static void DisconnectCmdSocket(void)
{
    CloseNetlinkSocket(g_wifiHalInfo.cmdSock);
    g_wifiHalInfo.cmdSock = NULL;
}

static int32_t CmdSocketErrorHandler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
    int *ret = (int *)arg;

    *ret = err->error;
    return NL_SKIP;
}

static int32_t CmdSocketFinishHandler(struct nl_msg *msg, void *arg)
{
    int *ret = (int *)arg;

    *ret = 0;
    return NL_SKIP;
}

static int32_t CmdSocketAckHandler(struct nl_msg *msg, void *arg)
{
    int *err = (int *)arg;

    *err = 0;
    return NL_STOP;
}

int32_t SendCmdSync(struct nl_msg *msg, const RespHandler handler, void *data)
{
    int32_t rc;
    int32_t error;
    int32_t count = 0;
    struct nl_cb *cb = NULL;

    if (g_wifiHalInfo.cmdSock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: sock is null", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }

    while ((rc = pthread_mutex_trylock(&g_wifiHalInfo.mutex)) == EBUSY) {
        if (count < RETRIES) {
            HILOG_ERROR(LOG_DOMAIN, "%s: pthread b trylock", __FUNCTION__);
            count++;
            usleep(WAITFORMUTEX);
        } else {
            HILOG_ERROR(LOG_DOMAIN, "%s: pthread trylock timeout", __FUNCTION__);
            return RET_CODE_SUCCESS;
        }
    }

    if (rc != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: pthread trylock failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    rc = nl_send_auto(g_wifiHalInfo.cmdSock, msg); // seq num auto add
    if (rc < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nl_send_auto failed", __FUNCTION__);
        goto out;
    }

    cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (cb == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nl_cb_alloc failed", __FUNCTION__);
        rc = RET_CODE_FAILURE;
        goto out;
    }
    nl_cb_err(cb, NL_CB_CUSTOM, CmdSocketErrorHandler, &error);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, CmdSocketFinishHandler, &error);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, CmdSocketAckHandler, &error);
    if (handler != NULL) {
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, handler, data);
    }
    /* wait for reply */
    error = 1;
    while (error > 0) {
        rc = nl_recvmsgs(g_wifiHalInfo.cmdSock, cb);
        if (rc < 0) {
            HILOG_ERROR(LOG_DOMAIN, "%s: nl_recvmsgs failed: rc = %d, errno = %d, (%s)",
                __FUNCTION__, rc, errno, strerror(errno));
        }
    }
    nl_cb_put(cb);
out:
    pthread_mutex_unlock(&g_wifiHalInfo.mutex);
    return ((rc == 0) ? RET_CODE_SUCCESS : RET_CODE_FAILURE);
}

static int FamilyIdHandler(struct nl_msg *msg, void *arg)
{
    struct FamilyData *familyData = (struct FamilyData *)arg;
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *attr[CTRL_ATTR_MAX + 1];
    struct nlattr *mcastGrp = NULL;
    int i;

    if (hdr == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get nlmsg header fail", __FUNCTION__);
        return NL_SKIP;
    }

    nla_parse(attr, CTRL_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);
    if (!attr[CTRL_ATTR_MCAST_GROUPS]) {
        return NL_SKIP;
    }

    nla_for_each_nested(mcastGrp, attr[CTRL_ATTR_MCAST_GROUPS], i) {
        struct nlattr *attrMcastGrp[CTRL_ATTR_MCAST_GRP_MAX + 1];
        nla_parse(attrMcastGrp, CTRL_ATTR_MCAST_GRP_MAX, nla_data(mcastGrp),
            nla_len(mcastGrp), NULL);
        if (attrMcastGrp[CTRL_ATTR_MCAST_GRP_NAME] &&
            attrMcastGrp[CTRL_ATTR_MCAST_GRP_ID] &&
            strncmp(nla_data(attrMcastGrp[CTRL_ATTR_MCAST_GRP_NAME]), familyData->group,
                nla_len(attrMcastGrp[CTRL_ATTR_MCAST_GRP_NAME])) == 0) {
            familyData->id = nla_get_u32(attrMcastGrp[CTRL_ATTR_MCAST_GRP_ID]);
            break;
        }
    };

    return NL_SKIP;
}

static int GetMulticastId(const char *family, const char *group)
{
    struct nl_msg *msg = NULL;
    int ret;
    static struct FamilyData familyData;
    int familyId = genl_ctrl_resolve(g_wifiHalInfo.cmdSock, "nlctrl");

    familyData.group = group;
    familyData.id = -ENOENT;

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg_alloc failed", __FUNCTION__);
        return RET_CODE_NOMEM;
    }

    if (!genlmsg_put(msg, 0, 0, familyId, 0, 0, CTRL_CMD_GETFAMILY, 0) ||
        nla_put_string(msg, CTRL_ATTR_FAMILY_NAME, family)) {
        HILOG_ERROR(LOG_DOMAIN, "%s: put msg failed", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }

    ret = SendCmdSync(msg, FamilyIdHandler, &familyData);
    if (ret == 0) {
        ret = familyData.id;
    }

    nlmsg_free(msg);
    return ret;
}

static int NlsockAddMembership(struct nl_sock *sock, const char *group)
{
    int id;
    int ret;

    id = GetMulticastId(NL80211_GENL_NAME, group);
    if (id < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get multicast id failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    ret = nl_socket_add_membership(sock, id);
    if (ret < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: Could not add multicast membership for %d: %d (%s)",
            __FUNCTION__, id, ret, strerror(-ret));
        return RET_CODE_FAILURE;
    }

    return RET_CODE_SUCCESS;
}

static int32_t ConnectEventSocket(void)
{
    int ret;

    g_wifiHalInfo.eventSock = OpenNetlinkSocket();
    if (g_wifiHalInfo.eventSock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: fail to open event socket", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    ret = NlsockAddMembership(g_wifiHalInfo.eventSock, NL80211_MULTICAST_GROUP_SCAN);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlsock add membership for scan event failed.", __FUNCTION__);
        goto err;
    }

    ret = NlsockAddMembership(g_wifiHalInfo.eventSock, NL80211_MULTICAST_GROUP_MLME);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlsock add membership for mlme failed.\n", __FUNCTION__);
        goto err;
    }

    ret = NlsockAddMembership(g_wifiHalInfo.eventSock, NL80211_MULTICAST_GROUP_REG);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlsock add membership for regulatory failed.\n", __FUNCTION__);
        goto err;
    }

    ret = NlsockAddMembership(g_wifiHalInfo.eventSock, NL80211_MULTICAST_GROUP_VENDOR);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlsock add membership for vendor failed.\n", __FUNCTION__);
        goto err;
    }

    return RET_CODE_SUCCESS;
err:
    CloseNetlinkSocket(g_wifiHalInfo.eventSock);
    return RET_CODE_FAILURE;
}

void DisconnectEventSocket(void)
{
    CloseNetlinkSocket(g_wifiHalInfo.eventSock);
    g_wifiHalInfo.eventSock = NULL;
}

static int32_t WifiMsgRegisterEventListener(void)
{
    int32_t rc;
    int32_t count = 0;
    struct WifiThreadParam threadParam;

    threadParam.eventSock = g_wifiHalInfo.eventSock;
    threadParam.familyId = g_wifiHalInfo.familyId;
    threadParam.status = &g_wifiHalInfo.status;

    g_wifiHalInfo.status = THREAD_STARTING;
    rc = pthread_create(&(g_wifiHalInfo.thread), NULL, EventThread, &threadParam);
    if (rc != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: failed create event thread", __FUNCTION__);
        g_wifiHalInfo.status = THREAD_STOP;
        return RET_CODE_FAILURE;
    }

    // waiting for thread start running
    while (g_wifiHalInfo.status != THREAD_RUN) {
        HILOG_INFO(LOG_DOMAIN, "%s: waiting for thread start running.", __FUNCTION__);
        if (count < RETRIES) {
            count++;
            usleep(WAITFORTHREAD);
        } else {
            HILOG_ERROR(LOG_DOMAIN, "%s: warit for thread running timeout", __FUNCTION__);
            if (g_wifiHalInfo.status != THREAD_STOP) {
                g_wifiHalInfo.status = THREAD_STOP;
                pthread_join(g_wifiHalInfo.thread, NULL);
            }
            return RET_CODE_FAILURE;
        }
    }

    return RET_CODE_SUCCESS;
}

static void WifiMsgUnregisterEventListener(void)
{
    g_wifiHalInfo.status = THREAD_STOPPING;
    pthread_join(g_wifiHalInfo.thread, NULL);
}

int32_t WifiDriverClientInit(void)
{
    if (g_wifiHalInfo.cmdSock != NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: already create cmd socket", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    if (pthread_mutex_init(&g_wifiHalInfo.mutex, NULL) != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: init mutex failed.", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    if (ConnectCmdSocket() != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: connect cmd socket failed.", __FUNCTION__);
        goto err_cmd;
    }

    if (ConnectEventSocket() != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: connect event socket failed\n", __FUNCTION__);
        goto err_event;
    }

    if (WifiMsgRegisterEventListener() != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: WifiMsgRegisterEventListener failed\n", __FUNCTION__);
        goto err_reg;
    }

    return RET_CODE_SUCCESS;
err_reg:
    DisconnectEventSocket();
err_event:
    DisconnectCmdSocket();
err_cmd:
    pthread_mutex_destroy(&g_wifiHalInfo.mutex);
    return RET_CODE_FAILURE;
}

void WifiDriverClientDeinit(void)
{
    WifiMsgUnregisterEventListener();

    if (g_wifiHalInfo.cmdSock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: cmd socket not inited", __FUNCTION__);
    } else {
        DisconnectCmdSocket();
    }

    if (g_wifiHalInfo.eventSock == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: event socket not inited", __FUNCTION__);
    } else {
        DisconnectEventSocket();
    }

    pthread_mutex_destroy(&g_wifiHalInfo.mutex);
}

static int32_t ParserIsSupportCombo(struct nl_msg *msg, void *arg)
{
    struct nlattr *attr[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *nlComb = NULL;
    struct nlattr *attrComb[NUM_NL80211_IFACE_COMB];
    uint8_t *isSupportCombo = (uint8_t *)arg;
    int32_t ret, i;
    static struct nla_policy
    ifaceCombPolicy[NUM_NL80211_IFACE_COMB];
    ifaceCombPolicy[NL80211_IFACE_COMB_LIMITS].type = NLA_NESTED;
    ifaceCombPolicy[NL80211_IFACE_COMB_MAXNUM].type = NLA_U32;
    ifaceCombPolicy[NL80211_IFACE_COMB_NUM_CHANNELS].type = NLA_U32;
    
    // parse all enum nl80211_attrs type
    ret = nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nla_parse tb failed", __FUNCTION__);
        return NL_SKIP;
    }

    if (attr[NL80211_ATTR_INTERFACE_COMBINATIONS] != NULL) {
        nla_for_each_nested(nlComb, attr[NL80211_ATTR_INTERFACE_COMBINATIONS], i) {
            // parse all enum nl80211_if_combination_attrs type
            ret = nla_parse_nested(attrComb, MAX_NL80211_IFACE_COMB,
                nlComb, ifaceCombPolicy);
            if (ret != 0) {
                HILOG_ERROR(LOG_DOMAIN, "%s: nla_parse_nested nlComb failed", __FUNCTION__);
                return NL_SKIP;
            }
            if (!attrComb[NL80211_IFACE_COMB_LIMITS] ||
                !attrComb[NL80211_IFACE_COMB_MAXNUM] ||
                !attrComb[NL80211_IFACE_COMB_NUM_CHANNELS]) {
                    *isSupportCombo = 0;
            } else {
                    *isSupportCombo = 1;
            }
        }
    }
    HILOG_INFO(LOG_DOMAIN, "%s: isSupportCombo is %hhu", __FUNCTION__, *isSupportCombo);
    return NL_SKIP;
}

static int32_t ParserSupportComboInfo(struct nl_msg *msg, void *arg)
{
    (void)arg;
    struct nlattr *attr[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *nlComb = NULL, *nlLimit = NULL, *nlMode = NULL;
    struct nlattr *attrComb[NUM_NL80211_IFACE_COMB];
    struct nlattr *attrLimit[NUM_NL80211_IFACE_LIMIT];
    int32_t ret, i, j, k, type;
    static struct nla_policy ifaceCombPolicy[NUM_NL80211_IFACE_COMB];
    ifaceCombPolicy[NL80211_IFACE_COMB_LIMITS].type  = NLA_NESTED;
    ifaceCombPolicy[NL80211_IFACE_COMB_MAXNUM].type  = NLA_U32;
    ifaceCombPolicy[NL80211_IFACE_COMB_NUM_CHANNELS].type  = NLA_U32;

    static struct nla_policy ifaceLimitPolicy[NUM_NL80211_IFACE_LIMIT];
    ifaceLimitPolicy[NL80211_IFACE_LIMIT_MAX].type  = NLA_U32;
    ifaceLimitPolicy[NL80211_IFACE_LIMIT_TYPES].type  = NLA_NESTED;
    
    ret = nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nla_parse tb failed", __FUNCTION__);
        return NL_SKIP;
    }

    if (attr[NL80211_ATTR_INTERFACE_COMBINATIONS] != NULL) {
        // get each ieee80211_iface_combination
        nla_for_each_nested(nlComb, attr[NL80211_ATTR_INTERFACE_COMBINATIONS], i) {
            ret = nla_parse_nested(attrComb, MAX_NL80211_IFACE_COMB,
                nlComb, ifaceCombPolicy);
            if (ret != 0) {
                HILOG_ERROR(LOG_DOMAIN, "%s: nla_parse_nested nlComb failed", __FUNCTION__);
                return NL_SKIP;
            }
            if (!attrComb[NL80211_IFACE_COMB_LIMITS] ||
                !attrComb[NL80211_IFACE_COMB_MAXNUM] ||
                !attrComb[NL80211_IFACE_COMB_NUM_CHANNELS]) {
                return RET_CODE_NOT_SUPPORT;
            }
            // parse each ieee80211_iface_limit
            nla_for_each_nested(nlLimit, attrComb[NL80211_IFACE_COMB_LIMITS], j) {
                ret = nla_parse_nested(attrLimit, MAX_NL80211_IFACE_LIMIT,
                    nlLimit, ifaceLimitPolicy);
                if (ret || !attrLimit[NL80211_IFACE_LIMIT_TYPES])
                    return RET_CODE_NOT_SUPPORT; /* broken combination */
                // parse each ieee80211_iface_limit's types
                nla_for_each_nested(nlMode, attrLimit[NL80211_IFACE_LIMIT_TYPES], k) {
                    type = nla_type(nlMode);
                    if (type > WIFI_IFTYPE_UNSPECIFIED && type < WIFI_IFTYPE_MAX) {
                        HILOG_INFO(LOG_DOMAIN, "%s: mode: %d", __FUNCTION__, type);
                    }
                }
                HILOG_INFO(LOG_DOMAIN, "%s: has parse a attrLimit", __FUNCTION__);
            }
        }
    }
    return NL_SKIP;
}

struct PrivDevMac {
    uint8_t *mac;
    uint8_t len;
};

static int32_t ParserValidFreq(struct nl_msg *msg, void *arg)
{
    struct FreqInfoResult *result = (struct FreqInfoResult *)arg;
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *attrMsg[NL80211_ATTR_MAX + 1];
    struct nlattr *attrBand[NL80211_BAND_ATTR_MAX + 1];
    struct nlattr *attrFreq[NL80211_FREQUENCY_ATTR_MAX + 1];
    struct nlattr *nlBand = NULL, *nlFreq = NULL;
    int i, j;
    int32_t freq;
    static struct nla_policy freqPolicy[NL80211_FREQUENCY_ATTR_MAX + 1];
    freqPolicy[NL80211_FREQUENCY_ATTR_FREQ].type = NLA_U32;
    freqPolicy[NL80211_FREQUENCY_ATTR_MAX_TX_POWER].type = NLA_U32;

    nla_parse(attrMsg, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);
    if (!attrMsg[NL80211_ATTR_WIPHY_BANDS]) {
        HILOG_ERROR(LOG_DOMAIN, "%s: no wiphy bands", __FUNCTION__);
        return NL_SKIP;
    }
    HILOG_INFO(LOG_DOMAIN, "%s: parse freq 000000000000000000", __FUNCTION__);
    // get each ieee80211_supported_band
    nla_for_each_nested(nlBand, attrMsg[NL80211_ATTR_WIPHY_BANDS], i) {
        nla_parse(attrBand, NL80211_BAND_ATTR_MAX, nla_data(nlBand),
            nla_len(nlBand), NULL);
        if (attrBand[NL80211_BAND_ATTR_FREQS] == NULL)
            continue;
        // get each ieee80211_channel
        nla_for_each_nested(nlFreq, attrBand[NL80211_BAND_ATTR_FREQS], j) {
            nla_parse(attrFreq, NL80211_FREQUENCY_ATTR_MAX,
                nla_data(nlFreq), nla_len(nlFreq), freqPolicy);
            // get center freq
            if (!attrFreq[NL80211_FREQUENCY_ATTR_FREQ] && attrFreq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER])
                continue;
            freq = nla_get_u32(attrFreq[NL80211_FREQUENCY_ATTR_FREQ]);
            switch (result->band) {
                case NL80211_BAND_2GHZ:
                    if (freq > 2400 && freq < 2500) {
                        result->freqs[result->nums] = freq;
                        result->txPower[result->nums] = nla_get_u32(attrFreq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]);
                        result->nums++;
                    }
                    break;
                case NL80211_BAND_5GHZ:
                    if (freq > 5100 && freq < 5900) {
                        result->freqs[result->nums] = freq;
                        result->nums++;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return NL_SKIP;
}

static bool IsWifiIface(const char *name)
{
    if (strncmp(name, "wlan", 4) != 0 && strncmp(name, "p2p", 3) != 0) {
        /* not a wifi interface; ignore it */
        return false;
    } else {
        return true;
    }
}

static int32_t GetAllIfaceInfo(struct NetworkInfoResult *infoResult)
{
    struct dirent *de;

    DIR *d = opendir("/sys/class/net");
    if (d == 0) {
        return RET_CODE_FAILURE;
    }
    infoResult->nums = 0;
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.') {
            continue;
        }
        if (IsWifiIface(de->d_name)) {
            if (strncpy_s(infoResult->infos[infoResult->nums].name, IFNAMSIZ,
                de->d_name, sizeof(de->d_name)) != EOK) {
                HILOG_ERROR(LOG_DOMAIN, "%s: strncpy_s infoResult->infos failed", __FUNCTION__);
                return RET_CODE_FAILURE;
            }
            infoResult->nums++;
        }
    }
    closedir(d);
    if (infoResult->nums == 0)
        return RET_CODE_NOT_AVAILABLE;
    return RET_CODE_SUCCESS;
}

int32_t GetUsableNetworkInfo(struct NetworkInfoResult *result)
{
    int32_t ret;
    uint32_t i;

    ret = GetAllIfaceInfo(result);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: GetAllIfaceInfo failed", __FUNCTION__);
        return ret;
    }

    HILOG_INFO(LOG_DOMAIN, "%s: wifi iface num %d", __FUNCTION__, result->nums);
    for (i = 0; i < result->nums; ++i) {
        ret = memset_s(result->infos[i].supportMode, sizeof(result->infos[i].supportMode),
            0, sizeof(result->infos[i].supportMode));
        if (ret != EOK) {
            HILOG_ERROR(LOG_DOMAIN, "%s: memset_s esult->infos failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
        if (strncmp(result->infos[i].name, STR_WLAN0, STRLEN_WLAN0) == 0) {
            result->infos[i].supportMode[WIFI_IFTYPE_STATION] = 1;
            result->infos[i].supportMode[WIFI_IFTYPE_AP] = 1;
        } else if (strncmp(result->infos[i].name, STR_WLAN1, STRLEN_WLAN1) == 0) {
            result->infos[i].supportMode[WIFI_IFTYPE_STATION] = 1;
        } else if (strncmp(result->infos[i].name, STR_P2P0, STRLEN_P2P0) == 0) {
            result->infos[i].supportMode[WIFI_IFTYPE_P2P_DEVICE] = 1;
        } else if (strncmp(result->infos[i].name, STR_P2P0_X, STRLEN_P2P0_X) == 0) {
            result->infos[i].supportMode[WIFI_IFTYPE_P2P_CLIENT] = 1;
            result->infos[i].supportMode[WIFI_IFTYPE_P2P_GO] = 1;
        }
    }
    return RET_CODE_SUCCESS;
}

int32_t IsSupportCombo(uint8_t *isSupportCombo)
{
    uint32_t ifaceId;
    struct nl_msg *msg = NULL;
    struct NetworkInfoResult networkInfo;
    int32_t ret;

    ret = GetUsableNetworkInfo(&networkInfo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get network info failed", __FUNCTION__);
        return ret;
    }

    ifaceId = if_nametoindex(networkInfo.infos[0].name);
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get iface id(%u) failed", __FUNCTION__, ifaceId);
        return RET_CODE_FAILURE;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __FUNCTION__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, NLM_F_DUMP,
        NL80211_CMD_GET_WIPHY, 0);
    nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    ret = SendCmdSync(msg, ParserIsSupportCombo, isSupportCombo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nlmsg_free(msg);

    return RET_CODE_SUCCESS;
}

int32_t GetComboInfo(uint64_t *comboInfo, uint32_t size)
{
    (void)size;
    uint32_t ifaceId;
    struct nl_msg *msg = NULL;
    struct NetworkInfoResult networkInfo;
    int32_t ret;

    ret = GetUsableNetworkInfo(&networkInfo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get network info failed", __FUNCTION__);
        return ret;
    }

    ifaceId = if_nametoindex(networkInfo.infos[0].name);
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get iface id(%d) failed", __FUNCTION__, ifaceId);
        return RET_CODE_FAILURE;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __FUNCTION__);
        return RET_CODE_NOMEM;
    }
    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, NLM_F_DUMP,
        NL80211_CMD_GET_WIPHY, 0);
    nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    ret = SendCmdSync(msg, ParserSupportComboInfo, comboInfo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nlmsg_free(msg);
    return RET_CODE_SUCCESS;
}

int32_t SetMacAddr(const char *ifName, unsigned char *mac, uint8_t len)
{
    int32_t fd;
    int32_t ret;
    struct ifreq req;

    fd = socket(PF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: open socket failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    strncpy_s(req.ifr_name, IFNAMSIZ, ifName, strlen(ifName));
    req.ifr_addr.sa_family = ARPHRD_ETHER;
    if (memcpy_s(req.ifr_hwaddr.sa_data, len, mac, len) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy_s req.ifr_hwaddr.sa_data failed", __FUNCTION__);
        close(fd);
        return RET_CODE_FAILURE;
    }

    ret = ioctl(fd, SIOCSIFHWADDR, &req);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: ioctl failed, errno = %d, (%s)\n",
            __FUNCTION__, errno, strerror(errno));
        printf("%s: octl failed, errno = %d, (%s)\n", __FUNCTION__, errno, strerror(errno));
        ret = RET_CODE_FAILURE;
    }
    close(fd);
    return ret;
}

static int32_t ParserChipId(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *attr[NL80211_ATTR_MAX + 1];
    uint8_t *chipId = (uint8_t *)arg;
    uint8_t *getChipId = NULL;
    int32_t ret;

    ret = nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nla_parse failed", __FUNCTION__);
        return NL_SKIP;
    }

    if (attr[NL80211_ATTR_MAX]) {
        getChipId = nla_data(attr[NL80211_ATTR_MAX]);
        *chipId = *getChipId;
    }

    return NL_SKIP;
}

int32_t GetDevMacAddr(const char *ifName,
    int32_t type, uint8_t *mac, uint8_t len)
{
    (void)type;
    int fd, ret;
    struct ifreq req;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: open socket failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    req.ifr_addr.sa_family = AF_INET;
    strncpy_s(req.ifr_name, IFNAMSIZ, ifName, strlen(ifName));
    ret = ioctl(fd, SIOCGIFHWADDR, &req);
    if (ret != 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: ioctl failed, errno = %d, (%s)\n",
            __FUNCTION__, errno, strerror(errno));
        close(fd);
        return RET_CODE_FAILURE;
    }
    if (memcpy_s(mac, len, (unsigned char *)req.ifr_hwaddr.sa_data, len) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy_s mac failed", __FUNCTION__);
        ret = RET_CODE_FAILURE;
    }
    close(fd);
    return ret;
}

int32_t GetValidFreqByBand(const char *ifName, int32_t band, struct FreqInfoResult *result, uint32_t size)
{
    uint32_t ifaceId;
    struct nl_msg *msg = NULL;
    int32_t ret;

    if (result == NULL || result->freqs == NULL || result->txPower == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s:  Invalid input parameter", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }

    ifaceId = if_nametoindex(ifName);
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get iface id(%d) failed", __FUNCTION__, ifaceId);
        return RET_CODE_INVALID_PARAM;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __FUNCTION__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, NLM_F_DUMP,
        NL80211_CMD_GET_WIPHY, 0);
    nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    ret = memset_s(result->freqs, size * sizeof(int32_t), 0, sizeof(result->freqs));
    if (ret != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memset_s result->freqs  failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    result->nums = 0;
    result->band = band;
    ret = SendCmdSync(msg, ParserValidFreq, result);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nlmsg_free(msg);
    return RET_CODE_SUCCESS;
}

int32_t SetTxPower(const char *ifName, int32_t power)
{
    uint32_t ifaceId;
    struct nl_msg *msg = NULL;
    int32_t ret;

    ifaceId = if_nametoindex(ifName);
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get iface id(%d) failed", __func__, ifaceId);
        return RET_CODE_INVALID_PARAM;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __func__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, 0,
        NL80211_CMD_SET_WIPHY, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    nla_put_u32(msg, NL80211_ATTR_WIPHY_TX_POWER_SETTING, NL80211_TX_POWER_LIMITED);
    nla_put_u32(msg, NL80211_ATTR_WIPHY_TX_POWER_LEVEL, 100 * power);
    ret = SendCmdSync(msg, NULL, NULL);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __func__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nlmsg_free(msg);
    HILOG_INFO(LOG_DOMAIN, "%s: send end success", __func__);
    return RET_CODE_SUCCESS;
}

int32_t GetAssociatedStas(const char *ifName, struct AssocStaInfoResult *result)
{
    (void)ifName;
    if (memset_s(result, sizeof(struct AssocStaInfoResult), 0, sizeof(struct AssocStaInfoResult)) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memset_s result failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    return RET_CODE_SUCCESS;
}

int32_t WifiSetCountryCode(const char *ifName, const char *code, uint32_t len)
{
    uint32_t ifaceId = if_nametoindex(ifName);
    struct nl_msg *msg = NULL;
    struct nlattr *data = NULL;
    int32_t ret;

    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: if_nametoindex failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __FUNCTION__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, 0, NL80211_CMD_VENDOR, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, VENDOR_ID);
    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD, WIFI_SUBCMD_SET_COUNTRY_CODE);
    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (data == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nla_nest_start failed", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nla_put(msg, WIFI_ATTRIBUTE_COUNTRY, len, code);
    nla_nest_end(msg, data);

    ret = SendCmdSync(msg, NULL, NULL);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __FUNCTION__);
    }
    nlmsg_free(msg);
    return ret;
}

int32_t SetScanMacAddr(const char *ifName, uint8_t *scanMac, uint8_t len)
{
    int32_t ret;
    uint32_t ifaceId = if_nametoindex(ifName);
    struct nl_msg *msg = nlmsg_alloc();
    struct nlattr *data = NULL;

    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __func__);
        return RET_CODE_NOMEM;
    }
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: if_nametoindex failed", __func__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, 0,
        NL80211_CMD_VENDOR, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);
    nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, VENDOR_ID);
    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD, WIFI_SUBCMD_SET_RANDOM_MAC_OUI);
    data = nla_nest_start(msg, NL80211_ATTR_VENDOR_DATA);
    if (data == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nla_nest_start failed", __func__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }
    nla_put(msg, ANDR_WIFI_ATTRIBUTE_RANDOM_MAC_OUI, len, scanMac);
    nla_nest_end(msg, data);
    ret = SendCmdSync(msg, NULL, NULL);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed", __func__);
    }
    nlmsg_free(msg);
    return ret;
}

int32_t AcquireChipId(const char *ifName, uint8_t *chipId)
{
    struct nl_msg *msg = NULL;
    uint32_t ifaceId;
    int32_t ret;

    if (ifName == NULL || chipId == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }

    ifaceId = if_nametoindex(ifName);
    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: if_nametoindex failed", __func__);
        return RET_CODE_FAILURE;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __func__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, NLM_F_DUMP, NL80211_CMD_GET_WIPHY, 0);
    nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);

    ret = SendCmdSync(msg, ParserChipId, chipId);

    return RET_CODE_SUCCESS;
}

int32_t GetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    (void)chipId;

    if (ifNames == NULL || num == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s params is NULL", __FUNCTION__);
        return RET_CODE_INVALID_PARAM;
    }
    *num = 1;
    *ifNames = (char *)calloc(*num, IFNAMSIZ);
    if (*ifNames == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: calloc failed", __FUNCTION__);
        return RET_CODE_FAILURE;
    }
    if (memcpy_s(*ifNames, IFNAMSIZ, "wlan0", IFNAMSIZ) != EOK) {
        HILOG_ERROR(LOG_DOMAIN, "%s: memcpy failed", __FUNCTION__);
        free(*ifNames);
        *ifNames = NULL;
        return RET_CODE_FAILURE;
    }
    return RET_CODE_SUCCESS;
}

int32_t SetResetDriver(const uint8_t chipId, const char *ifName)
{
    (void)chipId;
    (void)ifName;
    return RET_CODE_SUCCESS;
}

static int NetDeviceInfoHandler(struct nl_msg *msg, void *arg)
{
    struct nlattr *attr[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *hdr = nlmsg_data(nlmsg_hdr(msg));
    struct NetDeviceInfo *info = (struct NetDeviceInfo *)arg;

    nla_parse(attr, NL80211_ATTR_MAX, genlmsg_attrdata(hdr, 0),
        genlmsg_attrlen(hdr, 0), NULL);

    if (attr[NL80211_ATTR_IFTYPE]) {
        info->iftype = nla_get_u32(attr[NL80211_ATTR_IFTYPE]);
        HILOG_ERROR(LOG_DOMAIN, "%s: %s iftype is %hhu\n", __FUNCTION__, info->ifName, info->iftype);
    }

    if (attr[NL80211_ATTR_MAC]) {
        memcpy_s(info->mac, ETH_ADDR_LEN, nla_data(attr[NL80211_ATTR_MAC]), ETH_ADDR_LEN);
    }

    return NL_SKIP;
}

static uint32_t GetIftypeAndMac(struct NetDeviceInfo *info)
{
    struct nl_msg *msg = nlmsg_alloc();
    int ret;

    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg_alloc failed.\n", __FUNCTION__);
        return RET_CODE_FAILURE;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, 0, NL80211_CMD_GET_INTERFACE, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(info->ifName));

    ret = SendCmdSync(msg, NetDeviceInfoHandler, info);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: SendCmdSync failed.\n", __FUNCTION__);
        nlmsg_free(msg);
        return RET_CODE_FAILURE;
    }

    nlmsg_free(msg);
    return RET_CODE_SUCCESS;
}

int32_t GetNetDeviceInfo(struct NetDeviceInfoResult *netDeviceInfoResult)
{
    struct NetworkInfoResult networkInfo;
    uint32_t i;
    int32_t ret;

    ret = GetUsableNetworkInfo(&networkInfo);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: get network info failed", __FUNCTION__);
        return ret;
    }

    for (i = 0; i < networkInfo.nums && i < MAX_NETDEVICE_COUNT; i++) {
        memset_s(&netDeviceInfoResult->deviceInfos[i], sizeof(struct NetDeviceInfo), 0, sizeof(struct NetDeviceInfo));
        netDeviceInfoResult->deviceInfos[i].index = i + 1;
        strncpy_s(netDeviceInfoResult->deviceInfos[i].ifName, IFNAMSIZ, networkInfo.infos[i].name, IFNAMSIZ);
        ret = GetIftypeAndMac(&netDeviceInfoResult->deviceInfos[i]);
        if (ret != RET_CODE_SUCCESS) {
            HILOG_ERROR(LOG_DOMAIN, "%s: get iftype and mac failed", __FUNCTION__);
            return RET_CODE_FAILURE;
        }
    }

    return RET_CODE_SUCCESS;
}

static int CmdScanPutMsg(struct nl_msg *msg, const WifiScan *scan)
{
    struct nlattr *nest = NULL;
    int i;

    if (scan->ssids) {
        nest = nla_nest_start(msg, NL80211_ATTR_SCAN_SSIDS);
        if (nest == NULL) {
            HILOG_ERROR(LOG_DOMAIN, "%s: nla_nest_start failed", __func__);
            return RET_CODE_FAILURE;
        }
        for (i = 0; i < scan->numSsids; i++) {
            nla_put(msg, i + 1, scan->ssids->ssidLen, scan->ssids->ssid);
        }
        nla_nest_end(msg, nest);
    }

    if (scan->freqs) {
        nest = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
        if (nest == NULL) {
            HILOG_ERROR(LOG_DOMAIN, "%s: nla_nest_start failed", __func__);
            return RET_CODE_FAILURE;
        }
        for (i = 0; i < scan->numFreqs; i++) {
            nla_put_u32(msg, i + 1, scan->freqs[i]);
        }
        nla_nest_end(msg, nest);
    }

    if (scan->extraIes) {
        nla_put(msg, NL80211_ATTR_IE, scan->extraIesLen, scan->extraIes);
    }

    if (scan->bssid) {
        nla_put(msg, NL80211_ATTR_MAC, ETH_ADDR_LEN, scan->bssid);
    }

    return RET_CODE_SUCCESS;
}

int32_t WifiCmdScan(const char *ifName, WifiScan *scan)
{
    uint32_t ifaceId = if_nametoindex(ifName);
    struct nl_msg *msg = NULL;
    int32_t ret;

    if (ifaceId == 0) {
        HILOG_ERROR(LOG_DOMAIN, "%s: if_nametoindex failed", __func__);
        return RET_CODE_FAILURE;
    }

    msg = nlmsg_alloc();
    if (msg == NULL) {
        HILOG_ERROR(LOG_DOMAIN, "%s: nlmsg alloc failed", __func__);
        return RET_CODE_NOMEM;
    }

    genlmsg_put(msg, 0, 0, g_wifiHalInfo.familyId, 0, 0, NL80211_CMD_TRIGGER_SCAN, 0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, ifaceId);

    ret = CmdScanPutMsg(msg, scan);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: put msg failed\n", __func__);
        goto err;
    }

    ret = SendCmdSync(msg, NULL, NULL);
    if (ret != RET_CODE_SUCCESS) {
        HILOG_ERROR(LOG_DOMAIN, "%s: send cmd failed\n", __func__);
        goto err;
    }

    return RET_CODE_SUCCESS;
err:
    nlmsg_free(msg);
    return RET_CODE_FAILURE;
}
