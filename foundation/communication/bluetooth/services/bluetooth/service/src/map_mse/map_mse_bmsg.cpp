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

#include "map_mse_bmsg.h"
using std::string;
using std::vector;

namespace OHOS {
namespace bluetooth {
const std::string MapMseBmsg::MAP_MSE_BMESSAGE_VERSION = "VERSION:";

MapMseBmsg::MapMseBmsg()
{}

MapMseBmsg::~MapMseBmsg()
{}

std::string MapMseBmsg::GetVersion(void) const
{
    return version_;
}

std::string MapMseBmsg::GetStatus(void) const
{
    return status_;
}

std::string MapMseBmsg::GetExtendedData(void) const
{
    return extendedData_;
}

void MapMseBmsg::SetExtendedData(const std::string &exdData)
{
    extendedData_ = exdData;
}

std::string MapMseBmsg::GetMsgBody(void) const
{
    return msgData_;
}

void MapMseBmsg::SetMsgBody(const std::string &body)
{
    msgData_ = body;
}

std::string MapMseBmsg::GetEncoding(void) const
{
    return encoding_;
}

void MapMseBmsg::SetEncoding(const std::string &encoding)
{
    encoding_ = encoding;
}

std::string MapMseBmsg::GetCharset(void) const
{
    return charset_;
}

void MapMseBmsg::SetCharset(const std::string &charset)
{
    charset_ = charset;
}

std::string MapMseBmsg::GetLanguage(void) const
{
    return language_;
}

void MapMseBmsg::SetLanguage(const std::string &language)
{
    language_ = language;
}

std::string MapMseBmsg::GetBmsgLength(void) const
{
    return std::to_string(bmsgLength_);
}

void MapMseBmsg::SetBmsgLength(const int length)
{
    bmsgLength_ = length;
}

void MapMseBmsg::SetVersion(const std::string &version)
{
    version_ = version;
}

void MapMseBmsg::SetMessageType(const std::string &typeName)
{
    if (MAP_MSE_TYPE_EMAIL == typeName) {
        msgType_ = MessageType::EMAIL;
    } else if (MAP_MSE_TYPE_IM == typeName) {
        msgType_ = MessageType::IM;
    } else if (MAP_MSE_TYPE_MMS == typeName) {
        msgType_ = MessageType::MMS;
    } else if (MAP_MSE_TYPE_SMS_CDMA == typeName) {
        msgType_ = MessageType::SMS_CDMA;
    } else if (MAP_MSE_TYPE_SMS_GSM == typeName) {
        msgType_ = MessageType::SMS_GSM;
    }
}

int MapMseBmsg::Init(std::string body, const uint8_t charset)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    vector<string> bodyList;
    if (body.find("\r\n") != string::npos) {
        Split(body, "\r\n", bodyList);
    } else {
        Split(body, "\n", bodyList);
    }
    size_t pos = 0;
    if (ParseCheck(bodyList, pos, charset) != RET_NO_ERROR) {
        return RET_BAD_STATUS;
    }
    ParseOrgEnve(bodyList, pos);
    ParseBody(bodyList, pos);
    return RET_NO_ERROR;
}

int MapMseBmsg::ParseCheck(const std::vector<std::string> &src, size_t &pos, const uint8_t charset)
{
    if (src.size() == pos) {
        return RET_BAD_STATUS;
    }
    for (; pos < src.size(); pos++) {
        if (src[pos].find("BEGIN:BMSG") != string::npos || src[pos].find("VERSION:") != string::npos)
            continue;
        if (src[pos].find("BEGIN:VCARD") != string::npos || src[pos].find("BEGIN:BENV") != string::npos) {
            break;
        }
        if (ParseProperty(src, pos)) {
            continue;
        }
    }
    if (static_cast<uint8_t>(Charset::NATIVE) == charset && MessageType::SMS_CDMA != msgType_ &&
        MessageType::SMS_GSM != msgType_) {
        MSE_LOG_ERROR("Native appParamsCharset only supported for SMS.");
        return RET_BAD_STATUS;
    }
    if (status_.size() == 0) {
        MSE_LOG_ERROR("Missing value for 'STATUS'.");
        return RET_BAD_STATUS;
    }
    return RET_NO_ERROR;
}

bool MapMseBmsg::ParseProperty(const std::vector<std::string> &src, size_t &pos)
{
    bool hath = false;
    if (auto temp = GetNodeValue(src[pos], "STATUS:"); temp.size() > 0) {
        status_ = temp;
        hath = true;
    }
    if (auto temp = GetNodeValue(src[pos], "TYPE:"); temp.size() > 0) {
        SetMessageType(temp);
        hath = true;
    }
    if (auto temp = GetNodeValue(src[pos], "FOLDER:"); temp.size() > 0) {
        folder_ = temp;
        hath = true;
    }
    if (auto temp = GetNodeValue(src[pos], "EXTENDEDDATA:"); temp.size() > 0) {
        extendedData_ = temp;
        hath = true;
    }
    return hath;
}

void MapMseBmsg::ParseOrgEnve(const std::vector<std::string> &src, size_t &pos)
{
    for (; pos < src.size(); pos++) {
        if (src[pos].find("BEGIN:VCARD") != string::npos) {
            ++pos;
            originator_.push_back(ParseVCard(src, pos, 0));
            if (src[pos].find("END:VCARD") != string::npos)
                continue;
        }
        if (src[pos].find("BEGIN:BENV") != string::npos)
            break;
    }
    for (; pos < src.size(); pos++) {
        if (src[pos].find("BEGIN:BENV") != string::npos) {
            ++pos;
            ParseEnvelope(src, pos, 0);
        }
        if (src[pos].find("BEGIN:BBODY") != string::npos)
            break;
    }
}

void MapMseBmsg::ParseBody(const std::vector<std::string> &src, size_t &pos)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);
    while (src[pos].find("END:") == string::npos) {
        if (auto temp = GetNodeValue(src[pos], "PARTID:"); temp.size() > 0) {
            partId_ = std::stoi(temp);
            pos++;
            continue;
        }
        if (auto temp = GetNodeValue(src[pos], "ENCODING:"); temp.size() > 0) {
            encoding_ = temp;
            pos++;
            continue;
        }
        if (auto temp = GetNodeValue(src[pos], "CHARSET:"); temp.size() > 0) {
            charset_ = temp;
            pos++;
            continue;
        }
        if (auto temp = GetNodeValue(src[pos], "LANGUAGE:"); temp.size() > 0) {
            language_ = temp;
            pos++;
            continue;
        }
        if (auto temp = GetNodeValue(src[pos], "LENGTH:"); temp.size() > 0) {
            bmsgLength_ = std::stoi(temp);
            pos++;
            continue;
        }
        if (src[pos].find("BEGIN:MSG") != string::npos) {
            msgData_.append(src[pos]);
            msgData_.append("\n");
            pos++;
            while (src[pos].find("END:MSG") == string::npos) {
                msgData_.append(src[pos]);
                msgData_.append("\n");
                pos++;
            }
            msgData_.append("/");
            msgData_.append(src[pos]);
        }
        pos++;
    }
    MSE_LOG_INFO("Msg Data: %{public}s ", msgData_.c_str());
}

