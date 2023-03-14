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
#include "zip_internal.h"

#include <algorithm>
#include <stddef.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "securec.h"
#include "zip_utils.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {

struct tm GetTmDataFromTickts(int64_t sec)
{
    time_t second = (time_t)sec;
    struct tm now {
        .tm_year = 0,
        .tm_mon = 0,
        .tm_mday = 0,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0,
    };
    struct tm *tmNow = nullptr;
    localtime_r(&second, tmNow);
    if (tmNow == nullptr) {
        return now;
    }
    int baseYeaar = 1900;
    now = *tmNow;
    now.tm_year += baseYeaar;
    now.tm_mon += 1;
    return now;
}

// Callback function for zlib that opens a file stream from a file descriptor.
// Since we do not own the file descriptor, dup it so that we can fdopen/fclose
// a file stream.
void *FdOpenFileFunc(void *opaque, const char *filename, int mode)
{
    if ((opaque == nullptr) || (filename == nullptr)) {
        return nullptr;
    }
    FILE *file = nullptr;
    const char *mode_fopen = nullptr;
    uint32_t modeInner = static_cast<uint32_t>(mode);
    if ((modeInner & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
        mode_fopen = "rb";
    else if (modeInner & ZLIB_FILEFUNC_MODE_EXISTING)
        mode_fopen = "r+b";
    else if (modeInner & ZLIB_FILEFUNC_MODE_CREATE)
        mode_fopen = "wb";

    if ((filename != nullptr) && (mode_fopen != nullptr)) {
        int fd = dup(*static_cast<int *>(opaque));
        if (fd != -1)
            file = fdopen(fd, mode_fopen);
    }

    return file;
}

int FdCloseFileFunc(void *opaque, void *stream)
{
    fclose(static_cast<FILE *>(stream));
    free(opaque);  // malloc'ed in FillFdOpenFileFunc()
    return 0;
}

// Fills |pzlib_filecunc_def| appropriately to handle the zip file
// referred to by |fd|.
void FillFdOpenFileFunc(zlib_filefunc_def *pzlibFilefuncDef, PlatformFile fd)
{
    if (pzlibFilefuncDef == nullptr) {
        return;
    }
    fill_fopen_filefunc(pzlibFilefuncDef);
    pzlibFilefuncDef->zopen_file = FdOpenFileFunc;
    pzlibFilefuncDef->zclose_file = FdCloseFileFunc;
    int *ptrFd = static_cast<int *>(malloc(sizeof(fd)));
    if (ptrFd == nullptr) {
        free(ptrFd);
        return;
    }
    *ptrFd = fd;
    pzlibFilefuncDef->opaque = ptrFd;
}

// A struct that contains data required for zlib functions to extract files from
// a zip archive stored in memory directly. The following I/O API functions
// expect their opaque parameters refer to this struct.
struct ZipBuffer {
    const char *data;
    size_t length;
    size_t offset;
};

// Opens the specified file. When this function returns a non-NULL pointer, zlib
// uses this pointer as a stream parameter while compressing or uncompressing
// data. (Returning NULL represents an error.) This function initializes the
// given opaque parameter and returns it because this parameter stores all
// information needed for uncompressing data. (This function does not support
// writing compressed data and it returns NULL for this case.)
void *OpenZipBuffer(void *opaque, const char *, int mode)
{
    uint32_t modeInner = static_cast<uint32_t>(mode);
    if ((modeInner & ZLIB_FILEFUNC_MODE_READWRITEFILTER) != ZLIB_FILEFUNC_MODE_READ) {
        APP_LOGI("%{public}s called, mode is not ZLIB_FILEFUNC_MODE_READ.", __func__);
        return NULL;
    }
    ZipBuffer *buffer = static_cast<ZipBuffer *>(opaque);
    if (!buffer || !buffer->data || !buffer->length) {
        return NULL;
    }
    buffer->offset = 0;
    return opaque;
}

// Reads compressed data from the specified stream. This function copies data
// refered by the opaque parameter and returns the size actually copied.
uLong ReadZipBuffer(void *opaque, void *, void *buf, uLong size)
{
    ZipBuffer *buffer = static_cast<ZipBuffer *>(opaque);
    if (buffer == nullptr) {
        APP_LOGI("%{public}s called, buffer = nullptr.", __func__);
        return 0;
    }
    if (buffer->offset > buffer->length) {
        APP_LOGI("%{public}s called, buffer->offset > buffer->length.", __func__);
        return 0;
    }

    size_t remaining_bytes = buffer->length - buffer->offset;
    if (!buffer || !buffer->data || !remaining_bytes) {
        return 0;
    }
    size = std::min(size, static_cast<uLong>(remaining_bytes));
    if (memcpy_s(buf, size, &buffer->data[buffer->offset], size) != EOK) {
        return 0;
    }
    buffer->offset += size;
    return size;
}

// Writes compressed data to the stream. This function always returns zero
// because this implementation is only for reading compressed data.
uLong WriteZipBuffer(void *opaque, void *stream, const void *buf, uLong)
{
    return 0;
}

// Returns the offset from the beginning of the data.
long GetOffsetOfZipBuffer(void *opaque, void *)
{
    ZipBuffer *buffer = static_cast<ZipBuffer *>(opaque);
    if (!buffer) {
        return -1;
    }
    return static_cast<long>(buffer->offset);
}

// Moves the current offset to the specified position.
long SeekZipBuffer(void *opaque, void *, uLong offset, int origin)
{
    ZipBuffer *buffer = static_cast<ZipBuffer *>(opaque);
    if (!buffer) {
        return -1;
    }
    if (origin == ZLIB_FILEFUNC_SEEK_CUR) {
        buffer->offset = std::min(buffer->offset + static_cast<size_t>(offset), buffer->length);
        return 0;
    }
    if (origin == ZLIB_FILEFUNC_SEEK_END) {
        buffer->offset = (buffer->length > offset) ? buffer->length - offset : 0;
        return 0;
    }
    if (origin == ZLIB_FILEFUNC_SEEK_SET) {
        buffer->offset = std::min(buffer->length, static_cast<size_t>(offset));
        return 0;
    }
    APP_LOGI("%{public}s called. origin is not supported.", __func__);
    return -1;
}

// Closes the input offset and deletes all resources used for compressing or
// uncompressing data. This function deletes the ZipBuffer object referred by
// the opaque parameter since zlib deletes the unzFile object and it does not
// use this object any longer.
int CloseZipBuffer(void *opaque, void *)
{
    if (opaque) {
        free(opaque);
    }
    return 0;
}

// Returns the last error happened when reading or writing data. This function
// always returns zero, which means there are not any errors.
int GetErrorOfZipBuffer(void *, void *)
{
    return 0;
}

bool TimeToZipFileInfo(const struct tm *fileTime, zip_fileinfo &zipInfo)
{
    if (fileTime == nullptr) {
        return false;
    }

    zipInfo.tmz_date.tm_year = (int)fileTime->tm_year + 1900;
    zipInfo.tmz_date.tm_mon = (int)fileTime->tm_mon + 1;
    zipInfo.tmz_date.tm_mday = (int)fileTime->tm_mday;
    zipInfo.tmz_date.tm_hour = (int)fileTime->tm_hour;
    zipInfo.tmz_date.tm_min = (int)fileTime->tm_min;
    zipInfo.tmz_date.tm_sec = (int)fileTime->tm_sec;
    return true;
}

unzFile OpenForUnzipping(std::string &fileNameUtf8)
{
    zlib_filefunc_def *zipFuncPtrs = nullptr;
    return unzOpen2(fileNameUtf8.c_str(), zipFuncPtrs);
}

unzFile OpenFdForUnzipping(int zipFD)
{
    zlib_filefunc_def zipFuncs;
    FillFdOpenFileFunc(&zipFuncs, zipFD);
    return unzOpen2("fd", &zipFuncs);
}

// static
unzFile PrepareMemoryForUnzipping(const std::string &data)
{
    if (data.empty()) {
        return NULL;
    }
    ZipBuffer *buffer = static_cast<ZipBuffer *>(malloc(sizeof(ZipBuffer)));
    if (!buffer) {
        free(buffer);
        return NULL;
    }
    buffer->data = data.data();
    buffer->length = data.length();
    buffer->offset = 0;

    zlib_filefunc_def zipFunctions;
    zipFunctions.zopen_file = OpenZipBuffer;
    zipFunctions.zread_file = ReadZipBuffer;
    zipFunctions.zwrite_file = WriteZipBuffer;
    zipFunctions.ztell_file = GetOffsetOfZipBuffer;
    zipFunctions.zseek_file = SeekZipBuffer;
    zipFunctions.zclose_file = CloseZipBuffer;
    zipFunctions.zerror_file = GetErrorOfZipBuffer;
    zipFunctions.opaque = static_cast<void *>(buffer);
    return unzOpen2(NULL, &zipFunctions);
}

zipFile OpenForZipping(const std::string &fileNameUtf8, int appendFlag)
{
    zlib_filefunc_def *zipFuncPtrs = nullptr;
    return zipOpen2(fileNameUtf8.c_str(),
        appendFlag,
        NULL,
        zipFuncPtrs);
}

zipFile OpenFdForZipping(PlatformFile zipFd, int appendFlag)
{
    zlib_filefunc_def zipFuncs;
    FillFdOpenFileFunc(&zipFuncs, zipFd);
    // Passing dummy "fd" filename to zlib.
    return zipOpen2("fd", appendFlag, NULL, &zipFuncs);
}

bool ZipOpenNewFileInZip(
    zipFile zipFile, const std::string &strPath, const OPTIONS &options, const struct tm *lastModifiedTime)
{
    const uLong LANGUAGE_ENCODING_FLAG = 0x1 << 11;

    zip_fileinfo fileInfo = {};
    TimeToZipFileInfo(lastModifiedTime, fileInfo);
    if (ZIP_OK != zipOpenNewFileInZip4(zipFile,    // file
        strPath.c_str(),    // filename
        &fileInfo,    // zip_fileinfo
        NULL,    // extrafield_local,
        0u,   // size_extrafield_local
        NULL,    // extrafield_global
        0u,    // size_extrafield_global
        NULL,    // comment
        Z_DEFLATED,    // method
        (int)options.level,    // level:default Z_DEFAULT_COMPRESSION
        0,    // raw
        -MAX_WBITS,    // windowBits
        (int)options.memLevel,    // memLevel: default DEF_MEM_LEVEL
        (int)options.strategy,    // strategy:default Z_DEFAULT_STRATEGY
        NULL,    // password
        0,    // crcForCrypting
        0,    // versionMadeBy
        LANGUAGE_ENCODING_FLAG)) {    // flagBase
        return false;
    }
    return true;
}

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
