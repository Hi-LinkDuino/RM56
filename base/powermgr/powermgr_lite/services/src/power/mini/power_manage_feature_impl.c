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

#include <iunknown.h>

#include "power_manage_feature.h"

static PowerManageFeature g_feature = {
    POWER_MANAGE_FEATURE_INTERFACE_IMPL,
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    POWER_MANAGE_INTERFACE_IMPL,
    DEFAULT_IUNKNOWN_ENTRY_END,
    .identity = { -1, -1, NULL },
};

PowerManageFeature *GetPowerManageFeatureImpl(void)
{
    return &g_feature;
}
