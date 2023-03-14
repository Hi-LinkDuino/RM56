/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Definition of header file for generating bytecode.
 * Create: 2020/09/07
 */

#ifndef GENERATE_BYTECODE_H
#define GENERATE_BYTECODE_H

#ifdef JERRY_FOR_IAR_CONFIG

#include "jerryscript.h"

typedef enum {
  EXCE_ACE_JERRY_EXEC_OK = 0, // program function OK
  EXCE_ACE_JERRY_NULL_PATH, // null path for generateing snapshot
  EXCE_ACE_JERRY_MALLOC_ERROR, // error when malloc
  EXCE_ACE_JERRY_INPUT_PATH_ERROR, // passed input path is NULL OR open failed
  EXCE_ACE_JERRY_INPUT_PATH_NOT_DIR, // passed input path is not a directory
  EXCE_ACE_JERRY_OPEN_DIR_FAILED, // open directory failed
  EXCE_ACE_JERRY_GENERATE_SNAPSHOT_FAILED, // jerry_generate_snapshot failed
  EXCE_ACE_JERRY_OPEN_FILE_FAILED, // open file failed
  EXCE_ACE_JERRY_WRITE_SNAPSHOT_FILE_FAILED, // write into snapshot file failed
  EXCE_ACE_JERRY_READ_JSFILE_FAILED, // read .js file process failed
  EXCE_ACE_JERRY_JSFILE_TOO_LARGE, // bytes of js file out of psRAM
  EXCE_ACE_JERRY_SPLICE_PATH_ERROR, // error when splice path
  EXCE_ACE_JERRY_SPLICE_OUTPUT_PATH_ERROR, // error when splice output file abs name
  EXCE_ACE_JERRY_SPRINTFS_VERSION_ERROR, // error when sprintf_s for jerry_version
  EXCE_ACE_JERRY_GET_FILE_STAT_ERROR, // error when getting file stat
  EXCE_ACE_JERRY_LINKLIST_ERROR, // error when malloc for list node
  EXCE_ACE_JERRY_UNLINKFILE_ERROR, // error when unlink bytecode file
} EXECRES;

typedef struct
{
  jerry_context_t *context_p;
} ContextRecord;

char* get_jerry_version_no();
EXECRES walk_directory(char* filefolder);
EXECRES walk_del_bytecode(char* filefolder);

void bms_task_context_init (void);
void js_task_context_init (void);
void jerry_port_default_remove_current_context_record ();

void jerry_external_context_init(uint32_t heap_size, jerry_context_alloc_t alloc, void *cb_data_p);

#endif  // JERRY_FOR_IAR_CONFIG

#endif  // GENERATE_BYTECODE_H
