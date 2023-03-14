/* Copyright JS Foundation and other contributors, http://js.foundation
 *
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

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

#if (JERRY_EXTERNAL_CONTEXT == 1)

extern jerry_context_t *jerry_dynamic_global_context_p;

#if defined (JERRY_FOR_IAR_CONFIG)

#include "generate-bytecode.h"
#include "los_task.h"

/**
 * use dynamic size array to record the correspondence between task id and jerry-heap/context
 */
extern ContextRecord* g_contextRecords;

void jerry_switch_context();

/**
 * set context function: store task id and context
 */
void
jerry_port_default_set_current_context (jerry_context_t *context_p) /**< store created context */
{
  uint32_t curTaskId = LOS_CurTaskIDGet();
  g_contextRecords[curTaskId].context_p = context_p;
  jerry_dynamic_global_context_p = context_p;
}

void jerry_switch_context()
{
  jerry_dynamic_global_context_p = g_contextRecords[LOS_NextTaskIDGet()].context_p;
}

/**
 * when task ends, the context_pointer point to NULL
 */
void
jerry_port_default_remove_current_context_record () /**< remove current task's context record in Array */
{
  uint32_t curTaskId = LOS_CurTaskIDGet();
  g_contextRecords[curTaskId].context_p = NULL;
  jerry_dynamic_global_context_p = NULL;
}

/**
 * key: global dynamic context_p for current context
 */
jerry_context_t *
jerry_port_get_current_context (void) /**< points to current task's context */
{
  return jerry_dynamic_global_context_p;
}

#else // not defined JERRY_FOR_IAR_CONFIG, but enabled JERRY_EXTERNAL_CONTEXT

/**
 * Set the current_context_p as the passed pointer.
 */
void
jerry_port_default_set_current_context (jerry_context_t *context_p) /**< points to the created context */
{
  jerry_dynamic_global_context_p = context_p;
} /* jerry_port_default_set_current_context */

/**
 * Get the current context.
 *
 * @return the pointer to the current context
 */
jerry_context_t *
jerry_port_get_current_context (void)
{
  return jerry_dynamic_global_context_p;
} /* jerry_port_get_current_context */

#endif // defined (JERRY_FOR_IAR_CONFIG)

#endif // (JERRY_EXTERNAL_CONTEXT == 1)
