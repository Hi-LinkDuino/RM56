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

/**
 * @addtogroup OTA
 * @{
 *
 * @brief Provides system upgrades.
 *
 * @since 1.0
 * @version 1.0
 */

 /**
 * @file hota_updater.h
 *
 * @brief Defines the functions for system upgrades.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OHOS_LITE_HOTA_UPDATER_H
#define OHOS_LITE_HOTA_UPDATER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define USE_DEFAULT_PKG                  1
#define NOT_USE_DEFAULT_PKG              0

typedef enum {
    HOTA_NOT_INIT = 0X01,
    HOTA_INITED = 0x02,
    HOTA_TRANSPORT_INFO_DONE = 0x03,
    HOTA_TRANSPORT_ALL_DONE = 0x04,
    HOTA_FAILED = 0x05,
    HOTA_CANCELED = 0x06
} HotaStatus;

typedef enum {
    HOTA_DATA_COMMON_ERR = 0,
    HOTA_DATA_WRITE_ERR = 1,
    HOTA_VERSION_INVALID = 2,
    HOTA_DATA_SIGN_CHECK_ERR = 3,
    HOTA_DATA_VERIFY_HASH_ERR = 4,
    HOTA_DATA_COPY_TO_BUFFER_ERR = 5
} HotaErrorCode;

typedef void (*ErrorCallBackFunc)(HotaErrorCode errorCode);
typedef void (*StatusCallBackFunc)(HotaStatus status);

/**
 * @brief Sets the switch for using the default upgrade package format.
 *
 * You can call this function to choose the default upgrade package format when developing the system upgrade
 * capability. \n
 * If the default upgrade package format is used, the system ensures the security and integrity of the
 * upgrade package. \n
 * If it is not used, you need to ensure the security and integrity of the upgrade package you have chosen.
 * To be specific, you need to call the {@link HotaRead} function to read the data that has been written into
 * flash memory, and then verify the data. \n
 *
 * @param flag Specifies whether to use the default system upgrade package format. The value <b>1</b> (default value)
 * means to use it, and <b>0</b> means not to use it.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaSetPackageType(unsigned int flag);

/**
 * @brief Obtains the index of the A/B partition to be upgraded.
 *
 * In the A/B upgrade scenario, you can use this function to determine whether partition A or B will be upgraded. \n
 *
 * @param index Indicates the index of a partition. The value <b>1</b> means partition A, and <b>2</b>
 * means partition B.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaGetUpdateIndex(unsigned int *index);

/**
 * @brief Initializes the OTA module.
 *
 * @param errorCallback Indicates the callback invoked when an error occurs during the upgrade.
 * This parameter can be null.
 * @param statusCallback Indicates the callback invoked when the upgrade status changes. This parameter can be null.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaInit(ErrorCallBackFunc errorCallback, StatusCallBackFunc statusCallback);

/**
 * @brief Writes specified data into flash memory.
 *
 * @param buffer Indicates the data to be written into flash memory.
 * @param offset Indicates the offset from where to start writing data.
 * @param buffSize Indicates the size of the data to be written.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaWrite(unsigned char *buffer, unsigned int offset, unsigned int buffSize);

/**
 * @brief Reads the data that has been written into flash memory.
 *
 * This function is required for verifying data correctness when the default upgrade package format is not used. \n
 * It is not required when the default upgrade package format is used. \n
 *
 * @param offset Indicates the offset from where to start reading data.
 * @param bufLen Indicates the length of the data to be read.
 * @param buf Indicates the buffer to store the data that has been read.
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaRead(unsigned int offset, unsigned int bufLen, unsigned char *buf);

/**
 * @brief Cancels an upgrade.
 *
 * If an upgrade fails or is interrupted, you can use this function to cancel it. \n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaCancel(void);

/**
 * @brief Sets the system state to be upgrading after data has been written into flash memory by {@link HotaWrite}.
 *
 * After this operation is successful, you need to call the {@link HotaRestart} function to complete the upgrade. \n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaSetBootSettings(void);

/**
 * @brief Restarts the system after an upgrade.
 *
 * You need to call this function after you have called the {@link HotaSetBootSettings} function. \n
 *
 * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaRestart(void);

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
int HotaGetUpdateAbility(void);

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
int HotaGetOtaPkgPath(char *path, int len);

/**
 * @brief judge deivce can auto reboot.
 *
 * You need to call this function when update process init.\n
 *
 * @return Returns <b>1</b> if device can auto update; returns <b>0</b> if device can not auto update.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaIsDeviceCanReboot(void);

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
int HotaIsDevelopMode(void);

/**
 * @brief get update status.
 *
 * You need to call this function when update process .\n
 *
 * @return Returns UpdateStatus if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaGetUpdateStatus(void);

/**
 * @brief reboot and clean userdata.
 *
 * You need to call this function when update process .\n
 *
 * @return Returns UpdateStatus if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaRebootAndCleanUserData(void);

/**
 * @brief reboot and clean cache.
 *
 * You need to call this function when update process .\n
 *
 * @return Returns UpdateStatus if the operation is successful; returns <b>-1</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int HotaRebootAndCleanCache(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OHOS_LITE_HOTA_UPDATER_H */
/** @} */
