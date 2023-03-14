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

#include "xml_parse.h"
#include <algorithm>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace utility {
struct XmlParse::impl {
    xmlDocPtr doc_;
    xmlNodePtr IntHasSection(const std::string &section, const std::string &subSection);
    xmlNodePtr IntHasProperty(const std::string &section, const std::string &subSection, const std::string &property);
    void ConstructPropertyNode(xmlNodePtr &sectionNode, xmlNodePtr &propertyNode, const std::string &property);
    xmlNodePtr FindOrCreatePropertyNode(
        const std::string &section, const std::string &subSection, const std::string &property);
    bool GetValue(xmlNodePtr node, int &value);
    bool GetValue(xmlNodePtr node, std::string &value);
    bool GetValue(xmlNodePtr node, bool &value);
    bool HasProperty(xmlNodePtr node, const std::string &property);
    bool RemoveProperty(xmlNodePtr node, const std::string &property);
};

xmlNodePtr XmlParse::impl::IntHasSection(const std::string &section, const std::string &subSection)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc_);
    xmlNodePtr getSectionNode = NULL;
    if (rootNode == NULL) {
        return NULL;
    }
    for (xmlNodePtr btSectionNode = rootNode->children; btSectionNode; btSectionNode = btSectionNode->next) {
        xmlChar *btSectionNodeProp = xmlGetProp(btSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), btSectionNodeProp) == 0) {
            getSectionNode = btSectionNode;
        }
        xmlFree(btSectionNodeProp);
    }

    if (subSection == "") {
        return getSectionNode;
    }
    if (getSectionNode == NULL) {
        return NULL;
    }
    for (xmlNodePtr btSubSectionNode = getSectionNode->children; btSubSectionNode;
            btSubSectionNode = btSubSectionNode->next) {
        xmlChar *btSubSectionNodeProp = xmlGetProp(btSubSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)subSection.c_str(), btSubSectionNodeProp) == 0) {
            xmlFree(btSubSectionNodeProp);
            return btSubSectionNode;
        }
        xmlFree(btSubSectionNodeProp);
    }
    return NULL;
}

xmlNodePtr XmlParse::impl::IntHasProperty(
    const std::string &section, const std::string &subSection, const std::string &property)
{
    xmlNodePtr sectionNode = IntHasSection(section, subSection);
    if ((sectionNode != NULL) && (sectionNode->children)) {
        for (xmlNodePtr btPropertyNode = sectionNode->children; btPropertyNode;
                btPropertyNode = btPropertyNode->next) {
            xmlChar *btPropertyNodeProp = xmlGetProp(btPropertyNode, BAD_CAST "property");
            if (xmlStrcmp((xmlChar *)property.c_str(), btPropertyNodeProp) == 0) {
                xmlFree(btPropertyNodeProp);
                return btPropertyNode;
            }
            xmlFree(btPropertyNodeProp);
        }
    }
    return NULL;
}

void XmlParse::impl::ConstructPropertyNode(
    xmlNodePtr &sectionNode, xmlNodePtr &propertyNode, const std::string &property)
{
    for (propertyNode = sectionNode->children; propertyNode; propertyNode = propertyNode->next) {
        xmlChar *propertyNodeProp = xmlGetProp(propertyNode, BAD_CAST "property");
        if (xmlStrcmp((xmlChar *)property.c_str(), propertyNodeProp) == 0) {
            xmlFree(propertyNodeProp);
            break;
        }
        xmlFree(propertyNodeProp);
    }
    if (propertyNode == NULL) {
        propertyNode = xmlNewNode(NULL, BAD_CAST "T1");
        xmlSetProp(propertyNode, BAD_CAST "property", (xmlChar *)property.c_str());
        xmlAddChild(sectionNode, propertyNode);
    }
}

