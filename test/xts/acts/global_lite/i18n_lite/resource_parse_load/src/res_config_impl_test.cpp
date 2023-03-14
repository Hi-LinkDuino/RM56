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
#include "res_config_impl_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "res_config_impl.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResConfigImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResConfigImplTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResConfigImplTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResConfigImplTest::SetUp()
{
}

void ResConfigImplTest::TearDown()
{
}

ResConfigImpl *CreateResConfigImpl(const char *language, const char *script, const char *region)
{
    ResConfigImpl *resConfigImpl = new ResConfigImpl;
    resConfigImpl->SetLocaleInfo(language, script, region);
    return resConfigImpl;
}

/*
 * @tc.name: ResConfigImplMatchTest001
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest001, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest002
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest002, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest003
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest003, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("tl", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("fil", nullptr, "PH");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest004
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest004, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest005
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest005, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest006
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest006, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", "Latn", nullptr);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest007
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest007, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, "IR");
    ResConfigImpl *current = CreateResConfigImpl("az", "Arab", nullptr);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest008
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest008, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "TN");
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest009
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest009, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", "Latn", "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest010
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest010, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", "Latn", "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest011
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest011, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", "Cyrl", nullptr);
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest012
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest012, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("az", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("az", nullptr, "IR");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest013
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest013, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("qaa", nullptr, "FR");
    ResConfigImpl *current = CreateResConfigImpl("qaa", nullptr, "CA");
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest014
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest014, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("he", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("iw", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest015
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest015, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("ji", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("yi", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest016
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest016, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("jw", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("jv", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest017
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest017, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("in", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("id", nullptr, nullptr);
    EXPECT_TRUE(current->Match(other));
    EXPECT_TRUE(other->Match(current));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest018
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest018, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", "Latn", nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", "Qaag", nullptr);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest019
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest019, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_CAR);
    other->SetDeviceType(DeviceType::DEVICE_CAR);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest020
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest020, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_CAR);
    other->SetDeviceType(DeviceType::DEVICE_PC);
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest021
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest021, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    other->SetDeviceType(DeviceType::DEVICE_PC);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest022
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest022, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDeviceType(DeviceType::DEVICE_PC);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest023
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest023, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest024
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest024, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetDirection(Direction::DIRECTION_VERTICAL);
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest025
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest025, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest026
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest026, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest027
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest027, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_VERTICAL);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest028
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest028, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplMatchTest029
 * @tc.desc: Test ResConfig Match
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplMatchTest029, TestSize.Level1)
{
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    current->SetDirection(Direction::DIRECTION_VERTICAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_FALSE(current->Match(other));
    delete current;
    delete other;
};

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest001
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest001, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "FR");
    ResConfigImpl *other = CreateResConfigImpl("fr", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest002
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest002, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fr", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest003
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest003, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fr", nullptr, "CA");
    ResConfigImpl *current = CreateResConfigImpl("fr", nullptr, "FR");
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest004
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest004, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("tl", nullptr, "PH");
    ResConfigImpl *other = CreateResConfigImpl("fil", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest005
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest005, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *current = CreateResConfigImpl("fil", nullptr, "PH");
    ResConfigImpl *other = CreateResConfigImpl("tl", nullptr, "PH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest006
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest006, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest007
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest007, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest008
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest008, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest009
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest009, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "419");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest010
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest010, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest011
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest011, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "PE");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "ES");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest012
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest012, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest013
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest013, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "AR");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "BO");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest014
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest014, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "IC");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "ES");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "GQ");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest015
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest015, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("es", nullptr, "GQ");
    ResConfigImpl *current = CreateResConfigImpl("es", nullptr, "IC");
    ResConfigImpl *other = CreateResConfigImpl("es", nullptr, "419");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest016
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest016, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "001");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest017
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest017, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest018
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest018, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "DE");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "150");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest019
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest019, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "AU");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest020
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest020, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "001");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest021
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest021, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "AU");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest022
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest022, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "IN");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "AU");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest023
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest023, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("pt", nullptr, "MZ");
    ResConfigImpl *current = CreateResConfigImpl("pt", nullptr, "PT");
    ResConfigImpl *other = CreateResConfigImpl("pt", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest024
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest024, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("pt", nullptr, "MZ");
    ResConfigImpl *current = CreateResConfigImpl("pt", nullptr, "PT");
    ResConfigImpl *other = CreateResConfigImpl("pt", nullptr, "BR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest025
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest025, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("zh", "Hant", "MO");
    ResConfigImpl *current = CreateResConfigImpl("zh", "Hant", "HK");
    ResConfigImpl *other = CreateResConfigImpl("zh", "Hant", "TW");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest026
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest026, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("zh", "Hant", "US");
    ResConfigImpl *current = CreateResConfigImpl("zh", "Hant", "TW");
    ResConfigImpl *other = CreateResConfigImpl("zh", "Hant", "HK");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest027
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest027, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "DZ");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "015");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest028
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest028, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest029
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest029, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "QA");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "EG");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "BH");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest030
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest030, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("ar", nullptr, "QA");
    ResConfigImpl *current = CreateResConfigImpl("ar", nullptr, "SA");
    ResConfigImpl *other = CreateResConfigImpl("ar", nullptr, "015");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest031
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest031, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest032
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest032, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest033
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest033, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "001");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest034
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest034, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest035
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest035, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "PR");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl(nullptr, nullptr, nullptr);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest036
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest036, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "PR");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest037
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest037, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "CN");
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest038
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest038, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", "Qaag", nullptr);
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "GB");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest039
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest039, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", "Qaag", nullptr);
    ResConfigImpl *current = CreateResConfigImpl(nullptr, nullptr, nullptr);
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "CA");
    EXPECT_FALSE(current->IsMoreSuitable(other, request));
    EXPECT_TRUE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest040
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest040, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "CN");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "GB");
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest041
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest041, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest042
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest042, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest043
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest043, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest044
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest044, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest045
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest045, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XLDPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XXLDPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest046
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest046, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_MDPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest047
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest047, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    request->SetDirection(Direction::DIRECTION_HORIZONTAL);
    request->SetDeviceType(DeviceType::DEVICE_PHONE);
    request->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_LDPI);
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_XXXLDPI);
    other->SetDeviceType(DeviceType::DEVICE_PHONE);
    other->SetDirection(Direction::DIRECTION_HORIZONTAL);
    other->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest048
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest048, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    current->SetDeviceType(DeviceType::DEVICE_PHONE);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest049
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest049, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    current->SetDirection(Direction::DIRECTION_HORIZONTAL);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}

/*
 * @tc.name: ResConfigImplIsMoreSuitableTest050
 * @tc.desc: Test ResConfig IsMoreSuitable
 * @tc.type: FUNC
 */
HWTEST_F(ResConfigImplTest, ResConfigImplIsMoreSuitableTest050, TestSize.Level1)
{
    ResConfigImpl *request = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *current = CreateResConfigImpl("en", nullptr, "US");
    ResConfigImpl *other = CreateResConfigImpl("en", nullptr, "US");
    current->SetScreenDensity(ScreenDensity::SCREEN_DENSITY_SDPI);
    EXPECT_TRUE(current->IsMoreSuitable(other, request));
    EXPECT_FALSE(other->IsMoreSuitable(current, request));
    delete request;
    delete current;
    delete other;
}