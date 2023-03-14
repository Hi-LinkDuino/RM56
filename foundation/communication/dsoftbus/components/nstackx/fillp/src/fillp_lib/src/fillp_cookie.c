/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "utils.h"
#include "fillp_function.h"
#include "fillp_cookie.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
    Function       : FillpMacTimerExpire

    Description    : This function initializes mac key peroiodically

    Input          : NONE

    Output         : NONE

    Return         : SUCCESS/FAILURE
******************************************************************************/
void FillpMacTimerExpire(
    FillpMacInfo *macInfo,
    FILLP_LLONG curTime)
{
    FILLP_UINT32 counter;

    for (counter = 0; counter < FILLP_KEYSIZE; counter += (FILLP_UINT32)sizeof(FILLP_UINT32)) {
        macInfo->oldMacKey[counter] = macInfo->currentMacKey[counter];

        macInfo->currentMacKey[counter] = (FILLP_UINT8)FILLP_CRYPTO_RAND();
    }

    macInfo->switchOverTime = (FILLP_ULLONG)curTime;
    return;
}


#ifdef __cplusplus
}
#endif

