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

#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_INTERNAL_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_INTERNAL_H

#include <string>
#include <time.h>
#include "contrib/minizip/unzip.h"
#include "contrib/minizip/zip.h"
#include "zip_utils.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
const int kZipMaxPath = 256;
const int kZipBufSize = 8192;
}  // namespace

// Callback function for zlib that opens a file stream from a file descriptor.
// Since we do not own the file descriptor, dup it so that we can fdopen/fclose
// a file stream.
void *FdOpenFileFunc(void *opaque, const char *filename, int mode);

int FdCloseFileFunc(void *opaque, void *stream);

// Fills |pzlib_filecunc_def| appropriately to handle the zip file
// referred to by |fd|.
void FillFdOpenFileFunc(zlib_filefunc_def *pzlibFilefuncDef, PlatformFile fd);

unzFile OpenFdForUnzipping(PlatformFile zipFD);

struct tm GetTmDataFromTickts(int64_t ticks);

unzFile OpenForUnzipping(std::string &fileNameUtf8);

// Opens the file referred to by |zipFD| for unzipping.
unzFile OpenFdForUnzipping(int zipFD);

// Creates a custom unzFile object which reads data from the specified string.
// This custom unzFile object overrides the I/O API functions of zlib so it can
// read data from the specified string.
unzFile PrepareMemoryForUnzipping(const std::string &data);

// Opens the given file name in UTF-8 for zipping, with some setup for
// Windows. |append_flag| will be passed to zipOpen2().
zipFile OpenForZipping(const std::string &fileNameUtf8, int appendFlag);

// Opens the file referred to by |zipFD| for zipping. |append_flag| will be
// passed to zipOpen2().
zipFile OpenFdForZipping(PlatformFile zipFd, int appendFlag);

bool ZipOpenNewFileInZip(
    zipFile zipFile, const std::string &strPath, const OPTIONS &options, const struct tm *lastModifiedTime);

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_ZIP_INTERNAL_H
