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
 * @addtogroup bluetooth
 * @{
 *
 * @brief
 *
 */

/**
 * @file map_mce_data_analyse.c
 *
 * @brief Defines map client service request data analyse object.
 *
 */
#include "map_mce_data_analyse.h"
#include <codecvt>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include "log.h"
#include "map_mce_xml.h"

namespace OHOS {
namespace bluetooth {
MceSplitString::MceSplitString(const std::string &target, const std::string &filter)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    targetString_ = target;
    filterString_ = filter;
    count_ = 0;
    maxCount_ = targetString_.size();
}

MceSplitString::~MceSplitString()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

std::string MceSplitString::NextWord()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int begin = count_;
    for (; count_ < maxCount_; count_++) {
        if (filterString_.find(targetString_[count_]) != std::string::npos) {
            if (begin == count_) {
                begin++;
            } else {
                break;
            }
        }
    }
    return targetString_.substr(begin, count_ - begin);
}

MceCombineNode::MceCombineNode()
{
    stream_.str("");
}

MceCombineNode::~MceCombineNode()
{
    stream_.str("");
}

void MceCombineNode::NodeBegin(std::string nodeName)
{
    nodeNameList_.push_back(nodeName);
    stream_ << "BEGIN:" << nodeName << "\r\n";
}

void MceCombineNode::NodeEnd()
{
    if (nodeNameList_.size() != 0) {
        std::string nodeName = nodeNameList_.back();
        stream_ << "END:" << nodeName << "\r\n";
        nodeNameList_.pop_back();
    }
}

void MceCombineNode::AddParamStr(const std::string &param, const std::string &value)
{
    stream_ << param << value << "\r\n";
}

void MceCombineNode::AddText(const std::string &text)
{
    stream_ << text << "\r\n";
}

std::string MceCombineNode::GetCombineString() const
{
    return stream_.str();
}

MceBMessageNode::MceBMessageNode()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceBMessageNode::MceBMessageNode(const std::list<std::string> &list, const std::string &name)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MceConstructNode(list, name);
}

void MceBMessageNode::MceConstructNode(const std::list<std::string> &list, const std::string &name)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    wordList_ = list;
    name_ = name;
}

MceBMessageNode::~MceBMessageNode()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

MceBMessageNode MceBMessageNode::GetNextChild()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::list<std::string> nextNode;
    std::string nextName = "";
    std::string begin;
    int beginNum;
    int beginLength = strlen("BEGIN:");
    int endLength = strlen("END:");

    while (wordList_.size() != 0) {
        begin = wordList_.front();
        wordList_.pop_front();

        if ((begin.size() >= static_cast<std::size_t>(beginLength)) &&
            (begin.substr(0, static_cast<std::size_t>(beginLength)) == "BEGIN:")) {
            nextName = begin.substr(beginLength);
            break;
        }
    }

    std::string end;
    beginNum = 0;
    MceBMessageNode node;
    if (nextName == "") {
        return node;
    }
    while (wordList_.size() != 0) {
        end = wordList_.front();
        wordList_.pop_front();
        if ((end.size() >= static_cast<std::size_t>(beginLength)) &&
            (end.substr(0, static_cast<std::size_t>(beginLength)) == "BEGIN:")) {
            beginNum++;
        }
        if ((end.size() >= static_cast<std::size_t>(endLength)) &&
            (end.substr(0, static_cast<std::size_t>(endLength)) == "END:")) {
            if (beginNum == 0) {
                node.MceConstructNode(nextNode, nextName);
                break;
            } else {
                beginNum--;
            }
        }
        nextNode.push_back(end);  // do not put "BEGIN:" or "END:" to the list
    }
    return node;
}

std::string MceBMessageNode::GetNodeName() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return name_;
}

std::string MceBMessageNode::GetNodeValue() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return value_;
}

std::string MceBMessageNode::GetParamValue(std::string param) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string value;
    std::string curWold;
    int beginNum;
    std::string beginEnd;
    int beginLength = strlen("BEGIN:");
    int endLength = strlen("END:");

    beginNum = 0;
    for (auto it = wordList_.begin(); it != wordList_.end(); it++) {
        curWold = *it;
        if ((beginEnd.size() >= (std::size_t)beginLength) &&
            (beginEnd.substr(0, (std::size_t)beginLength) == "BEGIN:")) {
            beginNum++;
        }
        if ((beginEnd.size() >= (std::size_t)endLength) && (beginEnd.substr(0, (std::size_t)endLength) == "END:")) {
            if (beginNum == 0) {
                break;
            } else {
                beginNum--;
            }
        }
        // not in the sub node
        if ((beginNum == 0) && (curWold.size() >= param.size()) && (param == curWold.substr(0, param.size()))) {
            value = curWold.substr(param.size());
            break;
        }
    }
    return value;
}

