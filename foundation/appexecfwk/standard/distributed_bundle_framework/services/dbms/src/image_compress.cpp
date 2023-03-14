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
#include <cstdio>
#include <cmath>
#include <unistd.h>

#include "image_compress.h"

#include "png.h"
#include "jpeglib.h"
#include "app_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    constexpr int32_t RGBA_COMPONENTS = 4;
    constexpr int32_t RGB_COMPONENTS = 3;
    constexpr int32_t FILE_MAX_SIZE = 10240;
    constexpr int32_t FILE_COMPRESS_SIZE = 4196;
    constexpr int32_t BITDEPTH_SIXTHEN = 16;
    constexpr int32_t BITDEPTH_EIGHT = 8;
    constexpr int32_t COLOR_CHANNEL_GREEN = 1;
    constexpr int32_t COLOR_CHANNEL_BLUE = 2; 
    constexpr int32_t COLOR_CHANNEL_ALPHA = 3;
    constexpr int32_t NUMBER_ONE = 1;
    constexpr int32_t QUALITY = 30;
    constexpr double EPSILON = 1e-5;
    const std::string JPG = "jpg";
    const std::string PNG = "png";
    const std::string JPEG = "jpeg";
    const std::string BUNDLE_PATH = "/data/app/el1/bundle";
    struct EncodeMemo {
        ImageRow buffer;
        uint32_t size;
    };
}

void ImageCompress::ReleasePngPointer(png_bytepp& rowPointers, uint32_t height)
{
    if (rowPointers != nullptr) {
        for (uint32_t i = 0; i < height; ++i) {
            if (rowPointers[i] != nullptr) {
                free(rowPointers[i]);
            }
        }
        free(rowPointers);
    }
    rowPointers = nullptr;
}

bool ImageCompress::MallocPngPointer(png_bytepp& rowPointers, uint32_t height, uint32_t strides)
{
    if (rowPointers != nullptr || height == 0 || strides == 0) {
        return false;
    }
    rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (rowPointers == nullptr) {
        APP_LOGE("ImageCompress: MallocPngPointer malloc buffer failed");
        return false;
    }
    for (uint32_t h = 0; h < height; ++h) {
        rowPointers[h] = (png_byte*)malloc(strides);
        if (rowPointers[h] == nullptr) {
            return false;
        }
    }
    return true;
}

bool ImageCompress::IsPathValid(std::string& fileName)
{
    if (fileName.find(BUNDLE_PATH) != std::string::npos) {
        return access(fileName.c_str(), F_OK) == 0;
    } else {
        return false;
    }
}

double ImageCompress::CalRatio(std::string fileName)
{
    if (!IsPathValid(fileName)) {
        APP_LOGE("%{public}s is unavailable", fileName.c_str());
        return -1.0;
    }
    FILE* file = fopen(fileName.c_str(), "rb");
    if (!file) {
        APP_LOGE("ImageCompress: CalRatio %{public}s is unavailable", fileName.c_str());
    }
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return -1.0;
    }
    int32_t fileSize = ftell(file);
    if (fileSize < FILE_MAX_SIZE) {
        APP_LOGE("ImageCompress: CalRatio do not need compress");
        fclose(file);
        return -1.0;
    }
    if (fclose(file) != 0) {
        return -1.0;
    }
    return sqrt(static_cast<double>(FILE_COMPRESS_SIZE) / fileSize);
}

bool ImageCompress::NeedCompress(std::string fileName)
{
    if (!IsPathValid(fileName)) {
        APP_LOGE("%{public}s is unavailable", fileName.c_str());
        return false;
    }
    FILE* file = fopen(fileName.c_str(), "rb");
    if (!file) {
        APP_LOGE("ImageCompress: CalRatio %{public}s is unavailable", fileName.c_str());
    }
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return false;
    }
    int32_t fileSize = ftell(file);
    if (fileSize < FILE_MAX_SIZE) {
        APP_LOGE("ImageCompress: CalRatio do not need compress");
        fclose(file);
        return false;
    }
    if (fclose(file) != 0) {
        return false;
    }
    return true;
}

void ImageCompress::PngToBuffer(png_structp png, png_bytep data, png_size_t length)
{
    struct EncodeMemo *p = (struct EncodeMemo*)png_get_io_ptr(png);
    if (memcpy_s(p->buffer + p->size, length, data, length) != EOK) {
        APP_LOGE("ImageCompress: memcpy_s buffer failed");
        return;
    }
    p->size += length;
}

