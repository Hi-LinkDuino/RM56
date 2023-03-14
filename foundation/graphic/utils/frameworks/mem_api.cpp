/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "gfx_utils/mem_api.h"
namespace OHOS {
#ifndef IMG_CACHE_MEMORY_CUSTOM
void *aligned_malloc(size_t required_bytes, size_t alignment)
{
    int offset = alignment - 1 + sizeof(void *);
    void *p1 = (void *)malloc(required_bytes + offset);
    if (p1 == NULL)
        return NULL;
    void **p2 = (void **)(((size_t)p1 + offset) & ~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void aligned_free(void *p2)
{
    void *p1 = ((void **)p2)[-1];
    free(p1);
}

void* ImageCacheMalloc(ImageInfo& info,size_t alignment)
{
    return aligned_malloc(info.dataSize,alignment);
}

void ImageCacheFreeAlignment(ImageInfo& info)
{
    uint8_t* buf = const_cast<uint8_t*>(info.data);
    aligned_free(buf);
    info.data = nullptr;
    if(info.header.colorMode==L8)
    {
        free(info.userData);
        info.userData = nullptr;
    }
    return;
}

void* ImageCacheMalloc(ImageInfo& info)
{
    return malloc(info.dataSize);
}

void ImageCacheFree(ImageInfo& info)
{
    uint8_t* buf = const_cast<uint8_t*>(info.data);
    free(buf);
    info.data = nullptr;
    return;
}
#endif

#if !ENABLE_MEMORY_HOOKS
void* UIMalloc(uint32_t size)
{
    return malloc(size);
}

void UIFree(void* buffer)
{
    free(buffer);
}

void* UIRealloc(void* buffer, uint32_t size)
{
    return realloc(buffer, size);
}
#endif
} // namespace OHOS
