/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "bundle_util.h"

#include <new>

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "bundle_daemon_client.h"
#else
#include "adapter.h"
#include "bundlems_log.h"
#include "cstdio"
#include "dirent.h"
#include "fcntl.h"
#include "los_tick.h"
#include "sys/stat.h"
#include "unistd.h"
#endif
#include <cstdlib>
#include "utils.h"

namespace OHOS {
const char DIGITSTR[] = "0123456789";
const uint8_t MAX_CHARACTER_VALUE = 26;
const uint8_t NUM_OF_TYPE = 3;
#ifdef __LITEOS_M__
const int32_t MAX_JSON_SIZE = 1024 * 64;
#else
const uint32_t MAX_JSON_SIZE = 1024 * 64;
#endif

/*
 * path should not include ".." or "./" or ".\0"
 */
bool BundleUtil::CheckRealPath(const char *path)
{
    if (path == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] CheckRealPath path null");
#endif
        return false;
    }
    if (strlen(path) > PATH_LENGTH) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] CheckRealPath path too long:%d", strlen(path));
#endif
        return false;
    }
    char *next = const_cast<char *>(path);
    for (; *next != '\0'; next++) {
        if (*next != '.') {
            continue;
        }
        next++;
        if (*next == '\0' || *next == '.' || *next == '/') {
#ifdef APP_PLATFORM_WATCHGT
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] CheckRealPath path fail:%d", *next);
#endif
            return false;
        }
    }
    return true;
}

bool BundleUtil::IsFile(const char *path)
{
    if (!CheckRealPath(path)) {
        return false;
    }

    struct stat buf = { 0 };
    int32_t ret = stat(path, &buf);
    if (ret != 0) {
        return false;
    }

    int32_t fp = open(path, O_RDONLY, S_IREAD | S_IWRITE);
    if (fp >= 0) {
        close(fp);
        return true;
    }
    return false;
}

bool BundleUtil::IsDir(const char *path)
{
    if (path == nullptr) {
        return false;
    }

    struct stat buf = { 0 };
    int32_t ret = stat(path, &buf);
    if (ret != 0) {
        return false;
    }

    if ((buf.st_mode & S_IFDIR) == S_IFDIR) {
        return true;
    }
    return false;
}

bool BundleUtil::EndWith(const char *str, const char *subStr)
{
    if (str == nullptr || subStr == nullptr) {
        return false;
    }

    int32_t strLen = strlen(str);
    int32_t subStrLen = strlen(subStr);
    if (strLen == 0 || subStrLen == 0 || strLen < subStrLen) {
        return false;
    }

    while (subStrLen >= 1) {
        if (str[strLen - 1] != subStr[subStrLen - 1]) {
            return false;
        }
        strLen--;
        subStrLen--;
    }
    return true;
}

bool BundleUtil::StartWith(const char *str, const char *subStr)
{
    if (str == nullptr || subStr == nullptr) {
        return false;
    }

    int32_t strLen = strlen(str);
    int32_t subStrLen = strlen(subStr);
    if (strLen == 0 || subStrLen == 0 || strLen < subStrLen) {
        return false;
    }

    int32_t index = 0;
    while (index < subStrLen) {
        if (str[index] != subStr[index]) {
            return false;
        }
        index++;
    }
    return true;
}

bool BundleUtil::IsDigitStr(const char *str)
{
    if (str == nullptr) {
        return false;
    }
    return strspn(str, DIGITSTR) == strlen(str);
}

uint32_t BundleUtil::GetFileSize(const char *filePath)
{
    if (!CheckRealPath(filePath)) {
        return 0;
    }

    struct stat fileInfo;
    int32_t ret = stat(filePath, &fileInfo);
    if (ret != 0) {
        return 0;
    }
    return fileInfo.st_size;
}

uint32_t BundleUtil::GetFileFolderSize(const char *filePath)
{
    if (!CheckRealPath(filePath)) {
        return 0;
    }
    List<char *>* list = new (std::nothrow)List<char *>();
    if (list == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetFolderSize failed, list is null");
#endif
        return 0;
    }

    list->PushFront(Utils::Strdup(filePath));
    uint32_t fileFolderSize = 0;
    while (!list->IsEmpty()) {
        char *curPath = list->Front();
        if (curPath == nullptr) {
            break;
        }
        fileFolderSize += GetCurrentFolderSize(curPath, list);
        list->PopFront();
        AdapterFree(curPath);
    }

    if (!list->IsEmpty()) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] After get folder size, list is still not empty");
