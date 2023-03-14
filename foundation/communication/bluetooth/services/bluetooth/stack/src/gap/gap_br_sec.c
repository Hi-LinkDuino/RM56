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

#include "gap.h"
#include "gap_internal.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"
#include "thread.h"

#include "btm.h"
#include "btm/btm_interop.h"
#include "btm/btm_thread.h"

typedef struct {
    GapSecurityCallback callback;
    void *context;
} SecurityCallback;

typedef struct {
    GapAuthenticationCallback callback;
    void *context;
} AuthenticationCallback;

static SecurityCallback g_securityCallback;
static AuthenticationCallback g_authenticationCallback;

static int GapAuthenticationRequested(uint16_t handle);
static int GapSetConnectionEncryption(uint16_t handle, uint8_t encryptionEnable);

static bool GapCompareServiceSecurityInfo(const GapServiceSecurityInfo *info1, const GapServiceSecurityInfo *info2)
{
    return ((info1->direction == info2->direction) && (info1->protocolId == info2->protocolId) &&
            (GapCompareChannelID(info2->protocolId, info1->channelId, info2->channelId)) &&
            ((info1->serviceId == info2->serviceId) || (info1->serviceId == UNKNOWN_SERVICE) ||
                (info2->serviceId == UNKNOWN_SERVICE)));
}

static RequestSecInfo *GapAllocReqSecInfo(
    const BtAddr *addr, GapSecurityResultCallback callback, void *context, const ProfileSecurityInfo *info)
{
    RequestSecInfo *reqInfo = NULL;

    reqInfo = MEM_MALLOC.alloc(sizeof(RequestSecInfo));
    if (reqInfo == NULL) {
        LOG_ERROR("%{public}s:malloc error.", __FUNCTION__);
    } else {
        (void)memset_s(reqInfo, sizeof(RequestSecInfo), 0x00, sizeof(RequestSecInfo));
        reqInfo->addr = *addr;
        reqInfo->callback = callback;
        reqInfo->info = info->info;
        reqInfo->context = context;
        reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_ACTION;
        if (info->info.direction == OUTGOING) {
            reqInfo->needAuthentication = !!(info->securityMode & GAP_SEC_OUT_AUTHENTICATION);
            reqInfo->needEncryption = !!(info->securityMode & GAP_SEC_OUT_ENCRYPTION);
            reqInfo->needAuthorization = false;
            if (*GapGetSecurityMode() != SEC_MODE_2) {
                reqInfo->needEncryption = true;
            }
        } else if (info->info.direction == INCOMING) {
            reqInfo->needAuthentication = !!(info->securityMode & GAP_SEC_IN_AUTHENTICATION);
            reqInfo->needEncryption = !!(info->securityMode & GAP_SEC_IN_ENCRYPTION);
            reqInfo->needAuthorization = !!(info->securityMode & GAP_SEC_IN_AUTHORIZATION);
        }
        reqInfo->needUnauthentication = reqInfo->needEncryption && !reqInfo->needAuthentication;
        reqInfo->needMITM = !!(info->securityMode & GAP_SEC_MITM);
    }

    return reqInfo;
}

NO_SANITIZE("cfi") void GapDoAuthenticationCallback(const void *req)
{
    const RequestSecInfo *reqInfo = req;
    BtAddr addr = BT_ADDR_NULL;
    uint8_t result = GAP_STATUS_FAILED;
    if (reqInfo != NULL && reqInfo->status == GAP_SEC_REQ_STATUS_SUCCESS) {
        result = GAP_STATUS_SUCCESS;
    }
    if (reqInfo != NULL) {
        addr = reqInfo->addr;
    }

    if (g_authenticationCallback.callback.authenticationComplete) {
        g_authenticationCallback.callback.authenticationComplete(&addr, result, g_authenticationCallback.context);
    }
}

static void GapTrySecurityCallback(RequestSecInfo *reqInfo)
{
    if (!reqInfo->doCallback) {
        reqInfo->doCallback = true;
        GapDoSecurityCallback(reqInfo);
    }
}

NO_SANITIZE("cfi") void GapDoSecurityCallback(void *req)
{
    RequestSecInfo *reqInfo = (RequestSecInfo *)req;
    uint16_t result = GAP_STATUS_FAILED;

    if (GapIsBredrEnable() == false) {
        return;
    }

    if (reqInfo->status == GAP_SEC_REQ_STATUS_SUCCESS) {
        result = GAP_STATUS_SUCCESS;
    } else if (reqInfo->hciStatus != HCI_SUCCESS) {
        result = reqInfo->hciStatus;
    }

    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s result:%02x:",
        __FUNCTION__,
        reqInfo->info.direction ? "IN" : "OUT",
        reqInfo->info.serviceId,
        reqInfo->info.protocolId ? "RFCOMM" : "L2CAP",
        result);

    if (reqInfo->callback) {
        reqInfo->callback(result, reqInfo->info, reqInfo->context);
    }

    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    ListRemoveNode(profileSecurityBlock->requestlist, req);
}

static void GapDoAuthorizationCallback(const void *req)
{
    const RequestSecInfo *reqInfo = req;
    bool reject = false;

    if (g_securityCallback.callback.authorizeInd) {
        g_securityCallback.callback.authorizeInd(&reqInfo->addr, reqInfo->info.serviceId, reqInfo->context);
    } else {
        reject = true;
    }

    if (reject) {
        GAP_AuthorizeRes(&reqInfo->addr, reqInfo->info.serviceId, GAP_NOT_ACCEPT);
    }
}

static void GapDoSecurityAction(RequestSecInfo *reqInfo, DeviceInfo *devInfo)
{
    int ret;

    if (devInfo->status != GAP_DEV_SEC_STATUS_ACTION) {
        if (devInfo->status == GAP_DEV_SEC_STATUS_IDLE) {
            BTM_AclAddRef(devInfo->handle);
        } else {
            AlarmCancel(devInfo->alarm);
        }
        devInfo->status = GAP_DEV_SEC_STATUS_ACTION;
    }
    if (reqInfo->needAuthorization) {
        GapDoAuthorizationCallback(reqInfo);
    } else if (reqInfo->needAuthentication || reqInfo->needUnauthentication) {
        if (devInfo->authenticationStatus == GAP_AUTH_STATUS_IDLE) {
            devInfo->authenticationStatus = GAP_AUTH_STATUS_ACTION;
        }
        ret = GapAuthenticationRequested(devInfo->handle);
        if (ret != BT_NO_ERROR) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            devInfo->status = GAP_DEV_SEC_STATUS_IDLE;
            devInfo->authenticationStatus = GAP_AUTH_STATUS_IDLE;
            GapDoAuthenticationCallback(reqInfo);
            GapTrySecurityCallback(reqInfo);
        }
    } else if (reqInfo->needEncryption) {
        if (devInfo->encryptionStatus == GAP_ENC_STATUS_IDLE) {
            devInfo->encryptionStatus = GAP_ENC_STATUS_ACTION;
        }
        ret = GapSetConnectionEncryption(devInfo->handle, LINK_LEVEL_ENCRYPTION_ON);
        if (ret != BT_NO_ERROR) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            devInfo->status = GAP_DEV_SEC_STATUS_IDLE;
            devInfo->encryptionStatus = GAP_ENC_STATUS_IDLE;
            GapTrySecurityCallback(reqInfo);
        }
    } else {
        devInfo->status = GAP_DEV_SEC_STATUS_IDLE;
    }
}

