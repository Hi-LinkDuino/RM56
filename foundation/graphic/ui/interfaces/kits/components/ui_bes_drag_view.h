#ifndef UI_BES_DRAG_VIEW_H
#define UI_BES_DRAG_VIEW_H

#include "components/ui_canvas.h"
#include "common/text.h"
#include <stdio.h>

namespace OHOS {


class UIBesDragView : public UICanvas{

    static const uint16_t VIEW_SIZE = 80;

private:
    Paint paint_;
    Paint textPaint_;
    Paint textPaintY_;

    FontStyle style_;

    void initStyleData();



    int x_;
    int y_;

    char xStr_[5];
    char yStr_[5];

public:
    UIBesDragView();
    ~UIBesDragView();

    void onPaint();

    void onRePaint(const DragEvent& event);

    bool OnDragEvent(const DragEvent& event) override;

    bool OnDragEndEvent(const DragEvent& event) override;

    bool OnDragStartEvent(const DragEvent& event) override;

};






}//namespace OHOS



#endif // UI_BES_DRAG_VIEW_H
