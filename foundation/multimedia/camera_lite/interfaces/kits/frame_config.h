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
 * @addtogroup MultiMedia_FrameConfig
 * @{
 *
 * @brief Defines the <b>FrameConfig</b> class for operations related to frame
 * configurations.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file frame_config.h
 *
 * @brief Declares APIs of the <b>FrameConfig</b> class.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_FRAME_CONFIG_H
#define OHOS_FRAME_CONFIG_H

#include "meta_data.h"
#include "surface.h"
#include <list>
#include <map>

namespace OHOS {
namespace Media {
constexpr int32_t FRAME_CONFIG_PREVIEW = 0;
constexpr int32_t FRAME_CONFIG_RECORD = 1;
constexpr int32_t FRAME_CONFIG_CAPTURE = 2;
constexpr int32_t FRAME_CONFIG_CALLBACK = 3;
constexpr int32_t PRIVATE_TAG_LEN = 32;

/**
 * @brief Provides functions to configure frames.
 *
 *
 * @since 1.0
 * @version 1.0
 */
class FrameConfig {
public:
    /**
     * @brief A constructor used to create a <b>FrameConfig</b> instance.
     *
     */
    FrameConfig() = delete;
    /**
     * @brief A constructor used to create a <b>FrameConfig</b> instance based on the configuration mode.
     *
     * @param type Indicates the frame configuration mode, which can be <b>FRAME_CONFIG_PREVIEW</b>,
     * @<b>FRAME_CONFIG_RECORD</b>, or <b>FRAME_CONFIG_CAPTURE</b>.
     */
    explicit FrameConfig(int32_t type);

    /**
     * @brief A destructor used to delete the <b>FrameConfig</b> instance.
     *
     */
    ~FrameConfig() {}

    /**
     * @brief Obtains the frame configuration type.
     *
     * @return Returns the frame configuration type if obtained; returns <b>-1</b>
     * otherwise.
     */
    int32_t GetFrameConfigType();

    /**
     * @brief Obtains a list of shared memories (surface objects).
     *
     * @return Returns the list of shared memories if obtained; returns
     * <b>NULL</b> otherwise.
     *
     */
    std::list<Surface *> GetSurfaces();

    /**
     * @brief Adds a surface (an object of shared memory).
     *
     * @param surface Indicates the surface to add.
     */
    void AddSurface(Surface &surface);

    /**
     * @brief Removes a surface (an object of shared memory).
     *        You can call this function to release the surface when your
     * application does not need to obtain data.
     *
     * @param surface Indicates the surface to remove.
     */
    void RemoveSurface(Surface &surface);

    /**
     * @brief Sets the common parameter.
     * @param key Indicates the common parameter key to set.
     * @param value Indicates the common parameter value to set.
     */
    template<typename T> void SetParameter(uint32_t key, const T value)
    {
        SetValue(key, static_cast<const void *>(&value));
    }

    /**
     * @brief Obtains the value of a common parameter based on its key. For
     * details, see {@link CAMERA_FUC_KEY}.
     * @param key Indicates the common parameter key.
     */
    template<typename T> void GetParameter(uint32_t key, T &value)
    {
        T *pvalue = static_cast<T *>(GetValue(key));
        if (pvalue != nullptr) {
            value = *pvalue;
        }
    }

	/**
     * @brief set the private config value
     * @param value Indicates the private config value.
     * @param len Indicates the length of the private config value;
     */
    void SetVendorParameter(uint8_t *value, uint32_t len);

    /**
     * @brief get the private config value
     * @param value Indicates the private config value.
     * @param len Indicates the length of the private config value;
     */
    void GetVendorParameter(uint8_t *value, uint32_t len);

private:
    int32_t type_;
    std::list<Surface *> surfaceList_;
    std::map<uint32_t, int32_t> keyMap_;
    CameraRect crop;
    uint8_t privateTag_[PRIVATE_TAG_LEN];
    void *GetValue(uint32_t key);
    void SetValue(uint32_t key, const void *value);
};
} // namespace Media
} // namespace OHOS
#endif // OHOS_FRAME_CONFIG_H