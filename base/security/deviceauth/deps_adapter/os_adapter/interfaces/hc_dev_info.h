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

#ifndef HC_DEV_INFO_H
#define HC_DEV_INFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_UDID_LEN 65
#define MAX_INPUT_UDID_LEN 200
#define MAX_SESSION_COUNT 30

/*
 * Get the unique device ID of the device(UDID).
 * udid: It is used to store the UDID finally obtained.
 * udidLen: The size of array to store UDID. Its value is the real udid size + 1 (including '\0').
 *          In liteos and linux, the udidLen must be 65.
 * Return 0 if the UDID is successfully obtained, other returns an error code.
 */
int32_t HcGetUdid(uint8_t *udid, int32_t udidLen);

const char *GetStoragePath(void);
const char *GetStorageDirPath(void);

#ifdef __cplusplus
}
#endif
#endif