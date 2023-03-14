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
#ifndef GRAPHIC_LTE_TRANSFORM_IMAGE_ACCESSORS_H
#define GRAPHIC_LTE_TRANSFORM_IMAGE_ACCESSORS_H

#include "gfx_utils/diagram/common/common_basics.h"

namespace OHOS {
/**
 * @brief Image accessor with copy
 *
 * @since 1.0
 * @version 1.0
 */
template <class PixelFormatType>
class ImageAccessorClone {
public:
    using ColorType = typename PixelFormatType::ColorType;
    using OrderType = typename PixelFormatType::OrderType;
    enum PixWidth {
        PIX_WIDTH = PixelFormatType::PIX_WIDTH
    };

    ImageAccessorClone() {}
    explicit ImageAccessorClone(PixelFormatType& pixFormat) : pixFormat_(&pixFormat) {}
    /**
     * @brief Associated GetPixelBuf set
     * @param pixFormat The associated GetPixelBuf set is required
     * @since 1.0
     * @version 1.0
     */
    void Attach(PixelFormatType& pixFormat)
    {
        pixFormat_ = &pixFormat;
    }

    /**
     * @brief Get GetPixelBuf address
     * @param x x-coordinate
     * @param y y-coordinate
     * @param len segment length
     * @return Coordinate GetPixelBuf address
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t len)
    {
        xPixel_ = x;
        xSpanPixel_ = x;
        yPixel_ = y;
        if (x >= 0 && x + len <= static_cast<int32_t>(pixFormat_->GetWidth()) &&
            y >= 0 && y < static_cast<int32_t>(pixFormat_->GetHeight())) {
            return pixPtr_ = pixFormat_->GetPixPtr(x, y);
        }
        pixPtr_ = 0;
        return GetPixelPtr();
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextX()
    {
        if (pixPtr_) {
            return pixPtr_ += PIX_WIDTH;
        }
        ++xPixel_;
        return GetPixelPtr();
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetGetNextY()
    {
        ++yPixel_;
        xPixel_ = xSpanPixel_;
        if (pixPtr_ != 0 &&
            yPixel_ >= 0 && yPixel_ < static_cast<int32_t>(pixFormat_->GetHeight())) {
            return pixPtr_ = pixFormat_->GetPixPtr(xPixel_, yPixel_);
        }
        pixPtr_ = 0;
        return GetPixelPtr();
    }

private:
    /**
     * @brief The obtained coordinates are (xPixel_, yPixel_) GetPixelPtr address of
     * @return Coordinates are (xPixel_, yPixel_) GetPixelPtr address of
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetPixelPtr() const
    {
        int32_t x = xPixel_;
        int32_t y = yPixel_;
        if (x < 0) {
            x = 0;
        }
        if (x >= static_cast<int32_t>(pixFormat_->GetWidth())) {
            x = pixFormat_->GetWidth() - 1;
        }
        if (y < 0) {
            y = 0;
        }
        if (y >= static_cast<int32_t>(pixFormat_->GetHeight())) {
            y = pixFormat_->GetHeight() - 1;
        }
        return pixFormat_->GetPixPtr(x, y);
    }
    const PixelFormatType* pixFormat_;
    int32_t xPixel_;
    int32_t xSpanPixel_;
    int32_t yPixel_;
    const uint8_t* pixPtr_;
};

template <class PixFmt>
class ImageAccessorClip {
public:
    using PixelFormatType = PixFmt;
    using ColorType =  typename PixelFormatType::ColorType;
    using OrderType =  typename PixelFormatType::OrderType;
    using ValueType =  typename PixelFormatType::ValueType;

    enum PixWidth { PIX_WIDTH = PixelFormatType::PIX_WIDTH };

    ImageAccessorClip() {}
    explicit ImageAccessorClip(PixelFormatType& pixf, const ColorType& bk)
        : pixFormat_(&pixf)
    {
        PixelFormatType::MakePix(bkBuf_, bk);
    }

    void attach(PixelFormatType& pixf)
    {
        pixFormat_ = &pixf;
    }

    void BackgroundColor(const ColorType& bk)
    {
        PixelFormatType::MakePix(bkBuf_, bk);
    }

    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t len)
    {
        x_ = x0_ = x;
        y_ = y;
        if (y >= 0 && y < static_cast<int32_t>(pixFormat_->GetHeight()) &&
            x >= 0 && x + static_cast<int32_t>(len) <= static_cast<int32_t>(pixFormat_->GetWidth())) {
            return pixPtr_ = pixFormat_->GetPixPtr(x, y);
        }
        pixPtr_ = 0;
        return GetPixelBuf();
    }

    const uint8_t* GetNextX()
    {
        if (pixPtr_) {
            return pixPtr_ += PIX_WIDTH;
        }
        ++x_;
        return GetPixelBuf();
    }

    const uint8_t* GetGetNextY()
    {
        ++y_;
        x_ = x0_;
        if (pixPtr_ &&
            y_ >= 0 && y_ < static_cast<int32_t>(pixFormat_->GetHeight())) {
            return pixPtr_ = pixFormat_->GetPixPtr(x_, y_);
        }
        pixPtr_ = 0;
        return GetPixelBuf();
    }

private:
    const uint8_t* GetPixelBuf() const
    {
        if (y_ >= 0 && y_ < static_cast<int32_t>(pixFormat_->GetHeight()) &&
            x_ >= 0 && x_ < static_cast<int32_t>(pixFormat_->GetWidth())) {
            return pixFormat_->GetPixPtr(x_, y_);
        }
        return bkBuf_;
    }

    const PixelFormatType* pixFormat_;
    uint8_t bkBuf_[PIX_WIDTH];
    int32_t x_;
    int32_t x0_;
    int32_t y_;
    const uint8_t* pixPtr_;
};

/**
 * @brief Image accessor
 * @since 1.0
 * @version 1.0
 */
template <class PixelFormatType>
class ImageAccessorNoRepeat {
public:
    using ColorType = typename PixelFormatType::ColorType;
    using OrderType = typename PixelFormatType::OrderType;
    enum PixWidth {
        PIX_WIDTH = PixelFormatType::PIX_WIDTH
    };

