/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: add ref tracker.
 * Create: 2020/11/20
 */

#include "tracker.h"

#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ext-utils.h"
#include "heapdump.h"
#include "vm.h"

#if defined(JERRY_REF_TRACKER)

#define BUFFER_SIZE 65536
#define LogTracker(...) do { fprintf(gLogTrackerFile, __VA_ARGS__); } while (0)

bool gNativeManip = false;
// This variable shall be used only in "streaming" mode.
bool gRefTrackerEnabled = false;
static Storage gStorages[2];
static FILE* gLogTrackerFile = NULL;

bool GetRefTrackerEnabled(void)
{
  return gRefTrackerEnabled;
}

void SetRefTrackerEnabled(bool flag)
{
  gRefTrackerEnabled = flag;
}

void LogTrackerInit(const char* filepath)
{
  gLogTrackerFile = fopen(filepath, "w+");
}

void LogTrackerClose(void)
{
  fclose(gLogTrackerFile);
}

void RefInfoResetFn(IStorageItem* item)
{
  RefInfoItem* ref_item = (RefInfoItem*)item;
  JerryExtFree(ref_item->stacktrace);
  JerryExtFree(ref_item);
}

void InitTracker(void)
{
  StorageInit(&gStorages[kObjectRefInfo], BUFFER_SIZE);
  StorageInit(&gStorages[kStringRefInfo], BUFFER_SIZE);
}

void StorageInit(Storage* storage, size_t size)
{
  storage->size = size;
  storage->data = JerryExtAllocStorageData(size);
}

void StoragePut(Storage* storage, size_t key, size_t value)
{
  if (storage->data[key] == 0) {
    storage->data[key] = value;
    return;
  }

  IStorageItem* item = (IStorageItem*)storage->data[key];
  storage->data[key] = value;

  IStorageItem* new_item = (IStorageItem*)value;
  new_item->next = item;
}

size_t StorageGet(Storage* storage, size_t key)
{
  return storage->data[key];
}

void StorageReset(Storage* storage, size_t key, void(*reset_fn)(IStorageItem*))
{
  if (storage->data[key] == 0) {
    // Nothing to reset.
    return;
  }

  IStorageItem* item = (IStorageItem*)storage->data[key];
  storage->data[key] = 0;
  while (item != NULL) {
    IStorageItem* next = item->next;
    reset_fn(item);
    item = next;
  }
}

char* GetStacktraceString(ecma_string_t* stack)
{
  if (stack == NULL) {
    return NULL;
  }
  // Copy string to jsheap-independent storage.
  ECMA_STRING_TO_UTF8_STRING(stack, data, data_size);
  char* dst = JerryExtAllocStr(data_size + 1);
  memcpy(dst, data, data_size);
  dst[data_size] = '\0';
  return dst;
}

RefInfoItem* CreateRefInfoItem(RefInfoItemFlags flags, char* stack_str)
{
  RefInfoItem* item = JerryExtAlloc(sizeof(RefInfoItem));
  item->next = NULL;
  item->stacktrace = stack_str;
  item->flags = flags;
  return item;
}

void ReportObjRefManip(ecma_object_t* obj, RefInfoItemFlags flags)
{
  if (!gRefTrackerEnabled) {
    return;
  }

  if (gNativeManip) {
    flags &= kRefNative;
  }

  // XXX: For now don't do anything regarding within-gc ops.
  if (flags & (kRefMark | kRefUnmark)) {
    return;
  }

  const char* native_str = "    native";
  const char* init_str   = "      init";
  const char* inc_str    = " increment";
  const char* dec_str    = " decrement";

  uint32_t refcnt = obj->type_flags_refs >> REF_CNT_SHIFT;

  LogTracker("Object %p%s%s%s%s: refcount = %u\n", (void*)obj,
      (flags & kRefNative) ? native_str : "",
      (flags & kRefInit) ? init_str : "",
      (flags & kRefRef) ? inc_str : "",
      (flags & kRefDeref) ? dec_str : "",
      refcnt);
  LogTracker("\n");
}

void ReportObjDelete(ecma_object_t* obj)
{
  if (!gRefTrackerEnabled) {
    return;
  }

  LogTracker("Object %p deleted\n", (void*)obj);
  LogTracker("\n");
}

void DumpTracker(void)
{
  Storage* storage = &gStorages[kObjectRefInfo];
  for (int i = 0; i < (int)storage->size; ++i) {
    if (storage->data[i] != 0) {
      RefInfoItem* item = (RefInfoItem*)storage->data[i];
      printf("--== %p ==--:\n", (void*)ECMA_OBJECT_FROM_STORAGE_KEY(i));
      while (item != NULL) {
        printf("flags = %d\nStacktrace:\n%s\n", item->flags, item->stacktrace);
        item = item->next;
      }
    }
  }
}

#endif
