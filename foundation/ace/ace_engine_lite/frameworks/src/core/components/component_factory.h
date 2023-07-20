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
#ifndef OHOS_ACELITE_COMPONENT_FACTORY_H
#define OHOS_ACELITE_COMPONENT_FACTORY_H

#include "acelite_config.h"

#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
#include "analog_clock_component.h"
#include "bes_button_component.h"
#include "bes_slide_component.h"
#include "bes_menu_list_component.h"
#include "bes_menu_component.h"
#include "bes_drag_view_component.h"
#include "clock_hand_component.h"
#include "drawer_layout_component.h"
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
#if (FEATURE_COMPONENT_CAMERA == 1)
#include "camera_component.h"
#endif // FEATURE_COMPONENT_CAMERA
#if (FEATURE_COMPONENT_CANVAS == 1)
#include "canvas_component.h"
#endif // FEATURE_COMPONENT_CANVAS
#include "chart_component.h"
#include "circle_progress_component.h"
#include "component.h"
#include "div_component.h"
#include "fatal_handler.h"
#include "horizon_progress_component.h"
#include "image_animator_component.h"
#include "image_component.h"
#include "input_button_component.h"
#include "input_checkbox_component.h"
#include "input_radio_component.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"
#include "list_component.h"
#include "marquee_component.h"
#include "non_copyable.h"
#include "picker_view_component.h"
#if (FEATURE_COMPONENT_QRCODE == 1)
#include "qrcode_component.h"
#endif
#include "slider_component.h"
#include "stack_component.h"
#include "swiper_component.h"
#include "switch_component.h"
#if (FEATURE_COMPONENT_TABS == 1)
#include "tab_bar_component.h"
#include "tab_content_component.h"
#include "tabs_component.h"
#endif // FEATURE_COMPONENT_TABS
#include "text_component.h"
#include "video_component.h"
#include "fisheye_menu_component.h"
#include "fragment_component.h"
#include "bestechnic/bes_list_component.h"

