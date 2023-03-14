/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <sys/stat.h>
#include <unistd.h>

#include "base/resource/data_provider_manager.h"

namespace OHOS::Ace {

std::unique_ptr<DataProviderRes> DataProviderManager::GetDataProviderResFromUri(const std::string& uriStr)
{
    LOGD("DataProviderManager::GetDataProviderResFromUri start uri: %{private}s", uriStr.c_str());
    if (platformImpl_) {
        return platformImpl_(uriStr);
    }
    return nullptr;
}

std::unique_ptr<DataProviderRes> DataProviderManagerStandard::GetDataProviderResFromUri(const std::string& uriStr)
{
    LOGD("DataProviderManagerStandard::GetDataProviderResFromUri start uri: %{private}s", uriStr.c_str());
    InitHelper();
    if (!helper_) {
        LOGE("data ability helper is null");
        return nullptr;
    }
    auto fd = helper_->OpenFile(uriStr, "r");
    if (fd == -1) {
        LOGE("file descriptor is not valid");
        return nullptr;
    }
    
    // get size of file.
    struct stat statBuf;
    auto statRes = fstat(fd, &statBuf);
    if (statRes != 0) {
        LOGE("get stat fail");
        close(fd);
        return nullptr;
    }
    auto size = statBuf.st_size;

    // read file content.
    std::vector<uint8_t> buffer(size);
    auto readRes = read(fd, buffer.data(), size);
    close(fd);
    if (readRes == -1) {
        LOGE("read file fail");
        return nullptr;
    }

    auto result = std::make_unique<DataProviderRes>(std::move(buffer));
    return result;
}

void DataProviderManagerStandard::InitHelper()
{
    if (!helper_ && dataAbilityHelperImpl_) {
        helper_ = dataAbilityHelperImpl_();
    }
}

} // namespace OHOS::Ace
