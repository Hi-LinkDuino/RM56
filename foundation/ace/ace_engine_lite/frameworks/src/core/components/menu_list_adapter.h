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

#ifndef MENU_LIST_ADAPTER_H
#define MENU_LIST_ADAPTER_H

#include "components/abstract_adapter.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_bes_menu.h"

#include "ace_log.h"
#include "handler.h"
#include "key_parser.h"
#include "keys.h"
#include<vector>

namespace OHOS {
namespace ACELite {
class MenuListAdapter : public AbstractAdapter,UIView::OnClickListener {
public:
    MenuListAdapter()
        : rootView_(RootView::GetInstance()),
        FramesPath_(nullptr),
        imgPath_(nullptr),
        itemClickListener_(nullptr),
        ListType_(false),
        itemsCount_(0)
        {}
    ~MenuListAdapter();
    UIView* GetView(UIView* inView, int16_t index) override;
    int16_t GetItemWidthWithMargin(int16_t index) override;
    int16_t GetItemHeightWithMargin(int16_t index) override;
    uint16_t GetCount() override;

    class BesItemClickListener : public HeapBase {
    public:
        BesItemClickListener() {}
        ~BesItemClickListener() {}
        virtual void OnItemClick(int16_t index, const ClickEvent& event) {}
    };

    void SetItemClickListener(BesItemClickListener *itemClickListener);

    void SetFrames(MenuItemInfo* path);

    void SetImageSrc(char* src);

    void SetCount(uint16_t count);

    void SetListType(bool type);

    bool GetImgType();

    int16_t CalculateItemIndex(int16_t itemIndex);

    void RemoveItem(UIView* view) override
    {
        if(views_.size()>0)
        {
            for (std::vector<UIView*>::iterator itBegin = views_.begin(); itBegin != views_.end();itBegin++) {
                std::vector<UIView*>::iterator tempIt = itBegin;
                if(view==*tempIt)
                {
                    views_.erase(tempIt);
                    break;
                }
            }
        }
    }

    ListAdapterType GetAdapterType() override
    {
        return  LIST_MENU_TYPE;
    };

protected:
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    RootView* rootView_;
    MenuItemInfo* FramesPath_;
    BesItemClickListener *itemClickListener_;

    uint16_t itemsCount_;
    bool ListType_;
    char* imgPath_;

    std::vector<UIView*> views_;
};
}
} // namespace OHOS
#endif // MENU_LIST_ADAPTER_H