    ImageAccessorNoRepeat() {}
    explicit ImageAccessorNoRepeat(PixelFormatType& pixFormat) : pixFormat_(&pixFormat) {}
    /**
     * @brief Associated GetPixelBuf set
     * @param pixFormat The associated GetPixelBuf set is required
     * @since 1.0
     * @version 1.0
     */
    void Attach(PixelFormatType& pixFormat)
    {
        pixFormat_ = &pixFormat;
    }
    /**
     * @brief Get GetPixelBuf address
     * @param x x-coordinate
     * @param y y-coordinate
     * @param len segment length
     * @return Coordinate GetPixelBuf address
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t len)
    {
        xPixel_ = xSpanPixel_ = x;
        yPixel_ = y;
        if (x >= 0 && x + len <= static_cast<int32_t>(pixFormat_->GetWidth()) &&
            y >= 0 && y < static_cast<int32_t>(pixFormat_->GetHeight())) {
            return pixPtr_ = pixFormat_->GetPixPtr(x, y);
        }
        pixPtr_ = 0;
        return GetPixelPtr();
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextX()
    {
        if (pixPtr_ != 0) {
            return pixPtr_ += PIX_WIDTH;
        }
        ++xPixel_;
        return GetPixelPtr();
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetGetNextY()
    {
        ++yPixel_;
        xPixel_ = xSpanPixel_;
        if (pixPtr_ != 0 &&
            yPixel_ >= 0 && yPixel_ < static_cast<int32_t>(pixFormat_->GetHeight())) {
            return pixPtr_ = pixFormat_->pixPtr(xPixel_, yPixel_);
        }
        pixPtr_ = 0;
        return GetPixelPtr();
    }

private:
    /**
     * @brief The obtained coordinates are (xPixel_, yPixel_) GetPixelPtr address of
     * @return Coordinates are (xPixel_, yPixel_) GetPixelPtr address of
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetPixelPtr() const
    {
        int32_t x = xPixel_;
        int32_t y = yPixel_;
        if (x < 0) {
            x = 0;
        }
        if (x >= static_cast<int32_t>(pixFormat_->GetWidth())) {
            x = pixFormat_->GetWidth() - 1;
            return NULL;
        }
        if (y < 0) {
            y = 0;
        }
        if (y >= static_cast<int32_t>(pixFormat_->GetHeight())) {
            y = pixFormat_->GetHeight() - 1;
            return NULL;
        }

        return pixFormat_->GetPixPtr(x, y);
    }

    const PixelFormatType* pixFormat_;
    int32_t xPixel_;
    int32_t xSpanPixel_;
    int32_t yPixel_;
    const uint8_t* pixPtr_;
};

/**
 * @brief Image accessor with line feed function
 *
 * @since 1.0
 * @version 1.0
 */
template <class PixelFormatType, class WrapX, class WrapY>
class ImageAccessorWrap {
public:
    using ColorType = typename PixelFormatType::ColorType;
    using OrderType = typename PixelFormatType::OrderType;
    enum PixWidth {
        PIX_WIDTH = PixelFormatType::PIX_WIDTH
    };

    ImageAccessorWrap() {}
    explicit ImageAccessorWrap(PixelFormatType& pixFormat)
        : pixFormat_(&pixFormat),
          wrapX_(pixFormat.GetWidth()),
          wrapY_(pixFormat.GetHeight()) {}

    /**
     * @brief Associated GetPixelBuf set
     * @param pixFormat The associated GetPixelBuf set is required
     * @since 1.0
     * @version 1.0
     */
    void Attach(PixelFormatType& pixFormat)
    {
        pixFormat_ = &pixFormat;
    }
    /**
     * @brief Get GetPixelBuf address with line feed
     * @param x x-coordinate
     * @param y y-coordinate
     * @return Coordinate GetPixelBuf address
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t)
    {
        x_ = x;
        rowPtr_ = pixFormat_->GetPixPtr(0, wrapY_(y));
        return rowPtr_ + wrapX_(x) * PIX_WIDTH;
    }
    /**
     * @brief Add one GetPixelBuf width to the GetPixelBuf address, with line feed function
     * @return Add one GetPixelBuf width to the GetPixelBuf address, with line feed function
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextX()
    {
        int32_t x = ++wrapX_;
        return rowPtr_ + x * PIX_WIDTH;
    }

    /**
     * @brief Add one GetPixelBuf width to the GetPixelBuf address, with line feed function
     * @return Add one GetPixelBuf width to the GetPixelBuf address, with line feed function
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextY()
    {
        rowPtr_ = pixFormat_->pixPtr(0, ++wrapY_);
        return rowPtr_ + wrapX_(x_) * PIX_WIDTH;
    }

private:
    const PixelFormatType* pixFormat_;
    const uint8_t* rowPtr_;
    int32_t x_;
    WrapX wrapX_;
    WrapY wrapY_;
};

/**
 * @brief An image accessor in which images are repeated on the x-axis
 *
 * @since 1.0
 * @version 1.0
 */
template <class PixelFormatType, class WrapX>
class ImageAccessorRepeatX {
public:
    using ColorType = typename PixelFormatType::ColorType;
    using OrderType = typename PixelFormatType::OrderType;
    enum PixWidth {
        PIX_WIDTH = PixelFormatType::PIX_WIDTH
    };

    ImageAccessorRepeatX() {}
    explicit ImageAccessorRepeatX(PixelFormatType& pixFormat)
        : pixFormat_(&pixFormat),
          wrapX_(pixFormat.GetWidth()) {}
    /**
     * @brief Associated GetPixelBuf set
     * @param pixFormat The associated GetPixelBuf set is required
     * @since 1.0
     * @version 1.0
     */
    void Attach(PixelFormatType& pixFormat)
    {
        pixFormat_ = &pixFormat;
    }
    /**
     * @brief Get GetPixelBuf address
     * @param x x-coordinate
     * @param y y-coordinate
     * @param len segment length
     * @return Coordinate GetPixelBuf address
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t len)
    {
        xPixel_ = x;
        yPixel_ = y;
        if (y >= static_cast<int32_t>(pixFormat_->GetHeight())) {
            y = pixFormat_->GetHeight() - 1;
            return NULL;
        }
        rowPtr_ = pixFormat_->GetPixPtr(0, y);
        return rowPtr_ + wrapX_(x) * PIX_WIDTH;
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextX()
    {
        if (yPixel_ >= static_cast<int32_t>(pixFormat_->GetHeight())) {
            return NULL;
        }
        int32_t x = ++wrapX_;
        return rowPtr_ + x * PIX_WIDTH;
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextY()
    {
        rowPtr_ = pixFormat_->pixPtr(0, yPixel_);
        return rowPtr_ + wrapX_(xPixel_) * PIX_WIDTH;
    }

private:
    const PixelFormatType* pixFormat_;
    const uint8_t* rowPtr_;
    int32_t xPixel_;
    int32_t yPixel_;
    WrapX wrapX_;
};
/**
 * @brief Image accessor for image repetition on y-axis
 *
 * @since 1.0
 * @version 1.0
 */
template <class PixelFormatType, class WrapY>
class ImageAccessorRepeatY {
public:
    using ColorType = typename PixelFormatType::ColorType;
    using OrderType = typename PixelFormatType::OrderType;
    enum PixWidth {
        PIX_WIDTH = PixelFormatType::PIX_WIDTH
    };

    ImageAccessorRepeatY() {}
    explicit ImageAccessorRepeatY(PixelFormatType& pixFormat)
        : pixFormat_(&pixFormat), wrapY_(pixFormat.GetHeight()) {}
    /**
     * @brief Associated GetPixelBuf set
     * @param pixFormat The associated GetPixelBuf set is required
     * @since 1.0
     * @version 1.0
     */
    void Attach(PixelFormatType& pixFormat)
    {
        pixFormat_ = &pixFormat;
    }
    /**
     * @brief Get GetPixelBuf address
     * @param x x-coordinate
     * @param y y-coordinate
     * @return Coordinate GetPixelBuf address
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetSpan(int32_t x, int32_t y, uint32_t)
    {
        xPixel_ = x;
        if (x >= static_cast<int32_t>(pixFormat_->GetWidth())) {
            x = pixFormat_->GetWidth() - 1;
            return NULL;
        }
        rowPtr_ = pixFormat_->GetPixPtr(0, wrapY_(y));
        return rowPtr_ + x * PIX_WIDTH;
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetNextX()
    {
        int32_t x = ++xPixel_;
        if (x >= static_cast<int32_t>(pixFormat_->GetWidth())) {
            x = pixFormat_->GetWidth() - 1;
            return NULL;
        }
        return rowPtr_ + x * PIX_WIDTH;
    }
    /**
     * @brief GetPixelPtr address increases by one GetPixelBuf width
     * @return Coordinates are changed GetPixelBuf addresses
     * @since 1.0
     * @version 1.0
     */
    const uint8_t* GetGetNextY()
    {
        rowPtr_ = pixFormat_->pixPtr(0, ++wrapY_);
        return rowPtr_ + xPixel_ * PIX_WIDTH;
    }

private:
    const PixelFormatType* pixFormat_;
    const uint8_t* rowPtr_;
    int32_t xPixel_;
    WrapY wrapY_;
};

class WrapModeRepeat {
public:
    WrapModeRepeat() {}
    WrapModeRepeat(uint32_t size)
        : size_(size), add_(size == 0 ? 0 : (size * (0x3FFFFFFF / size))), value_(0) {}

    uint32_t operator()(int32_t value)
    {
        return value_ = (uint32_t(value) + add_) % size_;
    }

    uint32_t operator++()
    {
        ++value_;
        if (value_ >= size_) {
            value_ = 0;
        }
        return value_;
    }

private:
    uint32_t size_;
    uint32_t add_;
    uint32_t value_;
};
} // namespace OHOS
#endif
