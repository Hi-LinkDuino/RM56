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

#include "hdi_device_test.h"
#include <chrono>
#include <cinttypes>
#include <algorithm>
#include "display_device.h"
#include "display_gralloc.h"
#include "display_layer.h"
#include "display_test.h"
#include "display_test_utils.h"
#include "hdi_composition_check.h"
#include "hdi_test_device.h"
#include "hdi_test_device_common.h"
#include "hdi_test_display.h"
#include "hdi_test_render_utils.h"

using namespace OHOS::HDI::DISPLAY::TEST;
static std::shared_ptr<HdiTestLayer> g_testFreshLayer;

static const std::vector<std::vector<LayerSettings>> TEST_SINGLE_LAYER = {
    // one layer display test
    {
        {
            .rectRatio = { 0, 0, 1.0f, 0.125f },
            .color = RED },
    },
    {
        {
            .rectRatio = { 0, 0.875f, 1.0f, 0.125f },
            .color = GREEN },
    },
    {
        {
            .rectRatio = { 0, 0.0, 1.0f, 1.0f },
            .color = YELLOW },
    },
};

static const std::vector<std::vector<LayerSettings>> TEST_MULTILAYER = {

    // three layer displayrect test
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED },
        {
            .rectRatio = { 0, 0, 1.0f, 0.125f },
            .color = GREEN },
        {
            .rectRatio = { 0, 0.875f, 1.0f, 0.125f },
            .color = YELLOW },
    }

};

static const std::vector<std::vector<LayerSettings>> TEST_SCALE = {
    // scale layer test
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED,
            .bufferRatio = { 1.5, 1.5 } },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 0.125f },
            .color = GREEN,
            .bufferSize = { 1, 1 } },
    },
    {
        {
            .rectRatio = { 0, 0.875f, 1.0f, 0.125f },
            .color = YELLOW,
            .bufferRatio = { 0.25, 0.25 } },
    }
};

static const std::vector<std::vector<LayerSettings>> TEST_VIDEO = {
    // video layer test
    {
        {
            .rectRatio = { 0, 0, 1.0f, 0.125f },
            .color = GREEN,
            .compositionType = COMPOSITION_VIDEO },
    },

    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED },
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED,
            .compositionType = COMPOSITION_VIDEO },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED },
        {
            .rectRatio = { 0, 0, 1.0f, 0.125f },
            .color = GREEN,
            .compositionType = COMPOSITION_VIDEO },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED },
        {
            .rectRatio = { 0, 0.875f, 1.0f, 0.125f },
            .color = YELLOW,
            .compositionType = COMPOSITION_VIDEO },
    }
};

static const std::vector<std::vector<LayerSettings>> TEST_ALPHA = {
    // alpha layer test
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED,
            .alpha = 0xFF },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED,
            .alpha = 0x55 },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = RED,
            .alpha = 0x00 },
    },
    // RED float will overide, must use green now
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = GREEN,
            .alpha = 0xFF,
            .blendType = BLEND_SRCOVER },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = GREEN,
            .alpha = 0xA7,
            .blendType = BLEND_SRCOVER },
    },
    {
        {
            .rectRatio = { 0, 0, 1.0f, 1.0f },
            .color = GREEN,
            .alpha = 0x00,
            .blendType = BLEND_SRCOVER },
    },
};

static const std::vector<LayerSettings> TEST_ROTATE = {
    {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED,
        .rotate = ROTATE_NONE },
    {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED,
        .rotate = ROTATE_90 },
    {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED,
        .rotate = ROTATE_180 },
    {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED,
        .rotate = ROTATE_270 },
};

static inline std::shared_ptr<HdiTestDisplay> GetFirstDisplay()
{
    return HdiTestDevice::GetInstance().GetFirstDisplay();
}

