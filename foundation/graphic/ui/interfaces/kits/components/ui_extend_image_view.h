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
 * @file ui_extend_image_view.h
 *
 * @brief Defines the Canvas GIF image handling class.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GRAPHIC_LITE_UI_EXTEMD_IMAGE_VIEW_H
#define GRAPHIC_LITE_UI_EXTEMD_IMAGE_VIEW_H

#include "components/ui_image_view.h"

namespace OHOS {
class UIExtendImageView : public UIImageView {
public:
    virtual void SetSrc(const char* src);
    virtual void SetSrc(const ImageInfo* src);
    void SetCanvas(UIView* view);

private:
    UIView* canvas_;
};
}
#endif // GRAPHIC_LITE_UI_EXTEMD_IMAGE_VIEW_H
