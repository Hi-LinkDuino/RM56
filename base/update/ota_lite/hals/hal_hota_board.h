/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_LITE_HOTA_HAL_BOARD_H
#define OHOS_LITE_HOTA_HAL_BOARD_H

#include "hota_partition.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**
 * @brief OTA module initialization.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalInit(void);

/**
 * @brief Release OTA module resource.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalDeInit(void);

/**
 * @brief Release OTA module resource.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalGetUpdateIndex(unsigned int *index);

/**
 * @brief Write image to partition.
 *
 * @param partition [in] scan result, result array size must larger than WIFI_SCAN_AP_LIMIT.
 * @param buffer    [in] image buffer.
 * @param offset    [in] The buffer offset of file.
 * @param bufLen    [in] The Length of buffer.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalWrite(int partition, unsigned char *buffer, unsigned int offset, unsigned int bufLen);

/**
 * @brief read image of partition.
 *
 * @param partition [in]  scan result, result array size must larger than WIFI_SCAN_AP_LIMIT.
 * @param offset    [in]  The buffer offset of file.
 * @param bufLen    [in]  The Length of buffer.
 * @param buffer    [out] image buffer.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRead(int partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer);

/**
 * @brief Write Boot Settings in order to notify device upgrade success or enter Recovery Part.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalSetBootSettings(void);

/**
 * @brief Restart after upgrade finish or go bootloader to upgrade.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRestart(void);

/**
 * @brief Rollback if ota failed.
 *
 * @return OHOS_SUCCESS: Success,
 *         Others: Failure.
 */
int HotaHalRollback(void);

/**
 * @brief Get partition info.
 *
 * You need to call this funtion in Init function, you need partition info when upgrade. \n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
const ComponentTableInfo *HotaHalGetPartitionInfo();

/**
 * @brief Get public key.
 *
 * You need to call this funtion when verfiy sign data \n
 *
 * @param length Indicates  pubkey len.
 *
 * @return Returns <b>0</b> if the operation is successful; public key.
 *
 * @since 1.0
 * @version 1.0
 */
unsigned char *HotaHalGetPubKey(unsigned int *length);

/**
 * @brief get update ability.
 *
 * You need to call this function when update process init. \n
 *
 * @return Returns update abilty.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalGetUpdateAbility(void);

/**
 * @brief get ota package update path.
 *
 * You need to call this function before update process. \n
 * 
 * @param path Indicates where ota package you place.
 * @param len Indicates  path len.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalGetOtaPkgPath(char *path, int len);

/**
 * @brief judge deivce can auto update.
 *
 * You need to call this function when update process init.\n
 *
 * @return Returns <b>1</b> if device can auto update; returns <b>0</b> if device can not auto update.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalIsDeviceCanReboot(void);

/**
 * @brief judge is develop mode now.
 *
 * You need to call this function to get develop mode.\n
 *
 * @return Returns <b>1</b> if device is develop mode; returns <b>0</b> if device is not develop mode.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalIsDevelopMode(void);

/**
 * @brief get update metadata.
 *
 * You need to call this function when update process .\n
 *
 * @return Returns OtaStatus if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalGetMetaData(UpdateMetaData *metaData);

/**
 * @brief set update metadata.
 *
 * You need to call this function when update process.\n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalSetMetaData(UpdateMetaData *metaData);

/**
 * @brief reboot and clean userdata.
 *
 * You need to call this function when recovery system.\n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalRebootAndCleanUserData(void);

/**
 * @brief reboot and clean cache.
 *
 * You need to call this function when recovery system.\n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalRebootAndCleanCache(void);

/**
 * @brief check whether pkgVersion is valid.
 *
 * You need to call this function before update process.\n
 *
 * @return Returns <b>1</b> if pkgVersion is valid compared to currentVersion; returns <b>0</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaHalCheckVersionValid(const char *currentVersion, const char *pkgVersion, unsigned int pkgVersionLength);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_HAL_BOARD_H */
