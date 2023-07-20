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

#ifndef OHOS_ACELITE_COMPONENT_H
#define OHOS_ACELITE_COMPONENT_H

#include "ace_log.h"
#include "ace_mem_base.h"
#include "component_utils.h"
#include "event_listener.h"
#include "memory_heap.h"
#include "non_copyable.h"
#include "stylemgr/app_style_manager.h"
#include "transition_impl.h"
#include "string_util.h"
#include "cJSON.h"

#include "graphic_config.h"

namespace OHOS {
namespace ACELite {
enum DimensionType : uint8_t {
    TYPE_UNKNOWN = 0,
    TYPE_PIXEL,
    TYPE_PERCENT,
};

union DimensionValue {
    float percentage;
    int16_t pixel;
};

struct Dimension : public MemoryHeap {
    Dimension() : type(DimensionType::TYPE_UNKNOWN)
    {
        value.percentage = -1;
    }
    Dimension(float dValue, DimensionType dType)
    {
        value.percentage = dValue;
        type = dType;
    }
    DimensionValue value;
    DimensionType type;
};

/**
 * @brief The ConstrainedParameter struct record the content rang limitation
 */
struct ConstrainedParameter : public MemoryHeap {
    ConstrainedParameter() : maxWidth(0), maxHeight(0) {}
    ConstrainedParameter(int16_t width, int16_t height) : maxWidth(width), maxHeight(height) {}
    int16_t maxWidth;
    int16_t maxHeight;
};

class Component : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(Component);
    Component() = delete;
    Component(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);
    virtual ~Component()
    {
        if (trans_ != nullptr) {
            delete (trans_);
            trans_ = nullptr;
        }
        if (jsonTrans_ != nullptr) {
            cJSON_Delete(jsonTrans_);
            jsonTrans_ = nullptr;
        }
    }

    /**
     * @brief After construct a specific component, call this function to setup this component's native view
     * and process attribute/events/style/children properly before binding it on an JS object.
     * It generally calls a series of functions to complete the render work, some of which are needed to be
     * implemented by child class. See step1~step6 function notes.
     *
     * @return true if success, false if any error occurs
     */
    bool Render();
    /**
     * @brief Call this function to release all the resource related to this component, includes the native views,
     * JS objects and any event listeners.
     */
    void Release();
    /**
     * @brief This method is called to change/update the attribute/style values on native view.
     * It mainly is used by watcher callbacks to do the view updating.
     *
     * NOTE: currently, we don't support style data binding.
     *
     * @return true if any attribute/style matches and update successfully, otherwise false
     */
    bool UpdateView(uint16_t attrKeyId, jerry_value_t attrValue);
    /**
     * @brief Child class must implement this method to return its own native view, if it only creates one
     * native view, just return it simaply, if it creates multiple views out, it should return the root of
     * those views, it's child class's responsibility to organize their hierarchy.
     *
     * @return the root of the component's native view
     */
    virtual UIView *GetComponentRootView() const = 0;

    /**
     * @brief This is the entry to set all combined styles into one specific component, one default
     * implementation is provided, which calls ApplyPrivateStyle() to try the component's private special
     * style first, if one gets matched, the appling process ends, if not, common styles will be
     * tried by calling ApplyCommonStyle() method.
     *
     * NOTE: The default implementation just apply styles into the ui view returned by GetComponentRootView(),
     * if this doesn't meet your requirement, go ahead to override this whole function as you want.
     *
     * @return true if the given style gets matched and is set properly, false for otherwise.
     */
    virtual bool ApplyStyle(const AppStyleItem *style);
    /**
     * Call this to invalidate the views this component is holding, the default implementation is just to
     * invalidate the root view of the component, if the child class has special requirement, override this
     * method to do that.
     */
    virtual void Invalidate();
    jerry_value_t GetNativeElement() const
    {
        return nativeElement_;
    }

    uint16_t GetComponentName() const
    {
        return componentName_;
    }

    bool IsFreeze() const
    {
        return freeze_;
    }

    JSValue GetViewModel() const
    {
        return viewModel_;
    }

