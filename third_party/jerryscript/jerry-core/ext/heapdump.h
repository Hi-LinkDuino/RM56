/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: improve heap dump.
 * Create: 2020/11/20
 */

#ifndef HEAPDUMP_H
#define HEAPDUMP_H

#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "jerryscript-core.h"

#define REF_CNT_SHIFT 6

FILE* GetHeapdumpFile(void);
bool GetHeapdumpTracing(void);
void SetHeapdumpTraring(bool flag);

#define LogHeapdump(...) do { fprintf(GetHeapdumpFile(), __VA_ARGS__); } while (0)

typedef enum {
  HEAPDUMP_OBJECT_SIMPLE,
  HEAPDUMP_OBJECT_GLOBAL,
  HEAPDUMP_OBJECT_ROOT
} heapdump_object_flags_t;

void LogHeapdumpInit(const char* filepath);
void LogHeapdumpClose(void);

void DumpInfoObject(ecma_object_t* object, heapdump_object_flags_t flags);
void DumpInfoLexEnv(const ecma_object_t* object);
void DumpInfoFunction(const ecma_object_t* object);
void DumpPropertyPair(ecma_property_pair_t* pair);

#endif
