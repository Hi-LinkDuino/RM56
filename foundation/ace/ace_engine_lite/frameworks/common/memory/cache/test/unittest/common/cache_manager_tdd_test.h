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

#ifndef OHOS_ACELITE_CACHE_MANAGER_TDD_TEST_H
#define OHOS_ACELITE_CACHE_MANAGER_TDD_TEST_H

#include "acelite_config.h"
#include "cache_config.h"
#include "cache_manager.h"
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#endif
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class CacheManagerTddTest : public testing::Test {
#else
class CacheManagerTddTest {
#endif
public:
    CacheManagerTddTest() = default;
    ~CacheManagerTddTest() = default;
    void SetUp();
    void CacheDistribute001();
    void CacheDistribute002();
    void CacheDistribute003();
    void CacheDistribute004();
    void CacheDistribute005();
    void CacheDistribute006();
    void RunTests();

private:
    static const CacheUnit TEST_CONFIG_TABLE[];
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_CACHE_MANAGER_TDD_TEST_H