xmlNodePtr XmlParse::impl::FindOrCreatePropertyNode(
    const std::string &section, const std::string &subSection, const std::string &property)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc_);
    xmlNodePtr sectionNode = NULL;
    xmlNodePtr subSectionNode = NULL;
    xmlNodePtr propertyNode = NULL;

    if (rootNode == NULL) {
        return NULL;
    }
    if (rootNode->children != NULL) {
        for (sectionNode = rootNode->children; sectionNode; sectionNode = sectionNode->next) {
            xmlChar *sectionNodeProp = xmlGetProp(sectionNode, BAD_CAST "section");
            if (xmlStrcmp((xmlChar *)section.c_str(), sectionNodeProp) == 0) {
                xmlFree(sectionNodeProp);
                break;
            }
            xmlFree(sectionNodeProp);
        }
    }

    if (sectionNode == NULL) {
        sectionNode = xmlNewNode(NULL, BAD_CAST "T1");
        xmlSetProp(sectionNode, BAD_CAST "section", (xmlChar *)section.c_str());
        xmlAddChild(rootNode, sectionNode);
    }

    if (subSection != "") {
        for (subSectionNode = sectionNode->children; subSectionNode; subSectionNode = subSectionNode->next) {
            xmlChar *subSectionNodeProp = xmlGetProp(subSectionNode, BAD_CAST "section");
            if (xmlStrcmp((xmlChar *)subSection.c_str(), subSectionNodeProp) == 0) {
                xmlFree(subSectionNodeProp);
                break;
            }
            xmlFree(subSectionNodeProp);
        }
        if (!subSectionNode) {
            subSectionNode = xmlNewNode(NULL, BAD_CAST "T1");
            xmlSetProp(subSectionNode, BAD_CAST "section", (xmlChar *)subSection.c_str());
            xmlAddChild(sectionNode, subSectionNode);
        }
        ConstructPropertyNode(subSectionNode, propertyNode, property);
    } else {
        ConstructPropertyNode(sectionNode, propertyNode, property);
    }
    return propertyNode;
}

bool XmlParse::impl::GetValue(xmlNodePtr node, int &value)
{
    if (node == NULL) {
        return false;
    }
    xmlChar *nodeContent = xmlNodeGetContent(node->children);
    if (nodeContent == NULL) {
        return false;
    }
    std::string nodeContentStr = (char *)nodeContent;
    std::string sValue = nodeContentStr;
    sValue = sValue.substr(SIZEOF_0X, sValue.size() - SIZEOF_0X);
    value = std::stol(sValue, nullptr, BASE_16);
    xmlFree(nodeContent);
    return true;
}

bool XmlParse::impl::GetValue(xmlNodePtr node, std::string &value)
{
    if (node == NULL) {
        return false;
    }
    xmlChar *nodeContent = xmlNodeGetContent(node->children);
    if (nodeContent == NULL) {
        return false;
    }
    value = (char *)nodeContent;
    xmlFree(nodeContent);
    return true;
}

bool XmlParse::impl::GetValue(xmlNodePtr node, bool &value)
{
    if (node == NULL) {
        return false;
    }
    xmlChar *nodeContent = xmlNodeGetContent(node->children);
    if (nodeContent == NULL) {
        return false;
    }
    bool retVal = true;
    std::string sValue = (char *)nodeContent;
    if (sValue == "true") {
        value = true;
    } else if (sValue == "false") {
        value = false;
    } else {
        retVal = false;
    }
    xmlFree(nodeContent);
    return retVal;
}

bool XmlParse::impl::HasProperty(xmlNodePtr node, const std::string &property)
{
    if (node == NULL) {
        return false;
    }
    for (xmlNodePtr btPropertyNode = node->children; btPropertyNode; btPropertyNode = btPropertyNode->next) {
        xmlChar *btPropertyNodeProp = xmlGetProp(btPropertyNode, BAD_CAST "property");
        if (xmlStrcmp((xmlChar *)property.c_str(), btPropertyNodeProp) == 0) {
            xmlFree(btPropertyNodeProp);
            return true;
        }
        xmlFree(btPropertyNodeProp);
    }
    return false;
}

bool XmlParse::impl::RemoveProperty(xmlNodePtr node, const std::string &property)
{
    if (node == NULL) {
        return false;
    }
    xmlNodePtr propertyNode = NULL;
    for (propertyNode = node->children; propertyNode; propertyNode = propertyNode->next) {
        xmlChar *propertyNodeProp = xmlGetProp(propertyNode, BAD_CAST "property");
        if (xmlStrcmp((xmlChar *)property.c_str(), propertyNodeProp) == 0) {
            xmlFree(propertyNodeProp);
            break;
        }
        xmlFree(propertyNodeProp);
    }
    if (propertyNode == NULL) {
        return false;
    }
    xmlUnlinkNode(propertyNode);
    xmlFreeNode(propertyNode);
    return true;
}

XmlParse::XmlParse() : pimpl()
{
    pimpl = std::make_unique<impl>();
    pimpl->doc_ = NULL;
}

XmlParse::~XmlParse()
{
    xmlFreeDoc(pimpl->doc_);
}

bool XmlParse::Load(const std::string &path)
{
    this->filePath_ = path;
    return Parse();
}

