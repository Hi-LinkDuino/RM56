/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "imgdecode/image_load.h"

#include "gfx_utils/file.h"
#include "gfx_utils/mem_api.h"
#include "test_resource_config.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace {
constexpr uint8_t MAX_FILE_NUM = 2;
}

namespace OHOS {
class ImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static void ResetFds();
    static void InitImageInfo(ImageInfo& info);

    static int32_t imgResFds_[MAX_FILE_NUM];
    static ImageInfo imgInfos_[MAX_FILE_NUM];
    static uint32_t sizes_[MAX_FILE_NUM];
};

int32_t ImageTest::imgResFds_[MAX_FILE_NUM];
ImageInfo ImageTest::imgInfos_[MAX_FILE_NUM];
uint32_t ImageTest::sizes_[MAX_FILE_NUM];

void ImageTest::SetUpTestCase()
{
    const char* paths[] = {
        IMAGE_DIR "A001_001.bin",
        IMAGE_DIR "A001_002.bin"
    };
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        imgResFds_[i] = open(paths[i], O_RDONLY);
        struct stat fileStat;
        fstat(imgResFds_[i], &fileStat);
        sizes_[i] = fileStat.st_size - sizeof(ImageHeader);
        read(imgResFds_[i], &imgInfos_[i].header, sizeof(ImageHeader));
    }
}

void ImageTest::TearDownTestCase()
{
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        close(imgResFds_[i]);
    }
}

void ImageTest::ResetFds()
{
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        lseek(imgResFds_[i], sizeof(ImageHeader), SEEK_SET);
    }
}

/**
 * @tc.name: Graphic_Image_Test_GetImageInfo_001
 * @tc.desc: Verify ImageLoad::GetImageInfo function, invalid param.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(ImageTest, Graphic_Image_Test_GetImageInfo_001, TestSize.Level0)
{
    /* test invalid fd */
    bool ret = ImageLoad::GetImageInfo(-1, sizes_[0], imgInfos_[0]);
    EXPECT_EQ(ret, false);

    /* test invalid size */
    ret = ImageLoad::GetImageInfo(imgResFds_[0], 0, imgInfos_[0]);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_Image_Test_GetImageInfo_002
 * @tc.desc: Verify ImageLoad::GetImageInfo function, invalid compress mode.
 * @tc.type: FUNC
 * @tc.require: SR000ERCR7
 */
HWTEST_F(ImageTest, Graphic_Image_Test_GetImageInfo_002, TestSize.Level0)
{
    ResetFds();

    auto info = imgInfos_[0];
    info.header.compressMode = COMPRESS_MODE_BLOCK_ALG;
    bool ret = ImageLoad::GetImageInfo(imgResFds_[0], sizes_[0], info);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_Image_Test_GetImageInfo_003
 * @tc.desc: Verify ImageLoad::GetImageInfo function, multi-images.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(ImageTest, Graphic_Image_Test_GetImageInfo_003, TestSize.Level0)
{
    ResetFds();

    for (int32_t i = 0; i < MAX_FILE_NUM; i++) {
        bool success = ImageLoad::GetImageInfo(imgResFds_[i], sizes_[i], imgInfos_[i]);
        if (success) {
            ImageCacheFree(imgInfos_[i]);
        } else {
            EXPECT_EQ(success, false);
        }
    }
}
} // namespace OHOS
