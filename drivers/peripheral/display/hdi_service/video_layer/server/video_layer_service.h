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

#ifndef HDI_VIDEO_LAYER_SERVICE_V1_0_H
#define HDI_VIDEO_LAYER_SERVICE_V1_0_H

#include "idisplay_layer.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class VideoLayerService : public IDisplayLayer {
public:
    virtual DispErrCode InitDisplay(unsigned int devId) override;
    virtual DispErrCode DeinitDisplay(unsigned int devId) override;
    virtual DispErrCode GetDisplayInfo(unsigned int devId, std::shared_ptr<DisplayInfo> &dispInfo) override;
    virtual DispErrCode CreateLayer(unsigned int devId, LayerInfo &layerInfo, unsigned int &layerId) override;
    virtual DispErrCode CloseLayer(unsigned int devId, unsigned int layerId) override;
    virtual DispErrCode SetLayerVisible(unsigned int devId, unsigned int layerId, bool visible) override;
    virtual DispErrCode GetLayerVisibleState(unsigned int devId, unsigned int layerId, bool &visible) override;
    virtual DispErrCode SetLayerRect(unsigned int devId, unsigned int layerId, IRect &rect) override;
    virtual DispErrCode GetLayerRect(unsigned int devId, unsigned int layerId, std::shared_ptr<IRect> &rect) override;
    virtual DispErrCode SetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int zorder) override;
    virtual DispErrCode GetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int &zorder) override;
    virtual DispErrCode SetTransformMode(unsigned int devId, unsigned int layerId, TransformType &type) override;
    virtual DispErrCode SetLayerBuffer(unsigned int devId, unsigned int layerId, const BufferHandle &buffer,
        int fence) override;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_VIDEO_LAYER_SERVICE_V1_0_H