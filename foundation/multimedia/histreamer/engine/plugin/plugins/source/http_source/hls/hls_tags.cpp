/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "hls_tags.h"
#include <cstring>
#include <sstream>
#include <stack>
#include <utility>

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
struct {
    const char* name_;
    HlsTag type_;
} const exttagmapping[] = {
    {"EXT-X-BYTERANGE",              HlsTag::EXTXBYTERANGE},
    {"EXT-X-DISCONTINUITY",          HlsTag::EXTXDISCONTINUITY},
    {"EXT-X-KEY",                    HlsTag::EXTXKEY},
    {"EXT-X-MAP",                    HlsTag::EXTXMAP},
    {"EXT-X-VERSION",                HlsTag::EXTXVERSION},
    {"EXT-X-PROGRAM-DATE-TIME",      HlsTag::EXTXPROGRAMDATETIME},
    {"EXT-X-TARGETDURATION",         HlsTag::EXTXTARGETDURATION},
    {"EXT-X-MEDIA-SEQUENCE",         HlsTag::EXTXMEDIASEQUENCE},
    {"EXT-X-DISCONTINUITY-SEQUENCE", HlsTag::EXTXDISCONTINUITYSEQUENCE},
    {"EXT-X-ENDLIST",                HlsTag::EXTXENDLIST},
    {"EXT-X-PLAYLIST-TYPE",          HlsTag::EXTXPLAYLISTTYPE},
    {"EXT-X-I-FRAMES-ONLY",          HlsTag::EXTXIFRAMESONLY},
    {"EXT-X-MEDIA",                  HlsTag::EXTXMEDIA},
    {"EXT-X-START",                  HlsTag::EXTXSTART},
    {"EXT-X-STREAM-INF",             HlsTag::EXTXSTREAMINF},
    {"EXT-X-I-FRAME-STREAM-INF",     HlsTag::EXTXIFRAMESTREAMINF},
    {"EXT-X-SESSION-KEY",            HlsTag::EXTXSESSIONKEY},
    {"EXTINF",                       HlsTag::EXTINF},
    {"",                             HlsTag::URI}
};

Attribute::Attribute(std::string name, std::string value)
    : name_(std::move(name)), value_(std::move(value))
{
}

uint64_t Attribute::Decimal() const
{
    std::istringstream is(value_);
    is.imbue(std::locale("C"));
    uint64_t ret;
    is >> ret;
    return ret;
}

double Attribute::FloatingPoint() const
{
    std::istringstream is(value_);
    is.imbue(std::locale("C"));
    double ret;
    is >> ret;
    return ret;
}

std::vector<uint8_t> Attribute::HexSequence() const
{
    std::vector<uint8_t> ret;
    if (value_.length() > 2 && (value_.substr(0, 2) == "0X" || value_.substr(0, 2) == "0x")) { // 2
        for (size_t i = 2; i <= (value_.length() - 2); i += 2) { // 2
            unsigned val;
            std::stringstream ss(value_.substr(i, 2));  // 2
            ss.imbue(std::locale("C"));
            ss >> std::hex >> val;
            ret.push_back(val);
        }
    }
    return ret;
}

std::pair<std::size_t, std::size_t> Attribute::GetByteRange() const
{
    std::size_t length = 0;
    std::size_t offset = 0;
    std::istringstream is(value_);
    is.imbue(std::locale("C"));
    if (!is.eof()) {
        is >> length;
        if (!is.eof()) {
            char c = static_cast<char>(is.get());
            if (c == '@' && !is.eof()) {
                is >> offset;
            }
        }
    }
    return std::make_pair(offset, length);
}

std::pair<int, int> Attribute::GetResolution() const
{
    int w = 0, h = 0;
    std::istringstream is(value_);
    is.imbue(std::locale("C"));
    if (!is.eof()) {
        is >> w;
        if (!is.eof()) {
            char c = is.get();
            if (c == 'x' && !is.eof()) {
                is >> h;
            }
        }
    }
    return std::make_pair(w, h);
}
std::string Attribute::GetName() const
{
    return name_;
}

