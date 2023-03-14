/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "bind_peer.h"

#include "common_defs.h"

int32_t RequestBindPeer(int64_t requestId, const char *appId, const char *bindParams)
{
    (void)requestId;
    (void)appId;
    (void)bindParams;
    return HC_ERR_NOT_SUPPORT;
}

int32_t RequestUnbindPeer(int64_t requestId, const char *appId, const char *unbindParams)
{
    (void)requestId;
    (void)appId;
    (void)unbindParams;
    return HC_ERR_NOT_SUPPORT;
}

int32_t RequestProcessLiteData(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen)
{
    (void)requestId;
    (void)appId;
    (void)data;
    (void)dataLen;
    return HC_ERR_NOT_SUPPORT;
}