static int32_t CheckComposition(std::vector<LayerSettings> &layers, BufferHandle *clientBuffer,
    uint32_t checkType = HdiCompositionCheck::CHECK_VERTEX)
{
    DISPLAY_TEST_CHK_RETURN((clientBuffer == nullptr), DISPLAY_NULL_PTR, DISPLAY_TEST_LOGE("client buffer is nullptr"));
    return HdiCompositionCheck::GetInstance().Check(layers, *clientBuffer, checkType);
}

static std::shared_ptr<HdiTestLayer> CreateTestLayer(LayerSettings setting, uint32_t zorder)
{
    int ret;
    HdiTestDevice::GetInstance();
    DISPLAY_TEST_LOGD("color 0x%x", setting.color);
    std::shared_ptr<HdiTestDisplay> display = HdiTestDevice::GetInstance().GetFirstDisplay();
    DISPLAY_TEST_CHK_RETURN((display == nullptr), nullptr, DISPLAY_TEST_LOGE("can not get display"));

    std::shared_ptr<HdiTestLayer> layer = display->CreateHdiTestLayer(setting.bufferSize.w, setting.bufferSize.h);
    DISPLAY_TEST_CHK_RETURN((layer == nullptr), nullptr, DISPLAY_TEST_LOGE("can not create hdi test layer"));

    layer->SetLayerSize(setting.displayRect);

    layer->SetCompType(setting.compositionType);

    if ((setting.alpha >= 0) && (setting.alpha <= 0xff)) { // alpha rang 0x00 ~ 0xff
        LayerAlpha alpha = { 0 };
        alpha.gAlpha = setting.alpha;
        alpha.enGlobalAlpha = true;
        layer->SetAlpha(alpha);
    }
    HdiGrallocBuffer *handle = layer->GetFrontBuffer();
    DISPLAY_TEST_CHK_RETURN((handle == nullptr), nullptr, DISPLAY_TEST_LOGE("can not get front buffer"));
    ClearColor(*(handle->Get()), setting.color);
    ret = layer->SwapFrontToBackQ();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), nullptr, DISPLAY_TEST_LOGE("SwapFrontToBackQ failed"));
    layer->SetZorder(zorder);
    layer->SetBlendType(setting.blendType);
    layer->SetTransform(setting.rotate);
    return layer;
}

static int PrepareAndPrensent()
{
    int ret;
    DISPLAY_TEST_LOGD();
    std::shared_ptr<HdiTestDisplay> display = HdiTestDevice::GetInstance().GetFirstDisplay();
    DISPLAY_TEST_CHK_RETURN((display == nullptr), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("can not get display"));

    ret = display->PrepareDisplayLayers();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("PrepareDisplayLayers failed"));

    ret = display->Commit();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("Commit failed"));
    return DISPLAY_SUCCESS;
}

static void TestVBlankCallback(unsigned int sequence, uint64_t ns, void *data)
{
    static uint64_t lastns;
    DISPLAY_TEST_LOGD("seq %d  ns %" PRId64 " duration %" PRId64 " ns", sequence, ns, (ns - lastns));
    lastns = ns;
    VblankCtr::GetInstance().NotifyVblank(sequence, ns, data);
}

