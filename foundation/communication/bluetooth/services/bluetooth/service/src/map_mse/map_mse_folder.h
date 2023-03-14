/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef MAP_MSE_FOLDER_H
#define MAP_MSE_FOLDER_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include "base_def.h"
#include "bt_def.h"
#include "log.h"
#include "map_mse_types.h"

namespace OHOS {
namespace bluetooth {
class MapMseFolder {
public:
    explicit MapMseFolder(const std::string &name);
    explicit MapMseFolder(const std::string &name, MapMseFolder &parent);
    virtual ~MapMseFolder();
    MapMseFolder *SetFolder(const std::string &name);
    MapMseFolder *SetSmsMmsFolder(const std::string &name);
    MapMseFolder *SetEmailFolder(int folderId, const std::string &name);
    MapMseFolder *SetImFolder(int folderId, const std::string &name);
    MapMseFolder *GetRoot(void);
    MapMseFolder *GetParent(void) const;
    MapMseFolder *GetFolderByName(const std::string &name);
    MapMseFolder *GetSubFolder(const std::string &folderName);
    std::string GetData(uint16_t offset, uint16_t count) const;
    std::string GetName(void) const;
    std::string GetFullPath(void) const;
    uint16_t GetSubFolderSize(void) const;
    int GetFolderId(void) const;
    bool IsIgnore(void) const;
    void SetIgnore(const bool ignore);

private:
    std::string name_ = "";
    MapMseFolder *parent_ = nullptr;
    MapMseFolder *root_ = nullptr;
    void SetFolderId(const int folderId);
    int folderId_ = 0;
    bool ignore_ = false;
    std::unordered_map<std::string, std::shared_ptr<MapMseFolder>> subFolders_ {};
    BT_DISALLOW_COPY_AND_ASSIGN(MapMseFolder);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_FOLDER_H
