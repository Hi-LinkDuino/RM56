/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "das_task_common.h"
#include "alg_defs.h"
#include "alg_loader.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "string_util.h"

#define KEY_TYPE_PAIR_LEN 2
#define PACKAGE_NAME_MAX_LEN 256
#define SERVICE_TYPE_MAX_LEN 256
#define AUTH_ID_MAX_LEN 64

#define MESSAGE_RETURN 0x8000
#define MESSAGE_PREFIX 0x0010

/* in order to expand to uint16_t */
static const uint8_t KEY_TYPE_PAIRS[KEY_ALIAS_TYPE_END][KEY_TYPE_PAIR_LEN] = {
    { 0x00, 0x00 }, /* ACCESSOR_PK */
    { 0x00, 0x01 }, /* CONTROLLER_PK */
    { 0x00, 0x02 }, /* ed25519 KEYPAIR */
    { 0x00, 0x03 }, /* KEK, key encryption key, used only by DeviceAuthService */
    { 0x00, 0x04 }, /* DEK, data encryption key, used only by upper apps */
    { 0x00, 0x05 }, /* key tmp */
    { 0x00, 0x06 }, /* PSK, preshared key index */
    { 0x00, 0x07 }  /* AUTHTOKEN */
};

void DasSendErrorToOut(CJson *out, int errCode)
{
    CJson *sendToSelf = CreateJson();
    if (sendToSelf == NULL) {
        LOGE("Create sendToSelf json failed.");
        return;
    }
    CJson *sendToPeer = CreateJson();
    if (sendToPeer == NULL) {
        LOGE("Create sendToPeer json failed.");
        FreeJson(sendToSelf);
        return;
    }
    CJson *payload = CreateJson();
    if (payload == NULL) {
        LOGE("Create payload json failed.");
        goto ERR;
    }
    int res;
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToSelf, FIELD_AUTH_FORM, AUTH_FORM_ACCOUNT_UNRELATED), res);
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToSelf, FIELD_ERROR_CODE, errCode), res);

    GOTO_ERR_AND_SET_RET(AddIntToJson(payload, FIELD_ERROR_CODE, errCode), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(sendToPeer, FIELD_PAYLOAD, payload), res);
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToPeer, FIELD_MESSAGE, ERR_MESSAGE), res);

    GOTO_ERR_AND_SET_RET(AddObjToJson(out, FIELD_SEND_TO_PEER, sendToPeer), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(out, FIELD_SEND_TO_SELF, sendToSelf), res);
ERR:
    FreeJson(sendToPeer);
    FreeJson(sendToSelf);
    FreeJson(payload);
}

void DasSendErrMsgToSelf(CJson *out, int errCode)
{
    CJson *sendToSelf = CreateJson();
    if (sendToSelf == NULL) {
        LOGE("Create sendToSelf json failed.");
        return;
    }

    if (AddIntToJson(sendToSelf, FIELD_AUTH_FORM, AUTH_FORM_ACCOUNT_UNRELATED) != 0) {
        FreeJson(sendToSelf);
        LOGE("Add authForm failed.");
        return;
    }
    if (AddIntToJson(sendToSelf, FIELD_ERROR_CODE, errCode) != 0) {
        FreeJson(sendToSelf);
        LOGE("Add errCode failed.");
        return;
    }
    if (AddObjToJson(out, FIELD_SEND_TO_SELF, sendToSelf) != 0) {
        FreeJson(sendToSelf);
        LOGE("Add sendToSelf failed.");
        return;
    }
    FreeJson(sendToSelf);
}

uint32_t ProtocolMessageIn(const CJson *in)
{
    int32_t message = 0;
    if (GetIntFromJson(in, FIELD_MESSAGE, &message) != 0) {
        return INVALID_MESSAGE;
    }
    if (message == ERR_MESSAGE) {
        return ERR_MESSAGE;
    }
    return message & 0x000F; /* get lower 8 bit */
}

int ClientProtocolMessageOut(CJson *out, int opCode, uint32_t step)
{
    CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        LOGD("No need to send to peer");
        return HC_SUCCESS;
    }
    int res;
    switch (opCode) {
        case OP_BIND:
        case AUTH_KEY_AGREEMENT:
            res = AddIntToJson(sendToPeer, FIELD_MESSAGE, step);
            break;
        case AUTHENTICATE:
        case OP_UNBIND:
            step = step | MESSAGE_PREFIX;
            res = AddIntToJson(sendToPeer, FIELD_MESSAGE, step);
            break;
        default:
            LOGE("Unsupported opCode: %d.", opCode);
            return HC_ERR_NOT_SUPPORT;
    }
    return (res == 0) ? HC_SUCCESS : HC_ERR_JSON_ADD;
}