static void GapDoReleaseACLConnection(const void *dev)
{
    const DeviceInfo *devInfo = dev;

    ListNode *node = ListGetFirstNode(GapGetConnectionInfoBlock()->devicelist);
    while (node != NULL) {
        if (dev == ListGetNodeData(node)) {
            break;
        }
        node = ListGetNextNode(node);
    }

    if (node != NULL) {
        BTM_AclRelease(devInfo->handle);
    }
}

static void GapDiscACLTimerTimeoutTask(void *ctx)
{
    const DeviceInfo *dev = ((const GapGeneralPointerInfo *)ctx)->pointer;

    if (GapIsBredrEnable()) {
        GapDoReleaseACLConnection(dev);
    }
}

static void GapDiscACLTimerTimeout(void *dev)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    ctx->pointer = dev;

    int ret = GapRunTaskUnBlockProcess(GapDiscACLTimerTimeoutTask, ctx, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapCheckConnection(void)
{
    ListNode *node = NULL;
    ConnectionInfoBlock *connectionInfoBlock = NULL;
    DeviceInfo *devInfo = NULL;
    connectionInfoBlock = GapGetConnectionInfoBlock();
    node = ListGetFirstNode(connectionInfoBlock->devicelist);
    while (node != 0) {
        devInfo = ListGetNodeData(node);
        if (devInfo->status == GAP_DEV_SEC_STATUS_WAIT_DISC) {
            LOG_DEBUG("%{public}s: " BT_ADDR_FMT " 4s disconnect ACL", __FUNCTION__, BT_ADDR_FMT_OUTPUT(devInfo->addr.addr));
            AlarmCancel(devInfo->alarm);
            AlarmSet(devInfo->alarm, GAP_DISC_ACL_WAIT_TIME, GapDiscACLTimerTimeout, devInfo);
            devInfo->status = GAP_DEV_SEC_STATUS_IDLE;
        }
        node = ListGetNextNode(node);
    }
}

void GapRequestSecurityProcess(void)
{
    ListNode *requestNode = ListGetFirstNode(GapGetProfileSecurityBlock()->requestlist);
    while (requestNode != 0) {
        RequestSecInfo *reqInfo = ListGetNodeData(requestNode);
        requestNode = ListGetNextNode(requestNode);

        if (reqInfo->status == GAP_SEC_REQ_STATUS_FAILED) {
            GapTrySecurityCallback(reqInfo);
            continue;
        }

        if (reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_CONNECT || reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_FEATURE ||
            reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_ENCRYPT) {
            continue;
        }

        DeviceInfo *devInfo =
            ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, &reqInfo->addr);
        if (devInfo == NULL) {
            continue;
        }

        if (reqInfo->info.serviceId == GAP) {
            devInfo->inDedicatedBonding = true;
        } else {
            if (devInfo->isAuthentication) {
                reqInfo->needAuthentication = false;
                reqInfo->needUnauthentication = false;
            }
            if (devInfo->isEncryption) {
                reqInfo->needEncryption = false;
            }
        }

        if (reqInfo->needAuthorization) {
            reqInfo->status = GAP_SEC_REQ_STATUS_ACTION;
            GapDoSecurityAction(reqInfo, devInfo);
        } else if (reqInfo->needAuthentication || reqInfo->needUnauthentication || reqInfo->needEncryption) {
            if (devInfo->actionReq != NULL) {
                continue;
            } else {
                reqInfo->status = GAP_SEC_REQ_STATUS_ACTION;
                devInfo->actionReq = reqInfo;
                GapDoSecurityAction(reqInfo, devInfo);
            }
        } else {
            reqInfo->status = GAP_SEC_REQ_STATUS_SUCCESS;
            GapTrySecurityCallback(reqInfo);
        }
    }

    GapCheckConnection();
}

static void GapDoDiscACLConnection(void *dev)
{
    DeviceInfo *deviceInfo = dev;

    ListNode *node = ListGetFirstNode(GapGetConnectionInfoBlock()->devicelist);
    while (node != NULL) {
        if (deviceInfo == ListGetNodeData(node)) {
            break;
        }
        node = ListGetNextNode(node);
    }

    if (node != NULL) {
        BTM_AclDisconnect(deviceInfo->handle, GAP_HCI_DISC_REASON_AUTH_FAIL);
    }
}

static void GapWaitEncryptTimeoutTask(void *ctx)
{
    DeviceInfo *dev = ((GapGeneralPointerInfo *)ctx)->pointer;

    GapDoDiscACLConnection(dev);
    GapUpdateSecurityRequest(dev, GAP_SEC_EVENT_WAIT_ENC_TIMEOUT, HCI_SUCCESS);
    GapRequestSecurityProcess();
}

static void GapWaitEncryptTimeout(void *dev)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    ctx->pointer = dev;

    int ret = GapRunTaskUnBlockProcess(GapWaitEncryptTimeoutTask, ctx, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

void GapRemoteDeviceSupportHostSecureSimplePairingCallback(const BtAddr *addr, bool support)
{
    DeviceInfo *devInfo =
        ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL) {
        devInfo->supportSSP = support;
        GapUpdateSecurityRequest(devInfo, GAP_SEC_EVENT_FEATURE_COMP, HCI_SUCCESS);
        if (devInfo->waitEncryptAlarm != NULL) {
            AlarmSet(devInfo->waitEncryptAlarm, GAP_SEC_WAIT_ENCRYPT_TIME, GapWaitEncryptTimeout, devInfo);
        }
        GapRequestSecurityProcess();
    }
}

