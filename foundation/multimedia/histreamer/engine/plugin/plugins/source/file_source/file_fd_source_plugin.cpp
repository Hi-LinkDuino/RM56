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

#define HST_LOG_TAG "FileFdSourcePlugin"

#include "file_fd_source_plugin.h"
#include <cerrno>
#include <cstring>
#include <regex>
#ifdef WIN32
#include <fcntl.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include "foundation/log.h"
#include "foundation/osal/filesystem/file_system.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FileFdSource {
namespace {
uint64_t GetFileSize(int32_t fd)
{
    uint64_t fileSize = 0;
    struct stat s {};
    if (fstat(fd, &s) == 0) {
        fileSize = static_cast<uint64_t>(s.st_size);
        return fileSize;
    }
    return fileSize;
}
}
Status FileFdSourceRegister(const std::shared_ptr<Register>& reg)
{
    SourcePluginDef definition;
    definition.name = "FileFdSource";
    definition.description = "File Fd source";
    definition.rank = 100; // 100: max rank
    definition.protocol.emplace_back(ProtocolType::FD);
    definition.creator = [](const std::string& name) -> std::shared_ptr<SourcePlugin> {
        return std::make_shared<FileFdSourcePlugin>(name);
    };
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(FileFdSource, LicenseType::APACHE_V2, FileFdSourceRegister, [] {});

FileFdSourcePlugin::FileFdSourcePlugin(std::string name)
    : SourcePlugin(std::move(name))
{
}

Status FileFdSourcePlugin::SetCallback(Callback* cb)
{
    MEDIA_LOG_D("IN");
    return Status::ERROR_UNIMPLEMENTED;
}

Status FileFdSourcePlugin::SetSource(std::shared_ptr<MediaSource> source)
{
    MEDIA_LOG_D("IN");
    auto err = ParseUriInfo(source->GetSourceUri());
    if (err != Status::OK) {
        MEDIA_LOG_E("Parse file name from uri fail, uri: " PUBLIC_LOG "s", source->GetSourceUri().c_str());
        return err;
    }
    return err;
}

Status FileFdSourcePlugin::Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen)
{
    if (!buffer) {
        buffer = std::make_shared<Buffer>();
    }
    std::shared_ptr<Memory> bufData;
    if (buffer->IsEmpty()) {
        bufData = buffer->AllocMemory(GetAllocator(), expectedLen);
    } else {
        bufData = buffer->GetMemory();
    }
    expectedLen = std::min(static_cast<size_t>(fileSize_ - position_), expectedLen);
    expectedLen = std::min(bufData->GetCapacity(), expectedLen);
    MEDIA_LOG_DD("buffer position " PUBLIC_LOG_U64 ", expectedLen " PUBLIC_LOG_ZU, position_, expectedLen);
    auto size = read(fd_, bufData->GetWritableAddr(expectedLen), expectedLen);
    bufData->UpdateDataSize(size);
    position_ += bufData->GetSize();
    MEDIA_LOG_DD("position_: " PUBLIC_LOG_U64 ", readSize: " PUBLIC_LOG_ZU, position_, bufData->GetSize());
    return Status::OK;
}

Status FileFdSourcePlugin::GetSize(size_t& size)
{
    MEDIA_LOG_DD("IN");
    size = fileSize_;
    MEDIA_LOG_DD("fileSize_: " PUBLIC_LOG_ZU, size);
    return Status::OK;
}

Seekable FileFdSourcePlugin::GetSeekable()
{
    MEDIA_LOG_DD("IN");
    return seekable_;
}

Status FileFdSourcePlugin::SeekTo(uint64_t offset)
{
    FALSE_RETURN_V_MSG_E(fd_ != -1 && seekable_ == Seekable::SEEKABLE,
                         Status::ERROR_WRONG_STATE, "no valid fd or no seekable.");
    int32_t ret = lseek(fd_, offset, SEEK_SET);
    if (ret == -1) {
        MEDIA_LOG_E("seek to " PUBLIC_LOG_U64 " failed due to " PUBLIC_LOG_S, offset, strerror(errno));
        return Status::ERROR_UNKNOWN;
    }
    position_ = offset;
    MEDIA_LOG_D("now seek to " PUBLIC_LOG_D32, ret);
    return Status::OK;
}

// uri format:fd://xxxx?offset=xxxx&size=xxx or fd://xxxx
Status FileFdSourcePlugin::ParseUriInfo(const std::string& uri)
{
    if (uri.empty()) {
        MEDIA_LOG_E("uri is empty");
        return Status::ERROR_INVALID_PARAMETER;
    }
    MEDIA_LOG_D("uri: " PUBLIC_LOG_S, uri.c_str());
    std::smatch fdUriMatch;
    FALSE_RETURN_V_MSG_E(std::regex_match(uri, fdUriMatch, std::regex("^fd://(.*)?offset=(.*)&size=(.*)")) ||
        std::regex_match(uri, fdUriMatch, std::regex("^fd://(.*)")),
        Status::ERROR_INVALID_PARAMETER, "Invalid fd uri format: " PUBLIC_LOG_S, uri.c_str());
    fd_ = std::stoi(fdUriMatch[1].str()); // 1: sub match fd subscript
    FALSE_RETURN_V_MSG_E(fd_ != -1 && OSAL::FileSystem::IsRegularFile(fd_),
        Status::ERROR_INVALID_PARAMETER, "Invalid fd: " PUBLIC_LOG_D32, fd_);
    fileSize_ = GetFileSize(fd_);
    if (fdUriMatch.size() == 4) { // 4：4 sub match
        offset_ = std::stoll(fdUriMatch[2].str()); // 2: sub match offset subscript
        if (offset_ > fileSize_) {
            offset_ = fileSize_;
        }
        size_ = std::stoll(fdUriMatch[3].str()); // 3: sub match size subscript
        if ((offset_ + size_) > fileSize_) {
            size_ = fileSize_ - offset_;
        }
    } else {
        size_ = fileSize_;
    }
    position_ = offset_;
    seekable_ = OSAL::FileSystem::IsSeekable(fd_) ? Seekable::SEEKABLE : Seekable::UNSEEKABLE;
    if (seekable_ == Seekable::SEEKABLE) {
        NOK_LOG(SeekTo(offset_));
    }
    MEDIA_LOG_D("fd: " PUBLIC_LOG_D32 ", offset: " PUBLIC_LOG_D64 ", size: " PUBLIC_LOG_D64, fd_, offset_, size_);
    return Status::OK;
}
} // namespace FileFdSource
} // namespace Plugin
} // namespace Media
} // namespace OHOS

