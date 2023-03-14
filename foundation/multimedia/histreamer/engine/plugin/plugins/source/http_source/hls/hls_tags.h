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

#ifndef HISTREAMER_HLS_TAGS_H
#define HISTREAMER_HLS_TAGS_H

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class Attribute {
public:
    Attribute(std::string name, std::string value);
    std::string GetName() const;
    Attribute UnescapeQuotes() const;
    uint64_t Decimal() const;
    std::string QuotedString() const;
    double FloatingPoint() const;
    std::vector<uint8_t> HexSequence() const;
    std::pair<std::size_t, std::size_t> GetByteRange() const;
    std::pair<int, int> GetResolution() const;
private:
    std::string name_;
    std::string value_;
};

enum struct HlsTagSection : uint8_t {
    REGULAR,
    SINGLE_VALUE,
    ATTRIBUTES,
    VALUES_LIST,
};

enum struct HlsTag : uint32_t {
    SECTION_REGULAR_START = static_cast<uint8_t>(HlsTagSection::REGULAR) << 16U,  // regular tag
    SECTION_SINGLE_VALUE_START = static_cast<uint8_t>(HlsTagSection::SINGLE_VALUE) << 16U,  // single value tag
    SECTION_ATTRIBUTES_START = static_cast<uint8_t>(HlsTagSection::ATTRIBUTES) << 16U,  // attributes tag
    SECTION_VALUES_LIST_START = static_cast<uint8_t>(HlsTagSection::VALUES_LIST) << 16U,  // values list tag

    EXTXDISCONTINUITY = SECTION_REGULAR_START,
    EXTXENDLIST,
    EXTXIFRAMESONLY,

    URI = SECTION_SINGLE_VALUE_START,
    EXTXVERSION,
    EXTXBYTERANGE,
    EXTXPROGRAMDATETIME,
    EXTXTARGETDURATION,
    EXTXMEDIASEQUENCE,
    EXTXDISCONTINUITYSEQUENCE,
    EXTXPLAYLISTTYPE,

    EXTXKEY = SECTION_ATTRIBUTES_START,
    EXTXMAP,
    EXTXMEDIA,
    EXTXSTART,
    EXTXSTREAMINF,
    EXTXIFRAMESTREAMINF,
    EXTXSESSIONKEY,

    EXTINF = SECTION_VALUES_LIST_START,
};

class Tag {
public:
    explicit Tag(HlsTag type);
    virtual ~Tag() = default;
    HlsTag GetType() const;
private:
    HlsTag type_;
};

class SingleValueTag : public Tag {
public:
    SingleValueTag(HlsTag type, const std::string& v);
    ~SingleValueTag() override = default;
    const Attribute& GetValue() const;
private:
    Attribute attr_;
};

class AttributesTag : public Tag {
public:
    AttributesTag(HlsTag type, const std::string& v);
    ~AttributesTag() override = default;
    std::shared_ptr<Attribute> GetAttributeByName(const char* name) const;
    void AddAttribute(std::shared_ptr<Attribute>& attr);
protected:
    std::list<std::shared_ptr<Attribute>> attributes;
private:
    void ParseAttributes(const std::string& field);
    std::string ParseAttributeName(std::istringstream& iss, std::ostringstream& oss) const;
    std::string ParseAttributeValue(std::istringstream& iss, std::ostringstream& oss);
};

class ValuesListTag : public AttributesTag {
public:
    ValuesListTag(HlsTag type, const std::string& v);
    ~ValuesListTag() override = default;
private:
    void ParseAttributes(const std::string& field);
};

class TagFactory {
public:
    static std::shared_ptr<Tag> CreateTagByName(const std::string& name, const std::string& value);
    static std::shared_ptr<Attribute> CreateAttributeByName(const std::string&);
};

std::list<std::shared_ptr<Tag>> ParseEntries(std::string& s);
}
}
}
}

#endif // HISTREAMER_HLS_TAGS_H
