/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "camera_metadata_operator.h"
#include <cstring>
#include <securec.h>
#include "camera_metadata_item_info.h"
#include "metadata_log.h"

namespace OHOS {
namespace CameraStandard {
const int METADATA_HEADER_DATA_SIZE = 4;

uint32_t AlignTo(uint32_t val, uint32_t alignment)
{
    return (((uintptr_t)(val) + ((alignment) - 1)) & ~((alignment) - 1));
}

uint32_t MaxAlignment(uint32_t dataAlignment, uint32_t metadataAlignment)
{
    return ((dataAlignment > metadataAlignment) ? dataAlignment : metadataAlignment);
}

uint8_t *GetMetadataData(const common_metadata_header_t *metadataHeader)
{
    return (uint8_t *)metadataHeader + metadataHeader->data_start;
}

camera_metadata_item_entry_t *GetMetadataItems(const common_metadata_header_t *metadataHeader)
{
    return (camera_metadata_item_entry_t *)
        ((uint8_t *)metadataHeader + metadataHeader->items_start);
}

common_metadata_header_t *FillCameraMetadata(common_metadata_header_t *buffer, size_t memoryRequired,
                                             uint32_t itemCapacity, uint32_t dataCapacity)
{
    METADATA_DEBUG_LOG("FillCameraMetadata start");
    if (buffer == nullptr) {
        METADATA_ERR_LOG("FillCameraMetadata buffer is null");
        return nullptr;
    }

    common_metadata_header_t *metadataHeader = (common_metadata_header_t *)buffer;
    metadataHeader->version = CURRENT_CAMERA_METADATA_VERSION;
    metadataHeader->size = memoryRequired;
    metadataHeader->item_count = 0;
    metadataHeader->item_capacity = itemCapacity;
    metadataHeader->items_start = AlignTo(sizeof(common_metadata_header_t), ITEM_ALIGNMENT);
    metadataHeader->data_count = 0;
    metadataHeader->data_capacity = dataCapacity;
    size_t dataUnaligned = (uint8_t *)(GetMetadataItems(metadataHeader) +
                                metadataHeader->item_capacity) - (uint8_t *)metadataHeader;
    metadataHeader->data_start = AlignTo(dataUnaligned, DATA_ALIGNMENT);

    METADATA_DEBUG_LOG("FillCameraMetadata end");
    return metadataHeader;
}

size_t CalculateCameraMetadataMemoryRequired(uint32_t itemCount, uint32_t dataCount)
{
    METADATA_DEBUG_LOG("CalculateCameraMetadataMemoryRequired start");
    size_t memoryRequired = sizeof(common_metadata_header_t);
    memoryRequired = AlignTo(memoryRequired, ITEM_ALIGNMENT);

    memoryRequired += sizeof(camera_metadata_item_entry_t[itemCount]);
    memoryRequired = AlignTo(memoryRequired, DATA_ALIGNMENT);

    memoryRequired += sizeof(uint8_t[dataCount]);
    memoryRequired = AlignTo(memoryRequired, METADATA_PACKET_ALIGNMENT);

    METADATA_DEBUG_LOG("CalculateCameraMetadataMemoryRequired memoryRequired: %{public}zu", memoryRequired);
    METADATA_DEBUG_LOG("CalculateCameraMetadataMemoryRequired end");
    return memoryRequired;
}

common_metadata_header_t *AllocateCameraMetadataBuffer(uint32_t item_capacity, uint32_t data_capacity)
{
    METADATA_DEBUG_LOG("AllocateCameraMetadataBuffer start");
    METADATA_DEBUG_LOG("AllocateCameraMetadataBuffer item_capacity: %{public}d, data_capacity: %{public}d",
        item_capacity, data_capacity);
    size_t memoryRequired = CalculateCameraMetadataMemoryRequired(item_capacity, data_capacity);
    void *buffer = calloc(1, memoryRequired);
    if (buffer == nullptr) {
        METADATA_ERR_LOG("AllocateCameraMetadataBuffer memory allocation failed");
        return (common_metadata_header_t *) buffer;
    }

    common_metadata_header_t *metadataHeader = FillCameraMetadata((common_metadata_header_t *)buffer, memoryRequired,
                                                                  item_capacity, data_capacity);
    if (metadataHeader == nullptr) {
        METADATA_ERR_LOG("AllocateCameraMetadataBuffer metadataHeader is null");
        free(buffer);
    }

    METADATA_DEBUG_LOG("AllocateCameraMetadataBuffer end");
    return metadataHeader;
}

int32_t GetMetadataSection(uint32_t itemSection, uint32_t *section)
{
    METADATA_DEBUG_LOG("GetMetadataSection start");
    if (itemSection < OHOS_CAMERA_PROPERTIES ||
        itemSection >= OHOS_ABILITY_SECTION_END) {
        METADATA_ERR_LOG("GetMetadataSection itemSection is not valid");
        return CAM_META_FAILURE;
    }

    int32_t ret = CAM_META_SUCCESS;
    switch (itemSection) {
        case OHOS_CAMERA_PROPERTIES:
            *section = OHOS_SECTION_CAMERA_PROPERTIES;
            break;
        case OHOS_CAMERA_SENSOR:
            *section = OHOS_SECTION_CAMERA_SENSOR;
            break;
        case OHOS_CAMERA_SENSOR_INFO:
            *section = OHOS_SECTION_CAMERA_SENSOR_INFO;
            break;
        case OHOS_CAMERA_STATISTICS:
            *section = OHOS_SECTION_CAMERA_STATISTICS;
            break;
        case OHOS_DEVICE_CONTROL:
            *section = OHOS_SECTION_CAMERA_CONTROL;
            break;
        case OHOS_DEVICE_EXPOSURE:
            *section = OHOS_SECTION_DEVICE_EXPOSURE;
            break;
        case OHOS_DEVICE_FOCUS:
            *section = OHOS_SECTION_DEVICE_FOCUS;
            break;
        case OHOS_DEVICE_FLASH:
            *section = OHOS_SECTION_DEVICE_FLASH;
            break;
        case OHOS_DEVICE_ZOOM:
            *section = OHOS_SECTION_DEVICE_ZOOM;
            break;
        case OHOS_STREAM_ABILITY:
            *section = OHOS_SECTION_STREAM_ABILITY;
            break;
        case OHOS_STREAM_JPEG:
            *section = OHOS_SECTION_STREAM_JPEG;
            break;
        default:
            METADATA_ERR_LOG("GetMetadataSection item section is not defined");
            ret = CAM_META_FAILURE;
            break;
    }

    METADATA_DEBUG_LOG("GetMetadataSection end");
    return ret;
}

int32_t GetCameraMetadataItemType(uint32_t item, uint32_t *dataType)
{
    METADATA_DEBUG_LOG("GetCameraMetadataItemType start");
    uint32_t section;
    int32_t ret = GetMetadataSection(item >> BITWISE_SHIFT_16, &section);
    if (ret != CAM_META_SUCCESS) {
        METADATA_ERR_LOG("GetCameraMetadataItemType section is not valid");
        return ret;
    }

    if (item >= g_ohosCameraSectionBounds[section][1]) {
        METADATA_ERR_LOG("GetCameraMetadataItemType item is not in section bound");
        return CAM_META_FAILURE;
    }

    uint32_t itemIndex = item & 0xFFFF;
    if (g_ohosItemInfo[section][itemIndex].item_type < META_TYPE_BYTE) {
        METADATA_ERR_LOG("GetCameraMetadataItemType item is not initialized");
        return CAM_META_FAILURE;
    }

    *dataType = g_ohosItemInfo[section][itemIndex].item_type;

    METADATA_DEBUG_LOG("GetCameraMetadataItemType end");
    return CAM_META_SUCCESS;
}

const char *GetCameraMetadataItemName(uint32_t item)
{
    METADATA_DEBUG_LOG("GetCameraMetadataItemName start");
    METADATA_DEBUG_LOG("GetCameraMetadataItemName item: %{public}d", item);
    uint32_t section;
    int32_t ret = GetMetadataSection(item >> BITWISE_SHIFT_16, &section);
    if (ret != CAM_META_SUCCESS) {
        METADATA_ERR_LOG("GetCameraMetadataItemName section is not valid");
        return nullptr;
    }

    if (item >= g_ohosCameraSectionBounds[section][1]) {
        METADATA_ERR_LOG("GetCameraMetadataItemName item is not in section bound");
        return nullptr;
    }

    uint32_t itemIndex = item & 0xFFFF;
    METADATA_DEBUG_LOG("GetCameraMetadataItemName end");
    return g_ohosItemInfo[section][itemIndex].item_name;
}

size_t CalculateCameraMetadataItemDataSize(uint32_t type, size_t dataCount)
{
    METADATA_DEBUG_LOG("CalculateCameraMetadataItemDataSize start");
    if (type < META_TYPE_BYTE || type >= META_NUM_TYPES) {
        METADATA_ERR_LOG("CalculateCameraMetadataItemDataSize invalid type");
        return 0;
    }

    size_t dataBytes = dataCount * OHOS_CAMERA_METADATA_TYPE_SIZE[type];

    METADATA_DEBUG_LOG("CalculateCameraMetadataItemDataSize end");
    return (dataBytes <= METADATA_HEADER_DATA_SIZE) ? 0 : AlignTo(dataBytes, DATA_ALIGNMENT);
}

int AddCameraMetadataItem(common_metadata_header_t *dst, uint32_t item, const void *data, size_t dataCount)
{
    METADATA_DEBUG_LOG("AddCameraMetadataItem start");
    METADATA_DEBUG_LOG("AddCameraMetadataItem item: %{public}d, dataCount: %{public}zu", item, dataCount);
    if (dst == nullptr) {
        METADATA_ERR_LOG("AddCameraMetadataItem common_metadata_header_t is null");
        return CAM_META_INVALID_PARAM;
    }

    if (dst->item_count == dst->item_capacity) {
        METADATA_ERR_LOG("AddCameraMetadataItem item_capacity limit reached. "
                         "item_count: %{public}d, item_capacity: %{public}d",
                         dst->item_count, dst->item_capacity);
        return CAM_META_ITEM_CAP_EXCEED;
    }

    if (dataCount && data == nullptr) {
        METADATA_ERR_LOG("AddCameraMetadataItem data is not valid. dataCount: %{public}zu, data: %{public}p",
                         dataCount, data);
        return CAM_META_INVALID_PARAM;
    }

    uint32_t dataType;
    int32_t ret = GetCameraMetadataItemType(item, &dataType);
    if (ret != CAM_META_SUCCESS) {
        METADATA_ERR_LOG("AddCameraMetadataItem invalid item type");
        return CAM_META_INVALID_PARAM;
    }

    size_t dataBytes =
            CalculateCameraMetadataItemDataSize(dataType, dataCount);
    if (dataBytes + dst->data_count > dst->data_capacity) {
        METADATA_ERR_LOG("AddCameraMetadataItem data_capacity limit reached");
        return CAM_META_DATA_CAP_EXCEED;
    }

    size_t dataPayloadBytes =
            dataCount * OHOS_CAMERA_METADATA_TYPE_SIZE[dataType];
    camera_metadata_item_entry_t *metadataItem = GetMetadataItems(dst) + dst->item_count;
    memset_s(metadataItem, sizeof(camera_metadata_item_entry_t), 0, sizeof(camera_metadata_item_entry_t));
    metadataItem->item = item;
    metadataItem->data_type = dataType;
    metadataItem->count = dataCount;

    if (dataBytes == 0) {
        ret = memcpy_s(metadataItem->data.value, dataPayloadBytes, data, dataPayloadBytes);
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("AddCameraMetadataItem memory copy failed");
            return CAM_META_FAILURE;
        }
    } else {
        metadataItem->data.offset = dst->data_count;
        ret = memcpy_s(GetMetadataData(dst) + metadataItem->data.offset, dataPayloadBytes, data,
            dataPayloadBytes);
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("AddCameraMetadataItem memory copy failed");
            return CAM_META_FAILURE;
        }
        dst->data_count += dataBytes;
    }
    dst->item_count++;