void MapMseBmsg::ParseEnvelope(const std::vector<std::string> &src, size_t &pos, int level)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    for (; pos < src.size(); pos++) {
        if (src[pos].find("BEGIN:VCARD") != string::npos) {
            ++pos;
            recipient_.push_back(ParseVCard(src, pos, level));
            if (src[pos].find("END:VCARD") != string::npos) {
                continue;
            }
        }
        if (src[pos].find("BEGIN:BENV") != string::npos) {
            ++pos;
            ParseEnvelope(src, pos, ++level);
        }
        if (src[pos].find("BEGIN:BBODY") != string::npos) {
            break;
        }
    }
}

MapMseVcard MapMseBmsg::ParseVCard(const std::vector<std::string> &src, size_t &pos, int level) const
{
    string vcardVersion, name, formattedName;
    vector<string> phoneNumbers, emailAddresses, btUids, btUcis;
    for (; pos < src.size(); pos++) {
        if (src[pos].find("END:VCARD") == string::npos) {
            if (src[pos].find("BEGIN:BENV") != string::npos || src[pos].find("EGIN:VCARD") != string::npos) {
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "VERSION:"); tempStr.size() > 0) {
                vcardVersion = tempStr == "3.0" ? "3.0" : "2.1";
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "FN:"); tempStr.size() > 0) {
                formattedName = tempStr;
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "N:"); tempStr.size() > 0) {
                name = tempStr;
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "TEL:"); tempStr.size() > 0) {
                Split(tempStr, ";", phoneNumbers);
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "EMAIL:"); tempStr.size() > 0) {
                Split(tempStr, ";", emailAddresses);
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "X-BT-UID:"); tempStr.size() > 0) {
                Split(tempStr, ";", btUids);
                continue;
            }
            if (string tempStr = GetNodeValue(src[pos], "X-BT-UCI:"); tempStr.size() > 0) {
                Split(tempStr, ";", btUcis);
                continue;
            }
        } else {
            break;
        }
    }
    if (version_ == MAP_V11) {
        MseVcard3 vcard3 {
            name, vcardVersion, formattedName, phoneNumbers, emailAddresses, btUids, btUcis, level
        };
        return MapMseVcard(vcard3);
    }
    MseVcard2 vcard2 {name, vcardVersion, formattedName, phoneNumbers, emailAddresses, level};
    return MapMseVcard(vcard2);
}

std::string MapMseBmsg::GetNodeValue(const std::string &src, std::string name)
{
    MSE_LOG_INFO("%{public}s Enter", __PRETTY_FUNCTION__);

    string str = src;
    std::string ret;
    if (str.find(name) != string::npos) {
        string::size_type start = str.find_first_of(':', 0);
        if (start != string::npos) {
            ret = str.substr(start + 1);
        }
    }
    MSE_LOG_INFO("Node value: %{public}s ", ret.c_str());
    return ret;
}

