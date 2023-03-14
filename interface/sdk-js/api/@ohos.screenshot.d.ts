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

import { AsyncCallback, ErrorCallback } from './basic';
import image from './@ohos.multimedia.image';

/**
 * interface for screenshot
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @systemapi Hide this for inner system use.
 * @since 7
 */
declare namespace screenshot {
    /**
     * Takes a screenshot and saves it as a PixelMap object
     * @param options Screenshot options, which consist of screenRect, imageSize, and rotation. You need to set these parameters
     * @permission ohos.permission.CAPTURE_SCREEN
     * @since 7
     */
    function save(options?: ScreenshotOptions, callback: AsyncCallback<image.PixelMap>): void;

    /**
     * Takes a screenshot and saves it as a PixelMap object
     * @param options Screenshot options, which consist of screenRect, imageSize, and rotation. You need to set these parameters
     * @permission ohos.permission.CAPTURE_SCREEN
     * @since 7
     */
    function save(options?: ScreenshotOptions): Promise<image.PixelMap>;

    /**
     * Describes the region of the screen to capture
     * @since 7
     */
    interface Rect {
        left: number;
        top: number;
        width: number;
        height: number;
    }

    /**
     * Describes the size of the screen region to capture
     * @since 7
     */
    interface Size {
        width: number;
        height: number;
    }

    /**
     * Describes screenshot options
     * @since 7
     */
    interface ScreenshotOptions {
        /**
         * Region of the screen to capture. If this parameter is null, the full screen will be captured
         */
        screenRect?: Rect;
        /**
         * Region of the screen to capture. If this parameter is null, the full screen will be captured
         */
        imageSize?: Size;
        /**
         * Rotation angle of the screenshot. The value can be 0, 90, 180, or 270. The default value is 0
         */
        rotation?: number;
        /**
         * The specify screen to be captured.
         * @since 8
         */
        displayId?: number;
    }
}

export default screenshot;
