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
 * @file codec_component_if.h
 *
 * @brief Defines the APIs for codec components.
 *
 * The APIs can be used to obtain component information, send commands to components, set component parameters,
 * and control and transfer buffer data.
 *
 * @since 3.1
 */

#ifndef CODEC_COMPONENT_H
#define CODEC_COMPONENT_H

#include <stdint.h>
#include "codec_callback_if.h"
#include "codec_component_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Defines the APIs for codec components.
 *
 * The APIs can be used to:
 * Obtain the component version.
 * Obtain and set component parameters.
 * Send a command to a component to obtain the component status.
 * Set a callback.
 * Set or release the buffer used by a component.
 * Process the input and output buffers for encoding and decoding.
 * For details, see the description of the APIs.
 */
struct CodecComponentType {
    /**
     * @brief Obtains the version of a codec component.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param compName Indicates the pointer to the component name.
     * @param compVersion Indicates the pointer to the OMX component version. For details, see {@link OMX_VERSIONTYPE}.
     * @param specVersion Indicates the pointer to the version information of the specifications based on which the
     * component is built. For details, see {@link OMX_VERSIONTYPE}.
     * @param compUUID Indicates the pointer to the UUID that uniquely identifies the component.
     * @param compUUIDLen Indicates the length of <b>compUUID</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetComponentVersion)(struct CodecComponentType *self, char *compName,
        union OMX_VERSIONTYPE *compVersion, union OMX_VERSIONTYPE *specVersion,
        uint8_t *compUUID, uint32_t compUUIDLen);

    /**
     * @brief Sends a command to a component.
     *
     * If the command is used to set status, a callback will be invoked to return the result. There is no event
     * reporting for other commands.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param cmd Indicates the command to be executed by the component. For details, see {@link OMX_COMMANDTYPE}.
     * @param param Indicates the parameters to be carried in the command.
     * If <b>cmd</b> is <b>OMX_CommandStateSet</b>, see {@link OMX_STATETYPE} for the value of <b>param</b>.
     * If <b>cmd</b> is <b>OMX_CommandFlush</b>, <b>OMX_CommandPortDisable</b>, <b>OMX_CommandPortEnable</b>,
     * or <b>OMX_CommandMarkBuffer</b>, <b>param</b> specifies the target port.
     * @param cmdData Indicates the pointer to <b>OMX_MARKTYPE</b> if <b>cmd</b> is <b>OMX_CommandMarkBuffer</b>.
     * @param cmdDataLen Indicates the length of <b>cmdData</b>, in bytes.
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*SendCommand)(struct CodecComponentType *self, enum OMX_COMMANDTYPE cmd, uint32_t param,
        int8_t *cmdData, uint32_t cmdDataLen);

    /**
     * @brief Obtains parameter settings of a component.
     *
     * When a component is in a state other than OMX_StateInvalid,
     * you can call this API to obtain component parameters.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param paramIndex Indicates the index of the structure to fill. For details, see {@link OMX_INDEXTYPE}.
     * @param paramStruct Indicates the pointer to the structure, allocated by the application,
     * to be filled by the component.
     * @param paramStructLen Indicates the length of <b>paramStruct</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetParameter)(struct CodecComponentType *self, uint32_t paramIndex, int8_t *paramStruct,
        uint32_t paramStructLen);

    /**
     * @brief Sets parameters for a component.
     *
     * This API can be used to set component parameters when the component is in the <b>OMX_StateLoaded</b> or
     * <b>OMX_StateWaitForResources</b> state or the port is disabled.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param index Indicates the index of the structure to set. For details, see {@link OMX_INDEXTYPE}.
     * @param paramStruct Indicates the pointer to the structure allocated by the application
     * for component initialization.
     * @param paramStructLen Indicates the length of <b>paramStruct</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*SetParameter)(struct CodecComponentType *self, uint32_t index, int8_t *paramStruct,
        uint32_t paramStructLen);

    /**
     * @brief Obtains the configuration of a component.
     *
     * This API can be used to obtain the component configuration after a component is loaded.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param index Indicates the index of the structure to fill. For details, see {@link OMX_INDEXTYPE}.
     * @param cfgStruct Indicates the pointer to the structure, allocated by the application,
     * to be filled by the component.
     * @param cfgStructLen Indicates the length of <b>cfgStruct</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetConfig)(struct CodecComponentType *self, uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen);

    /**
     * @brief Sets the component parameters.
     *
     * This API can be used to set a component after it is loaded.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param index Indicates the index of the structure to set. For details, see {@link OMX_INDEXTYPE}.
     * @param cfgStruct Indicates the pointer to the structure allocated by the application
     * for component initialization.
     * @param cfgStructLen Indicates the length of <b>cfgStruct</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*SetConfig)(struct CodecComponentType *self, uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen);

    /**
     * @brief Obtains the extended index of a component based on a given string.
     *
     * This API converts an extended string into an Openmax IL structure index.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param paramName Indicates the pointer to the string to be converted.
     * @param indexType Indicates the pointer to the configuration index converted from the given <b>paramName</b>.
     * For details, see {@link OMX_INDEXTYPE}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetExtensionIndex)(struct CodecComponentType *self, const char *paramName, uint32_t *indexType);

    /**
     * @brief Obtains component status.
     *
     * For details about component status, see {@link OMX_STATETYPE}.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param state Indicates the pointer to the status obtained. For more details, see {@link OMX_STATETYPE}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*GetState)(struct CodecComponentType *self, enum OMX_STATETYPE *state);

    /**
     * @brief Sets tunnel communication for a component.
     *
     * When a component is in the OMX_StateLoaded state, you can call this API to set tunnel communication
     * if the component supports tunnel transmission.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param port Indicates the port to set for the component.
     * @param tunneledComp Indicates the tunnel handle of the component.
     * @param tunneledPort Indicates the port to be used for tunnel communication.
     * @param tunnelSetup Indicates the pointer to the tunnel structure set. For details,
     * see {@link OMX_TUNNELSETUPTYPE}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*ComponentTunnelRequest)(struct CodecComponentType *self, uint32_t port,
        int32_t tunneledComp, uint32_t tunneledPort, struct OMX_TUNNELSETUPTYPE *tunnelSetup);

    /**
     * @brief Specify the buffer of the component port.
     *
     * This API is used when:
     * The component is in the OMX_StateLoaded state, and has sent a request for changing the state to OMX_StateIdle.
     * The component is in the OMX_StateWaitForResources state, the required resources are available,
     * and the component is ready to enter the OMX_StateIdle state.
     * The component is in the OMX_StateExecuting, OMX_StatePause, or OMX_StateIdle state on a disabled port.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param portIndex Indicates the port of the component.
     * @param buffer Indicates the pointer to the buffer to use. For details, see {@link OmxCodecBuffer}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*UseBuffer)(struct CodecComponentType *self, uint32_t portIndex, struct OmxCodecBuffer *buffer);

    /**
     * @brief Requests a port buffer from the component.
     *
     * This API is used to request a new buffer from a component when:
     * The component is in the OMX_StateLoaded state and has sent a request for changing the state to OMX_StateIdle.
     * The component is in the OMX_StateWaitForResources state, the required resources are available,
     * and the component is ready to enter the OMX_StateIdle state.
     * The component is in the OMX_StateExecuting, OMX_StatePause, or OMX_StateIdle state on a disabled port.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param buffer Indicates the pointer to the buffer requested. For details, see {@link OmxCodecBuffer}.
     * @param portIndex Indicates the port of the component.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*AllocateBuffer)(struct CodecComponentType *self, struct OmxCodecBuffer *buffer, uint32_t portIndex);

    /**
     * @brief Releases a buffer.
     *
     * This API is used to release a buffer when:
     * The component is in the OMX_StateIdle state and has sent a request for changing the state to OMX_StateLoaded.
     * The component is in the OMX_StateExecuting, OMX_StatePause, or OMX_StateIdle state on a disabled port.
     * This API can be called at any time. However, if it is not called in any of the previous conditions,
     * the component may report an OMX_ErrorPortUnpopulated event.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param portIndex Indicates the port of the component.
     * @param buffer Indicates the pointer to the buffer to release. For details, see {@link OmxCodecBuffer}.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*FreeBuffer)(struct CodecComponentType *self, uint32_t portIndex, const struct OmxCodecBuffer *buffer);

    /**
     * @brief Specify the buffer to be emptied by a component.
     *
     * This API should be called when the component is in the OMX_StateExecuting or OMX_StatePause state.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param buffer Indicates the pointer to the {@link OmxCodecBuffer} structure.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*EmptyThisBuffer)(struct CodecComponentType *self, const struct OmxCodecBuffer *buffer);

    /**
     * @brief Specify the buffer to be filled with the encoding and decoding output by a component.
     *
     * This API should be called when the component is in the OMX_StateExecuting or OMX_StatePause state.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param buffer Indicates the pointer to the {@link OmxCodecBuffer} structure.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*FillThisBuffer)(struct CodecComponentType *self, const struct OmxCodecBuffer *buffer);

    /**
     * @brief Set a callback for the codec component.
     *
     * This API is called to report an event or report available input or output information when the component
     * is in the OMX_StateLoaded state.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param callback Indicates the pointer to the {@link CodecCallbackType} object.
     * @param appData Indicates the pointer to the value defined by the application.
     * The value is returned by the callback.
     * @param appDataLen Indicates the length of <b>appData</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*SetCallbacks)(struct CodecComponentType *self, struct CodecCallbackType *callback,
        int8_t *appData, uint32_t appDataLen);

    /**
     * @brief Deinitializes a component.
     *
     * This API can be called to close a component in the OMX_StateLoaded state.
     *
     * @param self Indicates the pointer to the codec component to close.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*ComponentDeInit)(struct CodecComponentType *self);

    /**
     * @brief Uses the space allocated by EGL.
     *
     * This API is used when:
     * The component is in the OMX_StateLoaded state and has sent a request for changing the state to OMX_StateIdle.
     * The component is in the OMX_StateWaitForResources state, the required resources are available,
     * and the component is ready to enter the OMX_StateIdle state.
     * The component is in the OMX_StateExecuting, OMX_StatePause, or OMX_StateIdle state on a disabled port.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param buffer Indicates the pointer to the {@link OmxCodecBuffer} structure.
     * @param portIndex Indicates the port of the component.
     * @param eglImage Indicates the pointer to the image provided by EGL.
     * @param eglImageLen Indicates the length of <b>eglImage</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*UseEglImage)(struct CodecComponentType *self, struct OmxCodecBuffer *buffer, uint32_t portIndex,
        int8_t *eglImage, uint32_t eglImageLen);

    /**
     * @brief Obtains the component role.
     *
     * This API is used to obtain the role of a component based on the role index.
     *
     * @param self Indicates the pointer to the target codec component.
     * @param role Indicates the pointer to the role name.
     * @param roleLen Indicates the length of the role, in bytes.
     * @param index Indicates the index of a role. A component may support multiple roles.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*ComponentRoleEnum)(struct CodecComponentType *self, uint8_t *role, uint32_t roleLen, uint32_t index);
};

/**
 * @brief Instantiates a <b>CodecComponentType</b> object.
 *
 * @param remote Indicates the pointer to the <b>RemoteService</b>.
 *
 * @return Returns the <b>CodecComponentType</b> object instantiated.
 */
struct CodecComponentType *CodecComponentTypeGet(struct HdfRemoteService *remote);

/**
 * @brief Releases a <b>CodecComponentType</b> object.
 *
 * @param instance Indicates the pointer to the <b>CodecComponentType</b> object to release.
 */
void CodecComponentTypeRelease(struct CodecComponentType *instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CODEC_COMPONENT_H