namespace OHOS {
namespace ACELite {
class ComponentFactory final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ComponentFactory);
    ~ComponentFactory() {}
    static Component* CreateComponent(uint16_t componentNameId, jerry_value_t options, jerry_value_t children)
    {
        if (!KeyParser::IsKeyValid(componentNameId)) {
            return nullptr;
        }

        JsAppContext* context = JsAppContext::GetInstance();
        if (context == nullptr) {
            return nullptr;
        }
        AppStyleManager* styleManager = const_cast<AppStyleManager *>(context->GetStyleManager());

        //yongxianglai@bestechnic.com: 优先匹配自定义 component
        Component* component = CreatePrivateComponent(componentNameId, options, children, styleManager);
        if (component != nullptr) {
            //如果命中，禁止继续匹配 component
            FatalHandler::GetInstance().AttachComponentNode(component);
            return component;
        }

        //Component* component;
        switch (componentNameId) {
            case K_BES_SLIDE:
                component = new BesSlideComponent(options, children, styleManager);
                break;
            case K_BES_DRAG_VIEW:
                component = new BesDragViewComponent(options, children, styleManager);
                break;
            case K_BES_LIST_MENU_VIEW:
                component = new BesMenuListComponent(options, children, styleManager);
                break;
            case K_BES_MENU:
                component = new BesMenuComponent(options, children, styleManager);
                break;
            case K_BES_DRAWER_LAYOUT:
                component = new DrawerLayoutComponent(options, children, styleManager);
                break;
#if (FEATURE_COMPONENT_CAMERA == 1)
            case K_CAMERA:
                component = new CameraComponent(options, children, styleManager);
                break;
#endif
#if (FEATURE_COMPONENT_CANVAS == 1)
            case K_CANVAS:
                component = new CanvasComponent(options, children, styleManager);
                break;
#endif // FEATURE_COMPONENT_CANVAS
            case K_DIV:
                component = new DivComponent(options, children, styleManager);
                break;
            case K_STACK:
                component = new StackComponent(options, children, styleManager);
                break;
            case K_IMAGE:
                component = new ImageComponent(options, children, styleManager);
                break;
            case K_IMAGE_ANIMATOR:
                component = new ImageAnimatorComponent(options, children, styleManager);
                break;
            case K_PROGRESS: {
                uint16_t id = ComponentUtils::GetComponentType(options);
                if (id == K_ARC) {
                    component = new CircleProgressComponent(options, children, styleManager);
                } else {
                    // default type is horizon progress
                    component = new HorizonProgressComponent(options, children, styleManager);
                }
                break;
            }
#if (FEATURE_COMPONENT_TABS == 1)
            case K_TAB_BAR:
                component = new TabBarComponent(options, children, styleManager);
                break;
            case K_TAB_CONTENT:
                component = new TabContentComponent(options, children, styleManager);
                break;
            case K_TABS:
                component = new TabsComponent(options, children, styleManager);
                break;
#endif // FEATURE_COMPONENT_TABS
            case K_TEXT:
                component = new TextComponent(options, children, styleManager);
                break;  
            case K_SWIPER:
                component = new SwiperComponent(options, children, styleManager);
                break;
            case K_SWITCH:
                component = new SwitchComponent(options, children, styleManager);
                break;
            case K_SLIDER:
                component = new SliderComponent(options, children, styleManager);
                break;
            case K_LIST:
                component = new ListComponent(options, children, styleManager);
                break;
            case K_LIST_ITEM:
                component = new DivComponent(options, children, styleManager);
                break;
#if (FEATURE_COMPONENT_ANALOG_CLOCK == 1)
            case K_CLOCK_HAND:
                component = new ClockHandComponent(options, children, styleManager);
                break;
            case K_ANALOG_CLOCK:
                component = new AnalogClockComponent(options, children, styleManager);
                break;
#endif // FEATURE_COMPONENT_ANALOG_CLOCK
            case K_INPUT: {
                uint16_t id = ComponentUtils::GetComponentType(options);
                if (id == K_CHECKBOX) {
                    component = new InputCheckboxComponent(options, children, styleManager);
                } else if (id == K_RADIO) {
                    component = new InputRadioComponent(options, children, styleManager);
                } else if(id == K_BES){
                    component = new BesButtonComponent(options, children, styleManager);
                }else {
                    // default type is input button
                    component = new InputButtonComponent(options, children, styleManager);
                }
                break;
            }
            case K_MARQUEE:
                component = new MarqueeComponent(options, children, styleManager);
                break;
            case K_CHART:
                component = new ChartComponent(options, children, styleManager);
                break;
            case K_PICKER_VIEW:
                component = new PickerViewComponent(options, children, styleManager);
                break;
#if (FEATURE_COMPONENT_QRCODE == 1)
            case K_QRCODE:
                component = new QrcodeComponent(options, children, styleManager);
                break;
#endif // FEATURE_COMPONENT_QRCODE
#if (FEATURE_COMPONENT_VIDEO == 1)
            case K_VIDEO:
                component = new VideoComponent(options, children, styleManager);
                break;
#endif // FEATURE_COMPONENT_VIDEO

            case K_BES_FISHEYE_MENU:
                component = new FisheyeMenuComponent(options, children, styleManager);
                break;
            default:
                component = nullptr;
                break;
        }

        FatalHandler::GetInstance().AttachComponentNode(component);
        return component;
    }

private:
    /**
     * @brief 创建自定义 Component 对象
     * @param componentNameId 可能的取值见 keys.h 中定义的 KEYWORD(XXX, xxx)
     * @return 匹配的 Component 对象。返回 nullptr 表示无法匹配或对象创建失败。
     */
    static Component* CreatePrivateComponent(uint16_t componentNameId, jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager)
    {
        Component* component = nullptr;
        switch (componentNameId) {
            case K_BES_LIST: {
                component = new BesListComponent(options, children, styleManager);
                break;
            }
            case K_LIST: {
                uint16_t id = ComponentUtils::GetComponentType(options);
                if (id == K_BES_LIST) {
                    component = new BesListComponent(options, children, styleManager);
                    break;
                }
            }
            case K_DIV: {
                uint16_t id = ComponentUtils::GetComponentType(options);
                if(id == K_BES_FRAGEMNT){
                    component = new FragmentComponent(options, children, styleManager);
                } else if(id == K_BES_FISHEYE_MENU){
                    component = new FisheyeMenuComponent(options, children, styleManager);
                } else if(id == K_BES_DRAWER_LAYOUT){
                    component = new DrawerLayoutComponent(options, children, styleManager);
                }
                break;

            }
            default: {
                component = nullptr;
                break;
            }
        }

        return component;
    }
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_COMPONENT_FACTORY_H
