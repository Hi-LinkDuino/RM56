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

#ifndef DMS_TEST_UTILS
#define DMS_TEST_UTILS

#include <cstdio>
#include <cstdlib>
#include <string>
#include "samgr_lite.h"

#include "dmslite_msg_parser.h"
#include "dmslite_tlv_common.h"

const int MS2US = 1000;
const int OPER_INTERVAL = 200;

const int PRESSURE_LEVEL0 = 10;
const int PRESSURE_LEVEL1 = 1024;
const int PRESSURE_LEVEL2 = 1024 * 10;

BOOL SystemInitProxy();

long long GetSystemTime();

std::string GetStringByLen(int len);

BOOL InstallHap();
BOOL UninstallHap();

#endif