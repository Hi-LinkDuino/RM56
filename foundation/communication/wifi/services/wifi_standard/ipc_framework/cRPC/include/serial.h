/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef CRPC_SERIAL_H
#define CRPC_SERIAL_H

#include <stdint.h>
#include "common.h"
#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Write the message start flag to the context
 *
 * @param context - Communication Context
 * @param type - 0 normal message and reply message; 1 callback message
 * @return int - 0 Success; -1 Failed
 */
int WriteBegin(Context *context, int type);

/**
 * @Description Write the function to be invoked by the peer end
 *
 * @param context - Communication Context
 * @param funcName - Name of the peer function to be invoked
 * @return int - 0 Success; -1 Failed
 */
int WriteFunc(Context *context, const char *funcName);

/**
 * @Description Write data of the int type
 *
 * @param context - Communication Context
 * @param i - integer
 * @return int - 0 Success; -1 Failed
 */
int WriteInt(Context *context, int i);

/**
 * @Description Write data of the long type
 *
 * @param context - Communication Context
 * @param L - long integer
 * @return int - 0 Success; -1 Failed
 */
int WriteLong(Context *context, long L);

/**
 * @Description Write data of the int64 type
 *
 * @param context - Communication Context
 * @param i - int64 integer
 * @return int - 0 Success; -1 Failed
 */
int WriteInt64(Context *context, int64_t i);

/**
 * @Description Write data of the double type
 *
 * @param context - Communication Context
 * @param d - double
 * @return int - 0 Success; -1 Failed
 */
int WriteDouble(Context *context, double d);

/**
 * @Description Write data of the char type
 *
 * @param context - Communication Context
 * @param c - char
 * @return int - 0 Success; -1 Failed
 */
int WriteChar(Context *context, char c);

/**
 * @Description Write data of a string type
 *
 * @param context - Communication Context
 * @param str - string
 * @return int - 0 Success; -1 Failed
 */
int WriteStr(Context *context, const char *str);

/**
 * @Description Write data of the unsigned char* type
 *
 * @param context - Communication Context
 * @param uStr - unsigned char
 * @param len - String length
 * @return int - 0 Success; -1 Failed
 */
int WriteUStr(Context *context, const unsigned char *uStr, unsigned int len);

/**
 * @Description Write the message end flag to the context
 *
 * @param context - Communication Context
 * @return int - 0 Success; -1 Failed
 */
int WriteEnd(Context *context);

/**
 * @Description Read the function to be called from the context
 *
 * @param context - communication context
 * @param funcName - the string of the function name
 * @param count - size of funcName
 * @return int - 0 success; < 0 read error; > 0 the message is longger than count, return the length
 */
int ReadFunc(Context *context, char *funcName, int count);

/**
 * @Description Reads data of the int type from the context
 *
 * @param context - Communication Context
 * @param i - pointer to an integer
 * @return int - 0 Success; < 0 read error
 */
int ReadInt(Context *context, int *i);

/**
 * @Description Reads data of the long type from the context
 *
 * @param context - Communication Context
 * @param L - pointer to a long
 * @return int - 0 Success; < 0 read error
 */
int ReadLong(Context *context, long *L);

/**
 * @Description Reads data of the int64 type from the context
 *
 * @param context - Communication Context
 * @param i - pointer to a int64
 * @return int - 0 Success; < 0 read error
 */
int ReadInt64(Context *context, int64_t *i);

/**
 * @Description Reads data of the double type from the context
 *
 * @param context - Communication Context
 * @param d - pointer to a double
 * @return int - 0 Success; < 0 read error
 */
int ReadDouble(Context *context, double *d);

/**
 * @Description Reads data of the char type from the context
 *
 * @param context - Communication Context
 * @param c - pointer to a char
 * @return int - 0 Success; < 0 read error
 */
int ReadChar(Context *context, char *c);

/**
 * @Description Reads data of the char* type from the context
 *
 * @param context - Communication Context
 * @param str - pointer to the string
 * @param count - Size of str
 * @return int - 0 Success; < 0 read error; > 0 the message is longger than count, return the length
 */
int ReadStr(Context *context, char *str, int count);

/**
 * @Description Reads data of the unsigned char* type from the context
 *
 * @param context - Communication Context
 * @param uStr - pointer to the unsigned string
 * @param count - Size of uStr
 * @return int - 0 Success; < 0 read error; > 0 the message is longger than count, return the length
 */
int ReadUStr(Context *context, unsigned char *uStr, int count);

#ifdef __cplusplus
}
#endif
#endif