#endif
        for (auto node = list->Begin(); node != list->End(); node = node->next_) {
            AdapterFree(node->value_);
        }
    }
    delete list;
    return fileFolderSize;
}

uint32_t BundleUtil::GetCurrentFolderSize(const char *dirPath, List<char *>* list)
{
    DIR *dir = nullptr;
    if ((dir = opendir(dirPath)) == nullptr) {
        return 0;
    }
    dirent *dp = nullptr;
    char filePath[PATH_LENGTH] = { 0 };
    struct stat buf = { 0 };
    uint32_t fileSize = 0;
    while ((dp = readdir(dir)) != nullptr) {
        if ((strcmp(dp->d_name, ".") == 0) || (strcmp(dp->d_name, "..")) == 0) {
            continue;
        }

        if (memset_s(filePath, PATH_LENGTH, 0, PATH_LENGTH) != EOK) {
            continue;
        }

        if (sprintf_s(filePath, PATH_LENGTH, "%s/%s", dirPath, dp->d_name) < 0) {
            continue;
        }

        if (!IsDir(filePath)) {
            if (stat(filePath, &buf) != 0 || buf.st_size <= 0) {
                fileSize = 0;
                break;
            }
            fileSize += buf.st_size;
        } else {
            list->PushBack(Utils::Strdup(filePath));
        }
    }
    closedir(dir);
    return fileSize;
}

void BundleUtil::DeleteJsonFile(const char *bundleName, const char *randStr)
{
    if (bundleName == nullptr || randStr == nullptr) {
        return;
    }
    char *bundleTmpJsonPathComp[] = {
        const_cast<char *>(JSON_PATH), const_cast<char *>(bundleName), const_cast<char *>(randStr),
        const_cast<char *>(JSON_SUFFIX)
    };

    char *bundleTmpJsonPath = Strscat(bundleTmpJsonPathComp, sizeof(bundleTmpJsonPathComp) / sizeof(char *));
    if (bundleTmpJsonPath == nullptr) {
        return;
    }

    if (IsFile(bundleTmpJsonPath)) {
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
        BundleDaemonClient::GetInstance().RemoveFile(bundleTmpJsonPath);
#else
        (void)unlink(bundleTmpJsonPath);
#endif
        AdapterFree(bundleTmpJsonPath);
        return;
    }
    AdapterFree(bundleTmpJsonPath);

    char *bundleJsonPathComp[] = {
        const_cast<char *>(JSON_PATH), const_cast<char *>(bundleName), const_cast<char *>(JSON_SUFFIX)
    };
    char *bundleJsonPath = Strscat(bundleJsonPathComp, sizeof(bundleJsonPathComp) / sizeof(char *));
    if (bundleJsonPath == nullptr) {
        return;
    }
    if (IsFile(bundleJsonPath)) {
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
        BundleDaemonClient::GetInstance().RemoveFile(bundleJsonPath);
#else
        (void)unlink(bundleJsonPath);
#endif
    }
    AdapterFree(bundleJsonPath);
}

char *BundleUtil::Strscat(char *str[], uint32_t len)
{
    int32_t strSize = 0;
    for (uint32_t i = 0; i < len; i++) {
        if (str[i] == nullptr) {
            return nullptr;
        }
        strSize += strlen(str[i]);
    }

    char *outStr = reinterpret_cast<char *>(AdapterMalloc((strSize + 1) * sizeof(char)));
    if (outStr == nullptr) {
        return nullptr;
    }

    char *pos = outStr;
    int32_t count = 0;
    for (uint32_t i = 0; i < len; i++) {
        int32_t size = strlen(str[i]);
        if (memcpy_s(pos, strSize + 1 - count, str[i], size) != EOK) {
            AdapterFree(outStr);
            return nullptr;
        }
        count += size;
        pos += size;
    }
    *pos = '\0';
    return outStr;
}

cJSON *BundleUtil::GetJsonStream(const char *path)
{
    struct stat fileInfo;
    int32_t size = 0;

    if (!IsFile(path)) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream IsFile fail");
#endif
        return nullptr;
    }

    if (stat(path, &fileInfo) != 0 || (size = fileInfo.st_size) <= 0) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream path fail");
#endif
        return nullptr;
    }

    if (size > MAX_JSON_SIZE) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream size fail:%d", size);
#endif
        return nullptr;
    }

    int32_t fp = open(path, O_RDONLY, S_IREAD | S_IWRITE);
    if (fp < 0) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream open fail:%d", fp);
#endif
        return nullptr;
    }

    char *json = reinterpret_cast<char *>(AdapterMalloc(size));
    if (json == nullptr) {
        close(fp);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream AdapterMalloc fail");
#endif
        return nullptr;
    }

    if (read(fp, json, size) != size) {
        AdapterFree(json);
        close(fp);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] GetJsonStream read fail");
#endif
        return nullptr;
    }
    close(fp);

    cJSON *root = cJSON_Parse(json);
    AdapterFree(json);
    json = nullptr;
    if (root == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] cJSON_Parse fail");
#endif
    }
    return root;
}

bool BundleUtil::CheckBundleJsonIsValid(const char *bundleName, char **codePath, char **appId, int32_t &versionCode)
{
    if (bundleName == nullptr || codePath == nullptr || appId == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] CheckBundleJsonIsValid parameters are not valid");
#endif
        return false;
    }

    char bundleJsonPath[PATH_LENGTH] = { 0 };
    if (sprintf_s(bundleJsonPath, PATH_LENGTH, "%s%s%s", JSON_PATH, bundleName, JSON_SUFFIX) < 0) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] failed to organize bundleJson data file path");
#endif
        return false;
    }

    cJSON *object = GetJsonStream(bundleJsonPath);
    if (object == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] failed to open json file");
#endif
        return false;
    }

    cJSON *item = cJSON_GetObjectItem(object, JSON_SUB_KEY_CODEPATH);
    if (!cJSON_IsString(item)) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] result of parsing codePath is not a string");
#endif
        return false;
    }
    *codePath = Utils::Strdup(item->valuestring);
    if (*codePath == nullptr) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] parsed codePath is nullptr");
#endif
        return false;
    }
    if (!IsDir(*codePath)) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] parsed codePath is not dir");
#endif
        return false;
    }
    if (!EndWith(*codePath, bundleName)) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] parsed codePath is not valid");
#endif
        return false;
    }

    item = cJSON_GetObjectItem(object, JSON_SUB_KEY_APPID);
    if (!cJSON_IsString(item)) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] result of parsinng appId is not a string");
#endif
        return false;
    }
    *appId = Utils::Strdup(item->valuestring);
    if (*appId == nullptr) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] parsed appId is null");
#endif
        return false;
    }

    item = cJSON_GetObjectItem(object, JSON_SUB_KEY_VERSIONCODE);
    if (!cJSON_IsNumber(item)) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] result of parsinng versionCode is not a string");
#endif
        return false;
    }
    versionCode = item->valueint;
    if (versionCode < 0) {
        cJSON_Delete(object);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] parsed versionCode is not valid");
#endif
        return false;
    }

    cJSON_Delete(object);
    return true;
}

char *BundleUtil::GetValueFromBundleJson(const char *bundleName, const char *key)
{
    if (bundleName == nullptr || key == nullptr) {
        return nullptr;
    }

    char bundleJsonPath[PATH_LENGTH] = { 0 };
    if (sprintf_s(bundleJsonPath, PATH_LENGTH, "%s%s%s", JSON_PATH, bundleName, JSON_SUFFIX) < 0) {
        return nullptr;
    }

    cJSON *object = GetJsonStream(bundleJsonPath);
    if (object == nullptr) {
        return nullptr;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (!cJSON_IsString(item)) {
        cJSON_Delete(object);
        return nullptr;
    }

    char *value = Utils::Strdup(item->valuestring);
    cJSON_Delete(object);
    return value;
}

int32_t BundleUtil::GetValueFromBundleJson(const char *bundleName, const char *key, int32_t defaultValue)
{
    if (bundleName == nullptr || key == nullptr) {
        return defaultValue;
    }

    char bundleJsonPath[PATH_LENGTH] = { 0 };
    if (sprintf_s(bundleJsonPath, PATH_LENGTH, "%s%s%s", JSON_PATH, bundleName, JSON_SUFFIX) < 0) {
        return defaultValue;
    }

    cJSON *object = GetJsonStream(bundleJsonPath);
    if (object == nullptr) {
        return defaultValue;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (!cJSON_IsNumber(item)) {
        cJSON_Delete(object);
        return defaultValue;
    }

    int32_t value = item->valueint;
    cJSON_Delete(object);
    return value;
}

cJSON *BundleUtil::ConvertInstallRecordToJson(const InstallRecord &installRecord)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return nullptr;
    }

    if ((cJSON_AddStringToObject(root, JSON_SUB_KEY_PACKAGE, installRecord.bundleName) == nullptr) ||
        (cJSON_AddStringToObject(root, JSON_SUB_KEY_APPID, installRecord.appId) == nullptr) ||
#ifdef APP_PLATFORM_WATCHGT
#ifdef BC_TRANS_ENABLE
    (cJSON_AddStringToObject(root, JSON_SUB_KEY_JSENGINE_VERSION, installRecord.jsEngineVersion) == nullptr) ||
    (cJSON_AddNumberToObject(root, JSON_SUB_KEY_TRANSFORM_RESULT, installRecord.transformResult) == nullptr) ||
#endif
#endif
    (cJSON_AddNumberToObject(root, JSON_SUB_KEY_VERSIONCODE, installRecord.versionCode) == nullptr) ||
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    (cJSON_AddNumberToObject(root, JSON_SUB_KEY_UID, installRecord.uid) == nullptr) ||
    (cJSON_AddNumberToObject(root, JSON_SUB_KEY_GID, installRecord.gid) == nullptr) ||
#endif
    (cJSON_AddStringToObject(root, JSON_SUB_KEY_CODEPATH, installRecord.codePath) == nullptr)) {
        cJSON_Delete(root);
        return nullptr;
    }
    return root;
}

