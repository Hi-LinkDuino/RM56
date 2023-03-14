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
#include "smp_aes_encryption.h"

#include <string.h>

#include "log.h"
#include "openssl/aes.h"

#include "smp.h"

static void SMP_ReverseData(const uint8_t *intput, uint8_t *output, int size)
{
    for (int i = 0x00; i < size; i++) {
        output[i] = intput[size - 0x01 - i];
    }
}

static int SMP_Aes128Internal(
    const uint8_t key[AES_BLOCK_SIZE], const uint8_t in[AES_BLOCK_SIZE], uint8_t out[AES_BLOCK_SIZE])
{
    if ((in == NULL) || (key == NULL) || (out == NULL)) {
        return -1;
    }

    uint8_t inReverse[AES_BLOCK_SIZE];
    uint8_t keyReverse[AES_BLOCK_SIZE];
    uint8_t outReverse[AES_BLOCK_SIZE];
    AES_KEY aesKey;

    SMP_ReverseData(key, keyReverse, sizeof(keyReverse));
    SMP_ReverseData(in, inReverse, sizeof(inReverse));

    AES_set_encrypt_key(keyReverse, 0x80, &aesKey);
    AES_encrypt(inReverse, outReverse, &aesKey);

    SMP_ReverseData(outReverse, out, sizeof(outReverse));
    (void)memset_s(keyReverse, sizeof(keyReverse), 0x00, sizeof(keyReverse));

    return 0;
}

int SMP_Aes128(
    const uint8_t *key, const uint8_t keyLen, const uint8_t *in, const uint8_t inLen, uint8_t out[AES_BLOCK_SIZE])
{
    if ((key == NULL) || (in == NULL) || (out == NULL)) {
        return -1;
    }

    uint8_t input[AES_BLOCK_SIZE] = {0x00};
    if (inLen > AES_BLOCK_SIZE) {
        LOG_WARN("aes_128 inLen longer than AES_BLOCK_SIZE(16).");
        (void)memcpy_s(input, AES_BLOCK_SIZE, in, AES_BLOCK_SIZE);
    } else {
        (void)memcpy_s(input, AES_BLOCK_SIZE, in, inLen);
    }

    uint8_t keyInput[AES_BLOCK_SIZE] = {0x00};
    if (keyLen > AES_BLOCK_SIZE) {
        LOG_WARN("aes_128 keyLen longer than AES_BLOCK_SIZE(16).");
        (void)memcpy_s(keyInput, AES_BLOCK_SIZE, key, AES_BLOCK_SIZE);
    } else {
        (void)memcpy_s(keyInput, AES_BLOCK_SIZE, key, keyLen);
    }

    return SMP_Aes128Internal(&keyInput[0], &input[0], &out[0]);
}