/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_ACCESS_TOKEN_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_ACCESS_TOKEN_H

namespace OHOS {
namespace Security {
namespace AccessToken {
typedef unsigned int AccessTokenID;
typedef unsigned int AccessTokenAttr;
static const int DEFAULT_TOKEN_VERSION = 1;
static const int FIRSTCALLER_TOKENID_DEFAULT = 0;

enum AccessTokenKitRet {
    RET_FAILED = -1,
    RET_SUCCESS = 0,
};

typedef struct {
    unsigned int tokenUniqueID : 24;
    unsigned int res : 3;
    unsigned int type : 2;
    unsigned int version : 3;
} AccessTokenIDInner;

typedef enum TypeATokenTypeEnum {
    TOKEN_INVALID = -1,
    TOKEN_HAP = 0,
    TOKEN_NATIVE,
} ATokenTypeEnum;

typedef enum TypeATokenAplEnum {
    APL_NORMAL = 1,
    APL_SYSTEM_BASIC = 2,
    APL_SYSTEM_CORE = 3,
} ATokenAplEnum;

typedef union {
    unsigned long long tokenIDEx;
    struct {
        AccessTokenID tokenID;
        AccessTokenAttr tokenAttr;
    } tokenIdExStruct;
} AccessTokenIDEx;

typedef enum TypePermissionState {
    PERMISSION_DENIED = -1,
    PERMISSION_GRANTED = 0,
} PermissionState;

typedef enum TypeGrantMode {
    USER_GRANT = 0,
    SYSTEM_GRANT = 1,
} GrantMode;

typedef enum TypePermissionFlag {
    PERMISSION_DEFAULT_FLAG = 0,
    PERMISSION_USER_SET = 1 << 0,
    PERMISSION_USER_FIXED = 1 << 1,
    PERMISSION_SYSTEM_FIXED = 1 << 2,
} PermissionFlag;

class PermissionDef final {
public:
    std::string permissionName;
    std::string bundleName;
    int grantMode;
    TypeATokenAplEnum availableLevel;
    bool provisionEnable;
    bool distributedSceneEnable;
    std::string label;
    int labelId;
    std::string description;
    int descriptionId;
};

class PermissionStateFull final {
public:
    std::string permissionName;
    bool isGeneral;
    std::vector<std::string> resDeviceID;
    std::vector<int> grantStatus;
    std::vector<int> grantFlags;
};

class HapInfoParams final {
public:
    int userID;
    std::string bundleName;
    int instIndex;
    std::string appIDDesc;
    int dlpType;
};

class HapPolicyParams final {
public:
    ATokenAplEnum apl;
    std::string domain;
    std::vector<PermissionDef> permList;
    std::vector<PermissionStateFull> permStateList;
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_ACCESS_TOKEN_H