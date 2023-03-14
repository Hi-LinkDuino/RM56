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

#ifndef HDI_TEST_DEVICE
#define HDI_TEST_DEVICE
#include <map>
#include <memory>
#include "display_device.h"
#include "display_gralloc.h"
#include "display_layer.h"
#include "hdi_test_device_common.h"
#include "hdi_test_display.h"
#include "hdi_test_layer.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
namespace TEST {
class HdiTestDevice {
public:
    static HdiTestDevice &GetInstance();
    static void HotPlug(uint32_t outputId, bool connected, void *data);
    int32_t InitDevice();

    GrallocFuncs &GetGrallocFuncs()
    {
        return *mGrallocFuncs;
    }
    LayerFuncs &GetLayerFuncs()
    {
        return *mLayerFuncs;
    }
    DeviceFuncs &GetDeviceFuncs()
    {
        return *mDeviceFuncs;
    }

    std::shared_ptr<HdiTestDisplay> GetDisplayFromId(uint32_t id);
    std::shared_ptr<HdiTestDisplay> GetFirstDisplay();
    void Clear();

private:
    std::shared_ptr<HdiTestDisplay> FindDisplayOrCreate(uint32_t id);

    std::map<uint32_t, std::shared_ptr<HdiTestDisplay>> mDisplays;
    DeviceFuncs *mDeviceFuncs = nullptr;
    LayerFuncs *mLayerFuncs = nullptr;
    GrallocFuncs *mGrallocFuncs = nullptr;
};
} // OHOS
} // HDI
} // DISPLAY
} // TEST

#endif // HDI_TEST_DEVICE