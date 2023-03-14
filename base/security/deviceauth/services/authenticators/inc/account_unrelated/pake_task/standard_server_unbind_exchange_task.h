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

#ifndef STANDARD_SERVER_UNBIND_EXCHANGE_TASK_H
#define STANDARD_SERVER_UNBIND_EXCHANGE_TASK_H

#include "pake_base_cur_task.h"
#include "common_standard_unbind_exchange.h"

typedef struct {
    AsyBaseCurTask taskBase;
    StandardUnbindExchangeParams params;
} StandardUnbindExchangeServerTask;

AsyBaseCurTask *CreateStandardUnbindExchangeServerTask(void);

#endif
