/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "base/log/log.h"
#include "base/network/download_manager.h"
#include "base/utils/singleton.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

class MockDownloadManager final : public DownloadManager, public Singleton<MockDownloadManager> {
    DECLARE_SINGLETON(MockDownloadManager);
    ACE_DISALLOW_MOVE(MockDownloadManager);

public:
    bool Download(const std::string& url, std::vector<uint8_t>& dataOut) override
    {
        return false;
    }
};

MockDownloadManager::MockDownloadManager() = default;

MockDownloadManager::~MockDownloadManager() = default;

} // namespace

DownloadManager& DownloadManager::GetInstance()
{
    return Singleton<MockDownloadManager>::GetInstance();
}

} // namespace OHOS::Ace