void BundleUtil::CreateRandStr(char *str, uint32_t len)
{
    if (str == nullptr) {
        return;
    }
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    srand(time(NULL));
#else
    srand(LOS_TickCountGet());
#endif
    uint32_t i;
    for (i = 0; i < len - 1; ++i) {
        switch ((rand() % NUM_OF_TYPE)) {
            case 0:
                str[i] = 'A' + rand() % MAX_CHARACTER_VALUE;
                break;
            case 1:
                str[i] = 'a' + rand() % MAX_CHARACTER_VALUE;
                break;
            default:
                str[i] = '0' + rand() % MAX_SINGLE_DIGIT_VALUE;
                break;
        }
    }
    str[i] = '\0';
}

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
cJSON *BundleUtil::ObtainUidAndGidJson(bool flag)
{
    std::string uidJsonPath = std::string(JSON_PATH) + UID_GID_MAP + JSON_SUFFIX;
    cJSON *object = BundleUtil::GetJsonStream(uidJsonPath.c_str());
    if ((object != nullptr) || (!flag)) {
        return object;
    }

    object = cJSON_CreateObject();
    if (object == nullptr) {
        return nullptr;
    }
    cJSON *size = cJSON_CreateNumber(0);
    if (size == nullptr) {
        cJSON_Delete(object);
        return nullptr;
    }
    if (!cJSON_AddItemToObject(object, PROFILE_KEY_UID_SIZE, size)) {
        cJSON_Delete(size);
        cJSON_Delete(object);
        return nullptr;
    }
    cJSON *uids = cJSON_AddArrayToObject(object, PROFILE_KEY_UID_AND_GID);
    if (uids == nullptr) {
        cJSON_Delete(object);
        return nullptr;
    }
    return object;
}

bool BundleUtil::AddUidAndGidInfo(const InstallRecord &installRecord, cJSON *size, cJSON *uids)
{
    if ((size == nullptr) || (uids == nullptr)) {
        return false;
    }
    cJSON_SetNumberValue(size, size->valueint + 1);
    cJSON *uid = cJSON_CreateObject();
    if (uid == nullptr) {
        return false;
    }
    if ((cJSON_AddStringToObject(uid, JSON_SUB_KEY_PACKAGE, installRecord.bundleName) == nullptr) ||
        (cJSON_AddNumberToObject(uid, JSON_SUB_KEY_UID, installRecord.uid) == nullptr) ||
        (cJSON_AddNumberToObject(uid, JSON_SUB_KEY_GID, installRecord.gid) == nullptr)) {
        cJSON_Delete(uid);
        return false;
    }
    if (!cJSON_AddItemToArray(uids, uid)) {
        cJSON_Delete(uid);
        return false;
    }
    return true;
}

