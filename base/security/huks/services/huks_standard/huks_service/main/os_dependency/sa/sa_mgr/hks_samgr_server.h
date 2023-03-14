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

#ifndef HKS_SAMGR_SERVER_H
#define HKS_SAMGR_SERVER_H

#include <stdio.h>

#include <ohos_init.h>

#include "iproxy_server.h"
#include "samgr_lite.h"
#include "serializer.h"
#include "service.h"

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_request.h"

#define HKS_SAMGR_SERVICE "hks_service"
#define HKS_SAMGR_FEATRURE "hks_feature"

typedef struct {
    IpcIo *reply;
    uint32_t callingUid;
} HksIpcContext;

typedef struct {
    INHERIT_SERVER_IPROXY;
} HksMgrFeatureApi;

typedef struct {
    INHERIT_SERVICE;
    Identity identity;
} HksMgrService;

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(HksMgrFeatureApi);
    Identity identity;
    Service *parent;
} HksMgrFeature;

#endif