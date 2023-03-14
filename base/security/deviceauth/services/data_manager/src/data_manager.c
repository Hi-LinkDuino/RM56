/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "data_manager.h"

#include "broadcast_manager.h"
#include "common_defs.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "hc_dev_info.h"
#include "hc_file.h"
#include "hc_log.h"
#include "hc_mutex.h"
#include "hc_string_vector.h"
#include "hc_types.h"
#include "securec.h"

typedef struct {
    DECLARE_TLV_STRUCT(9)
    TlvString name;
    TlvString id;
    TlvUint32 type;
    TlvInt32 visibility;
    TlvInt32 expireTime;
    TlvString userIdHash;
    TlvBuffer sharedUserIdHashVec;
    TlvBuffer managers;
    TlvBuffer friends;
} TlvGroupElement;
DECLEAR_INIT_FUNC(TlvGroupElement)
DECLARE_TLV_VECTOR(TlvGroupVec, TlvGroupElement)

typedef struct {
    uint8_t credential;
    uint8_t devType;
    int64_t userId;
    uint64_t lastTm;
} DevAuthFixedLenInfo;
DECLARE_TLV_FIX_LENGTH_TYPE(TlvDevAuthFixedLenInfo, DevAuthFixedLenInfo)
DECLEAR_INIT_FUNC(TlvDevAuthFixedLenInfo)

typedef struct {
    DECLARE_TLV_STRUCT(7)
    TlvString groupId;
    TlvString udid;
    TlvString authId;
    TlvString serviceType;
    TlvBuffer ext;
    TlvDevAuthFixedLenInfo info;
    TlvString userIdHash;
} TlvDeviceElement;
DECLEAR_INIT_FUNC(TlvDeviceElement)
DECLARE_TLV_VECTOR(TlvDeviceVec, TlvDeviceElement)

typedef struct {
    DECLARE_TLV_STRUCT(3)
    TlvInt32 version;
    TlvGroupVec groups;
    TlvDeviceVec devices;
} HCDataBaseV1;
DECLEAR_INIT_FUNC(HCDataBaseV1)

DEFINE_TLV_FIX_LENGTH_TYPE(TlvDevAuthFixedLenInfo, NO_REVERT)

BEGIN_TLV_STRUCT_DEFINE(TlvGroupElement, 0x0001)
    TLV_MEMBER(TlvString, name, 0x4001)
    TLV_MEMBER(TlvString, id, 0x4002)
    TLV_MEMBER(TlvUint32, type, 0x4003)
    TLV_MEMBER(TlvInt32, visibility, 0x4004)
    TLV_MEMBER(TlvInt32, expireTime, 0x4005)
    TLV_MEMBER(TlvString, userIdHash, 0x4006)
    TLV_MEMBER(TlvBuffer, sharedUserIdHashVec, 0x4007)
    TLV_MEMBER(TlvBuffer, managers, 0x4008)
    TLV_MEMBER(TlvBuffer, friends, 0x4009)
END_TLV_STRUCT_DEFINE()
IMPLEMENT_TLV_VECTOR(TlvGroupVec, TlvGroupElement, 1)

BEGIN_TLV_STRUCT_DEFINE(TlvDeviceElement, 0x0002)
    TLV_MEMBER(TlvString, groupId, 0x4101)
    TLV_MEMBER(TlvString, udid, 0x4102)
    TLV_MEMBER(TlvString, authId, 0x4103)
    TLV_MEMBER(TlvString, userIdHash, 0x4107)
    TLV_MEMBER(TlvString, serviceType, 0x4104)
    TLV_MEMBER(TlvBuffer, ext, 0x4105)
    TLV_MEMBER(TlvDevAuthFixedLenInfo, info, 0x4106)
END_TLV_STRUCT_DEFINE()
IMPLEMENT_TLV_VECTOR(TlvDeviceVec, TlvDeviceElement, 1)

BEGIN_TLV_STRUCT_DEFINE(HCDataBaseV1, 0x0001)
    TLV_MEMBER(TlvInt32, version, 0x6001)
    TLV_MEMBER(TlvGroupVec, groups, 0x6002)
    TLV_MEMBER(TlvDeviceVec, devices, 0x6003)
END_TLV_STRUCT_DEFINE()

IMPLEMENT_HC_VECTOR(GroupEntryVec, TrustedGroupEntry*, 1)
IMPLEMENT_HC_VECTOR(DeviceEntryVec, TrustedDeviceEntry*, 1)

typedef struct {
    int32_t osAccountId;
    GroupEntryVec groups;
    DeviceEntryVec devices;
} OsAccountTrustedInfo;

DECLARE_HC_VECTOR(DeviceAuthDb, OsAccountTrustedInfo)
IMPLEMENT_HC_VECTOR(DeviceAuthDb, OsAccountTrustedInfo, 1)

#define MAX_DB_PATH_LEN 256

static HcMutex *g_databaseMutex = NULL;
static DeviceAuthDb g_deviceauthDb;

static bool EndWithZero(HcParcel *parcel)
{
    const char *p = GetParcelLastChar(parcel);
    if (p == NULL) {
        return false;
    }
    return (*p == '\0');
}

