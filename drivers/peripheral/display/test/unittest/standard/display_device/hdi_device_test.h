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

#ifndef HDI_DEVICE_TEST_H
#define HDI_DEVICE_TEST_H
#include <condition_variable>
#include <vector>
#include <mutex>
#include "gtest/gtest.h"
#include "display_type.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
constexpr uint32_t DISPLAY_DEAULT_W = 480;
constexpr uint32_t DISPLAY_DEAULT_H = 960;
constexpr uint32_t RED = 0xff0000ff;
constexpr uint32_t GREEN = 0x00ff00ff;
constexpr uint32_t BLUE = 0x0000ffff;
constexpr uint32_t TRANSPARENT = 0;
constexpr uint32_t YELLOW = 0xffff29ff;

struct FRect {
    float x = 0;
    float y = 0;
    float w = 0; // ratio
    float h = 0; // ratio
};

struct BufferRatio {
    float w;
    float h;
};

struct Size {
    uint32_t w;
    uint32_t h;
};

struct LayerSettings {
    IRect displayRect;
    IRect displayCrop;
    FRect rectRatio = { 0.0f };
    uint32_t color;
    uint32_t zorder = 0;
    Size bufferSize = { 0 };
    BufferRatio bufferRatio { 0 };
    int32_t alpha = -1;
    CompositionType compositionType = COMPOSITION_DEVICE;
    BlendType blendType = BLEND_SRC;
    TransformType rotate = ROTATE_NONE;
};

struct TestParemeter {
    static TestParemeter &GetInstance()
    {
        static TestParemeter instance;
        return instance;
    }
    int32_t mTestSleep = 0;
};

using LayersSetting = std::vector<LayerSettings>;
class DeviceLayerDisplay : public ::testing::TestWithParam<std::vector<LayerSettings>> {
protected:
    void SetUp() {}
    void TearDown();
};

class DeviceTest : public ::testing::Test {
protected:
    void TearDown();
};

// only support single layer test
class LayerRotateTest : public ::testing::TestWithParam<LayerSettings> {
protected:
    void TearDown();
};

class VblankTest : public ::testing::Test {
protected:
    void TearDown();
};

class VblankCtr {
public:
    static VblankCtr &GetInstance()
    {
        static VblankCtr instance;
        return instance;
    }
    void NotifyVblank(unsigned int sequence, uint64_t ns, void *data);
    int32_t WaitVblank(uint32_t ms);

protected:
    void TearDown();

private:
    std::mutex mVblankMutex;
    std::condition_variable mVblankCondition;
    VblankCtr() {}
    ~VblankCtr();
    bool mHasVblank = false;
};
} // OHOS
} // HDI
} // DISPLAY
} // TEST

#endif // HDI_DEVICE_TEST_H