cJSON *BundleUtil::ConvertUidAndGidToJson(const InstallRecord &installRecord)
{
    if (installRecord.bundleName == nullptr) {
        return nullptr;
    }

    cJSON *object = ObtainUidAndGidJson(true);
    if (object == nullptr) {
        return nullptr;
    }

    cJSON *size = cJSON_GetObjectItemCaseSensitive(object, PROFILE_KEY_UID_SIZE);
    if (!cJSON_IsNumber(size)) {
        cJSON_Delete(object);
        return nullptr;
    }
    cJSON *uids = cJSON_GetObjectItemCaseSensitive(object, PROFILE_KEY_UID_AND_GID);
    if (!cJSON_IsArray(uids)) {
        cJSON_Delete(object);
        return nullptr;
    }

    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, uids) {
        cJSON *innerBundleName = cJSON_GetObjectItemCaseSensitive(item, JSON_SUB_KEY_PACKAGE);
        if ((cJSON_IsString(innerBundleName)) && (innerBundleName->valuestring != nullptr)) {
            if (strcmp(innerBundleName->valuestring, installRecord.bundleName) == 0) {
                return object;
            }
        }
    }

    if (!AddUidAndGidInfo(installRecord, size, uids)) {
        cJSON_Delete(object);
        return nullptr;
    }

    return object;
}

bool BundleUtil::DeleteInnerUidInfoFromUidArray(const char *bundleName, cJSON *size, cJSON *uids)
{
    if ((size == nullptr) || (uids == nullptr) || (bundleName == nullptr)) {
        return false;
    }
    cJSON *uid = nullptr;
    int32_t index = -1;
    bool isFound = false;
    cJSON_ArrayForEach(uid, uids) {
        index++;
        cJSON *innerBundleName = cJSON_GetObjectItemCaseSensitive(uid, JSON_SUB_KEY_PACKAGE);
        if ((cJSON_IsString(innerBundleName)) && (innerBundleName->valuestring != nullptr)) {
            if (strcmp(innerBundleName->valuestring, bundleName) == 0) {
                isFound = true;
                break;
            }
        }
    }
    if (isFound) {
        cJSON_DeleteItemFromArray(uids, index);
        cJSON_SetNumberValue(size, size->valueint - 1);
    }
    return true;
}

bool BundleUtil::DeleteUidInfoFromJson(const char *bundleName)
{
    cJSON *object = ObtainUidAndGidJson(false);
    if (object == nullptr) {
        return false;
    }

    cJSON *size = cJSON_GetObjectItemCaseSensitive(object, PROFILE_KEY_UID_SIZE);
    if (!cJSON_IsNumber(size)) {
        cJSON_Delete(object);
        return false;
    }
    cJSON *uids = cJSON_GetObjectItemCaseSensitive(object, PROFILE_KEY_UID_AND_GID);
    if (!cJSON_IsArray(uids)) {
        cJSON_Delete(object);
        return false;
    }
    if (!DeleteInnerUidInfoFromUidArray(bundleName, size, uids)) {
        cJSON_Delete(object);
        return false;
    }

    std::string uidJsonPath = std::string(JSON_PATH) + UID_GID_MAP + JSON_SUFFIX;
    if (size->valueint == 0) {
        BundleDaemonClient::GetInstance().RemoveFile(uidJsonPath.c_str());
    } else {
        char *out = cJSON_Print(object);
        if (out == nullptr) {
            cJSON_Delete(object);
            return false;
        }
        BundleDaemonClient::GetInstance().StoreContentToFile(uidJsonPath.c_str(), out, strlen(out) + 1);
        cJSON_free(out);
    }

    cJSON_Delete(object);
    return true;
}
#else
bool BundleUtil::MkDirs(const char *dir)
{
    if (IsDir(dir)) {
        return true;
    }
    if (strlen(dir) > PATH_LENGTH) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] MkDirs failed, path is too long");
#endif
        return false;
    }
    int32_t len = strlen(dir);
    char *rootDir = nullptr;
    bool isRootDirExists = true;
    for (int32_t i = 1; i < len; i++) {
        if (dir[i] != '/') {
            continue;
        }
        rootDir = GetRootDir(dir, i);
        if (rootDir == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] MkDirs failed, rootDir is null");
#endif
            return false;
        }
        if (isRootDirExists) {
            if (IsDir(rootDir)) {
                UI_Free(rootDir);
                rootDir = nullptr;
                continue;
            }
            isRootDirExists = false;
        }
        if (mkdir(rootDir, S_IREAD | S_IWRITE) < 0) {
            UI_Free(rootDir);
#ifdef APP_PLATFORM_WATCHGT
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] make rootDir failed");
#endif
            return false;
        }
        UI_Free(rootDir);
        rootDir = nullptr;
    }
    return (mkdir(dir, S_IREAD | S_IWRITE) < 0) ? false : true;
}

