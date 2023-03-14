/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_saver_handler.h"

static OHOS::ScreenSaverHandler g_handler;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void ScreenSaverMgrInit(void)
{
    g_handler.Init();
}

int32_t ScreenSaverMgrSetState(BOOL enable)
{
    return g_handler.SetState(enable == TRUE);
}

#ifdef __cplusplus
}
#endif // __cplusplus