bool XmlParse::Parse()
{
    if (pimpl->doc_ != NULL) {
        xmlFreeDoc(pimpl->doc_);
    }
    pimpl->doc_ = xmlReadFile(this->filePath_.c_str(), "UTF-8", XML_PARSE_NOBLANKS);
    if (pimpl->doc_ == NULL) {
        return false;
    }
    return true;
}

bool XmlParse::Save()
{
    int result = xmlSaveFormatFileEnc(this->filePath_.c_str(), pimpl->doc_, "UTF-8", 1);
    if (result == -1) {
        return false;
    }
    return true;
}

bool XmlParse::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, int &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, subSection, property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, std::string &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, subSection, property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, bool &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, subSection, property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const int &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, subSection, property);
    std::string storeVal = "0x";
    std::stringstream ss;
    ss << std::hex << value;
    std::string convertVal = ss.str();
    std::transform(convertVal.begin(), convertVal.end(), convertVal.begin(), ::toupper);
    storeVal += convertVal;
    xmlNodeSetContent(propertyNode, (xmlChar *)storeVal.c_str());
    return true;
}

bool XmlParse::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const std::string &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, subSection, property);
    xmlNodeSetContent(propertyNode, (xmlChar *)value.c_str());
    return true;
}

bool XmlParse::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const bool &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, subSection, property);
    if (propertyNode == NULL) {
        return false;
    }
    if (value) {
        xmlNodeSetContent(propertyNode, BAD_CAST "true");
    } else {
        xmlNodeSetContent(propertyNode, BAD_CAST "false");
    }
    return true;
}

bool XmlParse::HasProperty(const std::string &section, const std::string &subSection, const std::string &property)
{
    xmlNodePtr sectionNode = pimpl->IntHasSection(section, subSection);
    return pimpl->HasProperty(sectionNode, property);
}

bool XmlParse::HasSection(const std::string &section, const std::string &subSection)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(pimpl->doc_);
    if (rootNode == NULL) {
        return false;
    }
    xmlNodePtr btSectionNode = rootNode->children;
    xmlNodePtr btSubSectionNode = NULL;
    for (; btSectionNode; btSectionNode = btSectionNode->next) {
        xmlChar *btSectionNodeProp = xmlGetProp(btSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), btSectionNodeProp) == 0) {
            btSubSectionNode = btSectionNode->children;
            xmlFree(btSectionNodeProp);
            break;
        }
        xmlFree(btSectionNodeProp);
    }
    if (btSectionNode != NULL) {
        for (; btSubSectionNode; btSubSectionNode = btSubSectionNode->next) {
            xmlChar *btSubSectionNodeProp = xmlGetProp(btSubSectionNode, BAD_CAST "section");
            if (xmlStrcmp((xmlChar *)subSection.c_str(), btSubSectionNodeProp) == 0) {
                xmlFree(btSubSectionNodeProp);
                return true;
            }
            xmlFree(btSubSectionNodeProp);
        }
    }
    return false;
}

bool XmlParse::GetSubSections(const std::string &section, std::vector<std::string> &subSections)
{
    std::string btSubSectionNodePropStr;
    xmlNodePtr rootNode = xmlDocGetRootElement(pimpl->doc_);
    if (rootNode == NULL) {
        return false;
    }
    for (xmlNodePtr btSectionNode = rootNode->children; btSectionNode; btSectionNode = btSectionNode->next) {
        xmlChar *btSectionNodeProp = xmlGetProp(btSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), btSectionNodeProp) == 0) {
            if (btSectionNode == NULL) {
                return false;
            }
            for (xmlNodePtr btSubSectionNode = btSectionNode->children; btSubSectionNode;
                 btSubSectionNode = btSubSectionNode->next) {
                xmlChar *btSubSectionNodeProp = xmlGetProp(btSubSectionNode, BAD_CAST "section");
                if (btSubSectionNodeProp == NULL) {
                    continue;
                }
                btSubSectionNodePropStr = (char *)btSubSectionNodeProp;
                subSections.push_back(btSubSectionNodePropStr);
                xmlFree(btSubSectionNodeProp);
            }
        }
        xmlFree(btSectionNodeProp);
    }
    if (subSections.size() == 0) {
        return false;
    }
    return true;
}

