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
 * @brief Defines map mce xml .
 *
 */

/**
 * @file map_mce_xml.cpp
 *
 * @brief map mce xml source file .
 *
 */

#include "map_mce_xml.h"
#include <cstring>

namespace OHOS {
namespace bluetooth {
MceXmlAttribute::MceXmlAttribute() : value_("")
{
    // do nothing
}

void MceXmlAttribute::SetAttribute(const std::string &value)
{
    value_ = value;
}

std::string MceXmlAttribute::AsString() const
{
    return value_;
}

int MceXmlAttribute::AsInt()
{
    if (value_ != "") {
        int num = atoi(value_.c_str());
        return num;
    } else {
        return 0;
    }
}

MceXmlNode::MceXmlNode()
{
    empty_ = true;
}

void MceXmlNode::SetNode(xmlNode node)
{
    node_ = node;
    empty_ = false;
}

MceXmlAttribute MceXmlNode::Attribute(std::string name) const
{
    xmlChar *value = xmlGetProp(&node_, (const xmlChar *)name.c_str());
    std::string str;
    MceXmlAttribute mceAttr;
    if (value != nullptr) {
        str = (char *)value;
        mceAttr.SetAttribute(str);
        xmlFree(value);
    } else {
        str = "";
        mceAttr.SetAttribute(str);
    }
    return mceAttr;
}

bool MceXmlNode::Empty() const
{
    return empty_;
}

std::string MceXmlNode::Name() const
{
    std::string retStr((const char *)node_.name);
    return retStr;
}

MceXmlNode MceXmlNode::FirstChild() const
{
    xmlNodePtr first = xmlFirstElementChild((xmlNodePtr)&node_);
    MceXmlNode node;
    if (first != nullptr) {
        node.SetNode(*first);
    }
    return node;
}

MceXmlNode MceXmlNode::NextSibling() const
{
    xmlNodePtr next = xmlNextElementSibling((xmlNodePtr)&node_);
    MceXmlNode node;
    if (next != nullptr) {
        node.SetNode(*next);
    }
    return node;
}

MceXmlDoc::~MceXmlDoc()
{
    if (pDoc_ != nullptr) {
        xmlFreeDoc(pDoc_);
    }
}

void MceXmlDoc::LoadString(std::string content)
{
    if (content != "") {
        pDoc_ = xmlReadMemory(content.c_str(), int(content.size()), NULL, "UTF-8", XML_PARSE_RECOVER);
    }
}

MceXmlNode MceXmlDoc::Child(std::string name) const
{
    xmlNodePtr root = xmlDocGetRootElement(pDoc_);
    MceXmlNode node;
    if (root != nullptr) {
        if (!xmlStrcmp(root->name, (const xmlChar *)name.c_str())) {
            node.SetNode(*root);
        }
    }
    return node;
}
}  // namespace bluetooth
}  // namespace OHOS
