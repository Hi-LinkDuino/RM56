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

#include "adapter_config.h"

#include <fstream>

#include "xml_parse.h"

namespace OHOS {
namespace bluetooth {
struct AdapterConfig::impl {
    utility::XmlParse parse_ = {};
    std::string fileName_ = {"bt_config.xml"};
    std::string filePath_ = {BT_CONFIG_PATH + fileName_};
    std::string fileBasePath_ = {BT_CONFIG_PATH_BASE + fileName_};
};

IAdapterConfig *AdapterConfig::GetInstance()
{
    static AdapterConfig instance;
    return reinterpret_cast<IAdapterConfig *>(&instance);
}

AdapterConfig::AdapterConfig() : pimpl(std::make_unique<impl>())
{}

AdapterConfig::~AdapterConfig()
{}

bool AdapterConfig::Load()
{
    if (pimpl->parse_.Load(pimpl->filePath_)) {
        return true;
    } else {
        if (!Reload()) {
            return false;
        }
        return pimpl->parse_.Load(pimpl->filePath_);
    }
}

bool AdapterConfig::Reload()
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

bool AdapterConfig::GetValue(const std::string &section, const std::string &property, int &value)
{
    return pimpl->parse_.GetValue(section, property, value);
}
bool AdapterConfig::GetValue(const std::string &section, const std::string &property, std::string &value)
{
    return pimpl->parse_.GetValue(section, property, value);
}
bool AdapterConfig::GetValue(const std::string &section, const std::string &property, bool &value)
{
    return pimpl->parse_.GetValue(section, property, value);
}
bool AdapterConfig::HasSection(const std::string &section)
{
    return pimpl->parse_.HasSection(section);
}
}  // namespace bluetooth
}  // namespace OHOS