bool ImageCompress::DoubleEqual(double left, double right)
{
    return std::abs(left - right ) <= EPSILON;
}

bool ImageCompress::InitPngFile(std::shared_ptr<ImageBuffer>& imageBuffer,
    png_structp& png, png_infop& info)
{
    imageBuffer->SetWidth(png_get_image_width(png, info));
    imageBuffer->SetHeight(png_get_image_height(png, info));
    imageBuffer->SetColorType(png_get_color_type(png, info));
    imageBuffer->SetBitDepth(png_get_bit_depth(png, info));
    imageBuffer->SetPngComponents(png_get_channels(png, info));
    if (imageBuffer->GetBitDepth() == BITDEPTH_SIXTHEN) {
        png_set_strip_16(png);
    }
    if (imageBuffer->GetColorType() == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (imageBuffer->GetColorType() == PNG_COLOR_TYPE_RGB ||
        imageBuffer->GetColorType() == PNG_COLOR_TYPE_GRAY ||
        imageBuffer->GetColorType() == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (imageBuffer->GetColorType() == PNG_COLOR_TYPE_GRAY ||
        imageBuffer->GetColorType() == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }
    png_read_update_info(png, info);
    if (imageBuffer->GetImageDataPointer()) {
        png_destroy_read_struct(&png, &info, NULL);
        return false;
    }

    imageBuffer->MallocImageMap(RGBA_COMPONENTS);
    imageBuffer->SetComponents(RGBA_COMPONENTS);
    uint32_t strides = imageBuffer->GetWidth() *  RGBA_COMPONENTS;
    png_bytepp rowPointers = nullptr;
    if (!MallocPngPointer(rowPointers, imageBuffer->GetHeight(), strides)) {
        APP_LOGE("ImageCompress: MallocPngPointer image buffer failed");
        png_destroy_read_struct(&png, &info, NULL);
        return false;
    }
    png_read_image(png, rowPointers);
    ImageRow imageRow = imageBuffer->GetImageDataPointer().get();
    if (rowPointers == nullptr) {
        png_destroy_read_struct(&png, &info, NULL);
        return false;
    }
    for (uint32_t h = 0; h < imageBuffer->GetHeight(); ++h) {
        if (memcpy_s(imageRow, strides, rowPointers[h], strides) != EOK) {
            ReleasePngPointer(rowPointers, imageBuffer->GetHeight());
            png_destroy_read_struct(&png, &info, NULL);
            return false;
        }
        imageRow += strides;
    }
    ReleasePngPointer(rowPointers, imageBuffer->GetHeight());
    return true;
}

int ImageCompress::DecodePngFile(std::string fileName, std::shared_ptr<ImageBuffer>& imageBuffer)
{
    if (imageBuffer == nullptr) {
        APP_LOGE("ImageCompress: DecodePngFile image buffer is unavailable");
        return -1;
    }
    if (!IsPathValid(fileName)) {
        APP_LOGE("%{public}s is unavailable", fileName.c_str());
        return -1.0;
    }
    double ratio = CalRatio(fileName);
    if (ratio < 0) {
        APP_LOGE("ImageCompress: DecodePngFile file %{public}s do not need compress", fileName.c_str());
        return -1;
    }

    FILE* inFile = fopen(fileName.c_str(), "rb");
    if (inFile == nullptr) {
        APP_LOGE("open file %{private}s failed", fileName.c_str());
        return -1;
    }
    imageBuffer->SetRatio(ratio);
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(inFile);
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(inFile);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(inFile);
        return -1;
    }

    png_init_io(png, inFile);
    png_read_info(png, info);

    if (!InitPngFile(imageBuffer, png, info)) {
        if (fclose(inFile) != EOK) {
            APP_LOGE("ImageCompress: fclose file %{public}s error", fileName.c_str());
            return -1;
        }
        return -1;
    }

    if (fclose(inFile) != EOK) {
        png_destroy_read_struct(&png, &info, NULL);
        APP_LOGE("ImageCompress: fclose file %{public}s error", fileName.c_str());
        return -1;
    }
    png_destroy_read_struct(&png, &info, NULL);
    return 0;
}

int32_t ImageCompress::EncodePngFile(std::shared_ptr<ImageBuffer>& imageBuffer)
{
    if (imageBuffer == nullptr) {
        APP_LOGE("ImageCompress: Encode none data");
        return -1;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        png_destroy_read_struct(&png, NULL, NULL);
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, &info, NULL);
        return -1;
    }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        return -1;
    }
    png_set_IHDR(
        png,
        info,
        imageBuffer->GetWidth(),
        imageBuffer->GetHeight(),
        BITDEPTH_EIGHT,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    uint32_t strides = imageBuffer->GetWidth() * RGBA_COMPONENTS;
    struct EncodeMemo memo;
    memo.buffer = (ImageRow)malloc(FILE_MAX_SIZE * RGBA_COMPONENTS);
    if (memo.buffer == nullptr) {
        png_destroy_read_struct(&png, &info, NULL);
        return -1;
    }
    memo.size = 0;
    if (!imageBuffer->GetImageDataPointer()) {
        free(memo.buffer);
        png_destroy_read_struct(&png, &info, NULL);
        APP_LOGE("ImageCompress: EncodePngFile should input image buffer");
        return -1;
    }
    ImageRow imageRow = imageBuffer->GetImageDataPointer().get();
    png_bytep* rowPointers;
    rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * imageBuffer->GetHeight());
    if (rowPointers == nullptr) {
        free(memo.buffer);
        png_destroy_read_struct(&png, &info, NULL);
        APP_LOGE("ImageCompress: malloc failed");
        free(memo.buffer);
        return -1;
    }
    for (uint32_t h = 0; h < imageBuffer->GetHeight(); ++h) {
        rowPointers[h] = (png_byte*)malloc(strides);
    }
    for (uint32_t h = 0; h < imageBuffer->GetHeight(); ++h) {
        memcpy_s(rowPointers[h], strides, imageRow, strides);
        imageRow += strides;
    }
    png_set_rows(png, info, &rowPointers[0]);
    png_set_write_fn(png, &memo, PngToBuffer, NULL);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png, &info);
    imageBuffer->SetCompressData(memo.buffer, memo.size);
    imageBuffer->SetCompressSize(memo.size);
    if (memo.buffer) {
        free(memo.buffer);
    }
    memo.buffer = nullptr;
    ReleasePngPointer(rowPointers, imageBuffer->GetHeight());
    return 0;
}