static void AdjustLayerSettings(std::vector<LayerSettings> &settings, uint32_t w, uint32_t h)
{
    DISPLAY_TEST_LOGD();
    for (uint32_t i = 0; i < settings.size(); i++) {
        LayerSettings &setting = settings[i];
        DISPLAY_TEST_LOGD(" ratio w: %f  ratio h: %f", setting.rectRatio.w, setting.rectRatio.h);
        if ((setting.rectRatio.w > 0.0f) && (setting.rectRatio.h > 0.0f)) {
            setting.displayRect.h = static_cast<uint32_t>(setting.rectRatio.h * h);
            setting.displayRect.w = static_cast<uint32_t>(setting.rectRatio.w * w);
            setting.displayRect.x = static_cast<uint32_t>(setting.rectRatio.x * w);
            setting.displayRect.y = static_cast<uint32_t>(setting.rectRatio.y * h);
            DISPLAY_TEST_LOGD("display rect adust form %f %f %f %f to %d %d %d %d ", setting.rectRatio.x,
                setting.rectRatio.y, setting.rectRatio.w, setting.rectRatio.h, setting.displayRect.x,
                setting.displayRect.y, setting.displayRect.w, setting.displayRect.h);
        }

        if ((setting.bufferRatio.h > 0.0f) || (setting.bufferRatio.w > 0.0f)) {
            setting.bufferSize.h = static_cast<uint32_t>(setting.bufferRatio.h * h);
            setting.bufferSize.w = static_cast<uint32_t>(setting.bufferRatio.w * w);
            DISPLAY_TEST_LOGD("buffer szie adjust for %f %f to %d %d", setting.bufferRatio.w, setting.bufferRatio.h,
                setting.bufferSize.w, setting.bufferSize.h);
        }

        if ((setting.bufferSize.w == 0) || (setting.bufferSize.h == 0)) {
            DISPLAY_TEST_LOGD("buffer szie adjust for %d %d to %d %d", setting.bufferSize.w, setting.bufferSize.h,
                setting.displayRect.w, setting.displayRect.h);

            setting.bufferSize.w = setting.displayRect.w;
            setting.bufferSize.h = setting.displayRect.h;
        }
    }
}

static std::vector<std::shared_ptr<HdiTestLayer>> CreateLayers(std::vector<LayerSettings> &settings)
{
    DISPLAY_TEST_LOGD("settings %zd", settings.size());
    std::vector<std::shared_ptr<HdiTestLayer>> layers;
    DisplayModeInfo mode = GetFirstDisplay()->GetCurrentMode();
    AdjustLayerSettings(settings, mode.width, mode.height);
    for (uint32_t i = 0; i < settings.size(); i++) {
        LayerSettings setting = settings[i];

        auto layer = CreateTestLayer(setting, i);
        layers.push_back(layer);
    }

    return layers;
}

static inline void PresentAndCheck(std::vector<LayerSettings> &layerSettings,
    uint32_t checkType = HdiCompositionCheck::CHECK_VERTEX)
{
    int ret = PrepareAndPrensent();
    ASSERT_TRUE((ret == DISPLAY_SUCCESS));
    HdiTestDevice::GetInstance().GetGrallocFuncs().InvalidateCache(GetFirstDisplay()->SnapShot());
    ret = CheckComposition(layerSettings, GetFirstDisplay()->SnapShot(), checkType);
    ASSERT_TRUE((ret == DISPLAY_SUCCESS));
}

void LayerRotateTest::TearDown()
{
    DISPLAY_TEST_LOGD();
    HdiTestDevice::GetInstance().Clear();
}

void DeviceTest::TearDown()
{
    DISPLAY_TEST_LOGD();
    HdiTestDevice::GetInstance().Clear();
}

void DeviceLayerDisplay::TearDown()
{
    HdiTestDevice::GetInstance().Clear();
}

void VblankCtr::NotifyVblank(unsigned int sequence, uint64_t ns, void *data)
{
    DISPLAY_TEST_LOGD();
    if (data != nullptr) {
        DISPLAY_TEST_LOGD("sequence = %u, ns = %" PRIu64 "", sequence, ns);
    }
    std::unique_lock<std::mutex> lg(mVblankMutex);
    mHasVblank = true;
    mVblankCondition.notify_one();
    DISPLAY_TEST_LOGD();
}

VblankCtr::~VblankCtr() {}

