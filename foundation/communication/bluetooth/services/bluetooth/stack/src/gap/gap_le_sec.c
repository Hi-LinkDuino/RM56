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

#include "gap_le.h"
#include "gap_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"
#include "thread.h"

#include "btm.h"
#include "btm/btm_thread.h"

typedef enum {
    SIGNATURE_GENERATION,
    SIGNATURE_CONFIRMATION,
} SignatureType;

typedef struct {
    uint16_t handle;
    uint8_t type;
    uint32_t counter;
    uint8_t signature[GAP_SIGNATURE_SIZE];
    GAP_SignatureResult result;
    bool processing;
    void *callback;
    void *context;
} SignatureRequestInfo;

typedef struct {
    GapLePairCallback callback;
    void *context;
} LePairCallback;

typedef struct {
    GapLeSecurityCallback callback;
    void *context;
} LeSecurityCallback;

static LePairCallback g_lePairCallback;
static LeSecurityCallback g_leSecurityCallback;

static int GapLePair(const BtAddr *addr);

static bool GapLeSecurityNeedAuthentication(const LeLocalInfo *localInfo)
{
    return localInfo->mode1Level >= LE_MODE_1_LEVEL_3 || localInfo->mode2Level >= LE_MODE_2_LEVEL_2;
}

void GapDoLeSecurityCallback(const void *data)
{
    uint16_t handle = 0;
    GapLeRequestSecurityResult callback = NULL;
    void *context = NULL;
    uint8_t result;
    uint8_t encryptionStatus = GAP_LE_NO_ENCRYPTION;

    if (data == NULL) {
        return;
    } else {
        handle = *(const uint16_t *)data;
    }

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL && deviceInfo->securityReq != NULL) {
        callback = deviceInfo->securityReq->callback;
        context = deviceInfo->securityReq->context;
        result = deviceInfo->securityReq->result;
        MEM_MALLOC.free(deviceInfo->securityReq);
        deviceInfo->securityReq = NULL;
    }

    if (deviceInfo != NULL) {
        encryptionStatus = deviceInfo->encryptionStatus;
        if (callback != NULL) {
            callback(&deviceInfo->addr, result, encryptionStatus, context);
        }
    }
}

static void GapLePasskeyRequestProcess(
    const BtAddr *addr, uint16_t handle, uint8_t pairMethod, const void *displayValue)
{
    int ret;

    if (pairMethod == SMP_PAIR_METHOD_PASSKEY_DISPLAY) {
        if (g_lePairCallback.callback.lePairPassKeyNotification) {
            g_lePairCallback.callback.lePairPassKeyNotification(
                addr, *(uint32_t *)displayValue, g_lePairCallback.context);
            ret = SMP_AuthenticationRequestReply(handle, true, 0x00, pairMethod, displayValue);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
        } else {
            ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION, pairMethod, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
        }
    } else if (pairMethod == SMP_PAIR_METHOD_PASSKEY_ENTRY) {
        if (g_lePairCallback.callback.lePairPassKeyReq) {
            g_lePairCallback.callback.lePairPassKeyReq(addr, g_lePairCallback.context);
        } else {
            ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION, pairMethod, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
        }
    }
}

static void GapLeUserConfirmProcess(const BtAddr *addr, uint16_t handle, uint8_t pairMethod, const void *displayValue)
{
    if (g_lePairCallback.callback.lePairScUserConfirmReq) {
        g_lePairCallback.callback.lePairScUserConfirmReq(addr, *(uint32_t *)displayValue, g_lePairCallback.context);
    } else {
        int ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION, pairMethod, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
        }
    }
}

static void GapLeLegacyOOBProcess(const BtAddr *addr, uint16_t handle, uint8_t pairMethod, const void *displayValue)
{
    if (g_lePairCallback.callback.lePairOobReq) {
        g_lePairCallback.callback.lePairOobReq(addr, g_lePairCallback.context);
    } else {
        int ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION, pairMethod, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
        }
    }
}