int32_t ImageCompress::ResizeRGBAImage(std::shared_ptr<ImageBuffer>& imageBufferIn,
                                       std::shared_ptr<ImageBuffer>& imageBufferOut)
{
    if (imageBufferIn == nullptr || imageBufferOut == nullptr) {
        APP_LOGE("ImageCompress: ResizePRGBAImage should input image buffer");
        return -1;
    }
    double ratio = imageBufferIn->GetRatio();
    ratio = ratio * (static_cast<double>(imageBufferIn->GetPngComponents()) / RGBA_COMPONENTS) ;
    if (DoubleEqual(ratio, 0.0)) {
        return -1;
    }
    imageBufferOut->SetWidth(imageBufferIn->GetWidth() * ratio);
    imageBufferOut->SetHeight(imageBufferIn->GetHeight() * ratio);
    imageBufferOut->SetComponents(imageBufferIn->GetComponents());
    imageBufferOut->SetColorType(imageBufferIn->GetColorType());
    imageBufferOut->SetBitDepth(imageBufferIn->GetBitDepth());
    imageBufferOut->MallocImageMap(imageBufferOut->GetComponents());
    ImageRow imageRowIn = imageBufferIn->GetImageDataPointer().get();
    ImageRow imageRowOut = imageBufferOut->GetImageDataPointer().get();
    uint32_t outRowStride = imageBufferOut->GetWidth() * imageBufferOut->GetComponents();
    uint32_t inRowStride = imageBufferIn->GetWidth() * imageBufferIn->GetComponents();
    uint32_t components = imageBufferIn->GetComponents();
    for (uint32_t h = 0; h < imageBufferOut->GetHeight(); ++h) {
        for (uint32_t w = 0; w < imageBufferOut->GetWidth(); ++w) {
            uint64_t heightIndex = std::round(h / ratio);
            uint64_t widthIndex = std::round(w / ratio);
            if (heightIndex > imageBufferIn->GetHeight()) {
                heightIndex = imageBufferIn->GetHeight() == 0 ? 0 : imageBufferIn->GetHeight() - 1;
            }
            if (widthIndex > imageBufferIn->GetWidth()) {
                widthIndex = imageBufferIn->GetWidth() == 0 ? 0 : imageBufferIn->GetWidth() - 1;
            }
            imageRowOut[h * outRowStride +  w * components] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components];
            imageRowOut[h * outRowStride +  w * components + COLOR_CHANNEL_GREEN] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components + COLOR_CHANNEL_GREEN];
            imageRowOut[h * outRowStride +  w * components + COLOR_CHANNEL_BLUE] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components + COLOR_CHANNEL_BLUE];
            imageRowOut[h * outRowStride +  w * components + COLOR_CHANNEL_ALPHA] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components + COLOR_CHANNEL_ALPHA];
        }
    }
    return 0;
}