int ServerProtocolMessageOut(CJson *out, int opCode, uint32_t step)
{
    int res;
    CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        LOGD("No need to send to peer");
        return HC_SUCCESS;
    }
    switch (opCode) {
        case OP_BIND:
        case AUTH_KEY_AGREEMENT:
            step = step | MESSAGE_RETURN;
            res = AddIntToJson(sendToPeer, FIELD_MESSAGE, step);
            break;
        case AUTHENTICATE:
        case OP_UNBIND:
            step = step | MESSAGE_RETURN;
            step = step | MESSAGE_PREFIX;
            res = AddIntToJson(sendToPeer, FIELD_MESSAGE, step);
            break;
        default:
            LOGE("Unsupported opCode: %d.", opCode);
            return HC_ERR_NOT_SUPPORT;
    }
    return (res == 0) ? HC_SUCCESS : HC_ERR_JSON_ADD;
}

static int32_t CombineServiceId(const Uint8Buff *pkgName, const Uint8Buff *serviceType, Uint8Buff *serviceId)
{
    int32_t res = HC_SUCCESS;
    Uint8Buff serviceIdPlain = { NULL, 0 };
    serviceIdPlain.length = pkgName->length + serviceType->length;
    serviceIdPlain.val = (uint8_t *)HcMalloc(serviceIdPlain.length, 0);
    if (serviceIdPlain.val == NULL) {
        LOGE("malloc serviceIdPlain.val failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    if (memcpy_s(serviceIdPlain.val, serviceIdPlain.length, pkgName->val, pkgName->length) != EOK) {
        LOGE("Copy service id: pkgName failed.");
        res =  HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    if (memcpy_s(serviceIdPlain.val + pkgName->length,  serviceIdPlain.length - pkgName->length,
        serviceType->val, serviceType->length) != EOK) {
        LOGE("Copy service id: serviceType failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    res = GetLoaderInstance()->sha256(&serviceIdPlain, serviceId);
    if (res != HC_SUCCESS) {
        LOGE("Service id Sha256 failed.");
        goto ERR;
    }
ERR:
    HcFree(serviceIdPlain.val);
    return res;
}

static int32_t CombineKeyAlias(const Uint8Buff *serviceId, const Uint8Buff *keyType,
    const Uint8Buff *authId, Uint8Buff *keyAliasHash)
{
    int32_t res = HC_SUCCESS;
    Uint8Buff keyAliasBuff = { NULL, 0 };
    keyAliasBuff.length = serviceId->length + authId->length + keyType->length;
    keyAliasBuff.val = (uint8_t *)HcMalloc(keyAliasBuff.length, 0);
    if (keyAliasBuff.val == NULL) {
        LOGE("Malloc mem failed.");
        return HC_ERR_ALLOC_MEMORY;
    }

    uint32_t totalLen = keyAliasBuff.length;
    uint32_t usedLen = 0;
    if (memcpy_s(keyAliasBuff.val, totalLen, serviceId->val, serviceId->length) != EOK) {
        LOGE("Copy serviceId failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    usedLen = usedLen + serviceId->length;

    if (memcpy_s(keyAliasBuff.val + usedLen, totalLen - usedLen, keyType->val, keyType->length) != EOK) {
        LOGE("Copy keyType failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    usedLen = usedLen + keyType->length;

    if (memcpy_s(keyAliasBuff.val + usedLen, totalLen - usedLen, authId->val, authId->length) != EOK) {
        LOGE("Copy authId failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    res = GetLoaderInstance()->sha256(&keyAliasBuff, keyAliasHash);
    if (res != HC_SUCCESS) {
        LOGE("Sha256 failed.");
        goto ERR;
    }
ERR:
    HcFree(keyAliasBuff.val);
    return res;
}

static int32_t CombineKeyAliasForIso(const Uint8Buff *serviceId, const Uint8Buff *keyType,
    const Uint8Buff *authId, Uint8Buff *outKeyAlias)
{
    if (outKeyAlias->length != SHA256_LEN) {
        return HC_ERR_INVALID_LEN;
    }
    int32_t res = CombineKeyAlias(serviceId, keyType, authId, outKeyAlias);
    if (res != HC_SUCCESS) {
        LOGE("CombineKeyAlias failed.");
        return res;
    }
    return res;
}

static int32_t CombineKeyAliasForPake(const Uint8Buff *serviceId, const Uint8Buff *keyType,
    const Uint8Buff *authId, Uint8Buff *outKeyAlias)
{
    int32_t res;
    Uint8Buff keyAliasHash = { NULL, SHA256_LEN };
    char *outKeyAliasHex = NULL;
    if (outKeyAlias->length != SHA256_LEN * BYTE_TO_HEX_OPER_LENGTH) {
        res = HC_ERR_INVALID_LEN;
        goto ERR;
    }
    keyAliasHash.val = (uint8_t *)HcMalloc(keyAliasHash.length, 0);
    if (keyAliasHash.val == NULL) {
        LOGE("Malloc keyAliasHash failed");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    res = CombineKeyAlias(serviceId, keyType, authId, &keyAliasHash);
    if (res != HC_SUCCESS) {
        LOGE("CombineKeyAlias failed.");
        goto ERR;
    }
    uint32_t outKeyAliasHexLen = keyAliasHash.length * BYTE_TO_HEX_OPER_LENGTH + 1;
    outKeyAliasHex = (char *)HcMalloc(outKeyAliasHexLen, 0);
    res = ByteToHexString(keyAliasHash.val, keyAliasHash.length, outKeyAliasHex, outKeyAliasHexLen);
    if (res != HC_SUCCESS) {
        LOGE("ByteToHexString failed");
        goto ERR;
    }
    if (memcpy_s(outKeyAlias->val, outKeyAlias->length, outKeyAliasHex, strlen(outKeyAliasHex)) != EOK) {
        LOGE("memcpy outkeyalias failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
ERR:
    HcFree(keyAliasHash.val);
    HcFree(outKeyAliasHex);
    return res;
}

int32_t GenerateKeyAlias(const Uint8Buff *pkgName, const Uint8Buff *serviceType,
    const KeyAliasType keyType, const Uint8Buff *authId, Uint8Buff *outKeyAlias)
{
    CHECK_PTR_RETURN_ERROR_CODE(pkgName, "pkgName");
    CHECK_PTR_RETURN_ERROR_CODE(pkgName->val, "pkgName->val");
    CHECK_PTR_RETURN_ERROR_CODE(serviceType, "serviceType");
    CHECK_PTR_RETURN_ERROR_CODE(serviceType->val, "serviceType->val");
    CHECK_PTR_RETURN_ERROR_CODE(authId, "authId");
    CHECK_PTR_RETURN_ERROR_CODE(authId->val, "authId->val");
    CHECK_PTR_RETURN_ERROR_CODE(outKeyAlias, "outKeyAlias");
    CHECK_PTR_RETURN_ERROR_CODE(outKeyAlias->val, "outKeyAlias->val");
    if (pkgName->length == 0 || serviceType->length == 0 || authId->length == 0 || outKeyAlias->length == 0) {
        LOGE("Invalid zero length params exist.");
        return HC_ERR_INVALID_LEN;
    }
    if (pkgName->length > PACKAGE_NAME_MAX_LEN || serviceType->length > SERVICE_TYPE_MAX_LEN ||
        authId->length > AUTH_ID_MAX_LEN || keyType >= KEY_ALIAS_TYPE_END) {
        LOGE("Out of length params exist.");
        return HC_ERR_INVALID_LEN;
    }

    int32_t res;
    Uint8Buff serviceId = { NULL, SHA256_LEN };
    serviceId.val = (uint8_t *)HcMalloc(serviceId.length, 0);
    if (serviceId.val == NULL) {
        LOGE("Malloc for serviceId failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    res = CombineServiceId(pkgName, serviceType, &serviceId);
    if (res != HC_SUCCESS) {
        LOGE("CombineServiceId failed, res: %x.", res);
        goto ERR;
    }
    Uint8Buff keyTypeBuff = { (uint8_t *)KEY_TYPE_PAIRS[keyType], KEY_TYPE_PAIR_LEN };
    if (keyType == KEY_ALIAS_AUTH_TOKEN) {
        res = CombineKeyAliasForIso(&serviceId, &keyTypeBuff, authId, outKeyAlias);
    } else {
        res = CombineKeyAliasForPake(&serviceId, &keyTypeBuff, authId, outKeyAlias);
    }
    if (res != HC_SUCCESS) {
        LOGE("CombineKeyAlias failed, keyType: %d, res: %d", keyType, res);
    }
ERR:
    HcFree(serviceId.val);
    return res;
}

int32_t GetIdPeer(const CJson *in, const char *peerIdKey, const Uint8Buff *authIdSelf, Uint8Buff *authIdPeer)
{
    const char *authIdStr = GetStringFromJson(in, peerIdKey);
    if (authIdStr == NULL) {
        LOGE("Get peer id from json failed.");
        return HC_ERR_JSON_GET;
    }
    uint32_t authIdLen = HcStrlen(authIdStr) / BYTE_TO_HEX_OPER_LENGTH;
    if (authIdLen == 0 || authIdLen > MAX_AUTH_ID_LEN) {
        LOGE("Invalid authIdPeerLen: %u.", authIdLen);
        return HC_ERR_INVALID_LEN;
    }
    int32_t res = InitSingleParam(authIdPeer, authIdLen);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for peer authId failed, res: %d.", res);
        return res;
    }
    if (HexStringToByte(authIdStr, authIdPeer->val, authIdPeer->length) != HC_SUCCESS) {
        LOGE("HexStringToByte for authIdPeer failed.");
        return HC_ERR_CONVERT_FAILED;
    }
    if ((authIdSelf->length == authIdPeer->length) &&
        memcmp(authIdSelf->val, authIdPeer->val, authIdSelf->length) == 0) {
        LOGE("Peer id can not be equal to self id.");
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

int32_t GetAndCheckAuthIdPeer(const CJson *in, const Uint8Buff *authIdSelf, const Uint8Buff *authIdPeer)
{
    const CJson *payload = GetObjFromJson(in, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Get payload failed.");
        return HC_ERR_JSON_GET;
    }
    const char *authIdStr = GetStringFromJson(payload, FIELD_PEER_AUTH_ID);
    if (authIdStr == NULL) {
        LOGE("Get peer authId from payload failed.");
        return HC_ERR_JSON_GET;
    }
    uint32_t authIdPeerLen = HcStrlen(authIdStr) / BYTE_TO_HEX_OPER_LENGTH;
    if (authIdPeerLen == 0 || authIdPeerLen > MAX_AUTH_ID_LEN) {
        LOGE("Invalid peer authId length.");
        return HC_ERR_INVALID_LEN;
    }
    uint8_t *authIdPeerTmp = (uint8_t *)HcMalloc(authIdPeerLen, 0);
    if (authIdPeerTmp == NULL) {
        LOGE("Malloc for authIdPeerTmp failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (HexStringToByte(authIdStr, authIdPeerTmp, authIdPeerLen) != HC_SUCCESS) {
        LOGE("Convert peer authId from hex string to byte failed.");
        HcFree(authIdPeerTmp);
        return HC_ERR_CONVERT_FAILED;
    }
    if ((authIdSelf->length == authIdPeer->length) &&
        memcmp(authIdSelf->val, authIdPeer->val, authIdSelf->length) == EOK) {
        LOGE("Peer id can not be equal to self id.");
        HcFree(authIdPeerTmp);
        return HC_ERR_INVALID_PARAMS;
    }
    if (memcmp(authIdPeer->val, authIdPeerTmp, authIdPeer->length) != EOK) {
        LOGE("Peer authId does not match.");
        HcFree(authIdPeerTmp);
        return HC_ERR_INVALID_PARAMS;
    }
    HcFree(authIdPeerTmp);
    return HC_SUCCESS;
}

int32_t GetAuthIdPeerFromPayload(const CJson *in, const Uint8Buff *authIdSelf, Uint8Buff *authIdPeer)
{
    const CJson *payload = GetObjFromJson(in, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Not have payload.");
        return HC_ERR_JSON_GET;
    }
    int res = GetIdPeer(payload, FIELD_PEER_AUTH_ID, authIdSelf, authIdPeer);
    if (res != HC_SUCCESS) {
        LOGE("GetIdPeer failed, res: %d.", res);
    }
    return res;
}

int32_t GetAndCheckKeyLenOnServer(const CJson *in, uint32_t *keyLen)
{
    const CJson *payload = GetObjFromJson(in, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Get payload failed.");
        return HC_ERR_JSON_GET;
    }
    uint32_t tmpKeyLen = 0;
    if (GetIntFromJson(payload, FIELD_KEY_LENGTH, (int32_t *)&tmpKeyLen) != HC_SUCCESS) {
        LOGE("Get tmpKeyLen from payload failed.");
        return HC_ERR_JSON_GET;
    }

    if (*keyLen != tmpKeyLen) {
        LOGE("Key length is not equal.");
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}