static void GapLeSecureConnectionOOBProcess(
    const BtAddr *addr, uint16_t handle, uint8_t pairMethod, const void *displayValue)
{
    int ret;
    bool accept = true;

    if (pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV ||
        pairMethod == SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV) {
        if (g_lePairCallback.callback.lePairScOobNotification) {
            GapOOBData oobData;
            const uint8_t *data = displayValue;
            if (memcpy_s(oobData.R,
                GAP_OOB_DATA_RANDOM_SIZE, data + BT_ADDRESS_SIZE, GAP_OOB_DATA_RANDOM_SIZE) != EOK) {
                    LOG_WARN("%{public}s: memcpy_s oobData.R fail.", __FUNCTION__);
                    return;
                }
            if (memcpy_s(oobData.C,
                GAP_OOB_DATA_CONFIRM_SIZE,
                data + BT_ADDRESS_SIZE + GAP_OOB_DATA_RANDOM_SIZE,
                GAP_OOB_DATA_CONFIRM_SIZE) != EOK) {
                    LOG_WARN("%{public}s: memcpy_s fail.", __FUNCTION__);
                    return;
                }
            g_lePairCallback.callback.lePairScOobNotification(addr, &oobData, g_lePairCallback.context);
        } else {
            accept = false;
        }
    }

    if (pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND ||
        pairMethod == SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV) {
        if (g_lePairCallback.callback.lePairScOobReq) {
            g_lePairCallback.callback.lePairScOobReq(addr, g_lePairCallback.context);
        } else {
            accept = false;
        }
    }

    if (accept) {
        if (pairMethod == SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV) {
            ret = SMP_AuthenticationRequestReply(handle, accept, 0x00, pairMethod, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
        }
    } else {
        ret = SMP_AuthenticationRequestReply(handle, accept, 0x00, pairMethod, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
        }
    }
}

static void GapLeAuthenticationRequestProcess(
    BtAddr addr, uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue)
{
    int ret;

    switch (pairMethod) {
        case SMP_PAIR_METHOD_PASSKEY_DISPLAY:
        case SMP_PAIR_METHOD_PASSKEY_ENTRY:
            GapLePasskeyRequestProcess(&addr, handle, pairMethod, displayValue);
            break;
        case SMP_PAIR_METHOD_NUMERIC_COMPARISON:
            GapLeUserConfirmProcess(&addr, handle, pairMethod, displayValue);
            break;
        case SMP_PAIR_METHOD_OOB_LEGACY:
            GapLeLegacyOOBProcess(&addr, handle, pairMethod, displayValue);
            break;
        case SMP_PAIR_METHOD_JUST_WORK:
            ret = SMP_AuthenticationRequestReply(handle, true, 0x00, pairMethod, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
            break;
        case SMP_PAIR_METHOD_OOB_SC_LOCAL_SEND_PEER_RECV:
        case SMP_PAIR_METHOD_OOB_SC_LOCAL_RECV_PEER_SEND:
        case SMP_PAIR_METHOD_OOB_SC_BOTH_SIDE_SEND_RECV:
            GapLeSecureConnectionOOBProcess(&addr, handle, pairMethod, displayValue);
            break;
        default:
            LOG_WARN("%{public}s:Invalid pair method:%hhu.", __FUNCTION__, pairMethod);
            break;
    }
}

void GapLeAuthenticationRequest(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue)
{
    BtAddr addr = {0};
    int ret;

    LeLocalInfo *localInfo = GapGetLeLocalInfo();
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        deviceInfo->pairMethod = pairMethod;
    }
    if (deviceInfo != NULL) {
        if (GapLeSecurityNeedAuthentication(localInfo) && pairMethod == SMP_PAIR_METHOD_JUST_WORK) {
            ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_AUTH_REQ, pairMethod, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
            }
            return;
        }

        if (g_lePairCallback.callback.lePairMethodNotify) {
            g_lePairCallback.callback.lePairMethodNotify(&addr, pairMethod, g_lePairCallback.context);
        }
        GapLeAuthenticationRequestProcess(addr, handle, pairMethod, displayValue);
    } else {
        ret = SMP_AuthenticationRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION, pairMethod, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:pairMethod:%{public}d Call SMP failed:%{public}d.", __FUNCTION__, pairMethod, ret);
        }
    }
}

static void GapLePairKeyConvert(const SMP_PairResult *result, LePairedKeys *keys)
{
    if (result->peerKeyDist | SMP_KEY_DIST_BIT_ENC_KEY) {
        (void)memcpy_s(keys->remoteEncKey->ltk, GAP_LTK_SIZE, result->peerLTK, GAP_LTK_SIZE);
        (void)memcpy_s(&keys->remoteEncKey->rand,
            sizeof(keys->remoteEncKey->rand),
            result->peerRandom,
            sizeof(result->peerRandom));
        keys->remoteEncKey->ediv = result->peerEdiv;
        keys->remoteEncKey->keySize = result->encKeySize;
    } else {
        keys->remoteEncKey = NULL;
    }

    if (result->peerKeyDist | SMP_KEY_DIST_BIT_ID_KEY) {
        (void)memcpy_s(&keys->remoteIdKey->identityAddr, sizeof(BtAddr), &result->peerIdentAddr, sizeof(BtAddr));
        (void)memcpy_s(keys->remoteIdKey->irk, GAP_IRK_SIZE, result->peerIRK, GAP_IRK_SIZE);
    } else {
        keys->remoteIdKey = NULL;
    }

    if (result->peerKeyDist | SMP_KEY_DIST_BIT_SIGN_KEY) {
        (void)memcpy_s(keys->remoteSignKey->csrk, GAP_CSRK_SIZE, result->peerCSRK, GAP_CSRK_SIZE);
        keys->remoteSignKey->counter = 0;
    } else {
        keys->remoteSignKey = NULL;
    }

    if (result->localKeyDist | SMP_KEY_DIST_BIT_ENC_KEY) {
        (void)memcpy_s(keys->localEncKey->ltk, GAP_LTK_SIZE, result->localLTK, GAP_LTK_SIZE);
        (void)memcpy_s(&keys->localEncKey->rand,
            sizeof(keys->localEncKey->rand),
            result->localRandom,
            sizeof(result->localRandom));
        keys->localEncKey->ediv = result->localEdiv;
        keys->localEncKey->keySize = result->encKeySize;
    } else {
        keys->localEncKey = NULL;
    }

    if (result->localKeyDist | SMP_KEY_DIST_BIT_SIGN_KEY) {
        (void)memcpy_s(keys->localSignKey->csrk, GAP_CSRK_SIZE, result->localCSRK, GAP_CSRK_SIZE);
        keys->localSignKey->counter = 0;
    } else {
        keys->localSignKey = NULL;
    }
}

