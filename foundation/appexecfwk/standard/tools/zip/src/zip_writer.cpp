
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "zip_writer.h"

#include <stdio.h>

#include "app_log_wrapper.h"
#include "contrib/minizip/zip.h"
#include "directory_ex.h"
#include "zip_internal.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
// Numbers of pending entries that trigger writting them to the ZIP file.
constexpr size_t g_MaxPendingEntriesCount = 50;
const std::string SEPARATOR = "/";

#define CALLING_CALL_BACK(callback, result) \
    if (callback != nullptr) {              \
        callback(result);                   \
    }

bool AddFileContentToZip(zipFile zip_file, FilePath &file_path)
{
    APP_LOGI("%{public}s called", __func__);
    uint32_t num_bytes;
    char buf[kZipBufSize];
    if (!FilePathCheckValid(file_path.Value())) {
        APP_LOGI(
            "%{public}s called, filePath is invalid!!! file_path=%{public}s", __func__, file_path.Value().c_str());
        return false;
    }
    if (!FilePath::PathIsValid(file_path)) {
        APP_LOGI("!!! %{public}s called PathIsValid returns false !!!", __func__);
        return false;
    }

    FILE *fp = fopen(file_path.Value().c_str(), "rb");
    if (fp == nullptr) {
        APP_LOGI("%{public}s called, filePath to realPath failed! filePath:%{private}s ",
            __func__,
            file_path.Value().c_str());
        return false;
    }

    while (!feof(fp)) {
        num_bytes = fread(buf, 1, kZipBufSize, fp);
        if (num_bytes > 0) {
            if (zipWriteInFileInZip(zip_file, buf, num_bytes) != ZIP_OK) {
                APP_LOGI("%{public}s called, Could not write data to zip for path:%{private}s ",
                    __func__, file_path.Value().c_str());
                fclose(fp);
                fp = nullptr;
                return false;
            }
        } else {
            break;
        }
    }
    fclose(fp);
    fp = nullptr;
    return true;
}

bool OpenNewFileEntry(
    zipFile zip_file, FilePath &path, bool isDirectory, struct tm *lastModified, const OPTIONS &options)
{
    APP_LOGI("%{public}s called", __func__);
    std::string strPath = path.Value();

    if (isDirectory) {
        strPath += SEPARATOR;
    }
    
    return ZipOpenNewFileInZip(zip_file, strPath, options, lastModified);
}

bool CloseNewFileEntry(zipFile zip_file)
{
    APP_LOGI("%{public}s called", __func__);
    return zipCloseFileInZip(zip_file) == ZIP_OK;
}

bool AddFileEntryToZip(zipFile zip_file, FilePath &relativePath, FilePath &absolutePath, const OPTIONS &options)
{
    APP_LOGI("%{public}s called", __func__);

    struct tm *lastModified = GetCurrentSystemTime();
    if (lastModified == nullptr) {
        return false;
    }
    if (!OpenNewFileEntry(zip_file, relativePath, false, lastModified, options)) {
        return false;
    }
    bool success = AddFileContentToZip(zip_file, absolutePath);
    if (!CloseNewFileEntry(zip_file)) {
        APP_LOGI("!!! CloseNewFileEntry returnValule is false !!!");
        return false;
    }
    return success;
}

bool AddDirectoryEntryToZip(zipFile zip_file, FilePath &path, struct tm *lastModified, const OPTIONS &options)
{
    APP_LOGI("%{public}s called", __func__);
    return OpenNewFileEntry(zip_file, path, true, lastModified, options) && CloseNewFileEntry(zip_file);
}

}  // namespace

std::unique_ptr<ZipWriter> ZipWriter::CreateWithFd(PlatformFile zipFilefd, const FilePath &rootDir)
{
    APP_LOGI("%{public}s called", __func__);
    if (zipFilefd == kInvalidPlatformFile) {
        return nullptr;
    }

    zipFile zip_file = OpenFdForZipping(zipFilefd, APPEND_STATUS_CREATE);
    if (!zip_file) {
        APP_LOGI("%{public}s called, Couldn't create ZIP file for FD", __func__);
        return nullptr;
    }
    return std::unique_ptr<ZipWriter>(new (std::nothrow) ZipWriter(zip_file, rootDir));
}