std::vector<std::string> MceBMessageNode::GetParamValueList(std::string param) const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string cWord;
    std::string beginEnd;
    std::string value;
    std::vector<std::string> retValue;
    int bNum = 0;
    int eLength = strlen("END:");
    int bLength = strlen("BEGIN:");
    for (auto it = wordList_.begin(); it != wordList_.end(); it++) {
        cWord = *it;
        if ((beginEnd.size() >= (std::size_t)bLength) && (beginEnd.substr(0, (std::size_t)bLength) == "BEGIN:")) {
            bNum++;
        }
        if ((beginEnd.size() >= (std::size_t)eLength) && (beginEnd.substr(0, (std::size_t)eLength) == "END:")) {
            if (bNum == 0) {
                break;
            } else {
                bNum--;
            }
        }
        // not in the sub node
        if ((bNum == 0) && (cWord.size() >= param.size()) && (param == cWord.substr(0, param.size()))) {
            value = cWord.substr(param.size());
            retValue.push_back(value);
        }
    }
    return retValue;
}

int MceBMessageNode::GetSize() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return wordList_.size();
}

MceBmessageParamMakeStringObject::MceBmessageParamMakeStringObject(const IProfileBMessageStruct &param)
    :msgParam_(param)
{
    combineNode_.NodeBegin("BMSG");
    MakeBMessageProperty();
    if (msgParam_.originator_.size() > 0) {
        MakeVcard(msgParam_.originator_);
    }
    if (param.envelope_.maxLevelOfEnvelope_ > 0) {
        MakeRecipientLevel1();
    }
    combineNode_.NodeEnd();  // BMSG
}

MceBmessageParamMakeStringObject::~MceBmessageParamMakeStringObject()
{}

std::string MceBmessageParamMakeStringObject::GetStringObject() const
{
    return combineNode_.GetCombineString();
}

void MceBmessageParamMakeStringObject::MakeBMessageProperty()
{
    combineNode_.AddParamStr("VERSION:", msgParam_.version_property);
    switch (msgParam_.readstatus_property) {
        case MapMessageStatus::READ:
            combineNode_.AddParamStr("STATUS:", "READ");
            break;
        case MapMessageStatus::UNREAD:
            combineNode_.AddParamStr("STATUS:", "UNREAD");
            break;
        default:
            combineNode_.AddParamStr("STATUS:", "");
            break;
    }
    switch (msgParam_.type_property) {
        case MapMessageType::EMAIL:
            combineNode_.AddParamStr("TYPE:", "EMAIL");
            break;
        case MapMessageType::SMS_GSM:
            combineNode_.AddParamStr("TYPE:", "SMS_GSM");
            break;
        case MapMessageType::SMS_CDMA:
            combineNode_.AddParamStr("TYPE:", "SMS_CDMA");
            break;
        case MapMessageType::MMS:
            combineNode_.AddParamStr("TYPE:", "MMS");
            break;
        case MapMessageType::IM:
            combineNode_.AddParamStr("TYPE:", "IM");
            break;
        default:
            combineNode_.AddParamStr("TYPE:", "");
            break;
    }

    if (msgParam_.folder_property != u"") {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        std::string utf8Handle = converter.to_bytes(msgParam_.folder_property);
        combineNode_.AddParamStr("FOLDER:", utf8Handle);
    }
    if ((msgParam_.version_property == "1.1") && (msgParam_.extendeddata_property != "")) {
        combineNode_.AddParamStr("EXTENDEDDATA:", msgParam_.extendeddata_property);  // extended data only V1.1
    }
}

void MceBmessageParamMakeStringObject::MakeRecipientLevel1()
{
    combineNode_.NodeBegin("BENV");
    if (msgParam_.envelope_.recipientLevel1_.size() != 0) {
        MakeVcard(msgParam_.envelope_.recipientLevel1_);
    }

    if (msgParam_.envelope_.maxLevelOfEnvelope_ > 1) {
        MakeRecipientLevel2();
    } else {
        MakeMsgBodyProperty();
    }
    combineNode_.NodeEnd();  // BENV
}

void MceBmessageParamMakeStringObject::MakeRecipientLevel2()
{
    combineNode_.NodeBegin("BENV");
    if (msgParam_.envelope_.recipientLevel2_.size() != 0) {
        MakeVcard(msgParam_.envelope_.recipientLevel2_);
    }
    if (msgParam_.envelope_.maxLevelOfEnvelope_ > MCE_RECIPIENT_LEVEL2) {
        MakeRecipientLevel3();
    } else {
        MakeMsgBodyProperty();
    }
    combineNode_.NodeEnd();  // BENV
}

void MceBmessageParamMakeStringObject::MakeRecipientLevel3()
{
    combineNode_.NodeBegin("BENV");
    if (msgParam_.envelope_.recipientLevel3_.size() != 0) {
        MakeVcard(msgParam_.envelope_.recipientLevel3_);
    }
    MakeMsgBodyProperty();
    combineNode_.NodeEnd();  // BENV
}

void MceBmessageParamMakeStringObject::MakeMsgBodyProperty()
{
    combineNode_.NodeBegin("BBODY");
    if (msgParam_.envelope_.msgBody_.bodyPartID != "") {
        combineNode_.AddParamStr("PARTID:", msgParam_.envelope_.msgBody_.bodyPartID);
    }
    if (msgParam_.envelope_.msgBody_.body_encoding != "") {
        combineNode_.AddParamStr("ENCODING:", msgParam_.envelope_.msgBody_.body_encoding);
    }
    if (msgParam_.envelope_.msgBody_.body_charset != "") {
        combineNode_.AddParamStr("CHARSET:", msgParam_.envelope_.msgBody_.body_charset);
    }
    if (msgParam_.envelope_.msgBody_.body_language != "") {
        combineNode_.AddParamStr("LANGUAGE:", msgParam_.envelope_.msgBody_.body_language);
    }
    combineNode_.AddParamStr("LENGTH:", std::to_string(msgParam_.envelope_.msgBody_.body_content_length));
    MakeMsgBodyText();
    combineNode_.NodeEnd();  // BBODY
}

