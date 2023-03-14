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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_IMAGE_DATA_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_IMAGE_DATA_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"

namespace OHOS::Ace::Framework {

class JSCanvasImageData : public Referenced {
public:
    JSCanvasImageData();
    ~JSCanvasImageData() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSCanvasImageData* scroller);

    void getData(const JSCallbackInfo& args);

    double width_ = 0;
    double height_ = 0;

    double GetWidth() const;
    double GetHeight() const;

    void setX(int32_t x)
    {
        x_ = x;
    }
    int32_t getX() const
    {
        return x_;
    }
    void setY(int32_t y)
    {
        y_ = y;
    }
    int32_t getY() const
    {
        return y_;
    }
    void setDirtyX(int32_t dirtyX)
    {
        dirtyX_ = dirtyX;
    }
    int32_t getDirtyX() const
    {
        return dirtyX_;
    }
    void setDirtyY(int32_t dirtyY)
    {
        dirtyY_ = dirtyY;
    }
    int32_t getDirtyY() const
    {
        return dirtyY_;
    }
    void setDirtyWidth(int32_t dirtyWidth)
    {
        dirtyWidth_ = dirtyWidth;
    }
    int32_t getDirtyWidth() const
    {
        return dirtyWidth_;
    }
    void setDirtyHeight(int32_t dirtyHeight)
    {
        dirtyHeight_ = dirtyHeight;
    }
    int32_t getDirtyHeight() const
    {
        return dirtyHeight_;
    }
    void setData(const std::vector<Color>& data)
    {
        data_ = data;
    }

private:
    int32_t x_ = 0;
    int32_t y_ = 0;
    int32_t dirtyX_ = 0;
    int32_t dirtyY_ = 0;
    int32_t dirtyWidth_ = 0;
    int32_t dirtyHeight_ = 0;
    std::vector<Color> data_;

    ACE_DISALLOW_COPY_AND_MOVE(JSCanvasImageData);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_CANVAS_IMAGE_DATA_H