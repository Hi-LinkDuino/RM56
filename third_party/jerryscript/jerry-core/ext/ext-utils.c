/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: improve heap dump.
 * Create: 2020/11/20
 */

#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ext-utils.h"
#include "vm.h"

#include "stdlib.h"

void JerryExtFree(void* ptr)
{
  free(ptr);
}

void* JerryExtAlloc(size_t size)
{
  return malloc(size);
}

char* JerryExtAllocStr(size_t size)
{
  return (char*)malloc(size);
}

size_t* JerryExtAllocStorageData(size_t size)
{
  return (size_t*)calloc(size, sizeof(size_t));
}
