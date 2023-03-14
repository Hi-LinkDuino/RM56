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

#include "gtest/gtest.h"

#include "core/components/test/unittest/image/image_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderImageFitNoneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RenderImageFitNoneTest::SetUpTestCase() {}
void RenderImageFitNoneTest::TearDownTestCase() {}
void RenderImageFitNoneTest::SetUp() {}
void RenderImageFitNoneTest::TearDown() {}

/**
 * @tc.name: ImageFitNone001
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone001, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone002
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone002, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_CENTER, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone003
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone003, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone004
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone004, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone005
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone005, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone006
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone006, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, EXTRA_SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone007
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone007, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone008
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone008, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone009
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone009, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::NONE, Size(EXTRA_LARGE_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, SMALL_LENGTH), Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(MIDDLE_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone010
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone010, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone011
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone011, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_CENTER, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone012
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone012, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone013
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone013, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone014
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone014, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone015
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone015, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone016
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone016, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone017
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone017, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone018
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight > dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone018, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone019
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone019, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone020
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone020, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_CENTER, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone021
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone021, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone022
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone022, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone023
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone023, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone024
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone024, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone025
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone025, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone026
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone026, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone027
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth > dstWidth,
 *           alignment : BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone027, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::NONE, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitNone028
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone028, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone029
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone029, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_CENTER, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone030
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone030, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone031
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone031, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone032
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone032, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone033
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone033, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone034
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone034, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone035
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone035, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitNone036
 * @tc.desc: Verify the implementation of imageFit : NONE when srcHeight <= dstHeight and srcWidth <= dstWidth,
 *           alignment : BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitNoneTest, ImageFitNone036, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::NONE, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, SMALL_LENGTH), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

} // namespace OHOS::Ace
