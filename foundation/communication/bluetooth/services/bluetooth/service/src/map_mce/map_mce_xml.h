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
 * @brief Defines map client xml object.
 *
 */

/**
 * @file map_mce_xml.h
 *
 * @brief map client xml header file .
 *
 */

#ifndef MAP_MCE_XML_H
#define MAP_MCE_XML_H

#include <string>
#include "libxml/parser.h"

namespace OHOS {
namespace bluetooth {
class MceXmlAttribute {
public:
    MceXmlAttribute();
    void SetAttribute(const std::string &value);
    std::string AsString() const;
    int AsInt();

private:
    std::string value_ {};
};
class MceXmlNode {
public:
    MceXmlNode();
    void SetNode(xmlNode node);
    MceXmlAttribute Attribute(std::string name) const;
    bool Empty() const;
    std::string Name() const;
    MceXmlNode FirstChild() const;
    MceXmlNode NextSibling() const;

private:
    bool empty_ = true;
    xmlNode node_ {};
};
class MceXmlDoc {
public:
    ~MceXmlDoc();
    void LoadString(std::string content);
    MceXmlNode Child(std::string name) const;

private:
    xmlDocPtr pDoc_ = nullptr;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_XML_H
