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

#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_texture_mapper.h"
#include "components/ui_view_group.h"
#include "dock/screen_device.h"
#include "dock/screen_device_proxy.h"
#include "gfx_utils/style.h"
#include "test_resource_config.h"
#include "window/window.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class TestDevice : public ScreenDevice {
public:
    TestDevice() {}
    virtual ~TestDevice() {}

    bool HardwareFill(const Rect& fillArea,
                      uint32_t color,
                      OpacityType opa,
                      uint8_t* dst,
                      uint32_t dstStride,
                      ColorMode dstColorMode) override
    {
        fillIn_ = true;
        return false;
    }

    bool HardwareBlend(const uint8_t* src,
                       const Rect& srcRect,
                       uint32_t srcStride,
                       uint32_t srcLineNumber,
                       ColorMode srcColorMode,
                       uint32_t color,
                       OpacityType opa,
                       uint8_t* dst,
                       uint32_t dstStride,
                       ColorMode dstColorMode,
                       uint32_t x,
                       uint32_t y) override
    {
        blendIn_ = true;
        return false;
    }

    bool HardwareTransform(const uint8_t* src,
                           ColorMode srcColorMode,
                           const Rect& srcRect,
                           const Matrix3<float>& transformMatrix,
                           OpacityType opa,
                           uint32_t color,
                           const Rect& mask,
                           uint8_t* dst,
                           uint32_t dstStride,
                           ColorMode dstColorMode,
                           const TransformOption& option) override
    {
        transformIn_ = true;
        return false;
    }

    bool fillIn_ =  false;
    bool blendIn_ = false;
    bool transformIn_ = false;
};

static TestDevice* g_testDevice = nullptr;

class HardwareAccelerationTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        GraphicStartUp::Init();
        g_testDevice = new TestDevice;
        ScreenDeviceProxy::GetInstance()->SetDevice(g_testDevice);
    }
    static void TearDownTestCase(void)
    {
        delete g_testDevice;
        g_testDevice = nullptr;
        ScreenDeviceProxy::GetInstance()->SetDevice(nullptr);
    }
    void SetUp()
    {
        g_testDevice->fillIn_ = false;
        g_testDevice->blendIn_ = false;
        g_testDevice->transformIn_ = false;
    }
    void TearDown() {}

    static void CreateDefaultWindow(RootView* rootView, int x, int y)
    {
        WindowConfig config = {};
        config.rect = rootView->GetRect();
        config.rect.SetPosition(x, y);
        Window* window = Window::CreateWindow(config);
        if (window != nullptr) {
            window->BindRootView(rootView);
        }
    }
};

/**
 * @tc.name: HardwareFill_001
 * @tc.desc: Verify HardwareFill function, equal.
 *           Need set ENABLE_HARDWARE_ACCELERATION to 1.
 * @tc.type: FUNC
 * @tc.require: SR000ERCR6
 */
#if ENABLE_HARDWARE_ACCELERATION
HWTEST_F(HardwareAccelerationTest, HardwareFill_001, TestSize.Level1)
{
    RootView* rootView = RootView::GetInstance();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(300); // 300: height
    rootView->SetPosition(0, 0);
    rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);
    rootView->Invalidate();

    HardwareAccelerationTest::CreateDefaultWindow(rootView, 0, 0);

    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_testDevice->fillIn_, true);
    Window::DestoryWindow(rootView->GetBoundWindow());
}
#endif

/**
 * @tc.name: HardwareBlend_001
 * @tc.desc: Verify HardwareBlend function, equal.
 *           Need set ENABLE_HARDWARE_ACCELERATION to 1.
 * @tc.type: FUNC
 * @tc.require: AR000EVI3I
 */
#if ENABLE_HARDWARE_ACCELERATION
HWTEST_F(HardwareAccelerationTest, HardwareBlend_001, TestSize.Level0)
{
    RootView* rootView = RootView::GetInstance();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(300); // 300: height
    rootView->SetPosition(0, 0);
    rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    UIImageView* imageView = new UIImageView;
    imageView->SetPosition(200, 50, 200, 200); // 200: x; 50: y; 200: width; 200: height
    imageView->SetSrc(RED_IMAGE_PATH);

    rootView->Add(imageView);
    rootView->Invalidate();

    HardwareAccelerationTest::CreateDefaultWindow(rootView, 0, 0);

    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_testDevice->blendIn_, true);
    Window::DestoryWindow(rootView->GetBoundWindow());
}
#endif