// static
std::unique_ptr<ZipWriter> ZipWriter::Create(const FilePath &zip_file_path, const FilePath &rootDir)
{
    FilePath zipFilePath = zip_file_path;
    APP_LOGI("%{public}s called", __func__);
    if (zipFilePath.Value().empty()) {
        APP_LOGI("%{public}s called, Path is empty", __func__);
        return nullptr;
    }

    zipFile zip_file = OpenForZipping(zipFilePath.Value(), APPEND_STATUS_CREATE);
    if (!zip_file) {
        APP_LOGI("%{public}s called, Couldn't create ZIP file at path", __func__);
        return nullptr;
    }
    return std::unique_ptr<ZipWriter>(new (std::nothrow) ZipWriter(zip_file, rootDir));
}

ZipWriter::ZipWriter(zipFile zip_file, const FilePath &rootDir) : zipFile_(zip_file), rootDir_(rootDir)
{}

ZipWriter::~ZipWriter()
{
    pendingEntries_.clear();
}

bool ZipWriter::WriteEntries(const std::vector<FilePath> &paths, const OPTIONS &options, CALLBACK callback)
{
    APP_LOGI("%{public}s called", __func__);
    return AddEntries(paths, options, callback) && Close(options, callback);
}

bool ZipWriter::AddEntries(const std::vector<FilePath> &paths, const OPTIONS &options, CALLBACK callback)
{
    if (!zipFile_) {
        return false;
    }
    pendingEntries_.insert(pendingEntries_.end(), paths.begin(), paths.end());
    return FlushEntriesIfNeeded(false, options, callback);
}

bool ZipWriter::Close(const OPTIONS &options, CALLBACK callback)
{
    bool success = FlushEntriesIfNeeded(true, options, callback) && zipClose(zipFile_, nullptr) == ZIP_OK;
    zipFile_ = nullptr;
    return success;
}

bool ZipWriter::FlushEntriesIfNeeded(bool force, const OPTIONS &options, CALLBACK callback)
{
    if (pendingEntries_.size() < g_MaxPendingEntriesCount && !force) {
        return true;
    }
    std::string rootDir = rootDir_.Value();
    if (EndsWith(rootDir_.Value(), SEPARATOR)) {
        rootDir = rootDir.substr(0, rootDir.size() - 1);
    }
    while (pendingEntries_.size() >= g_MaxPendingEntriesCount || (force && !pendingEntries_.empty())) {

        size_t entry_count = std::min(pendingEntries_.size(), g_MaxPendingEntriesCount);
        std::vector<FilePath> relativePaths;
        std::vector<FilePath> absolutePaths;

        relativePaths.insert(relativePaths.begin(), pendingEntries_.begin(), pendingEntries_.begin() + entry_count);
        for (auto iter = pendingEntries_.begin(); iter != pendingEntries_.begin() + entry_count; ++iter) {
            // The FileAccessor requires absolute paths.
            if (FilePath::IsDir(rootDir_)) {
                absolutePaths.push_back(FilePath(rootDir_.Value() + iter->Value()));
            } else {
                absolutePaths.push_back(FilePath(rootDir_.Value()));
            }
        }
        pendingEntries_.erase(pendingEntries_.begin(), pendingEntries_.begin() + entry_count);
        for (size_t i = 0; i < absolutePaths.size(); i++) {
            FilePath &relativePath = relativePaths[i];
            FilePath &absolutePath = absolutePaths[i];
            bool isValid = FilePath::PathIsValid(absolutePath);
            bool isDir = FilePath::IsDir(absolutePath);
            if (isValid && !isDir) {
                if (!AddFileEntryToZip(zipFile_, relativePath, absolutePath, options)) {
                    CALLING_CALL_BACK(callback, ERROR_CODE_ERRNO)
                    APP_LOGI("%{public}s called, Failed to write file", __func__);
                    return false;
                }
            } else {
                // Missing file or directory case.
                struct tm *last_modified = GetCurrentSystemTime();
                if (!AddDirectoryEntryToZip(zipFile_, relativePath, last_modified, options)) {
                    CALLING_CALL_BACK(callback, ERROR_CODE_ERRNO)
                    APP_LOGI("%{public}s called, Failed to write directory", __func__);
                    return false;
                }
            }
        }
    }
    CALLING_CALL_BACK(callback, ERROR_CODE_OK)
    return true;
}

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
