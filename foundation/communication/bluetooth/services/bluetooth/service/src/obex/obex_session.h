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

#ifndef OBEX_SESSION_H
#define OBEX_SESSION_H

#include <cstdint>
#include "btstack.h"
#include "dispatcher.h"
#include "obex_body.h"
#include "obex_headers.h"
#include "obex_transport.h"

namespace OHOS {
namespace bluetooth {
enum ObexReliableSessionState {
    OBEX_RELIABLE_SESSION_INIT = 0,
    OBEX_RELIABLE_SESSION_ACTIVE,
    OBEX_RELIABLE_SESSION_SUSPENDED,
    OBEX_RELIABLE_SESSION_RESUMED,
    OBEX_RELIABLE_SESSION_CLOSED
};
class ObexSession {
public:
    struct ObexReliableSession {
        // session’s last modification in seconds since January 1, 1970.
        uint32_t lastAccessTimestampSec_ = 0;

        // This is a 16-byte value, which is formed by taking the device
        // address and nonce from the client and server and running the MD5 algorithm
        // over the resulting string. The Session ID is created as follows: MD5(“Client
        // Device Address” “Client Nonce” “Server Device Address” “Server Nonce”)
        uint8_t sessionId_[16];

        // This is a 4-byte value that contains the number of seconds a session can be
        // in suspend mode before it is considered closed. The value of 0xffffffff
        // indicates a timeout of infinity. This is the default timeout. If a device does not
        // send a timeout field then it can be assumed that the desired timeout is infinity.
        // The timeout in affect is the smallest timeout sent by the client or server
        uint32_t timeoutSec_ = 0;

        // the counting capacity of the sequence number is 256 using digits 0 – 255 (0xFF). At 255 the sequence
        // number wraps around to 0.
        // When the session is first created the Session-Sequence-Number number is set to 0. The first command
        // packet sent by the client after the session is established will contain a Session-Sequence-Number
        // header with value of 0. The response to this packet from the server contains the next packet number
        // expected. If the packet was received successfully then the value of the Session-Sequence-Number
        // header in the response would be 1.
        uint8_t sessionSequenceNumber_ = 0;

        // ObexReliableSessionState
        ObexReliableSessionState sessionState_;
    };
    explicit ObexSession(const RawAddress &remoteAddr);
    virtual ~ObexSession() = default;
    const RawAddress &GetRemoteAddr() const
    {
        return remoteAddr_;
    }
    const uint16_t &GetMaxPacketLength() const
    {
        return maxPacketLength_;
    }
    void SetMaxPacketLength(uint16_t maxPacketLength)
    {
        maxPacketLength_ = maxPacketLength;
    }
    const uint32_t &GetConnectId() const
    {
        return connectId_;
    }
    void SetConnectId(uint32_t connectId)
    {
        connectId_ = connectId;
    }
    const BtUuid &GetServiceUUID() const
    {
        return serviceUUID_;
    }
    void SetServiceUUID(const BtUuid &serviceUUID)
    {
        serviceUUID_ = serviceUUID;
    }
    const bool &IsReliableSession() const
    {
        return isReliableSession_;
    }
    const bool &IsBusy() const
    {
        return isBusy_;
    }
    void SetBusy(bool isBusy)
    {
        isBusy_ = isBusy;
    }
    const std::unique_ptr<ObexReliableSession> &GetReliableSession() const
    {
        return reliableSession_;
    }

protected:
    // The device address of the device sending the header.
    RawAddress remoteAddr_ {""};

    // The MOPL that an OBEX client can receive is stated in the OBEX Connect request,
    // this shall be less than or equal to the MTU configured for packets received by L2CAP for the OBEX connection.
    // A separate, and possibly different, MOPL is stated by the OBEX server in the OBEX Connect response,
    // this shall be less than or equal to the L2CAP MTU configured by L2CAP for the OBEX connection.
    // GOEP implementations shall support a minimum MOPL packet size of 255 bytes.
    // For OBEX over L2CAP this means that implementations shall also support a minimum MTU of 255 bytes.
    uint16_t maxPacketLength_ = 0;

    // Connection Id is a 4-byte value that tells the recipient of the request which OBEX connection this
    // request belongs to. The Connection Id header is optional. When in use, the Connection Id header must
    // be the first header in the request.
    uint32_t connectId_ = 0;

    // Target is a byte sequence that identifies the intended target of the operation. On the receiving end, object
    // name and type information provide one way of performing dispatching - this header offers an alternate
    // way of directing an operation to the intended recipient.
    BtUuid serviceUUID_ {};

    // true: reliable session
    bool isReliableSession_ = false;
    bool isBusy_ = false;
    // ObexReliableSession
    std::unique_ptr<ObexReliableSession> reliableSession_ = nullptr;
};
class ObexSetPathObject {
public:
    explicit ObexSetPathObject(const std::vector<std::u16string> &paths);
    virtual ~ObexSetPathObject() = default;
    std::u16string GetNextPath();
    bool IsDone() const;

private:
    std::vector<std::u16string> paths_ {};
    size_t pos_ = 0;
};

class ObexClientSendObject {
public:
    ObexClientSendObject(const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> bodyReader, uint16_t mtu);
    virtual ~ObexClientSendObject() = default;
    std::unique_ptr<ObexHeader> GetNextReqHeader(bool useSrm = false);
    bool IsDone() const;
    void SetSrmEnable(bool srmEnable);
    bool IsSrmEnable() const;
    void SetSrmWait(bool wait);
    bool IsSrmWait() const;
    void SetSrmSending();
    bool IsSrmSending() const;
    void SetBusy(bool isBusy);
    bool IsBusy() const;

private:
    bool SetBodyToHeader(ObexHeader &header, const uint16_t &remainLength);
    int reqIndex_ = 0;
    bool srmEnable_ = false;
    bool srmWait_ = false;
    bool isDone_ = false;
    bool isSrmSending_ = false;
    bool isBusy_ = false;
    uint16_t mtu_ = 0;
    std::unique_ptr<ObexHeader> firstReq_ = nullptr;
    std::shared_ptr<ObexBodyObject> bodyReader_ = nullptr;
};

class ObexClientReceivedObject {
public:
    ObexClientReceivedObject(
        const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> writer, bool supportSrm, int srmpCount);
    virtual ~ObexClientReceivedObject() = default;
    ObexHeader &GetFirstReqHeader() const;
    std::unique_ptr<ObexHeader> GetContinueReqHeader();
    std::shared_ptr<ObexBodyObject> &GetBodyWriter();
    const ObexHeader *GetFirstRespHeader() const;
    void CacheResp(const ObexHeader &resp);
    void AppendBody(const uint8_t *body, const uint16_t bodyLen);
    bool IsDone() const;
    void SetDone(bool done);
    void SetSrmEnable(bool srmEnable);
    bool IsSrmEnable() const;
    void SetSrmWait(bool wait);
    bool IsSrmWait() const;
    bool IsSupportSrmMode() const;
    bool IsSendNextReq() const;

private:
    bool srmEnable_ = false;
    bool srmWait_ = false;
    bool isDone_ = false;
    bool supportSrmMode_ = false;
    int srmpCount_ = 0;
    bool srmpSended_ = false;
    size_t receivedSize_ = 0;
    std::unique_ptr<ObexHeader> firstReq_ = nullptr;
    std::shared_ptr<ObexBodyObject> bodyWriter_ = nullptr;
    std::unique_ptr<ObexHeader> firstResp_ = nullptr;
};

class ObexClientSession : public ObexSession {
public:
    explicit ObexClientSession(const RawAddress &remoteAddr);
    ~ObexClientSession() override = default;
    const uint8_t &GetLastOpeId() const
    {
        return lastOpeId_;
    }
    void SetLastOpeId(uint8_t lastOpeId)
    {
        lastOpeId_ = lastOpeId;
    }
    const uint8_t &GetLastRespCd() const
    {
        return lastRespCd_;
    }
    void SetLastRespCd(uint8_t lastRespCd)
    {
        lastRespCd_ = lastRespCd;
    }
    const std::unique_ptr<ObexHeader> &GetLastReqHeader() const
    {
        return lastReqHeader_;
    }
    void SetLastReqHeader(const ObexHeader &req)
    {
        lastReqHeader_ = std::make_unique<ObexHeader>(req);
    }
    std::unique_ptr<ObexSetPathObject> &GetSetPathObject()
    {
        return objForSetPath_;
    }
    void CreateSetPathObject(const std::vector<std::u16string> &paths)
    {
        objForSetPath_ = std::make_unique<ObexSetPathObject>(paths);
    }
    void FreeSetPathObject()
    {
        objForSetPath_ = nullptr;
    }

    std::unique_ptr<ObexClientReceivedObject> &GetReceivedObject()
    {
        return receivedObject_;
    }
    std::unique_ptr<ObexClientReceivedObject> &CreateReceivedObject(
        const ObexHeader &req, std::shared_ptr<ObexBodyObject> writer, bool supportSrm, int srmpCount)
    {
        receivedObject_ = std::make_unique<ObexClientReceivedObject>(req, writer, supportSrm, srmpCount);
        return receivedObject_;
    }
    void FreeReceivedObject()
    {
        receivedObject_ = nullptr;
    }