int GAP_RequestSecurity(const BtAddr *addr, const GapRequestSecurityParam *param)
{
    int ret = GAP_SUCCESS;
    ProfileSecurityInfo *regInfo = NULL;

    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s",
        __FUNCTION__,
        param->info.direction ? "IN" : "OUT",
        param->info.serviceId,
        param->info.protocolId ? "RFCOMM" : "L2CAP");

    do {
        ListNode *node = ListGetFirstNode(GapGetProfileSecurityBlock()->registerlist);
        while (node != 0) {
            regInfo = ListGetNodeData(node);
            if ((GapIsEmptyAddr(regInfo->addr.addr) || GapAddrCompare(&regInfo->addr, addr)) &&
                GapCompareServiceSecurityInfo(&regInfo->info, &param->info)) {
                break;
            }
            node = ListGetNextNode(node);
        }

        if (node == NULL) {
            LOG_ERROR("%{public}s:Register(%{public}d, %{public}d)not found.", __FUNCTION__, param->info.serviceId, param->info.direction);
            ret = GAP_ERR_INVAL_PARAM;
            break;
        }

        RequestSecInfo *reqInfo = GapAllocReqSecInfo(addr, param->callback, param->context, regInfo);
        if (reqInfo == NULL) {
            LOG_ERROR("%{public}s:reqInfo alloc failed.", __FUNCTION__);
            ret = GAP_ERR_OUT_OF_RES;
            break;
        }
        ListAddLast(GapGetProfileSecurityBlock()->requestlist, reqInfo);

        DeviceInfo *deviceInfo =
            ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
        if (deviceInfo != NULL) {
            if (*GapGetSecurityMode() != SEC_MODE_2) {
                GapIsRemoteDeviceSupportHostSecureSimplePairingAsync(addr);
                reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_FEATURE;
            }
            GapRequestSecurityProcess();
        } else {
            ret = BTM_AclConnect(addr);
            reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_CONNECT;
        }
    } while (0);

    if (ret != BT_NO_ERROR) {
        param->callback(GAP_STATUS_FAILED, param->info, param->context);
        ret = BT_NO_ERROR;
    }

    return ret;
}

static int GapWriteSimplePairingMode(uint8_t simplePairingMode)
{
    int ret;

    if (BTM_IsControllerSupportSecureSimplePairing()) {
        HciWriteSimplePairingModeParam hciCmdParam = {
            .simplePairingMode = simplePairingMode,
        };
        ret = HCI_WriteSimplePairingMode(&hciCmdParam);
    } else {
        ret = GAP_ERR_NOT_SUPPORT;
    }

    return ret;
}

static int GapWriteSecureConnectionsHostSupport(uint8_t support)
{
    int ret;
    if (BTM_IsControllerSupportSecureConnections()) {
        HciWriteSecureConnectionsHostSupportParam hciCmdParam = {
            .secureConnectionsHostSupport = support,
        };

        ret = HCI_WriteSecureConnectionsHostSupport(&hciCmdParam);
    } else {
        ret = GAP_SUCCESS;
    }

    return ret;
}

int GAP_RegisterSecurityCallback(const GapSecurityCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_securityCallback.callback,
            sizeof(g_securityCallback.callback),
            0x00,
            sizeof(g_securityCallback.callback));
    } else {
        g_securityCallback.callback = *callback;
    }
    g_securityCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterSecurityCallback(void)
{
    (void)memset_s(
        &g_securityCallback.callback, sizeof(g_securityCallback.callback), 0x00, sizeof(g_securityCallback.callback));
    g_securityCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_SetSecurityMode(GAP_SecurityMode mode)
{
    LOG_INFO("%{public}s:mode[%{public}d]", __FUNCTION__, mode);

    if (mode != SEC_MODE_2 && mode != SEC_MODE_4) {
        return GAP_ERR_INVAL_PARAM;
    }

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    int ret;
    if (mode == SEC_MODE_4) {
        ret = GapWriteSimplePairingMode(SIMPLE_PAIRING_ENABLED);
        if (ret == BT_NO_ERROR) {
            ret = GapWriteSecureConnectionsHostSupport(SUPPORT_SECURE_CONNECTIONS);
        }
    } else {
        ret = GapWriteSimplePairingMode(SIMPLE_PAIRING_DISABLED);
        ret |= GapWriteSecureConnectionsHostSupport(UNSUPPORT_SECURE_CONNECTIONS);
    }

    if (ret == BT_NO_ERROR) {
        GAP_SecurityMode *secMode = GapGetSecurityMode();
        *secMode = mode;
    }

    return GAP_SUCCESS;
}

int GAP_AuthorizeRes(const BtAddr *addr, GAP_Service service, uint8_t accept)
{
    int ret = GAP_SUCCESS;
    ProfileSecurityBlock *profileSecurityBlock = NULL;
    ConnectionInfoBlock *connectionInfoBlock = NULL;
    ListNode *node = NULL;
    RequestSecInfo *reqInfo = NULL;
    DeviceInfo *devInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT " %{public}d, accept[%hhu]", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), service, accept);

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    profileSecurityBlock = GapGetProfileSecurityBlock();
    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (reqInfo->info.direction == OUTGOING) {
            node = ListGetNextNode(node);
            continue;
        }
        if (reqInfo->info.serviceId == service && reqInfo->status == GAP_SEC_REQ_STATUS_ACTION) {
            reqInfo->needAuthorization = false;
            break;
        } else {
            ret = GAP_ERR_INVAL_STATE;
        }
        node = ListGetNextNode(node);
    }

    connectionInfoBlock = GapGetConnectionInfoBlock();
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL) {
        if (devInfo->status == GAP_DEV_SEC_STATUS_ACTION) {
            devInfo->status = GAP_DEV_SEC_STATUS_WAIT_DISC;
        }
    }

    GapRequestSecurityProcess();
    return ret;
}

static bool GapDeviceLinkkeyIsAuthenticated(const DeviceInfo *deviceInfo)
{
    if (deviceInfo->linkkeyType == AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192 ||
        deviceInfo->linkkeyType == AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256 ||
        deviceInfo->linkkeyType == COMBINATION_KEY) {
        return true;
    } else {
        return false;
    }
}

static void GapUpdateSecurityStatusUnauthenticationSuccess(const DeviceInfo *devInfo)
{
    if (devInfo->actionReq != NULL) {
        if (devInfo->actionReq->needAuthentication) {
            devInfo->actionReq->needAuthentication = false;
            if (devInfo->actionReq->needMITM) {
                devInfo->actionReq->status = GAP_SEC_REQ_STATUS_FAILED;
            } else {
                devInfo->actionReq->status = GAP_SEC_REQ_STATUS_SUCCESS;
            }
        }
        if (devInfo->actionReq->needUnauthentication) {
            devInfo->actionReq->needUnauthentication = false;
        }
    }
}

