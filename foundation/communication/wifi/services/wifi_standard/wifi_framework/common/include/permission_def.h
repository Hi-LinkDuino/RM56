/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_PERMISSION_DEF_H
#define OHOS_PERMISSION_DEF_H

#define PERMISSION_NAME_LEN 96

namespace OHOS {
namespace Wifi {
enum GrantType { USER_GRANT = 0, SYSTEM_GRANT = 1 };

enum IsUpdate { FIRST_INSTALL = 0, UPDATE = 1 };

enum IsRestricted { RESTRICTED = 0, NOT_RESTRICTED = 1 };

enum IsGranted {
    PERMISSION_DENIED = 0,  /* Not granted */
    PERMISSION_GRANTED = 1, /* Granted */
    PERMISSION_FAILED = 2   /* Failed to judge permission */
};

typedef struct {
    char name[PERMISSION_NAME_LEN];
    enum GrantType type;
    enum IsRestricted isRestricted;
} PermissionDef;
}  // namespace Wifi
}  // namespace OHOS
#endif
