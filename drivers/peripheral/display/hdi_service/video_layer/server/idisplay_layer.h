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

/**
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * This module provides driver functions for the graphics subsystem, including graphics layer management,
 * device control, graphics hardware acceleration, display memory management, and callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file display_layer.h
 *
 * @brief Declares the driver functions for implementing layer operations.
 *
 * @since 1.0
 * @version 2.0
 */

#ifndef HDI_IDISPLAY_LAYER_STB_V1_0_H
#define HDI_IDISPLAY_LAYER_STB_V1_0_H

#include <iservmgr_hdi.h>
#include "display_type.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class IDisplayLayer {
public:
    explicit IDisplayLayer() {}
    virtual ~IDisplayLayer() {}

    /**
     * @brief Initializes a display device.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see DeinitDisplay
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode InitDisplay(unsigned int devId) = 0;

    /**
     * @brief Deinitializes a display device.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see InitDisplay
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode DeinitDisplay(unsigned int devId) = 0;

    /**
     * @brief Obtains information about a display device.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param dispInfo Indicates the pointer to the display device information obtained.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode GetDisplayInfo(unsigned int devId, std::shared_ptr<DisplayInfo> &dispInfo) = 0;

    /**
     * @brief Opens a layer on a specified display device.
     *
     * Before using a layer on the GUI, you must open the layer based on the layer information. After the layer is
     * opened, you can obtain the layer ID and then use other functions based on the layer ID.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerInfo Indicates the pointer to the layer information passed to open a layer, including the layer
     * type, layer size, and pixel format.
     * @param layerId Indicates the pointer to the layer ID, which identifies a unique layer. The layer ID is returned
     * to the GUI after the layer is successfully opened.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see CloseLayer
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode CreateLayer(unsigned int devId, LayerInfo &layerInfo, unsigned int &layerId) = 0;

    /**
     * @brief Closes a layer that is no longer required on a specified display device.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see OpenLayer
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode CloseLayer(unsigned int devId, unsigned int layerId) = 0;

    /**
     * @brief Sets whether a layer is visible.
     *
     * A visible layer is displayed on the screen, whereas an invisible layer is not displayed on the screen.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param visible Indicates the visibility to set for the layer. The value <b>true</b> means to set the layer to be
     * visible, and <b>false</b> means the opposite.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see GetLayerVisibleState
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode SetLayerVisible(unsigned int devId, unsigned int layerId, bool visible) = 0;

    /**
     * @brief Checks whether a layer is visible.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param visible Indicates the pointer to the obtained layer visibility. The value <b>true</b> indicates that the
     * layer is visible, and <b>false</b> indicates that the layer is invisible.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see SetLayerVisible
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode GetLayerVisibleState(unsigned int devId, unsigned int layerId, bool &visible) = 0;

    /**
     * @brief Sets the size of a layer.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param rect Indicates the pointer to the layer size to set, in pixels.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see SetLayerRect
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode SetLayerRect(unsigned int devId, unsigned int layerId, IRect &rect) = 0;

    /**
     * @brief Obtains the size of a layer.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param rect Indicates the pointer to the obtained layer size.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see SetLayerRect
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode GetLayerRect(unsigned int devId, unsigned int layerId, std::shared_ptr<IRect> &rect) = 0;

    /**
     * @brief Sets the z-order for a layer.
     *
     * A larger z-order value indicates a higher layer.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param zorder Indicates the z-order to set. The value is an integer ranging from 0 to 255.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see GetLayerZorder
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode SetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int zorder) = 0;

    /**
     * @brief Obtains the z-order of a layer.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param zorder Indicates the pointer to the obtained z-order. The value is an integer ranging from 0 to 255.
     * A larger z-order value indicates a higher layer.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @see SetLayerZorder
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode GetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int &zorder) = 0;

    /**
     * @brief Sets the transform mode for rotating, scaling, or moving a layer.
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param type Indicates the transformation mode to set.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual DispErrCode SetTransformMode(unsigned int devId, unsigned int layerId, TransformType &type) = 0;

    /**
     * @brief Set the buffer for a layer.
     *
     *
     * @param devId Indicates the ID of a display device. The value ranges from 0 to 4, where 0 indicates the first
     * display device and 4 indicates the last display device.
     * @param layerId Indicates the layer ID, which identifies a unique layer. You can perform operations on the layer
     * with the specified layer ID.
     * @param buffer Indiactes pointer of the buffer handle, the buffer handle should contain all the information of the
     * buffer which will been used for composition
     * @param fence Indiactes the fd of a sync file
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 2.0
     * @version 2.0
     */
    virtual DispErrCode SetLayerBuffer(unsigned int devId, unsigned int layerId, const BufferHandle &buffer,
        int fence) = 0;
}; // class IDisplayLayer
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif  // HDI_IDISPLAY_LAYER_STB_V1_0_H
/** @} */
