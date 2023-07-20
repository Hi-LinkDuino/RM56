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

#ifndef OHOS_ACELITE_BES_MENU_LIST_COMPNENT_H
#define OHOS_ACELITE_BES_MENU_LIST_COMPNENT_H

#include "component.h"
#include "non_copyable.h"
#include "menu_list_adapter.h"
#include "components/ui_bes_menu.h"
#include "components/ui_list.h"
#include "components/ui_view_group.h"

#include "ace_log.h"
#include "handler.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {

class BesMenuListComponent final : public Component {
public:
    BesMenuListComponent() = delete;
    BesMenuListComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~BesMenuListComponent() {
        if(itemClickListener_ != nullptr){
            ACE_DELETE(itemClickListener_);
            itemClickListener_ = nullptr;
        }
        if(list_ != nullptr){
            ACE_DELETE(list_);
            list_ = nullptr;
        }
    }
class ListItemClickListener : public MenuListAdapter::BesItemClickListener{
public:
    ACE_DISALLOW_COPY_AND_MOVE(ListItemClickListener);
    explicit ListItemClickListener(jerry_value_t vm,jerry_value_t fn,bool isStopPropagation)
    : vm_(jerry_acquire_value(vm)),
    beslist_(nullptr),
    fn_(jerry_acquire_value(fn)) {}

    ListItemClickListener() = delete;
    ~ListItemClickListener()
    {
        jerry_release_value(fn_);
        jerry_release_value(vm_);
    }
    void OnItemClick(int16_t index, const ClickEvent& event) override;

    void SetMenu(UIList *beslist){
        beslist_ = beslist;
    }

private:
    jerry_value_t vm_;
    jerry_value_t fn_;
    bool isStopPropagation_;
    UIList* beslist_;
};

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    void Invalidate() override;

private:
    UIList* list_;
    MenuListAdapter adapter_;
    UIBesMenu *besMenu_;
    MenuItemInfo *frames_;
    ListItemClickListener *itemClickListener_;

    uint16_t framesSize_;
    bool ListType_;
    char *src_;

    void ReleaseFrame();
    bool ParseToFrames(jerry_value_t value);
    void ParseToList(jerry_value_t value);

};
}// namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_BES_MENU_LIST_COMPNENT_H
