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

#ifndef HDI_DISPLAY_H
#define HDI_DISPLAY_H
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <memory.h>
#include "display_device.h"
#include "hdi_composer.h"
#include "hdi_layer.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
const uint32_t INVALIDE_DISPLAY_ID = 0xffffffff;
const uint32_t DISPLAY_TYPE_DRM = (1 << 31);

class VsyncCallBack {
public:
    VsyncCallBack(VBlankCallback cb, void *data);
    virtual void Vsync(unsigned int sequence, uint64_t ns);
    virtual ~VsyncCallBack() {}

private:
    VBlankCallback mVBlankCb;
    void *mData;
};


class HdiDisplay {
public:
    uint32_t GetId() const
    {
        return mId;
    }
    virtual int32_t Init();
    virtual void DeInit() {}
    HdiDisplay() {}
    virtual ~HdiDisplay();
    virtual int32_t GetDisplayCapability(DisplayCapability *info)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t GetDisplayMode(uint32_t *modeId)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t SetDisplayMode(uint32_t modeId)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t GetDisplayPowerStatus(DispPowerStatus *status)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t SetDisplayPowerStatus(DispPowerStatus status)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t GetDisplayBacklight(uint32_t *value)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t SetDisplayBacklight(uint32_t value)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t CreateLayer(const LayerInfo *layerInfo, uint32_t *layerId);
    virtual int32_t CloseLayer(uint32_t layerId);
    virtual int32_t PrepareDisplayLayers(bool *needFlushFb);
    virtual int32_t Commit(int32_t *fence);
    virtual int32_t GetDisplayCompChange(uint32_t *num, uint32_t *layers, int32_t *type);
    virtual int32_t SetLayerZorder(uint32_t layerId, uint32_t zorder);
    virtual bool IsConnected()
    {
        return false;
    }
    virtual int32_t RegDisplayVBlankCallback(VBlankCallback cb, void *data)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t SetDisplayVsyncEnabled(bool enabled)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    virtual int32_t GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences);
    virtual int32_t SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence);
    virtual int32_t WaitForVBlank(uint64_t *ns)
    {
        return DISPLAY_NOT_SUPPORT;
    }
    HdiLayer *GetHdiLayer(uint32_t id);

protected:
    virtual std::unique_ptr<HdiLayer> CreateHdiLayer(LayerType type);
    std::unique_ptr<HdiComposer> mComposer;

    static uint32_t GetIdleId();
    static uint32_t mIdleId;
    static std::unordered_set<uint32_t> mIdSets;
    uint32_t mId = INVALIDE_DISPLAY_ID;
    std::unordered_map<uint32_t, std::unique_ptr<HdiLayer>> mLayersMap;
    std::multiset<HdiLayer *, SortLayersByZ> mLayers;
    std::unique_ptr<HdiLayer> mClientLayer;
    std::vector<HdiLayer *> mChangeLayers;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_DISPLAY_H