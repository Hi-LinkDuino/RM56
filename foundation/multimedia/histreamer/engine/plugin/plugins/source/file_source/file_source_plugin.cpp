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

#define HST_LOG_TAG "FileSourcePlugin"

#include "file_source_plugin.h"
#include <sys/stat.h>
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FileSource {
namespace {
size_t GetFileSize(const std::string& fileName)
{
    size_t fileSize = 0;
    if (!fileName.empty()) {
        struct stat fileStatus {};
        if (stat(fileName.c_str(), &fileStatus) == 0) {
            fileSize = static_cast<size_t>(fileStatus.st_size);
        }
    }
    return fileSize;
}
}
std::shared_ptr<SourcePlugin> FileSourcePluginCreator(const std::string& name)
{
    return std::make_shared<FileSourcePlugin>(name);
}

Status FileSourceRegister(const std::shared_ptr<Register>& reg)
{
    SourcePluginDef definition;
    definition.name = "FileSource";
    definition.description = "File source";
    definition.rank = 100; // 100: max rank
    definition.protocol.emplace_back(ProtocolType::FILE);
    definition.creator = FileSourcePluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(FileSource, LicenseType::APACHE_V2, FileSourceRegister, [] {});

void* FileSourceAllocator::Alloc(size_t size)
{
    if (size == 0) {
        return nullptr;
    }
    return static_cast<void*>(new (std::nothrow) uint8_t[size]);
}

void FileSourceAllocator::Free(void* ptr) // NOLINT: void*
{
    if (ptr != nullptr) {
        delete[](uint8_t*) ptr;
    }
}

FileSourcePlugin::FileSourcePlugin(std::string name)
    : SourcePlugin(std::move(name)), fp_(nullptr), fileSize_(0), seekable_(Seekable::SEEKABLE), position_(0)
{
    MEDIA_LOG_D("IN");
}

FileSourcePlugin::~FileSourcePlugin()
{
    MEDIA_LOG_D("IN");
    if (fp_) {
        std::fclose(fp_);
        fp_ = nullptr;
    }
}

Status FileSourcePlugin::Init()
{
    MEDIA_LOG_D("IN");
    mAllocator_ = std::make_shared<FileSourceAllocator>();
    return Status::OK;
}

Status FileSourcePlugin::Deinit()
{
    MEDIA_LOG_D("IN");
    CloseFile();
    return Status::OK;
}

Status FileSourcePlugin::Prepare()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status FileSourcePlugin::Reset()
{
    MEDIA_LOG_D("IN");
    CloseFile();
    return Status::OK;
}

Status FileSourcePlugin::GetParameter(Tag tag, ValueType& value)
{
    MEDIA_LOG_D("IN");
    return Status::ERROR_UNIMPLEMENTED;
}

Status FileSourcePlugin::SetParameter(Tag tag, const ValueType& value)
{
    MEDIA_LOG_D("IN");
    return Status::ERROR_UNIMPLEMENTED;
}

std::shared_ptr<Allocator> FileSourcePlugin::GetAllocator()
{
    MEDIA_LOG_D("IN");
    return mAllocator_;
}

Status FileSourcePlugin::SetCallback(Callback* cb)
{
    MEDIA_LOG_D("IN");
    return Status::ERROR_UNIMPLEMENTED;
}

Status FileSourcePlugin::SetSource(std::shared_ptr<MediaSource> source)
{
    MEDIA_LOG_D("IN");
    auto err = ParseFileName(source->GetSourceUri());
    if (err != Status::OK) {
        MEDIA_LOG_E("Parse file name from uri fail, uri: " PUBLIC_LOG_S, source->GetSourceUri().c_str());
        return err;
    }
    return OpenFile();
}

Status FileSourcePlugin::Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen)
{
    if (std::feof(fp_)) {
        MEDIA_LOG_W("It is the end of file!");
        return Status::END_OF_STREAM;
    }
    if (buffer == nullptr) {
        buffer = std::make_shared<Buffer>();
    }

    std::shared_ptr<Memory> bufData;

    // There is no buffer, so alloc it
    if (buffer->IsEmpty()) {
        bufData = buffer->AllocMemory(GetAllocator(), expectedLen);
    } else {
        bufData = buffer->GetMemory();
    }
    expectedLen = std::min(static_cast<size_t>(fileSize_ - position_), expectedLen);
    expectedLen = std::min(bufData->GetCapacity(), expectedLen);

    MEDIA_LOG_DD("buffer position " PUBLIC_LOG_U64 ", expectedLen " PUBLIC_LOG_ZU, position_, expectedLen);
    auto size = std::fread(bufData->GetWritableAddr(expectedLen), sizeof(char), expectedLen, fp_);
    bufData->UpdateDataSize(size);
    position_ += bufData->GetSize();
    MEDIA_LOG_DD("position_: " PUBLIC_LOG_U64 ", readSize: " PUBLIC_LOG_ZU, position_, bufData->GetSize());
    return Status::OK;
}

Status FileSourcePlugin::GetSize(size_t& size)
{
    MEDIA_LOG_DD("IN");
    if (!fp_) {
        MEDIA_LOG_E("Need call SetSource() to open file first");
        return Status::ERROR_WRONG_STATE;
    }
    size = fileSize_;
    MEDIA_LOG_DD("fileSize_: " PUBLIC_LOG_ZU, size);
    return Status::OK;
}

Seekable FileSourcePlugin::GetSeekable()
{
    MEDIA_LOG_DD("IN");
    return seekable_;
}

Status FileSourcePlugin::SeekTo(uint64_t offset)
{
    if (!fp_ || (offset > fileSize_) || (position_ == offset)) {
        MEDIA_LOG_E("Invalid operation");
        return Status::ERROR_WRONG_STATE;
    }
    std::clearerr(fp_);
    if (std::fseek(fp_, static_cast<long int>(offset), SEEK_SET) != 0) {
        std::clearerr(fp_);
        (void)std::fseek(fp_, static_cast<long int>(position_), SEEK_SET);
        MEDIA_LOG_E("Seek to " PUBLIC_LOG_U64, offset);
        return Status::ERROR_UNKNOWN;
    }
    position_ = offset;
    if (std::feof(fp_)) {
        MEDIA_LOG_I("It is the end of file!");
    }
    MEDIA_LOG_D("seek to position_: " PUBLIC_LOG_U64 " success", position_);
    return Status::OK;
}

Status FileSourcePlugin::ParseFileName(const std::string& uri)
{
    if (uri.empty()) {
        MEDIA_LOG_E("uri is empty");
        return Status::ERROR_INVALID_PARAMETER;
    }
    MEDIA_LOG_D("uri: " PUBLIC_LOG_S, uri.c_str());
    if (uri.find("file:/") != std::string::npos) {
        if (uri.find('#') != std::string::npos) {
            MEDIA_LOG_E("Invalid file uri format: " PUBLIC_LOG_S, uri.c_str());
            return Status::ERROR_INVALID_PARAMETER;
        }
        auto pos = uri.find("file:");
        if (pos == std::string::npos) {
            MEDIA_LOG_E("Invalid file uri format: " PUBLIC_LOG_S, uri.c_str());
            return Status::ERROR_INVALID_PARAMETER;
        }
        pos += 5; // 5: offset
        if (uri.find("///", pos) != std::string::npos) {
            pos += 2; // 2: offset
        } else if (uri.find("//", pos) != std::string::npos) {
            pos += 2;                 // 2: offset
            pos = uri.find('/', pos); // skip host name
            if (pos == std::string::npos) {
                MEDIA_LOG_E("Invalid file uri format: " PUBLIC_LOG_S, uri.c_str());
                return Status::ERROR_INVALID_PARAMETER;
            }
            pos++;
        }
        fileName_ = uri.substr(pos);
    } else {
        fileName_ = uri;
    }
    MEDIA_LOG_D("fileName_: " PUBLIC_LOG_S, fileName_.c_str());
    return Status::OK;
}

Status FileSourcePlugin::CheckFileStat()
{
    struct stat fileStat;
    if (stat(fileName_.c_str(), &fileStat) < 0) {
        MEDIA_LOG_E("Cannot get info from " PUBLIC_LOG_S, fileName_.c_str());
        return Status::ERROR_NOT_EXISTED;
    }
    if (S_ISDIR(fileStat.st_mode)) {
        MEDIA_LOG_E(PUBLIC_LOG_S " is directory", fileName_.c_str());
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    if (S_ISSOCK(fileStat.st_mode)){
        MEDIA_LOG_E(PUBLIC_LOG_S " is a socket", fileName_.c_str());
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

Status FileSourcePlugin::OpenFile()
{
    MEDIA_LOG_D("IN");
    auto ret = CheckFileStat();
    if (ret != Status::OK) {
        CloseFile();
        return ret;
    }
    CloseFile();
    fp_ = std::fopen(fileName_.c_str(), "rb");
    if (fp_ == nullptr) {
        MEDIA_LOG_E("Fail to load file from " PUBLIC_LOG_S, fileName_.c_str());
        return Status::ERROR_UNKNOWN;
    }
    fileSize_ = GetFileSize(fileName_);
    position_ = 0;
    MEDIA_LOG_D("fileName_: " PUBLIC_LOG_S ", fileSize_: " PUBLIC_LOG_ZU, fileName_.c_str(), fileSize_);
    return Status::OK;
}

void FileSourcePlugin::CloseFile()
{
    if (fp_) {
        MEDIA_LOG_I("close file");
        std::fclose(fp_);
        fp_ = nullptr;
    }
}
} // namespace FileSource
} // namespace Plugin
} // namespace Media
} // namespace OHOS
