/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CHARGER_UI_ANIMATION_LABLE_H
#define CHARGER_UI_ANIMATION_LABLE_H
#include <ctime>
#include <thread>
#include <unistd.h>
#include <vector>
#include <memory>
#include "frame.h"
#include "png.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class AnimationLabel : public View {
public:
    enum class PlayMode {
        ANIMATION_MODE = 0,
        STATIC_MODE,
    };
    AnimationLabel(int32_t startX, int32_t startY, int32_t w, int32_t h, Frame* mParent);
    AnimationLabel() = default;
    ~AnimationLabel() override;
public:
    void AddImg(const std::string& imgFileName);
    int32_t AddStaticImg(const std::string& imgFileName);
    void SetStaticImg(int32_t picId);
    void SetPlayMode(AnimationLabel::PlayMode mode);
    void SetInterval(int32_t ms);
    std::thread updateThread;
    bool selectable = false;
    static void SetIsVisible(bool visible);
    void UpdateLoop();
    static bool needStop_;
private:
    struct PictureAttr {
        png_uint_32 pictureWidth;
        png_uint_32 pictureHeight;
        png_byte pictureChannels;
        int32_t bitDepth;
        int32_t colorType;
    };
    void* LoadPng(const std::string& imgFileName);
    View::BRGA888Pixel* HandleLoadPng(FILE** fp, char** pictureBufferTmp, struct PictureAttr& attr);
    int32_t LoadPngInternalWithFile(FILE* fp, png_structpp pngPtr, png_infopp pngInfoPtr, struct PictureAttr& attr);
    void CopyPictureBuffer(struct PictureAttr& attr, char* pictureBufferTmp, BRGA888Pixel* pictureBuffer) const;
    Frame* parent_ {};
    uint32_t intervalMs_ = 50;
    std::vector<char*> imgList_ {};
    char* staticImgList_[255] {};
    int32_t staticShowId_ = 0;
    bool showStatic_ = false;
    int32_t staticImgSize_ = 0;
    static bool isVisible_;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_ANIMATION_LABLE_H
