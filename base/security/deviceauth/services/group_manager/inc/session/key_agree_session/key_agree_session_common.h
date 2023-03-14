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

#ifndef KEY_AGREE_SESSION_COMMON_H
#define KEY_AGREE_SESSION_COMMON_H

#include "bind_session_common_defines.h"

int32_t ProcessKeyAgreeSession(Session *session, CJson *jsonParams);
int32_t SaveReceivedKeyAgreeData(BindSession *session, const CJson *jsonParams);
int32_t PrepareAndSendKeyAgreeData(BindSession *session, bool *isNeedInform);
#endif
