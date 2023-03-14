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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_ROUTER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_ROUTER_H

#include <list>
#include <string>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"

namespace OHOS::Ace::Framework {

class ManifestRouter : public Referenced {
public:
    ManifestRouter() = default;
    ~ManifestRouter() override = default;

    std::string GetPagePath(const std::string& uri, const std::string& suffix = ".js") const;
    std::string GetEntry(const std::string& suffix = ".js") const;
    const std::list<std::string>& GetPageList();
    void RouterParse(const std::unique_ptr<JsonValue>& router);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void InsertPageList(const std::string& uri);
#endif

private:
    std::string pagePath_;
    std::list<std::string> pages_;

    ACE_DISALLOW_COPY_AND_MOVE(ManifestRouter);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_ROUTER_H
