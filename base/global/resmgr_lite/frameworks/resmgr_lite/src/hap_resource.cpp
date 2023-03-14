/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hap_resource.h"

#include <fstream>
#include <iostream>

#include "hap_parser.h"
#include "hilog_wrapper.h"
#include "locale_matcher.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

#if defined(__linux__)
#include <malloc.h>
#endif

namespace OHOS {
namespace Global {
namespace Resource {
HapResource::ValueUnderQualifierDir::ValueUnderQualifierDir(const std::vector<KeyParam *> &keyParams, IdItem *idItem,
    HapResource *hapResource) : hapResource_(hapResource)
{
    keyParams_ = keyParams;
    folder_ = HapParser::ToFolderPath(keyParams_);
    idItem_ = idItem;
    InitResConfig();
}

HapResource::ValueUnderQualifierDir::~ValueUnderQualifierDir()
{
    // keyParams_ idItem_ was passed into this, we dont delete them because someone will do
    delete (resConfig_);
}

void HapResource::ValueUnderQualifierDir::InitResConfig()
{
    resConfig_ = HapParser::CreateResConfigFromKeyParams(keyParams_);
}

// IdValues
HapResource::IdValues::~IdValues()
{
    for (size_t i = 0; i < limitPaths_.size(); ++i) {
        auto path = limitPaths_[i];
        delete (path);
    }
}

// HapResource
HapResource::HapResource(const std::string path, time_t lastModTime, const ResConfig *defaultConfig, ResDesc *resDes)
    : indexPath_(path), lastModTime_(lastModTime), resDesc_(resDes), defaultConfig_(defaultConfig)
{
}

HapResource::~HapResource()
{
    delete (resDesc_);
    std::map<uint32_t, IdValues *>::iterator iter;
    for (iter = idValuesMap_.begin(); iter != idValuesMap_.end(); ++iter) {
        IdValues *ptr = iter->second;
        delete (ptr);
    }

    for (size_t i = 0; i < idValuesNameMap_.size(); ++i) {
        delete (idValuesNameMap_[i]);
    }
    lastModTime_ = 0;
    // defaultConfig_ was passed by constructor, we do not delete it here
    defaultConfig_ = nullptr;
}

const HapResource *HapResource::LoadFromIndex(const char *path, const ResConfigImpl *defaultConfig, bool system)
{
    std::ifstream inFile(path, std::ios::binary | std::ios::in);
    if (!inFile.good()) {
        return nullptr;
    }
    inFile.seekg(0, std::ios::end);
    int bufLen = inFile.tellg();
    if (bufLen <= 0) {
        HILOG_ERROR("file size is zero");
        inFile.close();
        return nullptr;
    }
    void *buf = malloc(bufLen);
    if (buf == nullptr) {
        HILOG_ERROR("Error allocating memory");
        inFile.close();
        return nullptr;
    }
    inFile.seekg(0, std::ios::beg);
    inFile.read(static_cast<char *>(buf), bufLen);
    inFile.close();

    HILOG_DEBUG("extract success, bufLen:%d", bufLen);

    ResDesc *resDesc = new (std::nothrow) ResDesc();
    if (resDesc == nullptr) {
        HILOG_ERROR("new ResDesc failed when LoadFromIndex");
        free(buf);
        return nullptr;
    }
    int32_t out = HapParser::ParseResHex(static_cast<char *>(buf), bufLen, *resDesc, defaultConfig);
    if (out != OK) {
        delete (resDesc);
        free(buf);
        HILOG_ERROR("ParseResHex failed! retcode:%d", out);
        return nullptr;
    } else {
        HILOG_DEBUG("ParseResHex success:\n%s", resDesc->ToString().c_str());
    }
    free(buf);

    HapResource *pResource = new (std::nothrow) HapResource(std::string(path), 0, defaultConfig, resDesc);
    if (pResource == nullptr) {
        HILOG_ERROR("new HapResource failed when LoadFromIndex");
        delete (resDesc);
        return nullptr;
    }
    if (!pResource->Init()) {
        delete (pResource);
        return nullptr;
    }
    return pResource;
}

bool HapResource::Init()
{
    auto index = indexPath_.rfind('/');
    if (index == std::string::npos) {
        HILOG_ERROR("index path format error, %s", indexPath_.c_str());
        return false;
    }
    index = indexPath_.rfind('/', index - 1);
    if (index == std::string::npos) {
        HILOG_ERROR("index path format error, %s", indexPath_.c_str());
        return false;
    }
    resourcePath_ = indexPath_.substr(0, index + 1);
    for (int i = 0; i < ResType::MAX_RES_TYPE; ++i) {
        auto mptr = new (std::nothrow) std::map<std::string, IdValues *>();
        if (mptr == nullptr) {
            HILOG_ERROR("new std::map failed in HapResource::Init");
            return false;
        }
        idValuesNameMap_.push_back(mptr);
    }
    return InitIdList();
}

bool HapResource::InitIdList()
{
    if (resDesc_ == nullptr) {
        HILOG_ERROR("resDesc_ is null ! InitIdList failed");
        return false;
    }
    for (size_t i = 0; i < resDesc_->keys_.size(); i++) {
        ResKey *resKey = resDesc_->keys_[i];

        for (size_t j = 0; j < resKey->resId_->idParams_.size(); ++j) {
            IdParam *idParam = resKey->resId_->idParams_[j];
            uint32_t id = idParam->id_;
            std::map<uint32_t, IdValues *>::iterator iter = idValuesMap_.find(id);
            if (iter == idValuesMap_.end()) {
                auto idValues = new (std::nothrow) HapResource::IdValues();
                if (idValues == nullptr) {
                    HILOG_ERROR("new IdValues failed in HapResource::InitIdList");
                    return false;
                }
                auto limitPath =
                    new (std::nothrow) HapResource::ValueUnderQualifierDir(resKey->keyParams_, idParam->idItem_, this);
                if (limitPath == nullptr) {
                    HILOG_ERROR("new ValueUnderQualifierDir failed in HapResource::InitIdList");
                    delete (idValues);
                    return false;
                }
                idValues->AddLimitPath(limitPath);
                idValuesMap_.insert(std::make_pair(id, idValues));
                std::string name = std::string(idParam->idItem_->name_);
                idValuesNameMap_[idParam->idItem_->resType_]->insert(std::make_pair(name, idValues));
            } else {
                HapResource::IdValues *idValues = iter->second;
                auto limitPath =
                    new (std::nothrow) HapResource::ValueUnderQualifierDir(resKey->keyParams_, idParam->idItem_, this);
                if (limitPath == nullptr) {
                    HILOG_ERROR("new ValueUnderQualifierDir failed in HapResource::InitIdList");
                    return false;
                }
                idValues->AddLimitPath(limitPath);
            }
        }
    }
    return true;
};

const HapResource::IdValues *HapResource::GetIdValues(const uint32_t id) const
{
    uint32_t uid = id;
    std::map<uint32_t, IdValues *>::const_iterator iter = idValuesMap_.find(uid);
    if (iter == idValuesMap_.end()) {
        return nullptr;
    }

    return iter->second;
}

const HapResource::IdValues *HapResource::GetIdValuesByName(
    const std::string name, const ResType resType) const
{
    const std::map<std::string, IdValues *> *map = idValuesNameMap_[resType];
    std::map<std::string, IdValues *>::const_iterator iter = map->find(name);
    if (iter == map->end()) {
        return nullptr;
    }

    return iter->second;
}

int HapResource::GetIdByName(const char *name, const ResType resType) const
{
    if (name == nullptr) {
        return -1;
    }
    const std::map<std::string, IdValues *> *map = idValuesNameMap_[resType];
    std::map<std::string, IdValues *>::const_iterator iter = map->find(name);
    if (iter == map->end()) {
        return OBJ_NOT_FOUND;
    }
    const IdValues *ids = iter->second;

    if (ids->GetLimitPathsConst().size() == 0) {
        HILOG_ERROR("limitPaths empty");
        return UNKNOWN_ERROR;
    }

    if (ids->GetLimitPathsConst()[0]->GetIdItem()->resType_ != resType) {
        HILOG_ERROR("ResType mismatch");
        return UNKNOWN_ERROR;
    }
    return ids->GetLimitPathsConst()[0]->GetIdItem()->id_;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
