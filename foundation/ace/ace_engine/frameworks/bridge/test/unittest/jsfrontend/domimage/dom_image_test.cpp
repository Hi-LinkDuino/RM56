/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/clip/clip_component.h"
#include "frameworks/bridge/common/dom/dom_image.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string IMAGE_SRC_VALUE = "/example/src";
const std::string IMAGE_ALT_VALUE = "/example/alt";
const ImageFit IMAGE_FIT_NONE = ImageFit::NONE;
const ImageFit IMAGE_FIT_COVER = ImageFit::COVER;
const ImageFit IMAGE_FIT_SCALEDOWN = ImageFit::SCALEDOWN;
const ImageFit IMAGE_FIT_CONTAIN = ImageFit::CONTAIN;
const ImageFit IMAGE_FIT_FILL = ImageFit::FILL;

} // namespace

class DomImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomImageTest::SetUpTestCase() {}
void DomImageTest::TearDownTestCase() {}
void DomImageTest::SetUp() {}
void DomImageTest::TearDown() {}

/**
 * @tc.name: SetImageAttributes001
 * @tc.desc: Verify that DomImage can create imageComponent correctly with all attributes set, object-fit is "none".
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage with all attributes set, object-fit is "none".
     */
    const std::string jsonImageStr = "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"attr\": [{                               "
                                     "                \"src\" : \"/example/src\"   "
                                     "              },                             "
                                     "              {                              "
                                     "                \"alt\" : \"/example/alt\"   "
                                     "              }],                            "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"none\"       "
                                     "          }],                                "
                                     "  \"event\": [ \"error\", \"complete\" ]     "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether all attributes of imageComponent are as excepted.
     * @tc.expected: step3. All attributes of imageComponent are as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetSrc() == IMAGE_SRC_VALUE);
    ASSERT_TRUE(imageComponent->GetAlt() == IMAGE_ALT_VALUE);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_NONE);
    ASSERT_TRUE(imageComponent->GetLoadSuccessEvent() == std::to_string(domNodeRoot->GetNodeId()));
    ASSERT_TRUE(imageComponent->GetLoadFailEvent() == std::to_string(domNodeRoot->GetNodeId()));
}

/**
 * @tc.name: SetImageAttributes002
 * @tc.desc: Verify that DomImage can create imageComponent correctly width no attribute set.
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage with no attribute.
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"attr\": [],                              "
                                     "  \"style\": [],                             "
                                     "  \"event\": []                              "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether all attributes of imageComponent are as excepted.
     * @tc.expected: step3. All attributes of imageComponent are as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetSrc().empty());
    ASSERT_TRUE(imageComponent->GetAlt().empty());
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_COVER);
    ASSERT_TRUE(imageComponent->GetLoadSuccessEvent().IsEmpty());
    ASSERT_TRUE(imageComponent->GetLoadFailEvent().IsEmpty());
}

/**
 * @tc.name: SetImageAttributes003
 * @tc.desc: Verify that DomImage can create imageComponent correctly when object-fit is "fill".
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage when object-fit is "fill".
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"fill\"       "
                                     "          }]                                 "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether imageFit attribute of imageComponent is as excepted.
     * @tc.expected: step3. imageFit attribute of imageComponent is as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_FILL);
}

/**
 * @tc.name: SetImageAttributes004
 * @tc.desc: Verify that DomImage can create imageComponent correctly when object-fit is "cover".
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage when object-fit is "cover".
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"cover\"      "
                                     "          }]                                 "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether imageFit attribute of imageComponent is as excepted.
     * @tc.expected: step3. imageFit attribute of imageComponent is as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_COVER);
}

/**
 * @tc.name: SetImageAttributes005
 * @tc.desc: Verify that DomImage can create imageComponent correctly when object-fit is "scale-down".
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage when object-fit is "scale-down".
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"scale-down\" "
                                     "          }]                                 "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());
    /**
     * @tc.steps: step3. Verify whether imageFit attribute of imageComponent is as excepted.
     * @tc.expected: step3. imageFit attribute of imageComponent is as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_SCALEDOWN);
}

/**
 * @tc.name: SetImageAttributes006
 * @tc.desc: Verify that DomImage can create imageComponent correctly when object-fit is "contain".
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage when object-fit is "contain".
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"contain\"    "
                                     "          }]                                 "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether imageFit attribute of imageComponent is as excepted.
     * @tc.expected: step3. imageFit attribute of imageComponent is as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_CONTAIN);
}

/**
 * @tc.name: SetImageAttributes007
 * @tc.desc: Verify that DomImage can create imageComponent correctly when object-fit value is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(DomImageTest, SetImageAttributes007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomImage when object-fit value is invalid.
     */
    const std::string jsonImageStr = ""
                                     "{                                            "
                                     "  \"tag\": \"image\",                        "
                                     "  \"style\": [{                              "
                                     "                \"objectFit\":\"center\"     "
                                     "          }]                                 "
                                     "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomImage.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonImageStr);
    auto imageComponent = AceType::DynamicCast<ImageComponent>(domNodeRoot->GetSpecializedComponent());

    /**
     * @tc.steps: step3. Verify whether imageFit attribute of imageComponent is as excepted.
     * @tc.expected: step3. imageFit attribute of imageComponent is as excepted.
     */
    ASSERT_TRUE(imageComponent != nullptr);
    ASSERT_TRUE(imageComponent->GetImageFit() == IMAGE_FIT_COVER);
}

} // namespace OHOS::Ace::Framework