static void GapUpdateSecurityStatusAuthenticationSuccess(const DeviceInfo *devInfo)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->needAuthentication) {
            reqInfo->needAuthentication = false;
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusEncryptionSuccess(const DeviceInfo *devInfo)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->needEncryption) {
            reqInfo->needEncryption = false;
        }
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_ENCRYPT) {
            reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_ACTION;
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusFailed(const DeviceInfo *devInfo, uint8_t hciStatus)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_ACTION) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            reqInfo->hciStatus = hciStatus;
        }
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_ENCRYPT) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            reqInfo->hciStatus = hciStatus;
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusEncryptionTimeout(const DeviceInfo *devInfo)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_ENCRYPT) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusAclDisconnect(const DeviceInfo *devInfo, uint8_t hciStatus)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr)) {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            reqInfo->hciStatus = hciStatus;
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusConnectComplete(const DeviceInfo *devInfo)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != 0) {
        reqInfo = ListGetNodeData(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_CONNECT) {
            if (*GapGetSecurityMode() == SEC_MODE_2) {
                reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_ACTION;
            } else {
                reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_FEATURE;
            }
        }
        node = ListGetNextNode(node);
    }
}

static void GapUpdateSecurityStatusFeatureComplete(DeviceInfo *devInfo)
{
    ListNode *node = NULL;
    ProfileSecurityBlock *profileSecurityBlock = GapGetProfileSecurityBlock();
    RequestSecInfo *reqInfo = NULL;

    node = ListGetFirstNode(profileSecurityBlock->requestlist);
    while (node != NULL) {
        reqInfo = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if (GapAddrCompare(&devInfo->addr, &reqInfo->addr) && reqInfo->status == GAP_SEC_REQ_STATUS_WAIT_FEATURE) {
            if (!devInfo->supportSSP || reqInfo->info.direction == OUTGOING || devInfo->isEncryption) {
                reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_ACTION;
                continue;
            }
            reqInfo->status = GAP_SEC_REQ_STATUS_WAIT_ENCRYPT;
            if (devInfo->waitEncryptAlarm == NULL) {
                devInfo->waitEncryptAlarm = AlarmCreate("wait encrypt", false);
            }
        }
    }
}

void GapUpdateSecurityRequest(DeviceInfo *devInfo, enum DeviceSecurityEvent event, uint8_t hciStatus)
{
    switch (event) {
        case GAP_SEC_EVENT_CONNECT_COMP:
            GapUpdateSecurityStatusConnectComplete(devInfo);
            break;
        case GAP_SEC_EVENT_FEATURE_COMP:
            GapUpdateSecurityStatusFeatureComplete(devInfo);
            break;
        case GAP_SEC_EVENT_UNAUTH_SUCCESS:
            GapUpdateSecurityStatusUnauthenticationSuccess(devInfo);
            break;
        case GAP_SEC_EVENT_AUTH_SUCCESS:
            GapUpdateSecurityStatusAuthenticationSuccess(devInfo);
            break;
        case GAP_SEC_EVENT_ENC_SUCCESS:
            if (devInfo->waitEncryptAlarm != NULL) {
                AlarmCancel(devInfo->waitEncryptAlarm);
                AlarmDelete(devInfo->waitEncryptAlarm);
                devInfo->waitEncryptAlarm = NULL;
            }
            GapUpdateSecurityStatusEncryptionSuccess(devInfo);
            break;
        case GAP_SEC_EVENT_ENC_FAILED:
            if (devInfo->waitEncryptAlarm != NULL) {
                AlarmCancel(devInfo->waitEncryptAlarm);
                AlarmDelete(devInfo->waitEncryptAlarm);
                devInfo->waitEncryptAlarm = NULL;
            }
            GapUpdateSecurityStatusFailed(devInfo, hciStatus);
            break;
        case GAP_SEC_EVENT_AUTH_FAILED:
            GapUpdateSecurityStatusFailed(devInfo, hciStatus);
            break;
        case GAP_SEC_EVENT_WAIT_ENC_TIMEOUT:
            GapUpdateSecurityStatusEncryptionTimeout(devInfo);
            break;
        case GAP_SEC_EVENT_ACL_DISCONNECT:
            GapUpdateSecurityStatusAclDisconnect(devInfo, hciStatus);
            break;
        default:
            break;
    }
}

static int GapAuthenticationRequested(uint16_t handle)
{
    HciAuthenticationRequestedParam hciCmdParam = {
        .connectionHandle = handle,
    };

    return HCI_AuthenticationRequested(&hciCmdParam);
}

static void GapAuthenticationWaitRetryTimeoutTask(void *ctx)
{
    RequestSecInfo *reqInfo = ListForEachData(
        GapGetProfileSecurityBlock()->requestlist, GapFindCmpListData, ((GapGeneralPointerInfo *)ctx)->pointer);
    if (reqInfo != NULL) {
        DeviceInfo *deviceInfo =
            ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, &reqInfo->addr);
        if (deviceInfo != NULL) {
            deviceInfo->authenticationStatus = GAP_AUTH_STATUS_RETRY;
            deviceInfo->actionReq = NULL;
            GapRequestSecurityProcess();
        } else {
            BTM_AclConnect(&reqInfo->addr);
        }
    }
}

