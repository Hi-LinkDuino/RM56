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

#ifndef KERNEL_LITE_FUTEX_TEST_H
#define KERNEL_LITE_FUTEX_TEST_H

#include <gtest/gtest.h>

class FutexTest : public testing::Test {
};

#define DEF_PROCESS_CONDATTR_CLOCK CLOCK_REALTIME

void ChildAssertEQ(int src, int des);
int ChildExpectEQ(int src, int des);

#endif