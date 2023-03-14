/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include <memory>
#include "common/any.h"

#include <gtest/gtest.h>

#include <test_helper.h>
#include "pipeline/core/filter_base.h"

using namespace OHOS::Media::Pipeline;

namespace OHOS::Media::Test {
class UtTestFilter : public ::testing::Test {
public:
    std::shared_ptr<InPort> port1 = std::make_shared<InPort>(nullptr, "port1");
    std::shared_ptr<InPort> port2 = std::make_shared<InPort>(nullptr, "port2");
    std::vector<PInPort> list {port1, port2};
    virtual void SetUp() override
    {
    }
    virtual void TearDown() override
    {
    }
};

TEST_F(UtTestFilter, Can_find_port_if_it_exists)
{
    ASSERT_EQ(port2, FilterBase::FindPort(list, "port2"));
}

TEST_F(UtTestFilter, Can_not_find_port_if_it_does_not_exists)
{
    ASSERT_TRUE(nullptr == FilterBase::FindPort(list, "port3"));
}
}