/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_ipc_serialization.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"

static const uint8_t g_base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int32_t CopyUint32ToBuffer(uint32_t value, const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if ((*destOffset > destBlob->size) || ((destBlob->size - *destOffset) < sizeof(value))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset, &value, sizeof(value)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    *destOffset += sizeof(value);

    return HKS_SUCCESS;
}

static int32_t CopyBlobToBuffer(const struct HksBlob *blob, const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if ((*destOffset > destBlob->size) ||
        ((destBlob->size - *destOffset) < (sizeof(blob->size) + ALIGN_SIZE(blob->size)))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset,
        &(blob->size), sizeof(blob->size)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    *destOffset += sizeof(blob->size);

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset, blob->data, blob->size) != EOK) {
        *destOffset -= sizeof(blob->size);
        return HKS_ERROR_BAD_STATE;
    }
    *destOffset += ALIGN_SIZE(blob->size);

    return HKS_SUCCESS;
}

static int32_t CopyParamSetToBuffer(const struct HksParamSet *paramSet,
    const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if ((*destOffset > destBlob->size) || (destBlob->size - *destOffset < ALIGN_SIZE(paramSet->paramSetSize))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset,
        paramSet, paramSet->paramSetSize) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    *destOffset += ALIGN_SIZE(paramSet->paramSetSize);

    return HKS_SUCCESS;
}

static int32_t GetUint32FromBuffer(uint32_t *value, const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if ((*srcOffset > srcBlob->size) || (srcBlob->size - *srcOffset < sizeof(*value))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    *value = *((uint32_t *)(srcBlob->data + *srcOffset));
    *srcOffset += sizeof(*value);
    return HKS_SUCCESS;
}

static int32_t GetBlobFromBuffer(struct HksBlob *blob, const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if ((*srcOffset > srcBlob->size) || ((srcBlob->size - *srcOffset) < sizeof(blob->size))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    uint32_t size = *((uint32_t *)(srcBlob->data + *srcOffset));
    if (ALIGN_SIZE(size) > (srcBlob->size - *srcOffset - sizeof(blob->size))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    blob->size = size;
    *srcOffset += sizeof(blob->size);
    blob->data = (uint8_t *)(srcBlob->data + *srcOffset);
    *srcOffset += ALIGN_SIZE(blob->size);

    return HKS_SUCCESS;
}

static int32_t GetParamSetFromBuffer(struct HksParamSet **paramSet,
    const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if ((*srcOffset > srcBlob->size) || ((srcBlob->size - *srcOffset) < sizeof(struct HksParamSet))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    *paramSet = (struct HksParamSet *)(srcBlob->data + *srcOffset);
    if (ALIGN_SIZE((*paramSet)->paramSetSize) > (srcBlob->size - *srcOffset)) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    *srcOffset += ALIGN_SIZE((*paramSet)->paramSetSize);

    return HKS_SUCCESS;
}

int32_t HksGenerateKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, const struct HksBlob *keyOut)
{
    uint32_t offset = 0;
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }

    ret = CopyParamSetToBuffer(paramSetIn, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSetIn failed");
        return ret;
    }
    return CopyUint32ToBuffer(keyOut->size, destData, &offset);
}

int32_t HksImportKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key)
{
    uint32_t offset = 0;
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }

    ret = CopyParamSetToBuffer(paramSet, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
        return ret;
    }
    return CopyBlobToBuffer(key, destData, &offset);
}

int32_t HksExportPublicKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *key)
{
    uint32_t offset = 0;
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }
    return CopyUint32ToBuffer(key->size, destData, &offset);
}

int32_t HksGetKeyParamSetPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *keyOut)
{
    uint32_t offset = 0;
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }
    return CopyUint32ToBuffer(keyOut->size, destData, &offset);
}

int32_t HksOnceParamPack(struct HksBlob *destData, const struct HksBlob *key, const struct HksParamSet *paramSet,
    uint32_t *offset)
{
    int32_t ret = CopyBlobToBuffer(key, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy key failed");
        return ret;
    }

    return CopyParamSetToBuffer(paramSet, destData, offset);
}

