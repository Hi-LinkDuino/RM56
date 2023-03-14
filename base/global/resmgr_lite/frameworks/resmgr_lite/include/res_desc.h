/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_RESOURCE_MANAGER_RES_DESC_H
#define OHOS_RESOURCE_MANAGER_RES_DESC_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "res_common.h"

namespace OHOS {
namespace Global {
namespace Resource {
static constexpr uint32_t RES_HEADER_LEN = 136;

static constexpr uint32_t RES_VERSION_LEN = 128;

/**
 * resource.index file header
 */
typedef struct ResHeader {
    // Type identifier for this chunk.  The meaning of this value depends
    // on the containing chunk.
    char version_[RES_VERSION_LEN];

    // Size of the resource.index file (in bytes).  Including header
    uint32_t length_;

    // determiner key count
    uint32_t keyCount_;
} ResHeader;

class IdItem {
public:
    static const uint32_t HEADER_LEN = 12;

    /**
     * Whether the resType is array or not
     * @param type the resType
     * @return true if the resType is array, else false
     */
    static bool IsArrayOfType(ResType type)
    {
        if (type == ResType::STRINGARRAY || type == ResType::INTARRAY || type == ResType::THEME ||
            type == ResType::PLURALS || type == ResType::PATTERN) {
            return true;
        }
        return false;
    }

    void JudgeArray()
    {
        this->isArray_ = IsArrayOfType(resType_);
    }

    /**
     * only theme and pattern may have parent
     * @return true when have parent
     */
    bool HaveParent() const;

    static std::map<ResType, std::string> resTypeStrList;
    /**
     * judge the std::string value is ref or not
     * ref start with '$' end with id
     * sample: "$string:16777225"
     * @param value
     * @param resType when return true, set resType. as sample : ResType:STRING
     * @param id      when return true, set id. as sample : 16777225
     * @return        true: value is ref
     */
    static bool IsRef(const std::string &value, ResType &resType, int &id);

    std::string ToString() const;

    uint32_t size_;
    ResType resType_;
    uint32_t id_;
    uint16_t valueLen_;
    bool isArray_ = false;
    std::string value_;
    std::vector<std::string> values_;
    std::string name_;

private:
    static bool sInit;
    static bool Init();
};

class IdParam {
public:
    ~IdParam();
    std::string ToString() const;

    uint32_t id_;
    uint32_t offset_;
    IdItem *idItem_;
};

class ResId {
public:
    static const uint32_t RESID_HEADER_LEN = 8;
    static const uint32_t IDPARAM_HEADER_LEN = 8;

    ~ResId();
    std::string ToString() const;

    char tag_[4];
    uint32_t count_; // ID count
    std::vector<IdParam *> idParams_;
};

/**
 * describe the qualifier
 */
class KeyParam {
public:
    // type of qualifier
    KeyType type_;

    // value of qualifiers
    uint32_t value_;

    // convert from value_
    std::string str_;

    void InitStr()
    {
        str_ = ConvertToStr();
    }

    const std::string &GetStr() const
    {
        return str_;
    }

    std::string ToString() const;

private:
    const std::string ConvertToStr() const;
    std::string GetScreenDensityStr() const;
    std::string GetDeviceTypeStr() const;
};

/**
 * a ResKey means a Qualifiers Sub-directories
 *
 */
class ResKey {
public:
    static const uint32_t RESKEY_HEADER_LEN = 12;

    static const uint32_t KEYPARAM_HEADER_LEN = 8;

    ~ResKey();

    std::string ToString() const;
    // always 'KEYS'
    char tag_[4];

    // offset from the beginning of the index file, pointing to the resource ID data block
    uint32_t offset_;

    // count of qualifiers
    uint32_t keyParamsCount_;

    // the qualifiers
    std::vector<KeyParam *> keyParams_;

    // the resource ID data
    ResId *resId_;
};
/**
 * a ResDesc means a index file in hap zip
 */
class ResDesc {
public:
    ResDesc();

    ~ResDesc();

    std::string ToString() const;

    ResHeader *resHeader_;

    std::vector<ResKey *> keys_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