Attribute Attribute::UnescapeQuotes() const
{
    return {name_, QuotedString()};
}

std::string Attribute::QuotedString() const
{
    if (!value_.empty() && value_.at(0) != '"') {
        return value_;
    }
    if (value_.length() < 2) { // 2
        return "";
    }
    std::istringstream is(value_.substr(1, value_.length() - 2)); // 2
    std::ostringstream os;
    char c;
    while (is.get(c)) {
        if (c == '\\') {
            if (!is.get(c)) {
                break;
            }
        }
        os << c;
    }
    return os.str();
}

Tag::Tag(HlsTag type)
{
    type_ = type;
}

HlsTag Tag::GetType() const
{
    return type_;
}

SingleValueTag::SingleValueTag(HlsTag type, const std::string& v)
    : Tag(type), attr_("", v)
{
}

const Attribute& SingleValueTag::GetValue() const
{
    return attr_;
}

AttributesTag::AttributesTag(HlsTag type, const std::string& v) : Tag(type)
{
    ParseAttributes(v);
}

std::shared_ptr<Attribute> AttributesTag::GetAttributeByName(const char* name) const
{
    for (auto& attribute :attributes) {
        if (attribute->GetName() == name) {
            return attribute;
        }
    }
    return nullptr;
}

void AttributesTag::AddAttribute(std::shared_ptr<Attribute>& attr)
{
    attributes.push_back(attr);
}

void AttributesTag::ParseAttributes(const std::string& field)
{
    std::istringstream iss(field);
    std::ostringstream oss;
    while (!iss.eof()) {
        std::string attrName = ParseAttributeName(iss, oss);
        oss.str("");
        std::string attrValue = ParseAttributeValue(iss, oss);
        oss.str("");
        auto attribute = std::make_shared<Attribute>(attrName, attrValue);
        if (attribute) {
            attributes.push_back(attribute);
        }
    }
}

std::string AttributesTag::ParseAttributeValue(std::istringstream &iss, std::ostringstream &oss)
{
    bool bQuoted = false;
    while (!iss.eof()) {
        char c = static_cast<char>(iss.peek());
        if (c == '\\' && bQuoted) {
            iss.get();
        } else if (c == ',' && !bQuoted) {
            iss.get();
            break;
        } else if (c == '"') {
            bQuoted = !bQuoted;
            if (!bQuoted) {
                oss.put(static_cast<char>(iss.get()));
                break;
            }
        } else if (!bQuoted && (c < '-' || c > 'z'))  { /* out of range */
            iss.get();
            continue;
        }
        if (!iss.eof()) {
            oss.put(static_cast<char>(iss.get()));
        }
    }
    return oss.str();
}

std::string AttributesTag::ParseAttributeName(std::istringstream& iss, std::ostringstream& oss) const
{
    while (!iss.eof()) {
        char c = static_cast<char>(iss.peek());
        if ((c >= 'A' && c <= 'Z') || c == '-') {
            oss.put(static_cast<char>(iss.get()));
        } else if (c == '=') {
            iss.get();
            break;
        } else { /* out of range */
            iss.get();
        }
    }
    return oss.str();
}

ValuesListTag::ValuesListTag(HlsTag type, const std::string& v) : AttributesTag(type, v)
{
    ParseAttributes(v);
}

void ValuesListTag::ParseAttributes(const std::string& field)
{
    auto pos = field.find(',');
    std::shared_ptr<Attribute> attr;
    if (pos != std::string::npos) {
        attr = std::make_shared<Attribute>("DURATION", field.substr(0, pos));
        if (attr) {
            AddAttribute(attr);
        }
        attr = std::make_shared<Attribute>("TITLE", field.substr(pos));
        if (attr) {
            AddAttribute(attr);
        }
    } else { /* broken EXTINF without mandatory comma */
        attr = std::make_shared<Attribute>("DURATION", field);
        if (attr) {
            AddAttribute(attr);
        }
    }
}