int32_t HksOnceDataPack(struct HksBlob *destData, const struct HksBlob *inputData, const struct HksBlob *rsvData,
    const struct HksBlob *outputData, uint32_t *offset)
{
    int32_t ret = CopyBlobToBuffer(inputData, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy inputData failed");
        return ret;
    }

    if (rsvData != NULL) {
        ret = CopyBlobToBuffer(rsvData, destData, offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy rsvData failed");
            return ret;
        }
    }

    if (outputData != NULL) {
        ret = CopyUint32ToBuffer(outputData->size, destData, offset);
    }
    return ret;
}

int32_t HksAgreeKeyPack(struct HksBlob *destData, const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey)
{
    uint32_t offset = 0;
    int32_t ret = CopyParamSetToBuffer(paramSet, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
        return ret;
    }

    ret = CopyBlobToBuffer(privateKey, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy privateKey failed");
        return ret;
    }

    ret = CopyBlobToBuffer(peerPublicKey, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy peerPublicKey failed");
        return ret;
    }
    return CopyUint32ToBuffer(agreedKey->size, destData, &offset);
}

int32_t HksDeriveKeyPack(struct HksBlob *destData, const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    const struct HksBlob *derivedKey)
{
    uint32_t offset = 0;
    int32_t ret = CopyParamSetToBuffer(paramSet, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
        return ret;
    }

    ret = CopyBlobToBuffer(kdfKey, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy kdfKey failed");
        return ret;
    }
    return CopyUint32ToBuffer(derivedKey->size, destData, &offset);
}

