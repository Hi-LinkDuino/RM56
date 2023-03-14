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

class RenderImageFitOthersTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RenderImageFitOthersTest::SetUpTestCase() {}
void RenderImageFitOthersTest::TearDownTestCase() {}
void RenderImageFitOthersTest::SetUp() {}
void RenderImageFitOthersTest::TearDown() {}

/**
 * @tc.name: ImageFitOthers001
 * @tc.desc: Verify the implementation of imageFit : FILL with any alignment and combination of srcSize and dstSize,
 *           set this alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers001, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::FILL, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers002
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers002, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::CONTAIN, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers003
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers003, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::CONTAIN, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers004
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers004, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::CONTAIN, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers005
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers005, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::CONTAIN, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers006
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers006, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::CONTAIN, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers007
 * @tc.desc: Verify the implementation of imageFit : CONTAIN with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers007, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::CONTAIN, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(LARGE_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers008
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers008, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_LEFT, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers009
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers009, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers010
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignX takes effect, set alignment to CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers010, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers011
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers011, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers012
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers012, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers013
 * @tc.desc: Verify the implementation of imageFit : COVER with srcRatio <= dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers013, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::COVER, Size(LARGE_LENGTH, LARGE_LENGTH),
                       Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers014
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers014, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::FITWIDTH, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers015
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers015, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::FITWIDTH, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers016
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio > dstRatio, in which ratio = width / height.
 *           In this case, only alignY takes effect, set alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers016, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::FITWIDTH, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers017
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignY takes effect, set alignment to TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers017, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_RIGHT, ImageFit::FITWIDTH, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers018
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignY takes effect, set alignment to CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers018, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::CENTER_RIGHT, ImageFit::FITWIDTH, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, EXTRA_SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers019
 * @tc.desc: Verify the implementation of imageFit : FITWIDTH with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignY takes effect, set alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers019, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::FITWIDTH, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)),
        Rect(Offset(0.0, SMALL_LENGTH), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers020
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio > dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers020, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::FITHEIGHT, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers021
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio > dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers021, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::FITHEIGHT, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers022
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio > dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers022, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::FITHEIGHT, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers023
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers023, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_LEFT, ImageFit::FITHEIGHT, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers024
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers024, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_CENTER, ImageFit::FITHEIGHT, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(EXTRA_SMALL_LENGTH, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers025
 * @tc.desc: Verify the implementation of imageFit : FITHEIGHT with srcRatio <= dstRatio, in which ratio = width /
 *           height. In this case, only alignX takes effect, set alignment to BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers025, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::BOTTOM_RIGHT, ImageFit::FITHEIGHT, Size(SMALL_LENGTH, LARGE_LENGTH),
                       Size(MIDDLE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, LARGE_LENGTH)),
        Rect(Offset(SMALL_LENGTH, 0.0), Size(EXTRA_SMALL_LENGTH, SMALL_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers026
 * @tc.desc: Verify the implementation of imageFit : SCALEDOWN with srcHeight <= dstHeight and srcWidth <= dstWidth.
 *           In this kind of situation, we apply the same strategy as the implementation of imageFit : NONE.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers026, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::SCALEDOWN, Size(SMALL_LENGTH, MIDDLE_LENGTH),
                       Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, MIDDLE_LENGTH)));
}

/**
 * @tc.name: ImageFitOthers027
 * @tc.desc: Verify the implementation of imageFit : SCALEDOWN with srcHeight > dstHeight or srcWidth > dstWidth.
 *           In this kind of situation, we apply the same strategy as the implementation of imageFit : CONTAIN.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUO
 * @tc.author: chenxuankai
 */
HWTEST_F(RenderImageFitOthersTest, ImageFitOthers027, TestSize.Level1)
{
    VerifyImageFit(ImageFitConfig(Alignment::TOP_LEFT, ImageFit::SCALEDOWN, Size(LARGE_LENGTH, SMALL_LENGTH),
                       Size(SMALL_LENGTH, MIDDLE_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(LARGE_LENGTH, SMALL_LENGTH)),
        Rect(Offset(0.0, 0.0), Size(SMALL_LENGTH, EXTRA_SMALL_LENGTH)));
}

} // namespace OHOS::Ace