static void GapAuthenticationWaitRetryTimeout(void *parameter)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    ctx->pointer = parameter;

    int ret = GapRunTaskUnBlockProcess(GapAuthenticationWaitRetryTimeoutTask, ctx, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

void GapAuthenticationClearInfo(RequestSecInfo *reqInfo)
{
    if (reqInfo == NULL) {
        return;
    }
    if (reqInfo->waitRetryalarm != NULL) {
        AlarmCancel(reqInfo->waitRetryalarm);
        AlarmDelete(reqInfo->waitRetryalarm);
        reqInfo->waitRetryalarm = NULL;
    }
}

void GapAuthenticationRetry(DeviceInfo *deviceInfo, RequestSecInfo *reqInfo, uint8_t hciStatus)
{
    if (reqInfo == NULL) {
        return;
    }

    if (reqInfo->waitRetryalarm == NULL) {
        reqInfo->waitRetryalarm = AlarmCreate("retry pair", false);
    }

    if (++reqInfo->retryCount <= GAP_PAIR_RETRY_COUNT) {
        AlarmCancel(reqInfo->waitRetryalarm);
        AlarmSet(reqInfo->waitRetryalarm, GAP_PAIR_RETRY_WAIT_TIME, GapAuthenticationWaitRetryTimeout, reqInfo);
    } else {
        if (deviceInfo != NULL) {
            GapUpdateSecurityRequest(deviceInfo, GAP_SEC_EVENT_AUTH_FAILED, hciStatus);
        } else {
            reqInfo->status = GAP_SEC_REQ_STATUS_FAILED;
            reqInfo->hciStatus = hciStatus;
            if (reqInfo->info.serviceId == GAP) {
                GapDoAuthenticationCallback(reqInfo);
            }
            if (!reqInfo->doCallback) {
                reqInfo->doCallback = true;
                GapDoSecurityCallback(reqInfo);
            }
        }
    }
}

NO_SANITIZE("cfi") void GapOnAuthenticationComplete(const HciAuthenticationCompleteEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:", __FUNCTION__);
    bool inDedicatedBonding = false;
    bool isGapRequest = false;
    bool callback = true;
    BtAddr addr = {0};

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *deviceInfo = ListForEachData(
        connectionInfoBlock->devicelist, GapFindConnectionDeviceByHandle, (void *)&eventParam->connectionHandle);
    if (deviceInfo != NULL) {
        isGapRequest = (deviceInfo->actionReq && deviceInfo->actionReq->info.serviceId == GAP);
        deviceInfo->authenticationStatus = GAP_AUTH_STATUS_IDLE;
        if (eventParam->status == HCI_SUCCESS) {
            if (GapDeviceLinkkeyIsAuthenticated(deviceInfo)) {
                deviceInfo->isAuthentication = true;
                GapUpdateSecurityRequest(deviceInfo, GAP_SEC_EVENT_AUTH_SUCCESS, eventParam->status);
            } else {
                GapUpdateSecurityRequest(deviceInfo, GAP_SEC_EVENT_UNAUTH_SUCCESS, eventParam->status);
            }
            GapReadNewLocalOOBData();
            GapAuthenticationClearInfo(deviceInfo->actionReq);
            deviceInfo->actionReq = NULL;
        } else if ((eventParam->status == HCI_PIN_OR_KEY_MISSING && GapIsKeyMissingRetry()) ||
                   BtmInteropIsMatchedAddr(INTEROP_AUTO_RETRY_PAIRING, &deviceInfo->addr)) {
            deviceInfo->authenticationStatus = GAP_AUTH_STATUS_WAIT_RETRY;
            GapAuthenticationRetry(deviceInfo, deviceInfo->actionReq, eventParam->status);
            callback = false;
        } else {
            GapUpdateSecurityRequest(deviceInfo, GAP_SEC_EVENT_AUTH_FAILED, eventParam->status);
            deviceInfo->actionReq = NULL;
        }
        inDedicatedBonding = deviceInfo->inDedicatedBonding;
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (deviceInfo->status == GAP_DEV_SEC_STATUS_ACTION) {
            deviceInfo->status = GAP_DEV_SEC_STATUS_WAIT_DISC;
        }
    }

    GapRequestSecurityProcess();

    if (g_authenticationCallback.callback.authenticationComplete) {
        if (inDedicatedBonding == true) {
            if (isGapRequest || callback) {
                g_authenticationCallback.callback.authenticationComplete(
                    &addr, eventParam->status, g_authenticationCallback.context);
            }
        } else if (inDedicatedBonding == false && callback) {
            g_authenticationCallback.callback.authenticationComplete(
                &addr, eventParam->status, g_authenticationCallback.context);
        }
    }
}

static int GapIOCapabilityRequestReply(
    const BtAddr *addr, uint8_t ioCapability, uint8_t authReq, uint8_t oobDataPresent)
{
    bool isDedicatedBonding = false;
    uint8_t authenticationRequirements = AUTHENTICATION_UNKNOWN_MITM;

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL) {
        devInfo->localIOCap = ioCapability;
        isDedicatedBonding = devInfo->inDedicatedBonding;
        authenticationRequirements = devInfo->remoteAuthReq;
    }

    if (authenticationRequirements == AUTHENTICATION_UNKNOWN_MITM) {
        if (!GapIsBondMode()) {
            authenticationRequirements = AUTHENTICATION_NO_MITM_NO_BONDING + authReq;
        } else if (isDedicatedBonding) {
            authenticationRequirements = AUTHENTICATION_NO_MITM_DEDICATED_BONDING + authReq;
        } else {
            authenticationRequirements = AUTHENTICATION_NO_MITM_GENERAL_BONDING + authReq;
        }
    }

    HciIOCapabilityRequestReplyParam hciCmdParam;
    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    hciCmdParam.ioCapability = ioCapability;
    hciCmdParam.authenticationRequirements = authenticationRequirements;
    hciCmdParam.oobDataPresent = oobDataPresent;

    return HCI_IOCapabilityRequestReply(&hciCmdParam);
}

static int GapIOCapabilityRequestNegativeReply(const BtAddr *addr, uint8_t reason)
{
    HciIoCapabilityRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    hciCmdParam.reason = reason;

    return HCI_IOCapabilityRequestNegativeReply(&hciCmdParam);
}

NO_SANITIZE("cfi") void GapOnIOCapabilityRequestEvent(const HciIoCapabilityRequestEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT "", __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr = BT_ADDR_NULL;
    bool doReject = false;
    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)&addr);
    if (devInfo != NULL) {
        if (devInfo->authenticationStatus == GAP_AUTH_STATUS_IDLE) {
            GapStartUseAclConnection(devInfo, GAP_USE_ACL_CONNECTION_TIME);
        }
        if ((GapIsBondMode() == false) && (devInfo->remoteAuthReq != AUTHENTICATION_UNKNOWN_MITM) &&
            (devInfo->remoteAuthReq != AUTHENTICATION_MITM_NO_BONDING) &&
            (devInfo->remoteAuthReq != AUTHENTICATION_NO_MITM_NO_BONDING)) {
            doReject = true;
        }
    }

    if (doReject) {
        GapIOCapabilityRequestNegativeReply(&addr, HCI_PAIRING_NOT_ALLOWED);
    } else {
        if (g_authenticationCallback.callback.IOCapabilityReq) {
            g_authenticationCallback.callback.IOCapabilityReq(&addr, g_authenticationCallback.context);
        } else {
            LOG_INFO("Not Register");
            GapIOCapabilityRequestNegativeReply(&addr, HCI_PAIRING_NOT_ALLOWED);
        }
    }
}

void GapIOCapabilityRequestReplyComplete(const HciIOCapabilityRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr = BT_ADDR_NULL;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed: %02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
        GapIOCapabilityRequestNegativeReply(&addr, HCI_PAIRING_NOT_ALLOWED);
    }
}

