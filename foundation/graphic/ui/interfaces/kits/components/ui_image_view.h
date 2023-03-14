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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_image_view.h
 *
 * @brief Declares an image view.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_IMAGE_VIEW_H
#define GRAPHIC_LITE_UI_IMAGE_VIEW_H

#include "common/image.h"
#include "components/ui_view.h"
#include "gfx_utils/graphic_types.h"
#if (ENABLE_GIF == 1)
#include "animator/animator.h"
#endif

namespace OHOS {
/**
 * @brief Defines the functions related to an image view.
 *
 * @since 1.0
 * @version 1.0
 */
class UIImageView : public UIView {
public:
    /**
     * @brief A default constructor used to create a <b>UIImageView</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIImageView();

    /**
     * @brief A destructor used to delete the <b>UIImageView</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIImageView();

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_IMAGE_VIEW</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_IMAGE_VIEW;
    }

    /**
     * @brief Obtains the width of this image view.
     *
     * @return Returns the width of this image view.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetWidth() override
    {
        if (needRefresh_ && autoEnable_) {
            ReMeasure();
        }
        return UIView::GetWidth();
    }

    /**
     * @brief Obtains the height of this image view.
     *
     * @return Returns the height of this image view.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetHeight() override
    {
        if (needRefresh_ && autoEnable_) {
            ReMeasure();
        }
        return UIView::GetHeight();
    }

    /**
     * @brief Checks whether this image view needs to be covered to optimize the drawing process.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @return Returns <b>true</b> if this image view needs to be covered; returns <b> false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool OnPreDraw(Rect& invalidatedArea) const override;

    /**
     * @brief Draws this image view.
     *
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Sets the image path.
     *
     * @param src Indicates the pointer to the image path represented by a string.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetSrc(const char* src);

    /**
     * @brief Sets the image information.
     *
     * @param src Indicates the pointer to the image information. For details, see {@link ImageInfo}.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetSrc(const ImageInfo* src);

    /**
     * @brief Sets whether the image view size needs to be adaptive to the image size.
     *
     * @param enable Specifies whether the image view size needs to be adaptive to the image size.
     * <b>true</b> indicates that automatic adaption is enabled, and <b> false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void SetAutoEnable(bool enable)
    {
        if (autoEnable_ != enable) {
            needRefresh_ = autoEnable_ ? needRefresh_ : true;
            autoEnable_ = enable;
            UpdateDrawTransMap(true);
        }
    }

    /*
     * @brief Checks whether automatic adaptation is enabled.
     *
     * @return Returns <b>true</b> if automatic adaptation is enabled; returns <b> false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetAutoEnable() const
    {
        return autoEnable_;
    }

    /**
     * @brief Sets the blur level for this image when it is rotated or scaled.
     *
     * @param level Indicates the blur level to set. For details, see {@link BlurLevel}.
     * @since 1.0
     * @version 1.0
     */
    void SetBlurLevel(BlurLevel level)
    {
        blurLevel_ = level;
    }

    /**
     * @brief Obtains the blur level of this image when it is rotated or scaled.
     *
     * @return Returns the blur level of this image, as defined in {@link BlurLevel}.
     * @since 1.0
     * @version 1.0
     */
    BlurLevel GetBlurLevel() const
    {
        return static_cast<BlurLevel>(blurLevel_);
    }

    /**
     * @brief Sets the algorithm used for image rotation and scaling.
     *
     * @param algorithm Indicates the image transformation algorithm. For details, see {@link TransformAlgorithm}.
     * @since 1.0
     * @version 1.0
     */
    void SetTransformAlgorithm(TransformAlgorithm algorithm)
    {
        algorithm_ = algorithm;
    }

    /**
     * @brief Obtains the algorithm used for image rotation and scaling.
     *
     * @return Returns the image transform algorithm, as defined in {@link TransformAlgorithm}.
     * @since 1.0
     * @version 1.0
     */
    TransformAlgorithm GetTransformAlgorithm() const
    {
        return static_cast<TransformAlgorithm>(algorithm_);
    }

    /**
     * @brief Obtains the image path in binary.
     *
     * @return Returns the pointer to the image path.
     * @since 1.0
     * @version 1.0
     */
    const char* GetPath() const
    {
        return image_.GetPath();
    }

    /**
     * @brief Obtains the image information in an array.
     *
     * @return Returns the pointer to the image information.
     * @since 1.0
     * @version 1.0
     */
    const ImageInfo* GetImageInfo() const
    {
        return image_.GetImageInfo();
    }

    /**
     * @brief Obtains the image type.
     *
     * @return Returns <b>IMG_SRC_VARIABLE</b> for image information in an array; returns <b>IMG_SRC_FILE</b> for an
     * image path in binary.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetSrcType() const
    {
        return image_.GetSrcType();
    }

    enum ImageResizeMode : uint8_t {
        NONE,
        COVER,
        CONTAIN,
        FILL,
        CENTER,
        SCALE_DOWN,
    };
    void SetResizeMode(ImageResizeMode mode);
    void SetWidth(int16_t width) override;
    void SetHeight(int16_t height) override;
#if (ENABLE_GIF == 1)
    Animator* GetGifImageAnimator() const
    {
        return gifImageAnimator_;
    }
#endif
protected:
    /**
     * @brief Represents the width of this image.
     */
    int16_t imageWidth_;
    /**
     * @brief Represents the height of this image.
     */
    int16_t imageHeight_;
    /**
     * @brief Specifies whether automatic adaptation is enabled.
     */
    bool autoEnable_;
    /**
     * @brief Specifies whether a refresh is needed.
     */
    bool needRefresh_;
    /**
     * @brief Represents the color format of this image.
     */
    uint8_t colorFormat_;
    /**
     * @brief Represents the blur level of this image when it is rotated or scaled.
     */
    uint8_t blurLevel_ : 2;
    /**
     * @brief Represents the algorithm used for image rotation and scaling.
     */
    uint8_t algorithm_ : 1;
    uint8_t reserve_ : 1;
    Image image_;
    ImageResizeMode imageResizeMode_ = ImageResizeMode::NONE;
    TransformMap* drawTransMap_ = nullptr;
    Matrix4<float>* contentMatrix_ = nullptr;
    bool transMapInvalid_ = true;

private:
    void ReMeasure() override;
#if (ENABLE_GIF == 1)
    friend class GifImageAnimator;
    void AddAndStartGifAnimator();
    void RemoveAndStopGifAnimator();
    Animator* gifImageAnimator_;
    bool gifFrameFlag_;
#endif
    void UpdateContentMatrix();
    void UpdateDrawTransMap(bool updateContentMatrix = false);
    void AdjustScaleAndTranslate(Vector3<float>& scale, Vector3<int16_t>& translate,
        int16_t widgetWidth, int16_t widgetHeight) const;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_IMAGE_VIEW_H
