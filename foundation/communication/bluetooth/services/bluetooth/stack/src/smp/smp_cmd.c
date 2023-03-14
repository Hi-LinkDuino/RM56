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

#include "smp_cmd.h"

#include "l2cap_le_if.h"
#include "log.h"
#include "packet.h"
#include "smp_def.h"

static int SMP_SendData(uint16_t handle, Packet *pkt, SMP_SendDataCb cb);

int SMP_SendPairingRequest(uint16_t handle, const SMP_PairParam *pairReqParam, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_REQ;

    LOG_INFO("%{public}s", __FUNCTION__);
    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_REQ_DATA_LEN);
    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, &pairReqParam->ioCapability, offset, sizeof(pairReqParam->ioCapability));
    offset += sizeof(pairReqParam->ioCapability);
    PacketPayloadWrite(pkt, &pairReqParam->oobDataFlag, offset, sizeof(pairReqParam->oobDataFlag));
    offset += sizeof(pairReqParam->oobDataFlag);
    PacketPayloadWrite(pkt, &pairReqParam->authReq, offset, sizeof(pairReqParam->authReq));
    offset += sizeof(pairReqParam->authReq);
    PacketPayloadWrite(pkt, &pairReqParam->maxEncKeySize, offset, sizeof(pairReqParam->maxEncKeySize));
    offset += sizeof(pairReqParam->maxEncKeySize);
    PacketPayloadWrite(pkt, &pairReqParam->initKeyDist, offset, sizeof(pairReqParam->initKeyDist));
    offset += sizeof(pairReqParam->initKeyDist);
    PacketPayloadWrite(pkt, &pairReqParam->respKeyDist, offset, sizeof(pairReqParam->respKeyDist));

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d.", ret);
    }
    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingResponse(uint16_t handle, const SMP_PairParam *pairRspParam, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_RSP;

    LOG_INFO("%{public}s", __FUNCTION__);
    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_RSP_DATA_LEN);
    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, &pairRspParam->ioCapability, offset, sizeof(pairRspParam->ioCapability));
    offset += sizeof(pairRspParam->ioCapability);
    PacketPayloadWrite(pkt, &pairRspParam->oobDataFlag, offset, sizeof(pairRspParam->oobDataFlag));
    offset += sizeof(pairRspParam->oobDataFlag);
    PacketPayloadWrite(pkt, &pairRspParam->authReq, offset, sizeof(pairRspParam->authReq));
    offset += sizeof(pairRspParam->authReq);
    PacketPayloadWrite(pkt, &pairRspParam->maxEncKeySize, offset, sizeof(pairRspParam->maxEncKeySize));
    offset += sizeof(pairRspParam->maxEncKeySize);
    PacketPayloadWrite(pkt, &pairRspParam->initKeyDist, offset, sizeof(pairRspParam->initKeyDist));
    offset += sizeof(pairRspParam->initKeyDist);
    PacketPayloadWrite(pkt, &pairRspParam->respKeyDist, offset, sizeof(pairRspParam->respKeyDist));

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d. ", ret);
    }
    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingConfirm(uint16_t handle, const uint8_t *confirm, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_CFM;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_CFM_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, confirm, offset, SMP_CONFIRM_DATA_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingRandom(uint16_t handle, const uint8_t *random, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_RAND;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_RAND_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, random, offset, SMP_RANDOM_DATA_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingFailed(uint16_t handle, uint8_t reason, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_FAIL;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_FAIL_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, &reason, offset, sizeof(reason));

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingPublicKey(uint16_t handle, const uint8_t *publicKey, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_PUBLIC_KEY;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_PUBLIC_KEY_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, publicKey, offset, SMP_PUBLICKEY_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendPairingDHKeyCheck(uint16_t handle, const uint8_t *dhKeyCheck, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_PAIRING_DHKEY_CHECK;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_PAIR_DHK_CHECK_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, dhKeyCheck, offset, SMP_DHKEY_CHECK_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendEncryptionInformation(uint16_t handle, const uint8_t *ltk, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_ENCRYPTION_INFO;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_ENCRYPTION_INFO_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, ltk, offset, SMP_LTK_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendMasterIdentification(uint16_t handle, uint16_t ediv, const uint8_t *rand, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_MASTER_IDENTITY;
    uint8_t edivBuf[sizeof(ediv)] = {0x00};

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_MASTER_IDENTITY_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    edivBuf[0] = (uint8_t)(ediv & 0x00FFu);
    edivBuf[1] = (uint8_t)((ediv >> 0x08) & 0x00FFu);
    PacketPayloadWrite(pkt, edivBuf, offset, sizeof(edivBuf));
    offset += sizeof(edivBuf);
    PacketPayloadWrite(pkt, rand, offset, SMP_MASTER_RAND_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendIdentityInformation(uint16_t handle, const uint8_t *irk, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_IDENTITY_INFO;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_IDENTITY_INFO_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, irk, offset, SMP_IRK_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendIdentityAddressInformation(uint16_t handle, const BtAddr *addr, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_IDENTITY_ADDR_INFO;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_IDENTITY_ADDR_INFO_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, &addr->type, offset, sizeof(addr->type));
    offset += sizeof(addr->type);
    PacketPayloadWrite(pkt, addr->addr, offset, BT_ADDRESS_SIZE);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendSigningInformation(uint16_t handle, const uint8_t *csrk, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_SIGNING_INFO;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_SIGNING_INFO_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, csrk, offset, SMP_CSRK_LEN);

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

int SMP_SendSecurityRequest(uint16_t handle, uint8_t authReq, SMP_SendDataCb cb)
{
    Packet *pkt = NULL;
    size_t offset = 0x00;
    uint8_t code = SMP_CODE_SECURITY_REQ;

    LOG_INFO("%{public}s", __FUNCTION__);

    pkt = PacketMalloc(0x00, 0x00, SMP_CMD_SECURITY_REQ_DATA_LEN);

    PacketPayloadWrite(pkt, &code, offset, sizeof(code));
    offset += sizeof(code);
    PacketPayloadWrite(pkt, &authReq, offset, sizeof(authReq));

    int ret = SMP_SendData(handle, pkt, cb);
    if (ret != SMP_SUCCESS) {
        LOG_ERROR("Send LE Packet Error: %{public}d", ret);
    }

    PacketFree(pkt);

    return ret;
}

static int SMP_SendData(uint16_t handle, Packet *pkt, SMP_SendDataCb cb)
{
    LOG_INFO("%{public}s: size(%u)", __FUNCTION__, PacketSize(pkt));
    int ret = L2CIF_LeSendFixChannelData(handle, L2CAP_LE_SMP_CHANNEL, pkt, cb);

    return ret;
}