int32_t ImageCompress::DecodeJPGFile(std::string fileName, std::shared_ptr<ImageBuffer>& imageBuffer)
{
    if (imageBuffer == nullptr) {
        APP_LOGE("ImageCompress: DecodeJPGFile imageBuffer is unavailable");
        return -1;
    }
    if (!IsPathValid(fileName)) {
        APP_LOGE("%{public}s is unavailable", fileName.c_str());
        return -1.0;
    }
    double ratio = CalRatio(fileName);
    if (ratio < 0) {
        APP_LOGE("ImageCompress: DecodePngFile file %{public}s do not need compress", fileName.c_str());
        return -1;
    }
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    FILE* inFile = fopen(fileName.c_str(), "rb");
    if (!inFile) {
        APP_LOGE("ImageCompress: DecodeJPGFile file %{public}s is unavailable", fileName.c_str());
        return -1;
    }

    imageBuffer->SetRatio(ratio);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inFile);
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);
    imageBuffer->SetWidth(cinfo.output_width);
    imageBuffer->SetHeight(cinfo.output_height);
    imageBuffer->SetComponents(RGB_COMPONENTS);
    imageBuffer->MallocImageMap(RGB_COMPONENTS);
    
    ImageRow imageRow = imageBuffer->GetImageDataPointer().get();
    uint32_t row_stride = cinfo.output_width * cinfo.output_components;
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, NUMBER_ONE);
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, buffer, NUMBER_ONE);
        if (memcpy_s(imageRow, row_stride, *buffer, row_stride) != EOK) {
            APP_LOGE("ImageCompress: memcpy_s buffer failed");
            fclose(inFile);
            return -1;
        }
        imageRow += row_stride;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    if (fclose(inFile) != 0) {
        return -1;
    }
    return 0;
}

