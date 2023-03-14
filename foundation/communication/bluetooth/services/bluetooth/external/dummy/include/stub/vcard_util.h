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

#ifndef VCARD_UTIL_H
#define VCARD_UTIL_H

#include <iostream>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace stub {
enum class VCardVersion : uint8_t { VER_OTHER = 0x00, VER_2_1 = 0x21, VER_3_0 = 0x30 };

typedef std::multimap<std::string, std::string> VCardParams;

class VCardProperty {
public:
    VCardProperty(VCardVersion ver, std::string group, std::string name, std::string value, std::string paramsStr)
    {}
    VCardProperty(std::string group, std::string name, std::string value, VCardParams params)
    {}
    virtual ~VCardProperty() = default;
    std::string GetGroup() const
    {
        return "";
    }
    std::string GetName() const
    {
        return "";
    }
    const std::vector<std::string> &GetValues() const
    {
        static std::vector<std::string> v;
        return v;
    }
    const VCardParams &GetParams() const
    {
        static VCardParams v;
        return v;
    }
    void BuildParamString(VCardVersion ver, std::vector<uint8_t> &outList) const
    {}
    void BuildValueString(std::vector<uint8_t> &outList) const
    {}

private:
    std::string group_ = "";
    std::string name_ = "";
    std::vector<std::string> values_ {};
    VCardParams params_ {};
};

class VCard {
public:
    VCard() = default;
    virtual ~VCard() = default;
    VCardVersion GetVersion() const
    {
        static VCardVersion v;
        return v;
    }
    void SetVersion(const VCardVersion ver)
    {}
    static std::string Version2Str(VCardVersion ver)
    {
        return "";
    }
    static VCardVersion Str2Version(const std::string &str)
    {
        static VCardVersion v;
        return v;
    }
    const std::vector<VCardProperty> &Properties() const
    {
        static std::vector<VCardProperty> v;
        return v;
    }
    void AddProperty(const VCardProperty &property)
    {}
    bool HasProperty(std::string propertyId)
    {
        return true;
    }
};

class VCardUtil {
public:
    static std::vector<uint8_t> Build(const VCard &vCard, VCardVersion ver)
    {
        static std::vector<uint8_t> v;
        return v;
    }
    static std::vector<uint8_t> Build(const std::vector<VCard> &vCards, VCardVersion ver)
    {
        static std::vector<uint8_t> v;
        return v;
    }
    static std::vector<VCard> Parse(std::istream &is)
    {
        std::vector<VCard> v;
        return v;
    }
    static std::vector<VCard> Parse(const std::vector<uint8_t> &datas)
    {
        std::vector<VCard> v;
        return v;
    }

private:
    VCardUtil() = default;
    virtual ~VCardUtil() = default;
};
}  // namespace stub
#endif // VCARD_UTIL_H