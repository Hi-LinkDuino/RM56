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
#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_H

#include <vector>
#include <functional>
#include <iostream>
#include <memory>
#include <time.h>
#include "zip_utils.h"
#include "file_path.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {

class WriterDelegate;
// Abstraction for file access operation required by Zip().
// Can be passed to the ZipParams for providing custom access to the files,
// for example over IPC.
// If none is provided, the files are accessed directly.
// All parameters paths are expected to be absolute.
class FileAccessor {
public:
    virtual ~FileAccessor() = default;

    struct DirectoryContentEntry {
        DirectoryContentEntry(const FilePath &path, bool directory) : path(path), isDirectory(directory)
        {}
        FilePath path;
        bool isDirectory = false;
    };
};

class ZipParams {
public:
    ZipParams(const FilePath &srcDir, const FilePath &destFile);

    // Does not take ownership of |destFd|.
    ZipParams(const FilePath &srcDir, int destFd);
    virtual ~ZipParams()
    {}
    int DestFd() const
    {
        return destFd_;
    }

    const FilePath &SrcDir() const
    {
        return srcDir_;
    }

    const FilePath &DestFile() const
    {
        return destFile_;
    }

    // Restricts the files actually zipped to the paths listed in
    // |srcRelativePaths|. They must be relative to the |srcDir| passed in the
    // constructor and will be used as the file names in the created zip file. All
    // source paths must be under |srcDir| in the file system hierarchy.
    void SetFilesTozip(const std::vector<FilePath> &srcRelativePaths)
    {
        srcFiles_ = srcRelativePaths;
    }
    const std::vector<FilePath> &GetFilesTozip() const
    {
        return srcFiles_;
    }

    using FilterCallback = std::function<bool(const FilePath &)>;
    void SetFilterCallback(FilterCallback filterCallback)
    {
        filterCallback_ = filterCallback;
    }
    const FilterCallback &GetFilterCallback() const
    {
        return filterCallback_;
    }

    void SetIncludeHiddenFiles(bool includeHiddenFiles)
    {
        includeHiddenFiles_ = includeHiddenFiles;
    }
    bool GetIncludeHiddenFiles() const
    {
        return includeHiddenFiles_;
    }

    // Sets a custom file accessor for file operations. Default is to directly
    // access the files (with fopen and the rest).
    // Useful in cases where running in a sandbox process and file access has to
    // go through IPC, for example.
    void SetFileAccessor(std::unique_ptr<FileAccessor> file_accessor)
    {
        fileAccessor_ = std::move(file_accessor);
    }
    FileAccessor *GetFileAccessor() const
    {
        return fileAccessor_.get();
    }

private:
    FilePath srcDir_;

    FilePath destFile_;

    int destFd_ = kInvalidPlatformFile;

    // The relative paths to the files that should be included in the zip file. If
    // this is empty, all files in |srcDir_| are included.
    std::vector<FilePath> srcFiles_;

    // Filter used to exclude files from the ZIP file. Only effective when
    // |srcFiles_| is empty.
    FilterCallback filterCallback_;

    // Whether hidden files should be included in the ZIP file. Only effective
    // when |srcFiles_| is empty.
    bool includeHiddenFiles_ = true;

    // Abstraction around file system access used to read files. An implementation
    // that accesses files directly is provided by default.
    std::unique_ptr<FileAccessor> fileAccessor_;
};

// Convenience method for callers who don't need to set up the filter callback.
// If |includeHiddenFiles| is true, files starting with "." are included.
// Otherwise they are omitted.
// example No1
// srcDir = /ziptest/zipdata/
// destFile = /ziptest/hapresult/hapfourfile.zip
// example No2
// srcDir = /ziptest/zipdata/zip1/zip1-1.cpp
// destFile = /ziptest/hapresult/singlefile.zip
// options is default value.
bool Zip(const FilePath &srcDir, const FilePath &destFile, const OPTIONS &options, CALLBACK callback,
    bool includeHiddenFiles);

// Unzip the contents of zipFile into destDir.
// example No1
// srcDir = /ziptest/hapresult/hapfourfile.zip
// destFile = /ziptest/hapunzipdir/01
// example No2
// srcDir = /ziptest/hapresult/singlefile.zip
// destFile = /ziptest/hapunzipdir/single
// options is default value.
bool Unzip(const FilePath &zipFile, const FilePath &destDir, const OPTIONS &options, CALLBACK callback);

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_H
