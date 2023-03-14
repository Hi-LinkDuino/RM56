/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DRM_PANEL_H
#define HDF_DRM_PANEL_H
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include "hdf_disp.h"

struct HdfDrmPanel {
    struct drm_panel panel;
    struct DispManager *manager;
    struct drm_display_mode mode;
    struct mipi_dsi_device *dsiDev;
    uint32_t index;
};

#endif /* HDF_DRM_PANEL_H */

