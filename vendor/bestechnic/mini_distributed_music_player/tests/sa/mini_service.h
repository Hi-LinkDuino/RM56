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

#ifndef HOS_LITE_HIVIEW_SERVICE_H
#define HOS_LITE_HIVIEW_SERVICE_H

#include <service.h>
#include <iunknown.h>
#include "ohos_types.h"
#include "iproxy_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define SYNC_FILE 1

typedef struct {
    INHERIT_SERVER_IPROXY;
} MiniInterface;

typedef struct {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(MiniInterface);
    Identity identity;
} MiniService;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_SERVICE_H */
