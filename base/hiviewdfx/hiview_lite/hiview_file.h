/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HOS_LITE_HIVIEW_FILE_H
#define HOS_LITE_HIVIEW_FILE_H

#include "hiview_util.h"
#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HIVIEW_FILE_HEADER_PREFIX_LOG      0x48565701      /* HVW ASCii + 0x01 */
#define HIVIEW_FILE_HEADER_PREFIX_EVENT    0x48565702      /* HVW ASCii + 0x02 */
#define HIVIEW_FILE_HEADER_PREFIX_TEXT     0x48565703      /* HVW ASCii + 0x03 */
#define HIVIEW_FILE_HEADER_PREFIX_MASK     0x48565700      /* HVW ASCii + 0x00 */
#define HIVIEW_FILE_HEADER_MAIN_VERSION    1               /* Main version:1 */
#define HIVIEW_FILE_HEADER_SUB_VERSION     10              /* Sub version:10 (lite) */
#define HIVIEW_UE_EVENT_VER                991231100       /* UE event version */
#define HIVIEW_FAULT_EVENT_VER             991231000       /* Fault event version */
#define HIVIEW_STATIC_EVENT_VER            991231000       /* Static event version */
#define HIVIEW_CONF_PRODUCT_VER_STR        "1.0.0"

#ifndef FILE_PROC_DEFINED
#define FILE_PROC_DEFINED
/**
 * Callback function prototype for file processing .
 *
 * @param path the path of the file to be processed.
 * @param type the type of the file to be processed.
 * @param event the type of event that triggered the function. 0 for file full.
 **/
typedef void (*FileProc)(const char *path, uint8 type, uint8 event);
#endif

typedef enum {
    HIVIEW_LOG_TEXT_FILE = 0,
    HIVIEW_LOG_BIN_FILE,
    HIVIEW_DUMP_FILE,
    HIVIEW_FAULT_EVENT_FILE,
    HIVIEW_UE_EVENT_FILE,
    HIVIEW_STAT_EVENT_FILE,
} HiviewFileType;

typedef enum {
    HIVIEW_FILE_COPY = 0,
    HIVIEW_FILE_RENAME
} FileProcMode;

typedef enum {
    HIVIEW_FILE_FULL = 0
} HiviewFileEvent;

#pragma pack(1)
typedef struct {
    uint32 prefix;
    uint8 type;     /* HiviewFileType */
    uint8 codeMainVersion;
    uint8 codeSubVersion;
    uint32 defineFileVersion;
} FileHeaderCommon;

typedef struct {
    FileHeaderCommon common;
    uint32 createTime;
    uint32 wCursor;
    uint32 rCursor;
    uint32 size;    /* Max size. Include the file header. */
} HiviewFileHeader;

typedef struct {
    HiviewFileHeader header;
    const char *path;
    char *outPath;
    FileProc pFunc;
    HiviewMutexId_t mutex;  /* file lock of outPath */
    int32 fhandle;  /* Circular file */
} HiviewFile;
#pragma pack()

/**
 * Initializes the file object.
 *
 * @param fp The address of hivew file object.
 * @param type file type.
 * @param size file size.
 * @return TRUE if success, otherwise FALSE.
 **/
boolean InitHiviewFile(HiviewFile *fp, HiviewFileType type, uint32 size);

/**
 * Writes the file header to file.
 *
 * @param fp the pointer of hiview file object.
 * @return TRUE if success, otherwise FALSE.
 **/
boolean WriteFileHeader(HiviewFile *fp);

/**
 * Reads the file header info.
 *
 * @param fp the pointer of hiview file object.
 * @return TRUE if the file contains the correct header info, otherwise FALSE.
 **/
boolean ReadFileHeader(HiviewFile *fp);

/**
 * Writes data to file.
 *
 * @param fp the pointer of hiview file object.
 * @param data bytes to be written to the file.
 * @param len the length of the data to be written. The length should be a multiple of a structure.
 * @return length of bytes written.
 * @attention Avoid calling this function too frequently, the watchdog may timeout otherwise.
 **/
int32 WriteToFile(HiviewFile *fp, const uint8 *data, uint32 len);

/**
 * Reads data from file.
 *
 * @param fp the pointer of hiview file object.
 * @param data buffer for holding reading data.
 * @param readLen the length of the data to be read. The length should be a multiple of a structure.
 * @return length of bytes read.
 * @attention Avoid calling this function too frequently, the watchdog may timeout otherwise.
 **/
int32 ReadFromFile(HiviewFile *fp, uint8 *data, uint32 readLen);

/**
 * Gets used size of file, excluding the size of file header
 *
 * @param fp the pointer of hiview file object.
 * @return used size.
 **/
uint32 GetFileUsedSize(HiviewFile *fp);

/**
 * Gets free size of file, excluding the size of file header
 *
 * @param fp the pointer of hiview file object.
 * @return free size.
 **/
uint32 GetFileFreeSize(HiviewFile *fp);

/**
 * Closes the file.
 *
 * @param fp the pointer of hiview file object.
 * @return 0 if success, otherwise -1.
 **/
int32 CloseHiviewFile(HiviewFile *fp);

/**
 * Process files according to mode.
 *
 * @param fp the pointer of hiview file object.
 * @param dest target file path.
 * @param mode file processing mode.
 * @return 0 if success, otherwise -1.
 **/
int8 ProcFile(HiviewFile *fp, const char *dest, FileProcMode mode);

/**
 * Register a monitoring function when file is full .
 *
 * @param fp the pointer of file object.
 * @param func callback function.
 * @param dest target file path.
 **/
void RegisterFileWatcher(HiviewFile *fp, FileProc func, const char *path);

/**
 * Unregister a monitoring function.
 *
 * @param fp the pointer of file object.
 * @param func callback function.
 * 
 **/
void UnRegisterFileWatcher(HiviewFile *fp, FileProc func);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_FILE_H */