void MceBmessageParamMakeStringObject::MakeMsgBodyText()
{
    combineNode_.NodeBegin("MSG");
    // <bmessage-body-content>
    combineNode_.AddText(msgParam_.envelope_.msgBody_.body_content);
    combineNode_.NodeEnd();  // MSG
}

void MceBmessageParamMakeStringObject::MakeVcard(std::vector<IProfileMapVcard> vcard)
{
    IProfileMapVcard data;
    for (auto it = vcard.begin(); it != vcard.end(); it++) {
        data = *it;
        combineNode_.NodeBegin("VCARD");
        combineNode_.AddParamStr("VERSION:", data.VERSION);
        if (data.N != "") {
            combineNode_.AddParamStr("N:", data.N);
        }
        if (data.VERSION == "3.0") {
            if (data.FN != "") {
                combineNode_.AddParamStr("FN:", data.FN);
            }
        }
        if (data.TEL.size() != 0) {
            for (auto itTel = data.TEL.begin(); itTel != data.TEL.end(); itTel++) {
                combineNode_.AddParamStr("TEL:", *itTel);
            }
        }
        if (data.EMAIL.size() != 0) {
            for (auto itEmail = data.EMAIL.begin(); itEmail != data.EMAIL.end(); itEmail++) {
                combineNode_.AddParamStr("EMAIL:", *itEmail);
            }
        }
        if ((msgParam_.version_property == "1.1") && (data.X_BT_UID.size() != 0)) {
            for (auto itUid = data.X_BT_UID.begin(); itUid != data.X_BT_UID.end(); itUid++) {
                combineNode_.AddParamStr("X-BT-UID:", *itUid);
            }
        }
        if ((msgParam_.version_property == "1.1") && (data.X_BT_UCI.size() != 0)) {
            for (auto itUci = data.X_BT_UCI.begin(); itUci != data.X_BT_UCI.end(); itUci++) {
                combineNode_.AddParamStr("X-BT-UCI:", *itUci);
            }
        }
        combineNode_.NodeEnd();
    }
}

MceBmessageParamAnalyser::MceBmessageParamAnalyser(const std::string &object)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    msgStr_ = object;
}

void MceBmessageParamAnalyser::StartAnalyse()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MceBMessageNode nodeLevel1;
    MceBMessageNode nodeLevel2;
    MceBMessageNode nodeLevel3;

    // init
    bMsgParamStruct_.originator_.clear();
    bMsgParamStruct_.envelope_.recipientLevel1_.clear();
    bMsgParamStruct_.envelope_.recipientLevel2_.clear();
    bMsgParamStruct_.envelope_.recipientLevel3_.clear();
    bMsgParamStruct_.envelope_.msgBody_.body_content = "";

    MakeWordList();
    MceBMessageNode rootNode(wordList_, "BMSG");
    PickUpBMessageProperty(rootNode);

    nodeLevel1 = rootNode.GetNextChild();
    while (int(nodeLevel1.GetSize()) != 0) {
        if (nodeLevel1.GetNodeName() == "VCARD") {
            PickUpBMessagePropertyOriginator(nodeLevel1);
        } else if (nodeLevel1.GetNodeName() == "BENV") {
            nodeLevel2 = PickUpRecipientLevel1(nodeLevel1);
        } else {
            // error
            LOG_ERROR("%{public}s BMessage format error!", __PRETTY_FUNCTION__);
        }
        nodeLevel1 = rootNode.GetNextChild();
    }

    if (nodeLevel2.GetSize() != 0) {
        nodeLevel3 = PickUpRecipientLevel2(nodeLevel2);
    }

    if (nodeLevel3.GetSize() != 0) {
        PickUpRecipientLevel3(nodeLevel3);
    }

    PickUpMsgBodyProperty();
    PickUpMsgBodyText();
}

void MceBmessageParamAnalyser::MakeWordList()
{
    std::string filter("\r\n");
    MceSplitString stringData(msgStr_, filter);
    std::string word = stringData.NextWord();

    wordList_.clear();

    // make word list
    if (word == "BEGIN:BMSG") {
        word = stringData.NextWord();
        while (word != "END:BMSG") {
            if (word != "") {
                wordList_.push_back(word);
                word = stringData.NextWord();
            } else {
                // format error clear list
                wordList_.clear();
                LOG_ERROR("%{public}s not find END:BMSG", __PRETTY_FUNCTION__);
                break;
            }
        }
    } else {
        LOG_ERROR("%{public}s not find BEGIN:BMSG", __PRETTY_FUNCTION__);
    }
}

MceBmessageParamAnalyser::~MceBmessageParamAnalyser()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
}

