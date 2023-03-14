/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "client_trans_file.h"

#include "client_trans_file_listener.h"
#include "file_adapter.h"
#include "nstackx_dfile.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_utils.h"

#define DEFAULT_KEY_LENGTH 16

static const UdpChannelMgrCb *g_udpChannelMgrCb = NULL;

static void FileSendListener(int32_t dfileId, DFileMsgType msgType, const DFileMsg *msgData)
{
    if (msgData == NULL || msgType == DFILE_ON_BIND || msgType == DFILE_ON_SESSION_IN_PROGRESS ||
        msgType == DFILE_ON_SESSION_TRANSFER_RATE) {
        return;
    }
    UdpChannel udpChannel = {0};
    if (TransGetUdpChannelByFileId(dfileId, &udpChannel) != SOFTBUS_OK) {
        return;
    }
    if (msgType == DFILE_ON_CONNECT_SUCCESS) {
        g_udpChannelMgrCb->OnUdpChannelOpened(udpChannel.channelId);
        return;
    }
    if (msgType == DFILE_ON_CONNECT_FAIL || msgType == DFILE_ON_FATAL_ERROR) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "fatal error occurred.");
        TransOnUdpChannelClosed(udpChannel.channelId);
        return;
    }
    FileListener fileListener = {0};
    if (TransGetFileListener(udpChannel.info.peerSessionName, &fileListener) != SOFTBUS_OK) {
        return;
    }

    int32_t sessionId = -1;
    if (g_udpChannelMgrCb->OnFileGetSessionId(udpChannel.channelId, &sessionId) != SOFTBUS_OK) {
        return;
    }

    switch (msgType) {
        case DFILE_ON_FILE_SEND_SUCCESS:
            if (fileListener.sendListener.OnSendFileFinished != NULL) {
                fileListener.sendListener.OnSendFileFinished(sessionId, msgData->fileList.files[0]);
            }
            break;
        case DFILE_ON_FILE_SEND_FAIL:
            if (fileListener.sendListener.OnFileTransError != NULL) {
                fileListener.sendListener.OnFileTransError(sessionId);
            }
            break;
        case DFILE_ON_TRANS_IN_PROGRESS:
            if (fileListener.sendListener.OnSendFileProcess != NULL) {
                uint64_t bytesUpload = msgData->transferUpdate.bytesTransferred;
                uint64_t bytesTotal = msgData->transferUpdate.totalBytes;
                fileListener.sendListener.OnSendFileProcess(sessionId, bytesUpload, bytesTotal);
            }
            break;
        default:
            break;
    }
}

static void FileReceiveListener(int32_t dfileId, DFileMsgType msgType, const DFileMsg *msgData)
{
    if (msgData == NULL || msgType == DFILE_ON_BIND || msgType == DFILE_ON_SESSION_IN_PROGRESS ||
        msgType == DFILE_ON_SESSION_TRANSFER_RATE) {
        return;
    }
    UdpChannel udpChannel = {0};
    if (TransGetUdpChannelByFileId(dfileId, &udpChannel) != SOFTBUS_OK) {
        return;
    }
    if (msgType == DFILE_ON_CONNECT_FAIL || msgType == DFILE_ON_FATAL_ERROR) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "fatal error occurred.");
        TransOnUdpChannelClosed(udpChannel.channelId);
        return;
    }
    FileListener fileListener = {0};
    if (TransGetFileListener(udpChannel.info.peerSessionName, &fileListener) != SOFTBUS_OK) {
        return;
    }
    int32_t sessionId = -1;
    if (g_udpChannelMgrCb->OnFileGetSessionId(udpChannel.channelId, &sessionId) != SOFTBUS_OK) {
        return;
    }
    const char *firstFile = msgData->fileList.files[0];
    uint32_t fileNum = msgData->fileList.fileNum;
    switch (msgType) {
        case DFILE_ON_FILE_LIST_RECEIVED:
            if (fileListener.recvListener.OnReceiveFileStarted != NULL) {
                fileListener.recvListener.OnReceiveFileStarted(sessionId, firstFile, fileNum);
            }
            break;
        case DFILE_ON_FILE_RECEIVE_SUCCESS:
            if (fileListener.recvListener.OnReceiveFileFinished != NULL) {
                fileListener.recvListener.OnReceiveFileFinished(sessionId, firstFile, fileNum);
            }
            break;
        case DFILE_ON_FILE_RECEIVE_FAIL:
            if (fileListener.recvListener.OnFileTransError != NULL) {
                fileListener.recvListener.OnFileTransError(sessionId);
            }
            break;
        case DFILE_ON_TRANS_IN_PROGRESS:
            if (fileListener.recvListener.OnReceiveFileProcess != NULL) {
                uint64_t bytesUpload = msgData->transferUpdate.bytesTransferred;
                uint64_t bytesTotal = msgData->transferUpdate.totalBytes;
                fileListener.recvListener.OnReceiveFileProcess(sessionId, firstFile, bytesUpload, bytesTotal);
            }
            break;
        default:
            break;
    }
}

int32_t TransOnFileChannelOpened(const ChannelInfo *channel, int32_t *filePort)
{
    if (channel == NULL || filePort == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param.");
        return SOFTBUS_INVALID_PARAM;
    }
    int32_t fileSession;
    (void)NSTACKX_DFileSetCapabilities(NSTACKX_CAPS_UDP_GSO | NSTACKX_CAPS_WLAN_CATAGORY, NSTACKX_WLAN_CAT_TCP);
    if (channel->isServer) {
        FileListener fileListener = {0};
        if (TransGetFileListener(channel->peerSessionName, &fileListener) != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get file listener failed");
            return SOFTBUS_ERR;
        }
        fileSession = StartNStackXDFileServer(channel->myIp, (uint8_t *)channel->sessionKey,
            DEFAULT_KEY_LENGTH, FileReceiveListener, filePort);
        if (fileSession < 0) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "start file channel as server failed");
            return SOFTBUS_ERR;
        }
        if (NSTACKX_DFileSetStoragePath(fileSession, fileListener.rootDir) != SOFTBUS_OK) {
            NSTACKX_DFileClose(fileSession);
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "set storage path failed");
            return SOFTBUS_ERR;
        }
        g_udpChannelMgrCb->OnUdpChannelOpened(channel->channelId);
    } else {
        fileSession = StartNStackXDFileClient(channel->peerIp, channel->peerPort,
            (uint8_t *)channel->sessionKey, DEFAULT_KEY_LENGTH, FileSendListener);
        if (fileSession < 0) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "start file channel as client failed");
            return SOFTBUS_ERR;
        }
    }
    return fileSession;
}

void TransCloseFileChannel(int32_t dfileId)
{
    NSTACKX_DFileClose(dfileId);
}

void RegisterFileCb(const UdpChannelMgrCb *fileCb)
{
    if (fileCb == NULL) {
        g_udpChannelMgrCb = NULL;
        return;
    }
    if (g_udpChannelMgrCb != NULL) {
        return;
    }
    g_udpChannelMgrCb = fileCb;
}

int32_t TransSendFile(int32_t sessionId, const char *sFileList[], const char *dFileList[], uint32_t fileCnt)
{
    if (dFileList == NULL) {
        return NSTACKX_DFileSendFiles(sessionId, sFileList, fileCnt, NULL);
    }
    return NSTACKX_DFileSendFilesWithRemotePath(sessionId, sFileList, dFileList, fileCnt, NULL);
}