    // update this view binded by for instruction when watch triggered.
    virtual bool UpdateForView()
    {
        return false;
    }

    jerry_value_t GetChildren() const
    {
        return children_;
    }

    jerry_value_t GetDescriptors() const
    {
        return descriptors_;
    }

    jerry_value_t AddWatcherItem(const jerry_value_t attrKey, const jerry_value_t attrValue,
                                    bool isLazyLoading = false);

    void HandleChildrenChange(jerry_value_t descriptor);

    /**
     * @brief AttachView call this function to link native views together to the tree
     */
    virtual void AttachView(const Component *child)
    {
        UNUSED(child);
    }
    /**
     * @brief OnVisibleChanged the component can be notified if the visibility status is changed
     */
    virtual void OnVisibilityChanged(bool isVisible) {}
    /**
     * @brief OnViewAttached called when the native view is attached to the tree
     */
    virtual void OnViewAttached() {}
    virtual void LayoutChildren() {}
    static void BuildViewTree(Component *currComponent, Component *parent, ConstrainedParameter &parentParameter);

    const Component *GetParent() const
    {
        return parent_;
    }

    const Component *GetChildHead() const
    {
        return childHead_;
    }

    const Component *GetNextSibling() const
    {
        return nextSibling_;
    }
    struct AnimationsNode : public MemoryHeap {
        TransitionImpl *transitionImpl;
        AnimationsNode *next;

        AnimationsNode() : transitionImpl(nullptr), next(nullptr){}
    };

        /**
     * @brief 执行（重新启动）指定的组件动画。当未指定 view 层级时（nullptr），视为执行所有动画
     * @param pageRootView 指定一个顶级 view，只会清理此 view 层级中的对应动画
     */
    static void HandlerAnimations(const UIView* const pageRootView = nullptr);

    /**
     * @brief 清理指定的 view 层级中的组件动画。当未指定 view 层级时（nullptr），视为清理所有动画
     * @param pageRootView 指定一个顶级 view，只会清理此 view 层级中的对应动画
     */
    static void ReleaseAnimations(const UIView* const pageRootView = nullptr);

    /**
     * @brief 停止指定的 view 层级中的组件动画。当未指定 view 层级时（nullptr），视为停止所有动画
     * @param pageRootView 
     */
    static void StopAnimations(const UIView* const pageRootView = nullptr);

    /**
     * @brief GetDimension return the dimension data, only width, height, margin, top and left are supported
     * @param keyNameId the key ID for representing which dimension data is wanted
     * @return the requested dimension data
     */
    const Dimension &GetDimension(uint16_t keyNameId) const;

    void SetWidth(const int16_t width)
    {
        width_.type = DimensionType::TYPE_PIXEL;
        width_.value.pixel = width;
    }

    void SetHeight(const int16_t height)
    {
        height_.type = DimensionType::TYPE_PIXEL;
        height_.value.pixel = height;
    }
    void GetConstrainedParam(ConstrainedParameter &param) const;
    /**
     * @brief This function will be called after the ApplyCommonStyle, make padding style work.
     */
    bool AdaptBoxSizing(uint16_t attrKeyId = K_UNKNOWN) const;
    void AlignDimensions(const ConstrainedParameter &param);
    void EnableTransmitSwipe();

    /**
     * @brief 绑定的 view 是否位于指定的 view 层级中
     * @param root view 层级根
     */
    bool IsBindViewTree(const UIView* const root) {
        UIView* peek = GetComponentRootView();
        while (peek != nullptr) { // && peek != RootView::GetInstance()
            if (peek == root) {
                return true;
            }
            peek = peek->GetParent();
        }
        return false;
    }

    /**
     * @brief 对应的 view 是否匹配指定的 viewId
     * @param viewId 
     */
    bool IsBindView(const char* viewId) {
        return viewId_ == viewId;
    }

protected:
    void SetComponentName(uint16_t name)
    {
        componentName_ = name;
    }

    void RegisterNamedFunction(const char * const name, jerry_external_handler_t handler) const;

