/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "common/image.h"
#include "common/image_decode_ability.h"
#include "draw/draw_image.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "imgdecode/cache_manager.h"
#if ENABLE_JPEG_AND_PNG
#include "jpeglib.h"
#include "png.h"
#endif
#include "securec.h"

namespace OHOS {
Image::Image() : imageInfo_(nullptr), path_(nullptr), srcType_(IMG_SRC_UNKNOWN), mallocFlag_(false) {}

Image::~Image()
{
    if (srcType_ == IMG_SRC_FILE) {
        CacheManager::GetInstance().Close(path_);
    }

    ReInitImageInfo(nullptr, false);

    if (path_ != nullptr) {
        UIFree(reinterpret_cast<void*>(const_cast<char*>(path_)));
        path_ = nullptr;
    }
    srcType_ = IMG_SRC_UNKNOWN;
}

void Image::GetHeader(ImageHeader& header) const
{
    if ((srcType_ == IMG_SRC_VARIABLE) && (imageInfo_ != nullptr)) {
        header = imageInfo_->header;
    } else if ((srcType_ == IMG_SRC_FILE) && (path_ != nullptr)) {
        CacheManager::GetInstance().GetImageHeader(path_, header);
    }
}

#if ENABLE_JPEG_AND_PNG
OHOS::Image::ImageType Image::CheckImgType(const char* src)
{
    char buf[IMG_BYTES_TO_CHECK] = {0};
#ifdef _WIN32
    int32_t fd = open(src, O_RDONLY | O_BINARY);
#else
    int32_t fd = open(src, O_RDONLY);
#endif
    if (fd < 0) {
        GRAPHIC_LOGE("can't open %s\n", src);
        return IMG_UNKNOWN;
    }
    if (read(fd, buf, IMG_BYTES_TO_CHECK) != IMG_BYTES_TO_CHECK) {
        close(fd);
        return IMG_UNKNOWN;
    }
    close(fd);
    if (!png_sig_cmp(reinterpret_cast<png_const_bytep>(buf), 0, IMG_BYTES_TO_CHECK)) {
        return IMG_PNG;
    // 0xFF 0xD8: JPEG file's header
    } else if ((static_cast<uint8_t>(buf[0]) == 0xFF) && (static_cast<uint8_t>(buf[1]) == 0xD8)) {
        return IMG_JPEG;
    } else if ((static_cast<uint8_t>(buf[0]) == 0x47) && (static_cast<uint8_t>(buf[1]) == 0x49) &&
          (static_cast<uint8_t>(buf[2]) == 0x46)) { // 2: array index of GIF file's header
        return IMG_GIF;
    }
    return IMG_UNKNOWN;
}
#endif

bool Image::SetStandardSrc(const char* src)
{
    if (src == nullptr) {
        return false;
    }
    srcType_ = IMG_SRC_UNKNOWN;

    const char* ptr = strrchr(src, '.');
    if (ptr == nullptr) {
        return false;
    }

#if ENABLE_JPEG_AND_PNG
    ImageType imageType = CheckImgType(src);
    if (imageType == IMG_PNG) {
        return SetPNGSrc(src);
    } else if (imageType == IMG_JPEG) {
        return SetJPEGSrc(src);
    }
#endif

    size_t strLen = strlen(src) + 1;
    char* imagePath = static_cast<char*>(UIMalloc(static_cast<uint32_t>(strLen)));
    if (imagePath == nullptr) {
        return false;
    }

    if (strcpy_s(imagePath, strLen, src) != EOK) {
        UIFree(reinterpret_cast<void*>(imagePath));
        imagePath = nullptr;
        return false;
    }
    path_ = imagePath;
    srcType_ = IMG_SRC_FILE;
    return true;
}

bool Image::SetLiteSrc(const char* src)
{
    if (src == nullptr) {
        return false;
    }
    srcType_ = IMG_SRC_UNKNOWN;

    const char* ptr = strrchr(src, '.');
    if (ptr == nullptr) {
        return false;
    }

    size_t strLen = strlen(src) + 1;
    char* imagePath = static_cast<char*>(UIMalloc(static_cast<uint32_t>(strLen)));
    if (imagePath == nullptr) {
        return false;
    }
    if (IsImgValid(ptr)) {
        const char* suffixName = "bin";
        if (memcpy_s(imagePath, strLen, src, strLen) != EOK) {
            UIFree(reinterpret_cast<void*>(imagePath));
            imagePath = nullptr;
            return false;
        }
        (ptr - src + imagePath)[1] = '\0'; // remove suffix
        if (strcat_s(imagePath, strLen, suffixName) != EOK) {
            UIFree(reinterpret_cast<void*>(imagePath));
            imagePath = nullptr;
            return false;
        }
    } else {
        if (memcpy_s(imagePath, strLen, src, strLen) != EOK) {
            UIFree(reinterpret_cast<void*>(imagePath));
            imagePath = nullptr;
            return false;
        }
    }
    path_ = imagePath;
    srcType_ = IMG_SRC_FILE;
    return true;
}

bool Image::SetSrc(const char* src)
{
    if (path_ != nullptr) {
        UIFree(reinterpret_cast<void*>(const_cast<char*>(path_)));
        path_ = nullptr;
    }

    if (src != nullptr) {
        uint32_t imageType = ImageDecodeAbility::GetInstance().GetImageDecodeAbility();
        if (((imageType & IMG_SUPPORT_JPEG) == IMG_SUPPORT_JPEG) ||
            ((imageType & IMG_SUPPORT_PNG) == IMG_SUPPORT_PNG)) {
            return SetStandardSrc(src);
        }
        return SetLiteSrc(src);
    }
    srcType_ = IMG_SRC_UNKNOWN;
    return true;
}

bool Image::SetSrc(const ImageInfo* src)
{
    ReInitImageInfo(nullptr, false);
    srcType_ = IMG_SRC_UNKNOWN;
    imageInfo_ = nullptr;

    if (src != nullptr) {
        imageInfo_ = static_cast<ImageInfo*>(UIMalloc(static_cast<uint32_t>(sizeof(ImageInfo))));
        if (imageInfo_ == nullptr) {
            return false;
        }

        if (memcpy_s(const_cast<ImageInfo*>(imageInfo_), sizeof(ImageInfo), src, sizeof(ImageInfo)) != EOK) {
            return false;
        }

        srcType_ = IMG_SRC_VARIABLE;
    }

    return true;
}

bool Image::PreParse(const char *src)
{
    if (src == nullptr) {
        return false;
    }
    const char* ptr = strrchr(src, '.');
    if (ptr == nullptr) {
        srcType_ = IMG_SRC_UNKNOWN;
        return false;
    }
    if (path_ != nullptr) {
        UIFree((void*)path_);
    }
    size_t strLen = strlen(src) + 1;
    char* path = (char*)UIMalloc(strLen);
    if (strcpy_s(path, strLen, src) != EOK) {
        UIFree(reinterpret_cast<void*>(path));
        return false;
    }
    path_ = path;
    bool isSucess = false;
    ImageType imageType = CheckImgType(src);
    if (imageType == IMG_PNG) {
        isSucess = SetPNGSrc(src);
    } else if (imageType == IMG_JPEG) {
        isSucess = SetJPEGSrc(src);
    } else if (imageType == IMG_GIF) {
        isSucess = true;
    } else {
        srcType_ = IMG_SRC_UNKNOWN;
        return false;
    }
    return isSucess;
}

void Image::DrawImage(BufferInfo& gfxDstBuffer,
                      const Rect& coords,
                      const Rect& mask,
                      const Style& style,
                      uint8_t opaScale) const
{
    if (srcType_ == IMG_SRC_VARIABLE) {
        DrawImage::DrawCommon(gfxDstBuffer, coords, mask, imageInfo_, style, opaScale);
    } else if (srcType_ == IMG_SRC_FILE) {
        DrawImage::DrawCommon(gfxDstBuffer, coords, mask, path_, style, opaScale);
    } else {
        GRAPHIC_LOGE("Image::DrawImage:: failed with error srctype!\n");
    }
}

#if ENABLE_JPEG_AND_PNG

static inline void FreePngBytep(png_bytep** rowPointer, uint16_t size)
{
    png_bytep* tmpRowPointer = *rowPointer;
    for (uint16_t i = 0; i < size; i++) {
        UIFree(tmpRowPointer[i]);
        tmpRowPointer[i] = nullptr;
    }
    UIFree(*rowPointer);
    *rowPointer = nullptr;
}

static inline png_bytep* MallocPngBytep(uint16_t height, uint32_t rowBytes)
{
    png_bytep* rowPointer = static_cast<png_bytep*>(UIMalloc(sizeof(png_bytep) * height));
    if (rowPointer == nullptr) {
        return nullptr;
    }
    for (uint16_t y = 0; y < height; y++) {
        rowPointer[y] = static_cast<png_byte*>(UIMalloc(rowBytes));
        if (rowPointer[y] == nullptr) {
            FreePngBytep(&rowPointer, y);
            return nullptr;
        }
    }
    return rowPointer;
}

bool Image::SetPNGSrc(const char* src)
{
    srcType_ = IMG_SRC_UNKNOWN;
    png_bytep* rowPointer = nullptr;
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr) {
        return false;
    }
    png_infop info = png_create_info_struct(png);
    if (info == nullptr) {
        png_destroy_read_struct(&png, &info, nullptr);
        return false;
    }
    FILE* infile = fopen(src, "rb");
    if (infile == nullptr) {
        GRAPHIC_LOGE("can't open %s\n", src);
        png_destroy_read_struct(&png, &info, nullptr);
        return false;
    }
    png_init_io(png, infile);
    png_read_info(png, info);

    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(ARGB8888) >> 3; // 3: Shift right 3 bits
    uint16_t width = png_get_image_width(png, info);
    uint16_t height = png_get_image_height(png, info);
    uint8_t colorType = png_get_color_type(png, info);
    uint8_t bitDepth = png_get_bit_depth(png, info);
    uint32_t dataSize = height * width * pixelByteSize;

