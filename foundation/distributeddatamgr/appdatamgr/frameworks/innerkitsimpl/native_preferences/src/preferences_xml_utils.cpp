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

#include "preferences_xml_utils.h"

#include <sys/stat.h>

#include <cerrno>
#include <cstring>

#include "libxml/parser.h"
#include "logger.h"

namespace OHOS {
namespace NativePreferences {
static bool ParseNodeElement(const xmlNode *node, Element &element);
static bool ParsePrimitiveNodeElement(const xmlNode *node, Element &element);
static bool ParseStringNodeElement(const xmlNode *node, Element &element);
static bool ParseSetNodeElement(const xmlNode *node, Element &element);
static xmlNode *CreateElementNode(Element &element);
static xmlNode *CreatePrimitiveNode(Element &element);
static xmlNode *CreateStringNode(Element &element);
static xmlNode *CreateSetNode(Element &element);
/* static */
bool PreferencesXmlUtils::ReadSettingXml(const std::string &fileName, std::vector<Element> &settings)
{
    LOG_DEBUG("Read setting xml %{private}s start.", fileName.c_str());
    if (fileName.size() == 0) {
        LOG_ERROR("The length of the file name is 0.");
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if (strlen(fileName.c_str()) > PATH_MAX || realpath(fileName.c_str(), path) == nullptr) {
        LOG_ERROR("The file name is incorrect.");
        return false;
    }
    const char *pathString = path;
    xmlDoc *doc = xmlReadFile(pathString, "UTF-8", XML_PARSE_NOBLANKS);
    if (doc == nullptr) {
        LOG_ERROR("The file name is incorrect.");
        return false;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, reinterpret_cast<const xmlChar *>("preferences"))) {
        xmlFreeDoc(doc);
        LOG_ERROR("Failed to obtain the XML root element.");
        return false;
    }

    bool success = true;
    const xmlNode *cur = nullptr;
    for (cur = root->children; cur != nullptr; cur = cur->next) {
        Element element;

        if (ParseNodeElement(cur, element)) {
            settings.push_back(element);
        } else {
            success = false;
            LOG_ERROR("The error occurred during getting xml child elements.");
            break;
        }
    }

    /* free the document */
    xmlFreeDoc(doc);
    LOG_DEBUG("Read setting xml %{private}s end.", fileName.c_str());
    return success;
}

/* static */
bool ParseNodeElement(const xmlNode *node, Element &element)
{
    if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("int"))
        || !xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("long"))
        || !xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("bool"))
        || !xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("float"))
        || !xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("double"))) {
        return ParsePrimitiveNodeElement(node, element);
    }

    if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("string"))) {
        return ParseStringNodeElement(node, element);
    }

    if (!xmlStrcmp(node->name, reinterpret_cast<const xmlChar *>("set"))) {
        return ParseSetNodeElement(node, element);
    }

    LOG_ERROR("An unsupported element type was encountered in parsing = %{public}s.", node->name);
    return false;
}

/* static */
bool ParsePrimitiveNodeElement(const xmlNode *node, Element &element)
{
    if (node == nullptr) {
        return false;
    }

    xmlChar *key = xmlGetProp(node, reinterpret_cast<const xmlChar *>("key"));
    xmlChar *value = xmlGetProp(node, reinterpret_cast<const xmlChar *>("value"));

    bool success = false;
    if (key != nullptr && value != nullptr) {
        element.tag_ = std::string(reinterpret_cast<const char *>(node->name));
        element.key_ = std::string(reinterpret_cast<char *>(key));
        element.value_ = std::string(reinterpret_cast<char *>(value));
        success = true;
    } else {
        LOG_ERROR("Failed to obtain a valid key or value when parsing %{public}s.", node->name);
    }

    if (key != nullptr) {
        xmlFree(key);
    }
    if (value != nullptr) {
        xmlFree(value);
    }
    return success;
}

/* static */
bool ParseStringNodeElement(const xmlNode *node, Element &element)
{
    if (node == nullptr) {
        return false;
    }

    xmlChar *key = xmlGetProp(node, (const xmlChar *)"key");
    xmlChar *text = xmlNodeGetContent(node);

    bool success = false;
    if (text != nullptr) {
        element.tag_ = std::string(reinterpret_cast<const char *>(node->name));
        if (key != nullptr) {
            element.key_ = std::string(reinterpret_cast<char *>(key));
        }
        element.value_ = std::string(reinterpret_cast<char *>(text));
        success = true;
    } else {
        LOG_ERROR("Failed to obtain a valid key or value when parsing a String element.");
    }

    if (key != nullptr) {
        xmlFree(key);
    }
    if (text != nullptr) {
        xmlFree(text);
    }
    return success;
}

/* static */
bool ParseSetNodeElement(const xmlNode *node, Element &element)
{
    if (node == nullptr) {
        return false;
    }

    xmlChar *key = xmlGetProp(node, (const xmlChar *)"key");
    const xmlNode *children = node->children;

    bool success = false;
    if (key != nullptr) {
        element.tag_ = std::string(reinterpret_cast<const char *>(node->name));
        element.key_ = std::string(reinterpret_cast<char *>(key));

        const xmlNode *cur = nullptr;
        bool finishTravelChild = true;
        for (cur = children; cur != nullptr; cur = cur->next) {
            Element child;
            if (ParseNodeElement(cur, child)) {
                element.children_.push_back(child);
            } else {
                finishTravelChild = false;
                LOG_ERROR("Failed to parse the Set element and could not be completed successfully.");
                break;
            }
        }
        success = finishTravelChild;
    } else {
        LOG_ERROR("Failed to obtain a valid key or value when parsing a Set element.");
    }

    if (key != nullptr) {
        xmlFree(key);
    }
    return success;
}

