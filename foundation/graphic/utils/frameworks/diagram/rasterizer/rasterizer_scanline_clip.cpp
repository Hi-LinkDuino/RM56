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

#include "gfx_utils/diagram/rasterizer/rasterizer_scanline_clip.h"

namespace OHOS {
/**
 * @brief In the RASTERIZER process,Judge the mark according to the last clipping range
 * And the cutting range judgment flag this time,
 * add the actual sampling points and set relevant attributes
 * @since 1.0
 * @version 1.0
 */
void RasterizerScanlineClip::LineClipY(RasterizerCellsAntiAlias& ras,
                                       int32_t x1, int32_t y1,
                                       int32_t x2, int32_t y2,
                                       uint32_t clipFlagsOne, uint32_t clipFlagsTwo) const
{
    clipFlagsOne &= DIRECTLY_BELOW;
    clipFlagsTwo &= DIRECTLY_BELOW;
    if ((clipFlagsOne | clipFlagsTwo) == 0) {
        /**
         * It indicates that the coordinates x1, y1, x2 and y2 are all within the range, and the line operates
         */
        ras.LineOperate(RasterDepictInt::GetXCoordinateValue(x1),
                        RasterDepictInt::GetYCoordinateValue(y1),
                        RasterDepictInt::GetXCoordinateValue(x2),
                        RasterDepictInt::GetYCoordinateValue(y2));
    } else {
        if (clipFlagsOne == clipFlagsTwo) {
            /**
             * It indicates that the coordinates x1, y1, x2 and y2 are all outside the range and do not operate
             */
            return;
        }
        int32_t tx1 = x1;
        int32_t ty1 = y1;
        int32_t tx2 = x2;
        int32_t ty2 = y2;
        /**
         * Indicates that the coordinate y1 < clip.y1
         */
        if (clipFlagsOne & 0x08) {
            tx1 = x1 + RasterDepictInt::MultDiv(clipBox_.GetTop() - y1, x2 - x1, y2 - y1);
            ty1 = clipBox_.GetTop();
        }

        /**
         * Indicates that the coordinate y1 > clip.y2
         */
        if (clipFlagsOne & 0x02) {
            tx1 = x1 + RasterDepictInt::MultDiv(clipBox_.GetBottom() - y1, x2 - x1, y2 - y1);
            ty1 = clipBox_.GetBottom();
        }
        /**
         * Indicates that the coordinate y1 > clip.y2
         */
        if (clipFlagsTwo & 0x08) {
            tx2 = x1 + RasterDepictInt::MultDiv(clipBox_.GetTop() - y1, x2 - x1, y2 - y1);
            ty2 = clipBox_.GetTop();
        }
        /**
         * Indicates that the coordinate y2 > clip.y2
         */
        if (clipFlagsTwo & 0x02) {
            tx2 = x1 + RasterDepictInt::MultDiv(clipBox_.GetBottom() - y1, x2 - x1, y2 - y1);
            ty2 = clipBox_.GetBottom();
        }
        ras.LineOperate(RasterDepictInt::GetXCoordinateValue(tx1), RasterDepictInt::GetYCoordinateValue(ty1),
                        RasterDepictInt::GetXCoordinateValue(tx2), RasterDepictInt::GetYCoordinateValue(ty2));
    }
}

/**
 * @brief Cohen–Sutherland
 * In the RASTERIZER process, add the set sampling point,
 * And set the sampling point, set the related cover and area attributes, etc.
 *         |        |
 *   0110  |  0010  | 0011
 *         |        |
 *  -------+--------+-------- clip_box.y2
 *         |        |
 *   0100  |  0000  | 0001
 *         |        |
 *  -------+--------+-------- clip_box.y1
 *         |        |
 *   1100  |  1000  | 1001
 *         |        |
 *   clip_box.x1  clip_box.x2
 * @since 1.0
 * @version 1.0
 */
void RasterizerScanlineClip::LineTo(RasterizerCellsAntiAlias& rasterLine, int32_t x2, int32_t y2)
{
    if (clipping_) {
        uint32_t cFlagsLineToPoint = ClippingFlags(x2, y2, clipBox_);
        if ((clippingFlags_ & 0x0A) == (cFlagsLineToPoint & 0x0A) && (clippingFlags_ & 0x0A) != 0) {
            x1_ = x2;
            y1_ = y2;
            clippingFlags_ = cFlagsLineToPoint;
            return;
        }

        int32_t x1 = x1_;
        int32_t y1 = y1_;
        uint32_t clipFlagsMoveToPoint = clippingFlags_;
        int32_t yPilotOne;
        int32_t yPilotTwo;
        uint32_t yClipFlagsOne = 0;
        uint32_t yClipFlagsTwo = 0;

        switch (((clipFlagsMoveToPoint & 0x05) << 1) | (cFlagsLineToPoint & 0x05)) {
            /**
             * It indicates that x1, y1, x2,and y2 are all in the clip area
             */
            case 0x00:
                LineClipY(rasterLine, x1, y1, x2, y2, clipFlagsMoveToPoint, cFlagsLineToPoint);
                break;
            /**
             * indicate x2 > clip.x2
             */
            case 0x01:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetRight() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                LineClipY(rasterLine, x1, y1, clipBox_.GetRight(), yPilotOne,
                          clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetRight(), yPilotOne,
                          clipBox_.GetRight(), y2, yClipFlagsOne, cFlagsLineToPoint);
                break;
            /**
             * indicate x1 > clip.x2
             */
            case 0x02:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetRight() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                LineClipY(rasterLine, clipBox_.GetRight(), y1, clipBox_.GetRight(),
                          yPilotOne, clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetRight(), yPilotOne, x2, y2,
                          yClipFlagsOne, cFlagsLineToPoint);
                break;
            /**
             * indicate x1 > clip.x2 && x2 > clip.x2
             */
            case 0x03:
                LineClipY(rasterLine, clipBox_.GetRight(), y1, clipBox_.GetRight(), y2,
                          clipFlagsMoveToPoint, cFlagsLineToPoint);
                break;
            /**
             * indicate x2 < clip.x1
             */
            case 0x04:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetLeft() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                LineClipY(rasterLine, x1, y1, clipBox_.GetLeft(), yPilotOne,
                          clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetLeft(), yPilotOne, clipBox_.GetLeft(), y2,
                          yClipFlagsOne, cFlagsLineToPoint);
                break;
            /**
             * indicate x1 > clip.x2 && x2 < clip.x1
             */
            case 0x06:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetRight() - x1, y2 - y1, x2 - x1);
                yPilotTwo = y1 + RasterDepictInt::MultDiv(clipBox_.GetLeft() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                yClipFlagsTwo = ClippingFlagsY(yPilotTwo, clipBox_);
                LineClipY(rasterLine, clipBox_.GetRight(), y1, clipBox_.GetRight(), yPilotOne,
                          clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetRight(), yPilotOne, clipBox_.GetLeft(), yPilotTwo,
                          yClipFlagsOne, yClipFlagsTwo);
                LineClipY(rasterLine, clipBox_.GetLeft(), yPilotTwo, clipBox_.GetLeft(), y2,
                          yClipFlagsTwo, cFlagsLineToPoint);
                break;

            /**
             * indicate x1 < clip.x1
             */
            case 0x08:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetLeft() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                LineClipY(rasterLine, clipBox_.GetLeft(), y1, clipBox_.GetLeft(), yPilotOne,
                          clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetLeft(), yPilotOne, x2, y2,
                          yClipFlagsOne, cFlagsLineToPoint);
                break;
            /**
             * indicate x1 < clip.x1 && x2 > clip.x2
             */
            case 0x09:
                yPilotOne = y1 + RasterDepictInt::MultDiv(clipBox_.GetLeft() - x1, y2 - y1, x2 - x1);
                yPilotTwo = y1 + RasterDepictInt::MultDiv(clipBox_.GetRight() - x1, y2 - y1, x2 - x1);
                yClipFlagsOne = ClippingFlagsY(yPilotOne, clipBox_);
                yClipFlagsTwo = ClippingFlagsY(yPilotTwo, clipBox_);
                LineClipY(rasterLine, clipBox_.GetLeft(), y1, clipBox_.GetLeft(), yPilotOne,
                          clipFlagsMoveToPoint, yClipFlagsOne);
                LineClipY(rasterLine, clipBox_.GetLeft(), yPilotOne, clipBox_.GetRight(), yPilotTwo,
                          yClipFlagsOne, yClipFlagsTwo);
                LineClipY(rasterLine, clipBox_.GetRight(), yPilotTwo, clipBox_.GetRight(), y2,
                          yClipFlagsTwo, cFlagsLineToPoint);
                break;
            /**
             * indicate x1 < clip.x1 && x2 < clip.x1
             */
            case 0x0c:
                LineClipY(rasterLine, clipBox_.GetLeft(), y1, clipBox_.GetLeft(), y2,
                          clipFlagsMoveToPoint, cFlagsLineToPoint);
                break;
        }
        clippingFlags_ = cFlagsLineToPoint;
    } else {
        rasterLine.LineOperate(RasterDepictInt::GetXCoordinateValue(x1_),
                               RasterDepictInt::GetYCoordinateValue(y1_),
                               RasterDepictInt::GetXCoordinateValue(x2),
                               RasterDepictInt::GetYCoordinateValue(y2));
    }
    x1_ = x2;
    y1_ = y2;
}
} // namespace OHOS