bool XmlParse::RemoveSection(const std::string &section, const std::string &subSection)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(pimpl->doc_);
    xmlNodePtr sectionNode = NULL;

    if (rootNode == NULL) {
        return false;
    }
    for (sectionNode = rootNode->children; sectionNode; sectionNode = sectionNode->next) {
        xmlChar *sectionNodeProp = xmlGetProp(sectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), sectionNodeProp) == 0) {
            xmlFree(sectionNodeProp);
            break;
        }
        xmlFree(sectionNodeProp);
    }

    if (sectionNode == NULL) {
        return false;
    }

    xmlNodePtr subSectionNode = NULL;

    for (subSectionNode = sectionNode->children; subSectionNode; subSectionNode = subSectionNode->next) {
        xmlChar *subSectionNodeProp = xmlGetProp(subSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)subSection.c_str(), subSectionNodeProp) == 0) {
            xmlFree(subSectionNodeProp);
            break;
        }
        xmlFree(subSectionNodeProp);
    }
    if (subSectionNode == NULL) {
        return false;
    }
    xmlUnlinkNode(subSectionNode);
    xmlFreeNode(subSectionNode);
    return true;
}

bool XmlParse::RemoveProperty(const std::string &section, const std::string &subSection, const std::string &property)
{
    xmlNodePtr sectionNode = pimpl->IntHasSection(section, subSection);
    return pimpl->RemoveProperty(sectionNode, property);
}

bool XmlParse::GetValue(const std::string &section, const std::string &property, int &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, "", property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::GetValue(const std::string &section, const std::string &property, std::string &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, "", property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::GetValue(const std::string &section, const std::string &property, bool &value)
{
    xmlNodePtr getPropertyNode = pimpl->IntHasProperty(section, "", property);
    return pimpl->GetValue(getPropertyNode, value);
}

bool XmlParse::SetValue(const std::string &section, const std::string &property, const int &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, "", property);
    std::string storeVal = "0x";
    std::stringstream ss;
    ss << std::hex << value;
    std::string convertVal = ss.str();
    std::transform(convertVal.begin(), convertVal.end(), convertVal.begin(), ::toupper);
    storeVal += convertVal;
    xmlNodeSetContent(propertyNode, (xmlChar *)storeVal.c_str());
    return true;
}

bool XmlParse::SetValue(const std::string &section, const std::string &property, const std::string &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, "", property);
    xmlNodeSetContent(propertyNode, (xmlChar *)value.c_str());
    return true;
}

bool XmlParse::SetValue(const std::string &section, const std::string &property, const bool &value)
{
    xmlNodePtr propertyNode = pimpl->FindOrCreatePropertyNode(section, "", property);
    if (propertyNode == NULL) {
        return false;
    }
    if (value) {
        xmlNodeSetContent(propertyNode, BAD_CAST "true");
    } else {
        xmlNodeSetContent(propertyNode, BAD_CAST "false");
    }
    return true;
}

bool XmlParse::HasProperty(const std::string &section, const std::string &property)
{
    xmlNodePtr sectionNode = pimpl->IntHasSection(section, "");
    return pimpl->HasProperty(sectionNode, property);
}

bool XmlParse::HasSection(const std::string &section)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(pimpl->doc_);
    if (rootNode == NULL) {
        return false;
    }
    for (xmlNodePtr btSectionNode = rootNode->children; btSectionNode; btSectionNode = btSectionNode->next) {
        xmlChar *btSectionNodeProp = xmlGetProp(btSectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), btSectionNodeProp) == 0) {
            xmlFree(btSectionNodeProp);
            return true;
        }
        xmlFree(btSectionNodeProp);
    }

    return false;
}

bool XmlParse::RemoveSection(const std::string &section)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(pimpl->doc_);
    xmlNodePtr sectionNode = NULL;

    if (rootNode == NULL) {
        return false;
    }
    for (sectionNode = rootNode->children; sectionNode; sectionNode = sectionNode->next) {
        xmlChar *sectionNodeProp = xmlGetProp(sectionNode, BAD_CAST "section");
        if (xmlStrcmp((xmlChar *)section.c_str(), sectionNodeProp) == 0) {
            xmlFree(sectionNodeProp);
            break;
        }
        xmlFree(sectionNodeProp);
    }
    if (sectionNode == NULL) {
        return false;
    }
    xmlUnlinkNode(sectionNode);
    xmlFreeNode(sectionNode);
    return true;
}

bool XmlParse::RemoveProperty(const std::string &section, const std::string &property)
{
    xmlNodePtr sectionNode = pimpl->IntHasSection(section, "");
    return pimpl->RemoveProperty(sectionNode, property);
}
}  // namespace utility