void GapIOCapabilityRequestNegativeReplyComplete(const HciIoCapabilityRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed: %02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

NO_SANITIZE("cfi") void GapOnIOCapabilityResponseEvent(const HciIoCapabilityResponseEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT "", __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr = BT_ADDR_NULL;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, (void *)&addr);
    if (devInfo != NULL) {
        devInfo->remoteAuthReq = eventParam->authenticationRequirements;
    }

    if (g_authenticationCallback.callback.IOCapabilityRsp) {
        g_authenticationCallback.callback.IOCapabilityRsp(
            &addr, eventParam->IOCapability, g_authenticationCallback.context);
    }
}

int GAP_RegisterAuthenticationCallback(const GapAuthenticationCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_authenticationCallback.callback,
            sizeof(g_authenticationCallback.callback),
            0x00,
            sizeof(g_authenticationCallback.callback));
    } else {
        g_authenticationCallback.callback = *callback;
    }
    g_authenticationCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterAuthenticationCallback(void)
{
    (void)memset_s(&g_authenticationCallback.callback,
        sizeof(g_authenticationCallback.callback),
        0x00,
        sizeof(g_authenticationCallback.callback));
    g_authenticationCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_AuthenticationReq(const BtAddr *addr)
{
    int ret = GAP_SUCCESS;
    ConnectionInfoBlock *connectionInfoBlock = NULL;
    ProfileSecurityBlock *profileSecurityBlock = NULL;
    DeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (!GapIsBondMode()) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        RequestSecInfo *secInfo = NULL;
        ProfileSecurityInfo tempInfo = {0};
        tempInfo.info.serviceId = GAP;
        tempInfo.info.direction = OUTGOING;
        tempInfo.securityMode = GAP_SEC_OUT_AUTHENTICATION;
        connectionInfoBlock = GapGetConnectionInfoBlock();
        profileSecurityBlock = GapGetProfileSecurityBlock();
        secInfo = GapAllocReqSecInfo(addr, NULL, NULL, &tempInfo);
        if (secInfo != NULL) {
            ListAddFirst(profileSecurityBlock->requestlist, secInfo);
            deviceInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
            if (deviceInfo != NULL) {
                GapRequestSecurityProcess();
            } else {
                ret = BTM_AclConnect(addr);
            }
        } else {
            ret = GAP_ERR_OUT_OF_RES;
        }
    }

    return ret;
}

int GAP_CancelAuthenticationReq(const BtAddr *addr)
{
    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL) {
        if (devInfo->status == GAP_DEV_SEC_STATUS_ACTION) {
            devInfo->status = GAP_DEV_SEC_STATUS_IDLE;
            BTM_AclRelease(devInfo->handle);
        }
    }

    return GAP_SUCCESS;
}

int GAP_IOCapabilityRsp(
    const BtAddr *addr, uint8_t accept, uint8_t ioCapability, uint8_t oobDataPresent, uint8_t authReq)
{
    int ret;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(GapGetConnectionInfoBlock()->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL && devInfo->actionReq != NULL) {
        if (!devInfo->actionReq->needAuthentication && devInfo->actionReq->needUnauthentication) {
            if (authReq == GAP_MITM_REQUIRED) {
                authReq = GAP_MITM_NOT_REQUIRED;
            }
        }
    }

    if (accept == GAP_ACCEPT) {
        ret = GapIOCapabilityRequestReply(addr, ioCapability, authReq, oobDataPresent);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapIOCapabilityRequestNegativeReply(addr, HCI_PAIRING_NOT_ALLOWED);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

static int GapUserConfirmationRequestReply(const BtAddr *addr)
{
    HciUserConfirmationRequestReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_UserConfirmationRequestReply(&hciCmdParam);
}

static int GapUserConfirmationRequestNegativeReply(const BtAddr *addr)
{
    HciUserConfirmationRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_UserConfirmationRequestNegativeReply(&hciCmdParam);
}

NO_SANITIZE("cfi") void GapOnUserConfirmationRequestEvent(const HciUserConfirmationRequestEventParam *eventParam)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT "", __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr = BT_ADDR_NULL;
    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
    if (g_authenticationCallback.callback.userConfirmReq) {
        g_authenticationCallback.callback.userConfirmReq(
            &addr, eventParam->numericValue, g_authenticationCallback.context);
    } else {
        GapUserConfirmationRequestNegativeReply(&addr);
    }
}

void GapUserConfirmationRequestReplyComplete(const HciUserConfirmationRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr = BT_ADDR_NULL;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapUserConfirmationRequestNegativeReply(&addr);
    }
}