int32_t ImageCompress::EncodeJPGFile(std::shared_ptr<ImageBuffer>& imageBuffer)
{

    if (imageBuffer == nullptr) {
        APP_LOGE("ImageCompress: EncodeJPGFile should read image buffer first");
        return -1;
    }
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    unsigned char* outBuffer = nullptr;
    int32_t compressSize = 0;
    jpeg_mem_dest(&cinfo, &outBuffer, reinterpret_cast<size_t *>(&compressSize));
    cinfo.image_width = imageBuffer->GetWidth();
    cinfo.image_height = imageBuffer->GetHeight();
    cinfo.input_components = RGB_COMPONENTS;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, QUALITY, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    uint32_t rowStride = imageBuffer->GetWidth() * imageBuffer->GetComponents();
    ImageRow imageRow = imageBuffer->GetImageDataPointer().get();
    JSAMPROW rowdata[1];
    while (cinfo.next_scanline < static_cast<unsigned int>(imageBuffer->GetHeight())) {
        rowdata[0] = &imageRow[cinfo.next_scanline * rowStride];
        (void)jpeg_write_scanlines(&cinfo, rowdata, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    imageBuffer->SetCompressSize(compressSize);
    imageBuffer->SetCompressData(outBuffer, compressSize);
    free(outBuffer);
    outBuffer = nullptr;
    return 0;
}

int32_t ImageCompress::ResizeRGBImage(std::shared_ptr<ImageBuffer>& imageBufferIn,
                                      std::shared_ptr<ImageBuffer>& imageBufferOut)
{
    if (imageBufferIn == nullptr || imageBufferOut == nullptr) {
        APP_LOGE("ImageCompress: ResizePRGBAImage should input image buffer");
        return -1;
    }
    double ratio = imageBufferIn->GetRatio();
    if (DoubleEqual(ratio, 0.0)) {
        return -1;
    }
    imageBufferOut->SetWidth(imageBufferIn->GetWidth() * ratio);
    imageBufferOut->SetHeight(imageBufferIn->GetHeight() * ratio);
    imageBufferOut->SetComponents(imageBufferIn->GetComponents());
    imageBufferOut->SetColorType(imageBufferIn->GetColorType());
    imageBufferOut->SetBitDepth(imageBufferIn->GetBitDepth());
    imageBufferOut->MallocImageMap(imageBufferOut->GetComponents());
    ImageRow imageRowIn = imageBufferIn->GetImageDataPointer().get();
    ImageRow imageRowOut = imageBufferOut->GetImageDataPointer().get();
    uint32_t outRowStride = imageBufferOut->GetWidth() * imageBufferOut->GetComponents();
    uint32_t inRowStride = imageBufferIn->GetWidth() * imageBufferIn->GetComponents();
    uint32_t components = imageBufferIn->GetComponents();
    for (uint32_t h = 0; h < imageBufferOut->GetHeight(); ++h) {
        for (uint32_t w = 0; w < imageBufferOut->GetWidth(); ++w) {
            uint64_t heightIndex = std::round(h / ratio);
            uint64_t widthIndex = std::round(w / ratio);
            if (heightIndex > imageBufferIn->GetHeight()) {
                heightIndex = imageBufferIn->GetHeight() == 0 ? 0 : imageBufferIn->GetHeight() - 1;
            }
            if (widthIndex > imageBufferIn->GetWidth()) {
                widthIndex = imageBufferIn->GetWidth() == 0 ? 0 : imageBufferIn->GetWidth() - 1;
            }
            imageRowOut[h * outRowStride +  w * components] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components];
            imageRowOut[h * outRowStride +  w * components + COLOR_CHANNEL_GREEN] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components + COLOR_CHANNEL_GREEN];
            imageRowOut[h * outRowStride +  w * components + COLOR_CHANNEL_BLUE] =
                imageRowIn[heightIndex * inRowStride + widthIndex * components + COLOR_CHANNEL_BLUE];
        }
    }
    return 0;
}

std::shared_ptr<ImageBuffer> ImageCompress::CompressImage(std::string inFileName)
{
    std::string inFileSuffix = inFileName.substr(inFileName.find_last_of('.') + 1);
    if (inFileSuffix.compare(PNG) != 0 && inFileSuffix.compare(JPG) != 0 && inFileSuffix.compare(JPEG) != 0) {
        return nullptr;
    }
    std::shared_ptr<ImageBuffer> imageBufferIn = std::make_shared<ImageBuffer>();
    std::shared_ptr<ImageBuffer> imageBufferOut = std::make_shared<ImageBuffer>();
    if (inFileSuffix.compare("png") == 0 || inFileSuffix.compare("PNG") == 0) {
        imageBufferIn->SetImageType(PNG);
        imageBufferOut->SetImageType(PNG);
        if (DecodePngFile(inFileName, imageBufferIn) != 0) {
            APP_LOGE("ImageCompress: DecodePngFile decode png file error");
            return nullptr;
        }
        if (ResizeRGBAImage(imageBufferIn, imageBufferOut) != 0) {
            APP_LOGE("ImageCompress: DecodePngFile resize png file error");
            return nullptr;
        }
        if (EncodePngFile(imageBufferOut) != 0) {
            return nullptr;
        }
    } else if (inFileSuffix.compare(JPG) == 0 || inFileSuffix.compare(JPEG) == 0) {
        imageBufferIn->SetImageType(JPG);
        imageBufferOut->SetImageType(JPG);
        if (DecodeJPGFile(inFileName, imageBufferIn) != 0) {
            APP_LOGE("ImageCompress: DecodeJPGFile decode jpeg file error");
            return nullptr;
        }
        if (ResizeRGBImage(imageBufferIn, imageBufferOut) != 0) {
            APP_LOGE("ImageCompress: ResizeRGBImage resize jpeg file error");
            return nullptr;
        }
        if (EncodeJPGFile(imageBufferOut) != 0) {
            APP_LOGE("ImageCompress: EncodeJPGFile jpeg file error");
            return nullptr;
        }
    } else {
        APP_LOGE("ImageCompress: input wrong image format");
        return nullptr;
    }
    return imageBufferOut;
}
}
}