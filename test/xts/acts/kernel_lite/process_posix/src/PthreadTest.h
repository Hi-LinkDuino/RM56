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

#ifndef KERNEL_LITE_PTHREAD_TEST_H
#define KERNEL_LITE_PTHREAD_TEST_H

#include <gtest/gtest.h>

#define FIFO_PATH "/dev/xtsTestFifo"

// Thread default value
#define DEF_PROCESS_DETACHSTATE PTHREAD_CREATE_JOINABLE
#define DEF_PROCESS_STACK_SIZE 131072
#define DEF_PROCESS_GUARD_SIZE 8192

class PthreadTest : public::testing::Test {
};

void *ThreadPublic(void *arg);

#endif