    /**
     * @brief The child class can implement this function to do some initialization before the whole render process
     * beginning. See PostRender() also. The default implementation just does nothing.
     */
    virtual void PreRender() {}
    /**
     * @brief This function will be called after the whole render process is done for current component.
     * The child class can implement this function to do some tail work.
     * See PreRender() also. The default implementation just does nothing.
     */
    virtual void PostRender() {}
    /**
     * @brief This function will be called when UpdateView begin to execute (attribute change which bind with data
     * will trigger UpdateView to execute), you can override this function to do some customise actions.
     * The default implementation just does nothing.
     */
    virtual void PreUpdate() {}
    /**
     * @brief This function will be called when UpdateView execute finished. See PreUpdate() also.
     * The default implementation just does nothing.
     */
    virtual void PostUpdate(uint16_t attrKeyId) {}
    /**
     * @brief step1: inherited class must override this method to create the native views, it's the child class's
     * responsibility to record the native views it creates out, and they should be released properly in release
     * method.
     *
     * NOTE: child class must check if the native views are not null after new xxx call, return true if it is
     * not nullptr, otherwise return false, so framework know this component's rendering is failing and it can
     * do the recycling action accordingly.
     *
     * @return true for success, false for failure
     */
    virtual bool CreateNativeViews()
    {
        return true;
    }
    /**
     * @brief This function must be implemented by child classes. Should release all the native views it
     * creates out in CreateNativeViews() function.
     */
    virtual void ReleaseNativeViews() {}
    /**
     * @brief Mapping native view with DOM element.
     */
    virtual void SetViewExtraMsg();
    virtual void ReleaseViewExtraMsg();
    /**
     * @brief This is the entry to set all attributes into one specific component, one default
     * implementation is provided, which calls SetPrivateAttribute() to try the component's private special
     * attribute first, if one gets matched, the setting process ends, if not, common attributes will be
     * tried by calling SetCommonAttribute() method.
     *
     * NOTE: The default implementation just apply attribute into the ui view returned by GetComponentRootView(),
     * if this doesn't meet your requirement, go ahead to override this whole function as you want.
     *
     * @return true if the given attribute gets matched and is set properly, false for otherwise.
     */
    virtual bool SetAttribute(uint16_t attrKeyId, jerry_value_t attrValue);
    /**
     * @brief Child class should call this to set common attribute to a given native view.
     *
     * @return true if any common attribute key matchs successfully, false if no match at all
     */
    bool SetCommonAttribute(UIView& view, const uint16_t attrKeyId, const jerry_value_t attrValue);
    /**
     * @brief Child class should call this to set own special attribute setting/update actions
     *
     * @return true if any common attribute key matchs successfully, false if no match at all
     */
    virtual bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
    {
        return false;
    }

    /**
     * @brief This is the entry to register all event listener into one specific component, one default
     * implementation is provided, which calls RegisterPrivateEventListener() to try the component's private
     * special event type first, if one gets matched, the registering process ends, if not, common attributes
     * will be tried by calling RegisterCommonEventListener() method.
     *
     * NOTE: The default implementation just apply attribute into the ui view returned by GetComponentRootView(),
     * if this doesn't meet your requirement, go ahead to override this whole function as you want.
     *
     * @return true if the given event gets matched and is set properly, false for otherwise.
     */
    virtual bool RegisterEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation);
    /**
     * @brief Child class should call this to set common event listener to a given native view.
     *
     * @return true if any common event type matchs successfully, false if no match at all
     */
    bool RegisterCommonEventListener(UIView& view,
                                     const uint16_t eventTypeId,
                                     const jerry_value_t funcValue,
                                     bool isStopPropagation);
    /**
     * @brief Child class should call this to set own special event setting/update actions.
     *
     * @return true if any common event type matchs successfully, false if no match at all
     */
    virtual bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation)
    {
        return false;
    }

    /**
     * @brief Used to set all common styles for all kinds of component.
     *
     * @return true if any common attribute key matchs successfully, false if no match at all
     */
    bool ApplyCommonStyle(UIView& view, const AppStyleItem *style);

    /**
     *@brief Process style transform.
     *
     */
    void ProcessTransform(UIView &view) const;
    void ProcessTransform(UIView &view, cJSON *array) const;
    /**
     * @brief Child class should call this to set own special attribute setting/update actions
     *
     * @return true if any common attribute key matchs successfully, false if no match at all
     */
    virtual bool ApplyPrivateStyle(const AppStyleItem *style)
    {
        return false;
    }

    /**
     * @brief If a child component is a container, it should implement this method to add children into itself.
     * This function returns true as default if child doesn't override it.
     */
    virtual bool ProcessChildren()
    {
        return true;
    }

    AppStyleManager *GetStyleManager() const
    {
        return styleManager_;
    }

    jerry_value_t GetOptions() const
    {
        return options_;
    }

    /**
     * @brief combine RGB color by red, green, blue from int color value,
     *
     * @param [in] colorIntValue.
     */
    ColorType GetRGBColor(uint32_t colorIntValue) const
    {
        uint32_t colorUintValue = colorIntValue;
        uint8_t red8 = uint8_t((colorUintValue & TEXT_RED_COLOR_MASK) >> RED_COLOR_START_BIT);
        uint8_t green8 = uint8_t((colorUintValue & TEXT_GREEN_COLOR_MASK) >> GREEN_COLOR_START_BIT);
        uint8_t blue8 = uint8_t((colorUintValue & TEXT_BLUE_COLOR_MASK));
        return Color::GetColorFromRGB(red8, green8, blue8);
    }

    jerry_value_t SetListForWatcher(jerry_value_t getter, jerry_value_t children);
    void HandleListForDireactive();
    void AppendChildren(Component *parent);
    bool AppendDescriptorOrElement(Component *parent, const jerry_value_t descriptorOrElement);
    void AppendIfDescriptor(Component *parent, const jerry_value_t descriptor);
    void AppendForDescriptor(Component *parent, const jerry_value_t descriptor);
    void AppendElement(Component *parent, const jerry_value_t element);
    void CreateDirectiveWatcher(jerry_value_t descriptor);
    void UpdateDescriptor(Component *parent, const jerry_value_t descriptor);
    void ReappendDescriptorOrElement(Component *parent, const jerry_value_t descriptor);
    int32_t GetStyleNumValue(const AppStyleItem *style) const
    {
        return style->GetNumValue();
    }
    uint16_t GetStylePropNameId(const AppStyleItem *style) const
    {
        return style->GetPropNameId();
    }
    const char *GetStyleStrValue(const AppStyleItem *style) const
    {
        return style->GetStrValue();
    }
    uint8_t GetStyleStrValueLen(const AppStyleItem *style) const
    {
        return style->GetStrValueLen();
    }
    bool IsStyleValueTypeNum(const AppStyleItem *styleItem) const
    {
        return styleItem->GetValueType() == STYLE_PROP_VALUE_TYPE_NUMBER;
    }
    bool IsStyleValueTypeString(const AppStyleItem *styleItem) const
    {
        return styleItem->GetValueType() == STYLE_PROP_VALUE_TYPE_STRING;
    }
    int16_t GetHeight() const
    {
        return (height_.type == TYPE_PIXEL) ? height_.value.pixel : INVALID_PIXEL_VALUE;
    }
    int16_t GetWidth() const
    {
        return (width_.type == TYPE_PIXEL) ? width_.value.pixel : INVALID_PIXEL_VALUE;
    }

    jerry_value_t *GetDomElementPointer()
    {
        return &nativeElement_;
    }

    int32_t GetStylePixelValue(const AppStyleItem *style, int32_t defaultValue = 0) const;
    int16_t GetStyleDegValue(const AppStyleItem *style, int16_t defaultValue = 0) const;
    bool GetStyleColorValue(const AppStyleItem *style, uint32_t &color, uint8_t &alpha) const;
    void SetPadding(UIView &view, const AppStyleItem& styleItem) const;
    void SetLeftPadding(UIView &view, const AppStyleItem& styleItem) const;
    void SetTopPadding(UIView &view, const AppStyleItem& styleItem) const;
    void SetRightPadding(UIView &view, const AppStyleItem& styleItem) const;
    void SetBottomPadding(UIView &view, const AppStyleItem& styleItem) const;
    void SetBorderWidth(UIView &view, const AppStyleItem& styleItem) const;
    void SetBorderColor(UIView &view, const AppStyleItem& styleItem) const;
    void SetBorderRadius(UIView &view, const AppStyleItem& styleItem) const;
    void SetRadiusDirection(UIView &view, const AppStyleItem& styleItem) const;
    void SetBackgroundColor(UIView &view, const AppStyleItem& styleItem) const;
    void SetOpacity(UIView &view, const AppStyleItem &styleItem) const;
    void SetLeftMargin(UIView &view) const;
    void SetTopMargin(UIView &view) const;
    void SetRightMargin(UIView &view) const;
    void SetBottomMargin(UIView &view) const;
    void SetMargin(UIView &view) const;
    virtual void ApplyAlignedMargin(UIView &uiView) const;

    /**
     * @brief get the pressedImage url and the normal image url
     * @param styleItem the style item which contains image info
     * @param pressedImage the pseudo value in style item
     * @param normalImage the normal image url
     * @return if get the image url success return true, else false
     */
    bool HandleBackgroundImg(const AppStyleItem &styleItem, char *&pressedImage, char *&normalImage) const;

