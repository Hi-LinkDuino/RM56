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

#ifndef OHOS_ACELITE_BES_SLIDE_COMPNENT_H
#define OHOS_ACELITE_BES_SLIDE_COMPNENT_H

#include "ace_log.h"
#include "component.h"
#include "non_copyable.h"
#include "ui_bes_slide_view.h"
#include "ui_view_group.h"
#include "ui_label.h"
#include "flex_layout.h"
#include "layout.h"
#include "event_util.h"

namespace  {
   constexpr int16_t LABEL_WIDTH = 200;
   constexpr int16_t LABEL_HEIGHT = 100;
   constexpr int16_t LABEL_INTERVAL = 50;
   constexpr int16_t LABEL_LEFT_INTERVAL = 200;
}

namespace OHOS {
namespace ACELite {

class BesSlideComponent final : public Component {

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    void OnViewAttached() override;
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    bool ApplyPrivateStyle(const AppStyleItem* styleItem) override;
    void PostRender() override;

private:
   class SlideListener:public UIScrollView::OnScrollListener{
        friend class BesSlideComponent;
    public:
        ACE_DISALLOW_COPY_AND_MOVE(SlideListener);

        SlideListener(UIBesSlideView *scrollView,FlexLayout *deleteGroup)
        : scrollView_(scrollView),
          deleteGroup_(deleteGroup),
          currentX_(200){}
        ~SlideListener()
        {
            ACE_DELETE(scrollView_);
            ACE_DELETE(deleteGroup_);
        }
        void OnScrollStart() override;
        void OnScrollEnd() override;

        bool isOpen_;

    private:
        UIBesSlideView* scrollView_;
        FlexLayout *deleteGroup_;
        uint16_t currentX_;
    };

    class SlideOnClickListener: public UIView::OnClickListener{
        friend class BesSlideComponent;
    public:
        ACE_DISALLOW_COPY_AND_MOVE(SlideOnClickListener);
        SlideOnClickListener(jerry_value_t vm, jerry_value_t fn, bool isStopPropagation,UIBesSlideView *scrollView)
        : vm_(jerry_acquire_value(vm)),
          fn_(jerry_acquire_value(fn)),
          scrollView_(scrollView),
          isStopPropagation_(isStopPropagation){

            deleteX_ =  scrollView_->GetChildrenTail()->GetX();
          }

        ~SlideOnClickListener()
        {
            AsyncTaskManager::GetInstance().CancelWithContext(this);
            jerry_release_value(vm_);
            jerry_release_value(fn_);
            ACE_DELETE(scrollView_);
            ACE_DELETE(slideListener_);
        }
        bool OnClick(UIView& view, const ClickEvent &event) override;

        void SetSlideListener(SlideListener *slideListener){
            this->slideListener_ = slideListener;
        }
    private:
        jerry_value_t vm_;
        jerry_value_t fn_;
        bool isStopPropagation_;
        SlideListener *slideListener_;
        UIBesSlideView* scrollView_;

        uint16_t deleteX_;
    };


    void initScrollViewStyle();
    void initView();
    void GetDimensionFromStyle(Dimension &dimension, const AppStyleItem &styleItem) const;
    void updateViewSize();

    Dimension heightDimen_;
    Dimension widthDimen_;
    FlexLayout *deleteGroup_;
    UILabel *contentLabel_;
    UIImageView imageView_;

    uint16_t width_;
    uint16_t height_;
    SlideListener *slideListener_;
    SlideOnClickListener *clickListener_;

public:
    BesSlideComponent() = delete;
    BesSlideComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~BesSlideComponent() override;

    UIBesSlideView scrollView_;
};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_BES_SLIDE_COMPNENT_H