static void GapCallbackKeyNotify(const BtAddr *addr, const SMP_PairResult *result)
{
    LeEncKey remoteEncKey;
    LeIdKey remoteIdKey;
    LeSignKey remoteSignKey;
    LeEncKey localEncKey;
    LeSignKey localSignKey;

    LePairedKeys keys;
    keys.remoteEncKey = &remoteEncKey;
    keys.remoteIdKey = &remoteIdKey;
    keys.remoteSignKey = &remoteSignKey;
    keys.localEncKey = &localEncKey;
    keys.localSignKey = &localSignKey;

    GapLePairKeyConvert(result, &keys);

    if (g_lePairCallback.callback.lePairKeyNotify) {
        g_lePairCallback.callback.lePairKeyNotify(addr, keys, g_lePairCallback.context);
    }
}

static void GapSetLeSigningInfo(uint16_t handle, const SMP_PairResult *result)
{
    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        if (result->localKeyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
            deviceInfo->localSigningExists = true;
            (void)memcpy_s(deviceInfo->localSigningInfo.csrk, GAP_CSRK_SIZE, result->localCSRK, GAP_CSRK_SIZE);
            deviceInfo->localSigningInfo.counter = 0;
        } else {
            deviceInfo->localSigningExists = false;
        }
        if (result->peerKeyDist & SMP_KEY_DIST_BIT_SIGN_KEY) {
            deviceInfo->remoteSigningExists = true;
            (void)memcpy_s(deviceInfo->remoteSigningInfo.csrk, GAP_CSRK_SIZE, result->peerCSRK, GAP_CSRK_SIZE);
            deviceInfo->remoteSigningInfo.counter = 0;
        } else {
            deviceInfo->remoteSigningExists = false;
        }
    }
}

static void GapSetEncryptionStatusForPairEnd(LeDeviceInfo *deviceInfo, uint8_t status, const SMP_PairResult *result)
{
    if (status == SMP_PAIR_STATUS_SUCCESS) {
        if (deviceInfo->encryptionStatus == GAP_LE_UNAUTHENTICATED_ENCRYPTION && result->authFlag) {
            deviceInfo->encryptionStatus = GAP_LE_AUTHENTICATED_ENCRYPTION;
        }
    } else {
        deviceInfo->encryptionStatus = GAP_LE_NO_ENCRYPTION;
    }
}

void GapDoPairResultCallback(const BtAddr *addr, uint8_t status)
{
    if (g_lePairCallback.callback.lePairComplete) {
        g_lePairCallback.callback.lePairComplete(addr, status, 0, g_lePairCallback.context);
    }
}

void GapLePairResult(uint16_t handle, uint8_t status, const SMP_PairResult *result)
{
    BtAddr addr = {0};

    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (deviceInfo->securityStatus == GAP_LE_SECURITY_STATUS_PAIR) {
            deviceInfo->securityStatus = GAP_LE_SECURITY_STATUS_IDLE;
        }
        GapSetEncryptionStatusForPairEnd(deviceInfo, status, result);
        if (deviceInfo->securityReq != NULL) {
            deviceInfo->securityReq->result = status;
            GapDoLeSecurityCallback(&deviceInfo->handle);
        }
    }
    bool isPairing = GapGetLeBondBlock()->isPairing;
    if (deviceInfo == NULL && isPairing == true) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &GapGetLeBondBlock()->addr, sizeof(BtAddr));
    }

    GapClearPairingStatus(&addr);

    if (deviceInfo != NULL || isPairing) {
        if (status == SMP_PAIR_STATUS_SUCCESS && result->bondedFlag == SMP_BONDED_FLAG_YES) {
            GapCallbackKeyNotify(&addr, result);
            GapSetLeSigningInfo(handle, result);
        }
        if (g_lePairCallback.callback.lePairComplete) {
            g_lePairCallback.callback.lePairComplete(&addr, status, result->authFlag, g_lePairCallback.context);
        }
    }
}

void GapLeRemotePairRequest(uint16_t handle, const SMP_PairParam *param)
{
    BtAddr addr = {0};
    int ret;

    if (GapGetLeLocalInfo()->minEncKeySize > param->maxEncKeySize) {
        SMP_RemotePairRequestReply(handle, SMP_PAIR_FAILED_ENC_KEY_SIZE, NULL, NULL, NULL);
        return;
    }

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
    }

    if (deviceInfo != NULL) {
        if (g_lePairCallback.callback.lePairFeatureReq) {
            g_lePairCallback.callback.lePairFeatureReq(
                &deviceInfo->addr, deviceInfo->isLocalSecurityRequest, g_lePairCallback.context);
        } else {
            ret = SMP_RemotePairRequestReply(handle, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL, NULL, NULL);
            if (ret != BT_NO_ERROR) {
                LOG_WARN("%{public}s:SMP_RemotePairRequestReply failed:%{public}d.", __FUNCTION__, ret);
            }
        }
    } else {
        ret = SMP_RemotePairRequestReply(handle, SMP_PAIR_FAILED_UNSPECIFIED_REASION, NULL, NULL, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:SMP_RemotePairRequestReply failed:%{public}d.", __FUNCTION__, ret);
        }
    }
}