    METADATA_DEBUG_LOG("AddCameraMetadataItem end");
    return CAM_META_SUCCESS;
}

int GetCameraMetadataItem(const common_metadata_header_t *src, uint32_t index, camera_metadata_item_t *item)
{
    METADATA_DEBUG_LOG("GetCameraMetadataItem start");
    if (src == nullptr || item == nullptr) {
        METADATA_ERR_LOG("GetCameraMetadataItem src or item is null");
        return CAM_META_INVALID_PARAM;
    }

    memset_s(item, sizeof(camera_metadata_item_t), 0, sizeof(camera_metadata_item_t));

    if (index >= src->item_count) {
        METADATA_ERR_LOG("GetCameraMetadataItem index is greater than item count");
        return CAM_META_INVALID_PARAM;
    }

    camera_metadata_item_entry_t *localItem = GetMetadataItems(src) + index;

    item->index = index;
    item->item = localItem->item;
    item->data_type = localItem->data_type;
    item->count = localItem->count;

    size_t dataBytes = CalculateCameraMetadataItemDataSize(localItem->data_type, localItem->count);
    if (dataBytes == 0) {
        item->data.u8 = localItem->data.value;
    } else {
        item->data.u8 = GetMetadataData(src) + localItem->data.offset;
    }

    METADATA_DEBUG_LOG("GetCameraMetadataItem end");
    return CAM_META_SUCCESS;
}

