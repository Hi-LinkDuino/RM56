/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "mct_protocol.h"

#include <cerrno>
#include <cstring>

#include <hdf_log.h>

namespace OHOS {
namespace HDI {
namespace BT {
namespace HCI {
MctProtocol::MctProtocol(int fds[HCI_MAX_CHANNEL], HciDataCallback onAclReceive, HciDataCallback onScoReceive,
    HciDataCallback onEventReceive)
{
    for (int ii = 0; ii < HCI_MAX_CHANNEL; ii++) {
        hciFds_[ii] = fds[ii];
    }
    onAclReceive_ = onAclReceive;
    onScoReceive_ = onScoReceive;
    onEventReceive_ = onEventReceive;
}

ssize_t MctProtocol::SendPacket(HciPacketType packetType, const std::vector<uint8_t> &packetData)
{
    uint8_t type = packetType;

    if (packetType == HciPacketType::HCI_PACKET_TYPE_COMMAND) {
        return Write(hciFds_[hci_channels_t::HCI_CMD], &type, sizeof(type));
    } else if (packetType == HciPacketType::HCI_PACKET_TYPE_ACL_DATA) {
        return Write(hciFds_[hci_channels_t::HCI_ACL_OUT], &type, sizeof(type));
    }

    return 0;
}

void MctProtocol::ReadEventData(int fd)
{
    ssize_t readLen;
    if (eventPacket_.size() == header_[HCI_PACKET_TYPE_EVENT].headerSize) {
        readLen = Read(fd, eventPacket_.data() + eventReadLength_, eventPacket_.size() - eventReadLength_);
        if (readLen < 0) {
            HDF_LOGE("read fd[%d] err:%s", fd, strerror(errno));
            return;
        } else if (readLen == 0) {
            HDF_LOGE("read fd[%d] readLen = 0.", fd);
            return;
        }

        eventReadLength_ += readLen;
        if (eventReadLength_ == eventPacket_.size()) {
            size_t dataLen = 0;
            dataLen += eventPacket_[header_[HCI_PACKET_TYPE_EVENT].dataLengthOffset];
            eventPacket_.resize(eventPacket_.size() + dataLen);
        }
    } else {
        readLen = Read(fd, eventPacket_.data() + eventReadLength_, eventPacket_.size() - eventReadLength_);
        if (readLen < 0) {
            HDF_LOGE("read fd[%d] err:%s", fd, strerror(errno));
            return;
        } else if (readLen == 0) {
            HDF_LOGE("read fd[%d] readLen = 0.", fd);
            return;
        }

        eventReadLength_ += readLen;
        if (eventReadLength_ == eventPacket_.size()) {
            if (onEventReceive_) {
                onEventReceive_(eventPacket_);
            }
            eventPacket_.resize(header_[HCI_PACKET_TYPE_EVENT].headerSize);
            eventReadLength_ = 0;
        }
    }
}

void MctProtocol::ReadAclData(int fd)
{
    ssize_t readLen;
    if (aclPacket_.size() == header_[HCI_PACKET_TYPE_ACL_DATA].headerSize) {
        readLen = Read(fd, aclPacket_.data() + aclReadLength_, aclPacket_.size() - aclReadLength_);
        if (readLen < 0) {
            HDF_LOGE("read fd[%d] err:%s", fd, strerror(errno));
            return;
        } else if (readLen == 0) {
            HDF_LOGE("read fd[%d] readLen = 0.", fd);
            return;
        }

        aclReadLength_ += readLen;
        if (aclReadLength_ == aclPacket_.size()) {
            size_t dataLen = 0;
            dataLen += aclPacket_[header_[HCI_PACKET_TYPE_ACL_DATA].dataLengthOffset];
            aclPacket_.resize(aclPacket_.size() + dataLen);
        }
    } else {
        readLen = Read(fd, aclPacket_.data() + aclReadLength_, aclPacket_.size() - aclReadLength_);
        if (readLen < 0) {
            HDF_LOGE("read fd[%d] err:%s", fd, strerror(errno));
            return;
        } else if (readLen == 0) {
            HDF_LOGE("read fd[%d] readLen = 0.", fd);
            return;
        }

        aclReadLength_ += readLen;
        if (aclReadLength_ == aclPacket_.size()) {
            if (onAclReceive_) {
                onAclReceive_(aclPacket_);
            }
            aclPacket_.resize(header_[HCI_PACKET_TYPE_ACL_DATA].headerSize);
            aclReadLength_ = 0;
        }
    }
}
}  // namespace HCI
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS