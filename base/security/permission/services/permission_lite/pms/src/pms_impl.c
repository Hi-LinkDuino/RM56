/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "pms.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cJSON.h"
#include "log.h"
#include "securec.h"

#include "hal_pms.h"
#include "perm_operate.h"

#define BUFF_PARAM_SIZE 16
#define BUFF_SIZE_MAX_LENGTH 1024
#define FIELD_PERMISSION "permissions"
#define FIELD_NAME "name"
#define FIELD_DESC "desc"
#define FIELD_IS_GRANTED "isGranted"
#define FIELD_FLAGS "flags"

// Permission matrix of run-time tasks
static struct TaskList g_taskList = {
    NULL
};

// don't forget free() afterwards
static char *ConcatString(const char *s1, const char *s2)
{
    unsigned int allocSize = strlen(s1) + strlen(s2) + 1;
    if (allocSize > BUFF_SIZE_MAX_LENGTH) {
        return NULL;
    }
    char *rst = (char *)HalMalloc(allocSize);
    if (rst == NULL) {
        return NULL;
    }
    if (memset_s(rst, allocSize, 0x0, allocSize) != EOK) {
        HalFree(rst);
        return NULL;
    }
    if (strcpy_s(rst, allocSize, s1) != EOK) {
        HalFree(rst);
        return NULL;
    }
    if (strcat_s(rst, allocSize, s2) != EOK) {
        HalFree(rst);
        return NULL;
    }
    return rst;
}

static int WriteString(const char *path, const char *string)
{
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return PERM_ERRORCODE_OPENFD_FAIL;
    }
    int stringLength = strlen(string);
    int writtenLength = write(fd, (void*)string, stringLength);
    close(fd);
    if (writtenLength != stringLength) {
        unlink(path);
        return PERM_ERRORCODE_WRITEFD_FAIL;
    }
    return PERM_ERRORCODE_SUCCESS;
}

// don't forget to call free() afterwards
static char *ReadString(const char *path, int *errCode)
{
    if (!HalIsValidPath(path)) {
        *errCode = PERM_ERRORCODE_PATH_INVALID;
        return NULL;
    }
    struct stat buf = { 0 };
    if (stat(path, &buf) || ((buf.st_mode & S_IFREG) != S_IFREG)) {
        *errCode = PERM_ERRORCODE_STAT_FAIL;
        return NULL;
    }
    unsigned int readSize = buf.st_size;
    char *rst = (char *)HalMalloc(readSize);
    if (rst == NULL) {
        *errCode = PERM_ERRORCODE_MALLOC_FAIL;
        return NULL;
    }

    if (memset_s(rst, readSize, 0x0, readSize) != EOK) {
        HalFree(rst);
        *errCode = PERM_ERRORCODE_MEMSET_FAIL;
        return NULL;
    }

    int fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        HalFree(rst);
        *errCode = PERM_ERRORCODE_OPENFD_FAIL;
        return NULL;
    }

    int ret = read(fd, rst, readSize);
    close(fd);
    if (ret < 0) {
        HalFree(rst);
        *errCode = PERM_ERRORCODE_READFD_FAIL;
        return NULL;
    }
    return rst;
}

static char *GetPath(const char *identifier)
{
    return ConcatString(HalGetPermissionPath(), identifier);
}

static int GetPermissionType(const char *permission)
{
    unsigned int permSize = 0;
    PermissionDef *permList = HalGetPermissionList(&permSize);

    for (int i = 0; i < permSize; i++) {
        if (strcmp(permission, permList[i].name) == 0) {
            return permList[i].type;
        }
    }
    return PERM_ERRORCODE_INVALID_PERMNAME;
}

static int ParseFixedPermissionsItem(const cJSON *object, PermissionSaved *perms)
{
    cJSON *itemName = cJSON_GetObjectItem(object, FIELD_NAME);
    cJSON *itemDesc = cJSON_GetObjectItem(object, FIELD_DESC);
    cJSON *itemGranted = cJSON_GetObjectItem(object, FIELD_IS_GRANTED);
    if (itemName == NULL || itemDesc == NULL || itemGranted == NULL ||
        !cJSON_IsString(itemName) || !cJSON_IsString(itemDesc)) {
        return PERM_ERRORCODE_JSONPARSE_FAIL;
    }
    if (strcpy_s(perms->name, PERM_NAME_LEN, itemName->valuestring) != EOK) {
        return PERM_ERRORCODE_COPY_ERROR;
    }
    if (strcpy_s(perms->desc, PERM_DESC_LEN, itemDesc->valuestring) != EOK) {
        return PERM_ERRORCODE_COPY_ERROR;
    }
    perms->granted = (enum IsGranted)itemGranted->valueint;
    return PERM_ERRORCODE_SUCCESS;
}

static int ParseNewPermissionsItem(const cJSON *object, PermissionSaved *perms)
{
    cJSON *itemFlags = cJSON_GetObjectItem(object, FIELD_FLAGS);
    if (itemFlags != NULL) {
        perms->flags = atoi(itemFlags->valuestring);
    } else {
        perms->flags = PMS_FLAG_DEFAULT;
    }
    return PERM_ERRORCODE_SUCCESS;
}


static int ParsePermissions(const char *jsonStr, PermissionSaved **perms, int *permNum)
{
    cJSON *root = cJSON_Parse(jsonStr);
    if (root == NULL) {
        return PERM_ERRORCODE_JSONPARSE_FAIL;
    }
    int ret;
    cJSON *array = cJSON_GetObjectItem(root, FIELD_PERMISSION);
    int pSize = cJSON_GetArraySize(array);
    int allocSize = sizeof(PermissionSaved) * pSize;
    if (allocSize == 0) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_SUCCESS;
    }
    *perms = (PermissionSaved *)HalMalloc(allocSize);
    if (*perms == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < pSize; i++) {
        cJSON *object = cJSON_GetArrayItem(array, i);
        ret = ParseFixedPermissionsItem(object, *perms + i);
        if (ret != PERM_ERRORCODE_SUCCESS) {
            cJSON_Delete(root);
            HalFree(*perms);
            *perms = NULL;
            return ret;
        }
        ret = ParseNewPermissionsItem(object, *perms + i);
        if (ret != PERM_ERRORCODE_SUCCESS) {
            cJSON_Delete(root);
            HalFree(*perms);
            *perms = NULL;
            return ret;
        }
    }
    *permNum = pSize;
    cJSON_Delete(root);
    return PERM_ERRORCODE_SUCCESS;
}