void GapUserConfirmationRequestNegativeReplyComplete(const HciUserConfirmationRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

int GAP_UserConfirmRsp(const BtAddr *addr, uint8_t accept)
{
    int ret;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (accept == GAP_ACCEPT) {
        ret = GapUserConfirmationRequestReply(addr);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapUserConfirmationRequestNegativeReply(addr);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

static int GapUserPasskeyRequestReply(const BtAddr *addr, int numericValue)
{
    HciUserPasskeyRequestReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    hciCmdParam.numericValue = numericValue;

    return HCI_UserPasskeyRequestReply(&hciCmdParam);
}

static int GapUserPasskeyRequestNegativeReply(const BtAddr *addr)
{
    HciUserPasskeyRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_UserPasskeyRequestNegativeReply(&hciCmdParam);
}

void GapOnUserPasskeyNotificationEvent(const HciUserPasskeyNotificationEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr = BT_ADDR_NULL;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
    if (g_authenticationCallback.callback.userPasskeyNotification) {
        g_authenticationCallback.callback.userPasskeyNotification(
            &addr, eventParam->passkey, g_authenticationCallback.context);
    } else {
        LOG_WARN("%{public}s:not register callback", __FUNCTION__);
    }
}

void GapOnUserPasskeyRequestEvent(const HciUserPasskeyRequestEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr = BT_ADDR_NULL;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    if (g_authenticationCallback.callback.userPasskeyReq) {
        g_authenticationCallback.callback.userPasskeyReq(&addr, g_authenticationCallback.context);
    } else {
        GapUserPasskeyRequestNegativeReply(&addr);
    }
}

void GapUserPasskeyRequestReplyComplete(const HciUserPasskeyRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr = BT_ADDR_NULL;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapUserPasskeyRequestNegativeReply(&addr);
    }
}

void GapUserPasskeyRequestNegativeReplyComplete(const HciUserPasskeyRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

int GAP_UserPasskeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number)
{
    int ret;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (accept == GAP_ACCEPT) {
        ret = GapUserPasskeyRequestReply(addr, number);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapUserPasskeyRequestNegativeReply(addr);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

static int GapRemoteOOBDataRequestReply(const BtAddr *addr, const uint8_t *c, const uint8_t *r)
{
    HciRemoteOobDataRequestReplyParam hciCmdParam;
    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    (void)memcpy_s(hciCmdParam.C, GAP_OOB_DATA_CONFIRM_SIZE, c, GAP_OOB_DATA_CONFIRM_SIZE);
    (void)memcpy_s(hciCmdParam.r, GAP_OOB_DATA_RANDOM_SIZE, r, GAP_OOB_DATA_RANDOM_SIZE);

    return HCI_RemoteOOBDataRequestReply(&hciCmdParam);
}

static int GapRemoteOOBDataRequestNegativeReply(const BtAddr *addr)
{
    HciRemoteOobDataRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_RemoteOOBDataRequestNegativeReply(&hciCmdParam);
}

void GapOnRemoteOOBDataRequestEvent(const HciRemoteOobDataRequestEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    if (g_authenticationCallback.callback.remoteOobReq) {
        g_authenticationCallback.callback.remoteOobReq(&addr, g_authenticationCallback.context);
    } else {
        GapRemoteOOBDataRequestNegativeReply(&addr);
    }
}

void GapRemoteOOBDataRequestReplyComplete(const HciRemoteOobDataRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapRemoteOOBDataRequestNegativeReply(&addr);
    }
}

void GapRemoteOOBExtendedDataRequestReplyComplete(const HciRemoteOobExtendedDataRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapRemoteOOBDataRequestNegativeReply(&addr);
    }
}

void GapRemoteOOBDataRequestNegativeReplyComplete(const HciRemoteOobDataRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

int GAP_RemoteOobRsp(const BtAddr *addr, uint8_t accept, const GapOOBData *data)
{
    int ret;

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (addr == NULL || data == NULL) {
        return GAP_ERR_INVAL_PARAM;
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (accept == GAP_ACCEPT) {
        ret = GapRemoteOOBDataRequestReply(addr, (uint8_t *)data->C, (uint8_t *)data->R);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapRemoteOOBDataRequestNegativeReply(addr);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

static int GapPINCodeRequestReply(const BtAddr *addr, uint8_t pinCode[GAP_PINCODE_SIZE], uint8_t pinCodeLength)
{
    HciPinCodeRequestReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    (void)memcpy_s(hciCmdParam.pinCode, GAP_PINCODE_SIZE, pinCode, GAP_PINCODE_SIZE);
    hciCmdParam.pinCodeLength = pinCodeLength;

    return HCI_PINCodeRequestReply(&hciCmdParam);
}

static int GapPINCodeRequestNegativeReply(const BtAddr *addr)
{
    HciPinCodeRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_PINCodeRequestNegativeReply(&hciCmdParam);
}

void GapOnPINCodeRequestEvent(const HciPinCodeRequestEventParam *eventParam)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr;
    bool isBondableMode = false;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)&addr);
    if (devInfo != NULL) {
        if (devInfo->authenticationStatus == GAP_AUTH_STATUS_IDLE) {
            GapStartUseAclConnection(devInfo, GAP_USE_ACL_CONNECTION_TIME);
        }
    }

    isBondableMode = GapIsBondMode();
    if (isBondableMode == false) {
        GapPINCodeRequestNegativeReply(&addr);
    } else if (g_authenticationCallback.callback.pinCodeReq) {
        g_authenticationCallback.callback.pinCodeReq(&addr, g_authenticationCallback.context);
    } else {
        GapPINCodeRequestNegativeReply(&addr);
    }
}

void GapPINCodeRequestReplyComplete(const HciPinCodeRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapPINCodeRequestNegativeReply(&addr);
    }
}

void GapPINCodeRequestNegativeReplyComplete(const HciPinCodeRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

int GAP_PinCodeRsp(const BtAddr *addr, uint8_t accept, const uint8_t *pinCode, uint8_t pinCodeLength)
{
    int ret;

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (addr == NULL) {
        return GAP_ERR_INVAL_PARAM;
    } else if (accept == GAP_ACCEPT && (pinCode == NULL || pinCodeLength > GAP_PINCODE_SIZE)) {
        return GAP_ERR_INVAL_PARAM;
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (accept == GAP_ACCEPT) {
        uint8_t pin[GAP_PINCODE_SIZE] = {0};
        (void)memcpy_s(pin, GAP_PINCODE_SIZE, pinCode, pinCodeLength);
        ret = GapPINCodeRequestReply(addr, pin, pinCodeLength);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapPINCodeRequestNegativeReply(addr);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

static int GapLinkKeyRequestReply(const BtAddr *addr, const uint8_t *linkKey)
{
    HciLinkKeyRequestReplyParam hciCmdParam;
    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);
    if (memcpy_s(hciCmdParam.linkKey, GAP_LINKKEY_SIZE, linkKey, GAP_LINKKEY_SIZE) != EOK) {
        return GAP_STATUS_FAILED;
    }

    return HCI_LinkKeyRequestReply(&hciCmdParam);
}

static int GapLinkKeyRequestNegativeReply(const BtAddr *addr)
{
    HciLinkKeyRequestNegativeReplyParam hciCmdParam;

    (void)memcpy_s(hciCmdParam.bdAddr.raw, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    return HCI_LinkKeyRequestNegativeReply(&hciCmdParam);
}

int GAP_PairIsFromLocal(const BtAddr *addr, bool *isLocal)
{
    int ret = GAP_SUCCESS;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
    if (devInfo != NULL) {
        *isLocal = !!devInfo->authenticationStatus;
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

NO_SANITIZE("cfi") void GapOnLinkKeyNotificationEvent(const HciLinkKeyNotificationEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)&addr);
    if (devInfo != NULL) {
        devInfo->linkkeyType = eventParam->keyType;
    }

    if (g_authenticationCallback.callback.linkKeyNotification) {
        g_authenticationCallback.callback.linkKeyNotification(
            &addr, (uint8_t *)eventParam->linkKey, eventParam->keyType, g_authenticationCallback.context);
    }
}

NO_SANITIZE("cfi") void GapOnLinkKeyRequestEvent(const HciLinkKeyRequestEventParam *eventParam)
{
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr;
    bool callback = true;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)&addr);
    if (devInfo != NULL && devInfo->authenticationStatus == GAP_AUTH_STATUS_RETRY) {
        callback = false;
        devInfo->authenticationStatus = GAP_AUTH_STATUS_ACTION;
    } else if (devInfo != NULL && devInfo->linkkeyType != GAP_LINK_KEY_TYPE_UNKNOWN &&
               !GapDeviceLinkkeyIsAuthenticated(devInfo) && devInfo->actionReq != NULL &&
               devInfo->actionReq->needAuthentication && devInfo->actionReq->needMITM) {
        callback = false;
    }

    if (g_authenticationCallback.callback.linkKeyReq) {
        if (callback) {
            g_authenticationCallback.callback.linkKeyReq(&addr, g_authenticationCallback.context);
        } else {
            GapLinkKeyRequestNegativeReply(&addr);
        }
    } else {
        GapLinkKeyRequestNegativeReply(&addr);
    }
}

void GapLinkKeyRequestReplyComplete(const HciLinkKeyRequestReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        BtAddr addr;
        GapChangeHCIAddr(&addr, &param->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr), param->status);
        GapLinkKeyRequestNegativeReply(&addr);
    }
}

void GapLinkKeyRequestNegativeReplyComplete(const HciLinkKeyRequestNegativeReplyReturnParam *param)
{
    if (param->status != HCI_SUCCESS) {
        LOG_WARN("%{public}s:" BT_ADDR_FMT "Failed:%02x", __FUNCTION__, BT_ADDR_FMT_OUTPUT(param->bdAddr.raw), param->status);
    }
}

int GAP_LinkKeyRsp(const BtAddr *addr, uint8_t accept, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType)
{
    int ret;

    if (GapIsBredrEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (addr == NULL || linkKey == NULL) {
        return GAP_ERR_INVAL_PARAM;
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT " %hhu, keyType[%hhu]", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);

    if (accept == GAP_ACCEPT) {
        ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
        DeviceInfo *devInfo = NULL;
        devInfo = ListForEachData(connectionInfoBlock->devicelist, GapFindConnectionDeviceByAddr, (void *)addr);
        if (devInfo != NULL) {
            devInfo->linkkeyType = keyType;
        }
        ret = GapLinkKeyRequestReply(addr, linkKey);
    } else if (accept == GAP_NOT_ACCEPT) {
        ret = GapLinkKeyRequestNegativeReply(addr);
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

NO_SANITIZE("cfi") void GapOnSimplePairingComplete(const HciSimplePairingCompleteEventParam *eventParam)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(eventParam->bdAddr.raw));
    BtAddr addr;

    GapChangeHCIAddr(&addr, &eventParam->bdAddr, BT_PUBLIC_DEVICE_ADDRESS);

    if (g_authenticationCallback.callback.simplePairComplete) {
        g_authenticationCallback.callback.simplePairComplete(
            &addr, eventParam->status, g_authenticationCallback.context);
    }
}

NO_SANITIZE("cfi") void GapOnEncryptionChangeEvent(const HciEncryptionChangeEventParam *eventParam)
{
    BtAddr addr = {0};
    GapEncryptionChangeCallback callback = NULL;
    void *context = NULL;

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(
        connectionInfoBlock->devicelist, GapFindConnectionDeviceByHandle, (void *)&eventParam->connectionHandle);
    if (devInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &devInfo->addr, sizeof(BtAddr));
        if (eventParam->status == HCI_SUCCESS) {
            if (eventParam->encryptionEnabled) {
                devInfo->isEncryption = true;
            } else {
                devInfo->isEncryption = false;
            }
            GapUpdateSecurityRequest(devInfo, GAP_SEC_EVENT_ENC_SUCCESS, eventParam->status);
        } else {
            GapUpdateSecurityRequest(devInfo, GAP_SEC_EVENT_ENC_FAILED, eventParam->status);
        }
        if (devInfo->status == GAP_DEV_SEC_STATUS_ACTION && devInfo->encryptionStatus == GAP_ENC_STATUS_ACTION) {
            devInfo->actionReq = NULL;
            devInfo->status = GAP_DEV_SEC_STATUS_WAIT_DISC;
            devInfo->encryptionStatus = GAP_ENC_STATUS_IDLE;
        }
    }

    GapRequestSecurityProcess();

    EncryptionBlock *encryptionBlock = GapGetEncryptionBlock();
    if (encryptionBlock->status == GAP_SET_ENCRYPTION_STATUS_SETTING) {
        callback = encryptionBlock->callback;
        context = encryptionBlock->context;
    }

    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr));

    if (devInfo != NULL) {
        if (g_authenticationCallback.callback.encryptionChangeCallback) {
            g_authenticationCallback.callback.encryptionChangeCallback(&addr,
                eventParam->encryptionEnabled ? GAP_ENCRYPTION_ON : GAP_ENCRYPTION_OFF,
                g_authenticationCallback.context);
        }
        if (callback) {
            callback(&addr, eventParam->encryptionEnabled ? GAP_ENCRYPTION_ON : GAP_ENCRYPTION_OFF, context);
        }
    }

    GapLeEncryptionComplete(eventParam->connectionHandle, eventParam->status);
}