int32_t HksGetKeyInfoListPack(struct HksBlob *destData, uint32_t listCount, const struct HksKeyInfo *keyInfoList)
{
    uint32_t offset = 0;
    int32_t ret = CopyUint32ToBuffer(listCount, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy listCount failed");
        return ret;
    }

    for (uint32_t i = 0; i < listCount; ++i) {
        ret = CopyUint32ToBuffer(keyInfoList[i].alias.size, destData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy alias failed");
            return ret;
        }

        ret = CopyUint32ToBuffer(keyInfoList[i].paramSet->paramSetSize, destData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy paramSetSize failed");
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksGetKeyInfoListUnpackFromService(const struct HksBlob *srcData, uint32_t *listCount,
    struct HksKeyInfo *keyInfoList)
{
    uint32_t offset = 0;
    uint32_t countFromBuffer;
    int32_t ret = GetUint32FromBuffer(&countFromBuffer, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get listCount failed");
        return ret;
    }

    if (countFromBuffer > *listCount) {
        HKS_LOG_E("listCount from buffer is invalid");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    *listCount = countFromBuffer;

    struct HksBlob alias = { 0, NULL };
    struct HksParamSet *paramSet = NULL;
    for (uint32_t i = 0; i < countFromBuffer; ++i) {
        ret = GetBlobFromBuffer(&alias, srcData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get alias failed");
            return ret;
        }

        if (memcpy_s(keyInfoList[i].alias.data, keyInfoList[i].alias.size, alias.data, alias.size) != EOK) {
            HKS_LOG_E("memcpy alias failed");
            return ret;
        }
        keyInfoList[i].alias.size = alias.size;

        ret = GetParamSetFromBuffer(&paramSet, srcData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get paramSet failed");
            return ret;
        }

        if (memcpy_s(keyInfoList[i].paramSet, keyInfoList[i].paramSet->paramSetSize,
            paramSet, paramSet->paramSetSize) != EOK) {
            HKS_LOG_E("memcpy paramSet failed");
            return ret;
        }

        ret = HksFreshParamSet(keyInfoList[i].paramSet, false);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("FreshParamSet fail, ret = %d", ret);
            return ret;
        }
    }

    return HKS_SUCCESS;
}

int32_t HksCertificateChainPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *certChainBlob)
{
    uint32_t offset = 0;
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }

    ret = CopyParamSetToBuffer(paramSet, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
        return ret;
    }

    return CopyUint32ToBuffer(certChainBlob->size, destData, &offset);
}

static int32_t Base64Encode(const uint8_t *srcData, const uint32_t srcDataSize,
    uint8_t *outData, const uint32_t outDataSize)
{
    /*
     * outDataSize is already calculated on the outside.
     * Base64 encode like this:
     * <------------ byte ------------>
     * <------><-src1-><-src2-><-src3->
     * +------------------------------+
     * |      24      16      08      |
     * +------------------------------+
     *         <out1><out2><out3><out4>
     */
    uint32_t j = 0;
    uint32_t i = 0;
    while (i < srcDataSize) {
        uint32_t a = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        uint32_t b = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        uint32_t c = (i < srcDataSize) ? (uint8_t)srcData[i] : 0;
        ++i;
        /* srcData each character takes up 8 bits. 1, 2 and 3 is offset */
        uint32_t byte = (a << (8 * 2)) + (b << (8 * 1)) + (c << (8 * 0));

        /* outData each character takes up 6 bits */
        outData[j++] = g_base64Table[(byte >> (6 * 3)) & 0b00111111]; /* 3 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 2)) & 0b00111111]; /* 2 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 1)) & 0b00111111]; /* 1 and 6 is offset */
        outData[j++] = g_base64Table[(byte >> (6 * 0)) & 0b00111111]; /* 0 and 6 is offset */
    }

    const int32_t padding = srcDataSize % 3; /* 3 in each group */
    if (padding == 0) {
        return HKS_SUCCESS;
    } else {
        outData[outDataSize - 1] = '='; /* 1: padding last character with '=' */
    }
    if (padding == 1) {
        outData[outDataSize - 2] = '='; /* 2: padding penultimate character with '=' */
    }

    return HKS_SUCCESS;
}

static int32_t CheckAndCalculateSize(const uint32_t inSize, const uint32_t extraSize, uint32_t *outSize)
{
    /*
     * 2: fill it up to a multiple of three
     * 3 and 4: every three original characters is converted to four base64 characters
     */
    if (inSize > UINT32_MAX - 2) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    /* 3 and 4: every three original characters is converted to four base64 characters */
    if (((inSize + 2) / 3) > (UINT32_MAX / 4)) { // 2: fill it up to a multiple of three
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    /* 3 and 4: every three original characters is converted to four base64 characters */
    if ((((inSize + 2) / 3) * 4) > UINT32_MAX - extraSize) { // 2: fill it up to a multiple of three
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    /* 3 and 4: every three original characters is converted to four base64 characters */
    *outSize = (((inSize + 2) / 3) * 4) + extraSize; // 2: fill it up to a multiple of three
    return HKS_SUCCESS;
}

static int32_t EncodeCertChain(const struct HksBlob *inBlob, struct HksBlob *outBlob)
{
    const char begin[] = "-----BEGIN CERTIFICATE-----\n";
    const char end[] = "\n-----END CERTIFICATE-----";

    const uint32_t beginLen = strlen(begin);
    const uint32_t endLen = strlen(end);

    uint32_t tmpSize;
    int32_t ret = CheckAndCalculateSize(inBlob->size, beginLen + endLen, &tmpSize);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check inBlob size fail");
        return ret;
    }

    struct HksBlob tmpBlob = { tmpSize, NULL };
    tmpBlob.data = (uint8_t *)HksMalloc(tmpSize);
    if (tmpBlob.data == NULL) {
        HKS_LOG_E("malloc certEncoded fail");
        return HKS_ERROR_MALLOC_FAIL;
    }

    do {
        if (memcpy_s(tmpBlob.data, tmpSize, begin, beginLen) != EOK) {
            HKS_LOG_E("memcpy_s cert begin fail");
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }

        if (memcpy_s(tmpBlob.data + tmpSize - endLen, endLen, end, endLen) != EOK) {
            HKS_LOG_E("memcpy_s cert end fail");
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }

        ret = Base64Encode(inBlob->data, inBlob->size, tmpBlob.data + beginLen, tmpSize - beginLen - endLen);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Base64Encode fail");
            break;
        }

        if (memcpy_s(outBlob->data, outBlob->size, tmpBlob.data, tmpBlob.size) != EOK) {
            HKS_LOG_E("copy certs encoded fail");
            ret = HKS_ERROR_BAD_STATE;
            break;
        }
        outBlob->size = tmpBlob.size;
    } while (0);

    HksFree(tmpBlob.data);
    return ret;
}

int32_t HksCertificateChainUnpackFromService(const struct HksBlob *srcData, bool needEncode,
    struct HksCertChain *certChain)
{
    if (srcData->size < sizeof(certChain->certsCount)) {
        HKS_LOG_E("invalid certs buffer");
        return HKS_FAILURE;
    }
    uint32_t certsCount = *(uint32_t *)(srcData->data);
    if (certsCount > certChain->certsCount) {
        HKS_LOG_E("not enough output certs, real count %u, output count %u", certsCount, certChain->certsCount);
        return HKS_FAILURE;
    }
    uint32_t offset = sizeof(certsCount);

    struct HksBlob tmp = { 0, NULL };
    for (uint32_t i = 0; i < certsCount; ++i) {
        if (GetBlobFromBuffer(&tmp, srcData, &offset) != HKS_SUCCESS) {
            HKS_LOG_E("get certs %d fail", i);
            return HKS_FAILURE;
        }
        if (memcpy_s(certChain->certs[i].data, certChain->certs[i].size, tmp.data, tmp.size)) {
            HKS_LOG_E("copy certs %d fail", i);
            return HKS_FAILURE;
        }

        if (needEncode) {
            struct HksBlob inBlob = { tmp.size, certChain->certs[i].data };
            int32_t ret = EncodeCertChain(&inBlob, &certChain->certs[i]);
            if (ret != HKS_SUCCESS) {
                HKS_LOG_E("EncodeCertChain fail, ret = %d", ret);
                return HKS_FAILURE;
            }
        } else {
            certChain->certs[i].size = tmp.size;
        }
    }
    certChain->certsCount = certsCount;
    return HKS_SUCCESS;
}

static int32_t WrapUnwrapKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksBlob *targetKeyAlias, const struct HksParamSet *paramSet, uint32_t *offset)
{
    int32_t ret = CopyBlobToBuffer(keyAlias, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyAlias failed");
        return ret;
    }

    ret = CopyBlobToBuffer(targetKeyAlias, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy targetKeyAlias failed");
        return ret;
    }

    ret = CopyParamSetToBuffer(paramSet, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
    }
    return ret;
}

