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

#include "ui_test_group.h"

#include "graphic_config.h"
#include "test_animator/ui_test_animator.h"
#include "test_anti_aliasing/ui_test_anti_aliasing.h"
#include "test_arc_label/ui_test_arc_label.h"
#include "test_border_margin_padding/ui_test_border_margin_padding.h"
#include "test_button/ui_test_button.h"
#include "test_canvas/ui_test_canvas.h"
#include "test_chart/ui_test_chart_pillar.h"
#include "test_chart/ui_test_chart_polyline.h"
#include "test_clip/ui_test_clip.h"
#ifdef ENABLE_WINDOW
#include "test_dialog/ui_test_dialog.h"
#endif
#include "test_draw_line/ui_test_draw_line.h"
#include "test_draw_rect/ui_test_draw_rect.h"
#include "test_event_injector/ui_test_event_injector.h"
#if ENABLE_FOCUS_MANAGER
#include "test_focus_manager/ui_test_focus_manager.h"
#endif
#include "test_font/ui_test_font.h"
#include "test_image/ui_test_image.h"
#include "test_image_animator/ui_test_image_animator.h"
#include "test_input_event/ui_test_input_event.h"
#include "test_label/ui_test_label.h"
#include "test_layout/ui_test_advanced_layout.h"
#include "test_layout/ui_test_basic_layout.h"
#include "test_opacity/ui_test_opacity.h"
#include "test_picker/ui_test_ui_picker.h"
#include "test_progress_bar/ui_test_box_progress.h"
#include "test_progress_bar/ui_test_circle_progress.h"
#include "test_qrcode/ui_test_qrcode.h"
#include "test_render/ui_test_render.h"
#if ENABLE_ROTATE_INPUT
#include "test_rotate_input/ui_test_rotate_input.h"
#endif
#ifdef VERSION_STANDARD
#include "test_screenshot/ui_test_screenshot.h"
#endif
#include "test_digital_clock/ui_test_digital_clock.h"
#include "test_scroll_bar/ui_test_scroll_bar.h"
#include "test_slider/ui_test_slider.h"
#include "test_texture_mapper/ui_test_texture_mapper.h"
#include "test_transform/ui_test_transform.h"
#include "test_ui_analog_clock/ui_test_analog_clock.h"
#include "test_ui_dump_dom_tree/ui_test_dump_dom.h"
#include "test_ui_list/ui_test_ui_list.h"
#include "test_ui_list_view/ui_test_list_layout.h"
#include "test_ui_scroll_view/ui_test_ui_scroll_view.h"
#include "test_ui_swipe_view/ui_test_ui_swipe_view.h"
#include "test_view_bitmap/ui_test_view_bitmap.h"
#include "test_view_bounds/ui_test_view_bounds.h"
#include "test_view_group/ui_test_view_group.h"
#include "test_view_percent/ui_test_view_percent.h"
#include "test_view_scale_rotate/ui_test_view_scale_rotate.h"
#if ENABLE_VECTOR_FONT
#include "test_vector_font/ui_test_vector_font.h"
#endif
#ifndef VERSION_LITE
#include "test_video/ui_test_video.h"
#endif