void GapOnEncryptionKeyRefreshComplete(const HciEncryptionKeyRefreshCompleteEventParam *eventParam)
{
    BtAddr addr = {0};

    ConnectionInfoBlock *connectionInfoBlock = GapGetConnectionInfoBlock();
    DeviceInfo *devInfo = NULL;
    devInfo = ListForEachData(
        connectionInfoBlock->devicelist, GapFindConnectionDeviceByHandle, (void *)&eventParam->connectionHandle);
    if (devInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &devInfo->addr, sizeof(BtAddr));
        if (eventParam->status == HCI_SUCCESS) {
            GapUpdateSecurityRequest(devInfo, GAP_SEC_EVENT_ENC_SUCCESS, eventParam->status);
        } else {
            GapUpdateSecurityRequest(devInfo, GAP_SEC_EVENT_ENC_FAILED, eventParam->status);
        }
        if (devInfo->status == GAP_DEV_SEC_STATUS_ACTION && devInfo->encryptionStatus == GAP_ENC_STATUS_ACTION) {
            devInfo->actionReq = NULL;
            devInfo->status = GAP_DEV_SEC_STATUS_WAIT_DISC;
            devInfo->encryptionStatus = GAP_ENC_STATUS_IDLE;
        }
    }
    GapRequestSecurityProcess();
    LOG_DEBUG("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr.addr));
}

static int GapSetConnectionEncryption(uint16_t handle, uint8_t encryptionEnable)
{
    HciSetConnectionEncryptionParam hciCmdParam = {
        .connectionHandle = handle,
        .encryptionEnable = encryptionEnable,
    };

    return HCI_SetConnectionEncryption(&hciCmdParam);
}