void GapLeRemotePairResponse(uint16_t handle, const SMP_PairParam *param)
{
    if (GapGetLeLocalInfo()->minEncKeySize > param->maxEncKeySize) {
        SMP_RemotePairResponseReply(handle, false, SMP_PAIR_FAILED_ENC_KEY_SIZE);
        return;
    }
    LeLocalInfo *localInfo = GapGetLeLocalInfo();
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        if (GapLeSecurityNeedAuthentication(localInfo) && !(param->authReq & SMP_AUTH_REQ_BIT_MITM)) {
            SMP_RemotePairResponseReply(handle, false, SMP_PAIR_FAILED_AUTH_REQ);
        } else {
            SMP_RemotePairResponseReply(handle, true, 0x00);
        }
    } else {
        SMP_RemotePairResponseReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION);
    }
}

void GapLeRemoteSecurityRequest(uint16_t handle, uint8_t authReq)
{
    BtAddr addr = {0};

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        deviceInfo->isRemoteSecurityRequest = true;
    }

    if (g_leSecurityCallback.callback.leRemoteEncryptionKeyReqEvent) {
        g_leSecurityCallback.callback.leRemoteEncryptionKeyReqEvent(&addr, g_leSecurityCallback.context);
    } else {
        int ret = SMP_RemoteSecurityRequestReply(handle, false, SMP_PAIR_FAILED_UNSPECIFIED_REASION);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:SMP_RemoteSecurityRequestReply failed:%{public}d.", __FUNCTION__, ret);
        }
    }
}

void GapLeLongTermKeyRequest(uint16_t handle, const uint8_t *random, uint16_t ediv)
{
    BtAddr addr = {0};

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
    }

    if (g_leSecurityCallback.callback.leLocalEncryptionKeyReqEvent) {
        g_leSecurityCallback.callback.leLocalEncryptionKeyReqEvent(
            &addr, *(uint64_t *)random, ediv, g_leSecurityCallback.context);
    } else {
        int ret = SMP_LongTermKeyRequestReply(handle, false, NULL);
        if (ret != BT_NO_ERROR) {
            LOG_WARN("%{public}s:SMP_LongTermKeyRequestReply failed:%{public}d.", __FUNCTION__, ret);
        }
    }
}

void GapLeEncryptionComplete(uint16_t handle, uint8_t status)
{
    BtAddr addr = {0};

    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByHandle, &handle);
    if (deviceInfo != NULL) {
        (void)memcpy_s(&addr, sizeof(BtAddr), &deviceInfo->addr, sizeof(BtAddr));
        if (status != HCI_SUCCESS) {
            deviceInfo->encryptionStatus = GAP_LE_NO_ENCRYPTION;
            if (deviceInfo->securityReq != NULL) {
                deviceInfo->securityReq->result = GAP_STATUS_FAILED;
                GapDoLeSecurityCallback(&handle);
            }
        } else {
            if (deviceInfo->keyType == LE_KEY_TYPE_AUTHENTICATION) {
                deviceInfo->encryptionStatus = GAP_LE_AUTHENTICATED_ENCRYPTION;
            } else {
                deviceInfo->encryptionStatus = GAP_LE_UNAUTHENTICATED_ENCRYPTION;
            }
        }
    }

    if (deviceInfo != NULL && g_leSecurityCallback.callback.encryptionComplete) {
        g_leSecurityCallback.callback.encryptionComplete(status, &addr, g_leSecurityCallback.context);
    }
}

