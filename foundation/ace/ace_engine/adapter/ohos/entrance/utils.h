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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_UTILS_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_UTILS_H

#include <string>

#include "base/log/log.h"

namespace OHOS::Ace {

inline bool GetIsArkFromConfig(const std::string& packagePathStr)
{
    auto configPath = packagePathStr + std::string("config.json");
    char realPath[PATH_MAX] = { 0x00 };
    if (realpath(configPath.c_str(), realPath) == nullptr) {
        LOGE("realpath fail! filePath: %{private}s, fail reason: %{public}s", configPath.c_str(), strerror(errno));
        LOGE("return not arkApp.");
        return false;
    }
    std::unique_ptr<FILE, decltype(&fclose)> file(fopen(realPath, "rb"), fclose);
    if (!file) {
        LOGE("open file failed, filePath: %{private}s, fail reason: %{public}s", configPath.c_str(), strerror(errno));
        LOGE("return not arkApp.");
        return false;
    }
    if (std::fseek(file.get(), 0, SEEK_END) != 0) {
        LOGE("seek file tail error, return not arkApp.");
        return false;
    }

    int64_t size = std::ftell(file.get());
    if (size == -1L) {
        return false;
    }
    char *fileData = new (std::nothrow) char[size];
    if (fileData == nullptr) {
        LOGE("new json buff failed, return not arkApp.");
        return false;
    }
    rewind(file.get());
    std::unique_ptr<char[]> jsonStream(fileData);
    size_t result = std::fread(jsonStream.get(), 1, size, file.get());
    if (result != (size_t)size) {
        LOGE("read file failed, return not arkApp.");
        return false;
    }

    std::string jsonString(jsonStream.get(), jsonStream.get() + size);
    auto rootJson = JsonUtil::ParseJsonString(jsonString);
    auto module = rootJson->GetValue("module");
    auto distro = module->GetValue("distro");
    std::string virtualMachine = distro->GetString("virtualMachine");
    return virtualMachine.find("ark") != std::string::npos;
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_ENTRANCE_UTILS_H