IProfileBMessageStruct MceBmessageParamAnalyser::GetMsgStruct() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return bMsgParamStruct_;
}

MceBMessageNode MceBmessageParamAnalyser::GetFirstChildNode()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MakeWordList();
    MceBMessageNode fistNode(wordList_, "BMSG");
    return fistNode;
}

int MceBmessageParamAnalyser::GetSize() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return wordList_.size();
}

std::string MceBmessageParamAnalyser::GetMsgText()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    PickUpMsgBodyText();
    return bMsgParamStruct_.envelope_.msgBody_.body_content;
}

void MceBmessageParamAnalyser::PickUpBMessageProperty(const MceBMessageNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    std::string tempStr;

    bMsgParamStruct_.version_property = node.GetParamValue("VERSION:");
    bMsgParamStruct_.readstatus_property =
        MceUtilityConvertFormat::ConvertStringToMapMessageStatus(node.GetParamValue("STATUS:"));
    bMsgParamStruct_.extendeddata_property = node.GetParamValue("EXTENDEDDATA:");
    // "FOLDER:"
    tempStr = node.GetParamValue("FOLDER:");
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    bMsgParamStruct_.folder_property = converter.from_bytes(tempStr);
    // "TYPE:"
    tempStr = node.GetParamValue("TYPE:");
    bMsgParamStruct_.type_property = MceUtilityConvertFormat::ConvertStringToMessageType(tempStr);
}

void MceBmessageParamAnalyser::PickUpBMessagePropertyOriginator(const MceBMessageNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    IProfileMapVcard vcard;
    vcard.VERSION = node.GetParamValue("VERSION:");        // shall be included
    vcard.N = node.GetParamValue("N:");                    // shall be included
    vcard.TEL = node.GetParamValueList("TEL:");            // may be used
    vcard.EMAIL = node.GetParamValueList("EMAIL:");        // may be used
    vcard.X_BT_UID = node.GetParamValueList("X-BT-UID:");  // bmsg V1.1
    vcard.X_BT_UCI = node.GetParamValueList("X-BT-UCI:");  // bmsg V1.1
    vcard.FN = node.GetParamValue("FN:");                  // vcard 3.0 , shall be included

    if (bMsgParamStruct_.originator_.size() == 0) {
        bMsgParamStruct_.originator_.push_back(vcard);
    } else {
        // error , only one originator_
        LOG_ERROR("%{public}s execute", __PRETTY_FUNCTION__);
    }
}

MceBMessageNode MceBmessageParamAnalyser::PickUpRecipientLevel1(MceBMessageNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MceBMessageNode nodeLevelXPlus1;
    MceBMessageNode nexLevelRecipient;
    IProfileMapVcard vcard1;

    if (node.GetNodeName() != "BENV") {
        return nexLevelRecipient;
    }

    bMsgParamStruct_.envelope_.maxLevelOfEnvelope_ = MCE_RECIPIENT_LEVEL1;
    nodeLevelXPlus1 = node.GetNextChild();
    while (int(nodeLevelXPlus1.GetSize()) != 0) {
        if (nodeLevelXPlus1.GetNodeName() == "VCARD") {
            vcard1.VERSION = nodeLevelXPlus1.GetParamValue("VERSION:");        // shall be included
            vcard1.N = nodeLevelXPlus1.GetParamValue("N:");                    // shall be included
            vcard1.TEL = nodeLevelXPlus1.GetParamValueList("TEL:");            // may be used
            vcard1.EMAIL = nodeLevelXPlus1.GetParamValueList("EMAIL:");        // may be used
            vcard1.X_BT_UID = nodeLevelXPlus1.GetParamValueList("X-BT-UID:");  // bmsg V1.1
            vcard1.X_BT_UCI = nodeLevelXPlus1.GetParamValueList("X-BT-UCI:");  // bmsg V1.1
            vcard1.FN = nodeLevelXPlus1.GetParamValue("FN:");                  // vcard 3.0 , shall be included
            bMsgParamStruct_.envelope_.recipientLevel1_.push_back(vcard1);
        }
        if ((nodeLevelXPlus1.GetNodeName() == "BBODY") && (msgNode_.GetSize() == 0)) {
            msgNode_ = nodeLevelXPlus1;
        }
        if ((nodeLevelXPlus1.GetNodeName() == "BENV") && (nexLevelRecipient.GetSize() == 0)) {
            nexLevelRecipient = nodeLevelXPlus1;
        }
        nodeLevelXPlus1 = node.GetNextChild();
    }
    return nexLevelRecipient;
}

