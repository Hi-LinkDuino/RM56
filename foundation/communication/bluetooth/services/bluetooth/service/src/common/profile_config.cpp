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

#include "profile_config.h"

#include <fstream>
#include <mutex>

#include "xml_parse.h"

namespace OHOS {
namespace bluetooth {
struct ProfileConfig::impl {
    utility::XmlParse parse_ = {};
    std::mutex mutex_ = {};
    std::string fileName_ = {"bt_profile_config.xml"};
    std::string filePath_ = {BT_CONFIG_PATH + fileName_};
    std::string fileBasePath_ = {BT_CONFIG_PATH_BASE + fileName_};
};

IProfileConfig *ProfileConfig::GetInstance()
{
    static ProfileConfig instance;
    return reinterpret_cast<IProfileConfig *>(&instance);
}

ProfileConfig::ProfileConfig() : pimpl(std::make_unique<impl>()){};

ProfileConfig::~ProfileConfig(){};

bool ProfileConfig::Load()
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    if (pimpl->parse_.Load(pimpl->filePath_)) {
        return true;
    } else {
        if (!Reload()) {
            return false;
        }
        return pimpl->parse_.Load(pimpl->filePath_);
    }
}

bool ProfileConfig::Reload()
{
    std::ifstream fin(pimpl->fileBasePath_, std::ios::in | std::ios::binary);
    if (!fin) {
        return false;
    }
    std::ofstream fout(pimpl->filePath_, std::ios::out | std::ios::trunc);
    if (!fout) {
        return false;
    }
    fout << fin.rdbuf();
    return true;
}

bool ProfileConfig::GetValue(
    const std::string &addr, const std::string &section, const std::string &property, int &value)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    return pimpl->parse_.GetValue(addr, section, property, value);
}

bool ProfileConfig::GetValue(
    const std::string &addr, const std::string &section, const std::string &property, bool &value)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    return pimpl->parse_.GetValue(addr, section, property, value);
}

bool ProfileConfig::SetValue(
    const std::string &addr, const std::string &section, const std::string &property, int &value)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    if (!pimpl->parse_.SetValue(addr, section, property, value)) {
        return false;
    }
    return pimpl->parse_.Save();
}

bool ProfileConfig::SetValue(
    const std::string &addr, const std::string &section, const std::string &property, bool &value)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    if (!pimpl->parse_.SetValue(addr, section, property, value)) {
        return false;
    }
    return pimpl->parse_.Save();
}

bool ProfileConfig::RemoveAddr(const std::string &addr)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    if (!pimpl->parse_.RemoveSection(addr)) {
        return false;
    }
    return pimpl->parse_.Save();
}

bool ProfileConfig::RemoveProperty(const std::string &addr, const std::string &section, const std::string &property)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    if (!pimpl->parse_.RemoveProperty(addr, section, property)) {
        return false;
    }
    return pimpl->parse_.Save();
}

bool ProfileConfig::HasSection(const std::string &addr, const std::string &section)
{
    std::lock_guard<std::mutex> lock(pimpl->mutex_);
    return pimpl->parse_.HasSection(addr, section);
}
}  // namespace bluetooth
}  // namespace OHOS