std::shared_ptr<Tag> TagFactory::CreateTagByName(const std::string& name, const std::string& value)
{
    auto size = sizeof(exttagmapping) / sizeof(exttagmapping[0]);
    for (int i = 0; i < size; i++) {
        if (name != exttagmapping[i].name_) {
            continue;
        }
        switch (exttagmapping[i].type_) {
            case HlsTag::EXTXDISCONTINUITY:
            case HlsTag::EXTXENDLIST:
            case HlsTag::EXTXIFRAMESONLY:
                return std::make_shared<Tag>(exttagmapping[i].type_);
            case HlsTag::URI:
            case HlsTag::EXTXVERSION:
            case HlsTag::EXTXBYTERANGE:
            case HlsTag::EXTXPROGRAMDATETIME:
            case HlsTag::EXTXTARGETDURATION:
            case HlsTag::EXTXMEDIASEQUENCE:
            case HlsTag::EXTXDISCONTINUITYSEQUENCE:
            case HlsTag::EXTXPLAYLISTTYPE:
                return std::make_shared<SingleValueTag>(exttagmapping[i].type_, value);
            case HlsTag::EXTINF:
                return std::make_shared<ValuesListTag>(exttagmapping[i].type_, value);
            case HlsTag::EXTXKEY:
            case HlsTag::EXTXSESSIONKEY:
            case HlsTag::EXTXMAP:
            case HlsTag::EXTXMEDIA:
            case HlsTag::EXTXSTART:
            case HlsTag::EXTXSTREAMINF:
            case HlsTag::EXTXIFRAMESTREAMINF:
                return std::make_shared<AttributesTag>(exttagmapping[i].type_, value);
            default:
                return nullptr;
        }
    }
    return nullptr;
}

static std::vector<std::string> Split(const std::string& s, const char* delim)
{
    std::vector<std::string> ret;
    std::string::size_type last = 0;
    auto index = s.find(delim, last);
    while (index != std::string::npos) {
        if (index - last > 0) {
            ret.push_back(s.substr(last, index - last));
        }
        last = index + strlen(delim);
        index = s.find(delim, last);
    }
    if (s.empty() || s.size() - last > 0) {
        ret.push_back(s.substr(last));
    }
    return ret;
}

static void ParseTag(std::list<std::shared_ptr<Tag>>& entriesList, std::shared_ptr<Tag>& lastTag, std::string& line)
{
    if (line.find("#EXT") != std::string::npos) { // tag
        line = line.substr(1);
        std::string key;
        std::string value;
        auto keyValue = Split(line, ":");
        key = keyValue[0];
        if (keyValue.size() == 2) { // 2
            value = keyValue[1];
        }
        if (!key.empty()) {
            auto tag = TagFactory::CreateTagByName(key, value);
            if (tag) {
                entriesList.push_back(tag);
            }
            lastTag = tag;
        }
    }
}

static void ParseURI(std::list<std::shared_ptr<Tag>>& entriesList,
                     std::shared_ptr<Tag>& lastTag, const std::string& line)
{
    if (lastTag && lastTag->GetType() == HlsTag::EXTXSTREAMINF) {
        auto streaminftag = std::static_pointer_cast<AttributesTag>(lastTag);
        /* master playlist uri, merge as attribute */
        auto uriAttr = std::make_shared<Attribute>("URI", line);
        if (uriAttr) {
            streaminftag->AddAttribute(uriAttr);
        }
    } else {  /* playlist tag, will take modifiers */
        auto tag = TagFactory::CreateTagByName("", line);
        if (tag) {
            entriesList.push_back(tag);
        }
    }
    lastTag = nullptr;
}

std::list<std::shared_ptr<Tag>> ParseEntries(std::string& s)
{
    std::list<std::shared_ptr<Tag>> list;
    std::shared_ptr<Tag> lastTag = nullptr;
    auto lines = Split(s, "\r\n");
    if (lines.size() == 1) {  // 1
        lines = Split(s, "\n");
    }
    for (auto line : lines) {
        if (line[0] == '#') {
            ParseTag(list, lastTag, line);
        } else if (!line.empty()) {
            /* URI */
            ParseURI(list, lastTag, line);
        } else {
            // drop
            lastTag = nullptr;
        }
    }
    return list;
}
}
}
}
}