MceBMessageNode MceBmessageParamAnalyser::PickUpRecipientLevel2(MceBMessageNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MceBMessageNode nodeLevelXPlus2;
    MceBMessageNode nexLevelRecipient;
    IProfileMapVcard vcard2;

    if (node.GetNodeName() != "BENV") {
        return nexLevelRecipient;
    }

    bMsgParamStruct_.envelope_.maxLevelOfEnvelope_ = MCE_RECIPIENT_LEVEL2;
    nodeLevelXPlus2 = node.GetNextChild();
    while (int(nodeLevelXPlus2.GetSize()) != 0) {
        if (nodeLevelXPlus2.GetNodeName() == "VCARD") {
            vcard2.VERSION = nodeLevelXPlus2.GetParamValue("VERSION:");        // shall be included
            vcard2.N = nodeLevelXPlus2.GetParamValue("N:");                    // shall be included
            vcard2.TEL = nodeLevelXPlus2.GetParamValueList("TEL:");            // may be used
            vcard2.EMAIL = nodeLevelXPlus2.GetParamValueList("EMAIL:");        // may be used
            vcard2.X_BT_UID = nodeLevelXPlus2.GetParamValueList("X-BT-UID:");  // bmsg V1.1
            vcard2.X_BT_UCI = nodeLevelXPlus2.GetParamValueList("X-BT-UCI:");  // bmsg V1.1
            vcard2.FN = nodeLevelXPlus2.GetParamValue("FN:");                  // vcard 3.0 , shall be included
            bMsgParamStruct_.envelope_.recipientLevel2_.push_back(vcard2);
        }
        if ((nodeLevelXPlus2.GetNodeName() == "BBODY") && (msgNode_.GetSize() == 0)) {
            msgNode_ = nodeLevelXPlus2;
        }
        if ((nodeLevelXPlus2.GetNodeName() == "BENV") && (nexLevelRecipient.GetSize() == 0)) {
            nexLevelRecipient = nodeLevelXPlus2;
        }
        nodeLevelXPlus2 = node.GetNextChild();
    }
    return nexLevelRecipient;
}

void MceBmessageParamAnalyser::PickUpRecipientLevel3(MceBMessageNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    MceBMessageNode nodeLevelXPlus3;
    IProfileMapVcard vcard3;

    if (node.GetNodeName() != "BENV") {
        return;
    }

    bMsgParamStruct_.envelope_.maxLevelOfEnvelope_ = MCE_RECIPIENT_LEVEL3;
    nodeLevelXPlus3 = node.GetNextChild();
    while (int(nodeLevelXPlus3.GetSize()) != 0) {
        if (nodeLevelXPlus3.GetNodeName() == "VCARD") {
            vcard3.VERSION = nodeLevelXPlus3.GetParamValue("VERSION:");        // shall be included
            vcard3.N = nodeLevelXPlus3.GetParamValue("N:");                    // shall be included
            vcard3.TEL = nodeLevelXPlus3.GetParamValueList("TEL:");            // may be used
            vcard3.EMAIL = nodeLevelXPlus3.GetParamValueList("EMAIL:");        // may be used
            vcard3.X_BT_UID = nodeLevelXPlus3.GetParamValueList("X-BT-UID:");  // bmsg V1.1
            vcard3.X_BT_UCI = nodeLevelXPlus3.GetParamValueList("X-BT-UCI:");  // bmsg V1.1
            vcard3.FN = nodeLevelXPlus3.GetParamValue("FN:");                  // vcard 3.0 , shall be included
            bMsgParamStruct_.envelope_.recipientLevel3_.push_back(vcard3);
        }
        if (nodeLevelXPlus3.GetNodeName() == "BBODY") {
            msgNode_ = nodeLevelXPlus3;
        }
        nodeLevelXPlus3 = node.GetNextChild();
    }
}

void MceBmessageParamAnalyser::PickUpMsgBodyProperty()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    if (msgNode_.GetSize() != 0) {
        bMsgParamStruct_.envelope_.msgBody_.bodyPartID = msgNode_.GetParamValue("PARTID:");
        bMsgParamStruct_.envelope_.msgBody_.body_encoding = msgNode_.GetParamValue("ENCODING:");
        bMsgParamStruct_.envelope_.msgBody_.body_charset = msgNode_.GetParamValue("CHARSET:");
        bMsgParamStruct_.envelope_.msgBody_.body_language = msgNode_.GetParamValue("LANGUAGE:");
        std::string tempStr = msgNode_.GetParamValue("LENGTH:");
        bMsgParamStruct_.envelope_.msgBody_.body_content_length = atoi(tempStr.c_str());
    }
}

void MceBmessageParamAnalyser::PickUpMsgBodyText()
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int next = 1;
    int msgBegin = 0;
    int msgEnd = 0;
    int endMsgLength = strlen("END:MSG");
    int beginMsgLength = strlen("BEGIN:MSG");

    // pickup the msg text
    for (int i = 0; i < int(msgStr_.size()); i++) {
        if (msgBegin == 0) {
            if (msgStr_.substr(i, beginMsgLength) == "BEGIN:MSG") {
                i = i + beginMsgLength + 1;
                msgBegin = i;
            }
            if ((msgBegin != 0) && ((msgStr_.substr(i, 1) == "\r") || (msgStr_.substr(i, 1) == "\n"))) {
                i++;
                msgBegin = i;
            }
            continue;
        }
        if (msgStr_.substr(i, endMsgLength) == "END:MSG") {
            next = 1;
            if ((msgStr_.substr(i - next - 1, 1) == "\r") || (msgStr_.substr(i - next - 1, 1) == "\n")) {
                next++;
            }
            msgEnd = i - next;
            if (msgEnd > msgBegin) {
                bMsgParamStruct_.envelope_.msgBody_.body_content = msgStr_.substr(msgBegin, msgEnd - msgBegin);
            } else {
                bMsgParamStruct_.envelope_.msgBody_.body_content = "";
            }
            break;
        }
    }
}