#if (FEATURE_ROTATION_API == 1)
    /**
     * @brief the rotation API handling function, the child component can register it for rotation API supporting
     * @param func function object
     * @param dom the context of function execution
     * @param args the list of arguments
     * @param size the length of arguments list
     * @return the handling result to caller
     */
    static jerry_value_t HandleRotationRequest(const jerry_value_t func,
                                               const jerry_value_t dom,
                                               const jerry_value_t args[],
                                               const jerry_length_t size);
#endif // FEATURE_ROTATION_API
private:
    /**
     * @brief Used to set animation keyframes, such as transform, background-color, height etc.
     * it is set as attribute, and support binding to data, so it can be changed dynamically.
     *
     * the following three functions will called in order
     */
    void SetAnimationKeyFrames(const UIView& view, const AppStyleItem *styleItem);
    void SetAnimationKeyFrames(const AppStyleItem *item);
    void SetAnimationKeyFrames(int16_t keyId, int32_t valueFrom, int32_t valueTo);
    /**
     * @brief Used to set animation style, such as animation during, delay, iteration count etc.
     * it is set as style, now it not support binding to data, so it can not be changed dynamiclly.
     */
    void SetAnimationStyle(const UIView& view, const AppStyleItem *styleItem, const int16_t keyId);
    /**
     * @brief Record current component`s animation. All animations of components will be called when the whole page
     * render complete.
     */
    void RecordAnimation();
    /**
     * @brief In updateView progress, the current component`s animation will be called immediately.
     */
    void StartAnimation();
    void ReleaseTransitionParam();
    /**
     * @brief Used to get animation item value.
     * for example: tranformX from 100px to 200px, index = 1 to get from value, index = 2 to get to value.
     *
     * @return animation value, such as 100px
     */
    int32_t GetAnimatorValue(char *animatorValue, const int8_t index, bool isOpacity = false) const;

    void ParseOptions();
    void ParseAttrs();
    void ParseEvents();
    void BindEvents(const char *type, bool isStopPropagation);
    /**
     * @brief Apply combined styles into native view.
     */
    void ApplyStyles(const jerry_value_t options, Component& currentComponent) const;
    bool IsLayoutRelatedAttrs(uint16_t attrKeyId) const;
    void ApplyAlignedPosition(UIView &uiView) const;
    void AdapteBoxRectArea(UIView &uiView) const;
    void SetVisible(UIView& view, const AppStyleItem *styleItem) const;
    void SetClickEventListener(UIView& view, const jerry_value_t eventFunc, bool isStopPropagation);
    void SetLongPressEventListener(UIView& view, const jerry_value_t eventFunc, bool isStopPropagation);
    void SetSwipeEventListener(UIView& view, jerry_value_t eventFunc, bool isStopPropagation);
    void SetTouchStartEventListener(UIView& view, jerry_value_t eventFunc, bool isStopPropagation);
    void SetTouchMoveEventListener(UIView& view, jerry_value_t eventFunc, bool isStopPropagation);
    void SetTouchEndEventListener(UIView& view, jerry_value_t eventFunc, bool isStopPropagation);
