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
#ifndef ACELITE_JSFWK_TEST_COMMON_H
#define ACELITE_JSFWK_TEST_COMMON_H

#ifdef TDD_ASSERTIONS
#define TDD_CASE_BEGIN() void()
#define TDD_CASE_END() void()
#else
#define TDD_CASE_BEGIN() SetUp()
#define TDD_CASE_END() TearDown()
#define EXPECT_STREQ(arg1, args2) void()
#define EXPECT_TRUE(arg1) void()
#define EXPECT_EQ(arg1, arg2) void()
#define EXPECT_NE(arg1, arg2) void()
#define EXPECT_FALSE(arg1) void()
#define HWTEST_F(className, funcName, level) void className::funcName()
#endif
#endif // ACELITE_JSFWK_TEST_COMMON_H
