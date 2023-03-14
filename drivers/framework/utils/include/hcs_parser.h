/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_PARSER_H
#define HCS_PARSER_H

#include "device_resource_if.h"

bool HcsDecompile(const char *hcsBlob, uint32_t offset, struct DeviceResourceNode **root);

#endif /* HCS_PARSER_H */