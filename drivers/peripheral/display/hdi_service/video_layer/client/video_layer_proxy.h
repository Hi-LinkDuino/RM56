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

#ifndef HDI_DISPLAY_LAYER_PROXY_V1_0_H
#define HDI_DISPLAY_LAYER_PROXY_V1_0_H

#include <iremote_proxy.h>
#include "idisplay_layer.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class VideoLayerProxy : public IRemoteProxy<IDisplayLayer> {
public:
    explicit VideoLayerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IDisplayLayer>(impl) {}
    virtual ~VideoLayerProxy() {}

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

private:
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_INIT_DISPLAY = 0;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_DEINIT_DISPLAY = 1;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_GET_DISPLAY_INFO = 2;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_CREATE_LAYER = 3;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_CLOSE_LAYER = 4;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_VISIBLE = 5;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_VISIBLE_STATE = 6;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_RECT = 7;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_RECT = 8;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_ZORDER = 9;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_ZORDER = 10;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_SET_TRANSFORM_MODE = 11;
    static constexpr int  CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_BUFFER = 12;

    static inline BrokerDelegator<VideoLayerProxy> delegator_;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_DISPLAY_LAYER_PROXY_V1_0_H
