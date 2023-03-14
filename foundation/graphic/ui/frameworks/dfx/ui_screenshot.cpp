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

#include "dfx/ui_screenshot.h"
#if ENABLE_DEBUG
#include "iwindows_manager.h"
#include "common/screen.h"
#include "draw/draw_utils.h"
#include "gfx_utils/color.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/image_info.h"
#include "securec.h"

namespace OHOS {
class UIScreenshotListener : public IWindowsManager::ScreenshotListener {
public:
    UIScreenshotListener() : filePath_(nullptr) {}

    virtual ~UIScreenshotListener()
    {
        if (filePath_ != nullptr) {
            UIFree(reinterpret_cast<void*>(filePath_));
            filePath_ = nullptr;
        }
    }

    void OnScreenshotEnd(uint8_t* virAddr, uint32_t width, uint32_t height,
                         ImagePixelFormat format, uint32_t stride) override
    {
        if ((virAddr == nullptr) || ((format != IMAGE_PIXEL_FORMAT_ARGB1555) &&
            (format != IMAGE_PIXEL_FORMAT_ARGB8888)) || (width == 0) || (height == 0)) {
            return;
        }

        ImageHeader header = {0};
        header.colorMode = ARGB8888;
        header.width = width;
        header.height = height;

        unlink(filePath_);
        int32_t fd = open(filePath_, O_RDWR | O_CREAT, DEFAULT_FILE_PERMISSION);
        UIFree(reinterpret_cast<void*>(filePath_));
        filePath_ = nullptr;
        if (fd < 0) {
            GRAPHIC_LOGE("UIScreenshotListener::OnScreenshotEnd open file failed Err!\n");
            return;
        }

        if (write(fd, &header, sizeof(ImageHeader)) != sizeof(ImageHeader)) {
            GRAPHIC_LOGE("UIScreenshotListener::OnScreenshotEnd write image header failed Err!\n");
            close(fd);
            return;
        }

        uint32_t row = MAX_MALLOC_SIZE / width;
        row = (row == 0) ? 1 : row;
        uint32_t size = row * width * sizeof(uint32_t);
        uint32_t* argb8888Addr = static_cast<uint32_t*>(UIMalloc(size));
        if (argb8888Addr == nullptr) {
            GRAPHIC_LOGE("UIScreenshotListener::OnScreenshotEnd memory allocation failed Err!");
            close(fd);
            return;
        }

        while (height >= row) {
            WriteBlockToFile(fd, argb8888Addr, virAddr, row, width, format, stride);
            height -= row;
        }
        if (height != 0) {
            WriteBlockToFile(fd, argb8888Addr, virAddr, height, width, format, stride);
        }
        UIFree(reinterpret_cast<void*>(argb8888Addr));
        close(fd);
    }

    void SetFilePath(char* path)
    {
        if (filePath_ != nullptr) {
            UIFree(reinterpret_cast<void*>(filePath_));
        }
        filePath_ = path;
    }

private:
    static constexpr uint8_t DEFAULT_COLOR_SIZE = 4;
    static constexpr uint16_t MAX_MALLOC_SIZE = 2048; // unit: 4 bytes
    char* filePath_;

    bool WriteBlockToFile(int32_t fd, uint32_t* buffer, uint8_t*& startAddr, uint32_t row,
                          uint32_t width, ImagePixelFormat format, uint32_t stride) const
    {
        uint32_t* argb8888Addr = buffer;
        for (uint32_t r = 0; r < row; ++r) {
            if (format == IMAGE_PIXEL_FORMAT_ARGB1555) {
                uint16_t* temp = reinterpret_cast<uint16_t*>(startAddr);
                for (uint32_t i = 0; i < width; ++i) {
                    buffer[i] = PixelFormatUtils::ARGB1555ToARGB8888(*temp++);
                }
            } else if (format == IMAGE_PIXEL_FORMAT_ARGB8888) {
                if (memcpy_s(buffer, width * DEFAULT_COLOR_SIZE, startAddr, width * DEFAULT_COLOR_SIZE) != EOK) {
                    GRAPHIC_LOGE("memcpy_s error!");
                }
            }
            startAddr += stride;
            buffer += width;
        }

        uint32_t blockSize = row * width * sizeof(uint32_t);
        if (static_cast<uint32_t>(write(fd, argb8888Addr, blockSize)) != blockSize) {
            GRAPHIC_LOGE("UIScreenshotListener::WriteBlockToFile wrong amount of written data Err!");
            return false;
        }
        return true;
    }
};

UIScreenshot::~UIScreenshot()
{
    if (screenshotListener_ != nullptr) {
        delete screenshotListener_;
        screenshotListener_ = nullptr;
    }
}

UIScreenshot* UIScreenshot::GetInstance()
{
    static UIScreenshot instance;
    return &instance;
}

bool UIScreenshot::ScreenshotToFile(const char* path)
{
    if (screenshotListener_ == nullptr) {
        screenshotListener_ = new UIScreenshotListener();
        if (screenshotListener_ == nullptr) {
            GRAPHIC_LOGE("UIScreenshot::ScreenshotToFile register screenshot listener failed Err!\n");
            return false;
        }
        IWindowsManager::GetInstance()->SetScreenshotListener(screenshotListener_);
    }

    const char* srcPath = (path == nullptr) ? DEFAULT_SCREENSHOT_PATH : path;
    uint32_t pathLength = strlen(srcPath);
    char* destPath = static_cast<char*>(UIMalloc(pathLength + 1));
    if (destPath == nullptr) {
        return false;
    }

    if (memcpy_s(destPath, pathLength + 1, srcPath, pathLength) != EOK) {
        UIFree(reinterpret_cast<void*>(destPath));
        return false;
    }
    destPath[pathLength] = '\0';
    screenshotListener_->SetFilePath(destPath);
    IWindowsManager::GetInstance()->Screenshot();
    return true;
}
} // namespace OHOS
#endif // ENABLE_DEBUG
