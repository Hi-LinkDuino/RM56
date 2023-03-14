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

#include "adapter_device_config.h"

#include <fstream>

#include "log.h"
#include "xml_parse.h"

namespace OHOS {
namespace bluetooth {
AdapterDeviceConfig *AdapterDeviceConfig::g_instance = nullptr;

struct AdapterDeviceConfig::impl {
    utility::XmlParse parse_ {};
    std::string fileName_ {"bt_device_config.xml"};
    std::string filePath_ {BT_CONFIG_PATH + fileName_};
    std::string fileBasePath_ {BT_CONFIG_PATH_BASE + fileName_};
};

IAdapterDeviceConfig *AdapterDeviceConfig::GetInstance()
{
    if (g_instance == nullptr) {
        static AdapterDeviceConfig instance;
        g_instance = &instance;
    }

    return static_cast<IAdapterDeviceConfig *>(g_instance);
}

AdapterDeviceConfig::AdapterDeviceConfig() : pimpl(std::make_unique<impl>()){};

AdapterDeviceConfig::~AdapterDeviceConfig()
{}

bool AdapterDeviceConfig::Load()
{
    std::lock_guard<std::mutex> lg(mutex_);
    if (pimpl->parse_.Load(pimpl->filePath_)) {
        return true;
    } else {
        if (!Reload()) {
            return false;
        }
        return pimpl->parse_.Load(pimpl->filePath_);
    }
}

bool AdapterDeviceConfig::Reload()
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

bool AdapterDeviceConfig::Save()
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.Save();
}

bool AdapterDeviceConfig::SetValue(const std::string &section, const std::string &property, const int &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.SetValue(section, property, value);
}

bool AdapterDeviceConfig::SetValue(const std::string &section, const std::string &property, const std::string &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.SetValue(section, property, value);
}

bool AdapterDeviceConfig::GetValue(const std::string &section, const std::string &property, int &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetValue(section, property, value);
}

bool AdapterDeviceConfig::GetValue(const std::string &section, const std::string &property, std::string &value)
{
    std::lock_guard<std::mutex> lg(mutex_);

    return pimpl->parse_.GetValue(section, property, value);
}

bool AdapterDeviceConfig::GetValue(const std::string &section, const std::string &property, bool &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetValue(section, property, value);
}

bool AdapterDeviceConfig::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const int &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.SetValue(section, subSection, property, value);
}
bool AdapterDeviceConfig::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const std::string &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.SetValue(section, subSection, property, value);
}

bool AdapterDeviceConfig::SetValue(
    const std::string &section, const std::string &subSection, const std::string &property, const bool &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.SetValue(section, subSection, property, value);
}

bool AdapterDeviceConfig::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, int &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetValue(section, subSection, property, value);
}

bool AdapterDeviceConfig::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, std::string &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetValue(section, subSection, property, value);
}

bool AdapterDeviceConfig::GetValue(
    const std::string &section, const std::string &subSection, const std::string &property, bool &value)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetValue(section, subSection, property, value);
}

bool AdapterDeviceConfig::GetSubSections(const std::string &section, std::vector<std::string> &subSections)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.GetSubSections(section, subSections);
}

bool AdapterDeviceConfig::RemoveSection(const std::string &section, const std::string &subSection)
{
    std::lock_guard<std::mutex> lg(mutex_);
    return pimpl->parse_.RemoveSection(section, subSection);
}
}  // namespace bluetooth
}  // namespace OHOS