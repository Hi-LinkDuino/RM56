/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UTEST_INPUT_HDI_BASE_H
#define UTEST_INPUT_HDI_BASE_H

#include <thread>
#include <unistd.h>
#include <vector>
#include <map>
#include <gtest/gtest.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "securec.h"
#include "iservice_registry.h"

class InputHdiBaseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);

    void SetUp(void);
    void TearDown(void);
};
#endif // UTEST_INPUT_HDI_BASE_H