/**
 * @tc.name: HardwareBlend_002
 * @tc.desc: Verify HardwareBlend function for text, equal.
 *           Need set ENABLE_HARDWARE_ACCELERATION and ENABLE_HARDWARE_ACCELERATION_FOR_TEXT to 1.
 * @tc.type: FUNC
 * @tc.require: AR000EVI3I
 */
#if ENABLE_HARDWARE_ACCELERATION && ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
HWTEST_F(HardwareAccelerationTest, HardwareBlend_002, TestSize.Level1)
{
    RootView* rootView = RootView::GetInstance();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(300); // 300: height
    rootView->SetPosition(0, 0);
    rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    UILabel* label = new UILabel();
    // 288: x-coordinate; 48: y-coordinate
    label->SetPosition(0, 0, 288, 48);
    label->SetText("HardwareBlend_002 测试");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18 : font size

    rootView->Add(label);
    rootView->Invalidate();

    HardwareAccelerationTest::CreateDefaultWindow(rootView, 0, 0);

    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_testDevice->blendIn_, true);
    Window::DestoryWindow(rootView->GetBoundWindow());
}
#endif

/**
 * @tc.name: HardwareTransform_001
 * @tc.desc: Verify HardwareTransform function, equal.
 *           Need set ENABLE_HARDWARE_ACCELERATION to 1.
 * @tc.type: FUNC
 * @tc.require: AR000EVI3I
 */
#if ENABLE_HARDWARE_ACCELERATION
HWTEST_F(HardwareAccelerationTest, HardwareTransform_001, TestSize.Level0)
{
    RootView* rootView = RootView::GetInstance();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(300); // 300: height
    rootView->SetPosition(0, 0);
    rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    UIImageView* imageView = new UIImageView;
    imageView->SetPosition(200, 50, 200, 200); // 200: x; 50: y; 200: width; 200: height
    imageView->SetSrc(RED_IMAGE_PATH);
    Rect viewRect = imageView->GetOrigRect();
    TransformMap transMap(viewRect);
    Vector2<float> pivot_(58, 58); // 58:x value 58:y value
    transMap.Rotate(90, pivot_); // 90:degree
    transMap.Scale(Vector2<float>(1.5, 1.5), pivot_); // 1.5:x scale 1.5:y scale
    imageView->SetTransformMap(transMap);

    rootView->Add(imageView);
    rootView->Invalidate();

    HardwareAccelerationTest::CreateDefaultWindow(rootView, 0, 0);

    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_testDevice->transformIn_, true);
    Window::DestoryWindow(rootView->GetBoundWindow());
}
#endif

/**
 * @tc.name: HardwareTransform_002
 * @tc.desc: Verify HardwareTransform function in abnormal scene, equal.
 *           Need set ENABLE_HARDWARE_ACCELERATION to 1.
 * @tc.type: FUNC
 * @tc.require: AR000EVI3I
 */
#if ENABLE_HARDWARE_ACCELERATION
HWTEST_F(HardwareAccelerationTest, HardwareTransform_002, TestSize.Level1)
{
    // set device to null
    ScreenDeviceProxy::GetInstance()->SetDevice(nullptr);

    RootView* rootView = RootView::GetInstance();
    rootView->SetWidth(600);  // 600: width
    rootView->SetHeight(300); // 300: height
    rootView->SetPosition(0, 0);
    rootView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Olive().full);

    UIImageView* imageView = new UIImageView;
    imageView->SetPosition(200, 50, 200, 200); // 200: x; 50: y; 200: width; 200: height
    imageView->SetSrc(RED_IMAGE_PATH);
    Rect viewRect = imageView->GetOrigRect();
    TransformMap transMap(viewRect);
    Vector2<float> pivot_(58, 58); // 58:x value 58:y value
    transMap.Rotate(90, pivot_); // 90:degree
    transMap.Scale(Vector2<float>(1.5, 1.5), pivot_); // 1.5:x scale 1.5:y scale
    imageView->SetTransformMap(transMap);

    rootView->Add(imageView);
    rootView->Invalidate();

    HardwareAccelerationTest::CreateDefaultWindow(rootView, 0, 0);

    TaskManager::GetInstance()->TaskHandler();
    EXPECT_EQ(g_testDevice->fillIn_, false);
    EXPECT_EQ(g_testDevice->blendIn_, false);
    EXPECT_EQ(g_testDevice->transformIn_, false);
    Window::DestoryWindow(rootView->GetBoundWindow());
}
#endif
} // namespace OHOS
