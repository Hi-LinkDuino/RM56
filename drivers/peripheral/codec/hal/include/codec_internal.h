/*
 * Copyright (c) 2022 Shenzhen Kaihong DID Co., Ltd.
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

#ifndef HDI_CODECINTERNAL_H
#define HDI_CODECINTERNAL_H

#include <stdint.h>
#include "codec_callback_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CODEC_HDI_OMX_SERVICE_NAME  "codec_hdi_omx_service"

enum ComponentDispatchCmd {
    CMD_CODEC_GET_COMPONENT_NUM,
    CMD_CODEC_GET_COMPONENT_CAPABILITY_LIST,
    CMD_CREATE_COMPONENT,
    CMD_DESTROY_COMPONENT,
    CMD_GET_COMPONENT_VERSION,
    CMD_SEND_COMMAND,
    CMD_GET_PARAMETER,
    CMD_SET_PARAMETER,
    CMD_GET_CONFIG,
    CMD_SET_CONFIG,
    CMD_GET_EXTENSION_INDEX,
    CMD_GET_STATE,
    CMD_COMPONENT_TUNNEL_REQUEST,
    CMD_USE_BUFFER,
    CMD_ALLOCATE_BUFFER,
    CMD_FREE_BUFFER,
    CMD_EMPTY_THIS_BUFFER,
    CMD_FILL_THIS_BUFFER,
    CMD_SET_CALLBACKS,
    CMD_COMPONENT_DE_INIT,
    CMD_USE_EGL_IMAGE,
    CMD_COMPONENT_ROLE_ENUM,
};

enum CallbackDispatchCmd {
    CMD_EVENT_HANDLER,
    CMD_EMPTY_BUFFER_DONE,
    CMD_FILL_BUFFER_DONE,
};

struct OmxComponentManager {
    int32_t (*CreateComponent)(OMX_HANDLETYPE *compHandle, char *compName, void *appData,
        int32_t appDataSize, struct CodecCallbackType *callbacks);
    int32_t (*DestoryComponent)(OMX_HANDLETYPE compHandle);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_OMXCOMPONENTTYPE_H