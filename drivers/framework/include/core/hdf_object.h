/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup Core
 * @{
 *
 * @brief Provides Hardware Driver Foundation (HDF) APIs.
 *
 * The HDF implements driver framework capabilities such as driver loading, service management,
 * and driver message model. You can develop drivers based on the HDF.
 *
 * @since 1.0
 */

/**
 * @file hdf_object.h
 *
 * @brief Declares the base object provided by the HDF for the driver.
 *
 * @since 1.0
 */

#ifndef HDF_OBJECT_H
#define HDF_OBJECT_H

#include "hdf_base.h"

/**
 * @brief Describes base class objects defined by the HDF.
 *
 * @since 1.0
 */
struct HdfObject {
    int32_t objectId; /**< Base object ID */
};

#endif /* HDF_OBJECT_H */
/** @} */
