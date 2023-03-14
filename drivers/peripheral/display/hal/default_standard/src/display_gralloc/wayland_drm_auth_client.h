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

#ifndef WAYLAND_DRM_AUTH_CLIENT_H
#define WAYLAND_DRM_AUTH_CLIENT_H
#include <stdint.h>

typedef enum {
    AUTH_SCUCCESS = 0, /* * authenticate sucess */
    AUTH_FAILED = 1    /* * authenticate failed */
} AuthStatus;

/* *
 * @brief authenticate the drm fd
 *
 * it will connect to the wayland server, and will block to authenticate the drm fd, then disconnect the wayland
 *
 * @param display Indicates the pointer of wayland display
 *
 * @param drmFd Indicates the file descriptor of drm device
 *
 * @return Returns <b>AUTH_SCUCCESS</b> if the operation is successful else returns AUTH_FAILED
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t WaylandDrmAuth(int drmFd);

#endif // WAYLAND_DRM_AUTH_CLIENT_H
