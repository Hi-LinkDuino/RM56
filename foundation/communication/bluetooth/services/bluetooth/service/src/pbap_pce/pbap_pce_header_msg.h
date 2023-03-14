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

#ifndef PBAP_PCE_HEADER_MSG_H
#define PBAP_PCE_HEADER_MSG_H

#include <cstdint>
#include <cstring>
#include <memory>
#include "../obex/obex_client.h"
#include "../obex/obex_headers.h"
#include "btstack.h"
#include "message.h"
#include "pbap_pce_def.h"
#include "pbap_pce_gap.h"

namespace OHOS {
namespace bluetooth {
/// pbap pce state mathine message
enum PbapPceStmMessage {
    PCE_SDP_FINISH = 0x1001,             // sdp search success
    PCE_SDP_FAILED,                      // sdp search failed
    PCE_GAP_FINISH,                      // gap security success
    PCE_GAP_FAILED,                      // gap security failed
    PCE_REQ_DISCONNECTED,                // disconnect request
    PCE_PASSWORD_INPUT,                  // when device password is input by user
    PCE_OBEX_CONNECTED,                  // obex connected
    PCE_OBEX_CONNECT_FAILED,             // obex connect failed
    PCE_TRANSPORT_FAILED,                // transport failed
    PCE_OBEX_DISCONNECTED,               // obex disconnected
    PCE_REQ_SET_TARGET_STATE,            // request setting target state
    PCE_REQ_TRANSIT_TARGET_STATE,        // transit target state
    PCE_REQ_PULLPHONEBOOKSIZE,           // pull phonebooksize
    PCE_PULLPHONEBOOKSIZE_COMPLETED,     // pull phonebooksize completed
    PCE_REQ_PULLPHONEBOOK,               // pull phonebook
    PCE_PULLPHONEBOOK_COMPLETED,         // pull phonebook completed
    PCE_REQ_SETPHONEBOOK,                // set phonebook
    PCE_SETPHONEBOOK_COMPLETED,          // pull setphonebook completed
    PCE_REQ_PULLVCARDLISTINGSIZE,        // pull vcardlisting size
    PCE_PULLVCARDLISTINGSIZE_COMPLETED,  // pull vcardlisting size completed
    PCE_REQ_PULLVCARDLISTING,            // pull vcard listing
    PCE_PULLVCARDLISTING_COMPLETED,      // pull pullvcardlisting completed
    PCE_REQ_PULLVCARDENTRY,              // pull vcard entry
    PCE_PULLVCARDENTRY_COMPLETED,        // pull pullvcardentry completed
    PCE_REQ_ABORTDOWNLOADING,            // abort downloading
    PCE_ABORTDOWNLOADING_COMPLETED,      // abort downloading completed
};

/**
 * @brief pbap pce message
 * pbap pce message
 */
class PbapPceHeaderSdpMsg {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param obexConfig obex config
     * @param versionNumber version number
     * @param supportedRes supported repositories
     * @param supportedFeature supported features
     * @param featureFlag
     * @return
     */
    explicit PbapPceHeaderSdpMsg(ObexClientConfig &obexConfig, uint16_t versionNumber, uint8_t supportedRes,
        uint32_t supportedFeature, bool featureFlag)
        : obexConfig_(obexConfig),
          versionNumber_(versionNumber),
          supportedRes_(supportedRes),
          supportedFeature_(supportedFeature),
          featureFlag_(featureFlag)
    {}
    /**
     * @brief copy contructor
     * @details copy contructor
     */
    PbapPceHeaderSdpMsg(const PbapPceHeaderSdpMsg &other) = default;
    /**
     * @brief operator=
     * @details operator=
     */
    PbapPceHeaderSdpMsg &operator=(const PbapPceHeaderSdpMsg &other) = default;
    /**
     * @brief destructor
     * @details destructor
     */
    virtual ~PbapPceHeaderSdpMsg() = default;

    /**
     * @brief get obex config
     * @details get obex config
     * @return reference to ObexClientConfig
     */
    const ObexClientConfig &GetObexClientConfig() const
    {
        return obexConfig_;
    }

    /**
     * @brief get version number
     * @details get version number
     * @return uint16_t
     */
    uint16_t GetVersionNumber() const
    {
        return versionNumber_;
    }

    /**
     * @brief get supported repositories
     * @details get supported repositories
     * @return uint8_t
     */
    uint8_t GetSupportedRes() const
    {
        return supportedRes_;
    }

    /**
     * @brief get supported features
     * @details get supported features
     * @return uint32_t
     */
    uint32_t GetSupportedFeature() const
    {
        return supportedFeature_;
    }

    /**
     * @brief get feature flag
     * @details get feature flag
     * @return bool
     */
    bool GetFeatureFlag() const
    {
        return featureFlag_;
    }

    /**
     * @brief get gap pointer
     * @details get gap pointer
     * @return reference to PbapPceGap
     */
    PbapPceGap &GetPbapPceGap() const
    {
        return *pbapPceGapPtr_;
    }

    /**
     * @brief set gap pointer
     * @details set gap pointer
     * @param gap reference to gap
     */
    void SetPbapPceGap(PbapPceGap &gap)
    {
        pbapPceGapPtr_ = &gap;
    }

private:
    ObexClientConfig obexConfig_ {};       // obex config
    uint16_t versionNumber_ = 0;           // received from pse version number
    uint8_t supportedRes_ = 0;             // received from pse Supported Repositories
    uint32_t supportedFeature_ = 0;        // received from pse Supported feature
    bool featureFlag_ = false;             // true:can received feature; false:cannot received feature
    PbapPceGap *pbapPceGapPtr_ = nullptr;  // gap pointer
};

/**
 * @brief pbap pce obex message
 * pbap pce obex message
 */
class PbapPceObexMessage {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param client obex client
     * @param resp obex header
     */
    explicit PbapPceObexMessage(ObexClient &client, const ObexHeader &resp) :
        client_(&client), resp_(std::make_unique<ObexHeader>(resp))
    {
    }

    /**
     * @brief destructor
     * @details destructor
     */
    virtual ~PbapPceObexMessage() = default;

    /**
     * @brief get obex client
     * @details get obex client
     * @return ObexClient*
     */
    ObexClient *GetObexClient() const
    {
        return this->client_;
    }

    /**
     * @brief get obex header
     * @details get obex header
     * @return ObexHeader*
     */
    const ObexHeader *GetObexHeader() const
    {
        return this->resp_.get();
    }

private:
    ObexClient *client_ = nullptr;                // obex client
    std::unique_ptr<ObexHeader> resp_ {nullptr};  // obex header
    BT_DISALLOW_COPY_AND_ASSIGN(PbapPceObexMessage);
};

/**
 * @brief pbap pce obex message for action
 * pbap pce message for action
 */
class PbapPceActionObexMessage : public PbapPceObexMessage {
public:
    explicit PbapPceActionObexMessage(
        ObexClient &client, const ObexHeader &resp, const std::u16string &name, uint8_t flags)
        : PbapPceObexMessage(client, resp), name_(name), flags_(flags)
    {}

    ~PbapPceActionObexMessage() override = default;

    std::u16string GetName() const
    {
        return name_;
    }

    uint8_t GetFlags() const
    {
        return flags_;
    }

private:
    std::u16string name_;
    uint8_t flags_;
};

/**
 * @brief Pbap Pce Password InputMsg
 * Pbap Pce Password InputMsg
 */
class PbapPcePasswordInputMsg {
public:
    explicit PbapPcePasswordInputMsg(const std::string &password, const std::string &userId)
        : password_(password), userId_(userId)
    {}
    explicit PbapPcePasswordInputMsg(const std::string &password) : password_(password), userId_("")
    {}

    virtual ~PbapPcePasswordInputMsg() = default;

    std::string GetPassword() const
    {
        return password_;
    }

    std::string GetUserId() const
    {
        return userId_;
    }

private:
    std::string password_ = "";
    std::string userId_ = "";
};

/**
 * @brief Pbap Pce PullPhoneBookMsg for interthread
 * Pbap Pce PullPhoneBook Msg
 */
class PbapPcePullPhoneBookMsg {
public:
    explicit PbapPcePullPhoneBookMsg() = default;
    virtual ~PbapPcePullPhoneBookMsg() = default;

    std::u16string GetName() const
    {
        return name_;
    }

    void SetName(const std::u16string& name)
    {
        name_ = name;
    }

    uint64_t GetPropertySelector() const
    {
        return propertySelector_;
    }

    void SetPropertySelector(uint64_t selector)
    {
        propertySelector_ = selector;
    }

    uint8_t GetFormat() const
    {
        return format_;
    }

    void SetFormat(uint8_t fmt)
    {
        format_ = fmt;
    }

    uint16_t GetMaxListCount() const
    {
        return maxListCount_;
    }

    void SetMaxListCount(uint16_t max)
    {
        maxListCount_ = max;
    }

    uint16_t GetListStartOffset() const
    {
        return listStartOffset_;
    }

    void SetListStartOffset(uint16_t offset)
    {
        listStartOffset_ = offset;
    }

    uint8_t GetResetNewMissedCalls() const
    {
        return resetNewMissedCalls_;
    }

    void SetResetNewMissedCalls(uint8_t reset)
    {
        resetNewMissedCalls_ = reset;
    }

    uint64_t GetvCardSelector() const
    {
        return vCardSelector_;
    }

    void SetvCardSelector(uint64_t selector)
    {
        vCardSelector_ = selector;
    }

    uint8_t GetvCardSelectorOp() const
    {
        return vCardSelectorOp_;
    }

    void SetvCardSelectorOp(uint8_t op)
    {
        vCardSelectorOp_ = op;
    }

    uint64_t GetSpecifiedBitset() const
    {
        return specifiedBitset_;
    }

