/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: New context and heap for JS && BMS task in JUPITER.
 * Create: 2020/11/17
 */

#if defined (JERRY_IAR_JUPITER) && (JERRY_EXTERNAL_CONTEXT == 1)

#include "jcontext.h"
#include "config-jupiter.h"

#pragma data_alignment = JMEM_ALIGNMENT
extern uint8_t* js_context_and_heap;

// heap for bms task
extern uint8_t* bms_context_and_heap;

/**
 * The alloc function passed to jerry_create_context for bms
 */
static void * bms_task_context_alloc (size_t total_size, void *cb_data_p)
{
  (void) cb_data_p; /* unused */

  if (total_size > (BMS_TASK_CONTEXT_AND_HEAP_SIZE * CONVERTION_RATIO)) {
    /* out of setted space */
    return NULL;
  }
  return bms_context_and_heap;
} /* bms_task_context_alloc */

/**
 * Create and set the default external context for bms task
 */
void bms_task_context_init (void)
{
  jerry_context_t * bms_task_context = jerry_create_context (BMS_TASK_HEAP_SIZE * CONVERTION_RATIO,
                                        bms_task_context_alloc, NULL);

  jerry_port_default_set_current_context (bms_task_context);
} /* bms_task_context_init */


/**
 * The alloc function passed to jerry_create_context for js
 */
static void * js_task_context_alloc (size_t total_size, void *cb_data_p)
{
  (void) cb_data_p; /* unused */

  if (total_size > (JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE)) {
    /* out of setted space */
    return NULL;
  }
  return js_context_and_heap;
} /* js_task_context_alloc */

/**
 * Create and set the default external context for js task., need free after task
 */
void js_task_context_init (void)
{
  jerry_context_t * js_task_context = jerry_create_context (JS_TASK_HEAP_SIZE * CONVERTION_RATIO,
                                        js_task_context_alloc, NULL);

  jerry_port_default_set_current_context (js_task_context);
} /* js_task_context_init */

/**
 * Unified interface for Creating external context
 */
void
jerry_external_context_init (uint32_t heap_size, /**< the size of heap */
                    jerry_context_alloc_t alloc, /**< the alloc function */
                    void *cb_data_p) /**< the cb_data for alloc function */
{
  jerry_context_t * external_context = jerry_create_context (heap_size, alloc, NULL);
  jerry_port_default_set_current_context (external_context);
}

#endif // defined (JERRY_IAR_JUPITER) && (JERRY_EXTERNAL_CONTEXT == 1)