void MapMseBmsg::Split(const std::string &src, const std::string &separator, std::vector<std::string> &dest)
{
    string str = src;
    string substring;
    string::size_type start = 0;
    dest.clear();
    string::size_type index = str.find_first_of(separator, start);
    do {
        if (index != string::npos) {
            substring = str.substr(start, index - start);
            dest.push_back(substring);
            start = index + separator.size();
            index = str.find(separator, start);
            if (start == string::npos) {
                break;
            }
        }
    } while (index != string::npos);

    substring = str.substr(start);
    dest.push_back(substring);
}

void MapMseBmsg::AddBbody(std::string &bmessage)
{
    bmessage.append("BEGIN:BBODY");
    bmessage.append("\n");
    if (partId_ != -1) {
        bmessage.append("PARTID:" + std::to_string(partId_));
        bmessage.append("\n");
    }
    if (encoding_ != "") {
        bmessage.append("ENCODING:" + encoding_);
        bmessage.append("\n");
    }
    if (charset_ != "") {
        bmessage.append("CHARSET:" + charset_);
        bmessage.append("\n");
    }
    if (language_ != "") {
        bmessage.append("LANGUAGE:" + language_);
        bmessage.append("\n");
    }
    if (bmsgLength_ == -1) {
        bmsgLength_ = msgData_.size();
    }
    bmessage.append("LENGTH:" + std::to_string(bmsgLength_));
    bmessage.append("\n");
    bmessage.append(msgData_);
    bmessage.append("\n");
    bmessage.append("END:BBODY");
    bmessage.append("\n");
}

void MapMseBmsg::AddRecipient(std::string &bmessage)
{
    for (auto &iter : recipient_) {
        if (iter.GetEnvLevel() == 0) {
            bmessage.append(iter.ToVcardString(version_));
            bmessage.append("\n");
        }
    }
    std::string benv1 = "";
    for (auto &iter : recipient_) {
        if (iter.GetEnvLevel() == 1) {
            benv1.append(iter.ToVcardString(version_));
            benv1.append("\n");
        }
    }
    if (benv1 != "") {
        bmessage.append("BEGIN:BENV");
        bmessage.append("\n");
        bmessage.append(benv1);
    }
    std::string benv2 = "";
    for (auto &iter : recipient_) {
        if (iter.GetEnvLevel() == 0x2) {
            benv2.append(iter.ToVcardString(version_));
            benv2.append("\n");
        }
    }
    if (benv2 != "") {
        bmessage.append("BEGIN:BENV");
        bmessage.append("\n");
        bmessage.append(benv2);
    }
    AddBbody(bmessage);
    if (benv2 != "") {
        bmessage.append("END:BENV");
        bmessage.append("\n");
    }
    if (benv1 != "") {
        bmessage.append("END:BENV");
        bmessage.append("\n");
    }
}

std::string MapMseBmsg::ToBmessageString(void)
{
    std::string bmessage;
    bmessage.append("BEGIN:BMSG");
    bmessage.append("\n");
    bmessage.append("VERSION:" + version_);
    bmessage.append("\n");
    bmessage.append("STATUS:" + status_);
    bmessage.append("\n");
    bmessage.append("TYPE:" + msgTypeName_);
    bmessage.append("\n");
    bmessage.append("FOLDER:" + folder_);
    bmessage.append("\n");
    if (version_ == "1.1") {
        bmessage.append("EXTENDEDDATA:" + extendedData_);
        bmessage.append("\n");
    }
    for (auto &iter : originator_) {
        bmessage.append(iter.ToVcardString(version_));
        bmessage.append("\n");
    }
    bmessage.append("BEGIN:BENV");
    bmessage.append("\n");
    AddRecipient(bmessage);
    bmessage.append("END:BENV");
    bmessage.append("\n");
    bmessage.append("END:BMSG");
    bmessage.append("\n");
    return bmessage;
}

void MapMseBmsg::SetStatus(const std::string &status)
{
    status_ = status;
}

void MapMseBmsg::SetType(const MessageType &type)
{
    msgType_ = type;
}

MessageType MapMseBmsg::GetType(void) const
{
    return msgType_;
}

void MapMseBmsg::SetMsgTypeName(const std::string &name)
{
    msgTypeName_ = name;
}

void MapMseBmsg::SetPartId(const int id)
{
    partId_ = id;
}

std::string MapMseBmsg::GetPartId(void) const
{
    return std::to_string(partId_);
}

std::string MapMseBmsg::GetFolder(void) const
{
    return folder_;
}

void MapMseBmsg::SetFolder(const std::string &folder)
{
    folder_ = folder;
}

std::vector<MapMseVcard> MapMseBmsg::GetOriginators(void) const
{
    return originator_;
}

void MapMseBmsg::SetOriginators(const std::vector<MapMseVcard> &originator)
{
    originator_ = originator;
}

std::vector<MapMseVcard> MapMseBmsg::GetRecipients(void) const
{
    return recipient_;
}

void MapMseBmsg::SetRecipients(const std::vector<MapMseVcard> &recipient)
{
    recipient_ = recipient;
}
}  // namespace bluetooth
}  // namespace OHOS
