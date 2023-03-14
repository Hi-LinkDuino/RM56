/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @addtogroup Codec
 * @{
 *
 * @brief Defines APIs related to the Codec module.
 *
 * The Codec module provides APIs for initializing the custom data and audio and video codecs,
 * setting codec parameters, and controlling and transferring data.
 *
 * @since 3.1
 */

/**
 * @file codec_component_manager.h
 *
 * @brief Provides APIs for managing the Codec component.
 *
 * The APIs can be used to obtain the component encoding and decoding capability list,
 * and create or destroy components for the Codec module.
 *
 * @since 3.1
 */

#ifndef CODEC_COMPONENT_MANAGER_H
#define CODEC_COMPONENT_MANAGER_H

#include "codec_component_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the APIs for managing the codec components.
 *
 * The APIs can be used to:
 * Obtain the number of codec components and a codec capability list.
 * Create or destroy a codec component.
 */
struct CodecComponentManager {
    /**
     * @brief Obtains the number of codec components.
     *
     * All codec capability sets can be further obtained based on the number of codec components.
     *
     * @return Returns the number of codec components obtained.
     */
    int32_t (*GetComponentNum)();

    /**
     * @brief Obtains the codec capability list.
     *
     * You can use this API to obtain the encoding and decoding capabilities provided by the Codec module.
     * The capability is represented in the {@link CodecCompCapability} structure.
     *
     * @param capList Indicates the pointer to the component capability list {@link CodecCompCapability} obtained.
     * @param count Indicates the number of codec components, which is obtained by {@link GetComponentNum}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetComponentCapabilityList)(CodecCompCapability *capList, int32_t count);

    /**
     * @brief Creates a codec instance.
     *
     * You can use this API to create a codec component instance based on the component name.
     *
     * @param component Indicates the pointer to the codec component created.
     * @param compName Indicates the name of the component to create.
     * @param appData Indicates the pointer to the value defined by the application.
     * The value is returned by the callback.
     * @param appDataSize Indicates the length of <b>appData</b>, in bytes.
     * @param callbacks Indicates the pointer to the callback defined by <b>OMX_CALLBACKTYPE</b>.
     * For details, see {@link CodecCallbackType}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*CreateComponent)(struct CodecComponentType **component, char *compName, void *appData,
        int32_t appDataSize, struct CodecCallbackType *callbacks);

    /**
     * @brief Destroys a codec component instance.
     *
     * @param component Indicates the codec component to destroy.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*DestoryComponent)(struct CodecComponentType *component);
};

/**
 * @brief Instantiates the <b>CodecComponentManager</b> object.
 *
 * @return Returns the <b>CodecComponentManager</b> object instantiated.
 */
struct CodecComponentManager *GetCodecComponentManager(void);

/**
 * @brief Releases the <b>CodecComponentManager</b> object.
 */
void CodecComponentManagerRelease(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_COMPONENT_MANAGER_H */
/** @} */