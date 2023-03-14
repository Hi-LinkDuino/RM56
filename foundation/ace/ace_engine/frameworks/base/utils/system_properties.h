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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_SYSTEM_PROPERTIES_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_SYSTEM_PROPERTIES_H

#include <string>

#include "base/utils/resource_configuration.h"
#include "base/utils/device_type.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

enum class ResolutionType : int32_t {
    RESOLUTION_NONE = -2,
    RESOLUTION_ANY = -1,
    RESOLUTION_LDPI = 120,
    RESOLUTION_MDPI = 160,
    RESOLUTION_HDPI = 240,
    RESOLUTION_XHDPI = 320,
    RESOLUTION_XXHDPI = 480,
    RESOLUTION_XXXHDPI = 640,
};

constexpr int32_t MCC_UNDEFINED = 0;
constexpr int32_t MNC_UNDEFINED = 0;

enum class LongScreenType : int32_t {
    LONG = 0,
    NOT_LONG,
    LONG_SCREEN_UNDEFINED,
};

enum class ScreenShape : int32_t {
    ROUND = 0,
    NOT_ROUND,
    SCREEN_SHAPE_UNDEFINED,
};

class ACE_EXPORT SystemProperties final {
public:
    /*
     * Init device type for Ace.
     */
    static void InitDeviceType(DeviceType deviceType);

    /*
     * Init device info for Ace.
     */
    static void InitDeviceInfo(int32_t deviceWidth, int32_t deviceHeight, int32_t orientation,
        double resolution, bool isRound);

    /*
     * Init device type according to system property.
     */
    static void InitDeviceTypeBySystemProperty();

    /*
     * Get type of current device.
     */
    static DeviceType GetDeviceType();

    /*
     * check SystemCapability.
     */
    static bool IsSyscapExist(const char *cap);

    /**
     * Set type of current device.
     * @param deviceType
     */
    static void SetDeviceType(DeviceType deviceType)
    {
        deviceType_ = deviceType;
    }

    /*
     * Get current orientation of device.
     */
    static DeviceOrientation GetDevcieOrientation()
    {
        return orientation_;
    }

    /*
     * Get width of device.
     */
    static int32_t GetDeviceWidth()
    {
        return deviceWidth_;
    }

    /*
     * Get height of device.
     */
    static int32_t GetDeviceHeight()
    {
        return deviceHeight_;
    }

    /*
     * Get wght scale of device.
     */
    static float GetFontWeightScale();

    /*
     * Get resolution of device.
     */
    static double GetResolution()
    {
        return resolution_;
    }

    /*
     * Set resolution of device.
     */
    static void SetResolution(double resolution)
    {
        resolution_ = resolution;
    }

    static bool GetIsScreenRound()
    {
        return isRound_;
    }

    static const std::string& GetBrand()
    {
        return brand_;
    }

    static const std::string& GetManufacturer()
    {
        return manufacturer_;
    }

    static const std::string& GetModel()
    {
        return model_;
    }

    static const std::string& GetProduct()
    {
        return product_;
    }

    static const std::string& GetApiVersion()
    {
        return apiVersion_;
    }

    static const std::string& GetReleaseType()
    {
        return releaseType_;
    }

    static const std::string& GetParamDeviceType()
    {
        return paramDeviceType_;
    }

    static bool GetRosenBackendEnabled()
    {
        return rosenBackendEnabled_;
    }

    static bool GetTraceEnabled()
    {
        return traceEnabled_;
    }

    static bool GetAccessibilityEnabled()
    {
        return accessibilityEnabled_;
    }

    static bool GetDebugEnabled();

    /*
     * Set device orientation.
     */
    static void SetDeviceOrientation(int32_t orientation);

    static constexpr char INVALID_PARAM[] = "N/A";

    static int32_t GetMcc()
    {
        return mcc_;
    }

    static int32_t GetMnc()
    {
        return mnc_;
    }

    static void SetColorMode(ColorMode colorMode)
    {
        if (colorMode_ != colorMode) {
            colorMode_ = colorMode;
        }
    }

    static ColorMode GetColorMode()
    {
        return colorMode_;
    }

    static void InitMccMnc(int32_t mcc, int32_t mnc);

    static ScreenShape GetScreenShape()
    {
        return screenShape_;
    }

    /*
     * Change px to vp
     */
    static double Px2Vp(double pxNum)
    {
        return pxNum / resolution_;
    }

    static double Vp2Px(double pxNum)
    {
        return pxNum * resolution_;
    }

    static int GetArkProperties();

    static std::string GetAsmInterOption();

    static bool IsScoringEnabled(const std::string& name);

    static void SetWindowPos(int32_t x, int32_t y)
    {
        windowPosX_ = x;
        windowPosY_ = y;
    }

    static int32_t GetWindowPosX()
    {
        return windowPosX_;
    }

    static int32_t GetWindowPosY()
    {
        return windowPosY_;
    }

    static bool IsWindowSizeAnimationEnabled()
    {
        return windowAnimationEnabled_;
    }

private:
    static bool traceEnabled_;
    static bool accessibilityEnabled_;
    static bool isRound_;
    static int32_t deviceWidth_;
    static int32_t deviceHeight_;
    static double resolution_;
    static DeviceType deviceType_;
    static DeviceOrientation orientation_;
    static std::string brand_;
    static std::string manufacturer_;
    static std::string model_;
    static std::string product_;
    static std::string apiVersion_;
    static std::string releaseType_;
    static std::string paramDeviceType_;
    static int32_t mcc_;
    static int32_t mnc_;
    static ColorMode colorMode_;
    static ScreenShape screenShape_;
    static LongScreenType LongScreen_;
    static bool rosenBackendEnabled_;
    static bool windowAnimationEnabled_;
    static bool debugEnabled_;
    static int32_t windowPosX_;
    static int32_t windowPosY_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_SYSTEM_PROPERTIES_H
