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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSFORM_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSFORM_MOCK_H

#include "core/components/transform/render_transform.h"

namespace OHOS::Ace {

class MockRenderTransform : public RenderTransform {
    DECLARE_ACE_TYPE(MockRenderTransform, RenderTransform);

public:
    using HookRenderTransform = std::function<void(const RefPtr<MockRenderTransform>&)>;
    void MockTranslate(Dimension x, Dimension y);
    void MockScale(float value);
    void MockScale(float x, float y);
    void MockRotate(float angle, float x, float y, float z);
    void MockRotateX(float angle);
    void MockRotateY(float angle);
    void MockRotateZ(float angle);

    void GetTranslateSetting(Dimension& x, Dimension& y) const;
    void GetScaleSetting(float& scaleX, float& scaleY) const;
    void GetScaleSetting(float& scale) const;
    void GetRotateSetting(float& angle, float& x, float& y, float& z) const;
    bool GetTouchable() const;
    void ResetSetting();

    static void SetMockHook(const HookRenderTransform& hook);

private:
    Dimension translateX_;
    Dimension translateY_;
    float rotateAngle_ = 0.0f;
    float rotateAxisX_ = 0.0f;
    float rotateAxisY_ = 0.0f;
    float rotateAxisZ_ = 0.0f;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    float scaleXY_ = 1.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSFORM_MOCK_H