static int WritePermissions(const char *identifier, const cJSON *root)
{
    const char *path = GetPath(identifier);
    if (path == NULL) {
        return PERM_ERRORCODE_PATH_INVALID;
    }
    const char *jsonStr = cJSON_PrintUnformatted(root);
    if (jsonStr == NULL) {
        HalFree(path);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    int ret = WriteString(path, jsonStr);
    HalFree(path);
    cJSON_free(jsonStr);
    return ret;
}

static int SavePermissions(const char *identifier, const PermissionSaved *permissions, int permNum)
{
    if (identifier == NULL || permissions == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    char buf[BUFF_PARAM_SIZE] = {0};
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    cJSON *array = cJSON_CreateArray();
    if (array == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < permNum; i++) {
        cJSON *object = cJSON_CreateObject();
        if (object == NULL) {
            cJSON_Delete(array);
            cJSON_Delete(root);
            return PERM_ERRORCODE_MALLOC_FAIL;
        }
        cJSON_AddItemToObject(object, FIELD_NAME, cJSON_CreateString(permissions[i].name));
        cJSON_AddItemToObject(object, FIELD_DESC, cJSON_CreateString(permissions[i].desc));
        cJSON_AddItemToObject(object, FIELD_IS_GRANTED, cJSON_CreateBool(permissions[i].granted));

        if (memset_s(buf, BUFF_PARAM_SIZE, 0, BUFF_PARAM_SIZE) != EOK) {
            cJSON_Delete(array);
            cJSON_Delete(root);
            cJSON_Delete(object);
            return PERM_ERRORCODE_MEMSET_FAIL;
        }
        if (sprintf_s(buf, BUFF_PARAM_SIZE, "%d", permissions[i].flags) < 0) {
            cJSON_Delete(array);
            cJSON_Delete(root);
            cJSON_Delete(object);
            return PERM_ERRORCODE_SPRINTFS_FAIL;
        }
        cJSON_AddItemToObject(object, FIELD_FLAGS, cJSON_CreateString(buf));
        cJSON_AddItemToArray(array, object);
    }
    cJSON_AddItemToObject(root, FIELD_PERMISSION, array);
    int ret = WritePermissions(identifier, root);
    cJSON_Delete(root);
    return ret;
}

static bool IsValidFlags(const unsigned int flags)
{
    if ((flags == PMS_FLAG_DEFAULT) || ((flags & (~PMS_FLAG_VALID_MASK)) == PMS_FLAG_DEFAULT)) {
        return true;
    }
    return false;
}

char *QueryPermissionString(const char *identifier, int *errCode)
{
    if (identifier == NULL) {
        *errCode = PERM_ERRORCODE_INVALID_PARAMS;
        return NULL;
    }

    int ret;
    char *path = NULL;
    char *jsonStr = NULL;

    path = GetPath(identifier);
    if (path == NULL) {
        *errCode = PERM_ERRORCODE_MALLOC_FAIL;
        return NULL;
    }
    ret = HalAccess(path);
    if (ret) {
        HalFree(path);
        *errCode = PERM_ERRORCODE_FILE_NOT_EXIST;
        return NULL;
    }

    jsonStr = ReadString(path, errCode);
    HalFree(path);
    return jsonStr;
}

int QueryPermission(const char *identifier, PermissionSaved **permissions, int *permNum)
{
    int errCode = 0;
    const char *jsonStr = QueryPermissionString(identifier, &errCode);
    if (errCode) {
        return errCode;
    }

    int ret = ParsePermissions(jsonStr, permissions, permNum);
    HalFree(jsonStr);
    return ret;
}

int QueryAppCapabilities(const char *identifier, unsigned int **caps, unsigned int *capNum)
{
    if (caps == NULL || capNum == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    unsigned int allocSize = sizeof(unsigned int) * permNum;
    if (allocSize == 0) {
        HalFree(permissions);
        return PERM_ERRORCODE_SUCCESS;
    }
    unsigned int *capsBinded = (unsigned int *)HalMalloc(allocSize);
    if (capsBinded == NULL) {
        HalFree(permissions);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }

    unsigned int permSize = 0;
    PermissionDef *permList = HalGetPermissionList(&permSize);
    unsigned int index = 0;
    for (int j = 0; j < permNum; j++) {
        for (int i = 0; i < permSize; i++) {
            if (strcmp(permissions[j].name, permList[i].name) || permList[i].cap == CAP_NOT_BINDED) {
                continue;
            }
            capsBinded[index] = permList[i].cap;
            index++;
            break;
        }
    }
    HalFree(permissions);
    allocSize = sizeof(unsigned int) * index;
    if (allocSize == 0) {
        HalFree(capsBinded);
        return PERM_ERRORCODE_SUCCESS;
    }
    *caps = (unsigned int *)HalMalloc(allocSize);
    if (*caps == NULL) {
        HalFree(capsBinded);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int k = 0; k < index; k++) {
        *(*caps + k) = capsBinded[k];
    }
    *capNum = index;
    HalFree(capsBinded);
    return PERM_ERRORCODE_SUCCESS;
}

static int UpdateAppPermission(
    const char *identifier, const PermissionTrans newPerms[], int newPermNum, enum IsUpdate isUpdate)
{
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int retCode = PERM_ERRORCODE_SUCCESS;
    if (isUpdate == UPDATE) {
        retCode = QueryPermission(identifier, &permissions, &permNum);
    }
    if ((retCode != PERM_ERRORCODE_SUCCESS) && (retCode != PERM_ERRORCODE_FILE_NOT_EXIST)) {
        return retCode;
    }
    int allocSize = sizeof(PermissionSaved) * newPermNum;
    PermissionSaved *updatePerms = (PermissionSaved *)HalMalloc(allocSize);
    if (updatePerms == NULL) {
        HalFree(permissions);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < newPermNum; i++) {
        if (strlen(newPerms[i].name) > PERM_NAME_LEN - 1 || strlen(newPerms[i].desc) > PERM_DESC_LEN - 1) {
            HalFree(updatePerms);
            HalFree(permissions);
            return PERM_ERRORCODE_FIELD_TOO_LONG;
        }
        if (strcpy_s(updatePerms[i].name, PERM_NAME_LEN, newPerms[i].name) != EOK) {
            HalFree(updatePerms);
            HalFree(permissions);
            return PERM_ERRORCODE_COPY_ERROR;
        }
        if (strcpy_s(updatePerms[i].desc, PERM_DESC_LEN, newPerms[i].desc) != EOK) {
            HalFree(updatePerms);
            HalFree(permissions);
            return PERM_ERRORCODE_COPY_ERROR;
        }
        int permType = GetPermissionType(newPerms[i].name);
        updatePerms[i].granted = NOT_GRANTED;
        updatePerms[i].flags = PMS_FLAG_DEFAULT;
        if (permType != PERM_ERRORCODE_INVALID_PERMNAME) {
            updatePerms[i].granted = (permType == SYSTEM_GRANT) ? GRANTED : NOT_GRANTED;
        }
        for (int j = 0; j < permNum; j++) {
            if (strcmp(newPerms[i].name, (permissions + j)->name) == 0) {
                updatePerms[i].granted = permissions[j].granted;
            }
        }
    }
    retCode = SavePermissions(identifier, updatePerms, newPermNum);
    HalFree(updatePerms);
    HalFree(permissions);
    return retCode;
}

int SaveOrUpdatePermissions(
    const char *identifier, PermissionTrans permissions[], int permNum, enum IsUpdate isUpdate)
{
    if ((identifier == NULL) || (permissions == NULL) || (permNum < 0) ||
        (isUpdate != UPDATE && isUpdate != FIRST_INSTALL)) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    if (permNum > HalGetMaxPermissionSize()) {
        return PERM_ERRORCODE_TOO_MUCH_PERM;
    }
    char *path = GetPath(identifier);
    if (path == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    if (permNum == 0) {
        unlink(path);
        HalFree(path);
        return PERM_ERRORCODE_SUCCESS;
    }
    HalFree(path);

    int distinctNum = 0;
    int *flag = (int *)HalMalloc(permNum * sizeof(int));
    if (flag == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    (void)memset_s(flag, permNum * sizeof(int), 0, permNum * sizeof(int));
    for (int i = 0; i < permNum; i++) {
        if (flag[i] != 0) {
            continue;
        }
        permissions[distinctNum++] = permissions[i];
        for (int j = i + 1; j < permNum; j++) {
            if (strcmp(permissions[i].name, permissions[j].name) == 0) {
                flag[j] = 1;
            }
        }
    }
    HalFree(flag);

    return UpdateAppPermission(identifier, permissions, distinctNum, isUpdate);
}

int DeletePermissions(const char *identifier)
{
    if (identifier == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    int ret;
    char *path = NULL;
    path = GetPath(identifier);
    if (path == NULL) {
        return PERM_ERRORCODE_MALLOC_FAIL;
    }

    ret = HalAccess(path);
    if (ret != 0) {
        HalFree(path);
        return PERM_ERRORCODE_SUCCESS;
    }

    ret = unlink(path);
    HalFree(path);
    return (ret != 0) ? PERM_ERRORCODE_UNLINK_ERROR : PERM_ERRORCODE_SUCCESS;
}

int IsPermissionValid(const char *permissionName)
{
    unsigned int permSize = 0;
    PermissionDef *permList = HalGetPermissionList(&permSize);

    for (int i = 0; i < permSize; i++) {
        if (strcmp(permissionName, permList[i].name) == 0) {
            return PERM_ERRORCODE_SUCCESS;
        }
    }
    return PERM_ERRORCODE_INVALID_PERMNAME;
}

int IsPermissionRestricted(const char *permissionName)
{
    unsigned int permSize = 0;
    PermissionDef *permList = HalGetPermissionList(&permSize);

    for (int i = 0; i < permSize; i++) {
        if (strcmp(permissionName, permList[i].name) == 0) {
            return permList[i].isRestricted;
        }
    }
    return PERM_ERRORCODE_INVALID_PERMNAME;
}

int LoadPermissions(const char *identifier, int uid)
{
    if (uid < 0 || identifier == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    HalMutexLock();
    TNode *task = GetTaskWithUid(&g_taskList, uid);
    if (task != NULL) {
        HalMutexUnlock();
        return PERM_ERRORCODE_SUCCESS;
    }

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret == PERM_ERRORCODE_FILE_NOT_EXIST) {
        HILOG_ERROR(HILOG_MODULE_APP, "Perm file not exists.");
        HalMutexUnlock();
        return PERM_ERRORCODE_SUCCESS;
    }
    if (ret != PERM_ERRORCODE_SUCCESS) {
        HalMutexUnlock();
        return ret;
    }
    TNode *node = (TNode *)HalMalloc(sizeof(TNode));
    if (node == NULL) {
        HalFree(permissions);
        HalMutexUnlock();
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    node->uid = uid;
    if (strcpy_s(node->pkgName, PKG_NAME_LEN, identifier) != EOK) {
        HalFree(permissions);
        HalFree(node);
        HalMutexUnlock();
        return PERM_ERRORCODE_COPY_ERROR;
    }
    node->permList = permissions;
    node->permNum = permNum;
    node->next = NULL;

    AddTask(&g_taskList, node);
    HalMutexUnlock();

    return PERM_ERRORCODE_SUCCESS;
}

int UnLoadPermissions(int uid)
{
    HalMutexLock();
    DeleteTask(&g_taskList, uid);
    HalMutexUnlock();
    return PERM_ERRORCODE_SUCCESS;
}

int CheckPermissionStat(int uid, const char *permissionName)
{
    if (uid < 0 || permissionName == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    HalMutexLock();
    int ret = PermissionIsGranted(&g_taskList, uid, permissionName);
    HalMutexUnlock();
    return ret;
}

static int OnPermissionFileSync(const char *identifier, const char *permName, const enum IsGranted granted)
{
    int retCode = PERM_ERRORCODE_SUCCESS;
    bool isSave = false;
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    for (int i = 0; i < permNum; i++) {
        if (strcmp(permissions[i].name, permName) == 0) {
            isSave = permissions[i].granted ^ granted;
            permissions[i].granted = granted;
            break;
        }
    }
    if (isSave) {
        retCode = SavePermissions(identifier, permissions, permNum);
    }
    HalFree(permissions);
    return retCode;
}

static int OnPermissionFlagsFileSync(const char *identifier, const char *permName, const int flags)
{
    int retCode = PERM_ERRORCODE_SUCCESS;
    bool isSave = false;
    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = QueryPermission(identifier, &permissions, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        return ret;
    }
    for (int i = 0; i < permNum; i++) {
        if (strcmp(permissions[i].name, permName) == 0) {
            isSave = permissions[i].flags ^ flags;
            permissions[i].flags = flags;
            break;
        }
    }
    if (isSave) {
        retCode = SavePermissions(identifier, permissions, permNum);
    }
    HalFree(permissions);
    return retCode;
}

int GrantPermission(const char *identifier, const char *permName)
{
    if ((identifier == NULL) || (permName == NULL)) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    int ret = PERM_ERRORCODE_SUCCESS;
    HalMutexLock();
    TNode *node = GetTaskWithPkgName(&g_taskList, identifier);
    if (node != NULL) {
        ret = ModifyPermission(node, permName, GRANTED);
    }
    HalMutexUnlock();

    if (ret != 0) {
        return PERM_ERRORCODE_PERM_NOT_EXIST;
    }

    return OnPermissionFileSync(identifier, permName, GRANTED);
}

int RevokePermission(const char *identifier, const char *permName)
{
    if ((identifier == NULL) || (permName == NULL)) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    int ret = PERM_ERRORCODE_SUCCESS;
    HalMutexLock();
    TNode *node = GetTaskWithPkgName(&g_taskList, identifier);
    if (node != NULL) {
        ret = ModifyPermission(node, permName, NOT_GRANTED);
    }
    HalMutexUnlock();

    if (ret != 0) {
        return PERM_ERRORCODE_PERM_NOT_EXIST;
    }

    return OnPermissionFileSync(identifier, permName, NOT_GRANTED);
}

int GrantRuntimePermission(int uid, const char *permissionName)
{
    if (permissionName == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    HalMutexLock();
    TNode *node = GetTaskWithUid(&g_taskList, uid);
    if (node == NULL) {
        HalMutexUnlock();
        return PERM_ERRORCODE_TASKID_NOT_EXIST;
    }

    int ret = ModifyPermission(node, permissionName, GRANTED);
    HalMutexUnlock();
    if (ret < 0) {
        return PERM_ERRORCODE_PERM_NOT_EXIST;
    }

    return OnPermissionFileSync(node->pkgName, permissionName, GRANTED);
}

int RevokeRuntimePermission(int uid, const char *permissionName)
{
    if (permissionName == NULL) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    HalMutexLock();
    TNode *node = GetTaskWithUid(&g_taskList, uid);
    if (node == NULL) {
        HalMutexUnlock();
        return PERM_ERRORCODE_TASKID_NOT_EXIST;
    }

    int ret = ModifyPermission(node, permissionName, NOT_GRANTED);
    HalMutexUnlock();
    if (ret < 0) {
        return PERM_ERRORCODE_PERM_NOT_EXIST;
    }

    return OnPermissionFileSync(node->pkgName, permissionName, NOT_GRANTED);
}

int UpdatePermissionFlags(const char *identifier, const char *permissionName, const int flags)
{
    if ((identifier == NULL) || (permissionName == NULL) || !IsValidFlags(flags)) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }

    return OnPermissionFlagsFileSync(identifier, permissionName, flags);
}
