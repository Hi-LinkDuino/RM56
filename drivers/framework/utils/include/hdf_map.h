/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_MAP_H
#define HDF_MAP_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct MapNode;

typedef struct {
    struct MapNode **nodes; /**< Map node bucket */
    uint32_t nodeSize; /**< Map node count */
    uint32_t bucketSize; /**< Map node bucket size */
} Map;

void MapInit(Map *map);

void MapDelete(Map *map);

int32_t MapSet(Map *map, const char *key, const void *value, uint32_t valueSize);

void *MapGet(const Map *map, const char *key);

int32_t MapErase(Map *map, const char *key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_MAP_H */