    if ((colorType == PNG_COLOR_TYPE_GRAY) && (bitDepth < 8)) { // 8: Expand grayscale images to the full 8 bits
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if ((colorType == PNG_COLOR_TYPE_GRAY) || (colorType == PNG_COLOR_TYPE_GRAY_ALPHA)) {
        png_set_gray_to_rgb(png);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (bitDepth == 16) { // 16: Chop 16-bit depth images to 8-bit depth
        png_set_strip_16(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (!(colorType & PNG_COLOR_MASK_ALPHA)) {
        png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
    }
    png_set_interlace_handling(png);
    png_read_update_info(png, info);

    rowPointer = MallocPngBytep(height, png_get_rowbytes(png, info));
    if (rowPointer == nullptr) {
        fclose(infile);
        png_destroy_read_struct(&png, &info, nullptr);
        return false;
    }

    png_read_image(png, rowPointer);
    fclose(infile);
    png_destroy_read_struct(&png, &info, nullptr);

    ImageInfo* imgInfo = static_cast<ImageInfo*>(UIMalloc(sizeof(ImageInfo)));
    if (imgInfo == nullptr) {
        FreePngBytep(&rowPointer, height);
        return false;
    }
    uint8_t* srcData = static_cast<uint8_t*>(UIMalloc(dataSize));
    if (srcData == nullptr) {
        FreePngBytep(&rowPointer, height);
        UIFree(imgInfo);
        return false;
    }
    uint32_t n = 0;
    for (uint16_t y = 0; y < height; y++) {
        png_bytep row = rowPointer[y];
        for (uint16_t x = 0; x < width * pixelByteSize; x += pixelByteSize) {
            srcData[n++] = row[x + 2]; // 2: B channel
            srcData[n++] = row[x + 1]; // 1: G channel
            srcData[n++] = row[x + 0]; // 0: R channel
            srcData[n++] = row[x + 3]; // 3: Alpha channel
        }
    }
    FreePngBytep(&rowPointer, height);

    imgInfo->header.width = width;
    imgInfo->header.height = height;
    imgInfo->header.colorMode = ARGB8888;
    imgInfo->dataSize = dataSize;
    imgInfo->data = srcData;

    ReInitImageInfo(imgInfo, true);
    srcType_ = IMG_SRC_VARIABLE;
    return true;
}

bool Image::SetJPEGSrc(const char* src)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    srcType_ = IMG_SRC_UNKNOWN;

    FILE* infile = fopen(src, "rb");
    if (infile == nullptr) {
        GRAPHIC_LOGE("can't open %s\n", src);
        return false;
    }
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(ARGB8888) >> 3; // 3: Shift right 3 bits
    uint16_t width = cinfo.output_width;
    uint16_t height = cinfo.output_height;
    uint32_t dataSize = width * height * pixelByteSize;
    uint16_t rowStride = cinfo.output_width * pixelByteSize;
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&cinfo), JPOOL_IMAGE, rowStride,
                                                   1); // 1: one-row-high array
    ImageInfo* imgInfo = static_cast<ImageInfo*>(UIMalloc(sizeof(ImageInfo)));
    if (imgInfo == nullptr) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
    }
    uint8_t* srcData = static_cast<uint8_t*>(UIMalloc(dataSize));
    if (srcData == nullptr) {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        UIFree(imgInfo);
        return false;
    }
    uint32_t n = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);       // 1: read one line each time
        for (uint16_t x = 0; x < width * 3; x += 3) { // 3: color components per pixel
            srcData[n++] = buffer[0][x + 2];          // 2: B channel
            srcData[n++] = buffer[0][x + 1];          // 1: G channel
            srcData[n++] = buffer[0][x + 0];          // 0: R channel
            srcData[n++] = 255;                       // 255: set alpha channel
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    imgInfo->header.width = width;
    imgInfo->header.height = height;
    imgInfo->header.colorMode = ARGB8888;
    imgInfo->dataSize = dataSize;
    imgInfo->data = srcData;

    ReInitImageInfo(imgInfo, true);
    srcType_ = IMG_SRC_VARIABLE;
    return true;
}
#endif

void Image::ReInitImageInfo(ImageInfo* imgInfo, bool mallocFlag)
{
    if (mallocFlag_) {
        if (imageInfo_->data != nullptr) {
            UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(imageInfo_->data)));
        }
    }
    UIFree(reinterpret_cast<void*>(const_cast<ImageInfo*>(imageInfo_)));

    imageInfo_ = imgInfo;
    mallocFlag_ = mallocFlag;
}
} // namespace OHOS
