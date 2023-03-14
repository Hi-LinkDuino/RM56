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
#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_WRITER_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_WRITER_H

#include <memory>
#include <vector>
#include "file_path.h"
#include "zip_utils.h"
#include "contrib/minizip/unzip.h"
#include "contrib/minizip/zip.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
// A class used to write entries to a ZIP file and buffering the reading of
// files to limit the number of calls to the FileAccessor. This is for
// performance reasons as these calls may be expensive when IPC based).
// This class is so far internal and only used by zip.cpp, but could be made
// public if needed.
class ZipWriter {
public:
    // Creates a writer that will write a ZIP file to |zipFilefd|/|zip_file|
    // and which entries (specifies with AddEntries) are relative to |rootDir|.
    // All file reads are performed using |file_accessor|.
    static std::unique_ptr<ZipWriter> CreateWithFd(PlatformFile zipFilefd, const FilePath &rootDir);
    static std::unique_ptr<ZipWriter> Create(const FilePath &zip_file, const FilePath &rootDir);
    ~ZipWriter();

    // Writes the files at |paths| to the ZIP file and closes this Zip file.
    // Note that the the FilePaths must be relative to |rootDir| specified in the
    // Create method.
    // Returns true if all entries were written successfuly.
    bool WriteEntries(const std::vector<FilePath> &paths, const OPTIONS &options, CALLBACK callback);

private:
    ZipWriter(zipFile zip_file, const FilePath &rootDir);

    // Writes the pending entries to the ZIP file if there are at least
    // |g_MaxPendingEntriesCount| of them. If |force| is true, all pending entries
    // are written regardless of how many there are.
    // Returns false if writing an entry fails, true if no entry was written or
    // there was no error writing entries.
    bool FlushEntriesIfNeeded(bool force, const OPTIONS &options, CALLBACK callback);

    // Adds the files at |paths| to the ZIP file. These FilePaths must be relative
    // to |rootDir| specified in the Create method.
    bool AddEntries(const std::vector<FilePath> &paths, const OPTIONS &options, CALLBACK callback);

    // Closes the ZIP file.
    // Returns true if successful, false otherwise (typically if an entry failed
    // to be written).
    bool Close(const OPTIONS &options, CALLBACK callback);

    // The entries that have been added but not yet written to the ZIP file.
    std::vector<FilePath> pendingEntries_;

    // The actual zip file.
    zipFile zipFile_;

    // Path to the directory entry paths are relative to.
    FilePath rootDir_;

    DISALLOW_COPY_AND_ASSIGN(ZipWriter);
};
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_WRITER_H