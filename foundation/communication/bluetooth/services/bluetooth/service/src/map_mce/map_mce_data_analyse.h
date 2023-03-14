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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines map client service request data analyse object.
 *
 */

/**
 * @file map_mce_data_analyse.h
 *
 * @brief map client service request data analyse header file .
 *
 */

#ifndef MAP_MCE_DATA_ANALYSE_H
#define MAP_MCE_DATA_ANALYSE_H

#include <cstdint>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "interface_profile_map_mce.h"
#include "map_mce_xml.h"

namespace OHOS {
namespace bluetooth {
enum MceAnalyseEnumType {
    MCE_RECIPIENT_LEVEL1 = 1,
    MCE_RECIPIENT_LEVEL2 = 2,
    MCE_RECIPIENT_LEVEL3 = 3
};
/**
 * @brief split string
 */
class MceSplitString {
public:
    /**
     * @brief Construct a new Mce Split String object
     * @param  target   target string
     * @param  filter   split filter
     */
    MceSplitString(const std::string &target, const std::string &filter);
    /**
     * @brief Destroy the Mce Split String object
     */
    ~MceSplitString();
    /**
     * @brief next word
     * @return std::string string of next word
     */
    std::string NextWord();

private:
    // count number
    int count_ = 0;
    // max of count
    int maxCount_ = 0;
    // target string
    std::string targetString_ = "";
    // filter string
    std::string filterString_ = "";
};
/**
 * @brief mce combine node
 */
class MceCombineNode {
public:
    /**
     * @brief Construct a new Mce Combine Node object
     */
    MceCombineNode();
    /**
     * @brief Destroy the Mce Combine Node object
     */
    ~MceCombineNode();
    /**
     * @brief Node Begin process
     * @param  nodeName         node name string
     */
    void NodeBegin(std::string nodeName);
    /**
     * @brief node end process
     */
    void NodeEnd();
    /**
     * @brief  add app parameter String
     * @param  param    parameter
     * @param  value    string value
     */
    void AddParamStr(const std::string &param, const std::string &value);
    /**
     * @brief  add text string
     * @param  text      text string
     */
    void AddText(const std::string &text);
    /**
     * @brief Get the Combine String object
     * @return std::string
     */
    std::string GetCombineString() const;

private:
    // not name list
    std::list<std::string> nodeNameList_ {};
    // string stream
    std::ostringstream stream_ {};
};
/**
 * @brief mce bmessage node
 */
class MceBMessageNode {
public:
    /**
     * @brief Construct a new Mce B Message Node object
     */
    MceBMessageNode();
    /**
     * @brief Construct a new Mce B Message Node object
     * @param  list             string list
     * @param  name             node name
     */
    MceBMessageNode(const std::list<std::string> &list, const std::string &name);
    /**
     * @brief Destroy the Mce B Message Node object
     */
    ~MceBMessageNode();
    /**
     * @brief  Mce Construct Node
     * @param  list             string list
     * @param  name             node name
     */
    void MceConstructNode(const std::list<std::string> &list, const std::string &name);
    /**
     * @brief Get the Next Child object
     * @return MceBMessageNode
     */
    MceBMessageNode GetNextChild();
    /**
     * @brief Get the Node Name object
     * @return std::string
     */
    std::string GetNodeName() const;
    /**
     * @brief Get the Node Value object
     * @return std::string
     */
    std::string GetNodeValue() const;
    /**
     * @brief Get the Parameter Value object
     * @param  param            parameter
     * @return std::string
     */
    std::string GetParamValue(std::string param) const;
    /**
     * @brief Get the Parameter Value List object
     * @param  param            parameter
     * @return std::vector<std::string>
     */
    std::vector<std::string> GetParamValueList(std::string param) const;
    /**
     * @brief Get the Size object
     * @return int
     */
    int GetSize() const;

private:
    // node name
    std::string name_ = "";
    // node value
    std::string value_ = "";
    // string list
    std::list<std::string> wordList_ {};
};
/**
 * @brief Mce Bmessage Parameter Make String Object
 */
class MceBmessageParamMakeStringObject {
public:
    /**
     * @brief Construct a new Mce Bmessage param Make String Object object
     * @param  param            Parameter
     */
    explicit MceBmessageParamMakeStringObject(const IProfileBMessageStruct &param);
    /**
     * @brief Destroy the Mce Bmessage param Make String Object object
     */
    ~MceBmessageParamMakeStringObject();
    /**
     * @brief Get the String Object object
     * @return std::string
     */
    std::string GetStringObject() const;

private:
    /**
     * @brief Make BMessage Property
     */
    void MakeBMessageProperty();
    /**
     * @brief Make Recipient Level1
     */
    void MakeRecipientLevel1();
    /**
     * @brief Make Recipient Level2
     */
    void MakeRecipientLevel2();
    /**
     * @brief Make Recipient Level3
     */
    void MakeRecipientLevel3();
    /**
     * @brief Make Message Body Property
     */
    void MakeMsgBodyProperty();
    /**
     * @brief Make Message Body Text
     */
    void MakeMsgBodyText();
    /**
     * @brief  Make Vcard
     * @param  vcard             vcard list
     */
    void MakeVcard(std::vector<IProfileMapVcard> vcard);
    // string object
    std::string stringObject_ = "";
    // message parameter
    IProfileBMessageStruct msgParam_ {};
    // combine node
    MceCombineNode combineNode_ {};
};
/**
 * @brief Mce Bmessage Parameter Analyser
 */
class MceBmessageParamAnalyser {
public:
    /**
     * @brief Construct a new Mce Bmessage param Analyser object
     * @param  object           string object
     */
    explicit MceBmessageParamAnalyser(const std::string &object);
    /**
     * @brief Destroy the Mce Bmessage param Analyser object
     */
    ~MceBmessageParamAnalyser();
    /**
     * @brief Get the Msg Struct object
     * @return IProfileBMessageStruct
     */
    IProfileBMessageStruct GetMsgStruct() const;
    /**
     * @brief Get the First Child Node object
     * @return MceBMessageNode
     */
    MceBMessageNode GetFirstChildNode();
    /**
     * @brief Get the Size object
     * @return int
     */
    int GetSize() const;
    /**
     * @brief Start Analyse Process
     */
    void StartAnalyse();
    /**
     * @brief Get the Msg Text object
     * @return std::string
     */
    std::string GetMsgText();

private:
    /**
     * @brief make word list
     */
    void MakeWordList();
    /**
     * @brief  PickUp BMessage Property
     * @param  node             message node
     */
    void PickUpBMessageProperty(const MceBMessageNode &node);
    /**
     * @brief  PickUp BMessage Property Originator
     * @param  node             message node
     */
    void PickUpBMessagePropertyOriginator(const MceBMessageNode &node);
    /**
     * @brief  PickUp Recipient Level1
     * @param  node             message node
     * @return MceBMessageNode
     */
    MceBMessageNode PickUpRecipientLevel1(MceBMessageNode &node);
    /**
     * @brief  PickUp Recipient Level2
     * @param  node             message node
     * @return MceBMessageNode
     */
    MceBMessageNode PickUpRecipientLevel2(MceBMessageNode &node);
    /**
     * @brief  PickUp Recipient Level3
     * @param  node             message node
     */
    void PickUpRecipientLevel3(MceBMessageNode &node);
    /**
     * @brief PickUp Message Body Property
     */
    void PickUpMsgBodyProperty();
    /**
     * @brief PickUp Message Body Text
     */
    void PickUpMsgBodyText();
    // message string object
    std::string msgStr_ = "";
    // word string list
    std::list<std::string> wordList_ {};
    // bmessage parameter
    IProfileBMessageStruct bMsgParamStruct_ {};
    // message node
    MceBMessageNode msgNode_ {};
};
/**
 * @brief Mce Types Conversation Listing
 */
class MceTypesConversationListing {
public:
    /**
     * @brief  Build Object Data
     * @param  stringParam      string parameter
     * @param  stringObject     string object
     * @return int
     */
    int BuildObjectData(const IProfileConversationListingParamStruct &stringParam, const std::string &stringObject);
    /**
     * @brief Get the List object
     * @return std::vector<IProfileConversation>
     */
    std::vector<IProfileConversation> GetList() const;
    /**
     * @brief Get the param object
     * @return IProfileConversationListingParamStruct
     */
    IProfileConversationListingParamStruct GetParam() const;
    /**
     * @brief Get the String Object object
     * @return std::string
     */
    std::string GetStringObject() const;

private:
    /**
     * @brief  PickUp Participant
     * @param  data             Participant data
     * @param  node             xml node
     */
    static void PickUpParticipant(IProfileParticipant &data, const MceXmlNode &node);
    /**
     * @brief  PickUp Conversation
     * @param  data             Conversation data
     * @param  node             xml node
     */
    static void PickUpConversation(IProfileConversation &data, const MceXmlNode &node);
    // conversation list
    std::vector<IProfileConversation> conversationList_ {};
    // conversationListing Parameter
    IProfileConversationListingParamStruct conversationListingParam_ {};
    // conversationListing Object
    std::string conversationListingObject_ = "";
};
/**
 * @brief Mce Types MessagesListing
 */
class MceTypesMessagesListing {
public:
    /**
     * @brief  Build Object Data
     * @param  stringParam      parameter
     * @param  stringObject     string object
     * @return int
     */
    int BuildObjectData(const IProfileMessagesListingParamStruct &stringParam, const std::string &stringObject);
    /**
     * @brief Get the List object
     * @return std::vector<IProfileMessageOutline>
     */
    std::vector<IProfileMessageOutline> GetList() const;
    /**
     * @brief Get the param object
     * @return IProfileMessagesListingParamStruct
     */
    IProfileMessagesListingParamStruct GetParam() const;
    /**
     * @brief Get the String Object object
     * @return std::string
     */
    std::string GetStringObject() const;

private:
    // message list
    std::vector<IProfileMessageOutline> messageList_ {};
    // message list parameter
    IProfileMessagesListingParamStruct messagesListingParam_ {};
    // message listing object
    std::string MessagesListingObject_ = "";
    /**
     * @brief  Pickup Outline Parameter
     * @param  outline          outline Parameter
     * @param  node             xml node
     */
    static void PickupOutlineParam(IProfileMessageOutline &msgOutline, const MceXmlNode &msgNode);
};
/**
 * @brief Mce Types BMessage
 */
class MceTypesBMessage {
public:
    /**
     * @brief  Build Object Data
     * @param  stringObject     string object
     * @return int
     */
    int BuildObjectData(const std::string &stringObject);
    /**
     * @brief Set the Fraction Deliver object
     * @param  value            FractionDeliver Type
     * @return int
     */
    int SetFractionDeliver(MapFractionDeliverType value);
    /**
     * @brief Get the Fraction Deliver object
     * @return MapFractionDeliverType
     */
    MapFractionDeliverType GetFractionDeliver() const;
    /**
     * @brief  Get BMessage Data
     * @return IProfileBMessageStruct
     */
    IProfileBMessageStruct GetBMessageData() const;
    /**
     * @brief Get the Bmessage Object object
     * @return std::string
     */
    std::string GetBmessageObject() const;

private:
    // Fraction Deliver
    MapFractionDeliverType FractionDeliver = MapFractionDeliverType::INVALID;
    // Bmessage string object
    std::string bMessageObject_ = "";
    // Bmessage Struct data
    IProfileBMessageStruct bMessageData_ {};
};
/**
 * @brief mce event report object
 */
class MceTypesEventReport {
public:
    /**
     * @brief  Build Object Data
     * @param  masInstanceId    masInstance Id
     * @param  stringObject     string Object
     * @return int
     */
    int BuildObjectData(const uint8_t &masInstanceId, const std::string &stringObject);
    /**
     * @brief Get the param object
     * @return IProfileMapEventReport
     */
    IProfileMapEventReport GetParam() const;
    /**
     * @brief Get the String Object object
     * @return std::string
     */
    std::string GetStringObject() const;

private:
    // event report
    IProfileMapEventReport eventReportParam_ {};
};
/**
 * @brief FolderListing Object
 */
class MceTypesFolderListing {
public:
    /**
     * @brief  Build Object Data
     * @param  stringObject     string Object
     * @return int
     */
    int BuildObjectData(const std::string &stringObject);
    /**
     * @brief Get the List object
     * @return std::vector<std::string>
     */
    std::vector<std::string> GetList() const;
    /**
     * @brief Get the Version object
     * @return std::string
     */
    std::string GetVersion() const;

private:
    // folder listing version
    std::string version_ = "";
    // folder name list
    std::vector<std::string> folderNameList_ {};
};
/**
 * @brief Utility of Convert Format
 */
class MceUtilityConvertFormat {
public:
    /**
     * @brief  Convert String To Message Type
     * @param  str              string object
     * @return MapMessageType
     */
    static MapMessageType ConvertStringToMessageType(const std::string &str);
    /**
     * @brief  Convert String To Message Direction
     * @param  str              string object
     * @return MapMsgDirection
     */
    static MapMsgDirection ConvertStringToMsgDirection(const std::string &str);
    /**
     * @brief  Convert String To Message Delivery Status
     * @param  str              string object
     * @return MapMsgDeliveryStatus
     */
    static MapMsgDeliveryStatus ConvertStringToMsgDeliveryStatus(const std::string &str);
    /**
     * @brief  Convert String To Message Reception Status
     * @param  str              string object
     * @return MapMsgReceptionStatus
     */
    static MapMsgReceptionStatus ConvertStringToMsgReceptionStatus(const std::string &str);
    /**
     * @brief  Convert Yes/No String To MapMessageStatus
     * @param  str              string object
     * @return MapMessageStatus
     */
    static MapMessageStatus ConvertYesNoStringToMapMessageStatus(const std::string &str);
    /**
     * @brief  Convert String To MessageStatus
     * @param  str              string object
     * @return MapMessageStatus
     */
    static MapMessageStatus ConvertStringToMapMessageStatus(const std::string &str);
    /**
     * @brief  Convert String To  MapBoolType
     * @param  str              string object
     * @return MapBoolType
     */
    static MapBoolType ConvertStringToMapBoolType(const std::string &str);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MCE_DATA_ANALYSE_H