int32_t VblankCtr::WaitVblank(uint32_t ms)
{
    bool ret;
    DISPLAY_TEST_LOGD();
    std::unique_lock<std::mutex> lck(mVblankMutex);
    mHasVblank = false; // must wait next vblank
    ret = mVblankCondition.wait_for(lck, std::chrono::milliseconds(ms), [=] { return mHasVblank; });
    DISPLAY_TEST_LOGD();
    if (!ret) {
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

void VblankTest::TearDown()
{
    auto display = HdiTestDevice::GetInstance().GetFirstDisplay();
    int32_t ret = display->SetDisplayVsyncEnabled(false);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("vsync disable failed");
    }
    VblankCtr::GetInstance().WaitVblank(100); // wait for last vsync 100ms.
    HdiTestDevice::GetInstance().Clear();
}

TEST_P(DeviceLayerDisplay, LayerDisplay)
{
    std::vector<LayerSettings> layerSettings = GetParam();
    CreateLayers(layerSettings);
    PresentAndCheck(layerSettings);
    if (TestParemeter::GetInstance().mTestSleep > 0) {
        sleep(TestParemeter::GetInstance().mTestSleep);
    }
}

TEST_F(DeviceTest, zorder)
{
    std::vector<LayerSettings> settings = {
        {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED },
        {
        .rectRatio = { 0, 0, 1.0f, 0.125f },
        .color = GREEN },
        {
        .rectRatio = { 0, 0.875f, 1.0f, 0.125f },
        .color = YELLOW },
    };

    std::vector<std::vector<int>> zorders = {
        { 3, 2, 1 }, { 1, 3, 2 }, { 3, 1, 2 }, { 1, 2, 3 }, { 2, 1, 3 }, { 9, 100, 3 },
    };
    std::vector<std::shared_ptr<HdiTestLayer>> layers = CreateLayers(settings);

    for (const auto &zorderList : zorders) {
        // adjust the zorder
        for (uint32_t i = 0; i < zorderList.size(); i++) {
            settings[i].zorder = zorderList[i];
            layers[i]->SetZorder(zorderList[i]);
        }
        std::vector<LayerSettings> tempSettings = settings;
        std::sort(tempSettings.begin(), tempSettings.end(),
            [=](const auto &l, auto const & r) { return l.zorder < r.zorder; });
        // present and check
        PresentAndCheck(tempSettings);
    }
    HdiTestDevice::GetInstance().Clear();
}

TEST_P(LayerRotateTest, SplitCheck)
{
    LayerSettings settings = GetParam();
    std::vector<LayerSettings> layersSetting = { settings };
    std::vector<uint32_t> splitColors = { RED, GREEN, YELLOW, TRANSPARENT };
    std::vector<std::shared_ptr<HdiTestLayer>> layers = CreateLayers(layersSetting);
    ASSERT_TRUE((layers.size() > 0));
    // split the buffer
    auto layer = layers[0];
    HdiGrallocBuffer *handle = layer->GetBackBuffer(); // the backbuffer has not present now
    ASSERT_TRUE((handle != nullptr));
    auto splitRects = SplitBuffer(*(handle->Get()), splitColors);
    ASSERT_TRUE(splitRects.size() == splitColors.size()); // ensure the  splitRects size
    PrepareAndPrensent();
    // chage the rect and color to clockwise.
    std::swap(splitColors[2], splitColors[1]);
    std::swap(splitColors[2], splitColors[3]);
    std::swap(splitRects[2], splitRects[1]);
    std::swap(splitRects[2], splitRects[3]);

    // rotation is clockwise,the color will change clockwise, we just change the color start index
    uint32_t startIndex = 0;
    switch (settings.rotate) {
        case ROTATE_90:
            startIndex = 3; // start form color index 3
            break;
        case ROTATE_180:
            startIndex = 2; // start form color index 2
            break;
        case ROTATE_270:
            startIndex = 1; // start form color index 1
            break;
        default:
            startIndex = 0;
            break;
    }
    std::vector<LayerSettings> layersCheck;
    for (uint32_t i = 0; i < splitRects.size(); i++) {
        uint32_t color = splitColors[(i + startIndex) % sizeof(splitColors.size())];
        layersCheck.push_back({
            .displayRect = splitRects[i],
            .color = color });
    }
    ASSERT_TRUE((handle != nullptr));
    /* for rotation may scale the buffer, Near the edge of rect the color will Smooth gradient,
       so we must use the center to check.
    */
    PresentAndCheck(layersCheck, HdiCompositionCheck::CHECK_CENTER);
}

TEST_F(DeviceTest, crop)
{
    std::vector<LayerSettings> settings = {
        {
        .rectRatio = { 0, 0, 1.0f, 1.0f },
        .color = RED },
    };
    std::vector<uint32_t> splitColors = { { RED, GREEN, YELLOW, TRANSPARENT } };

    std::vector<std::shared_ptr<HdiTestLayer>> layers = CreateLayers(settings);
    ASSERT_TRUE((layers.size() > 0));
    // split the buffer
    auto layer = layers[0];
    HdiGrallocBuffer *handle = layer->GetBackBuffer(); // the backbuffer has not present now
    ASSERT_TRUE((handle != nullptr));
    auto splitRects = SplitBuffer(*(handle->Get()), splitColors);
    PrepareAndPrensent();
    for (uint32_t i = 0; i < splitRects.size(); i++) {
        settings[0].color = splitColors[i];
        layer->SetLayerCrop(splitRects[i]);
        PresentAndCheck(settings);
    }
}

TEST_F(VblankTest, CtrlTest)
{
    int ret;
    DISPLAY_TEST_LOGD();
    std::shared_ptr<HdiTestDisplay> display = HdiTestDevice::GetInstance().GetFirstDisplay();
    ret = display->RegDisplayVBlankCallback(TestVBlankCallback, nullptr);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS) << "RegDisplayVBlankCallback failed";
    ret = display->SetDisplayVsyncEnabled(true);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS) << "SetDisplayVsyncEnabled failed";
    ret = VblankCtr::GetInstance().WaitVblank(1000); // 1000ms
    ASSERT_TRUE(ret == DISPLAY_SUCCESS) << "WaitVblank timeout";
    ret = display->SetDisplayVsyncEnabled(false);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS) << "SetDisplayVsyncEnabled failed";
    usleep(100 * 1000);                              // wait for 100ms avoid the last vsync.
    ret = VblankCtr::GetInstance().WaitVblank(1000); // 1000ms
    ASSERT_TRUE(ret != DISPLAY_SUCCESS) << "vblank do not disable";
}

