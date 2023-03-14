/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include <cstdlib>
#include <memory>
#include <string>
#include "gtest/gtest.h"
#define private public
#define protected public
#include "foundation/cpp_ext/algorithm_ext.h"

namespace OHOS {
namespace Media {
namespace Test {
TEST(TestAnyOf, any_of_should_return_true_if_int_item_exists)
{
    std::vector<int> vec {1, 2, 3, 4};
    ASSERT_EQ(true, CppExt::AnyOf(std::begin(vec), std::end(vec), [](int item) { return item == 3; }));
}
TEST(TestAnyOf, any_of_should_return_true_if_string_item_exists)
{
    std::vector<std::string> vec {"one", "two", "three"};
    ASSERT_EQ(true, CppExt::AnyOf(std::begin(vec), std::end(vec),
                          [](const std::string& item) { return item == "two"; }));
}

TEST(TestAnyOf, any_of_should_return_false_if_item_not_exists)
{
    std::vector<int> vec {1, 2, 3, 4};
    ASSERT_EQ(false, CppExt::AnyOf(std::begin(vec), std::end(vec), [](int item) { return item == 8; }));
}
} // namespace Test
} // namespace Media
} // namespace OHOS