int FindCameraMetadataItemIndex(const common_metadata_header_t *src, uint32_t item, uint32_t *idx)
{
    METADATA_DEBUG_LOG("FindCameraMetadataItemIndex start");
    METADATA_DEBUG_LOG("FindCameraMetadataItemIndex item: %{public}d", item);
    if (src == nullptr) {
        METADATA_ERR_LOG("FindCameraMetadataItemIndex src is null");
        return CAM_META_INVALID_PARAM;
    }

    camera_metadata_item_entry_t *searchItem = GetMetadataItems(src);
    uint32_t index;
    for (index = 0; index < src->item_count; index++, searchItem++) {
        if (searchItem->item == item) {
            break;
        }
    }

    if (index == src->item_count) {
        METADATA_ERR_LOG("FindCameraMetadataItemIndex item not found");
        return CAM_META_ITEM_NOT_FOUND;
    }

    *idx = index;
    METADATA_DEBUG_LOG("FindCameraMetadataItemIndex index: %{public}d", index);
    METADATA_DEBUG_LOG("FindCameraMetadataItemIndex end");
    return CAM_META_SUCCESS;
}

int FindCameraMetadataItem(const common_metadata_header_t *src, uint32_t item, camera_metadata_item_t *metadataItem)
{
    uint32_t index = 0;
    int ret = FindCameraMetadataItemIndex(src, item, &index);
    if (ret != CAM_META_SUCCESS) {
        return ret;
    }

    return GetCameraMetadataItem(src, index, metadataItem);
}