bool BundleUtil::RemoveDir(const char *path)
{
    if (!IsDir(path)) {
        (void) unlink(path);
        return true;
    }

    List<char *>* list = new (std::nothrow)List<char *>();
    if (list == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] RemoveDir failed, list is null");
#endif
        return false;
    }
    list->PushFront(Utils::Strdup(path));
    while (!list->IsEmpty()) {
        char *curPath = list->Front();
        if (curPath == nullptr) {
            break;
        }
        if (CheckDirIsEmpty(curPath, list)) {
            list->PopFront();
            if (rmdir(curPath) < 0) {
                AdapterFree(curPath);
                break;
            }
            AdapterFree(curPath);
        }
    }
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    for (auto node = list->Begin(); node != list->End(); node = node->next_) {
        AdapterFree(node->value_);
    }
#endif
    if (!list->IsEmpty()) {
        delete list;
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] After delete, file is still not empty");
#endif
        return false;
    }

    delete list;
    return true;
}

bool BundleUtil::RenameDir(const char *oldDir, const char *newDir)
{
    if (oldDir == nullptr || newDir == nullptr) {
        return false;
    }

    if (IsDir(newDir) && !RemoveDir(newDir)) {
        return false;
    }
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    if (frename(oldDir, newDir) != 0) {
        return false;
    }
#else
    if (rename(oldDir, newDir) != 0) {
        return false;
    }
#endif
    return true;
}

bool BundleUtil::RenameFile(const char *oldFile, const char *newFile)
{
    if (oldFile == nullptr || newFile == nullptr) {
        return false;
    }

    if (IsFile(newFile) && unlink(newFile) < 0) {
        return false;
    }

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    if (frename(oldFile, newFile) != 0) {
        return false;
    }
#else
    if (rename(oldFile, newFile) != 0) {
        return false;
    }
#endif
    return true;
}

char *BundleUtil::GetRootDir(const char *dir, int32_t index)
{
    char *rootDir = reinterpret_cast<char *>(UI_Malloc((index + 1) * sizeof(char)));
    if (rootDir == nullptr) {
        return nullptr;
    }
    errno_t err = strncpy_s(rootDir, index + 1, dir, index);
    if (err != EOK) {
        UI_Free(rootDir);
        return nullptr;
    }
    return rootDir;
}

bool BundleUtil::CheckDirIsEmpty(const char *dirPath, List<char *>* list)
{
    DIR *dir = nullptr;
    if ((dir = opendir(dirPath)) == nullptr) {
        return false;
    }
    bool isEmptyDir = true;
    dirent *dp = nullptr;
    char filePath[PATH_LENGTH] = { 0 };
    while ((dp = readdir(dir)) != nullptr) {
        if ((strcmp(dp->d_name, ".") == 0) || (strcmp(dp->d_name, "..")) == 0) {
            continue;
        }

        if (memset_s(filePath, PATH_LENGTH, 0, PATH_LENGTH) != EOK) {
            continue;
        }

        if (sprintf_s(filePath, PATH_LENGTH, "%s/%s", dirPath, dp->d_name) < 0) {
            continue;
        }

        if (IsDir(filePath)) {
            list->PushFront(Utils::Strdup(filePath));
            isEmptyDir = false;
        } else {
            (void) unlink(filePath);
        }
    }
    closedir(dir);
    return isEmptyDir;
}
bool BundleUtil::StoreJsonContentToFile(const char *packageJson, const cJSON *object)
{
    if (object == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Json data object is null");
#endif
        return false;
    }

    char *out = cJSON_Print(object);
    if (out == nullptr) {
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] turn Json data to char sequence failed");
#endif
        return false;
    }

    if (!CheckRealPath(packageJson)) {
        cJSON_free(out);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Json file path doest not exist");
#endif
        return false;
    }

    if (IsFile(packageJson) && unlink(packageJson) < 0) {
        cJSON_free(out);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Json file path is invalid");
#endif
        return false;
    }

    int32_t fp = open(packageJson, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
    if (fp < 0) {
        cJSON_free(out);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] open Json file failed");
#endif
        return false;
    }

    if (write(fp, out, strlen(out)) != strlen(out)) {
        cJSON_free(out);
        close(fp);
#ifdef APP_PLATFORM_WATCHGT
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] write to Json file failed");
#endif
        return false;
    }

    cJSON_free(out);
    close(fp);
    return true;
}

void BundleUtil::FreeStrArrayMemory(char **pointer, uint32_t len)
{
    if (len != 0 && pointer != nullptr) {
        for (uint32_t i = 0; i < len; i++) {
            AdapterFree(*(pointer + i));
        }
        AdapterFree(pointer);
    }
}
#endif
} // namespace OHOS