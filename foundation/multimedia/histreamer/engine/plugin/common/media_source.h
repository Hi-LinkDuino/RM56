/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_MEDIA_SOURCE_H
#define HISTREAMER_PLUGIN_MEDIA_SOURCE_H

#include <map>
#include <memory>
#include "plugin_buffer.h"
#include "plugin_types.h" // NOLINT: used it
#include "data_stream.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief Unified enumerates media source types.
 *
 * @since 1.0
 * @version 1.0
 */
enum class SourceType : int32_t {
    /** Local file path or network address */
    SOURCE_TYPE_URI = 0,
    /** Local file descriptor */
    SOURCE_TYPE_FD,
    /** Stream data */
    SOURCE_TYPE_STREAM,
};

class MediaSource {
public:
    /// Construct an a specified URI.
    explicit MediaSource(std::string uri);

    explicit MediaSource(std::shared_ptr<DataConsumer> dataStream);

    MediaSource(std::string uri, std::map<std::string, std::string> header);

    /// Destructor
    virtual ~MediaSource() = default;

    /// Obtains the source type.
    SourceType GetSourceType() const;

    /// Obtains the media source URI.
    const std::string &GetSourceUri() const;

    const std::map<std::string, std::string> &GetSourceHeader() const;

    std::shared_ptr<DataConsumer> GetDataConsumer() const;

private:
    std::string uri_ {};
    SourceType type_ {};
    std::map<std::string, std::string> header_ {};
    std::shared_ptr<DataConsumer> dataConsumer_ {};
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif