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

#include <stdlib.h>

#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

#ifdef JERRY_FOR_IAR_CONFIG
#include "jcontext.h"

#if !ENABLED (JERRY_EXTERNAL_CONTEXT)
static fatal_handler_t jerry_fatal_handler = NULL;
#endif

void jerry_port_default_set_fatal_handler (fatal_handler_t handler)
{
#if ENABLED (JERRY_EXTERNAL_CONTEXT)
  JERRY_CONTEXT (jerry_fatal_handler) = handler;
#else
  jerry_fatal_handler = handler;
#endif
}

/**
 * Default implementation of jerry_port_fatal. Calls 'abort' if exit code is
 * non-zero, 'exit' otherwise.
 */
void jerry_port_fatal (jerry_fatal_code_t code) /**< cause of error */
{
#if ENABLED (JERRY_EXTERNAL_CONTEXT)
  fatal_handler_t jerry_fatal_handler = JERRY_CONTEXT (jerry_fatal_handler);
#endif
  if (jerry_fatal_handler != NULL)
  {
    jerry_fatal_handler ((int) code);
  }
  if (code != 0
      && code != ERR_OUT_OF_MEMORY)
  {
    abort ();
  }

  exit ((int) code);
} /* jerry_port_fatal */

#else // not defined JERRY_FOR_IAR_CONFIG

static fatal_handler_t jerry_fatal_handler = NULL;

void jerry_port_default_set_fatal_handler (fatal_handler_t handler)
{
  jerry_fatal_handler = handler;
}

/**
 * Default implementation of jerry_port_fatal. Calls 'abort' if exit code is
 * non-zero, 'exit' otherwise.
 */
void jerry_port_fatal (jerry_fatal_code_t code) /**< cause of error */
{
  if (jerry_fatal_handler != NULL)
  {
    jerry_fatal_handler ((int) code);
  }
  if (code != 0
      && code != ERR_OUT_OF_MEMORY)
  {
    abort ();
  }

  exit ((int) code);
} /* jerry_port_fatal */

#endif