static bool LoadStringVectorFromParcel(StringVector *vec, HcParcel *parcel)
{
    uint32_t strLen = 0;
    do {
        if (!ParcelReadUint32(parcel, &strLen)) {
            return true;
        }
        if ((strLen == 0) || (strLen > MAX_STRING_LEN)) {
            return false;
        }
        HcString str = CreateString();
        ClearParcel(&str.parcel);
        if (!ParcelReadParcel(parcel, &str.parcel, strLen, false) ||
            !EndWithZero(&str.parcel)) {
            DeleteString(&str);
            return false;
        } else {
            if (vec->pushBack(vec, &str) == NULL) {
                DeleteString(&str);
                return false;
            }
        }
    } while (1);
}

static bool SaveStringVectorToParcel(const StringVector *vec, HcParcel *parcel)
{
    uint32_t index;
    HcString *str = NULL;
    FOR_EACH_HC_VECTOR(*vec, index, str) {
        uint32_t len = StringLength(str) + sizeof(char);
        if (!ParcelWriteUint32(parcel, len)) {
            return false;
        }
        if (!ParcelWrite(parcel, GetParcelData(&str->parcel), GetParcelDataSize(&str->parcel))) {
            return false;
        }
    }
    return true;
}

static OsAccountTrustedInfo *GetTrustedInfoByOsAccountId(int32_t osAccountId)
{
    uint32_t index = 0;
    OsAccountTrustedInfo *info = NULL;
    FOR_EACH_HC_VECTOR(g_deviceauthDb, index, info) {
        if ((info != NULL) && (info->osAccountId == osAccountId)) {
            return info;
        }
    }
    LOGI("[DB]: Create a new os account database cache! [Id]: %d", osAccountId);
    OsAccountTrustedInfo newInfo;
    newInfo.osAccountId = osAccountId;
    newInfo.groups = CreateGroupEntryVec();
    newInfo.devices = CreateDeviceEntryVec();
    OsAccountTrustedInfo *returnInfo = g_deviceauthDb.pushBackT(&g_deviceauthDb, newInfo);
    if (returnInfo == NULL) {
        LOGE("[DB]: Failed to push osAccountInfo to database!");
        DestroyGroupEntryVec(&newInfo.groups);
        DestroyDeviceEntryVec(&newInfo.devices);
    }
    return returnInfo;
}

static bool GetOsAccountInfoPath(int32_t osAccountId, char *infoPath, uint32_t pathBufferLen)
{
    const char *beginPath = GetStorageDirPath();
    if (beginPath == NULL) {
        LOGE("[DB]: Failed to get the storage path dir!");
        return false;
    }
    int32_t ret;
    if (osAccountId == DEFAULT_OS_ACCOUNT) {
        ret = sprintf_s(infoPath, pathBufferLen, "%s/hcgroup.dat", beginPath);
    } else {
        ret = sprintf_s(infoPath, pathBufferLen, "%s/hcgroup%d.dat", beginPath, osAccountId);
    }
    if (ret <= 0) {
        LOGE("[DB]: sprintf_s fail!");
        return false;
    }
    return true;
}

bool GenerateGroupEntryFromEntry(const TrustedGroupEntry *entry, TrustedGroupEntry *returnEntry)
{
    if (HC_VECTOR_SIZE(&entry->managers) <= 0) {
        LOGE("[DB]: The group owner is lost!");
        return false;
    }
    HcString entryOwner = HC_VECTOR_GET(&entry->managers, 0);
    if (!StringSet(&returnEntry->name, entry->name)) {
        LOGE("[DB]: Failed to copy groupName!");
        return false;
    }
    if (!StringSet(&returnEntry->id, entry->id)) {
        LOGE("[DB]: Failed to copy groupId!");
        return false;
    }
    if (!StringSet(&returnEntry->userIdHash, entry->userIdHash)) {
        LOGE("[DB]: Failed to copy userIdHash!");
        return false;
    }
    returnEntry->type = entry->type;
    returnEntry->visibility = entry->visibility;
    returnEntry->expireTime = entry->expireTime;
    HcString ownerName = CreateString();
    if (!StringSet(&ownerName, entryOwner)) {
        LOGE("[DB]: Failed to copy groupOwner!");
        DeleteString(&ownerName);
        return false;
    }
    if (returnEntry->managers.pushBack(&returnEntry->managers, &ownerName) == NULL) {
        LOGE("[DB]: Failed to push groupOwner to managers!");
        DeleteString(&ownerName);
        return false;
    }
    return true;
}

