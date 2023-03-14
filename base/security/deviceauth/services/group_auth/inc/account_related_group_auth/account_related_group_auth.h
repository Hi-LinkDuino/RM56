/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef ACCOUNT_RELATED_GROUP_AUTH_H
#define ACCOUNT_RELATED_GROUP_AUTH_H

#include <stdint.h>
#include "base_group_auth.h"
#include "data_manager.h"

typedef void (*GetAccountCandidateGroupFunc)(const CJson *param, const QueryGroupParams *queryParams,
    GroupEntryVec *vec);

typedef struct {
    BaseGroupAuth base;
    GetAccountCandidateGroupFunc getAccountCandidateGroup;
} AccountRelatedGroupAuth;

#ifdef __cplusplus
extern "C" {
#endif
BaseGroupAuth *GetAccountRelatedGroupAuth(void);
#ifdef __cplusplus
}
#endif
#endif
