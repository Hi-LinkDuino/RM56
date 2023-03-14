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
#define HST_LOG_TAG "HttpLiteSourcePlugin"
#include "http_lite_source_plugin.h"
#include "foundation/log.h"
#include "osal/utils/util.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpLitePlugin {
namespace {
constexpr int DEFAULT_BUFFER_SIZE  = 200 * 1024;
constexpr int DEFAULT_HTTP_PRORITY = 32;
constexpr int8_t RANK_MAX          = 100;
}

std::shared_ptr<SourcePlugin> HttpSourcePluginCreater(const std::string &name)
{
    return std::make_shared<HttpSourcePlugin>(name);
}

const Status HttpSourceRegister(std::shared_ptr<Register> reg)
{
    SourcePluginDef definition;
    definition.name = "HttpLiteSource";
    definition.description = "Http lite source";
    definition.rank = RANK_MAX;
    definition.protocol.emplace_back(ProtocolType::HTTP);
    definition.protocol.emplace_back(ProtocolType::HTTPS);
    definition.protocol.emplace_back(ProtocolType::HLS);
    definition.creator = HttpSourcePluginCreater;
    return reg->AddPlugin(definition);
}
PLUGIN_DEFINITION(HttpLiteSource, LicenseType::APACHE_V2, HttpSourceRegister, [] {});

void* HttpSourceAllocator::Alloc(size_t size)
{
    if (size == 0) {
        return nullptr;
    }
    return reinterpret_cast<void*>(new (std::nothrow) uint8_t[size]); // NOLINT: cast
}

void HttpSourceAllocator::Free(void* ptr) // NOLINT: void*
{
    if (ptr != nullptr) {
        delete[](uint8_t*) ptr;
    }
}

HttpSourcePlugin::HttpSourcePlugin(const std::string name) noexcept
    : SourcePlugin(std::move(name)),
      url_(""),
      certFile_(""),
      needExit_(false),
      isStream_(false),
      bufferSize_(DEFAULT_BUFFER_SIZE),
      position_(0),
      waterline_(0),
      fileSize_(0),
      httpHandle_(nullptr),
      mAllocator_(nullptr),
      httpMutex_()
{
    MEDIA_LOG_I("HttpSourcePlugin IN");
}

HttpSourcePlugin::~HttpSourcePlugin()
{
    MEDIA_LOG_I("~HttpSourcePlugin IN");
}

Status HttpSourcePlugin::Init()
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("Init IN");
    httpHandle_ = std::make_shared<HttpLiteManager>();
    if (httpHandle_ == nullptr) {
        MEDIA_LOG_E("httpHandle_ create error");
        return Status::ERROR_UNKNOWN;
    }
    mAllocator_ = std::make_shared<HttpSourceAllocator>();
    if (mAllocator_ == nullptr) {
        MEDIA_LOG_E("mAllocator_ create error");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_D("Init OUT");
    return Status::OK;
}

Status HttpSourcePlugin::Deinit()
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    CloseUri();
    return Status::OK;
}

Status HttpSourcePlugin::Prepare()
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status HttpSourcePlugin::Reset()
{
    needExit_ = true;
    {
        OSAL::ScopedLock lock(httpMutex_);
        needExit_ = false;
        MEDIA_LOG_D("IN");
        CloseUri();
        return Status::OK;
    }
}

Status HttpSourcePlugin::Stop()
{
    needExit_ = true;
    {
        MEDIA_LOG_D("IN");
        OSAL::ScopedLock lock(httpMutex_);
        needExit_ = false;
        if (httpHandle_ != nullptr) {
            httpHandle_->HttpClose();
            httpHandle_ = nullptr;
        }
        MEDIA_LOG_D("OUT");
        return Status::ERROR_UNKNOWN;
    }
}

Status HttpSourcePlugin::GetParameter(Tag tag, ValueType &value)
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    switch (tag) {
        case Tag::BUFFERING_SIZE:
            value = bufferSize_;
            return Status::OK;
        case Tag::WATERLINE_HIGH:
            value = waterline_;
            return Status::OK;
        default:
            return Status::ERROR_INVALID_PARAMETER;
    }
}