bool GenerateDeviceEntryFromEntry(const TrustedDeviceEntry *entry, TrustedDeviceEntry *returnEntry)
{
    returnEntry->groupEntry = NULL;
    if (!StringSet(&returnEntry->groupId, entry->groupId)) {
        LOGE("[DB]: Failed to copy udid!");
        return false;
    }
    if (!StringSet(&returnEntry->udid, entry->udid)) {
        LOGE("[DB]: Failed to copy udid!");
        return false;
    }
    if (!StringSet(&returnEntry->authId, entry->authId)) {
        LOGE("[DB]: Failed to copy authId!");
        return false;
    }
    if (!StringSet(&returnEntry->serviceType, entry->serviceType)) {
        LOGE("[DB]: Failed to copy serviceType!");
        return false;
    }
    if (!StringSet(&returnEntry->userIdHash, entry->userIdHash)) {
        LOGE("[DB]: Failed to copy userIdHash!");
        return false;
    }
    returnEntry->credential = entry->credential;
    returnEntry->devType = entry->devType;
    returnEntry->lastTm = entry->lastTm;
    return true;
}

static bool GenerateGroupEntryFromTlv(TlvGroupElement *group, TrustedGroupEntry *entry)
{
    if (!LoadStringVectorFromParcel(&entry->managers, &group->managers.data)) {
        return false;
    }
    if (!LoadStringVectorFromParcel(&entry->friends, &group->friends.data)) {
        return false;
    }
    if (!LoadStringVectorFromParcel(&entry->sharedUserIdHashVec, &group->sharedUserIdHashVec.data)) {
        return false;
    }
    if (!StringSet(&entry->name, group->name.data)) {
        return false;
    }
    if (!StringSet(&entry->id, group->id.data)) {
        return false;
    }
    if (!StringSet(&entry->userIdHash, group->userIdHash.data)) {
        return false;
    }
    entry->type = group->type.data;
    entry->visibility = group->visibility.data;
    entry->expireTime = group->expireTime.data;
    return true;
}

static bool GenerateDeviceEntryFromTlv(TlvDeviceElement *device, TrustedDeviceEntry *deviceEntry)
{
    deviceEntry->groupEntry = NULL;
    if (!StringSet(&deviceEntry->groupId, device->groupId.data)) {
        return false;
    }
    if (!StringSet(&deviceEntry->udid, device->udid.data)) {
        return false;
    }
    if (!StringSet(&deviceEntry->authId, device->authId.data)) {
        return false;
    }
    if (!StringSet(&deviceEntry->serviceType, device->serviceType.data)) {
        return false;
    }
    if (!StringSet(&deviceEntry->userIdHash, device->userIdHash.data)) {
        return false;
    }
    if (!ParcelCopy(&device->ext.data, &deviceEntry->ext)) {
        return false;
    }
    deviceEntry->credential = device->info.data.credential;
    deviceEntry->devType = device->info.data.devType;
    deviceEntry->lastTm = device->info.data.lastTm;
    return true;
}

static bool LoadGroups(HCDataBaseV1 *db, GroupEntryVec *vec)
{
    uint32_t index;
    TlvGroupElement *group = NULL;
    FOR_EACH_HC_VECTOR(db->groups.data, index, group) {
        TrustedGroupEntry *entry = CreateGroupEntry();
        if (entry == NULL) {
            LOGE("[DB]: Failed to allocate entry memory!");
            ClearGroupEntryVec(vec);
            return false;
        }
        if (!GenerateGroupEntryFromTlv(group, entry)) {
            DestroyGroupEntry(entry);
            ClearGroupEntryVec(vec);
            return false;
        }
        if (vec->pushBackT(vec, entry) == NULL) {
            LOGE("[DB]: Failed to push entry to vec!");
            DestroyGroupEntry(entry);
            ClearGroupEntryVec(vec);
            return false;
        }
    }
    return true;
}

static bool LoadDevices(HCDataBaseV1 *db, DeviceEntryVec *vec)
{
    uint32_t index;
    TlvDeviceElement *device = NULL;
    FOR_EACH_HC_VECTOR(db->devices.data, index, device) {
        TrustedDeviceEntry *entry = CreateDeviceEntry();
        if (entry == NULL) {
            LOGE("[DB]: Failed to allocate entry memory!");
            ClearDeviceEntryVec(vec);
            return false;
        }
        if (!GenerateDeviceEntryFromTlv(device, entry)) {
            DestroyDeviceEntry(entry);
            ClearDeviceEntryVec(vec);
            return false;
        }
        if (vec->pushBackT(vec, entry) == NULL) {
            LOGE("[DB]: Failed to push entry to vec!");
            DestroyDeviceEntry(entry);
            ClearDeviceEntryVec(vec);
            return false;
        }
    }
    return true;
}

static bool ReadInfoFromParcel(HcParcel *parcel, OsAccountTrustedInfo *info)
{
    bool ret = false;
    HCDataBaseV1 dbv1;
    TLV_INIT(HCDataBaseV1, &dbv1)
    if (DecodeTlvMessage((TlvBase *)&dbv1, parcel, false)) {
        if (!LoadGroups(&dbv1, &info->groups)) {
            TLV_DEINIT(dbv1)
            return false;
        }
        if (!LoadDevices(&dbv1, &info->devices)) {
            ClearGroupEntryVec(&info->groups);
            TLV_DEINIT(dbv1)
            return false;
        }
        ret = true;
    } else {
        LOGE("[DB]: Decode Tlv Message Failed!");
    }
    TLV_DEINIT(dbv1)
    return ret;
}

