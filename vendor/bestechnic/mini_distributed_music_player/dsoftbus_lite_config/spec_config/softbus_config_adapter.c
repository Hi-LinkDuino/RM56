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

#include "softbus_config_adapter.h"

#define MAX_BYTES_LENGTH 4194304

void SoftbusConfigAdapterInit(const ConfigSetProc *sets)
{
    int32_t val;
    val = MAX_BYTES_LENGTH;
    sets->SetConfig(SOFTBUS_INT_MAX_BYTES_LENGTH, (unsigned char*)&val, sizeof(val));
    val = 0x1;
    sets->SetConfig(SOFTBUS_INT_AUTH_ABILITY_COLLECTION, (unsigned char*)&val, sizeof(val));

    val = 3; // SOFTBUS_LOG_ERROR
    sets->SetConfig(SOFTBUS_INT_ADAPTER_LOG_LEVEL, (unsigned char*)&val, sizeof(val));
    val = 0;
    sets->SetConfig(SOFTBUS_INT_AUTO_NETWORKING_SWITCH, (unsigned char*)&val, sizeof(val));
}