int32_t HksWrapKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *wrappedData)
{
    uint32_t offset = 0;
    int32_t ret = WrapUnwrapKeyPack(destData, keyAlias, targetKeyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("WrapUnwrapKeyPack failed");
        return ret;
    }
    return CopyUint32ToBuffer(wrappedData->size, destData, &offset);
}

int32_t HksUnwrapKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksBlob *targetKeyAlias, const struct HksBlob *wrappedData, const struct HksParamSet *paramSet)
{
    uint32_t offset = 0;
    int32_t ret = WrapUnwrapKeyPack(destData, keyAlias, targetKeyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("WrapUnwrapKeyPack failed");
        return ret;
    }

    return CopyBlobToBuffer(wrappedData, destData, &offset);
}

static int32_t SignVerifyWithDeviceKeyPack(struct HksBlob *destData, uint32_t keyId, const struct HksParamSet *paramSet,
    const struct HksBlob *unsignedData, uint32_t *offset)
{
    int32_t ret = CopyUint32ToBuffer(keyId, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy keyId failed");
        return ret;
    }

    ret = CopyParamSetToBuffer(paramSet, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy paramSet failed");
        return ret;
    }

    ret = CopyBlobToBuffer(unsignedData, destData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy unsignedData failed");
    }
    return ret;
}

int32_t HksSignWithDeviceKeyPack(struct HksBlob *destData, uint32_t keyId, const struct HksParamSet *paramSet,
    const struct HksBlob *unsignedData, const struct HksBlob *signature)
{
    uint32_t offset = 0;
    int32_t ret = SignVerifyWithDeviceKeyPack(destData, keyId, paramSet, unsignedData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerifyWithDeviceKeyPack failed");
        return ret;
    }
    return CopyUint32ToBuffer(signature->size, destData, &offset);
}

int32_t HksParamsToParamSet(const struct HksParam *params, uint32_t cnt, struct HksParamSet **outParamSet)
{
    struct HksParamSet *newParamSet = NULL;

    int32_t ret = HksInitParamSet(&newParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("init param set failed");
        return ret;
    }

    ret = HksAddParams(newParamSet, params, cnt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add in params failed");
        HksFreeParamSet(&newParamSet);
        return ret;
    }

    ret = HksBuildParamSet(&newParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build paramset failed!");
        HksFreeParamSet(&newParamSet);
        return ret;
    }

    *outParamSet = newParamSet;

    return ret;
}