static bool ReadParcelFromFile(int32_t osAccountId, HcParcel *parcel)
{
    char infoPath[MAX_DB_PATH_LEN] = { 0 };
    if (!GetOsAccountInfoPath(osAccountId, infoPath, MAX_DB_PATH_LEN)) {
        return false;
    }
    FileHandle file;
    int ret = HcFileOpen(infoPath, MODE_FILE_READ, &file);
    if (ret != 0) {
        LOGE("[DB]: Failed to open database file!");
        return false;
    }
    int fileSize = HcFileSize(file);
    if (fileSize <= 0) {
        LOGE("[DB]: The database file size is invalid!");
        HcFileClose(file);
        return false;
    }
    char *fileData = (char *)HcMalloc(fileSize, 0);
    if (fileData == NULL) {
        LOGE("[DB]: Failed to allocate fileData memory!");
        HcFileClose(file);
        return false;
    }
    if (HcFileRead(file, fileData, fileSize) != fileSize) {
        LOGE("[DB]: Read file error!");
        HcFileClose(file);
        HcFree(fileData);
        return false;
    }
    HcFileClose(file);
    if (!ParcelWrite(parcel, fileData, fileSize)) {
        LOGE("[DB]: parcel write error!");
        HcFree(fileData);
        return false;
    }
    HcFree(fileData);
    return true;
}

static void LoadOsAccountDb(int32_t osAccountId)
{
    HcParcel parcel = CreateParcel(0, 0);
    if (!ReadParcelFromFile(osAccountId, &parcel)) {
        DeleteParcel(&parcel);
        return;
    }
    OsAccountTrustedInfo info;
    info.osAccountId = osAccountId;
    info.groups = CreateGroupEntryVec();
    info.devices = CreateDeviceEntryVec();
    if (!ReadInfoFromParcel(&parcel, &info)) {
        DestroyGroupEntryVec(&info.groups);
        DestroyDeviceEntryVec(&info.devices);
        DeleteParcel(&parcel);
        return;
    }
    DeleteParcel(&parcel);
    if (g_deviceauthDb.pushBackT(&g_deviceauthDb, info) == NULL) {
        LOGE("[DB]: Failed to push osAccountInfo to database!");
        ClearGroupEntryVec(&info.groups);
        ClearDeviceEntryVec(&info.devices);
    }
    LOGI("[DB]: Load os account db successfully! [Id]: %d", osAccountId);
}

static void LoadDeviceAuthDb(void)
{
    StringVector osAccountDbNameVec = CreateStrVector();
    HcFileGetSubFileName(GetStorageDirPath(), &osAccountDbNameVec);
    uint32_t index;
    HcString *dbName;
    FOR_EACH_HC_VECTOR(osAccountDbNameVec, index, dbName) {
        int32_t osAccountId;
        const char *osAccountIdStr = StringGet(dbName);
        if (osAccountIdStr == NULL) {
            continue;
        }
        if (strcmp(osAccountIdStr, "hcgroup.dat") == 0) {
            LoadOsAccountDb(DEFAULT_OS_ACCOUNT);
        } else if (sscanf_s(osAccountIdStr, "hcgroup%d.dat", &osAccountId) == 1) {
            LoadOsAccountDb(osAccountId);
        }
    }
    DestroyStrVector(&osAccountDbNameVec);
}

static bool SetGroupElement(TlvGroupElement *element, TrustedGroupEntry *entry)
{
    if (!StringSet(&element->name.data, entry->name)) {
        return false;
    }
    if (!StringSet(&element->id.data, entry->id)) {
        return false;
    }
    if (!StringSet(&element->userIdHash.data, entry->userIdHash)) {
        return false;
    }
    element->type.data = entry->type;
    element->visibility.data = entry->visibility;
    element->expireTime.data = entry->expireTime;
    if (!SaveStringVectorToParcel(&entry->managers, &element->managers.data)) {
        return false;
    }
    if (!SaveStringVectorToParcel(&entry->friends, &element->friends.data)) {
        return false;
    }
    if (!SaveStringVectorToParcel(&entry->sharedUserIdHashVec, &element->sharedUserIdHashVec.data)) {
        return false;
    }
    return true;
}

static bool SetDeviceElement(TlvDeviceElement *element, TrustedDeviceEntry *entry)
{
    if (!StringSet(&element->groupId.data, entry->groupId)) {
        return false;
    }
    if (!StringSet(&element->udid.data, entry->udid)) {
        return false;
    }
    if (!StringSet(&element->authId.data, entry->authId)) {
        return false;
    }
    if (!StringSet(&element->serviceType.data, entry->serviceType)) {
        return false;
    }
    if (!StringSet(&element->userIdHash.data, entry->userIdHash)) {
        return false;
    }
    if (!ParcelCopy(&element->ext.data, &entry->ext)) {
        return false;
    }
    element->info.data.credential = entry->credential;
    element->info.data.devType = entry->devType;
    element->info.data.lastTm = entry->lastTm;
    return true;
}

static bool SaveGroups(const GroupEntryVec *vec, HCDataBaseV1 *db)
{
    uint32_t index;
    TrustedGroupEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        TlvGroupElement tmp;
        TlvGroupElement *element = db->groups.data.pushBack(&db->groups.data, &tmp);
        if (element == NULL) {
            return false;
        }
        TLV_INIT(TlvGroupElement, element);
        if (!SetGroupElement(element, *entry)) {
            TLV_DEINIT((*element));
            return false;
        }
    }
    return true;
}

static bool SaveDevices(const DeviceEntryVec *vec, HCDataBaseV1 *db)
{
    uint32_t index;
    TrustedDeviceEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        TlvDeviceElement tmp;
        TlvDeviceElement *element = db->devices.data.pushBack(&db->devices.data, &tmp);
        if (element == NULL) {
            return false;
        }
        TLV_INIT(TlvDeviceElement, element);
        if (!SetDeviceElement(element, *entry)) {
            TLV_DEINIT((*element));
            return false;
        }
    }
    return true;
}

static bool SaveInfoToParcel(const OsAccountTrustedInfo *info, HcParcel *parcel)
{
    int32_t ret = false;
    HCDataBaseV1 dbv1;
    TLV_INIT(HCDataBaseV1, &dbv1)
    dbv1.version.data = 1;
    do {
        if (!SaveGroups(&info->groups, &dbv1)) {
            break;
        }
        if (!SaveDevices(&info->devices, &dbv1)) {
            break;
        }
        if (!EncodeTlvMessage((TlvBase *)&dbv1, parcel)) {
            LOGE("[DB]: Encode Tlv Message failed!");
            break;
        }
        ret = true;
    } while (0);
    TLV_DEINIT(dbv1)
    return ret;
}

static bool SaveParcelToFile(const OsAccountTrustedInfo *info, HcParcel *parcel)
{
    char infoPath[MAX_DB_PATH_LEN] = { 0 };
    if (!GetOsAccountInfoPath(info->osAccountId, infoPath, MAX_DB_PATH_LEN)) {
        return false;
    }
    FileHandle file;
    int ret = HcFileOpen(infoPath, MODE_FILE_WRITE, &file);
    if (ret != HC_SUCCESS) {
        LOGE("[DB]: Failed to open database file!");
        return false;
    }
    int fileSize = (int)GetParcelDataSize(parcel);
    const char *fileData = GetParcelData(parcel);
    if (HcFileWrite(file, fileData, fileSize) == fileSize) {
        ret = true;
    } else {
        LOGE("[DB]: write file error!");
        ret = false;
    }
    HcFileClose(file);
    return ret;
}

static bool CompareQueryGroupParams(const QueryGroupParams *params, const TrustedGroupEntry *entry)
{
    if ((params->groupId != NULL) && (strcmp(params->groupId, StringGet(&entry->id)) != 0)) {
        return false;
    }
    if ((params->groupName != NULL) && (strcmp(params->groupName, StringGet(&entry->name)) != 0)) {
        return false;
    }
    if ((params->groupType != ALL_GROUP) && (params->groupType != entry->type)) {
        return false;
    }
    if ((params->groupVisibility != ALL_GROUP_VISIBILITY) && (params->groupVisibility != entry->visibility)) {
        return false;
    }
    if (params->ownerName != NULL) {
        HcString entryOwner = HC_VECTOR_GET(&entry->managers, 0);
        if (strcmp(params->ownerName, StringGet(&entryOwner)) != 0) {
            return false;
        }
    }
    return true;
}

static bool CompareQueryDeviceParams(const QueryDeviceParams *params, const TrustedDeviceEntry *entry)
{
    if ((params->groupId != NULL) && (strcmp(params->groupId, StringGet(&entry->groupId)) != 0)) {
        return false;
    }
    if ((params->udid != NULL) && (strcmp(params->udid, StringGet(&entry->udid)) != 0)) {
        return false;
    }
    if ((params->authId != NULL) && (strcmp(params->authId, StringGet(&entry->authId)) != 0)) {
        return false;
    }
    return true;
}

static TrustedGroupEntry **QueryGroupEntryPtrIfMatch(const GroupEntryVec *vec, const QueryGroupParams *params)
{
    uint32_t index;
    TrustedGroupEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        if ((entry != NULL) && (*entry != NULL) && (CompareQueryGroupParams(params, *entry))) {
            return entry;
        }
    }
    return NULL;
}

static TrustedDeviceEntry **QueryDeviceEntryPtrIfMatch(const DeviceEntryVec *vec, const QueryDeviceParams *params)
{
    uint32_t index;
    TrustedDeviceEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        if ((entry != NULL) && (*entry != NULL) && (CompareQueryDeviceParams(params, *entry))) {
            return entry;
        }
    }
    return NULL;
}

static void PostGroupCreatedMsg(const TrustedGroupEntry *groupEntry)
{
    if (!IsBroadcastSupported()) {
        return;
    }
    GetBroadcaster()->postOnGroupCreated(groupEntry);
}

static void PostGroupDeletedMsg(const TrustedGroupEntry *groupEntry)
{
    if (!IsBroadcastSupported()) {
        return;
    }
    GetBroadcaster()->postOnGroupDeleted(groupEntry);
}

