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

#include "key_agree_session_client.h"
#include "hc_log.h"

Session *CreateClientKeyAgreeSession(CJson *jsonParams, const DeviceAuthCallback *callback)
{
    LOGE("Key agreement session is not supported!");
    (void)jsonParams;
    (void)callback;
    return NULL;
}