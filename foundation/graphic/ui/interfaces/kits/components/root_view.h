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
 * @file root_view.h
 *
 * @brief Manages a root view.
 *
 * A root view is the view containing its child views. It represents the root node in a tree structure and is
 * the parent to all the children.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_ROOT_VIEW_H
#define GRAPHIC_LITE_ROOT_VIEW_H

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#endif

#include "components/ui_view_group.h"
#include "events/key_event.h"
#include "events/virtual_device_event.h"
#include "gfx_utils/list.h"

#if LOCAL_RENDER
#include <map>
#include "gfx_utils/vector.h"
#endif

namespace OHOS {
#if ENABLE_WINDOW
class Window;
class WindowImpl;
#endif

/**
 * @brief Defines the functions related to a root view which contains its child views and represents
 *        the root node in a tree structure.
 *
 * @since 1.0
 * @version 1.0
 */
class RootView : public UIViewGroup {
public:
    /**
     * @brief Obtains a singleton <b>RootView</b> instance.
     *
     * @return Returns the singleton <b>RootView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    static RootView* GetInstance();

#if ENABLE_WINDOW
    /**
     * @brief Obtains a <b>RootView</b> instance bound to a window.
     *
     * @return Returns the <b>RootView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    static RootView* GetWindowRootView()
    {
        return new RootView;
    }

    /**
     * @brief Destroys the <b>RootView</b> bound to a window.
     *
     * @param rootView Indicates the pointer to the <b>RootView</b> to destroy.
     * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    static bool DestoryWindowRootView(RootView* rootView)
    {
        if (rootView == RootView::GetInstance()) {
            return false;
        }
        delete rootView;
        return true;
    }
#endif

    /**
     * @brief Represents the listener for monitoring physical key events.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnKeyActListener : public HeapBase {
    public:
        /**
         * @brief Responds to a physical key event.
         *
         * @param view Indicates the view displayed upon a physical key event.
         * @param event Indicates the physical key event to respond to.
         * @return Returns <b>true</b> if the view is normally displayed upon a physical key
         *         event; returns <b> false</b> otherwise.
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnKeyAct(UIView& view, const KeyEvent& event) = 0;
    };

    /**
     * @brief Obtains the view type.
     *
     * @return Returns <b>UI_ROOT_VIEW</b>, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_ROOT_VIEW;
    }

    /**
     * @brief Executes a physical key event.
     *
     * @param event Indicates the physical key event to execute.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnKeyEvent(const KeyEvent& event)
    {
        if (onKeyActListener_ != nullptr) {
            onKeyActListener_->OnKeyAct(*this, event);
        }
    }

    /**
     * @brief Sets the listener that contains a callback to be invoked upon a physical key event.
     *
     * @param onKeyActListener Indicates the pointer to the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnKeyActListener(OnKeyActListener* onKeyActListener)
    {
        onKeyActListener_ = onKeyActListener;
    }

    /**
     * @brief Clears the listener for monitoring physical key events.
     *
     * @since 1.0
     * @version 1.0
     */
    void ClearOnKeyActListener()
    {
        onKeyActListener_ = nullptr;
    }

    /**
     * @brief Listens for the input events triggered by a virtual device other than
     *        human touching or physical pressing.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnVirtualDeviceEventListener : public HeapBase {
    public:
        /**
         * @brief Responds to an input event triggered by a virtual device.
         *
          * @param view Indicates the view displayed upon an input event triggered by a virtual device.
         * @param event Indicates the input event to respond to.
         * @return Returns <b>true</b> if this view is normally displayed upon an input event triggered by a
         *         virtual device; returns <b> false</b> otherwise.
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnVirtualDeviceEvent(UIView& view, VirtualDeviceEvent event) = 0;
    };

    /**
     * @brief Executes an input event triggered by a virtual device.
     *
     * @param event Indicates the input event to respond to.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnVirtualDeviceEvent(const VirtualDeviceEvent& event)
    {
        if (onVirtualEventListener_ != nullptr) {
            onVirtualEventListener_->OnVirtualDeviceEvent(*this, event);
        }
    }

    /**
     * @brief Sets a listener for monitoring the input events of a virtual device.
     *
     * @param onVirtualDeviceEventListener Indicates the pointer to the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnVirtualDeviceEventListener(OnVirtualDeviceEventListener* onVirtualDeviceEventListener)
    {
        onVirtualEventListener_ = onVirtualDeviceEventListener;
    }

    /**
     * @brief Clears the listener for monitoring the input events of a virtual device.
     *
     * @since 1.0
     * @version 1.0
     */
    void ClearOnVirtualDeviceEventListener()
    {
        onVirtualEventListener_ = nullptr;
    }

