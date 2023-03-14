/*
 * Copyright (c) 2021 Shenzhen Kaihong DID Co., Ltd.
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

#include <gtest/gtest.h>
#include "hdf_io_service_if.h"
#include "codec_config_reader.h"
#include "codec_utils.h"
#include "codec_type.h"

using namespace std;
using namespace testing::ext;

namespace {
class CodecConfigTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(CodecConfigTest, CodecConfigTest_001, TestSize.Level0)
{
    struct HdfRemoteService *remote = GetConfigService();
    ASSERT_TRUE(remote != nullptr);

    for (int index = 0; index < 9; index++) {
        CodecCapbility cap;
        if (EnumrateCapability(remote, index, &cap) == HDF_SUCCESS) {
            PrintCapability("codec_config_utest", &cap);
        }
    }
}

HWTEST_F(CodecConfigTest, CodecConfigTest_002, TestSize.Level0)
{
    struct HdfRemoteService *remote = GetConfigService();
    ASSERT_TRUE(remote != nullptr);

    CodecCapbility cap;
    if (GetCapability(remote, MEDIA_MIMETYPE_VIDEO_HEVC, VIDEO_ENCODER, 0, &cap) == HDF_SUCCESS) {
        PrintCapability("codec_config_utest", &cap);
    }
}
}