void MceTypesConversationListing::PickUpParticipant(IProfileParticipant &data, const MceXmlNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    data.uci = node.Attribute("uci").AsString();
    data.display_name = node.Attribute("display_name").AsString();
    data.chat_state = node.Attribute("chat_state").AsString();
    data.last_activity = node.Attribute("last_activity").AsString();
    data.x_bt_uid = node.Attribute("x_bt_uid").AsString();
    data.name = node.Attribute("name").AsString();
    data.presence_availability = node.Attribute("presence_availability").AsString();
    data.presence_text = node.Attribute("presence_text").AsString();
    data.priority = node.Attribute("priority").AsString();
}

void MceTypesConversationListing::PickUpConversation(IProfileConversation &data, const MceXmlNode &node)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    IProfileParticipant participants;

    data.id = node.Attribute("id").AsString();
    data.name = node.Attribute("name").AsString();
    data.last_activity = node.Attribute("last_activity").AsString();
    data.read_status = node.Attribute("read_status").AsString();
    data.version_counter = node.Attribute("version_counter").AsString();
    data.summary = node.Attribute("summary").AsString();

    for (MceXmlNode covNode = node.FirstChild(); !covNode.Empty(); covNode = covNode.NextSibling()) {
        std::string checkName(covNode.Name());
        if (checkName == "participant") {
            PickUpParticipant(participants, covNode);
            // push to vector list
            data.participantList_.push_back(participants);
        }
    }
}

int MceTypesConversationListing::BuildObjectData(
    const IProfileConversationListingParamStruct &stringParam, const std::string &stringObject)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int ret = RET_NO_ERROR;
    conversationListingParam_ = stringParam;
    conversationListingObject_ = stringObject;
    IProfileConversation converOutline;

    MceXmlDoc xmlDoc {};
    xmlDoc.LoadString(conversationListingObject_.c_str());
    MceXmlNode rootNode = xmlDoc.Child("MAP-convo-listing");

    // init
    conversationList_.clear();

    if (!rootNode.Empty()) {
        // version
        conversationListingParam_.Version = rootNode.Attribute("version").AsString();

        for (MceXmlNode covNode = rootNode.FirstChild(); !covNode.Empty(); covNode = covNode.NextSibling()) {
            std::string checkName(covNode.Name());
            if (checkName == "conversation") {
                converOutline.participantList_.clear();
                PickUpConversation(converOutline, covNode);
                // push to vector list
                conversationList_.push_back(converOutline);
            }
        }
    }
    return ret;
}

std::vector<IProfileConversation> MceTypesConversationListing::GetList() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return conversationList_;
}

IProfileConversationListingParamStruct MceTypesConversationListing::GetParam() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return conversationListingParam_;
}

std::string MceTypesConversationListing::GetStringObject() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return conversationListingObject_;
}

void MceTypesMessagesListing::PickupOutlineParam(IProfileMessageOutline &msgOutline, const MceXmlNode &msgNode)
{
    // Attribute
    msgOutline.handle = msgNode.Attribute("handle").AsString();
    msgOutline.subject = msgNode.Attribute("subject").AsString();
    msgOutline.datetime = msgNode.Attribute("datetime").AsString();
    msgOutline.sender_name = msgNode.Attribute("sender_name").AsString();
    msgOutline.sender_addressing = msgNode.Attribute("sender_addressing").AsString();
    msgOutline.replyto_addressing = msgNode.Attribute("replyto_addressing").AsString();
    msgOutline.recipient_name = msgNode.Attribute("recipient_name").AsString();
    msgOutline.recipient_addressing = msgNode.Attribute("recipient_addressing").AsString();

    // "Type"
    std::string tempStr = msgNode.Attribute("type").AsString();
    msgOutline.type = MceUtilityConvertFormat::ConvertStringToMessageType(tempStr);

    // "receptionStatus"
    tempStr = msgNode.Attribute("receptionStatus").AsString();
    msgOutline.receptionStatus = MceUtilityConvertFormat::ConvertStringToMsgReceptionStatus(tempStr);

    msgOutline.size = msgNode.Attribute("size").AsInt();
    msgOutline.attachment_size = msgNode.Attribute("attachment_size").AsInt();
    msgOutline.text = MceUtilityConvertFormat::ConvertStringToMapBoolType(msgNode.Attribute("text").AsString());
    msgOutline.read =
        MceUtilityConvertFormat::ConvertYesNoStringToMapMessageStatus(msgNode.Attribute("read").AsString());
    msgOutline.sent = MceUtilityConvertFormat::ConvertStringToMapBoolType(msgNode.Attribute("sent").AsString());
    msgOutline.protected_ =
        MceUtilityConvertFormat::ConvertStringToMapBoolType(msgNode.Attribute("protected").AsString());
    msgOutline.priority = MceUtilityConvertFormat::ConvertStringToMapBoolType(msgNode.Attribute("priority").AsString());
    // "delivery_status" V1.1
    tempStr = msgNode.Attribute("delivery_status").AsString();
    msgOutline.delivery_status = MceUtilityConvertFormat::ConvertStringToMsgDeliveryStatus(tempStr);

    msgOutline.conversation_id = msgNode.Attribute("conversation_id").AsString();
    msgOutline.conversation_name = msgNode.Attribute("conversation_name").AsString();

    // "direction"  V1.1
    tempStr = msgNode.Attribute("direction").AsString();
    msgOutline.direction = MceUtilityConvertFormat::ConvertStringToMsgDirection(tempStr);

    // mime types
    msgOutline.attachment_mime_types = msgNode.Attribute("attachment_mime_types").AsString();
}

