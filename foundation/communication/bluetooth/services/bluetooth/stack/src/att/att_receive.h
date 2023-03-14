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

/**
 * @file att_receive.h
 *
 * @brief declare receive function to be called.
 *
 */

#ifndef ATT_RECEIVE_H
#define ATT_RECEIVE_H

#include "att_common.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief received error response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttErrorResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received exchangeMTU request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExchangeMTURequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received exchangeMTU response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExchangeMTUResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received findinformation request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindInformationRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received findinformation response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindInformationResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received findbytypevalue request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindByTypeValueRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received findbytypevalue response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttFindByTypeValueResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readbytype request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByTypeRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readbytype response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByTypeResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received read request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received read response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readblob request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadBlobRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readblob response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadBlobResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readmultiple request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadMultipleRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readmultiple response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadMultipleResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readbygrouptype request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByGroupTypeRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received readbygrouptype response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttReadByGroupTypeResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received write request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received write response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received write command.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttWriteCommand(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received signedwrite command.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttSignedWriteCommand(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received preparewrite request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttPrepareWriteRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received preparewrite response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttPrepareWriteResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received executewrite request.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExecuteWriteRequest(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received executewrite response.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttExecuteWriteResponse(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received handlevalue notification.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueNotification(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received handlevalue indication.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueIndication(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief received handlevalue confirmation.
 *
 * @param1 connect Indicates the AttConnectInfo.
 * @param2 buffer Indicates the pointer to Buffer.
 */
void AttHandleValueConfirmation(AttConnectInfo *connect, const Buffer *buffer);

/**
 * @brief initialize recv data function.
 *
 */
void FunctionListInit();

#ifdef __cplusplus
}
#endif

#endif  // ATT_RECEIVE_H
