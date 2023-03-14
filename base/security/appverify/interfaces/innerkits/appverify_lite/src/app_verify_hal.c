/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "app_verify_hal.h"
#include "app_verify_base.h"
#include "app_verify_default.h"

static ProductDiff g_productDiffFunc;

void RegistHalFunc()
{
    RegistBaseDefaultFunc(&g_productDiffFunc);
    RegistProductFunc(&g_productDiffFunc);
}

int InquiryDeviceUdid(unsigned char *udid, int size)
{
    if (g_productDiffFunc.devUdidFunc == NULL) {
        return INQUIRY_UDID_ERROR;
    }
    return g_productDiffFunc.devUdidFunc(udid, size);
}