static void PostDeviceBoundMsg(OsAccountTrustedInfo *info, const TrustedDeviceEntry *deviceEntry)
{
    if (!IsBroadcastSupported()) {
        return;
    }
    QueryGroupParams groupParams = InitQueryGroupParams();
    groupParams.groupId = StringGet(&deviceEntry->groupId);
    TrustedGroupEntry **groupEntryPtr = QueryGroupEntryPtrIfMatch(&info->groups, &groupParams);
    if (groupEntryPtr != NULL) {
        GetBroadcaster()->postOnDeviceBound(StringGet(&deviceEntry->udid), *groupEntryPtr);
    }
}

static void PostDeviceUnBoundMsg(OsAccountTrustedInfo *info, const TrustedDeviceEntry *deviceEntry)
{
    if (!IsBroadcastSupported()) {
        return;
    }
    const char *groupId = StringGet(&deviceEntry->groupId);
    const char *udid = StringGet(&deviceEntry->udid);
    QueryGroupParams groupParams = InitQueryGroupParams();
    groupParams.groupId = groupId;
    TrustedGroupEntry **groupEntryPtr = QueryGroupEntryPtrIfMatch(&info->groups, &groupParams);
    if (groupEntryPtr != NULL) {
        GetBroadcaster()->postOnDeviceUnBound(udid, *groupEntryPtr);
    }
    QueryDeviceParams deviceParams = InitQueryDeviceParams();
    deviceParams.udid = udid;
    if (QueryDeviceEntryPtrIfMatch(&info->devices, &deviceParams) == NULL) {
        GetBroadcaster()->postOnDeviceNotTrusted(udid);
    }
}

QueryGroupParams InitQueryGroupParams(void)
{
    QueryGroupParams params = {
        .groupId = NULL,
        .groupName = NULL,
        .ownerName = NULL,
        .groupType = ALL_GROUP,
        .groupVisibility = ALL_GROUP_VISIBILITY
    };
    return params;
}

QueryDeviceParams InitQueryDeviceParams(void)
{
    QueryDeviceParams params = {
        .groupId = NULL,
        .udid = NULL,
        .authId = NULL
    };
    return params;
}

TrustedGroupEntry *CreateGroupEntry(void)
{
    TrustedGroupEntry *ptr = (TrustedGroupEntry *)HcMalloc(sizeof(TrustedGroupEntry), 0);
    if (ptr == NULL) {
        LOGE("[DB]: Failed to allocate groupEntry memory!");
        return NULL;
    }
    ptr->name = CreateString();
    ptr->id = CreateString();
    ptr->userIdHash = CreateString();
    ptr->sharedUserIdHashVec = CreateStrVector();
    ptr->managers = CreateStrVector();
    ptr->friends = CreateStrVector();
    return ptr;
}

void DestroyGroupEntry(TrustedGroupEntry *groupEntry)
{
    DeleteString(&groupEntry->name);
    DeleteString(&groupEntry->id);
    DeleteString(&groupEntry->userIdHash);
    DestroyStrVector(&groupEntry->managers);
    DestroyStrVector(&groupEntry->friends);
    DestroyStrVector(&groupEntry->sharedUserIdHashVec);
    HcFree(groupEntry);
}

TrustedGroupEntry *DeepCopyGroupEntry(const TrustedGroupEntry *entry)
{
    TrustedGroupEntry *returnEntry = CreateGroupEntry();
    if (returnEntry == NULL) {
        return NULL;
    }
    if (!GenerateGroupEntryFromEntry(entry, returnEntry)) {
        DestroyGroupEntry(returnEntry);
        return NULL;
    }
    return returnEntry;
}

TrustedDeviceEntry *CreateDeviceEntry(void)
{
    TrustedDeviceEntry *ptr = (TrustedDeviceEntry *)HcMalloc(sizeof(TrustedDeviceEntry), 0);
    if (ptr == NULL) {
        LOGE("[DB]: Failed to allocate deviceEntry memory!");
        return NULL;
    }
    ptr->groupId = CreateString();
    ptr->udid = CreateString();
    ptr->authId = CreateString();
    ptr->serviceType = CreateString();
    ptr->userIdHash = CreateString();
    ptr->ext = CreateParcel(0, 0);
    return ptr;
}

void DestroyDeviceEntry(TrustedDeviceEntry *deviceEntry)
{
    DeleteString(&deviceEntry->groupId);
    DeleteString(&deviceEntry->udid);
    DeleteString(&deviceEntry->authId);
    DeleteString(&deviceEntry->serviceType);
    DeleteString(&deviceEntry->userIdHash);
    DeleteParcel(&deviceEntry->ext);
    HcFree(deviceEntry);
}

TrustedDeviceEntry *DeepCopyDeviceEntry(const TrustedDeviceEntry *entry)
{
    TrustedDeviceEntry *returnEntry = CreateDeviceEntry();
    if (returnEntry == NULL) {
        return NULL;
    }
    if (!GenerateDeviceEntryFromEntry(entry, returnEntry)) {
        DestroyDeviceEntry(returnEntry);
        return NULL;
    }
    return returnEntry;
}

