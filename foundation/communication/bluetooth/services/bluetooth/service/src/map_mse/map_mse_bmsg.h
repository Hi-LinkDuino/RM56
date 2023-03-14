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

#ifndef MAP_MSE_BMSG_H
#define MAP_MSE_BMSG_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "bt_def.h"
#include "log.h"
#include "map_mse_types.h"
#include "map_mse_vcard.h"

namespace OHOS {
namespace bluetooth {
class MapMseBmsg {
public:
    explicit MapMseBmsg();
    virtual ~MapMseBmsg();
    int Init(std::string body, const uint8_t charset);
    MessageType GetType(void) const;
    void SetMsgTypeName(const std::string &name);
    std::string GetPartId(void) const;
    void SetPartId(const int id);
    std::string GetVersion(void) const;
    void SetVersion(const std::string &version);
    std::string GetExtendedData(void) const;
    void SetExtendedData(const std::string &exdData);
    std::string GetStatus(void) const;
    std::string GetMsgBody(void) const;
    void SetMsgBody(const std::string &body);
    std::string GetEncoding(void) const;
    void SetEncoding(const std::string &encoding);
    std::string GetCharset(void) const;
    void SetCharset(const std::string &charset);
    std::string GetLanguage(void) const;
    void SetLanguage(const std::string &language);
    std::string GetBmsgLength(void) const;
    void SetBmsgLength(const int length);
    void SetStatus(const std::string &status);
    void SetType(const MessageType &type);
    std::vector<MapMseVcard> GetOriginators(void) const;
    void SetOriginators(const std::vector<MapMseVcard> &originator);
    std::vector<MapMseVcard> GetRecipients(void) const;
    void SetRecipients(const std::vector<MapMseVcard> &recipient);
    std::string GetFolder(void) const;
    void SetFolder(const std::string &folder);
    std::string ToBmessageString(void);

private:
    static const std::string MAP_MSE_BMESSAGE_VERSION;
    /* BMSG attributes */
    std::string version_ = "";
    std::string status_ = "";
    MessageType msgType_ = MessageType::NONE;
    std::string msgTypeName_ = "";
    std::string folder_ = "";
    std::string extendedData_ = "";
    /* BBODY attributes */
    int partId_ = -1;
    std::string encoding_ = "";
    std::string charset_ = "";
    std::string language_ = "";
    int bmsgLength_ = -1;
    std::vector<MapMseVcard> originator_ {};
    std::vector<MapMseVcard> recipient_ {};
    std::string msgData_ = "";
    int ParseCheck(const std::vector<std::string> &src, size_t &pos, const uint8_t charset);
    bool ParseProperty(const std::vector<std::string> &src, size_t &pos);
    void ParseEnvelope(const std::vector<std::string> &src, size_t &pos, int level);
    void ParseOrgEnve(const std::vector<std::string> &src, size_t &pos);
    void ParseBody(const std::vector<std::string> &src, size_t &pos);
    MapMseVcard ParseVCard(const std::vector<std::string> &src, size_t &pos, int level) const;
    static void Split(const std::string &src, const std::string &separator, std::vector<std::string> &dest);
    static std::string GetNodeValue(const std::string &src, std::string name);
    void SetMessageType(const std::string &typeName);
    void AddBbody(std::string &bmessage);
    void AddRecipient(std::string &bmessage);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_BMSG_H