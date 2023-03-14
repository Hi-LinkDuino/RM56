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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MEDIA_QUERY_MEDIA_QUERY_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MEDIA_QUERY_MEDIA_QUERY_INFO_H

#include <string>

#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "base/utils/system_properties.h"

namespace OHOS::Ace::Framework {

class ACE_FORCE_EXPORT_WITH_PREVIEW MediaQueryInfo : public AceType {
    DECLARE_ACE_TYPE(MediaQueryInfo, AceType);

public:
    MediaQueryInfo() = default;
    ~MediaQueryInfo() override = default;

    static std::unique_ptr<JsonValue> GetMediaQueryJsonInfo();
    std::string GetMediaQueryInfo() const;

    static std::string GetDeviceType();

    static std::string GetOrientation();

    bool GetIsInit() const
    {
        return isInit_;
    }

    void SetIsInit(bool init)
    {
        isInit_ = init;
    }

    void SetListenerId(const std::string& listenerId)
    {
        listenerId_ = listenerId;
        backupListenerId_ = listenerId;
    }

    const std::string& GetListenerId() const
    {
        return listenerId_;
    }

    // reset listenerId when it was consumed to handle multi-page case.
    void ResetListenerId()
    {
        listenerId_.clear();
    }

    // ensure listenerId is valid to handle split screen case.
    void EnsureListenerIdValid()
    {
        listenerId_ = backupListenerId_;
    }

private:
    bool isInit_ = true;
    std::string listenerId_;
    // backupListenerId_ is used for backup listenerId_ to handle split screen case.
    // under the circumstances, the viewsizechanged event will be triggered, but the
    // method of addListener will not run, so the listenerId is empty. this backup
    // can ensure listenerId_ is valid when screen is split.
    std::string backupListenerId_;

    ACE_DISALLOW_COPY_AND_MOVE(MediaQueryInfo);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MEDIA_QUERY_MEDIA_QUERY_INFO_H