/* static */
bool PreferencesXmlUtils::WriteSettingXml(const std::string &fileName, std::vector<Element> &settings)
{
    LOG_DEBUG("Write setting xml %{private}s start.", fileName.c_str());
    if (fileName.size() == 0) {
        LOG_ERROR("The length of the file name is 0.");
        return false;
    }

    // define doc and root Node
    xmlDoc *doc = xmlNewDoc(BAD_CAST "1.0");
    if (doc == nullptr) {
        LOG_ERROR("Failed to initialize the xmlDoc.");
        return false;
    }
    xmlNode *root_node = xmlNewNode(NULL, BAD_CAST "preferences");
    if (root_node == nullptr) {
        LOG_ERROR("The xmlDoc failed to initialize the root node.");
        xmlFreeDoc(doc);
        return false;
    }
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST "1.0");

    // set root node
    xmlDocSetRootElement(doc, root_node);

    // set children node
    for (Element element : settings) {
        xmlNode *node = CreateElementNode(element);
        if (node == nullptr) {
            LOG_ERROR("The xmlDoc failed to initialize the element node.");
            xmlFreeDoc(doc);
            return false;
        }
        if (root_node == nullptr || xmlAddChild(root_node, node) == nullptr) {
            /* free node in case of error */
            LOG_ERROR("The xmlDoc failed to add the child node.");
            xmlFreeNode(node);
            xmlFreeDoc(doc);
            return false;
        }
    }

    /* 1: formatting spaces are added. */
    int result = xmlSaveFormatFileEnc(fileName.c_str(), doc, "UTF-8", 1);

    xmlFreeDoc(doc);

    if (result > 0) {
        LimitXmlPermission(fileName);
    }

    LOG_DEBUG("Write setting xml %{private}s end.", fileName.c_str());
    return (result > 0) ? true : false;
}

/* static */
xmlNode *CreateElementNode(Element &element)
{
    if ((element.tag_.compare("int") == 0) || (element.tag_.compare("long") == 0)
        || (element.tag_.compare("float") == 0) || (element.tag_.compare("bool") == 0)
        || (element.tag_.compare("double") == 0)) {
        return CreatePrimitiveNode(element);
    }

    if (element.tag_.compare("string") == 0) {
        return CreateStringNode(element);
    }

    if (element.tag_.compare("set") == 0) {
        return CreateSetNode(element);
    }

    LOG_ERROR("An unsupported element type was encountered in parsing = %{public}s.", element.tag_.c_str());
    return nullptr;
}

/* static */
xmlNode *CreatePrimitiveNode(Element &element)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST element.tag_.c_str());
    if (node == nullptr) {
        LOG_ERROR("The xmlDoc failed to initialize the primitive element node.");
        return nullptr;
    }

    const char *key = element.key_.c_str();
    xmlNewProp(node, BAD_CAST "key", BAD_CAST key);

    const char *value = element.value_.c_str();
    xmlNewProp(node, BAD_CAST "value", BAD_CAST value);
    return node;
}

xmlNode *CreateStringNode(Element &element)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST element.tag_.c_str());
    if (node == nullptr) {
        LOG_ERROR("The xmlDoc failed to initialize the string element node.");
        return nullptr;
    }

    if (!element.key_.empty()) {
        const char *key = element.key_.c_str();
        xmlNewProp(node, BAD_CAST "key", BAD_CAST key);
    }

    const char *value = element.value_.c_str();
    xmlNodePtr text = xmlNewText(BAD_CAST value);
    if (xmlAddChild(node, text) == nullptr) {
        xmlFreeNode(text);
    }
    return node;
}

xmlNode *CreateSetNode(Element &element)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST element.tag_.c_str());
    if (node == nullptr) {
        LOG_ERROR("The xmlDoc failed to initialize the set element node.");
        return nullptr;
    }

    const char *key = element.key_.c_str();
    xmlNewProp(node, BAD_CAST "key", BAD_CAST key);

    for (Element child : element.children_) {
        xmlNode *childNode = CreateElementNode(child);
        if (childNode == nullptr) {
            continue;
        }
        if (xmlAddChild(node, childNode) == nullptr) {
            xmlFreeNode(childNode);
        }
    }
    return node;
}

void PreferencesXmlUtils::LimitXmlPermission(const std::string &fileName)
{
    /* clear execute permission of owner, clear execute permission of group, clear all permission of group. */
    struct stat fileStat = { 0 };
    if (stat(fileName.c_str(), &fileStat) != 0) {
        LOG_ERROR("Failed to obtain stat of file, errno:%{public}d.", errno);
        return;
    }
    if ((fileStat.st_mode & (S_IXUSR | S_IXGRP | S_IRWXO)) != 0) {
        int result = chmod(fileName.c_str(), fileStat.st_mode & (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP));
        if (result != 0) {
            LOG_ERROR("Failed to chmod file, errno:%{public}d.", errno);
        }
    }
}

void PreferencesXmlUtils::XmlInitParser()
{
    xmlInitParser();
    LOG_DEBUG("Xml parser get ready.");
}

void PreferencesXmlUtils::XmlCleanupParser()
{
    xmlCleanupParser();
    LOG_DEBUG("Xml parser clean up.");
}
} // End of namespace NativePreferences
} // End of namespace OHOS