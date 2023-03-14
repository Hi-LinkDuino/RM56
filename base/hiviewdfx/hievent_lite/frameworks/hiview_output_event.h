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

#ifndef HOS_LITE_HIVIEW_OUTPUT_EVENT_H
#define HOS_LITE_HIVIEW_OUTPUT_EVENT_H

#include "ohos_types.h"
#include "event.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

void InitCoreEventOutput(void);
void InitEventOutput(void);
void ClearEventOutput(void);

/**
 * Output the event to file.
 * @param data HiEvent data.
 **/
void OutputEvent(const uint8 *data);

/**
 * Get the event file size.
 * @param eventType Event type: HIEVENT_FAULT/HIEVENT_UE/HIEVENT_STAT
 * @retval File size.
 **/
uint32 GetEventFileSize(uint8 eventType);

/**
 * Read the event data form the event file.
 * @param eventType Event type: HIEVENT_FAULT/HIEVENT_UE/HIEVENT_STAT
 * @param buf Read buffer.
 * @param len Read buffer size.
 * @retval The length of read data.
 **/
uint32 ReadEventFile(uint8 eventType, uint8 *buf, uint32 len);

/**
 * Formatting event content.
 * @param outStr Output string buffer.
 * @param outStrLen Maximum length of the string buffer.
 * @param pEvent Pointer to the HiEvent object.
 * @retval The actual length of the formatted string.
 **/
int32 EventContentFmt(char *outStr, int32 outStrLen, const uint8 *pEvent);

/*
 * Interface for flush event before the system restarts.
 * @param syncFlag indicates synchronised flush or asynchronous flush.
 * @attention Use this interface to flush event to the UART or the files.
 */
void FlushEvent(boolean syncFlag);

/**
 * Interface for register the Hievent handle.
 * @param func Function Pointer.
 **/
void HiviewRegisterHieventProc(HieventProc func);

/**
 * Interface for deregister the Hievent handle.
 **/
void HiviewUnRegisterHieventProc(HieventProc func);

/**
 * Process files according to mode.
 * @param type hievent type.
 * @param dest hievent output target file path.
 * @param mode file processing mode. 0 for copy specified type hievent file to dest and keep the
 *             content in the source file, 1 for rename specified type hievent file to dest.
 * @return 0 if success, otherwise -1.
 **/
int HiEventFileProcImp(uint8 type, const char *dest, uint8 mode);

/**
 * Add a monitoring function when the specified type of file is full .
 * @param type hievent type.
 * @param func callback function.
 * @param dest hievent output target file path.
 **/
void HiviewRegisterHieventFileWatcher(uint8 type, FileProc func, const char *dest);

/**
 * Remove monitoring of specified types of files.
 * @param type hievent type.
 * @param func callback function.
 **/
void HiviewUnRegisterHieventFileWatcher(uint8 type, FileProc func);

/**
 * Lock the hievent output target file.
 *
 **/
void HiEventOutputFileLockImp(void);

/**
 * Unlock the hievent output target file.
 *
 **/
void HiEventOutputFileUnLockImp(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_OUTPUT_EVENT_H */
