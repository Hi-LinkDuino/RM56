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
 
#ifndef HISTREAMER_MEDIA_DOWNLOADER_H
#define HISTREAMER_MEDIA_DOWNLOADER_H

#include <string>
#include "plugin/interface/plugin_base.h"
#include "download/downloader.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class MediaDownloader {
public:
    virtual ~MediaDownloader() = default;
    virtual bool Open(const std::string& url) = 0;
    virtual void Close() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual bool Read(unsigned char* buff, unsigned int wantReadLength, unsigned int& realReadLength, bool& isEos) = 0;
    virtual bool Seek(int offset) = 0;
    virtual size_t GetContentLength() const = 0;
    virtual double GetDuration() const = 0;
    virtual Seekable GetSeekable() const = 0;
    virtual void SetCallback(Callback* cb) = 0;
    virtual void SetStatusCallback(StatusCallbackFunc cb) = 0;
};
}
}
}
}
#endif