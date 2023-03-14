/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: improve heap dump.
 * Create: 2020/11/20
 */

#include "heapdump.h"

#include "ecma-builtins.h"
#include "ecma-conversion.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-property-hashmap.h"
#include "ecma-array-object.h"

#include <stdio.h>

bool gHeapdumpTracing = false;
FILE* gLogHeapdumpFile = NULL;
bool GetHeapdumpTracing(void)
{
  return gHeapdumpTracing;
}

void SetHeapdumpTraring(bool flag)
{
  gHeapdumpTracing = flag;
}

FILE* GetHeapdumpFile(void)
{
  return gLogHeapdumpFile;
}

void LogHeapdumpInit(const char* filepath)
{
  gLogHeapdumpFile = fopen(filepath, "w+");
}

void LogHeapdumpClose(void)
{
  fclose(gLogHeapdumpFile);
}

static void StartList(void)
{
  LogHeapdump("[\n");
}

static void EndList(void)
{
  LogHeapdump("]\n");
}

static void Start(void)
{
  LogHeapdump("{\n");
}

static void End(void)
{
  LogHeapdump("}\n");
}

static void Next(void)
{
  LogHeapdump(",\n");
}

static void LogStr(const char* str)
{
  LogHeapdump("\"%s\"\n", str);
}

static void LogAddr(void* addr)
{
  LogHeapdump("\"%p\"\n", addr);
}

static void LogUint(unsigned int val)
{
  LogHeapdump("%u\n", val);
}

static void LogStrObj(const ecma_string_t* obj)
{
  ECMA_STRING_TO_UTF8_STRING(obj, str, str_size);
  LogHeapdump("\"");
  for (int ii = 0; ii < (int)str_size; ++ii) {
    LogHeapdump("%c", str[ii]);
  }
  LogHeapdump("\"\n");
}

static void Key(const char* key)
{
  LogHeapdump("\"%s\"", key);
  LogHeapdump(":\n");
}

static void KeyUint(unsigned int key)
{
  LogHeapdump("\"%u\"", key);
  LogHeapdump(":\n");
}

static void Type(const char* type)
{
  Key("type");
  LogHeapdump("\"%s\"", type);
  Next();
}

static void Addr(void* addr)
{
  Key("addr");
  LogHeapdump("\"%p\"", addr);
  Next();
}

void DumpInfoLexEnv(const ecma_object_t* object)
{
  Key("outer");
  jmem_cpointer_t outer_lex_env_cp = object->u2.outer_reference_cp;
  if (outer_lex_env_cp != JMEM_CP_NULL) {
    LogAddr(ECMA_GET_NON_NULL_POINTER(ecma_object_t, outer_lex_env_cp));
  } else {
    LogAddr(NULL);
  }
  Next();

  Key("subtype");
  if (ecma_get_lex_env_type(object) != ECMA_LEXICAL_ENVIRONMENT_DECLARATIVE) {
    LogStr("binding");
    Next();

    Key("binding");
    ecma_object_t *binding_object_p = ecma_get_lex_env_binding_object(object);
    LogAddr(binding_object_p);
  } else {
    LogStr("declarative");
  }
}

void DumpInfoFunction(const ecma_object_t* object)
{
  Key("is_builtin");
  ecma_extended_object_t* ext_obj = (ecma_extended_object_t*)object;
  if (ecma_get_object_is_builtin (object)) {
    LogStr("true");
    Next();

    Key("is_routine");
    if (ecma_builtin_function_is_routine ((ecma_object_t*)object)) {
      LogStr("true");
    } else {
      LogStr("true");
    }
    Next();

    Key("id");
    LogUint(ext_obj->u.built_in.id);
    Next();
    Key("routine_id");
    LogUint(ext_obj->u.built_in.routine_id);
  } else {
    LogStr("false");
    Next();

    Key("scope");
    LogAddr(ECMA_GET_INTERNAL_VALUE_POINTER(ecma_object_t, ext_obj->u.function.scope_cp));
  }
}

void DumpPropertyPair(ecma_property_pair_t* pair)
{
  ecma_property_header_t* header = (ecma_property_header_t*)pair;
  for (int i = 0; i < ECMA_PROPERTY_PAIR_ITEM_COUNT; i++) {
    Start();

    switch (ECMA_PROPERTY_GET_TYPE(header->types[i])) {
      case ECMA_PROPERTY_TYPE_NAMEDDATA: {
        Type("data");

        Key("key");
        ecma_string_t* key_str = ecma_string_from_property_name(header->types[i],
                                                                pair->names_cp[i]);
        LogStrObj(key_str);
        ecma_deref_ecma_string(key_str);
        Next();

        Key("value");
        ecma_value_t value = pair->values[i].value;
        if (ecma_is_value_object(value)) {
          ecma_object_t* value_obj = ecma_get_object_from_value(value);
          LogAddr(value_obj);
        } else {
          ecma_string_t* value_str = ecma_op_to_string(value);
          LogStrObj(value_str);
        }
        break;
      }
      case ECMA_PROPERTY_TYPE_NAMEDACCESSOR: {
        Type("accessor");

        ecma_property_value_t* accessor_objs_p = pair->values + i;
        ecma_getter_setter_pointers_t* get_set_pair_p =
            ecma_get_named_accessor_property(accessor_objs_p);

        Key("getter");
        if (get_set_pair_p->getter_cp != JMEM_CP_NULL) {
          LogAddr(ECMA_GET_NON_NULL_POINTER(ecma_object_t, get_set_pair_p->getter_cp));
        } else {
          LogAddr(NULL);
        }
        Next();

        Key("setter");
        if (get_set_pair_p->setter_cp != JMEM_CP_NULL) {
          LogAddr(ECMA_GET_NON_NULL_POINTER(ecma_object_t, get_set_pair_p->setter_cp));
        } else {
          LogAddr(NULL);
        }
        break;
      }
      case ECMA_PROPERTY_TYPE_INTERNAL: {
        Type("internal");
        Key("TODO");
        LogStr("Not implemented yet");
        break;
      }
      default: {
        break;
      }
    }
    End();
    if (i + 1 < ECMA_PROPERTY_PAIR_ITEM_COUNT) {
      Next();
    }
  }
}

void DumpInfoObject(ecma_object_t* object, heapdump_object_flags_t flags)
{
  Start();
  Addr(object);
  if (flags & HEAPDUMP_OBJECT_ROOT) {
    Key("Root");
    LogStr("true");
    Next();
  }

  if (flags & HEAPDUMP_OBJECT_GLOBAL) {
    Key("Global");
    LogStr("true");
    Next();
  }

  Key("RefCount");
  LogUint(object->type_flags_refs >> REF_CNT_SHIFT);
  Next();

  if (ecma_is_lexical_environment(object)) {
    Type("LexEnv");
    DumpInfoLexEnv(object);
    goto finish;
  } else {
    Key("__proto__");
    jmem_cpointer_t proto_cp = object->u2.prototype_cp;
    if (proto_cp != JMEM_CP_NULL) {
      LogAddr(ECMA_GET_NON_NULL_POINTER (ecma_object_t, proto_cp));
    } else {
      LogAddr(NULL);
    }
    Next();
    switch (ecma_get_object_type(object)) {
      case ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION: {
        Type("ExternalFunction");
        break;
      }
      case ECMA_OBJECT_TYPE_BOUND_FUNCTION: {
        Type("BoundFunction");
        break;
      }
      case ECMA_OBJECT_TYPE_FUNCTION: {
        Type("Function");
        DumpInfoFunction(object);
        Next();
        break;
      }
      case ECMA_OBJECT_TYPE_PSEUDO_ARRAY: {
        Type("PseudoArray");
        ecma_extended_object_t* ext_object = (ecma_extended_object_t*)object;
        if (ext_object->u.pseudo_array.type == ECMA_PSEUDO_ARRAY_ARGUMENTS) {
          Key("subtype");
          LogStr("arguments");
          Next();
          ecma_object_t* lex_env = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_object_t,
              ext_object->u.pseudo_array.u2.lex_env_cp);
          Key("lex_env");
          LogAddr(lex_env);
        } else {
          Key("subtype");
          LogStr("!!! Unknown");
        }
        goto finish;
      }
      case ECMA_OBJECT_TYPE_ARRAY: {
        Type("Array");
        ecma_extended_object_t* ext_object = (ecma_extended_object_t*)object;

        if (ecma_op_object_is_fast_array(object)) {
          Key("subtype");
          LogStr("fast");
          Next();

          Key("data");
          Start();
          if (object->u1.property_list_cp != JMEM_CP_NULL) {
            ecma_value_t *values =
                ECMA_GET_NON_NULL_POINTER (ecma_value_t, object->u1.property_list_cp);
            bool skip_comma = true;
            for (uint32_t i = 0; i < ext_object->u.array.length; i++) {
              if (ecma_is_value_array_hole(values[i])) {
                continue;
              }
              if (skip_comma) {
                skip_comma = false;
              } else {
                Next();
              }
              KeyUint(i);
              if (ecma_is_value_object(values[i])) {
                ecma_object_t* value_obj = ecma_get_object_from_value(values[i]);
                LogAddr(value_obj);
              } else {
                ecma_string_t* value_str = ecma_op_to_string(values[i]);
                LogStrObj(value_str);
              }
            }
          }
          End();
          goto finish;
        } else {
          Key("subtype");
          LogStr("sparse");
          Next();
        }
        break;
      }
      default: {
        Type("Object");
        break;
      }
    }
  }

  jmem_cpointer_t prop_iter_cp = object->u1.property_list_cp;

#if ENABLED (JERRY_PROPRETY_HASHMAP)
  if (prop_iter_cp != JMEM_CP_NULL) {
    ecma_property_header_t* prop_iter_p =
        ECMA_GET_NON_NULL_POINTER(ecma_property_header_t, prop_iter_cp);
    if (prop_iter_p->types[0] == ECMA_PROPERTY_TYPE_HASHMAP) {
      prop_iter_cp = prop_iter_p->next_property_cp;
    }
  }
#endif

  Key("properties");
  StartList();
  while (prop_iter_cp != JMEM_CP_NULL) {
    ecma_property_header_t* prop_iter_p =
        ECMA_GET_NON_NULL_POINTER(ecma_property_header_t, prop_iter_cp);
    DumpPropertyPair((ecma_property_pair_t *) prop_iter_p);

    prop_iter_cp = prop_iter_p->next_property_cp;
    if (prop_iter_cp != JMEM_CP_NULL) {
      Next();
    }
  }
  EndList();

 finish:
  End();
  Next();
}
