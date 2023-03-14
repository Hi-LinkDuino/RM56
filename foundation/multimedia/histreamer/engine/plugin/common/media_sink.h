/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_MEDIA_SINK_H
#define HISTREAMER_PLUGIN_MEDIA_SINK_H

#include <string>
#include "plugin_tags.h"
namespace OHOS {
namespace Media {
namespace Plugin {
class MediaSink {
public:
    explicit MediaSink(ProtocolType protocolType);
    virtual ~MediaSink() = default;
    void SetFd(const int32_t fd);
    void SetPath(const std::string& path);
    ProtocolType GetProtocolType() const;
    std::string GetPath() const;
    int32_t GetFd() const;
private:
    std::string path_ {};
    int32_t fd_ {-1};
    ProtocolType protocolType_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif