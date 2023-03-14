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

#include "zip_reader.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <utility>

#include "app_log_wrapper.h"
#include "checked_cast.h"
#include "contrib/minizip/unzip.h"
#include "string_ex.h"
#include "zip_internal.h"
#include "zip_utils.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {

// The implementation assumes that file names in zip files
// are encoded in UTF-8. This is true for zip files created by Zip()
// function in zip.h, but not true for user-supplied random zip files.
ZipReader::EntryInfo::EntryInfo(const std::string &fileNameInZip, const unz_file_info &rawFileInfo)
    : filePath_(FilePath::FromUTF8Unsafe(fileNameInZip)), isDirectory_(false), isUnsafe_(false), isEncrypted_(false)
{
    originalSize_ = rawFileInfo.uncompressed_size;

    // Directory entries in zip files end with "/".
    isDirectory_ = EndsWith(fileNameInZip, "/");

    // Check the file name here for directory traversal issues.
    isUnsafe_ = filePath_.ReferencesParent();

    // We also consider that the file name is unsafe, if it's absolute.
    // On Windows, IsAbsolute() returns false for paths starting with "/".
    if (filePath_.IsAbsolute() || StartsWith(fileNameInZip, "/")) {
        isUnsafe_ = false;
    }

    // Whether the file is encrypted is bit 0 of the flag.
    isEncrypted_ = rawFileInfo.flag & 1;

    // Construct the last modified time. The timezone info is not present in
    // zip files, so we construct the time as local time.
    if (GetCurrentSystemTime() != nullptr) {
        lastModified_ = *GetCurrentSystemTime();
    }
}

ZipReader::ZipReader()
{
    Reset();
}

ZipReader::~ZipReader()
{
    Close();
}

bool ZipReader::Open(FilePath &zipFilePath)
{
    if (zipFile_ != nullptr) {
        return false;
    }

    // Use of "Unsafe" function does not look good, but there is no way to do
    // this safely on Linux. See file_util.h for details.
    std::string zipfile = zipFilePath.Value();
    zipFile_ = OpenForUnzipping(zipfile);
    if (zipFile_ == nullptr) {
        return false;
    }

    return OpenInternal();
}

bool ZipReader::OpenFromPlatformFile(PlatformFile zipFd)
{
    if (zipFile_ != nullptr) {
        return false;
    }
    zipFile_ = OpenFdForUnzipping(zipFd);
    if (!zipFile_) {
        return false;
    }

    return OpenInternal();
}

bool ZipReader::OpenFromString(const std::string &data)
{
    zipFile_ = PrepareMemoryForUnzipping(data);
    if (!zipFile_) {
        return false;
    }

    return OpenInternal();
}

void ZipReader::Close()
{
    if (zipFile_) {
        unzClose(zipFile_);
    }
    Reset();
}

bool ZipReader::HasMore()
{
    return !reachedEnd_;
}

bool ZipReader::AdvanceToNextEntry()
{
    if (zipFile_ == nullptr) {
        return false;
    }
    // Should not go further if we already reached the end.
    if (reachedEnd_) {
        return false;
    }
    unz_file_pos position = {};
    if (unzGetFilePos(zipFile_, &position) != UNZ_OK) {
        return false;
    }

    const int currentEntryIndex = position.num_of_file;
    // If we are currently at the last entry, then the next position is the
    // end of the zip file, so mark that we reached the end.
    if (currentEntryIndex + 1 == numEntries_) {
        reachedEnd_ = true;
    } else {
        if (unzGoToNextFile(zipFile_) != UNZ_OK) {
            return false;
        }
    }
    currentEntryInfo_.reset();
    return true;
}

bool ZipReader::OpenCurrentEntryInZip()
{
    if (zipFile_ == nullptr) {
        return false;
    }

    unz_file_info raw_file_info = {};
    char raw_file_name_in_zip[kZipMaxPath] = {};

    const int result = unzGetCurrentFileInfo(zipFile_,
        &raw_file_info,
        raw_file_name_in_zip,
        sizeof(raw_file_name_in_zip) - 1,
        NULL,  // extraField.
        0,     // extraFieldBufferSize.
        NULL,  // szComment.
        0);    // commentBufferSize.

    if (result != UNZ_OK) {
        return false;
    }
    if (raw_file_name_in_zip[0] == '\0') {
        return false;
    }
    currentEntryInfo_.reset(new EntryInfo(std::string(raw_file_name_in_zip), raw_file_info));
    return true;
}