    void SetSpecifiedBitset(uint64_t bitSet)
    {
        specifiedBitset_ = bitSet;
    }

private:
    std::u16string name_ = u"";
    uint64_t propertySelector_ = 0ULL;
    uint8_t format_ = 0;
    uint16_t maxListCount_ = 0;
    uint16_t listStartOffset_ = 0;
    uint8_t resetNewMissedCalls_ = 0;
    uint64_t vCardSelector_ = 0ULL;
    uint8_t vCardSelectorOp_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap Pce SetPhoneBookMsg for interthread
 * Pbap Pce SetPhoneBook Msg
 */
class PbapPceSetPhoneBookMsg {
public:
    explicit PbapPceSetPhoneBookMsg(const std::u16string &name, uint8_t flag) : name_(name), flag_(flag)
    {}
    virtual ~PbapPceSetPhoneBookMsg() = default;

    std::u16string GetName() const
    {
        return name_;
    }

    uint8_t GetFlag() const
    {
        return flag_;
    }

private:
    std::u16string name_ = u"";
    uint8_t flag_ = 0;
};

/**
 * @brief Pbap Pce PullvCardListing Msg for interthread
 * Pbap Pce PullvCardListing Msg
 */
class PbapPcePullvCardListingMsg {
public:
    explicit PbapPcePullvCardListingMsg() = default;
    virtual ~PbapPcePullvCardListingMsg() = default;

    std::u16string GetName() const
    {
        return name_;
    }

    void SetName(const std::u16string& name)
    {
        name_ = name;
    }

    uint8_t GetOrder() const
    {
        return order_;
    }

    void SetOrder(uint8_t order)
    {
        order_ = order;
    }

    std::string GetSearchValue() const
    {
        return searchValue_;
    }

    void SetSearchValue(const std::string& val)
    {
        searchValue_ = val;
    }

    uint8_t GetSearchProperty() const
    {
        return searchProperty_;
    }

    void SetSearchProperty(uint8_t prop)
    {
        searchProperty_ = prop;
    }

    uint16_t GetMaxListCount() const
    {
        return maxListCount_;
    }

    void SetMaxListCount(uint16_t max)
    {
        maxListCount_ = max;
    }

    uint16_t GetListStartOffset() const
    {
        return listStartOffset_;
    }

    void SetListStartOffset(uint16_t offset)
    {
        listStartOffset_ = offset;
    }

    uint8_t GetResetNewMissedCalls() const
    {
        return resetNewMissedCalls_;
    }

    void SetResetNewMissedCalls(uint8_t reset)
    {
        resetNewMissedCalls_ = reset;
    }

    uint64_t GetvCardSelector() const
    {
        return vCardSelector_;
    }

    void SetvCardSelector(uint64_t selector)
    {
        vCardSelector_ = selector;
    }

    uint8_t GetvCardSelectorOp() const
    {
        return vCardSelectorOp_;
    }

    void SetvCardSelectorOp(uint8_t op)
    {
        vCardSelectorOp_ = op;
    }

    uint64_t GetSpecifiedBitset() const
    {
        return specifiedBitset_;
    }

    void SetSpecifiedBitset(uint64_t bitSet)
    {
        specifiedBitset_ = bitSet;
    }

private:
    std::u16string name_ = u"";
    uint8_t order_ = 0;
    std::string searchValue_ = "";
    uint8_t searchProperty_ = 0;
    uint16_t maxListCount_ = 0;
    uint16_t listStartOffset_ = 0;
    uint8_t resetNewMissedCalls_ = 0;
    uint64_t vCardSelector_ = 0ULL;
    uint8_t vCardSelectorOp_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap Pce PullvCardEntry Msg for interthread
 * Pbap Pce PullvCardEntry Msg
 */
class PbapPcePullvCardEntryMsg {
public:
    explicit PbapPcePullvCardEntryMsg(
        const std::u16string &name, uint64_t propertySelector, uint8_t format, uint64_t specifiedBitset)
        : name_(name), propertySelector_(propertySelector), format_(format), specifiedBitset_(specifiedBitset)
    {}

    virtual ~PbapPcePullvCardEntryMsg() = default;

    std::u16string GetName() const
    {
        return name_;
    }

    uint64_t GetPropertySelector() const
    {
        return propertySelector_;
    }

    uint8_t GetFormat() const
    {
        return format_;
    }

    uint64_t GetSpecifiedBitset() const
    {
        return specifiedBitset_;
    }

private:
    std::u16string name_ = u"";
    uint64_t propertySelector_ = 0ULL;
    uint8_t format_ = 0;
    uint64_t specifiedBitset_ = 0ULL;
};

/**
 * @brief Pbap Pce abort action Msg for interthread
 * Pbap Pce abort action Msg
 */
class PbapPceAbortMsg {
public:
    explicit PbapPceAbortMsg(const std::u16string &name) : name_(name)
    {}

    virtual ~PbapPceAbortMsg() = default;

    std::u16string GetName() const
    {
        return name_;
    }

private:
    std::u16string name_;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PCE_HEADER_MSG_H
