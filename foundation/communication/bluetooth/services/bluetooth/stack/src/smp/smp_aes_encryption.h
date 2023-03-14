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

#ifndef SMP_AES_ENCRYPTION_H
#define SMP_AES_ENCRYPTION_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE 16

/**
 * @brief aes 128 encrypt.
 *
 * @param key key data,must be 128bit.
 * @param keyLen input keyLen, must be 128bit.
 * @param in Plaintext,can be at most 16 bytes long.
 * @param inLen Plaintext's length in bytes.
 * @param out Encrypted data,must be 128bit.
 * @return Returns <b>0</b> if the operation is success.
 *         returns <b>-1</b> if the operation is failed.
 */
int SMP_Aes128(
    const uint8_t *key, const uint8_t keyLen, const uint8_t *in, const uint8_t inLen, uint8_t out[AES_BLOCK_SIZE]);

#ifdef __cplusplus
}
#endif

#endif