int GAP_RegisterLeSecurityCallback(const GapLeSecurityCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(&g_leSecurityCallback.callback,
            sizeof(g_leSecurityCallback.callback),
            0x00,
            sizeof(g_leSecurityCallback.callback));
    } else {
        g_leSecurityCallback.callback = *callback;
    }
    g_leSecurityCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterLeSecurityCallback(void)
{
    (void)memset_s(&g_leSecurityCallback.callback,
        sizeof(g_leSecurityCallback.callback),
        0x00,
        sizeof(g_leSecurityCallback.callback));
    g_leSecurityCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_LeRemoteEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey encKey, uint8_t keyType)
{
    LOG_INFO(
        "%{public}s:" BT_ADDR_FMT "accept:%hhu keyType:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    int ret = GAP_ERR_INVAL_PARAM;
    LeLocalInfo *localInfo = GapGetLeLocalInfo();
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            deviceInfo->keyType = keyType;
            deviceInfo->keySize = encKey.keySize;
            if (deviceInfo->isRemoteSecurityRequest) {
                ret = SMP_RemoteSecurityRequestReply(deviceInfo->handle, true, 0x00);
                if (ret != BT_NO_ERROR) {
                    LOG_WARN("%{public}s:SMP_RemoteSecurityRequestReply failed:%{public}d.", __FUNCTION__, ret);
                }
            }

            if (keyType == LE_KEY_TYPE_AUTHENTICATION) {
                ret = SMP_StartEncryption(deviceInfo->handle, (uint8_t *)&encKey.rand, encKey.ediv, encKey.ltk);
            } else if (GapLeSecurityNeedAuthentication(localInfo)) {
                ret = GapLePair(addr);
                GapGetLeBondBlock()->isPairing = true;
                (void)memcpy_s(&GapGetLeBondBlock()->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
            } else {
                ret = SMP_StartEncryption(deviceInfo->handle, (uint8_t *)&encKey.rand, encKey.ediv, encKey.ltk);
            }
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = GapLePair(addr);
            GapGetLeBondBlock()->isPairing = true;
            (void)memcpy_s(&GapGetLeBondBlock()->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    }

    return ret;
}

int GAP_LeLocalEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey encKey, uint8_t keyType)
{
    LOG_INFO(
        "%{public}s:" BT_ADDR_FMT "accept:%hhu keyType:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }
    int ret = GAP_ERR_INVAL_PARAM;
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        deviceInfo->keyType = keyType;
        deviceInfo->keySize = encKey.keySize;
        if (accept == GAP_ACCEPT) {
            ret = SMP_LongTermKeyRequestReply(deviceInfo->handle, true, encKey.ltk);
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = SMP_LongTermKeyRequestReply(deviceInfo->handle, false, NULL);
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    }

    return ret;
}

int GAP_RequestSigningAlgorithmInfoRsp(const BtAddr *addr, uint8_t accept, GapSigningAlgorithmInfo info)
{
    int ret = GAP_SUCCESS;
    LOG_INFO("%{public}s:" BT_ADDR_FMT "accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            if (info.localKey != NULL) {
                deviceInfo->localSigningExists = true;
                deviceInfo->localSigningInfo = *info.localKey;
            }
            if (info.remoteKey != NULL) {
                deviceInfo->remoteSigningExists = true;
                deviceInfo->remoteSigningInfo = *info.remoteKey;
            }
        } else if (accept == GAP_NOT_ACCEPT) {
            deviceInfo->localSigningExists = false;
            deviceInfo->remoteSigningExists = false;
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

static int GapLeRequestSecurityMaster(const LeDeviceInfo *deviceInfo)
{
    int ret = GAP_SUCCESS;
    bool isCalled = false;
    const BtAddr *addr = &deviceInfo->addr;

    if (deviceInfo->encryptionStatus >= deviceInfo->securityReq->secReqStatus) {
        deviceInfo->securityReq->callback(
            &deviceInfo->addr, GAP_STATUS_SUCCESS, deviceInfo->encryptionStatus, deviceInfo->securityReq->context);
        return ret;
    }

    if (!isCalled) {
        if (g_leSecurityCallback.callback.leRemoteEncryptionKeyReqEvent) {
            g_leSecurityCallback.callback.leRemoteEncryptionKeyReqEvent(addr, g_leSecurityCallback.context);
            isCalled = true;
        }
    }
    if (!isCalled) {
        if (g_lePairCallback.callback.lePairFeatureReq) {
            g_lePairCallback.callback.lePairFeatureReq(
                addr, !deviceInfo->isRemoteSecurityRequest, g_lePairCallback.context);
            isCalled = true;
        }
    }
    if (!isCalled) {
        ret = GAP_ERR_NOT_SUPPORT;
    }

    return ret;
}

int GapLeRequestSecurityProcess(LeDeviceInfo *deviceInfo)
{
    int ret = GAP_SUCCESS;
    uint8_t authReq = AUTH_REQ_DEFAULT;

    if (deviceInfo->securityStatus != GAP_LE_SECURITY_STATUS_IDLE) {
        return ret;
    }
    if (deviceInfo->role == LE_CONNECTION_ROLE_MASTER) {
        ret = GapLeRequestSecurityMaster(deviceInfo);
    } else {
        if (deviceInfo->securityReq != NULL && deviceInfo->securityReq->secReqStatus == GAP_LE_NO_ENCRYPTION) {
            deviceInfo->securityReq->result = GAP_STATUS_SUCCESS;
            GapDoLeSecurityCallback(&deviceInfo->handle);
        } else if (deviceInfo->securityReq != NULL &&
                   deviceInfo->securityReq->secReqStatus == GAP_LE_UNAUTHENTICATED_ENCRYPTION) {
            authReq &= GapIsLeBondableMode() ? ~0 : ~SMP_AUTH_REQ_BONDING;
        } else {
            authReq &= GapIsLeBondableMode() ? ~0 : ~SMP_AUTH_REQ_BONDING;
            authReq &= SMP_AUTH_REQ_BIT_MITM;
        }

        deviceInfo->isLocalSecurityRequest = true;
        ret = SMP_SendSecurityRequestToRemote(deviceInfo->handle, authReq);
    }
    return ret;
}

int GAP_LeRequestSecurity(
    const BtAddr *addr, GAP_LeSecurityStatus status, GapLeRequestSecurityResult callback, void *context)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT " status:%{public}d", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), status);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL && deviceInfo->securityReq == NULL) {
        deviceInfo->securityReq = MEM_MALLOC.alloc(sizeof(LeSecurityReq));
        if (deviceInfo->securityReq != NULL) {
            deviceInfo->securityReq->secReqStatus = status;
            deviceInfo->securityReq->callback = callback;
            deviceInfo->securityReq->context = context;

            ret = GapLeRequestSecurityProcess(deviceInfo);
        } else {
            ret = GAP_ERR_OUT_OF_RES;
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

static void GapDoLePairFeatureReqCallback(void *data)
{
    LeDeviceInfo *deviceInfo = data;

    if (g_lePairCallback.callback.lePairFeatureReq) {
        g_lePairCallback.callback.lePairFeatureReq(
            &deviceInfo->addr, !deviceInfo->isRemoteSecurityRequest, g_lePairCallback.context);
    }
}

int GAP_RegisterLePairCallback(const GapLePairCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(
            &g_lePairCallback.callback, sizeof(g_lePairCallback.callback), 0x00, sizeof(g_lePairCallback.callback));
    } else {
        g_lePairCallback.callback = *callback;
    }
    g_lePairCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterLePairCallback(void)
{
    (void)memset_s(
        &g_lePairCallback.callback, sizeof(g_lePairCallback.callback), 0x00, sizeof(g_lePairCallback.callback));
    g_lePairCallback.context = NULL;
    return GAP_SUCCESS;
}

void GapClearPairingStatus(const BtAddr *addr)
{
    LeBondBlock *leBondBlock = GapGetLeBondBlock();
    if (GapAddrCompare(addr, &leBondBlock->addr)) {
        leBondBlock->isPairing = false;
    }
}

static int GapLePair(const BtAddr *addr)
{
    int ret = GAP_SUCCESS;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;
    uint8_t authReq = AUTH_REQ_DEFAULT;
    BtmLocalVersionInformation version;

    BTM_GetLocalVersionInformation(&version);
    if (version.hciVersion < BLUETOOTH_CORE_SPECIFICATION_4_2) {
        authReq &= ~SMP_AUTH_REQ_BIT_SC;
    }

    connectionInfoBlock = GapGetLeConnectionInfoBlock();
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (deviceInfo->securityStatus == GAP_LE_SECURITY_STATUS_IDLE) {
            deviceInfo->securityStatus = GAP_LE_SECURITY_STATUS_PAIR;
            if (deviceInfo->role == LE_CONNECTION_ROLE_MASTER) {
                GapDoLePairFeatureReqCallback(deviceInfo);
            } else {
                deviceInfo->isLocalSecurityRequest = true;
                SMP_SendSecurityRequestToRemote(deviceInfo->handle, authReq);
            }
        } else {
            ret = GAP_ERR_INVAL_STATE;
        }
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

void GapLeDoPair(const void *addr)
{
    int ret = GapLePair(addr);
    if (ret != BT_NO_ERROR) {
        LOG_WARN("GapLePair ret = %{public}d", ret);
    }
}

int GAP_LePair(const BtAddr *addr)
{
    int ret;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    LeBondBlock *leBondBlock = GapGetLeBondBlock();
    if (leBondBlock->isPairing) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
        LeDeviceInfo *deviceInfo = NULL;
        deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
        if (deviceInfo != NULL) {
            ret = GapLePair(addr);
        } else {
            ret = BTM_LeConnect(addr);
        }
        leBondBlock->isPairing = true;
        (void)memcpy_s(&leBondBlock->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    }

    return ret;
}

int GAP_LeCancelPair(const BtAddr *addr)
{
    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    LeBondBlock *leBondBlock = GapGetLeBondBlock();
    if (!leBondBlock->isPairing) {
        LOG_WARN("%{public}s: invalid state", __FUNCTION__);
    }
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        return SMP_CancelPair(deviceInfo->handle);
    } else {
        return BTM_LeCancelConnect(addr);
    }
}

int GAP_LeSetMinEncKeySize(uint8_t minSize)
{
    LeLocalInfo *localInfo = NULL;

    LOG_INFO("%{public}s: min key size:%hhu", __FUNCTION__, minSize);

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    localInfo = GapGetLeLocalInfo();
    localInfo->minEncKeySize = minSize;

    return GAP_SUCCESS;
}

int GAP_LePairFeatureRsp(const BtAddr *addr, GapLePairFeature localFeature)
{
    int ret;
    LeLocalInfo *localInfo = NULL;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    localInfo = GapGetLeLocalInfo();
    connectionInfoBlock = GapGetLeConnectionInfoBlock();

    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (localInfo->bondableMode == GAP_BONDABLE_MODE_NON) {
            localFeature.authReq &= ~SMP_AUTH_REQ_BONDING;
            localFeature.initKeyDis &= (deviceInfo->role == LE_CONNECTION_ROLE_MASTER) ? GAP_LE_KEY_DIST_ID_KEY : 0;
            localFeature.respKeyDis &= (deviceInfo->role == LE_CONNECTION_ROLE_SLAVE) ? GAP_LE_KEY_DIST_ID_KEY : 0;
        }

        if (deviceInfo->securityReq && deviceInfo->securityReq->secReqStatus == GAP_LE_AUTHENTICATED_ENCRYPTION) {
            localFeature.authReq |= SMP_AUTH_REQ_BIT_MITM;
        }

        BtmLocalVersionInformation version;
        BTM_GetLocalVersionInformation(&version);
        if (version.hciVersion < BLUETOOTH_CORE_SPECIFICATION_4_2) {
            localFeature.authReq &= ~SMP_AUTH_REQ_BIT_SC;
        }

        SMP_PairParam pairParam = {
            .ioCapability = localFeature.ioCapability,
            .oobDataFlag = localFeature.oobDataFlag,
            .authReq = localFeature.authReq,
            .maxEncKeySize = localFeature.maxEncKeySize,
            .initKeyDist = localFeature.initKeyDis,
            .respKeyDist = localFeature.respKeyDis,
        };

        if (deviceInfo->role == LE_CONNECTION_ROLE_MASTER) {
            ret = SMP_StartPair(deviceInfo->handle, &deviceInfo->ownAddr, &deviceInfo->peerAddr, &pairParam);
        } else {
            ret = SMP_RemotePairRequestReply(
                deviceInfo->handle, SMP_PAIR_FAILED_NO_FAILED, &deviceInfo->ownAddr, &deviceInfo->peerAddr, &pairParam);
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}
int GAP_LePairPassKeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    connectionInfoBlock = GapGetLeConnectionInfoBlock();
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(
                deviceInfo->handle, true, 0x00, deviceInfo->pairMethod, (uint8_t *)&number);
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(
                deviceInfo->handle, false, SMP_PAIR_FAILED_PASSKEY_ENTRY, deviceInfo->pairMethod, NULL);
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}
int GAP_LePairOobRsp(const BtAddr *addr, uint8_t accept, uint8_t oobData[GAP_OOB_DATA_SIZE])
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    connectionInfoBlock = GapGetLeConnectionInfoBlock();
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(deviceInfo->handle, true, 0x00, deviceInfo->pairMethod, oobData);
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(
                deviceInfo->handle, false, SMP_PAIR_FAILED_OOB_NOT_AVAILABLE, deviceInfo->pairMethod, NULL);
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    } else {
        ret = GAP_ERR_INVAL_PARAM;
    }

    return ret;
}

