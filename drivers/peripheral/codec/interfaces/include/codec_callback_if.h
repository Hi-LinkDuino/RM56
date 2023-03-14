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
 * @file codec_callback_if.h
 *
 * @brief Defines the callbacks used to report codec events and processing results of the input and output buffers.
 *
 * @since 3.1
 */

#ifndef CODEC_CALLBACK_TYPE_H
#define CODEC_CALLBACK_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include "codec_component_type.h"
#include "OMX_Core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Defines the callbacks of the Codec module.
 *
 * The following callbacks are provided:
 * 1. Callback used to report component error events, command completion events, and port setting events.
 * For details, see {@link EventHandler}.
 * 2. Callback invoked when the input port processes data in the buffer. For details, see {@link EmptyBufferDone}.
 * 3. Callback invoked when the output port fills data into the buffer. For details, see {@link FillBufferDone}.
 * The callbacks are registered by using:
 * 1. {@link CreateComponent} when a component is created.
 * 2. {@link SetCallbacks} when the component is in the OMX_StateLoaded state.
 */
struct CodecCallbackType {
    struct HdfRemoteService *remote;

    /**
     * @brief Reports an event, such as an error, a command completion event, and port setting changes
     * during the running of a component.
     *
     * When <b>eEvent</b> is <b>OMX_EventCmdComplete</b>, <b>eventData</b> is null, and <b>data1</b> is
     * <b>OMX_COMMANDTYPE</b>, <b>data1</b> indicates a state, if <b>data1</b> is <b>OMX_CommandStateSet</b>
     * and indicates a port in other cases.
     * If <b>eEvent</b> is <b>OMX_EventError</b>, <b>data1</b> indicates an error code and <b>data2</b> and
     * <b>eventData</b> are both <b>0</b>.
     * If <b>eEvent</b> is <b>OMX_EventMark</b>, <b>data1</b> and <b>data2</b> are both <b>0</b> and
     * <b>eventData</b> points to the mark.
     * When <b>eEvent</b> is <b>OMX_EventPortSettingsChanged</b>, <b>data1</b> indicates a port and
     * <b>data2</b> and <b>eventData</b> are <b>0</b>.
     * When <b>eEvent</b> is <b>OMX_EventBufferFlag</b>, <b>data1</b> indicates a port, <b>data2</b> indicates a flag,
     * and <b>eventData</b> is <b>0</b>.
     * When <b>eEvent</b> is <b>OMX_EventResourcesAcquired</b> or <b>OMX_EventDynamicResourcesAvailable</b>,
     * the values of <b>data1</b>, <b>data2</b>, and <b>eventData</b> are <b>0</b>.
     *
     * @param self Indicates the pointer to the callback to be invoked.
     * @param appData Indicates the pointer to the upper-layer instance passed to the callback.
     * @param appDataLen Indicates the length of <b>appData</b>, in bytes.
     * @param eEvent Indicates the type of events to report. For details, see {@link OMX_EVENTTYPE}.
     * @param data1 Indicates data 1 carried in the event.
     * @param data2 Indicates data 2 carried in the event.
     * @param eventData Indicates the pointer to the data carried in the event.
     * @param eventDataLen Indicates the length of <b>eventData</b>, in bytes.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*EventHandler)(struct CodecCallbackType *self, int8_t *appData, uint32_t appDataLen,
        enum OMX_EVENTTYPE eEvent, uint32_t data1, uint32_t data2, int8_t *eventData, uint32_t eventDataLen);

    /**
     * @brief Reports an event indicating that the encoding or decoding in the input buffer is complete.
     *
     * @param self Indicates the pointer to the callback to be invoked.
     * @param appData Indicates the pointer to the upper-layer instance passed to the callback.
     * @param appDataLen Indicates the length of <b>appData</b>, in bytes.
     * @param buffer Indicates the pointer to the input buffer {@link OmxCodecBuffer} that has data processed.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*EmptyBufferDone)(struct CodecCallbackType *self, int8_t *appData,
        uint32_t appDataLen, const struct OmxCodecBuffer *buffer);

    /**
     * @brief Reports an event indicating that the output buffer is filled.
     *
     * @param self Indicates the pointer to the callback to be invoked.
     * @param appData Indicates the pointer to the upper-layer instance passed to the callback.
     * @param appDataLen Indicates the length of <b>appData</b>, in bytes.
     * @param buffer Indicates the pointer to the buffer {@link OmxCodecBuffer} that has data filled.
     *
     * @return Returns <b>HDF_SUCCESS</b> if the operation is successful.
     * @return Returns <b>HDF_ERR_INVALID_PARAM</b> if the operation failed due to invalid parameters.
     * @return Returns <b>HDF_ERR_INVALID_OBJECT</b> if the operation failed due to invalid objects.
     * @return Returns <b>HDF_ERR_MALLOC_FAIL</b> if the operation failed due to insufficient memory.
     */
    int32_t (*FillBufferDone)(struct CodecCallbackType *self, int8_t* appData,
        uint32_t appDataLen, struct OmxCodecBuffer* buffer);
};

/**
 * @brief Instantiates a <b>CodecCallbackType</b> object.
 *
 * @param remote Indicates the pointer to the <b>HdfRemoteService</b>.
 *
 * @return Returns the <b>CodecCallbackType</b> object instantiated.
 */
struct CodecCallbackType *CodecCallbackTypeGet(struct HdfRemoteService *remote);

/**
 * @brief Releases a <b>CodecCallbackType</b> instance.
 *
 * @param instance Indicates the pointer to the <b>CodecCallbackType</b> instance to release.
 */
void CodecCallbackTypeRelease(struct CodecCallbackType *instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CODEC_CALLBACK_TYPE_H