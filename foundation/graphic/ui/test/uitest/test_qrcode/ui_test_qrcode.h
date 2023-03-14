/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_QRCODE_H
#define UI_TEST_QRCODE_H

#include "components/ui_qrcode.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestQrcode : public UITest {
public:
    UITestQrcode() {}
    ~UITestQrcode() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    /**
     * @brief Test display UIQrcode
     */
    void UIKitUIQrcodeTestDisplay001();
    void UIKitUIQrcodeTestDisplay002();
    void UIKitUIQrcodeTestDisplay003();
    void UIKitUIQrcodeTestDisplay004();
    void UIKitUIQrcodeTestDisplay005();
    void UIKitUIQrcodeTestDisplay006();
    void UIKitUIQrcodeTestDisplay007();
    void UIKitUIQrcodeTestDisplay008();
    void UIKitUIQrcodeTestDisplay009();
private:
    UIScrollView* container_ = nullptr;
    UIViewGroup* CreateTestCaseGroup(const char* title) const;
};
} // namespace OHOS
#endif // UI_TEST_QRCODE_H