INSTANTIATE_TEST_CASE_P(MultiLayer, DeviceLayerDisplay, ::testing::ValuesIn(TEST_MULTILAYER));
INSTANTIATE_TEST_CASE_P(SingleLayer, DeviceLayerDisplay, ::testing::ValuesIn(TEST_SINGLE_LAYER));
INSTANTIATE_TEST_CASE_P(ScaleLayer, DeviceLayerDisplay, ::testing::ValuesIn(TEST_SCALE));
INSTANTIATE_TEST_CASE_P(VideoLayer, DeviceLayerDisplay, ::testing::ValuesIn(TEST_VIDEO));
INSTANTIATE_TEST_CASE_P(LayerAlpha, DeviceLayerDisplay, ::testing::ValuesIn(TEST_ALPHA));
INSTANTIATE_TEST_CASE_P(Rotation, LayerRotateTest, ::testing::ValuesIn(TEST_ROTATE));

int main(int argc, char **argv)
{
    int ret = HdiTestDevice::GetInstance().InitDevice();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_TEST_LOGE("Init Device Failed"));
    ::testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();
    auto display = HdiTestDevice::GetInstance().GetFirstDisplay();
    if (display != nullptr) {
        // avoid vsync call back affer the destruction of VblankCtr
        display->SetDisplayVsyncEnabled(false);
        VblankCtr::GetInstance().WaitVblank(100);
    }
    return ret;
}