    std::unique_ptr<ObexClientSendObject> &GetSendObject()
    {
        return sendObject_;
    }
    std::unique_ptr<ObexClientSendObject> &CreateSendObject(
        const ObexHeader &req, std::shared_ptr<ObexBodyObject> &bodyReader, uint16_t &mtu)
    {
        sendObject_ = std::make_unique<ObexClientSendObject>(req, bodyReader, mtu);
        return sendObject_;
    }
    void FreeSendObject()
    {
        sendObject_ = nullptr;
    }

private:
    std::unique_ptr<ObexClientSendObject> sendObject_ = nullptr;
    std::unique_ptr<ObexClientReceivedObject> receivedObject_ = nullptr;
    std::unique_ptr<ObexSetPathObject> objForSetPath_ = nullptr;
    std::unique_ptr<ObexHeader> lastReqHeader_ = nullptr;
    uint8_t lastOpeId_ = 0;
    uint8_t lastRespCd_ = 0;
};

class ObexServerReceivedObject {
public:
    ObexServerReceivedObject(const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> writer);
    virtual ~ObexServerReceivedObject() = default;
    void AppendBody(const uint8_t *body, const uint16_t bodyLen);
    std::shared_ptr<ObexBodyObject> &GetWriter();
    std::unique_ptr<ObexHeader> CreateReceivedHeader();
    void SetSrmEnable(bool srmEnable);
    bool IsSrmEnable() const;
    void SetSrmWait(bool wait);
    bool IsSrmWait() const;
    void SetSrmReceiving();
    bool IsSrmReceiving() const;

private:
    std::unique_ptr<ObexHeader> baseReq_ = nullptr;
    std::shared_ptr<ObexBodyObject> writer_ = nullptr;
    bool srmEnable_ = false;
    bool srmWait_ = false;
    bool isSrmReceiving_ = false;
    size_t receivedSize_ = 0;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerReceivedObject);
};

class ObexServerSendObject {
public:
    ObexServerSendObject(const ObexHeader &firstReq, const ObexHeader &firstResp,
        std::shared_ptr<ObexBodyObject> bodyReader, uint16_t mtu, bool supportSrm);
    virtual ~ObexServerSendObject() = default;
    std::unique_ptr<ObexHeader> GetNextRespHeader();
    bool IsDone() const;
    void SetSrmEnable(bool srmEnable);
    bool IsSrmEnable() const;
    void SetSrmWait(bool wait);
    bool IsSrmWait() const;
    void SetStartBodyResp(bool start);
    void SetSrmSending();
    bool IsSrmSending() const;
    void SetBusy(bool isBusy);
    bool IsBusy() const;
    bool IsSupportSrmMode() const;

private:
    void SetSrmParam(ObexHeader &header);
    bool SetBodyToHeader(ObexHeader &header, const uint16_t &remainLength);
    int reqIndex_ = 0;
    bool srmEnable_ = false;
    bool srmWait_ = false;
    bool isDone_ = false;
    bool supportSrmMode_ = false;
    bool startBodyResp_ = false;
    bool isSrmSending_ = false;
    bool isBusy_ = false;
    uint16_t mtu_ = 0;
    std::unique_ptr<ObexHeader> firstReq_ = nullptr;
    std::shared_ptr<ObexBodyObject> bodyReader_ = nullptr;
    std::unique_ptr<ObexHeader> firstResp_ = nullptr;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerSendObject);
};
class ObexServerSession : public ObexSession {
public:
    ObexServerSession(ObexTransport &transport, bool isSupportSrm, utility::Dispatcher &dispatcher,
        std::function<int(ObexServerSession &)> removeFun, std::function<void(ObexServerSession &, bool)> setBusyFun);
    ~ObexServerSession() override = default;
    int Disconnect();
    int SendResponse(ObexHeader &resp) const;
    int SendGetResponse(const ObexHeader &req, ObexHeader &resp, std::shared_ptr<ObexBodyObject> reader = nullptr);
    int SendSimpleResponse(ObexRspCode rspCode) const;
    int SendSrmResponse();
    void Invalid();
    ObexTransport &GetTransport() const
    {
        return transport_;
    }
    std::unique_ptr<ObexServerSendObject> &GetSendObject()
    {
        return sendObject_;
    }
    void FreeSendObject()
    {
        sendObject_ = nullptr;
    }
    std::unique_ptr<ObexServerReceivedObject> &GetReceivedObject()
    {
        return receivedObject_;
    }
    std::unique_ptr<ObexServerReceivedObject> &CreateReceivedObject(
        const ObexHeader &firstReq, std::shared_ptr<ObexBodyObject> &writer)
    {
        receivedObject_ = std::make_unique<ObexServerReceivedObject>(firstReq, writer);
        return receivedObject_;
    }
    void FreeReceivedObject()
    {
        receivedObject_ = nullptr;
    }
    bool IsSupportSrmMode() const
    {
        return isSupportSrm_;
    }

private:
    int ProcessSendSrmResponse();
    std::unique_ptr<ObexServerSendObject> sendObject_ = nullptr;
    std::unique_ptr<ObexServerReceivedObject> receivedObject_ = nullptr;
    ObexTransport &transport_;
    bool isSupportSrm_ = false;
    utility::Dispatcher &dispatcher_;
    std::function<int(ObexServerSession &)> removeFun_ {};
    std::function<void(ObexServerSession &, bool)> setBusyFun_ {};
    bool invalid_ = false;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexServerSession);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_SESSION_H
