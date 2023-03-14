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

/**
 * @addtogroup pms_types
 * @{
 *
 * @brief Defines variables and data structures used by the permission management module.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file pms_types.h
 *
 * @brief Declares variables and data structures used by the permission management module.
 *
 * To use permission management APIs, you must obtain this file.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef INTERFACES_KITS_PERMISSION_LITE_PMS_TYPES_H
#define INTERFACES_KITS_PERMISSION_LITE_PMS_TYPES_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Indicates the length of a permission name.
 *
 * The default length is 64 bytes, including the terminating null byte <b>'\0'</b>.
 */
#define PERM_NAME_LEN 64
/**
 * @brief Indicates the length of a permission description.
 *
 * The default length is 128 bytes, including the terminating null byte <b>'\0'</b>.
 */
#define PERM_DESC_LEN 128
/**
 * @brief Indicates the length of udid.
 *
 * The default length is 64 bytes, do not contain the terminating null byte <b>'\0'</b>.
 */
#define UDID_FINAL_BYTES 64

/**
 * @brief Enumerates granting statuses of the permission.
 *
 */
enum IsGranted {
    /** Not granted */
    NOT_GRANTED = 0,
    /** Granted */
    GRANTED = 1,
};

/**
 * @brief Defines the permission flag bits.
 *
 */
enum PmsFlagDef {
    /**
    * Permission flag: The default value of permission flag.
    */
    PMS_FLAG_DEFAULT = 0,

    /**
    * Permission flag[bit1]: The permission is set in its current state by the user and it is fixed,
    * i.e. apps can no longer request this permission.
    */
    PMS_FLAG_USER_FIXED = 1 << 1,

    /**
    * Permission flag mask: Indicates the valid flag definition.
    */ 
    PMS_FLAG_VALID_MASK = 0x2,
};

/**
 * @brief Defines the permission, including the name, description, and whether the permission is granted.
 *
 */
typedef struct {
    /** Permission name. For details about its length, see {@link PERM_NAME_LEN}. */
    char name[PERM_NAME_LEN];
    /** Permission description. For details about its length, see {@link PERM_DESC_LEN}. */
    char desc[PERM_DESC_LEN];
    /** Whether the permission is granted */
    enum IsGranted granted;
    /** Permission flags */
    int flags;
} PermissionSaved;

/**
 * @brief Enumerates error codes of the permission management module.
 *
 */
enum PmsErrorCode {
    /** Success */
    PERM_ERRORCODE_SUCCESS = 0,
    /** Invalid parameters */
    PERM_ERRORCODE_INVALID_PARAMS = 10,
    /** Invalid permission name */
    PERM_ERRORCODE_INVALID_PERMNAME,
    /** Failed to allocate memory using the <b>malloc</b> function. */
    PERM_ERRORCODE_MALLOC_FAIL,
    /** Failed to open the file descriptor. */
    PERM_ERRORCODE_OPENFD_FAIL,
    /** Failed to read the file descriptor. */
    PERM_ERRORCODE_READFD_FAIL,
    /** Failed to write the file descriptor. */
    PERM_ERRORCODE_WRITEFD_FAIL,
    /** Failed to parse the JSON string. */
    PERM_ERRORCODE_JSONPARSE_FAIL,
    /** Failed to copy the string. */
    PERM_ERRORCODE_COPY_ERROR,
    /** The permission name or description is too long. */
    PERM_ERRORCODE_FIELD_TOO_LONG,
    /** The permission does not exist. */
    PERM_ERRORCODE_PERM_NOT_EXIST,
    /** Failed to delete the permission file. */
    PERM_ERRORCODE_UNLINK_ERROR,
    /** The file does not exist. */
    PERM_ERRORCODE_FILE_NOT_EXIST,
    /** Failed to set memory using the <b>memset</b> function. */
    PERM_ERRORCODE_MEMSET_FAIL,
    /** Failed to obtain information about the named file using the <b>stat</b>
     * function.
     */
    PERM_ERRORCODE_STAT_FAIL,
    /** Invalid path */
    PERM_ERRORCODE_PATH_INVALID,
    /** Too many permissions */
    PERM_ERRORCODE_TOO_MUCH_PERM,
    /** The process ID does not exist. */
    PERM_ERRORCODE_TASKID_NOT_EXIST,
    /** Abnormal number of permissions */
    PERM_ERRORCODE_PERM_NUM_ERROR,
    /** Failed to generate udid */
    PERM_ERRORCODE_GENERATE_UDID_FAILED,
    /** Failed to format data using the <b>sprintf_s</b> function. */
    PERM_ERRORCODE_SPRINTFS_FAIL,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // INTERFACES_KITS_PERMISSION_LITE_PMS_TYPES_H
