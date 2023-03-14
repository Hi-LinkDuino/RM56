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
#define HST_LOG_TAG "FilePathSinkPlugin"
#include "file_path_sink_plugin.h"
#include "foundation/log.h"
#include "foundation/osal/filesystem/file_system.h"
#include "pipeline/filters/common/plugin_utils.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace FileSink {
std::shared_ptr<OutputSinkPlugin> FilePathSinkPluginCreator(const std::string& name)
{
    return std::make_shared<FilePathSinkPlugin>(name);
}

Status FilePathSinkRegister(const std::shared_ptr<Register>& reg)
{
    OutputSinkPluginDef definition;
    definition.protocolType = ProtocolType::FILE;
    definition.name = "file_path_sink";
    definition.description = "file path sink";
    definition.rank = 100; // 100
    definition.protocolType = ProtocolType::FILE;
    definition.creator = FilePathSinkPluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(FilePathSink, LicenseType::APACHE_V2, FilePathSinkRegister, [] {});

FilePathSinkPlugin::FilePathSinkPlugin(std::string name)
    : OutputSinkPlugin(std::move(name)), fp_(nullptr), seekable_(Seekable::SEEKABLE)
{
}

FilePathSinkPlugin::~FilePathSinkPlugin()
{
    if (fp_) {
        MEDIA_LOG_W("close file in dtor");
        std::fclose(fp_);
        fp_ = nullptr;
    }
}

Status FilePathSinkPlugin::Stop()
{
    CloseFile();
    return Status::OK;
}

Status FilePathSinkPlugin::SetSink(const MediaSink& sink)
{
    FALSE_RETURN_V((sink.GetProtocolType() == ProtocolType::FILE && !sink.GetPath().empty()),
        Status::ERROR_INVALID_DATA);
    fileName_ = sink.GetPath();
    return OpenFile();
}

Seekable FilePathSinkPlugin::GetSeekable()
{
    return seekable_;
}

Status FilePathSinkPlugin::SeekTo(uint64_t offset)
{
    FALSE_RETURN_V_MSG_E(fp_ != nullptr, Status::ERROR_WRONG_STATE, "no files have been opened.");
    if (std::fseek(fp_, offset, SEEK_SET) == 0) {
        return Status::OK;
    } else {
        MEDIA_LOG_W("Seek to " PUBLIC_LOG_U64 " failed due to " PUBLIC_LOG_S, offset, strerror(errno));
        std::clearerr(fp_);
    }
    return Status::ERROR_UNKNOWN;
}

Status FilePathSinkPlugin::Write(const std::shared_ptr<Buffer>& buffer)
{
    MEDIA_LOG_DD("Write begin");
    if (buffer == nullptr || buffer->IsEmpty()) {
        return Status::OK;
    }
    auto bufferData = buffer->GetMemory();
    std::fwrite(bufferData->GetReadOnlyData(), bufferData->GetSize(), 1, fp_);
    return Status::OK;
}

Status FilePathSinkPlugin::Flush()
{
    if (fp_) {
        MEDIA_LOG_I("flush file");
        std::fflush(fp_);
    }
    return Status::OK;
}

Status FilePathSinkPlugin::Reset()
{
    return Status::OK;
}
Status FilePathSinkPlugin::OpenFile()
{
    tmpFileName_ = OSAL::FileSystem::GetTmpFileName();
    fp_ = std::fopen(tmpFileName_.c_str(), "wb");
    if (fp_ == nullptr) {
        int32_t err = errno;
        MEDIA_LOG_E("Fail to open file due to " PUBLIC_LOG_S, strerror(err));
        switch (err) {
            case EPERM:
                return Status::ERROR_PERMISSION_DENIED;
            case ENOENT:
                return Status::ERROR_NOT_EXISTED;
            default:
                return Status::ERROR_UNKNOWN;
        }
    }
    MEDIA_LOG_D("open file %s", tmpFileName_.c_str()); // file name is privacy
    return Status::OK;
}

void FilePathSinkPlugin::CloseFile()
{
    if (fp_) {
        MEDIA_LOG_D("close file");
        std::fclose(fp_);
        fp_ = nullptr;
        std::rename(tmpFileName_.c_str(), fileName_.c_str());
    }
}
} // namespace FileSink
} // namespace Plugin
} // namespace Media
} // namespace OHOS