Status HttpSourcePlugin::SetParameter(Tag tag, const ValueType &value)
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    switch (tag) {
        case Tag::BUFFERING_SIZE:
            bufferSize_ = AnyCast<uint32_t>(value);
            return Status::OK;
        case Tag::WATERLINE_HIGH:
            waterline_ = AnyCast<uint32_t>(value);
            return Status::OK;
        default:
            return Status::ERROR_INVALID_PARAMETER;
    }
}

Status HttpSourcePlugin::SetCallback(Callback* cb)
{
    MEDIA_LOG_D("IN");
    callback_ = cb;
    return Status::OK;
}

Status HttpSourcePlugin::SetSource(std::shared_ptr<MediaSource> source)
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("SetSource IN");
    if (httpHandle_ == nullptr) {
        MEDIA_LOG_E("httpHandle_ null error");
        return Status::ERROR_UNKNOWN;
    }
    auto uri = source->GetSourceUri();
    MEDIA_LOG_D(PUBLIC_LOG_S, uri.c_str());
    Status ret = OpenUri(uri);
    if (ret != Status::OK) {
        MEDIA_LOG_E("OpenUri error");
        return ret;
    }
    MEDIA_LOG_D("OpenUri success");
    unsigned int downloadPos = 0;
    httpHandle_->GetHttpBufferRange(&position_, &downloadPos);
    MEDIA_LOG_D("position_ " PUBLIC_LOG_U32 "downloadPos " PUBLIC_LOG_U32, position_, downloadPos);
    int8_t retryTimes = 0;
    while (!needExit_ && position_ == downloadPos && retryTimes < 60) { // 60
        OHOS::Media::OSAL::SleepFor(200); // 200
        httpHandle_->GetHttpBufferRange(&position_, &downloadPos);
        retryTimes++;
    }
    MEDIA_LOG_D("position_ " PUBLIC_LOG_U32 "downloadPos " PUBLIC_LOG_U32, position_, downloadPos);
    if (position_ == downloadPos) {
        MEDIA_LOG_D("position_ == downloadPos");
        httpHandle_->HttpClose();
        return Status::ERROR_UNKNOWN;
    }
    isStream_ = httpHandle_->IsStreaming();
    waterline_ = 20; // 20
    httpHandle_->SetWaterline(waterline_, 0);
    fileSize_ = isStream_ ? 0 : httpHandle_->GetContentLength();
    MEDIA_LOG_D("SetSource OUT fileSize_ " PUBLIC_LOG_ZU, fileSize_);
    return Status::OK;
}

std::shared_ptr<Allocator> HttpSourcePlugin::GetAllocator()
{
    MEDIA_LOG_D("GetAllocator IN");
    return mAllocator_;
}

void HttpSourcePlugin::OnError(int httpError, int localError, void *param, int support_retry)
{
    MEDIA_LOG_D("httpError " PUBLIC_LOG_D32 " localError " PUBLIC_LOG_D32, httpError, localError);
    auto plugin = reinterpret_cast<HttpSourcePlugin *>(param);
    if (plugin == nullptr) {
        return;
    }
    plugin->needExit_ = true;
    plugin->OnHttpEvent(param, httpError, localError);
}

Status HttpSourcePlugin::OnHttpEvent(void *priv, int errorType, int32_t errorCode)
{
    if (priv == nullptr) {
        MEDIA_LOG_D("priv null error");
        return Status::ERROR_UNKNOWN;
    }
    auto plugin = reinterpret_cast<HttpSourcePlugin *>(priv);
    plugin->callback_->OnEvent(
        PluginEvent{PluginEventType::OTHER_ERROR, errorCode, "http lite error"});
    return Status::OK;
}

