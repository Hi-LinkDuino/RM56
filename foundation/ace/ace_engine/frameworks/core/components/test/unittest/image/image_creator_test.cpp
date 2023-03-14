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

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/image/image_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/image_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double LEFT = -1.0;
constexpr double TOP = -1.0;
constexpr double ALIGNMENT_CENTER = 0.0;
constexpr double IMAGE_WIDTH_DEFAULT = -1.0;
constexpr double IMAGE_HEIGHT_DEFAULT = -1.0;
constexpr double IMAGE_WIDTH_JSON = 300.0;
constexpr double IMAGE_HEIGHT_JSON = 200.0;
const ImageRepeat IMAGE_REPEAT_JSON = ImageRepeat::REPEAT;
const ImageRepeat IMAGE_NO_REPEAT = ImageRepeat::NOREPEAT;
const Color IMAGE_COLOR_BLACK = Color(255);
const Color IMAGE_COLOR_TRANSPARENT = Color::TRANSPARENT;
const ImageFit IMAGE_FIT_JSON = ImageFit::CONTAIN;
const std::string IMAGE_ALT_BLANK_JSON = "blank";
const std::string IMAGE_SRC_JSON = "file://data/data/com.example.test/res/example.jpg";

} // namespace

class ImageCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ImageCreatorTest::SetUpTestCase() {}
void ImageCreatorTest::TearDownTestCase() {}
void ImageCreatorTest::SetUp() {}
void ImageCreatorTest::TearDown() {}

/**
 * @tc.name: ImageCreator001
 * @tc.desc: Verify that ImageCreator can parse json when all attributes are set.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPR AR000DAQUL AR000DAQUM AR000DAQUN AR000DAQUO AR000DAQUP AR000DAQUR
 * @tc.author: chenxuankai
 */
HWTEST_F(ImageCreatorTest, ImageCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Image with all attributes.
     */
    const std::string jsonImageStr = ""
                                     "{                                                                         "
                                     "  \"className\": \"Image\", \"width\": 300, \"height\": 200,              "
                                     "  \"alignment\": { \"horizontal\": \"left\", \"vertical\": \"top\" },     "
                                     "  \"color\": { \"className\": \"Color\", \"value\": 255 },                "
                                     "  \"imageFit\": \"contain\", \"imageRepeat\": 0, \"alt\": \"blank\",      "
                                     "  \"src\": \"file://data/data/com.example.test/res/example.jpg\"          "
                                     "}";
    /**
     * @tc.steps: step2. call CreateFromJson interface and get ImageComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonImageStr.c_str());
    size_t dslLen = jsonImageStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<ImageComponent> imageComponent = AceType::DynamicCast<ImageComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the attributes of ImageComponent is as set.
     * @tc.expected: step3. The attributes of ImageComponent is as set.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(NearEqual(imageComponent->GetWidth().Value(), IMAGE_WIDTH_JSON));
    ASSERT_TRUE(NearEqual(imageComponent->GetHeight().Value(), IMAGE_HEIGHT_JSON));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetHorizontal(), LEFT));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetVertical(), TOP));
    ASSERT_TRUE(imageComponent->GetColor() == IMAGE_COLOR_BLACK);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_JSON);
    ASSERT_TRUE(imageComponent->GetImageRepeat() == IMAGE_REPEAT_JSON);
    ASSERT_TRUE(imageComponent->GetSrc() == IMAGE_SRC_JSON);
    ASSERT_TRUE(imageComponent->GetAlt() == IMAGE_ALT_BLANK_JSON);
}

/**
 * @tc.name: ImageCreator002
 * @tc.desc: Verify that ImageCreator can parse json with no attributes set.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPR AR000DAQUL AR000DAQUM AR000DAQUN AR000DAQUO AR000DAQUP AR000DAQUR
 * @tc.author: chenxuankai
 */
HWTEST_F(ImageCreatorTest, ImageCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Image with no attributes.
     */
    const std::string jsonImageStr = ""
                                     "{                          "
                                     "  \"className\": \"Image\" "
                                     "}";
    /**
     * @tc.steps: step2. call CreateFromJson interface and get ImageComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonImageStr.c_str());
    size_t dslLen = jsonImageStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<ImageComponent> imageComponent = AceType::DynamicCast<ImageComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the attributes of ImageComponent is as default.
     * @tc.expected: step3. The attributes of ImageComponent is as default.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(NearEqual(imageComponent->GetWidth().Value(), IMAGE_WIDTH_DEFAULT));
    ASSERT_TRUE(NearEqual(imageComponent->GetHeight().Value(), IMAGE_HEIGHT_DEFAULT));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetHorizontal(), ALIGNMENT_CENTER));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetVertical(), ALIGNMENT_CENTER));
    ASSERT_TRUE(imageComponent->GetColor() == IMAGE_COLOR_TRANSPARENT);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_JSON);
    ASSERT_TRUE(imageComponent->GetImageRepeat() == IMAGE_NO_REPEAT);
    ASSERT_TRUE(imageComponent->GetSrc().empty());
    ASSERT_TRUE(imageComponent->GetAlt().empty());
}

/**
 * @tc.name: ImageCreator003
 * @tc.desc: Verify that ImageCreator can parse json when all attributes are set incorrectly.
 * @tc.type: FUNC
 * @tc.require: AR000DBAPR AR000DAQUL AR000DAQUM AR000DAQUN AR000DAQUO AR000DAQUP AR000DAQUR
 * @tc.author: chenxuankai
 */
HWTEST_F(ImageCreatorTest, ImageCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of Image with all attributes.
     */
    const std::string jsonImageStr = ""
                                     "{                                                                       "
                                     "  \"className\": \"Image\", \"imageFit\": 0, \"imageRepeat\": -3,       "
                                     "  \"alignment\": \"BOTTOM_RIGHT\", \"color\": \"RED\", \"src\": 12,     "
                                     "  \"width\": \"wrap-content\", \"height\": \"wrap-content\", \"alt\": 1 "
                                     "}";
    /**
     * @tc.steps: step2. call CreateFromJson interface and get ImageComponent.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonImageStr.c_str());
    size_t dslLen = jsonImageStr.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<ImageComponent> imageComponent = AceType::DynamicCast<ImageComponent>(component);

    /**
     * @tc.steps: step3. Verify whether the attributes of ImageComponent is as default.
     * @tc.expected: step3. The attributes of ImageComponent is as default.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(NearEqual(imageComponent->GetWidth().Value(), IMAGE_WIDTH_DEFAULT));
    ASSERT_TRUE(NearEqual(imageComponent->GetHeight().Value(), IMAGE_HEIGHT_DEFAULT));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetHorizontal(), ALIGNMENT_CENTER));
    ASSERT_TRUE(NearEqual(imageComponent->GetAlignment().GetVertical(), ALIGNMENT_CENTER));
    ASSERT_TRUE(imageComponent->GetColor() == IMAGE_COLOR_TRANSPARENT);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_JSON);
    ASSERT_TRUE(imageComponent->GetImageRepeat() == IMAGE_NO_REPEAT);
    ASSERT_TRUE(imageComponent->GetSrc().empty());
    ASSERT_TRUE(imageComponent->GetAlt().empty());
}

} // namespace OHOS::Ace