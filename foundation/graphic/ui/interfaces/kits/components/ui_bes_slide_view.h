#ifndef GRAPHIC_LITE_UI_BES_SLIDE_VIEW_H
#define GRAPHIC_LITE_UI_BES_SLIDE_VIEW_H

#include "components/ui_scroll_view.h"
#include <stdio.h>

namespace OHOS {

class UIBesSlideView : public UIScrollView{

private:

public:
    UIBesSlideView();
    ~UIBesSlideView();

    void scrollToDistance(int16_t distance){
        DragXInner(distance);
    }

};




}//namespace OHOS



#endif // UI_BES_DRAG_VIEW_H
