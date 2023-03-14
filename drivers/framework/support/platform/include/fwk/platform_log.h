/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_LOG_H
#define PLATFORM_LOG_H

#include "hdf_log.h"

#if defined(__LITEOS__) || defined(__KERNEL__)
#define PLAT_LOGV(fmt, arg...)
#else
#define PLAT_LOGV(fmt, arg...) HDF_LOGV(fmt, ##arg)
#endif

#define PLAT_LOGD(fmt, arg...) HDF_LOGD(fmt, ##arg)
#define PLAT_LOGI(fmt, arg...) HDF_LOGI(fmt, ##arg)
#define PLAT_LOGW(fmt, arg...) HDF_LOGW(fmt, ##arg)
#define PLAT_LOGE(fmt, arg...) HDF_LOGE(fmt, ##arg)

#endif /* PLATFORM_LOG_H */