void ClearGroupEntryVec(GroupEntryVec *vec)
{
    uint32_t index;
    TrustedGroupEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        DestroyGroupEntry(*entry);
    }
    DESTROY_HC_VECTOR(GroupEntryVec, vec);
}

void ClearDeviceEntryVec(DeviceEntryVec *vec)
{
    uint32_t index;
    TrustedDeviceEntry **entry;
    FOR_EACH_HC_VECTOR(*vec, index, entry) {
        DestroyDeviceEntry(*entry);
    }
    DESTROY_HC_VECTOR(DeviceEntryVec, vec);
}

int32_t AddGroup(int32_t osAccountId, const TrustedGroupEntry *groupEntry)
{
    LOGI("[DB]: Start to add a group to database!");
    if (groupEntry == NULL) {
        LOGE("[DB]: The input groupEntry is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    TrustedGroupEntry *newEntry = DeepCopyGroupEntry(groupEntry);
    if (newEntry == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_MEMORY_COPY;
    }
    QueryGroupParams params = InitQueryGroupParams();
    params.groupId = StringGet(&groupEntry->id);
    TrustedGroupEntry **oldEntryPtr = QueryGroupEntryPtrIfMatch(&info->groups, &params);
    if (oldEntryPtr != NULL) {
        DestroyGroupEntry(*oldEntryPtr);
        *oldEntryPtr = newEntry;
        PostGroupCreatedMsg(newEntry);
        g_databaseMutex->unlock(g_databaseMutex);
        LOGI("[DB]: Replace an old group successfully! [GroupType]: %d", groupEntry->type);
        return HC_SUCCESS;
    }
    if (info->groups.pushBackT(&info->groups, newEntry) == NULL) {
        DestroyGroupEntry(newEntry);
        g_databaseMutex->unlock(g_databaseMutex);
        LOGE("[DB]: Failed to push groupEntry to vec!");
        return HC_ERR_MEMORY_COPY;
    }
    PostGroupCreatedMsg(newEntry);
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Add a group to database successfully! [GroupType]: %d", groupEntry->type);
    return HC_SUCCESS;
}

int32_t AddTrustedDevice(int32_t osAccountId, const TrustedDeviceEntry *deviceEntry)
{
    LOGI("[DB]: Start to add a trusted device to database!");
    if (deviceEntry == NULL) {
        LOGE("[DB]: The input deviceEntry is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    TrustedDeviceEntry *newEntry = DeepCopyDeviceEntry(deviceEntry);
    if (newEntry == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_MEMORY_COPY;
    }
    QueryDeviceParams params = InitQueryDeviceParams();
    params.udid = StringGet(&deviceEntry->udid);
    params.groupId = StringGet(&deviceEntry->groupId);
    TrustedDeviceEntry **oldEntryPtr = QueryDeviceEntryPtrIfMatch(&info->devices, &params);
    if (oldEntryPtr != NULL) {
        DestroyDeviceEntry(*oldEntryPtr);
        *oldEntryPtr = newEntry;
        PostDeviceBoundMsg(info, newEntry);
        g_databaseMutex->unlock(g_databaseMutex);
        LOGI("[DB]: Replace an old trusted device successfully!");
        return HC_SUCCESS;
    }
    if (info->devices.pushBackT(&info->devices, newEntry) == NULL) {
        DestroyDeviceEntry(newEntry);
        g_databaseMutex->unlock(g_databaseMutex);
        LOGE("[DB]: Failed to push deviceEntry to vec!");
        return HC_ERR_MEMORY_COPY;
    }
    PostDeviceBoundMsg(info, newEntry);
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Add a trusted device to database successfully!");
    return HC_SUCCESS;
}

int32_t DelGroup(int32_t osAccountId, const QueryGroupParams *params)
{
    LOGI("[DB]: Start to delete groups from database!");
    if (params == NULL) {
        LOGE("[DB]: The input params is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t count = 0;
    uint32_t index = 0;
    TrustedGroupEntry **entry = NULL;
    while (index < HC_VECTOR_SIZE(&info->groups)) {
        entry = info->groups.getp(&info->groups, index);
        if ((entry == NULL) || (*entry == NULL) || (!CompareQueryGroupParams(params, *entry))) {
            index++;
            continue;
        }
        TrustedGroupEntry *popEntry;
        HC_VECTOR_POPELEMENT(&info->groups, &popEntry, index);
        PostGroupDeletedMsg(popEntry);
        LOGI("[DB]: Delete a group from database successfully! [GroupType]: %d", popEntry->type);
        DestroyGroupEntry(popEntry);
        count++;
    }
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Number of groups deleted: %d", count);
    return HC_SUCCESS;
}

int32_t DelTrustedDevice(int32_t osAccountId, const QueryDeviceParams *params)
{
    LOGI("[DB]: Start to delete devices from database!");
    if (params == NULL) {
        LOGE("[DB]: The input params is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t count = 0;
    uint32_t index = 0;
    TrustedDeviceEntry **entry = NULL;
    while (index < HC_VECTOR_SIZE(&info->devices)) {
        entry = info->devices.getp(&info->devices, index);
        if ((entry == NULL) || (*entry == NULL) || (!CompareQueryDeviceParams(params, *entry))) {
            index++;
            continue;
        }
        TrustedDeviceEntry *popEntry;
        HC_VECTOR_POPELEMENT(&info->devices, &popEntry, index);
        PostDeviceUnBoundMsg(info, popEntry);
        LOGI("[DB]: Delete a trusted device from database successfully!");
        DestroyDeviceEntry(popEntry);
        count++;
    }
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Number of trusted devices deleted: %d", count);
    return HC_SUCCESS;
}

int32_t QueryGroups(int32_t osAccountId, const QueryGroupParams *params, GroupEntryVec *vec)
{
    if ((params == NULL) || (vec == NULL)) {
        LOGE("[DB]: The input params or vec is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    uint32_t index;
    TrustedGroupEntry **entry;
    FOR_EACH_HC_VECTOR(info->groups, index, entry) {
        if ((entry == NULL) || (*entry == NULL) || (!CompareQueryGroupParams(params, *entry))) {
            continue;
        }
        TrustedGroupEntry *newEntry = DeepCopyGroupEntry(*entry);
        if (newEntry == NULL) {
            continue;
        }
        if (vec->pushBackT(vec, newEntry) == NULL) {
            LOGE("[DB]: Failed to push entry to vec!");
            DestroyGroupEntry(newEntry);
        }
    }
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Number of groups queried: %d", vec->size(vec));
    return HC_SUCCESS;
}

int32_t QueryDevices(int32_t osAccountId, const QueryDeviceParams *params, DeviceEntryVec *vec)
{
    if ((params == NULL) || (vec == NULL)) {
        LOGE("[DB]: The input params or vec is NULL!");
        return HC_ERR_NULL_PTR;
    }
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    uint32_t index;
    TrustedDeviceEntry **entry;
    FOR_EACH_HC_VECTOR(info->devices, index, entry) {
        if ((entry == NULL) || (*entry == NULL) || (!CompareQueryDeviceParams(params, *entry))) {
            continue;
        }
        TrustedDeviceEntry *newEntry = DeepCopyDeviceEntry(*entry);
        if (newEntry == NULL) {
            continue;
        }
        if (vec->pushBackT(vec, newEntry) == NULL) {
            LOGE("[DB]: Failed to push entry to vec!");
            DestroyDeviceEntry(newEntry);
        }
    }
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Number of trusted devices queried: %d", vec->size(vec));
    return HC_SUCCESS;
}

int32_t SaveOsAccountDb(int32_t osAccountId)
{
    g_databaseMutex->lock(g_databaseMutex);
    OsAccountTrustedInfo *info = GetTrustedInfoByOsAccountId(osAccountId);
    if (info == NULL) {
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_INVALID_PARAMS;
    }
    HcParcel parcel = CreateParcel(0, 0);
    if (!SaveInfoToParcel(info, &parcel)) {
        DeleteParcel(&parcel);
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_MEMORY_COPY;
    }
    if (!SaveParcelToFile(info, &parcel)) {
        DeleteParcel(&parcel);
        g_databaseMutex->unlock(g_databaseMutex);
        return HC_ERR_MEMORY_COPY;
    }
    DeleteParcel(&parcel);
    g_databaseMutex->unlock(g_databaseMutex);
    LOGI("[DB]: Save an os account database successfully! [Id]: %d", osAccountId);
    return HC_SUCCESS;
}

int32_t InitDatabase(void)
{
    if (g_databaseMutex == NULL) {
        g_databaseMutex = (HcMutex *)HcMalloc(sizeof(HcMutex), 0);
        if (g_databaseMutex == NULL) {
            LOGE("[DB]: Alloc databaseMutex failed");
            return HC_ERR_ALLOC_MEMORY;
        }
        if (InitHcMutex(g_databaseMutex) != HC_SUCCESS) {
            LOGE("[DB]: Init mutex failed");
            HcFree(g_databaseMutex);
            g_databaseMutex = NULL;
            return HC_ERROR;
        }
    }
    g_deviceauthDb = CREATE_HC_VECTOR(DeviceAuthDb);
    LoadDeviceAuthDb();
    return HC_SUCCESS;
}

void DestroyDatabase(void)
{
    g_databaseMutex->lock(g_databaseMutex);
    uint32_t index;
    OsAccountTrustedInfo *info;
    FOR_EACH_HC_VECTOR(g_deviceauthDb, index, info) {
        ClearGroupEntryVec(&info->groups);
        ClearDeviceEntryVec(&info->devices);
    }
    DESTROY_HC_VECTOR(DeviceAuthDb, &g_deviceauthDb);
    g_databaseMutex->unlock(g_databaseMutex);
    if (g_databaseMutex != NULL) {
        DestroyHcMutex(g_databaseMutex);
        HcFree(g_databaseMutex);
        g_databaseMutex = NULL;
    }
}