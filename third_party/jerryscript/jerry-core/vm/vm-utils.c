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

#include "ecma-array-object.h"
#include "ecma-helpers.h"
#include "ecma-objects.h"
#include "jcontext.h"
#include "lit-char-helpers.h"
#include "vm.h"

/**
 * Check whether currently executed code is strict mode code
 *
 * @return true - current code is executed in strict mode,
 *         false - otherwise
 */
bool
vm_is_strict_mode (void)
{
  JERRY_ASSERT (JERRY_CONTEXT (vm_top_context_p) != NULL);

  return JERRY_CONTEXT (vm_top_context_p)->bytecode_header_p->status_flags & CBC_CODE_FLAGS_STRICT_MODE;
} /* vm_is_strict_mode */

/**
 * Check whether currently performed call (on top of call-stack) is performed in form,
 * meeting conditions of 'Direct Call to Eval' (see also: ECMA-262 v5, 15.1.2.1.1)
 *
 * Warning:
 *         the function should only be called from implementation
 *         of built-in 'eval' routine of Global object
 *
 * @return true - currently performed call is performed through 'eval' identifier,
 *                without 'this' argument,
 *         false - otherwise
 */
inline bool JERRY_ATTR_ALWAYS_INLINE
vm_is_direct_eval_form_call (void)
{
  return (JERRY_CONTEXT (status_flags) & ECMA_STATUS_DIRECT_EVAL) != 0;
} /* vm_is_direct_eval_form_call */

#if defined(JERRY_FUNCTION_BACKTRACE) && !defined(__APPLE__)
static ecma_string_t*
vm_get_function_name_string (vm_frame_ctx_t *context_p)
{
  ecma_stringbuilder_t func_name_builder;
  if (context_p->prev_context_p != NULL) {
    vm_frame_ctx_t* prev_ctx_p = context_p->prev_context_p;
    ecma_object_t* func_obj = ecma_get_object_from_value (prev_ctx_p->callee_value);
    ecma_object_type_t func_obj_type = ecma_get_object_type (func_obj);
    if (func_obj_type == ECMA_OBJECT_TYPE_FUNCTION
	|| func_obj_type == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION
	|| func_obj_type == ECMA_OBJECT_TYPE_BOUND_FUNCTION) {
      ecma_string_t* name_prop = ecma_get_magic_string (LIT_MAGIC_STRING_NAME);
      ecma_value_t func_name_value = ecma_op_object_get (func_obj, name_prop);
      if (func_name_value == ECMA_VALUE_UNDEFINED) {
        func_name_builder = ecma_stringbuilder_create_raw ((lit_utf8_byte_t *) ("<unknown>"), 9);
      } else {
        func_name_builder = ecma_stringbuilder_create_from(ecma_get_string_from_value (func_name_value));
      }
      ecma_deref_ecma_string (name_prop);
    } else {
      func_name_builder = ecma_stringbuilder_create_raw ((lit_utf8_byte_t *) ("<erroneous>"), 11);
    }
  } else {
    func_name_builder = ecma_stringbuilder_create_raw ((lit_utf8_byte_t *) ("<GLOBAL>"), 8);
  }
  ecma_string_t* lbracket = ecma_new_ecma_string_from_utf8((const lit_utf8_byte_t *) ("("), 1);
  ecma_string_t* rbracket = ecma_new_ecma_string_from_utf8((const lit_utf8_byte_t *) (")"), 1);
  ecma_stringbuilder_append (&func_name_builder, lbracket);
  ecma_stringbuilder_append (&func_name_builder, rbracket);
  ecma_stringbuilder_append_byte (&func_name_builder, LIT_CHAR_COMMA);
  ecma_stringbuilder_append_byte (&func_name_builder, LIT_CHAR_SP);
  ecma_deref_ecma_string(rbracket);
  ecma_deref_ecma_string(lbracket);
  return ecma_stringbuilder_finalize (&func_name_builder);
}
#endif

/**
 * Get backtrace. The backtrace is an array of strings where
 * each string contains the position of the corresponding frame.
 * The array length is zero if the backtrace is not available.
 *
 * @return array ecma value
 */
ecma_value_t
vm_get_backtrace (uint32_t max_depth) /**< maximum backtrace depth, 0 = unlimited */
{
#if ENABLED (JERRY_LINE_INFO)
  ecma_value_t result_array = ecma_op_create_array_object (NULL, 0, false);

  if (max_depth == 0)
  {
    max_depth = UINT32_MAX;
  }

  vm_frame_ctx_t *context_p = JERRY_CONTEXT (vm_top_context_p);
  ecma_object_t *array_p = ecma_get_object_from_value (result_array);
  JERRY_ASSERT (ecma_op_object_is_fast_array (array_p));
  uint32_t index = 0;

  while (context_p != NULL)
  {
    ecma_string_t *str_p = ecma_get_magic_string (LIT_MAGIC_STRING__EMPTY);

#if defined(JERRY_FUNCTION_BACKTRACE) && !defined(__APPLE__)
    ecma_string_t* func_name = vm_get_function_name_string (context_p);
    func_name = ecma_concat_ecma_strings (func_name, str_p);

    ecma_fast_array_set_property (array_p, index, ecma_make_string_value (func_name));
    ecma_deref_ecma_string(str_p);
    ecma_deref_ecma_string(func_name);
#else
    ecma_fast_array_set_property (array_p, index, ecma_make_string_value (str_p));
    ecma_deref_ecma_string (str_p);
#endif

    context_p = context_p->prev_context_p;
    index++;

    if (index >= max_depth)
    {
      break;
    }
  }

  return result_array;
#else /* !ENABLED (JERRY_LINE_INFO) */
  JERRY_UNUSED (max_depth);

  return ecma_op_create_array_object (NULL, 0, false);
#endif /* ENABLED (JERRY_LINE_INFO) */
} /* vm_get_backtrace */
