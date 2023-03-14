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

#include "audio_manager_test.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "audio_internal.h"

using namespace std;
using namespace testing::ext;
namespace {
class AudioManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void AudioManagerTest::SetUpTestCase()
{
}

void AudioManagerTest::TearDownTestCase()
{
}

HWTEST_F(AudioManagerTest, AudioManagerGetAllAdaptersWhenManagerIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    int32_t size = 0;
    struct AudioManager *manager = nullptr;
    struct AudioAdapterDescriptor *descs;
    int32_t ret = managerFuncs->GetAllAdapters(manager, &descs, &size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerGetAllAdaptersWhenDescsIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    int32_t size = 0;
    int32_t ret = managerFuncs->GetAllAdapters(managerFuncs, nullptr, &size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerGetAllAdaptersWhenSizeIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    int32_t *size = nullptr;
    struct AudioAdapterDescriptor *descs;
    int32_t ret = managerFuncs->GetAllAdapters(managerFuncs, &descs, size);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerGetAllAdaptersWhenParamIsVaild, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs;
    int32_t ret = managerFuncs->GetAllAdapters(managerFuncs, &descs, &size);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenManagerIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    struct AudioManager *manager = nullptr;
    const struct AudioAdapterDescriptor *desc = new AudioAdapterDescriptor;
    struct AudioAdapter *adapter;
    int32_t ret = managerFuncs->LoadAdapter(manager, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    delete(desc);
    desc = nullptr;
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenDescIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    const struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapter *adapter;
    int32_t ret = managerFuncs->LoadAdapter(managerFuncs, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenAdapterNameIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    struct AudioAdapterDescriptor descTemp;
    descTemp.adapterName = nullptr;
    const struct AudioAdapterDescriptor *desc = &descTemp;
    struct AudioAdapter *adapter;
    int32_t ret = managerFuncs->LoadAdapter(managerFuncs, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenAdapterIsNull, TestSize.Level1)
{
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    const struct AudioAdapterDescriptor *desc = new AudioAdapterDescriptor;
    int32_t ret = managerFuncs->LoadAdapter(managerFuncs, desc, nullptr);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    delete(desc);
    desc = nullptr;
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenDescIsError, TestSize.Level1)
{
    int32_t ret;
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs;
    ret = managerFuncs->GetAllAdapters(managerFuncs, &descs, &size);
    ASSERT_EQ(HDF_SUCCESS, ret);

    struct AudioAdapterDescriptor *desc = &descs[size];
    static struct AudioAdapter *adapter;
    ret = managerFuncs->LoadAdapter(managerFuncs, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_OBJECT, ret);
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenAdapterNameIsError, TestSize.Level1)
{
    int32_t ret;
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs;
    ret = managerFuncs->GetAllAdapters(managerFuncs, &descs, &size);
    ASSERT_EQ(HDF_SUCCESS, ret);

    struct AudioAdapterDescriptor *desc = &descs[0];
    desc->adapterName = "abc";
    static struct AudioAdapter *adapter;
    ret = managerFuncs->LoadAdapter(managerFuncs, desc, &adapter);
    EXPECT_EQ(AUDIO_HAL_ERR_INVALID_PARAM, ret);
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}

HWTEST_F(AudioManagerTest, AudioManagerLoadAdapterWhenParamIsVaild, TestSize.Level1)
{
    int32_t ret;
    struct AudioManager *managerFuncs = GetAudioManagerFuncs();
    ASSERT_NE(managerFuncs, nullptr);
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs;
    ret = managerFuncs->GetAllAdapters(managerFuncs, &descs, &size);
    ASSERT_EQ(HDF_SUCCESS, ret);

    struct AudioAdapterDescriptor *desc = &descs[0];
    static struct AudioAdapter *adapter;
    ret = managerFuncs->LoadAdapter(managerFuncs, desc, &adapter);
    EXPECT_EQ(HDF_SUCCESS, ret);
    managerFuncs->UnloadAdapter(managerFuncs, adapter);
    adapter = nullptr;
}
}
