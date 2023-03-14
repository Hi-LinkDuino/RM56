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

#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/dialog/dialog_component.h"
#include "core/components/test/unittest/dialog/dialog_test_utils.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

class RenderDialogTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderDialogUpdate001
 * @tc.desc: Verify the Update Interface of RenderDialog work correctly with dialog component.
 * @tc.type: FUNC
 */
HWTEST_F(RenderDialogTest, RenderDialogUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct DialogComponent and RenderDialog.
     */
    auto dialogTween = AceType::MakeRefPtr<DialogTweenComponent>();
    dialogTween->SetAutoCancel(false);
    auto renderDialogTween = AceType::MakeRefPtr<MockRenderDialogTween>();

    /**
     * @tc.steps: step2. call the Update interface of RenderDialog.
     * @tc.expected: step2. renderDialog is set as needLayout, autoCancel is set as false.
     */
    renderDialogTween->Update(dialogTween);
    EXPECT_TRUE(renderDialogTween->NeedLayout());
    EXPECT_TRUE(!renderDialogTween->GetAutoCancel());
}

} // namespace OHOS::Ace