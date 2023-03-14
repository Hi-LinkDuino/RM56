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

#ifndef HISILICON_DRM_H
#define HISILICON_DRM_H

#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif
#define DRM_HISILICON_GEM_FD_TO_PHYADDR 0x1

struct DrmHisiliconPhyaddr {
    /* * return the physical address */
    __u64 phyaddr;
    /* * dmabuf file descriptor */
    __s32 fd;
};

#define DRM_IOCTL_HISILICON_GEM_FD_TO_PHYADDR \
    DRM_IOWR(DRM_COMMAND_BASE + DRM_HISILICON_GEM_FD_TO_PHYADDR, struct DrmHisiliconPhyaddr)

#if defined(__cplusplus)
}
#endif

#endif // HISILICON_DRM_H
