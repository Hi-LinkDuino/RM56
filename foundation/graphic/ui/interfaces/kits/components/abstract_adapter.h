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
 * @file abstract_adapter.h
 *
 * @brief Defines the base class for adapters.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_ABSTRACT_ADAPTER_H
#define GRAPHIC_LITE_ABSTRACT_ADAPTER_H

#include "components/ui_view.h"

namespace OHOS {
/**
 * @brief Defines the base class for adapters. You can derive <b>AbstractAdapter</b> based on actual requirements
 *        and use the <b>GetView()</b> and <b>GetCount()</b> functions to implement adapters of different data types.
 *        For details, see {@link TextAdapter}.
 * @since 1.0
 * @version 1.0
 */
class AbstractAdapter : public HeapBase {
public:
    /**
     * @brief A constructor used to create an <b>AbstractAdapter</b> instance.
     * @since 1.0
     * @version 1.0
     */
    AbstractAdapter() {}

    /**
     * @brief A destructor used to delete the <b>AbstractAdapter</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~AbstractAdapter() {}

    /**
     * @brief Obtains the number of adapter data items.
     *
     * @return Returns the number of adapter data items.
     * @since 1.0
     * @version 1.0
     */
    virtual uint16_t GetCount() = 0;

    /**
     * @brief Obtains a <b>UIView</b> instance to convert adapter data into another <b>UIView</b> instance.
     *
     * @param inView Indicates the pointer to the reusable instance. If this parameter is not <b>NULL</b>, a reusable
     *               <b>UIView</b> instance is available. In this case, this function does not need to create a new
     *               <b>UIView</b> instance, just reusing the instance specified by <b>inView</b> to update the
     *               <b>inView</b> data.
     *               If this parameter is <b>NULL</b>, there is no reusable <b>UIView</b> instance. In this case, this
     *               function needs to create a new <b>UIView</b> instance.
     *
     * @param index Indicates the adapter data index.
     *
     * @return UIView Returns the pointer to the <b>UIView</b> instance constructed by the adapter.
     * @since 1.0
     * @version 1.0
     */
    virtual UIView* GetView(UIView* inView, int16_t index) = 0;

    virtual void DeleteView(UIView*& view)
    {
        delete view;
        view = nullptr;
    };

    virtual int16_t GetItemWidthWithMargin(int16_t index)
    {
        UIView* view = GetView(nullptr, index);
        if (view == nullptr) {
            return 0;
        }
        int16_t width = view->GetWidthWithMargin();
        DeleteView(view);
        return width;
    }

    virtual int16_t GetItemHeightWithMargin(int16_t index)
    {
        UIView* view = GetView(nullptr, index);
        if (view == nullptr) {
            return 0;
        }
        int16_t height = view->GetHeightWithMargin();
        DeleteView(view);
        return height;
    }
};
} // namespace OHOS
#endif // GRAPHIC_LITE_ABSTRACT_ADAPTER_H