int GAP_LePairScOobRsp(const BtAddr *addr, uint8_t accept, const uint8_t oobDataC[GAP_OOB_DATA_SIZE],
    const uint8_t oobDataR[GAP_OOB_DATA_SIZE])
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    connectionInfoBlock = GapGetLeConnectionInfoBlock();
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            const int length = BT_ADDRESS_SIZE + GAP_OOB_DATA_RANDOM_SIZE + GAP_OOB_DATA_CONFIRM_SIZE;
            uint8_t smData[BT_ADDRESS_SIZE + GAP_OOB_DATA_RANDOM_SIZE + GAP_OOB_DATA_CONFIRM_SIZE] = {0};
            int offset = 0;
            (void)memcpy_s(smData + offset, length - offset, deviceInfo->peerAddr.addr, BT_ADDRESS_SIZE);
            offset += BT_ADDRESS_SIZE;
            (void)memcpy_s(smData + offset, length - offset, oobDataR, GAP_OOB_DATA_RANDOM_SIZE);
            offset += GAP_OOB_DATA_RANDOM_SIZE;
            (void)memcpy_s(smData + offset, length - offset, oobDataC, GAP_OOB_DATA_CONFIRM_SIZE);
            
            ret = SMP_AuthenticationRequestReply(deviceInfo->handle, true, 0x00, deviceInfo->pairMethod, smData);
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(
                deviceInfo->handle, false, SMP_PAIR_FAILED_NUMERIC_COMPARISON, deviceInfo->pairMethod, NULL);
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}
int GAP_LePairScUserConfirmRsp(const BtAddr *addr, uint8_t accept)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;
    LeConnectionInfoBlock *connectionInfoBlock = NULL;
    LeDeviceInfo *deviceInfo = NULL;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    connectionInfoBlock = GapGetLeConnectionInfoBlock();
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL) {
        if (accept == GAP_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(deviceInfo->handle, true, 0x00, deviceInfo->pairMethod, NULL);
        } else if (accept == GAP_NOT_ACCEPT) {
            ret = SMP_AuthenticationRequestReply(
                deviceInfo->handle, false, SMP_PAIR_FAILED_NUMERIC_COMPARISON, deviceInfo->pairMethod, NULL);
        } else {
            ret = GAP_ERR_INVAL_PARAM;
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    return ret;
}

void GapDoSignatureCallback(void *data)
{
    SignatureRequestInfo *info = data;

    if (info->type == SIGNATURE_GENERATION) {
        GAPSignatureGenerationResult callback = info->callback;
        callback(info->result, info->signature, info->context);
    } else if (info->type == SIGNATURE_CONFIRMATION) {
        GAPSignatureConfirmationResult callback = info->callback;
        callback(info->result, info->context);
    }

    ListRemoveNode(GapGetLeSignatureBlock()->RequestList, data);
}

static SignatureRequestInfo *GapAllocSignatureRequestInfo(
    uint16_t handle, SignatureType type, uint32_t counter, void *callback, void *context)
{
    SignatureRequestInfo *info = MEM_MALLOC.alloc(sizeof(SignatureRequestInfo));
    if (info != NULL) {
        info->handle = handle;
        info->callback = callback;
        info->context = context;
        info->type = type;
        info->processing = true;
        info->counter = counter;
    }
    return info;
}

int GAP_LeDataSignatureGeneration(
    const BtAddr *addr, GapSignatureData dataInfo, GAPSignatureGenerationResult callback, void *context)
{
    if (callback == NULL) {
        return GAP_ERR_INVAL_PARAM;
    }
    uint8_t signature[GAP_SIGNATURE_SIZE] = {0};
    if (GapIsLeEnable() == false) {
        callback(GAP_SIGNATURE_ERR_EXECUTION, signature, context);
        return GAP_ERR_NOT_ENABLE;
    }

    if (addr == NULL) {
        callback(GAP_SIGNATURE_ERR_EXECUTION, signature, context);
        return GAP_ERR_INVAL_PARAM;
    }

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    int ret = GAP_ERR_INVAL_STATE;
    LeConnectionInfoBlock *connectionInfoBlock = GapGetLeConnectionInfoBlock();
    LeDeviceInfo *deviceInfo = NULL;
    deviceInfo = ListForEachData(connectionInfoBlock->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL && deviceInfo->localSigningExists) {
        uint32_t counter = deviceInfo->localSigningInfo.counter;
        SignatureRequestInfo *info =
            GapAllocSignatureRequestInfo(deviceInfo->handle, SIGNATURE_GENERATION, counter, callback, context);
        if (info != NULL) {
            ListAddLast(GapGetLeSignatureBlock()->RequestList, info);

            ret = SMP_GenerateSignature(deviceInfo->localSigningInfo.csrk, counter, dataInfo.data, dataInfo.dataLen);
            if (ret != BT_NO_ERROR) {
                ListRemoveLast(GapGetLeSignatureBlock()->RequestList);
            }
        } else {
            ret = GAP_ERR_OUT_OF_RES;
        }
    }

    if (ret != BT_NO_ERROR) {
        LOG_INFO("%{public}s: ret = %{public}d", __FUNCTION__, ret);
        callback(GAP_SIGNATURE_ERR_EXECUTION, signature, context);
    }

    return ret;
}

int GAP_LeDataSignatureConfirmation(const BtAddr *addr, GapSignatureData dataInfo,
    const uint8_t signature[GAP_SIGNATURE_SIZE], GAPSignatureConfirmationResult callback, void *context)
{
    int ret = GAP_SUCCESS;

    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));

    if (GapIsLeEnable() == false) {
        callback(GAP_SIGNATURE_ERR_EXECUTION, context);
        return GAP_ERR_NOT_ENABLE;
    }

    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByAddr, (void *)addr);
    if (deviceInfo != NULL && deviceInfo->remoteSigningExists) {
        uint32_t counter = *(uint32_t *)signature;
        SignatureRequestInfo *info =
            GapAllocSignatureRequestInfo(deviceInfo->handle, SIGNATURE_CONFIRMATION, counter, callback, context);
        if (info != NULL) {
            (void)memcpy_s(info->signature, GAP_SIGNATURE_SIZE, signature, GAP_SIGNATURE_SIZE);
            ListAddLast(GapGetLeSignatureBlock()->RequestList, info);
            if (deviceInfo->remoteSigningInfo.counter > counter) {
                info->processing = false;
                info->result = GAP_SIGNATURE_ERR_COUNTER;
                GapDoSignatureCallback(info);
            } else {
                ret =
                    SMP_GenerateSignature(deviceInfo->remoteSigningInfo.csrk, counter, dataInfo.data, dataInfo.dataLen);
            }
            if (ret != BT_NO_ERROR) {
                ListRemoveLast(GapGetLeSignatureBlock()->RequestList);
            }
        }
    } else {
        ret = GAP_ERR_INVAL_STATE;
    }

    if (ret != BT_NO_ERROR) {
        LOG_INFO("%{public}s: ret = %{public}d", __FUNCTION__, ret);
        callback(GAP_SIGNATURE_ERR_EXECUTION, context);
    }

    return ret;
}