namespace OHOS {
List<TestCaseInfo> UITestGroup::testCaseList_;

void UITestGroup::AddTestCase(TestCaseInfo testCaseInfo)
{
    testCaseList_.PushBack(testCaseInfo);
}

void UITestGroup::SetUpTestCase()
{
    testCaseList_.PushBack(TestCaseInfo{"Clip", new UITestClip()});
#if ENABLE_ROTATE_INPUT
    testCaseList_.PushBack(TestCaseInfo{"Rotate_Input", new UITestRotateInput()});
#endif
    testCaseList_.PushBack(TestCaseInfo{"View_Scale_Rotate", new UITestViewScaleRotate()});
#if ENABLE_VECTOR_FONT
    if (UIFont::GetInstance()->IsVectorFont()) {
        testCaseList_.PushBack(TestCaseInfo{"Vector_Font", new UITestVectorFont()});
    }
#endif
    testCaseList_.PushBack(TestCaseInfo{"Input_Event", new UITestInputEvent()});
    testCaseList_.PushBack(TestCaseInfo{"Button", new UITestBUTTON()});
    testCaseList_.PushBack(TestCaseInfo{"UILabel", new UITestLabel()});
    testCaseList_.PushBack(TestCaseInfo{"Image", new UITestImage()});
    testCaseList_.PushBack(TestCaseInfo{"Basic_Layout", new UITestBasicLayout()});
    testCaseList_.PushBack(TestCaseInfo{"Advanced_Layout", new UITestAdvancedLayout()});
    testCaseList_.PushBack(TestCaseInfo{"ListLayout", new UITestListLayout()});
    testCaseList_.PushBack(TestCaseInfo{"UIScrollView", new UITestUIScrollView()});
    testCaseList_.PushBack(TestCaseInfo{"Box_Progress", new UITestBoxProgress()});
    testCaseList_.PushBack(TestCaseInfo{"Circle_Progress", new UITestCircleProgress()});
    testCaseList_.PushBack(TestCaseInfo{"Slider", new UITestSlider()});
    testCaseList_.PushBack(TestCaseInfo{"Animator", new UITestAnimator()});
    testCaseList_.PushBack(TestCaseInfo{"Canvas", new UITestCanvas()});
    testCaseList_.PushBack(TestCaseInfo{"Draw_Rect", new UITestDrawRect()});
    testCaseList_.PushBack(TestCaseInfo{"Draw_Line", new UITestDrawLine()});
    testCaseList_.PushBack(TestCaseInfo{UI_TEST_RENDER_ID, new UITestRender()});
    testCaseList_.PushBack(TestCaseInfo{"Anti_Aliasing", new UITestAntiAliasing()});
    testCaseList_.PushBack(TestCaseInfo{"UIList", new UITestUIList()});
    testCaseList_.PushBack(TestCaseInfo{"UISwipeView", new UITestUISwipeView()});
    testCaseList_.PushBack(TestCaseInfo{"Image_Animator", new UITestImageAnimator()});
    testCaseList_.PushBack(TestCaseInfo{"UIPicker", new UITestUIPicker()});
    testCaseList_.PushBack(TestCaseInfo{"Font", new UITestFont()});
    testCaseList_.PushBack(TestCaseInfo{"Arc_Label", new UITestArcLabel()});
#if ENABLE_DEBUG
    testCaseList_.PushBack(TestCaseInfo{"Event_Injector", new UITestEventInjector()});
    testCaseList_.PushBack(TestCaseInfo{"Dump_Dom", new UITestDumpDomTree()});
    testCaseList_.PushBack(TestCaseInfo{"View Bounds", new UITestViewBounds()});
#ifdef VERSION_STANDARD
    testCaseList_.PushBack(TestCaseInfo{"Screenshot", new UITestScreenshot()});
#endif
#endif // ENABLE_DEBUG
    testCaseList_.PushBack(TestCaseInfo{"View_Percent", new UITestViewPercent()});
    testCaseList_.PushBack(TestCaseInfo{"Texture_Mapper", new UITestTextureMapper()});
    testCaseList_.PushBack(TestCaseInfo{"Chart_Polyline", new UITestChartPolyline()});
    testCaseList_.PushBack(TestCaseInfo{"Chart_Pillar", new UITestChartPillar()});
#if ENABLE_WINDOW
    testCaseList_.PushBack(TestCaseInfo{"Dialog", new UITestDialog()});
#endif
    testCaseList_.PushBack(TestCaseInfo{"Analog_Clock", new UITestAnalogClock()});
    testCaseList_.PushBack(TestCaseInfo{"Digital_Clock", new UITestDigitalClock()});
    testCaseList_.PushBack(TestCaseInfo{"Transform", new UITestTransform()});
    testCaseList_.PushBack(TestCaseInfo{"Opacity", new UITestOpacity()});
    testCaseList_.PushBack(TestCaseInfo{"UIQrcode", new UITestQrcode()});
    testCaseList_.PushBack(TestCaseInfo{"UIViewGroup", new UITestViewGroup()});
    testCaseList_.PushBack(TestCaseInfo{"View Bitmap", new UITestViewBitmap()});
#ifndef VERSION_LITE
#if ENABLE_VIDEO_COMPONENT
    testCaseList_.PushBack(TestCaseInfo{"Video", new UITestVideo()});
#endif
#endif
#if ENABLE_FOCUS_MANAGER
    testCaseList_.PushBack(TestCaseInfo{"FocusManager", new UITestFocusManager()});
#endif
    testCaseList_.PushBack(TestCaseInfo{"Border_Margin_Padding", new UITestBorderMarginPadding()});
    testCaseList_.PushBack(TestCaseInfo{"ScrollBar", new UITestScrollBar()});
}

List<TestCaseInfo>& UITestGroup::GetTestCase()
{
    return testCaseList_;
}

UITest* UITestGroup::GetTestCase(const char* id)
{
    if (id == nullptr) {
        return nullptr;
    }
    ListNode<TestCaseInfo>* node = testCaseList_.Begin();
    while (node != testCaseList_.End()) {
        if (!strcmp(id, node->data_.sliceId)) {
            return node->data_.testObj;
        }
        node = node->next_;
    }
    return nullptr;
}
void UITestGroup::TearDownTestCase()
{
    ListNode<TestCaseInfo>* node = testCaseList_.Begin();
    while (node != testCaseList_.End()) {
        delete node->data_.testObj;
        node->data_.testObj = nullptr;
        node = node->next_;
    }
    testCaseList_.Clear();
}
} // namespace OHOS