bool ZipReader::ExtractCurrentEntry(WriterDelegate *delegate, uint64_t numBytesToExtract) const
{
    if ((zipFile_ == nullptr) || (delegate == nullptr)) {
        return false;
    }
    const int openResult = unzOpenCurrentFile(zipFile_);
    if (openResult != UNZ_OK) {
        return false;
    }
    if (!delegate->PrepareOutput()) {
        return false;
    }
    auto buf = std::make_unique<char[]>(kZipBufSize);
    uint64_t remainingCapacity = numBytesToExtract;
    bool entirefileextracted = false;

    while (remainingCapacity > 0) {
        const int numBytesRead = unzReadCurrentFile(zipFile_, buf.get(), kZipBufSize);
        if (numBytesRead == 0) {
            entirefileextracted = true;
            break;
        } else if (numBytesRead < 0) {
            // If numBytesRead < 0, then it's a specific UNZ_* error code.
            break;
        } else if (numBytesRead > 0) {
            uint64_t numBytesToWrite = std::min<uint64_t>(remainingCapacity, checked_cast<uint64_t>(numBytesRead));
            if (!delegate->WriteBytes(buf.get(), numBytesToWrite)) {
                break;
            }
            if (remainingCapacity == checked_cast<uint64_t>(numBytesRead)) {
                // Ensures function returns true if the entire file has been read.
                entirefileextracted = (unzReadCurrentFile(zipFile_, buf.get(), 1) == 0);
            }
            if (remainingCapacity >= numBytesToWrite) {
                remainingCapacity -= numBytesToWrite;
            }
        }
    }

    unzCloseCurrentFile(zipFile_);
    // closeFile
    delegate->SetTimeModified(GetCurrentSystemTime());

    return entirefileextracted;
}

bool ZipReader::OpenInternal()
{
    if (zipFile_ == nullptr) {
        return false;
    }

    unz_global_info zipInfo = {};  // Zero-clear.
    if (unzGetGlobalInfo(zipFile_, &zipInfo) != UNZ_OK) {
        return false;
    }
    numEntries_ = zipInfo.number_entry;
    if (numEntries_ < 0) {
        return false;
    }

    // We are already at the end if the zip file is empty.
    reachedEnd_ = (numEntries_ == 0);
    return true;
}

void ZipReader::Reset()
{
    zipFile_ = nullptr;
    numEntries_ = 0;
    reachedEnd_ = false;
    currentEntryInfo_.reset();
}

// FilePathWriterDelegate
FilePathWriterDelegate::FilePathWriterDelegate(const FilePath &outputFilePath) : outputFilePath_(outputFilePath)
{}

FilePathWriterDelegate::~FilePathWriterDelegate()
{}

bool FilePathWriterDelegate::PrepareOutput()
{
    if (!FilePathCheckValid(outputFilePath_.Value())) {
        APP_LOGE("%{public}s called, outputFilePath_ invalid !!!.", __func__);
        return false;
    }
    // We can't rely on parent directory entries being specified in the
    // zip, so we make sure they are created.
    if (!FilePath::CreateDirectory(outputFilePath_.DirName())) {
        return false;
    }

    file_ = fopen(outputFilePath_.Value().c_str(), "wb");
    return FilePath::PathIsValid(outputFilePath_);
}

bool FilePathWriterDelegate::WriteBytes(const char *data, int numBytes)
{
    if ((file_ == nullptr) || (numBytes <= 0) || (data == nullptr)) {
        return false;
    }
    int writebytes = fwrite(data, 1, numBytes, file_);
    return numBytes == writebytes;
}

void FilePathWriterDelegate::SetTimeModified(const struct tm *time)
{
    fclose(file_);
    file_ = nullptr;
}

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS