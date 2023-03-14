#ifdef OH_UI_PROGRESS_TOUCHABLE

#ifndef OHOS_ACELITE_ABSTRACT_PROGESS_COMPONENT_H
#define OHOS_ACELITE_ABSTRACT_PROGESS_COMPONENT_H


#include "component.h"
#include "non_copyable.h"
#include "ui_abstract_progress.h"

namespace OHOS {
namespace ACELite {
constexpr char ATTR_VALUE[] = "value";


class AbstractProgressComponent : public Component {
public:
    AbstractProgressComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);

protected:
    void setProgressView(UIAbstractProgress *view){
      view_ = view;
    }
    UIAbstractProgress * getProgressView(){
      return view_;
    }
    void ReleaseNativeViews() override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;

    class ChangeListener : public UIAbstractProgress::OnChangeListener {
    public:
        ACE_DISALLOW_COPY_AND_MOVE(ChangeListener);
        explicit ChangeListener(jerry_value_t fn) : fn_(jerry_acquire_value(fn)) {}
        ChangeListener() = delete;
        ~ChangeListener()
        {
            jerry_release_value(fn_);
        }
        void OnChange(UIAbstractProgress &view) override;

    private:
        jerry_value_t fn_;
    };
    ChangeListener *changeListener_;

private:
  UIAbstractProgress *view_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_ABSTRACT_PROGESS_COMPONENT_H

#endif // OH_UI_PROGRESS_TOUCHABLE