void SetOffset(camera_metadata_item_entry_t *metadataItems, camera_metadata_item_entry_t *item, size_t oldItemSize)
{
    if (CalculateCameraMetadataItemDataSize(metadataItems->data_type, metadataItems->count) > 0 &&
        metadataItems->data.offset > item->data.offset) {
        metadataItems->data.offset -= oldItemSize;
    }
}

int UpdateCameraMetadataItemByIndex(common_metadata_header_t *dst, uint32_t index, const void *data, uint32_t dataCount,
                                    camera_metadata_item_t *updatedItem)
{
    METADATA_DEBUG_LOG("UpdateCameraMetadataItemByIndex start");
    if (dst == nullptr) {
        METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex dst is null");
        return CAM_META_INVALID_PARAM;
    }

    if (index >= dst->item_count) {
        METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex index not valid");
        return CAM_META_INVALID_PARAM;
    }

    int32_t ret = CAM_META_SUCCESS;
    camera_metadata_item_entry_t *item = GetMetadataItems(dst) + index;
    size_t dataSize = CalculateCameraMetadataItemDataSize(item->data_type, dataCount);
    size_t dataPayloadSize = dataCount * OHOS_CAMERA_METADATA_TYPE_SIZE[item->data_type];

    size_t oldItemSize = CalculateCameraMetadataItemDataSize(item->data_type, item->count);
    if (dataSize != oldItemSize) {
        if (dst->data_capacity < (dst->data_count + dataSize - oldItemSize)) {
            METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex data_capacity limit reached");
            return CAM_META_DATA_CAP_EXCEED;
        }

        if (oldItemSize != 0) {
            uint8_t *start = GetMetadataData(dst) + item->data.offset;
            uint8_t *end = start + oldItemSize;
            size_t length = dst->data_count - item->data.offset - oldItemSize;
            if (length != 0) {
                ret = memmove_s(start, length, end, length);
                if (ret != CAM_META_SUCCESS) {
                    METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex memory move failed");
                    return CAM_META_FAILURE;
                }
            }
            dst->data_count -= oldItemSize;

            camera_metadata_item_entry_t *metadataItems = GetMetadataItems(dst);
            for (uint32_t i = 0; i < dst->item_count; i++, ++metadataItems) {
                SetOffset(metadataItems, item, oldItemSize);
            }
        }

        if (dataSize != 0) {
            item->data.offset = dst->data_count;
            ret = memcpy_s(GetMetadataData(dst) + item->data.offset, dataPayloadSize, data, dataPayloadSize);
            if (ret != CAM_META_SUCCESS) {
                METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex memory copy failed");
                return CAM_META_FAILURE;
            }
            dst->data_count += dataSize;
        }
    } else if (dataSize != 0) {
        ret = memcpy_s(GetMetadataData(dst) + item->data.offset, dataPayloadSize, data, dataPayloadSize);
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex memory copy failed");
            return CAM_META_FAILURE;
        }
    }

    if (dataSize == 0) {
        ret = memcpy_s(item->data.value, dataPayloadSize, data, dataPayloadSize);
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("UpdateCameraMetadataItemByIndex memory copy failed");
            return CAM_META_FAILURE;
        }
    }

    item->count = dataCount;
    if (updatedItem != nullptr) {
        GetCameraMetadataItem(dst, index, updatedItem);
    }

    METADATA_DEBUG_LOG("UpdateCameraMetadataItemByIndex end");
    return CAM_META_SUCCESS;
}

