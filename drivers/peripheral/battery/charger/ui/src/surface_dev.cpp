/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "surface_dev.h"
#include <cerrno>
#include <cstring>
#include <drm_fourcc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <linux/fb.h>
#include "securec.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
struct BufferObject {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t handle;
    uint32_t size;
    uint8_t* vaddr;
    uint32_t fbId;
};

struct BufferObject g_buff;

void SurfaceDev::Flip(char* buf)
{
    if (!buf) {
        BATTERY_HILOGE(FEATURE_CHARGING, "buf is null.");
        return;
    }
    if (memcpy_s(g_buff.vaddr, g_buff.size, static_cast<void*>(buf), g_buff.size) != EOK) {
        BATTERY_HILOGE(FEATURE_CHARGING, "memcpy_s fail.");
        return;
    }
}

static int32_t ModesetCreateFb(int32_t fd, struct BufferObject* bo)
{
    struct drm_mode_create_dumb create = {};
    struct drm_mode_map_dumb map = {};
    const int32_t offsetNumber = 4;
    uint32_t handles[offsetNumber] = {0};
    uint32_t pitches[offsetNumber] = {0};
    uint32_t offsets[offsetNumber] = {0};
    int32_t ret;

    /* create a dumb-buffer, the pixel format is XRGB888 */
    const int32_t pixelDepth = 32;
    create.width = bo->width;
    create.height = bo->height;
    create.bpp = pixelDepth;
    drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);

    /* bind the dumb-buffer to an FB object */
    bo->pitch = create.pitch;
    bo->size = create.size;
    bo->handle = create.handle;

    handles[0] = bo->handle;
    pitches[0] = bo->pitch;
    offsets[0] = 0;
    ret = drmModeAddFB2(fd, bo->width, bo->height, DRM_FORMAT_ARGB8888, handles, pitches, offsets, &bo->fbId, 0);
    if (ret) {
        BATTERY_HILOGD(FEATURE_CHARGING, "[fbtest]failed to add fb %{public}d x %{public}d: errno %{public}s", \
            bo->width, bo->height, strerror(errno));
        return -1;
    }

    /* map the dumb-buffer to userspace */
    map.handle = create.handle;
    drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);
    bo->vaddr = static_cast<uint8_t*>(mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset));
    const uint32_t newColor = 0xff00ff00;
    uint32_t i = 0;
    uint32_t color = newColor;
    while (i < bo->size) {
        if (memcpy_s(&bo->vaddr[i], bo->size, &color, sizeof(color)) != EOK) {
            BATTERY_HILOGE(FEATURE_CHARGING, "memcpy_s fail.");
            return -1;
        }
        i += sizeof(color);
    }
    return 0;
}

int32_t DrmInit(void)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init drm");
    int32_t fd = -1;
    drmModeConnector* conn;
    uint32_t connId;
    uint32_t crtcId;
    fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "open failed.");
        return -1;
    }

    drmModeRes* res = drmModeGetResources(fd);
    if (res == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "res is nullptr");
        return -1;
    }

    crtcId = res->crtcs[0];
    connId = res->connectors[1];
    conn = drmModeGetConnector(fd, connId);
    if (conn == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "conn is nullptr");
        return -1;
    }
    g_buff.width = conn->modes[0].hdisplay;
    g_buff.height = conn->modes[0].vdisplay;

    ModesetCreateFb(fd, &g_buff);
    drmModeSetCrtc(fd, crtcId, g_buff.fbId, 0, 0, &connId, 1, &conn->modes[0]);
    BATTERY_HILOGD(FEATURE_CHARGING, "drm init success");
    return 0;
}

SurfaceDev::SurfaceDev(SurfaceDev::DevType devType)
{
    if (devType == SurfaceDev::DevType::DRM_DEVICE) {
        DrmInit();
    } else {
        BATTERY_HILOGE(FEATURE_CHARGING, "only support drm driver.");
    }
}

void SurfaceDev::GetScreenSize(int32_t& w, int32_t& h)
{
    const int32_t screenSizeW = 480;
    const int32_t screenSizeH = 960;
    w = screenSizeW;
    h = screenSizeH;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