int MceTypesMessagesListing::BuildObjectData(
    const IProfileMessagesListingParamStruct &stringParam, const std::string &stringObject)
{
    LOG_INFO("%{public}s enter", __PRETTY_FUNCTION__);
    int ret = RET_NO_ERROR;
    IProfileMessageOutline msgOutline;
    std::string tempStr;
    messagesListingParam_ = stringParam;
    MessagesListingObject_ = stringObject;
    MceXmlDoc xmlDoc {};
    xmlDoc.LoadString(MessagesListingObject_.c_str());
    MceXmlNode rootNode = xmlDoc.Child("MAP-msg-listing");
    if (!rootNode.Empty()) {
        messagesListingParam_.Version = rootNode.Attribute("version").AsString();  // version
        for (MceXmlNode msgNode = rootNode.FirstChild(); !msgNode.Empty(); msgNode = msgNode.NextSibling()) {
            PickupOutlineParam(msgOutline, msgNode);
            // push to vector list
            messageList_.push_back(msgOutline);
        }
    }
    LOG_INFO("%{public}s end, msglist size = %{public}d", __PRETTY_FUNCTION__, int(messageList_.size()));
    return ret;
}

std::vector<IProfileMessageOutline> MceTypesMessagesListing::GetList() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return messageList_;
}

IProfileMessagesListingParamStruct MceTypesMessagesListing::GetParam() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return messagesListingParam_;
}

std::string MceTypesMessagesListing::GetStringObject() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return MessagesListingObject_;
}

int MceTypesBMessage::BuildObjectData(const std::string &stringObject)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    int ret = RET_NO_ERROR;
    bMessageObject_ = stringObject;
    MceBmessageParamAnalyser analyser(stringObject);
    analyser.StartAnalyse();
    bMessageData_ = analyser.GetMsgStruct();
    return ret;
}

int MceTypesBMessage::SetFractionDeliver(MapFractionDeliverType value)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    FractionDeliver = value;
    return RET_NO_ERROR;
}

MapFractionDeliverType MceTypesBMessage::GetFractionDeliver() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return FractionDeliver;
}

IProfileBMessageStruct MceTypesBMessage::GetBMessageData() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return bMessageData_;
}

std::string MceTypesBMessage::GetBmessageObject() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return bMessageObject_;
}

int MceTypesEventReport::BuildObjectData(const uint8_t &masInstanceId, const std::string &stringObject)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    eventReportParam_.eventReportStringObject_ = stringObject;
    eventReportParam_.masInstanceId_ = masInstanceId;

    MceXmlDoc xmlDoc {};
    xmlDoc.LoadString(eventReportParam_.eventReportStringObject_.c_str());
    MceXmlNode rootNode = xmlDoc.Child("MAP-event-report");
    if (!rootNode.Empty()) {
        eventReportParam_.version = rootNode.Attribute("version").AsString();  // version
        MceXmlNode eventNode = rootNode.FirstChild();
        if (!eventNode.Empty()) {
            if (eventNode.Name() == "event") {
                std::string tempStr;
                eventReportParam_.type = eventNode.Attribute("type").AsString();
                eventReportParam_.handle = eventNode.Attribute("handle").AsString();
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
                eventReportParam_.folder = converter.from_bytes(eventNode.Attribute("folder").AsString());
                eventReportParam_.old_folder = converter.from_bytes(eventNode.Attribute("old_folder").AsString());
                // "Type"
                tempStr = eventNode.Attribute("msg_type").AsString();
                eventReportParam_.msg_type = MceUtilityConvertFormat::ConvertStringToMessageType(tempStr);
                eventReportParam_.datetime = eventNode.Attribute("datetime").AsString();
                eventReportParam_.subject = eventNode.Attribute("subject").AsString();
                eventReportParam_.sender_name = eventNode.Attribute("sender_name").AsString();
                // Value "yes" is high priority; "no" is not of high priority.
                eventReportParam_.priority =
                    MceUtilityConvertFormat::ConvertStringToMapBoolType(eventNode.Attribute("priority").AsString());
                eventReportParam_.conversation_name = eventNode.Attribute("conversation_name").AsString();
                eventReportParam_.conversation_id = eventNode.Attribute("conversation_id").AsString();
                eventReportParam_.presence_availability = eventNode.Attribute("presence_availability").AsString();
                eventReportParam_.presence_text = eventNode.Attribute("presence_text").AsString();
                eventReportParam_.last_activity = eventNode.Attribute("last_activity").AsString();
                eventReportParam_.chat_state = eventNode.Attribute("chat_state").AsString();
                // Shall be used only if the event “type” is “NewMessage” or “ReadStatusChanged”.
                eventReportParam_.read_status = MceUtilityConvertFormat::ConvertYesNoStringToMapMessageStatus(
                    eventNode.Attribute("read_status").AsString());
                // be used only if the event “type” is “MessageExtendedDataChanged”.
                eventReportParam_.extended_data = eventNode.Attribute("extended_data").AsString();
                eventReportParam_.participant_uci = eventNode.Attribute("participant_uci").AsString();
                eventReportParam_.contact_uid = eventNode.Attribute("contact_uid").AsString();
            }
        }
    }
    return RET_NO_ERROR;
}

