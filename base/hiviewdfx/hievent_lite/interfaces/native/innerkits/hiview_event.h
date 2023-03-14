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

#ifndef HOS_LITE_HIVIEW_EVENT_H
#define HOS_LITE_HIVIEW_EVENT_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef HIEVENT_NONE
#define HIEVENT_NONE   0   /* none event            */
#endif
#ifndef HIEVENT_FAULT
#define HIEVENT_FAULT  1   /* fault event           */
#endif
#ifndef HIEVENT_UE
#define HIEVENT_UE     2   /* user behavior event   */
#endif
#ifndef HIEVENT_STAT
#define HIEVENT_STAT   4   /* statistics event      */
#endif

#pragma pack(1)
typedef struct {
    uint8  mark;
    uint8  len;          /* payload length */
    uint16 eventId;      /* 0-65535        */
    uint32 time;
} HiEventCommon;

typedef struct {
    HiEventCommon common;
    uint8 type;          /* Do not write to file. */
    uint8 *payload;      /* T-L-V */
} HiEvent;
#pragma pack()

/**
 * Definitions for function Pointer.
 * @param data HieventContent pointer.
 * @return function handle result. If TRUE is returned, the platform does not process,
 *         else the platform continues to process.
 **/
typedef boolean (*HieventProc)(const HiEvent *event);

#ifndef FILE_PROC_DEFINED
#define FILE_PROC_DEFINED
/**
 * Callback function prototype for file processing.
 *
 * @param path the path of the file to be processed.
 * @param type the type of the file to be processed.
 * @param event the type of event that triggered the function. 0 for file full.
 **/
typedef void (*FileProc)(const char *path, uint8 type, uint8 event);
#endif

/**
 * Create and output a event.
 * Use the macro definition interface instead of directly using this interface.
 * @param type    Event type.
 * @param eventId Event ID. Event ID is assigned uniformly and globally unique.
 * @param key     The id of the parameter in the XML definition. Valid data range: 0-15. -1 means no parameters.
 * @param value   The Value of the parameter.
 * @attention This event carry only one parameter.
 **/
void HiEventPrintf(uint8 type, uint16 eventId, int8 key, int32 value);

/**
 * Create the HivewEvent object.
 * Use the macro definition interface instead of directly using this interface.
 * @param type    Event type.
 * @param eventId Event ID. Event ID is assigned uniformly and globally unique.
 * @param num     The number of values carried by the event. Valid data range: 2-16.
 * @return HiEvent object.
 * @attention This function is the atomic operation. This method cannot be used when no parameter
              is carried or only one parameter is carried.
 **/
HiEvent *HiEventCreate(uint8 type, uint16 eventId, uint8 num);

/**
 * Add parameter's value to event object.
 * Use the macro definition interface instead of directly using this interface.
 * @param event Operation object.
 * @param key   The id of the parameter in the XML definition. Valid data range: 0-15.
 * @param value The Value of the parameter.
 **/
void HiEventPutInteger(HiEvent *event, int8 key, int32 value);

/**
 * Ouput the event object and release memory.
 * Use the macro definition interface instead of directly using this interface.
 * @param event Operation object.
 * @attention This function will automatically release the event object.
 **/
void HiEventReport(HiEvent *event);

/*
 * Interface for flush event before the system restarts.
 * @param syncFlag indicates synchronised flush or asynchronous flush.
 * @attention Use this interface to flush event to the UART or the files.
 */
void HiEventFlush(boolean syncFlag);

/**
 * Interface for register the Hievent handle.
 * @param func Function Pointer.
 **/
void HiEventRegisterProc(HieventProc func);

/**
 * Interface for deregister the Hievent handle.
 **/
void HiEventUnRegisterProc(HieventProc func);

/**
 * Add a monitoring function when the specified type of file is full .
 *
 * @param type hievent type.
 * @param func callback function.
 * @param dest hievent output target file path.
 **/
void HiEventFileAddWatcher(uint8 type, FileProc func, const char *dest);

/**
 * Remove monitoring of specified types of files.
 *
 * @param type hievent type.
 * @param func callback function.
 **/
void HiEventFileRemoveWatcher(uint8 type, FileProc func);

/**
 * Process files according to mode.
 *
 * @param type hievent type.
 * @param dest hievent output target file path.
 * @param mode file processing mode. 0 for copy specified type hievent file to dest and keep the
 *             content in the source file, 1 for rename specified type hievent file to dest.
 * @return 0 if success, otherwise -1.
 **/
int HiEventFileProc(uint8 type, const char *dest, uint8 mode);

/**
 * Lock the hievent output target file.
 *
 **/
void HiEventOutputFileLock(void);

/**
 * Unlock the hievent output target file.
 *
 **/
void HiEventOutputFileUnLock(void);

#ifndef HIEVENT_COMPILE_TYPE
#define HIEVENT_COMPILE_TYPE (HIEVENT_FAULT | HIEVENT_UE | HIEVENT_STAT)
#endif

#define IS_COMPILE_EVENT(t) (((HIEVENT_COMPILE_TYPE) & (t)) == (t))

/**
 * If no parameter is carried or only one parameter is carried, use the following method.
 * @param id  Event ID. Event ID is assigned uniformly and globally unique. The maximum value is 65535.
 * @param k   Index of the parameter carried in the event. Valid data range: 0-15. If the value is -1,
 *            the event does not carry any parameter.
 * @param v   Value of the parameter carried in the event.
**/
#if IS_COMPILE_EVENT(HIEVENT_FAULT)
#define HIEVENT_FAULT_REPORT(id, k, v) HiEventPrintf(HIEVENT_FAULT, (id), (k), (v))
#else
#define HIEVENT_FAULT_REPORT(id, k, v)
#endif

#if IS_COMPILE_EVENT(HIEVENT_UE)
#define HIEVENT_UE_REPORT(id, k, v) HiEventPrintf(HIEVENT_UE, (id), (k), (v))
#else
#define HIEVENT_UE_REPORT(id, k, v)
#endif

#if IS_COMPILE_EVENT(HIEVENT_STAT)
#define HIEVENT_STAT_REPORT(id, k, v) HiEventPrintf(HIEVENT_STAT, (id), (k), (v))
#else
#define HIEVENT_STAT_REPORT(id, k, v)
#endif

/**
* If multiple parameters need to be carried, use the following method.
* @param type  Event type.
* @param id    Event id. Event ID is assigned uniformly and globally unique.
* @param num   Number of parameters carried in an event. Valid data range: 2-16.
* @attention   The following method cannot be used when no parameter is carried or only one parameter is carried.
**/
#if HIEVENT_COMPILE_TYPE > HIEVENT_NONE
#define HIEVENT_CREATE(type, id, num) HiEventCreate(type, id, num)
#define HIEVENT_PUT_INT_VALUE(pEvent, k, v) HiEventPutInteger(pEvent, k, v)
#define HIEVENT_REPORT(pEvent) HiEventReport(pEvent)
#else
#define HIEVENT_CREATE(type, id, num)
#define HIEVENT_PUT_INT_VALUE(pEvent, k, v)
#define HIEVENT_REPORT(pEvent)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_EVENT_H */
