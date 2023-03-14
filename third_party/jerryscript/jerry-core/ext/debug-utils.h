
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: add debug utils.
 * Create: 2020/11/20
 */

#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "jerryscript-core.h"

void PrintObjectValueProperties(ecma_value_t value);
void PrintObjectProperties(ecma_object_t* object);
void PrintString(ecma_string_t* str);

#endif
