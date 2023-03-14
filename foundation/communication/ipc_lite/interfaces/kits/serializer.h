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

#ifndef _LITEIPC_SERIALIZER_H_
#define _LITEIPC_SERIALIZER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ndk_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    char* bufferBase;
    size_t* offsetsBase;
    char* bufferCur;
    size_t* offsetsCur;
    size_t bufferLeft;
    size_t offsetsLeft;
    uint32_t flag;
    NDK_DL_LIST ptrFreeList;
} IpcIo;

typedef enum { OBJ_FD, OBJ_PTR, OBJ_SVC } ObjType;

typedef struct {
    uint32_t buffSz;
    void* buff;
} BuffPtr;

typedef struct {
    int fd;
    size_t mmapSize;
    void* mmapAddr;
} IpcContext;

typedef struct {
    uint32_t handle;
    uint32_t token;
    uint32_t cookie;
#ifdef __LINUX__
    IpcContext* ipcContext;
#endif
} SvcIdentity;

typedef union {
    uint32_t fd;
    BuffPtr ptr;
    SvcIdentity svc;
} ObjContent;

typedef struct {
    ObjType type;
    ObjContent content;
} SpecialObj;

typedef void (*IpcIoPtrFree)(void* ptr);

#define MIN_BINDER_HANDLE 0xffffffff

/* allocate a IpcIo, providing a stack-allocated working
 * buffer, size of the working buffer, and how many object
 * offset entries to reserve from the buffer
 */
void IpcIoInit(IpcIo* io, void* data, size_t maxdata, size_t maxobjects);
void IpcIoInitFromMsg(IpcIo* io, const void* msg);
bool IpcIoAvailable(IpcIo* io);
/* Must ensure all the input is valid */
void IpcIoPushChar(IpcIo* io, char c);
void IpcIoPushCharUnaligned(IpcIo* io, char c);
void IpcIoPushBool(IpcIo* io, bool b);
void IpcIoPushBoolUnaligned(IpcIo* io, bool b);
void IpcIoPushIntptr(IpcIo* io, intptr_t n);
void IpcIoPushUintptr(IpcIo* io, uintptr_t n);
void IpcIoPushInt8(IpcIo* io, int8_t n);
void IpcIoPushInt8Unaligned(IpcIo* io, int8_t n);
void IpcIoPushUint8(IpcIo* io, uint8_t n);
void IpcIoPushUint8Unaligned(IpcIo* io, uint8_t n);
void IpcIoPushInt16(IpcIo* io, int16_t n);
void IpcIoPushInt16Unaligned(IpcIo* io, int16_t n);
void IpcIoPushUint16(IpcIo* io, uint16_t n);
void IpcIoPushUint16Unaligned(IpcIo* io, uint16_t n);
void IpcIoPushInt32(IpcIo* io, int32_t n);
void IpcIoPushUint32(IpcIo* io, uint32_t n);
void IpcIoPushInt64(IpcIo* io, int64_t n);
void IpcIoPushUint64(IpcIo* io, uint64_t n);
void IpcIoPushFloat(IpcIo* io, float n);
void IpcIoPushDouble(IpcIo* io, double n);
void IpcIoPushString(IpcIo* io, const char* cstr);
void IpcIoPushFlatObj(IpcIo* io, const void* obj, uint32_t size);
void IpcIoPushSvc(IpcIo* io, const SvcIdentity* svc);
void IpcIoPushFd(IpcIo* io, uint32_t fd);
void IpcIoPushDataBuff(IpcIo* io, const BuffPtr* dataBuff);
void IpcIoPushDataBuffWithFree(IpcIo* io, const BuffPtr* dataBuff, IpcIoPtrFree ipcIoFree);
void IpcIoFreeDataBuff(IpcIo* io);

char IpcIoPopChar(IpcIo* io);
char IpcIoPopCharUnaligned(IpcIo* io);
bool IpcIoPopBool(IpcIo* io);
bool IpcIoPopBoolUnaligned(IpcIo* io);
intptr_t IpcIoPopIntptr(IpcIo* io);
uintptr_t IpcIoPopUintptr(IpcIo* io);
int8_t IpcIoPopInt8(IpcIo* io);
int8_t IpcIoPopInt8Unaligned(IpcIo* io);
uint8_t IpcIoPopUint8(IpcIo* io);
uint8_t IpcIoPopUint8Unaligned(IpcIo* io);
int16_t IpcIoPopInt16(IpcIo* io);
int16_t IpcIoPopInt16Unaligned(IpcIo* io);
uint16_t IpcIoPopUint16(IpcIo* io);
uint16_t IpcIoPopUint16Unaligned(IpcIo* io);
int32_t IpcIoPopInt32(IpcIo* io);
uint32_t IpcIoPopUint32(IpcIo* io);
int64_t IpcIoPopInt64(IpcIo* io);
uint64_t IpcIoPopUint64(IpcIo* io);
float IpcIoPopFloat(IpcIo* io);
double IpcIoPopDouble(IpcIo* io);
uint8_t* IpcIoPopString(IpcIo* io, size_t* sz);
void* IpcIoPopFlatObj(IpcIo* io, uint32_t* size);
SvcIdentity* IpcIoPopSvc(IpcIo* io);
int32_t IpcIoPopFd(IpcIo* io);
BuffPtr* IpcIoPopDataBuff(IpcIo* io);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