int UpdateCameraMetadataItem(common_metadata_header_t *dst, uint32_t item, const void *data,
                             uint32_t dataCount, camera_metadata_item_t *updatedItem)
{
    METADATA_DEBUG_LOG("UpdateCameraMetadataItem item: %{public}d, dataCount: %{public}d", item, dataCount);
    uint32_t index = 0;
    uint32_t ret = FindCameraMetadataItemIndex(dst, item, &index);
    if (ret != CAM_META_SUCCESS) {
        return ret;
    }

    return UpdateCameraMetadataItemByIndex(dst, index, data, dataCount, updatedItem);
}

int DeleteCameraMetadataItemByIndex(common_metadata_header_t *dst, uint32_t index)
{
    METADATA_DEBUG_LOG("DeleteCameraMetadataItemByIndex start");
    if (dst == nullptr) {
        METADATA_ERR_LOG("DeleteCameraMetadataItemByIndex dst is null");
        return CAM_META_INVALID_PARAM;
    }

    if (index >= dst->item_count) {
        METADATA_ERR_LOG("DeleteCameraMetadataItemByIndex item not valid");
        return CAM_META_INVALID_PARAM;
    }

    int32_t ret = CAM_META_SUCCESS;
    camera_metadata_item_entry_t *itemToDelete = GetMetadataItems(dst) + index;
    size_t dataBytes = CalculateCameraMetadataItemDataSize(itemToDelete->data_type, itemToDelete->count);
    if (dataBytes > 0) {
        uint8_t *start = GetMetadataData(dst) + itemToDelete->data.offset;
        uint8_t *end = start + dataBytes;
        size_t length = dst->data_count - itemToDelete->data.offset - dataBytes;
        if (length != 0) {
            ret = memmove_s(start, length, end, length);
            if (ret != CAM_META_SUCCESS) {
                METADATA_ERR_LOG("DeleteCameraMetadataItemByIndex memory move failed");
                return CAM_META_FAILURE;
            }
        }
        dst->data_count -= dataBytes;

        camera_metadata_item_entry_t *metadataItems = GetMetadataItems(dst);
        for (uint32_t i = 0; i < dst->item_count; i++, ++metadataItems) {
            if (CalculateCameraMetadataItemDataSize(
                metadataItems->data_type, metadataItems->count) > 0 &&
                metadataItems->data.offset > itemToDelete->data.offset) {
                metadataItems->data.offset -= dataBytes;
            }
        }
    }

    int32_t length = sizeof(camera_metadata_item_entry_t) * (dst->item_count - index - 1);
    if (length != 0) {
        ret = memmove_s(itemToDelete, length, itemToDelete + 1, length);
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("DeleteCameraMetadataItemByIndex memory move failed");
            return CAM_META_FAILURE;
        }
    }
    dst->item_count -= 1;
    METADATA_DEBUG_LOG("DeleteCameraMetadataItemByIndex end");
    return CAM_META_SUCCESS;
}

int DeleteCameraMetadataItem(common_metadata_header_t *dst, uint32_t item)
{
    METADATA_DEBUG_LOG("DeleteCameraMetadataItem item: %{public}d", item);
    uint32_t index = 0;
    uint32_t ret = FindCameraMetadataItemIndex(dst, item, &index);
    if (ret != CAM_META_SUCCESS) {
        return ret;
    }

    return DeleteCameraMetadataItemByIndex(dst, index);
}

void FreeCameraMetadataBuffer(common_metadata_header_t *dst)
{
    if (dst != nullptr) {
        free(dst);
    }
}

uint32_t GetCameraMetadataItemCount(const common_metadata_header_t *metadataHeader)
{
    return metadataHeader->item_count;
}

uint32_t GetCameraMetadataItemCapacity(const common_metadata_header_t *metadataHeader)
{
    return metadataHeader->item_capacity;
}

uint32_t GetCameraMetadataDataSize(const common_metadata_header_t *metadataHeader)
{
    return metadataHeader->data_capacity;
}

uint32_t CopyCameraMetadataItems(common_metadata_header_t *newMetadata, const common_metadata_header_t *oldMetadata)
{
    if (newMetadata == nullptr || oldMetadata == nullptr) {
        return CAM_META_INVALID_PARAM;
    }

    int32_t ret;
    if (oldMetadata->item_count != 0) {
        ret = memcpy_s(GetMetadataItems(newMetadata), sizeof(camera_metadata_item_entry_t[oldMetadata->item_count]),
            GetMetadataItems(oldMetadata), sizeof(camera_metadata_item_entry_t[oldMetadata->item_count]));
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("CopyCameraMetadataItems memory copy failed");
            return CAM_META_FAILURE;
        }
    }

    if (oldMetadata->data_count != 0) {
        ret = memcpy_s(GetMetadataData(newMetadata), sizeof(uint8_t[oldMetadata->data_count]),
            GetMetadataData(oldMetadata), sizeof(uint8_t[oldMetadata->data_count]));
        if (ret != CAM_META_SUCCESS) {
            METADATA_ERR_LOG("CopyCameraMetadataItems memory copy failed");
            return CAM_META_FAILURE;
        }
    }

    newMetadata->item_count = oldMetadata->item_count;
    newMetadata->data_count = oldMetadata->data_count;

    return CAM_META_SUCCESS;
}
} // CameraStandard
} // OHOS