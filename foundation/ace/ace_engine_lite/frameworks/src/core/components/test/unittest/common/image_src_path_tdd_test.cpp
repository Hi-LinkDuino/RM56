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

#include "image_src_path_tdd_test.h"
#include <cstdint>
#include <cstdio>
#include "js_app_context.h"

namespace OHOS {
namespace ACELite {
/**
 * @tc.name: ImageSrcPrivateDataPathSupport001
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. pass nullptr for processing
     */
    char *result = JsAppContext::GetInstance()->GetResourcePath(nullptr);
    /**
     * @tc.steps: step2. check if parse failed
     */
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ImageSrcPrivateDataPathSupport002
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. pass empty string for handling
     */
    char *resourcePath = JsAppContext::GetInstance()->GetResourcePath("");
    /**
     * @tc.steps: step2. check if handling failed
     */
    EXPECT_EQ(resourcePath, nullptr);
}

/**
 * @tc.name: ImageSrcPrivateDataPathSupport003
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. pass empty string for handling
     */
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("app/run/", "com.example.test", 0);
    char *resourcePath = JsAppContext::GetInstance()->GetResourcePath("abc.png");
    /**
     * @tc.steps: step2. check if handling failed
     */
    printf("resourcePath: %s\n", resourcePath);
    EXPECT_STREQ(resourcePath, "app/run/abc.png");
}

/**
 * @tc.name: ImageSrcPrivateDataPathSupport004
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. pass empty string for handling
     */
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("", "com.example.test", 0);
    char *resourcePath = JsAppContext::GetInstance()->GetResourcePath("internal://app/abc.png");
    /**
     * @tc.steps: step2. check if handling failed
     */
    EXPECT_STREQ(resourcePath, "com.example.test/abc.png");
}

/**
 * @tc.name: ImageSrcPrivateDataPathSupport005
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. pass empty string for handling
     */
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("", "com.example.test", 0);
    char *resourcePath = JsAppContext::GetInstance()->GetResourcePath("internal://app/testa/testb/testc/abc.png");
    /**
     * @tc.steps: step2. check if handling failed
     */
    EXPECT_STREQ(resourcePath, "com.example.test/testa/testb/testc/abc.png");
}

/**
 * @tc.name: ImageSrcPrivateDataPathSupport006
 * @tc.desc: Verify the path contact processing
 */
HWTEST_F(ImageSrcPathTddTest, PrivateDataPathSupport006, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare one very long string, longer than 255
     */
    const char *filePath =
        "internal://app/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/"
        "abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc//abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/"
        "abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc.png";
    char *resourcePath = JsAppContext::GetInstance()->GetResourcePath(filePath);
    /**
     * @tc.steps: step2. check if handling failed
     */
    EXPECT_EQ(resourcePath, nullptr);
}

/**
 * used for debugging TDD implementation on simulator
 */
void ImageSrcPathTddTest::RunTests()
{
#ifndef TDD_ASSERTIONS
    PrivateDataPathSupport001();
    PrivateDataPathSupport002();
    PrivateDataPathSupport003();
    PrivateDataPathSupport004();
    PrivateDataPathSupport005();
    PrivateDataPathSupport006();
#endif // TDD_ASSERTIONS
}
} // namespace ACELite
} // namespace OHOS