#ifdef JS_EXTRA_EVENT_SUPPORT
    void SetTouchCancelEventListener(UIView &view, jerry_value_t eventFunc, bool isStopPropagation);
    void SetKeyBoardEventListener(jerry_value_t eventFunc, bool isStopPropagation);
#endif
    /**
     * @brief release common event listeners if any is set
     */
    void ReleaseCommonEventListeners();
    void AppendDescriptorOrElements(Component *parent, const JSValue descriptorOrElements);
    /**
     * @brief For some situations, if the component's rect(position, width, height and so on) area is changed, need
     * to force the parent container to relayout all children, and need to invalided self before the changes are
     * applied.
     *
     * @param attrKeyId which attribute or style is being changing
     * @param invalidateSelf true for just invaliding self, false for to relayout parent, default is false.
     */
    void InvalidateIfNeeded(uint16_t attrKeyId, bool invalidateSelf = false) const;
    void AddAnimationToList(const TransitionImpl *transitionImpl) const;

    void GetDimensionFromStyle(Dimension &dimension, const AppStyleItem &styleItem) const;
    void CalculateDimensionPixel(Dimension &dimension, int16_t base) const;
    virtual void OnDimensionsAligned() {}
    bool IsAttached() const;

    /**
     * @brief SetParent assign parent node
     * @param parent current component's parent
     */
    void SetParent(Component *parent)
    {
        parent_ = parent;
    }
    void SetNextSibling(Component *next)
    {
        nextSibling_ = next;
    }
    /**
     * @brief AddChild insert one child
     * @param childNode child component
     *
     * NOTE: add one child will not attach the native view immediately, but
     * when removing one child, the child native view will be detached immediately
     */
    void AddChild(Component *childNode);
    /**
     * @brief RemoveChild cut off the specific child from current component
     * @param childNode the child component
     */
    void RemoveChild(Component *childNode);
    /**
     * @brief RemoveAllChildren clean all children
     */
    void RemoveAllChildren();

    /**
     * @brief childHead_ the child list
     */
    Component *childHead_;
    /**
     * @brief nextSibling_ next neighbor
     */
    Component *nextSibling_;
    /**
     * @brief parent_ parent component node
     */
    Component *parent_;

    AppStyleManager *styleManager_;
    jerry_value_t nativeElement_;
    jerry_value_t viewModel_;
    jerry_value_t options_;
    jerry_value_t children_;
    ViewOnClickListener *onClickListener_;
    ViewOnLongPressListener *onLongPressListener_;
    ViewOnTouchListener *onTouchListener_;
#ifdef JS_EXTRA_EVENT_SUPPORT
    ViewOnTouchCancelListener *onTouchCancelListener_;
    KeyBoardEventListener *keyBoardEventListener_;
#endif
    /**
     * record view id, need to be released when this component get released
     */
    char *viewId_;
    uint16_t componentName_;
    bool isHXAnima_=false;
    uint16_t hxFrom_;
    uint16_t hxTo_;
    bool rendered_;
    bool isAnimationKeyFramesSet_;
    bool freeze_;
    TransitionImpl *curTransitionImpl_;
    TransitionParams *trans_;
    jerry_value_t descriptors_;
    Watcher *watchersHead_;
    Dimension height_;
    Dimension width_;
    Dimension top_;
    Dimension left_;
    Dimension marginTop_;
    Dimension marginLeft_;
    Dimension marginRight_;
    Dimension marginBottom_;
    std::map<std::string,float> map;
    cJSON *jsonTrans_;
    static const char * const FUNC_GETRECT;
    static const char * const FUNC_GETABSRECT;
    // static jerry_value_t Component::JSRectObject(Rect rect);
    static JSValue JSGetRect(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
    static JSValue JSGetAbsRect(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_COMPONENT_H
