/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: add ref tracker.
 * Create: 2020/11/20
 */

#ifndef EXT_TRACKER_H
#define EXT_TRACKER_H

#if defined(JERRY_REF_TRACKER)

#include "jcontext.h"

typedef struct {
  size_t size;
  size_t* data;
} Storage;

typedef enum {
  kObjectRefInfo = 0,
  kStringRefInfo = 1
} StorageType;

typedef enum {
  kRefNative = 1 << 0,
  kRefInit = 1 << 1,
  kRefRef = 1 << 2,
  kRefDeref = 1 << 3,
  kRefMark = 1 << 4,
  kRefUnmark = 1 << 5
} RefInfoItemFlags;

typedef struct IStorageItem_s {
  struct IStorageItem_s* next;
} IStorageItem;

typedef struct RefInfoItem_s {
  struct RefInfoItem_s* next;
  char* stacktrace;
  RefInfoItemFlags flags;
} RefInfoItem; // Implements IStorageItem.

// Deleter.
void RefInfoResetFn(IStorageItem* item);

#define ECMA_OBJECT_TO_STORAGE_KEY(x) (size_t)((uintptr_t)x - (uintptr_t)&JERRY_HEAP_CONTEXT(first))
#define ECMA_OBJECT_FROM_STORAGE_KEY(x) (ecma_object_t*)((uintptr_t)x + (uintptr_t)&JERRY_HEAP_CONTEXT(first))

bool GetRefTrackerEnabled(void);
void SetRefTrackerEnabled(bool flag);

void LogTrackerInit(const char* filepath);
void LogTrackerClose(void);

void InitTracker(void);
void StorageInit(Storage* storage, size_t size);
void StoragePut(Storage* storage, size_t key, size_t value);
size_t StorageGet(Storage* storage, size_t key);
void StorageReset(Storage* storage, size_t key, void(*reset_fn)(IStorageItem*));
char* GetStacktraceString(ecma_string_t* stack);
RefInfoItem* CreateRefInfoItem(RefInfoItemFlags flags, char* stack_str);
void ReportObjRefManip(ecma_object_t* obj, RefInfoItemFlags flags);
void ReportObjDelete(ecma_object_t* obj);
void DumpTracker(void);

#endif
#endif