IProfileMapEventReport MceTypesEventReport::GetParam() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return eventReportParam_;
}

std::string MceTypesEventReport::GetStringObject() const
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);
    return eventReportParam_.eventReportStringObject_;
}

int MceTypesFolderListing::BuildObjectData(const std::string &stringObject)
{
    LOG_INFO("%{public}s execute", __PRETTY_FUNCTION__);

    MceXmlDoc xmlDoc {};
    xmlDoc.LoadString(stringObject.c_str());
    MceXmlNode rootNode = xmlDoc.Child("folder-listing");
    if (!rootNode.Empty()) {
        version_ = rootNode.Attribute("version").AsString();  // version
        std::string folderName;
        for (MceXmlNode msgNode = rootNode.FirstChild(); !msgNode.Empty(); msgNode = msgNode.NextSibling()) {
            if (msgNode.Name() == "folder") {
                folderName = msgNode.Attribute("name").AsString();
                // push to vector list
                folderNameList_.push_back(folderName);
            }
        }
    }

    return RET_NO_ERROR;
}

std::vector<std::string> MceTypesFolderListing::GetList() const
{
    return folderNameList_;
}

std::string MceTypesFolderListing::GetVersion() const
{
    return version_;
}

MapMessageType MceUtilityConvertFormat::ConvertStringToMessageType(const std::string &str)
{
    MapMessageType type;

    if (str == "SMS_GSM") {
        type = MapMessageType::SMS_GSM;
    } else if (str == "SMS_CDMA") {
        type = MapMessageType::SMS_CDMA;
    } else if (str == "MMS") {
        type = MapMessageType::MMS;
    } else if (str == "EMAIL") {
        type = MapMessageType::EMAIL;
    } else if (str == "IM") {
        type = MapMessageType::IM;
    } else {
        // error
        type = MapMessageType::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapMsgDirection MceUtilityConvertFormat::ConvertStringToMsgDirection(const std::string &str)
{
    MapMsgDirection type;

    if (str == "incoming") {
        type = MapMsgDirection::INCOMING;
    } else if (str == "outgoing") {
        type = MapMsgDirection::OUTGOING;
    } else if (str == "outgoingdraft") {
        type = MapMsgDirection::OUTGOINGDRAFT;
    } else if (str == "outgoingpending") {
        type = MapMsgDirection::OUTGOINGPENDING;
    } else {
        // error
        type = MapMsgDirection::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapMsgDeliveryStatus MceUtilityConvertFormat::ConvertStringToMsgDeliveryStatus(const std::string &str)
{
    MapMsgDeliveryStatus type;
    if (str == "delivered") {
        type = MapMsgDeliveryStatus::DELIVERED;
    } else if (str == "sent") {
        type = MapMsgDeliveryStatus::SENT;
    } else if (str == "unknown") {
        type = MapMsgDeliveryStatus::UNKNOWN;
    } else {
        // error
        type = MapMsgDeliveryStatus::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapMsgReceptionStatus MceUtilityConvertFormat::ConvertStringToMsgReceptionStatus(const std::string &str)
{
    MapMsgReceptionStatus type;

    if (str == "complete") {
        type = MapMsgReceptionStatus::COMPLETE;
    } else if (str == "fractioned") {
        type = MapMsgReceptionStatus::FRACTIONED;
    } else if (str == "notification") {
        type = MapMsgReceptionStatus::NOTIFICATION;
    } else {
        // error
        type = MapMsgReceptionStatus::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapMessageStatus MceUtilityConvertFormat::ConvertYesNoStringToMapMessageStatus(const std::string &str)
{
    MapMessageStatus type;
    if (str == "yes") {
        type = MapMessageStatus::READ;
    } else if (str == "no") {
        type = MapMessageStatus::UNREAD;
    } else {
        // error
        type = MapMessageStatus::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapMessageStatus MceUtilityConvertFormat::ConvertStringToMapMessageStatus(const std::string &str)
{
    MapMessageStatus type;
    if (str == "READ") {
        type = MapMessageStatus::READ;
    } else if (str == "UNREAD") {
        type = MapMessageStatus::UNREAD;
    } else {
        // error
        type = MapMessageStatus::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}

MapBoolType MceUtilityConvertFormat::ConvertStringToMapBoolType(const std::string &str)
{
    MapBoolType type;
    if (str == "yes") {
        type = MapBoolType::YES;
    } else if (str == "no") {
        type = MapBoolType::NO;
    } else {
        // error
        type = MapBoolType::INVALID;
        LOG_INFO("%{public}s INVALID", __PRETTY_FUNCTION__);
    }
    return type;
}
}  // namespace bluetooth
}  // namespace OHOS
