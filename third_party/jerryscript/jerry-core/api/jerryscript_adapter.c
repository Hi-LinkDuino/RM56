/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef JERRY_IAR_JUPITER

#include "jerryscript.h"
#include "jmem.h"
#include "config-jupiter.h"
#include "generate-bytecode.h"
#include "los_task.h"

#define MAX_CONTEXT_NUM (g_taskMaxNum+1)

uint8_t* input_buffer;
uint8_t* snapshot_buffer;
uint8_t* bms_context_and_heap;
uint8_t* js_context_and_heap;
ContextRecord* g_contextRecords;

void JerryPsRamMemInit()
{
  // memory for js task
#ifdef INDEPENDENT_HEAP
  js_context_and_heap = OhosMalloc(MEM_TYPE_JERRY_HEAP, JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE);
#else
  js_context_and_heap = OhosMalloc(MEM_TYPE_JERRY_LSRAM, JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE);
#endif // INDEPENDENT_HEAP
}

void JerryBmsPsRamMemInit()
{
  // memory for input_js file, snapshot file and bms task
  input_buffer = OhosMalloc(MEM_TYPE_JERRY_LSRAM, INPUTJS_BUFFER_SIZE);
  snapshot_buffer = OhosMalloc(MEM_TYPE_JERRY_LSRAM, SNAPSHOT_BUFFER_SIZE);
  bms_context_and_heap = OhosMalloc(MEM_TYPE_JERRY_LSRAM, BMS_TASK_CONTEXT_AND_HEAP_SIZE * CONVERTION_RATIO);
}

void JerryInitContextRecords()
{
  g_contextRecords = (ContextRecord*)OhosMalloc(MEM_TYPE_JERRY, (MAX_CONTEXT_NUM) * sizeof(ContextRecord));
  if (g_contextRecords == NULL) {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "[JERRYSCRIPT]Init g_contextRecords Error.\n");
  }
}

#endif // JERRY_IAR_JUPITER
