/*
* Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HAL_TOKEN_H
#define HAL_TOKEN_H

#include <stdio.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KIT_FRAMEWORK_UID_MAX 1000

/**
 * @brief Read token value from device.
 *
 * @param token the result token value, if read successfully.
 * @param len length of the token.
 * @returns 0 if success and get the update area token,
 *          1 if success and get the pre-made token,
 *         -1 if failed,
 *         -2 if no pre-made token.
 */
int HalReadToken(char *token, unsigned int len);

/**
 * @brief Write token value to device.
 *
 * @param token the token to write.
 * @param len length of the token.
 * @returns 0 if success, otherwise -1.
 */
int HalWriteToken(const char *token, unsigned int len);

/**
 * @brief Get AcKey value from device.
 *
 * @param acKey the result acKey, if get successfully.
 * @param len length of the acKey.
 * @returns 0 if success, otherwise -1.
 */
int HalGetAcKey(char *acKey, unsigned int len);

/**
 * @brief Get ProdId value from device.
 *
 * @param productId product IDs to be populated with.
 * @param len length of the productId.
 * @returns 0 if success, otherwise -1.
 */
int HalGetProdId(char *productId, unsigned int len);

/**
 * @brief Get ProdKey value from device.
 *
 * @param productKey The productKey value
 * @param len The productKey len.
 * @returns 0 if success, otherwise -1.
 */
int HalGetProdKey(char *productKey, unsigned int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // HAL_TOKEN_H
