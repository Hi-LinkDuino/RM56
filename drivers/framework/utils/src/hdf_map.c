/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_map.h"
#if defined(__KERNEL__)
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "osal_mem.h"
#include "securec.h"

struct MapNode {
    uint32_t hash;
    uint32_t valueSize;
    void *key;
    void *value;
    struct MapNode *next;
};

#define HDF_MIN_MAP_SIZE 8
#define HDF_ENLARGE_FACTOR 2
#define HDF_MAP_KEY_MAX_SIZE 1000
#define HDF_MAP_VALUE_MAX_SIZE 1000

const uint32_t HASH_SEED = 131;

/* BKDR Hash */
static uint32_t MapHash(const char *hashKey)
{
    uint32_t hashValue = 0;

    while (*hashKey) {
        hashValue = hashValue * HASH_SEED + (*hashKey++);
    }

    return (hashValue & 0x7FFFFFFF);
}

static uint32_t MapHashIdx(const Map *map, uint32_t hash)
{
    return (hash & (map->bucketSize - 1));
}

static void MapAddNode(Map *map, struct MapNode *node)
{
    uint32_t idx = MapHashIdx(map, node->hash);
    node->next = map->nodes[idx];
    map->nodes[idx] = node;
}

static int32_t MapResize(Map *map, uint32_t size)
{
    uint32_t bucketSize;
    struct MapNode **nodes = NULL;
    struct MapNode **tmp = NULL;
    uint32_t i;

    nodes = (struct MapNode **)OsalMemCalloc(size * sizeof(*nodes));
    if (nodes == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    tmp = map->nodes;
    bucketSize = map->bucketSize;
    map->nodes = nodes;
    map->bucketSize = size;

    if (tmp != NULL) {
        struct MapNode *node = NULL;
        struct MapNode *next = NULL;

        /* remap node with new map size */
        for (i = 0; i < bucketSize; i++) {
            node = tmp[i];
            while (node != NULL) {
                next = node->next;
                MapAddNode(map, node);
                node = next;
            }
        }

        OsalMemFree(tmp);
    }

    return HDF_SUCCESS;
}

static struct MapNode *MapCreateNode(const char *key, uint32_t hash,
    const void *value, uint32_t valueSize)
{
    uint32_t keySize = strlen(key) + 1;
    struct MapNode *node = (struct MapNode *)OsalMemCalloc(sizeof(*node) + keySize + valueSize);
    if (node == NULL) {
        return NULL;
    }

    node->hash = hash;
    node->key = (uint8_t *)node + sizeof(*node);
    node->value = (uint8_t *)node + sizeof(*node) + keySize;
    node->valueSize = valueSize;
    if (memcpy_s(node->key, keySize, key, keySize) != EOK) {
        OsalMemFree(node);
        return NULL;
    }
    if (memcpy_s(node->value, node->valueSize, value, valueSize) != EOK) {
        OsalMemFree(node);
        return NULL;
    }

    return node;
}

int32_t MapSet(Map *map, const char *key, const void *value, uint32_t valueSize)
{
    struct MapNode *node = NULL;
    uint32_t hash;

    if (map == NULL || key == NULL || value == NULL || valueSize == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (valueSize > HDF_MAP_KEY_MAX_SIZE || strlen(key) > HDF_MAP_VALUE_MAX_SIZE) {
        return HDF_ERR_INVALID_PARAM;
    }
    hash = MapHash(key);
    if (map->nodeSize > 0 && map->nodes != NULL) {
        uint32_t idx = MapHashIdx(map, hash);
        node = map->nodes[idx];
        while (node != NULL) {
            if (node->hash != hash || node->key == NULL || strcmp(node->key, key) != 0) {
                node = node->next;
                continue;
            }

            // size mismatch
            if (node->value == NULL || node->valueSize != valueSize) {
                return HDF_ERR_INVALID_OBJECT;
            }
            // update k-v node
            if (memcpy_s(node->value, node->valueSize, value, valueSize) != EOK) {
                return HDF_FAILURE;
            }

            return HDF_SUCCESS;
        }
    }
    // Increase the bucket size to decrease the possibility of map search conflict.
    if (map->nodeSize >= map->bucketSize) {
        uint32_t size = (map->bucketSize < HDF_MIN_MAP_SIZE) ? HDF_MIN_MAP_SIZE : \
            (map->bucketSize << HDF_ENLARGE_FACTOR);
        MapResize(map, size);
    }

    node = MapCreateNode(key, hash, value, valueSize);
    if (node == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    MapAddNode(map, node);
    map->nodeSize++;

    return HDF_SUCCESS;
}

void* MapGet(const Map *map, const char *key)
{
    uint32_t hash;
    uint32_t idx;
    struct MapNode *node = NULL;

    if (map == NULL || key == NULL || map->nodeSize == 0 || map->nodes == NULL) {
        return NULL;
    }

    hash = MapHash(key);
    idx = MapHashIdx(map, hash);
    node = map->nodes[idx];

    while (node != NULL) {
        if (node->hash == hash && node->key != NULL && !strcmp(node->key, key)) {
            return node->value;
        }

        node = node->next;
    }

    return NULL;
}

int32_t MapErase(Map *map, const char *key)
{
    uint32_t hash;
    uint32_t idx;
    struct MapNode *node = NULL;
    struct MapNode *prev = NULL;

    if (map == NULL || key == NULL || map->nodeSize == 0 || map->nodes == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    hash = MapHash(key);
    idx = MapHashIdx(map, hash);
    node = map->nodes[idx];
    prev = node;

    while (node != NULL) {
        if (node->hash == hash && node->key != NULL && !strcmp(node->key, key)) {
            if (map->nodes[idx] == node) {
                map->nodes[idx] = node->next;
            } else {
                prev->next = node->next;
            }
            OsalMemFree(node);
            map->nodeSize--;
            return HDF_SUCCESS;
        }
        prev = node;
        node = node->next;
    }

    return HDF_FAILURE;
}

void MapInit(Map *map)
{
    if (map == NULL) {
        return;
    }

    map->nodes = NULL;
    map->nodeSize = 0;
    map->bucketSize = 0;
}

void MapDelete(Map *map)
{
    uint32_t i;
    struct MapNode *node = NULL;
    struct MapNode *next = NULL;

    if (map == NULL || map->nodes == NULL) {
        return;
    }

    for (i = 0; i < map->bucketSize; i++) {
        node = map->nodes[i];
        while (node != NULL) {
            next = node->next;
            OsalMemFree(node);
            node = next;
        }
    }

    OsalMemFree(map->nodes);

    map->nodes = NULL;
    map->nodeSize = 0;
    map->bucketSize = 0;
}

