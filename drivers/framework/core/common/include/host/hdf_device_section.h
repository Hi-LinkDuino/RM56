/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_SECTION_H
#define HDF_DEVICE_SECTION_H

#if !defined(NON_HDF_DRIVER_SECTION)
#define USED_ATTR __attribute__((used))
#define HDF_SECTION __attribute__((section(".hdf.driver")))
#define HDF_DRIVER_INIT(module) \
    const size_t USED_ATTR module##HdfEntry HDF_SECTION = (size_t)(&(module))

#if (defined(__GNUC__) || defined(__clang__))
#define HDF_DRIVER_SEC_NAME(type, name) \
        ({extern type name;            \
           &name;                       \
        })
#define HDF_DRIVER_BEGIN() HDF_DRIVER_SEC_NAME(size_t, _hdf_drivers_start)
#define HDF_DRIVER_END() HDF_DRIVER_SEC_NAME(size_t, _hdf_drivers_end)

#elif defined(__ICCARM__)
#define HDF_DRIVER_BEGIN() __section_begin(".hdf.driver")
#define HDF_DRIVER_END() __section_end(".hdf.driver")
#pragma section = ".hdf.driver"
#else
#error "No support section begin and section end!"
#endif /* defined(__GNUC__) || defined(__clang__) */
#endif /* NON_HDF_DRIVER_SECTION */

#endif /* HDF_DEVICE_SECTION_H */

