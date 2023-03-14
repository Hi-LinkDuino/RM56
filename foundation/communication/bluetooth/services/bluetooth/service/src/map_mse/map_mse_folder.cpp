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

#include "map_mse_folder.h"
#include <sstream>
#include "libxml/parser.h"

namespace OHOS {
namespace bluetooth {
MapMseFolder::MapMseFolder(const std::string &name) : name_(name)
{}

MapMseFolder::MapMseFolder(const std::string &name, MapMseFolder &parent) : name_(name), parent_(&parent)
{}

MapMseFolder::~MapMseFolder()
{
    subFolders_.clear();
}

MapMseFolder *MapMseFolder::SetFolder(const std::string &name)
{
    if (subFolders_.find(name) == subFolders_.end()) {
        subFolders_[name] = std::make_shared<MapMseFolder>(name, *this);
    }
    return subFolders_[name].get();
}

MapMseFolder *MapMseFolder::SetSmsMmsFolder(const std::string &name)
{
    MapMseFolder *folder = SetFolder(name);
    return folder;
}

MapMseFolder *MapMseFolder::SetEmailFolder(int folderId, const std::string &name)
{
    MapMseFolder *folder = SetFolder(name);
    folder->SetFolderId(folderId);
    return folder;
}

MapMseFolder *MapMseFolder::SetImFolder(int folderId, const std::string &name)
{
    MapMseFolder *folder = SetFolder(name);
    folder->SetFolderId(folderId);
    return folder;
}

void MapMseFolder::SetFolderId(const int folderId)
{
    folderId_ = folderId;
}

MapMseFolder *MapMseFolder::GetParent() const
{
    return parent_;
}

MapMseFolder *MapMseFolder::GetRoot(void)
{
    root_ = this;
    while (root_->parent_ != nullptr) {
        root_ = root_->parent_;
    }
    return root_;
}

MapMseFolder *MapMseFolder::GetSubFolder(const std::string &folderName)
{
    if (subFolders_.find(folderName) != subFolders_.end()) {
        return subFolders_[folderName].get();
    }
    return nullptr;
}

MapMseFolder *MapMseFolder::GetFolderByName(const std::string &name)
{
    MapMseFolder *folderElement = GetRoot();
    folderElement = folderElement->GetSubFolder("telecom");
    folderElement = folderElement->GetSubFolder("msg");
    folderElement = folderElement->GetSubFolder(name);
    return folderElement;
}

std::string MapMseFolder::GetData(uint16_t offset, uint16_t count) const
{
    std::string data;
    if (static_cast<std::size_t>(offset) > subFolders_.size()) {
        return data;
    }
    auto limit = offset + count;
    if (static_cast<std::size_t>(limit) > subFolders_.size()) {
        limit = static_cast<int>(subFolders_.size());
    }

    xmlChar *xmlbuff;
    int buffersize;
    std::ostringstream oss;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "folder-listing");
    xmlDocSetRootElement(doc, root_node);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST MAP_V10.c_str());

    std::vector<std::string> folderNames;
    for (auto &iter : subFolders_) {
        folderNames.push_back(iter.second->GetName());
    }

    for (int i = static_cast<int>(offset); i < limit; i++) {
        xmlNodePtr node = xmlNewNode(NULL, BAD_CAST"folder");
        xmlAddChild(root_node, node);
        xmlNewProp(node, BAD_CAST"name", BAD_CAST folderNames[i].c_str());
    }
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    oss << xmlbuff;
    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return oss.str();
}

std::string MapMseFolder::GetName() const
{
    return name_;
}

std::string MapMseFolder::GetFullPath(void) const
{
    std::string fullPath = name_;
    auto temp = parent_;
    while (temp != nullptr) {
        fullPath.insert(0, temp->name_ + "/");
        temp = temp->parent_;
    }
    return fullPath;
}

uint16_t MapMseFolder::GetSubFolderSize(void) const
{
    return static_cast<uint16_t>(subFolders_.size());
}

int MapMseFolder::GetFolderId(void) const
{
    return folderId_;
}

bool MapMseFolder::IsIgnore(void) const
{
    return ignore_;
}

void MapMseFolder::SetIgnore(const bool ignore)
{
    ignore_ = ignore;
}
}  // namespace bluetooth
}  // namespace OHOS