Status HttpSourcePlugin::Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen)
{
    MEDIA_LOG_D("Read in");
    if (httpHandle_ == nullptr) {
        MEDIA_LOG_D("Read error");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (buffer == nullptr) {
        buffer = std::make_shared<Buffer>();
    }

    OSAL::ScopedLock lock(httpMutex_);
    std::shared_ptr<Memory> bufData;

    if (buffer->IsEmpty()) {
        bufData = buffer->AllocMemory(GetAllocator(), expectedLen);
    } else {
        bufData = buffer->GetMemory();
    }
    unsigned int read = 0;
    unsigned int write = 0;
    unsigned int realReadSize = 0;
    int isEos = -1;
    int retryReadTimes = 0;

    httpHandle_->GetHttpBufferRange(&read, &write);

    MEDIA_LOG_D("read pos " PUBLIC_LOG_U32 " write pos " PUBLIC_LOG_U32 " expectedLen " PUBLIC_LOG_ZU,
                read, write, expectedLen);

    do {
        MEDIA_LOG_D("bufData->GetCapacity() " PUBLIC_LOG_ZU, bufData->GetCapacity());
        httpHandle_->HttpRead(bufData->GetWritableAddr(expectedLen), expectedLen, realReadSize, isEos);
        if (realReadSize == 0 && retryReadTimes <= 200) { // 200
            OSAL::SleepFor(10); // 10
            retryReadTimes++;
            continue;
        }
        break;
    } while (true);
    bufData->UpdateDataSize(realReadSize);
    httpHandle_->GetHttpBufferRange(&position_, &write);
    MEDIA_LOG_D("position_ : " PUBLIC_LOG_U32 ", readSize = " PUBLIC_LOG_ZU ", isEos " PUBLIC_LOG_D32,
                position_, bufData->GetSize(), isEos);
    return Status::OK;
}

Status HttpSourcePlugin::GetSize(size_t &size)
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    size = fileSize_;
    return Status::OK;
}

Seekable HttpSourcePlugin::GetSeekable()
{
    OSAL::ScopedLock lock(httpMutex_);
    MEDIA_LOG_D("IN");
    return !isStream_ ? Seekable::SEEKABLE : Seekable::UNSEEKABLE;
}

Status HttpSourcePlugin::SeekTo(uint64_t offset)
{
    OSAL::ScopedLock lock(httpMutex_);
    unsigned int readPos = 0;
    unsigned int writePos = 0;
    if ((httpHandle_ == nullptr) || (isStream_) || (position_ == offset) || (offset > fileSize_)) {
        MEDIA_LOG_E("Invalid operation");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (!httpHandle_->HttpSeek(offset)) {
        MEDIA_LOG_E("seek to position_ " PUBLIC_LOG_U32 " failed", position_);
        return Status::ERROR_UNKNOWN;
    }
    position_ = static_cast<unsigned int>(offset);
    httpHandle_->GetHttpBufferRange(&readPos, &writePos);
    MEDIA_LOG_D("offset = " PUBLIC_LOG_U32 " , after SeekTo readPos = " PUBLIC_LOG_U32 ", writePos = " PUBLIC_LOG_U32,
        static_cast<uint32_t>(offset), readPos, writePos);
    MEDIA_LOG_D("seek to position_ " PUBLIC_LOG_U32 " success", position_);
    return Status::OK;
}

Status HttpSourcePlugin::OpenUri(std::string &url)
{
    MEDIA_LOG_I("OpenUri IN");
    if (httpHandle_ == nullptr) {
        return Status::ERROR_UNIMPLEMENTED;
    }
    httpHandle_->HttpClose();
    HttpLiteAttr httpAttr;
    httpAttr.certFile = certFile_;
    httpAttr.priority = DEFAULT_HTTP_PRORITY;
    httpAttr.bufferSize = bufferSize_;
    httpAttr.pluginHandle = this;
    httpAttr.callbackFunc = OnError;
    httpAttr.notVerifyCert = true;
    return httpHandle_->HttpOpen(url, httpAttr) ? Status::OK : Status::ERROR_UNKNOWN;
}

void HttpSourcePlugin::CloseUri()
{
    if (httpHandle_ != nullptr) {
        MEDIA_LOG_D("close uri");
        httpHandle_->HttpClose();
        httpHandle_ = nullptr;
    }
}
} // namespace HttpLitePlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS