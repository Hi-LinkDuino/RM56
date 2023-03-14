/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: improve heap dump.
 * Create: 2020/11/20
 */

#ifndef EXT_UTILS_H
#define EXT_UTILS_H
#include "jerryscript-core.h"

void* JerryExtAlloc(size_t size);
char* JerryExtAllocStr(size_t size);
size_t* JerryExtAllocStorageData(size_t size);
void JerryExtFree(void* ptr);

#endif
