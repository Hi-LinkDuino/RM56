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

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <stdbool.h>
#include "hc_string.h"
#include "hc_string_vector.h"
#include "hc_tlv_parser.h"
#include "hc_vector.h"

#define MAX_STRING_LEN 256
#define MAX_EXPIRE_TIME 90
#define HC_TRUST_DEV_ENTRY_MAX_NUM 101
#define HC_TRUST_GROUP_ENTRY_MAX_NUM 100

typedef struct {
    HcString name; /* group name */
    HcString id; /* group id */
    int32_t type; /* including identical account group(1), peer to peer group(256), across account group(1282) */
    int32_t visibility; /* visibility of the group */
    int32_t expireTime; /* the time of group expired, unit day, user config */
    HcString userIdHash; /* hash result of the user account id */
    StringVector sharedUserIdHashVec; /* vector of hash results for shared user account id */
    StringVector managers; /* group manager vector, group manager can add and delete members, index 0 is the owner */
    StringVector friends; /* group friend vector, group friend can query group information */
} TrustedGroupEntry;
DECLARE_HC_VECTOR(GroupEntryVec, TrustedGroupEntry*)

typedef struct {
    TrustedGroupEntry *groupEntry;
    HcString groupId;
    HcString udid; /* unique device id */
    HcString authId; /* id by service defined for authentication */
    HcString serviceType; /* compatible with previous versions, the value is the same as groupId */
    HcParcel ext; /* for caching extern data, user data */
    uint8_t credential; /* 1 - asymmetrical, 2 - symmetrical */
    uint8_t devType; /* 0 - accessory, 1 - controller, 2 - proxy */
    HcString userIdHash; /* hash result of the user account id */
    uint64_t lastTm; /* accessed time of the device of the auth information, absolute time */
} TrustedDeviceEntry;
DECLARE_HC_VECTOR(DeviceEntryVec, TrustedDeviceEntry*)

typedef struct {
    const char *groupId;
    const char *groupName;
    const char *ownerName;
    int32_t groupType;
    int32_t groupVisibility;
} QueryGroupParams;

typedef struct {
    const char *groupId;
    const char *udid;
    const char *authId;
} QueryDeviceParams;

#ifdef __cplusplus
extern "C" {
#endif

int32_t InitDatabase(void);
void DestroyDatabase(void);

int32_t AddGroup(int32_t osAccountId, const TrustedGroupEntry *groupEntry);
int32_t DelGroup(int32_t osAccountId, const QueryGroupParams *params);
int32_t AddTrustedDevice(int32_t osAccountId, const TrustedDeviceEntry *deviceEntry);
int32_t DelTrustedDevice(int32_t osAccountId, const QueryDeviceParams *params);
int32_t QueryGroups(int32_t osAccountId, const QueryGroupParams *params, GroupEntryVec *vec);
int32_t QueryDevices(int32_t osAccountId, const QueryDeviceParams *params, DeviceEntryVec *vec);
int32_t SaveOsAccountDb(int32_t osAccountId);
bool GenerateGroupEntryFromEntry(const TrustedGroupEntry *entry, TrustedGroupEntry *returnEntry);
bool GenerateDeviceEntryFromEntry(const TrustedDeviceEntry *entry, TrustedDeviceEntry *returnEntry);

TrustedGroupEntry *DeepCopyGroupEntry(const TrustedGroupEntry *entry);
TrustedDeviceEntry *DeepCopyDeviceEntry(const TrustedDeviceEntry *entry);

QueryGroupParams InitQueryGroupParams(void);
QueryDeviceParams InitQueryDeviceParams(void);

TrustedGroupEntry *CreateGroupEntry(void);
TrustedDeviceEntry *CreateDeviceEntry(void);
void DestroyGroupEntry(TrustedGroupEntry *groupEntry);
void DestroyDeviceEntry(TrustedDeviceEntry *groupEntry);
GroupEntryVec CreateGroupEntryVec(void);
DeviceEntryVec CreateDeviceEntryVec(void);
void ClearGroupEntryVec(GroupEntryVec *vec);
void ClearDeviceEntryVec(DeviceEntryVec *vec);

#ifdef __cplusplus
}
#endif
#endif