    /**
     * @brief Checks whether the target view is one of the child views of the specified parent view.
     *
     * @param parentView Indicates the specified parent view.
     * @param subView Indicates the target child view.
     * @return Returns <b>true</b> if the target view is available; returns  <b> false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    static bool FindSubView(const UIView& parentView, const UIView* subView);

#if ENABLE_WINDOW
    /**
     * @brief Obtains the window bound with a <b>RootView</b>.
     *
     * @return Returns the window.
     * @since 1.0
     * @version 1.0
     */
    Window* GetBoundWindow() const;
#endif

    void DrawTop(UIView* view, const Rect& rect);

    /**
     * @brief Measure all child view under RootView
     */
    void Measure();

    void MeasureView(UIView* view);

    /**
     * @brief update momory info based on FB buffer info.
     *
     * @param fbBufferInfo FB buffer info
     * @since 6.0
     * @version 6.0
     */
    void UpdateBufferInfo(BufferInfo* fbBufferInfo);

    /**
     * @brief save the drawing context.
     *
     * @since 6.0
     * @version 6.0
     */
    void SaveDrawContext();

    /**
     * @brief restore the drawing context.
     *
     * @since 6.0
     * @version 6.0
     */
    void RestoreDrawContext();

private:
    friend class RenderManager;
    friend class UIViewGroup;
    friend class UIView;
#if ENABLE_WINDOW
    friend class WindowImpl;
#endif

    RootView();

    ~RootView();

    inline Rect GetScreenRect();
    void AddInvalidateRectWithLock(Rect& rect, UIView *view);
    void AddInvalidateRect(Rect& rect, UIView* view);
    void Render();
    UIView* GetTopUIView(const Rect& rect);
    void InitDrawContext();
    void DestroyDrawContext();
    void InitMapBufferInfo(BufferInfo* bufferInfo);
    void DestroyMapBufferInfo();
    void BlitMapBuffer(Rect& curViewRect, TransformMap& transMap, const Rect& invalidatedArea);
    void ClearMapBuffer();
    void UpdateMapBufferInfo(Rect& invalidatedArea);
    void RestoreMapBufferInfo();
#if LOCAL_RENDER
    void RemoveViewFromInvalidMap(UIView *view);
    void DrawInvalidMap(const Rect &buffRect);
    void OptimizeInvalidView(UIView* curview, UIView* background, List<Rect> &renderedRects);
    void OptimizeInvalidMap();

    std::map<UIView*, Graphic::Vector<Rect>> invalidateMap_;
#else
    void OptimizeAddRect(Rect& rect);
    void OptimizeInvalidateRects();
    List<Rect> invalidateRects_;
#endif

    OnKeyActListener* onKeyActListener_ {nullptr};
    OnVirtualDeviceEventListener* onVirtualEventListener_ {nullptr};
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_t lock_;
#endif

#if ENABLE_WINDOW
    WindowImpl* boundWindow_ {nullptr};
#endif
    /**
     * @brief draw context info.
     * @param bufferInfo dst drawing buffer info.
     * @param mapBufferInfo dst animator drawing buffer info.
     * @since 6.0
     * @version 6.0
     */
    struct DrawContext {
        BufferInfo* bufferInfo;
        BufferInfo* mapBufferInfo;
    };
    DrawContext dc_;
    DrawContext bakDc_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_ROOT_VIEW_H
