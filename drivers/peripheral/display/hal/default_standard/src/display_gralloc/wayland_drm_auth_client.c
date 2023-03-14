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

#include "wayland_drm_auth_client.h"
#include <errno.h>
#include <stdlib.h>
#include "xf86drm.h"
#include "wayland-client.h"
#include "drm-auth-client-protocol.h"
#include "display_common.h"

typedef struct {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_drm_auth *drmAuth;
    enum wl_drm_auth_status authStatus;
} WaylandDisplay;

const char *AUTH_INTERFACE_NAME = "wl_drm_auth";

static void AuthenticationStatus(void *data, struct wl_drm_auth *wlDrmAuth, uint32_t status)
{
    (void)wlDrmAuth;
    DISPLAY_LOGD("AuthenticationStatus the status %{public}d", status);
    WaylandDisplay *display = data;
    display->authStatus = status;
}

static const struct wl_drm_auth_listener g_drmAuthListener = { AuthenticationStatus };

static void RegistryHandleGlobal(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
    uint32_t version)
{
    WaylandDisplay *display = data;
    DISPLAY_LOGD("interface global : %{public}s", interface);
    if (strcmp(interface, wl_drm_auth_interface.name) == 0) {
        display->drmAuth = wl_registry_bind(registry, id, &wl_drm_auth_interface, 1);
        wl_drm_auth_add_listener(display->drmAuth, &g_drmAuthListener, display);
    }
}

static void RegistryHandleGlobalRemove(void *data, struct wl_registry *registry, uint32_t name)
{
    DISPLAY_LOGD("RegistryHandleGlobalRemove  %{publuc}d name ", name);
}

static const struct wl_registry_listener g_registrListener = { RegistryHandleGlobal, RegistryHandleGlobalRemove };

void DeInitWaylandClient(WaylandDisplay *display)
{
    DISPLAY_LOGD("DeInitWaylandClient");
    DISPLAY_CHK_RETURN_NOT_VALUE((display == NULL), DISPLAY_LOGD("display is NULL"));
    if (display->registry != NULL) {
        wl_registry_destroy(display->registry);
    }

    if (display->display != NULL) {
        wl_display_flush(display->display);
        wl_display_disconnect(display->display);
    }
    free(display);
}

WaylandDisplay *InitWaylandClient()
{
    WaylandDisplay *dsp;
    int ret;
    dsp = calloc(1, sizeof(WaylandDisplay));
    DISPLAY_CHK_RETURN((dsp == NULL), NULL, DISPLAY_LOGE("can not alloc memory errno : %{public}d", errno));
    dsp->display = wl_display_connect(NULL);
    DISPLAY_CHK_RETURN((dsp->display == NULL), NULL, DISPLAY_LOGE("display connect failed, errno: %{public}d", errno);
        DeInitWaylandClient(dsp));
    dsp->registry = wl_display_get_registry(dsp->display);
    DISPLAY_CHK_RETURN((dsp->registry == NULL), NULL, DISPLAY_LOGE("can not get registry"); DeInitWaylandClient(dsp));
    ret = wl_registry_add_listener(dsp->registry, &g_registrListener, dsp);
    DISPLAY_CHK_RETURN((ret < 0), NULL, DISPLAY_LOGE("add listener failed"));
    wl_display_roundtrip(dsp->display); // for get registry
    wl_display_roundtrip(dsp->display); // for the listener will bind the service
    return dsp;
}

int32_t WaylandDrmAuth(int drmFd)
{
    WaylandDisplay *dsp;
    drm_magic_t magic;
    int ret;
    dsp = InitWaylandClient();
    DISPLAY_CHK_RETURN((dsp == NULL), AUTH_FAILED, DISPLAY_LOGE("init wayland client failed"));
    ret = drmGetMagic(drmFd, &magic);
    DISPLAY_CHK_RETURN((ret != 0), AUTH_FAILED, DISPLAY_LOGE("can not get magic"));
    DISPLAY_CHK_RETURN((dsp->drmAuth == NULL), AUTH_FAILED, DISPLAY_LOGE("drm auth service no find"));
    wl_drm_auth_authenticate(dsp->drmAuth, magic);
    wl_display_roundtrip(dsp->display); // wait for authenticate status return
    DISPLAY_LOGD("the status of authenticate is %{public}d", dsp->authStatus);
    if (dsp->authStatus == WL_DRM_AUTH_STATUS_SUCCESS) {
        ret = AUTH_SCUCCESS;
    }
    DeInitWaylandClient(dsp);
    return ret;
}