static void GapDoCallbackSignCounterChange(SignatureRequestInfo *info)
{
    LeSignKey *signKey = NULL;
    GAP_SignCounterType signCounterType = LOCAL_SIGN_COUNTER;
    LeDeviceInfo *deviceInfo =
        ListForEachData(GapGetLeConnectionInfoBlock()->deviceList, GapFindLeConnectionDeviceByHandle, &info->handle);
    if (deviceInfo != NULL) {
        if (info->type == SIGNATURE_GENERATION) {
            signKey = &deviceInfo->localSigningInfo;
            signCounterType = LOCAL_SIGN_COUNTER;
        } else if (info->type == SIGNATURE_CONFIRMATION) {
            signKey = &deviceInfo->remoteSigningInfo;
            signCounterType = REMOTE_SIGN_COUNTER;
        }
        if (signKey == NULL) {
            return;
        }
        signKey->counter = info->counter + 1;

        if (g_leSecurityCallback.callback.leSignCounterChangeNotification) {
            g_leSecurityCallback.callback.leSignCounterChangeNotification(
                &deviceInfo->addr, signCounterType, signKey->counter, g_leSecurityCallback.context);
        }
    }
}

void GapLeGenerateSignatureResult(uint8_t status, const uint8_t *sign)
{
    LOG_INFO("%{public}s: status:0x%02x", __FUNCTION__, status);

    List *list = GapGetLeSignatureBlock()->RequestList;
    ListNode *node = ListGetFirstNode(list);
    SignatureRequestInfo *info = NULL;
    while (node != 0) {
        info = ListGetNodeData(node);
        if (info != NULL && info->processing) {
            info->processing = false;
            break;
        }
        node = ListGetNextNode(node);
    }

    if (node != NULL) {
        if (status != SMP_GENERATE_SIGN_STATUS_SUCCESS) {
            info->result = GAP_SIGNATURE_ERR_EXECUTION;
        } else {
            if (info->type == SIGNATURE_CONFIRMATION && memcmp(sign, info->signature, GAP_SIGNATURE_SIZE)) {
                info->result = GAP_SIGNATURE_ERR_ALGORITHM;
            } else {
                (void)memcpy_s(info->signature, GAP_SIGNATURE_SIZE, sign, GAP_SIGNATURE_SIZE);
                info->result = GAP_SIGNATURE_OK;
                GapDoCallbackSignCounterChange(info);
            }
        }

        GapDoSignatureCallback(info);
    }
}

int GapRequestSigningAlgorithmInfo(const BtAddr *addr)
{
    int ret;
    if (g_leSecurityCallback.callback.GapRequestSigningAlgorithmInfo) {
        g_leSecurityCallback.callback.GapRequestSigningAlgorithmInfo(addr, g_leSecurityCallback.context);
        ret = GAP_SUCCESS;
    } else {
        ret = GAP_ERR_NOT_SUPPORT;
    }

    return ret;
}