/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hota_verify.h"

#include <securec.h>
#include "app_rsa.h"
#include "app_sha256.h"
#include "hal_hota_board.h"


static uint8 g_hash[HASH_LENGTH] = {0};

static AppSha256Context g_sha256 = { 0 }; /* hash tmp var */

void HotaHashInit(void)
{
    AppSha256Init(&g_sha256);
    // Init the hash value of data
    if (memset_s(g_hash, HASH_LENGTH, 0, HASH_LENGTH) != EOK) {
        return;
    }

    return;
}

void HotaHashCalc(const uint8 *buffer, uint32 length)
{
    if (buffer == NULL) {
        printf("HashCalc param in null!\r\n");
        return;
    }

    AppSha256Update(&g_sha256, buffer, length);
    return;
}

int32 HotaGetHash(uint8 *buffer, uint32 length)
{
    if (buffer == NULL) {
        printf("GetHash param in null!\r\n");
        return OHOS_FAILURE;
    }

    if (memset_s(buffer, length, 0, length) != EOK) {
        return OHOS_FAILURE;
    }

    if (length >= HASH_LENGTH) {
        // Calc the last hash, and save it
        AppSha256Finish(&g_sha256, g_hash);
        if (memcpy_s(buffer, HASH_LENGTH, g_hash, HASH_LENGTH) != EOK) {
            return OHOS_FAILURE;
        }
        return OHOS_SUCCESS;
    } else {
        printf("GetHash len illegal!\r\n");
        return OHOS_FAILURE;
    }
}

static int32 HotaCalcImageHash(uint8 *dataAddr, uint32 dataLen, uint8 *hash, uint32 hashLen)
{
    AppSha256Context sha256;
    uint32 count;

    if ((dataAddr == NULL) || (hash == NULL) || (dataLen == 0) || (hashLen < HASH_LENGTH)) {
        printf("HotaCalcImageHash param is illegal.\r\n");
        return OHOS_FAILURE;
    }

    uint8 *databuf = dataAddr;
    if (memset_s(hash, hashLen, 0, hashLen) != EOK) {
        printf("HotaCalcImageHash memset_s hash failed.\r\n");
        return OHOS_FAILURE;
    }
    AppSha256Init(&sha256);

    while (dataLen > 0) {
        count = (dataLen > BUFFR_LENGTH) ? BUFFR_LENGTH : dataLen;
        AppSha256Update(&sha256, databuf, count);
        dataLen -= count;
        databuf += count;
    }
    // Calc Final hash
    AppSha256Finish(&sha256, hash);

    return OHOS_SUCCESS;
}

static int32 HotaSignVerifyByHash(const uint8 *hash, uint32 hashLen, const uint8 *imageSign, uint32 signLen)
{
    AppRsaContext rsa;
    uint32 length = 0;
    if ((hash == NULL) || (imageSign == NULL)) {
        return OHOS_FAILURE;
    }

    AppRsaInit(&rsa);
    uint8 *keyBuf = HotaGetPubKey(&length);
    if (keyBuf == NULL) {
        return OHOS_FAILURE;
    }

    int ret = AppRsaDecodePublicKey(&rsa, keyBuf, length);
    if (ret != 0) {
        return OHOS_FAILURE;
    }

    ret = AppVerifyData(&rsa, hash, hashLen, imageSign, signLen);
    AppRsaFree(&rsa);
    if (ret < 0) {
        printf("App verify failed.\r\n");
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int32 HotaSignVerify(uint8 *image, uint32 imgLen, uint8 *imageSign, uint32 signLen)
{
    if ((image == NULL) || (imageSign == NULL)) {
        printf("SignVerify param in illegal, len %u!\r\n", signLen);
        return OHOS_FAILURE;
    }

    uint8 imageHashLocal[HASH_LENGTH] = {0};
    if (HotaCalcImageHash(image, imgLen, imageHashLocal, HASH_LENGTH) != OHOS_SUCCESS) {
        printf("HotaCalcImageHash fail!\r\n");
        return OHOS_FAILURE;
    }

    // Use local hash of data to verify the sign
    int ret = HotaSignVerifyByHash(imageHashLocal, HASH_LENGTH, imageSign, signLen);
    if (ret != OHOS_SUCCESS) {
        printf("SignVerify fail!\r\n");
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

uint8 *HotaGetPubKey(uint32 *length)
{
    return HotaHalGetPubKey(length);
}
