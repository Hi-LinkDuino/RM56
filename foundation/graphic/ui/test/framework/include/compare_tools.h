/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_COMPARE_TOOLS_H
#define GRAPHIC_LITE_COMPARE_TOOLS_H

#include "graphic_config.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif // _WIN32

namespace OHOS {
namespace {
    constexpr uint16_t DEFAULT_WAIT_TIME_MS = 300;
    constexpr size_t DEFAULT_FILE_NAME_MAX_LENGTH = 256;
    constexpr uint8_t BITMAP_HEADER_SIZE = 54;
}
class CompareTools {
public:
    struct BitmapInfoHeader {
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        uint32_t biXPelsPerMeter;
        uint32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    static void WaitSuspend();
    static bool StrnCatPath(char* filePath, size_t pathMax, const char* fileName, size_t count);
    static bool CompareFile(const char* filePath, size_t length);
    static bool SaveFile(const char* filePath, size_t length);
    static bool CheckFileExist(const char* filePath, size_t length);
    static void SetLogPath(const char* filePath, size_t length);
    static void UnsetLogPath();

private:
    CompareTools() {}
    virtual ~CompareTools() {}

    static bool SaveLog(const char* buff, size_t bufSize);
    static bool CompareByBit(uint32_t fd);
    static bool SaveByBit(uint32_t fd);
    static bool enableLog_;
    static char* logPath_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_COMPARE_TOOLS_H
