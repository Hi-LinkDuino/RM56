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
 * @addtogroup UI_DFX
 * @{
 *
 * @brief Provides test and analysis capabilities, such as stimulating input events and viewing information about a
 *        Document Object Model (DOM) tree.
 *
 * @since 6
 */

/**
 * @file ui_view_bounds.h
 *
 * @brief 支持显示或隐藏控件边框，方便UI布局和问题定位.
 *
 * @since 6
 */
#ifndef GRAPHIC_LITE_UI_VIEW_BOUNDS_H
#define GRAPHIC_LITE_UI_VIEW_BOUNDS_H

#include "graphic_config.h"
#if ENABLE_DEBUG
#include "gfx_utils/heap_base.h"
namespace OHOS {
/**
 * @brief 控件边框显示/隐藏控制类
 *
 * @since 6
 */
class UIViewBounds : public HeapBase {
public:
    /**
     * @brief Obtains a singleton <b>UIViewBounds</b> instance.
     *
     * @return Returns the <b>UIViewBounds</b> instance.
     * @since 6
     */
    static UIViewBounds* GetInstance();

    /**
     * @brief 获取当前边框显示状态 
     * 
     * @return true 显示，false 不显示
     * @since 6
     */
    bool GetShowState()
    {
        return showViewBounds_;
    }

    /**
     * @brief 设置当前边框显示状态
     * 
     * @since 6
     */
    void SetShowState(bool show);

private:
    UIViewBounds() {}
    virtual ~UIViewBounds() {}

    UIViewBounds(const UIViewBounds&) = delete;
    UIViewBounds& operator=(const UIViewBounds&) = delete;
    UIViewBounds(UIViewBounds&&) = delete;
    UIViewBounds& operator=(UIViewBounds&&) = delete;

    bool showViewBounds_ = false;
};
} // namespace OHOS
#endif // ENABLE_DEBUG
#endif // GRAPHIC_LITE_UI_VIEW_BOUNDS_H
