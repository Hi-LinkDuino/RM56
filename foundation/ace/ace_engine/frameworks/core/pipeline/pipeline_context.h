/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PIPELINE_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PIPELINE_CONTEXT_H

#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <utility>

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "base/image/pixel_map.h"
#include "base/memory/ace_type.h"
#include "base/resource/asset_manager.h"
#include "base/resource/data_provider_manager.h"
#include "base/resource/shared_image_manager.h"
#include "base/thread/task_executor.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "core/accessibility/accessibility_manager.h"
#include "core/animation/flush_event.h"
#include "core/animation/page_transition_listener.h"
#include "core/animation/schedule_task.h"
#include "core/common/draw_delegate.h"
#include "core/common/event_manager.h"
#include "core/common/focus_animation_manager.h"
#include "core/common/platform_bridge.h"
#include "core/common/platform_res_register.h"
#include "core/common/window_animation_config.h"
#include "core/components/common/properties/color.h"
#include "core/components/dialog/dialog_properties.h"
#include "core/components/page/page_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/event/event_trigger.h"
#include "core/gestures/gesture_info.h"
#include "core/image/image_cache.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/factories/render_factory.h"
#ifndef WEARABLE_PRODUCT
#include "core/event/multimodal/multimodal_manager.h"
#include "core/event/multimodal/multimodal_subscriber.h"
#endif
#include "core/common/clipboard/clipboard_proxy.h"

namespace OHOS::Rosen {
class RSUIDirector;
}

namespace OHOS::Ace {

class OffscreenCanvas;
class CardTransitionController;
class ComposedElement;
class FontManager;
class Frontend;
enum class FrontendType;
class OverlayElement;
class RenderNode;
class RenderFocusAnimation;
class RootElement;
class SharedTransitionController;
class StageElement;
class StackElement;
class Window;
class Animator;
class ManagerInterface;
class AccessibilityManager;
class RenderContext;
struct PageTarget;
class DialogComponent;
class SelectPopupComponent;
class RenderElement;

struct WindowBlurInfo {
    float progress_;
    WindowBlurStyle style_;
    RRect innerRect_;
    std::vector<RRect> coords_;
};


using OnRouterChangeCallback = bool (*)(const std::string currentRouterPath);
using SubscribeCtrlACallback = std::function<void()>;

class ACE_EXPORT PipelineContext final : public AceType {
    DECLARE_ACE_TYPE(PipelineContext, AceType);

public:
    static constexpr int32_t DEFAULT_HOVER_ENTER_ANIMATION_ID = -1;
    using TimeProvider = std::function<int64_t(void)>;
    using OnPageShowCallBack = std::function<void()>;
    using AnimationCallback = std::function<void()>;
    using GetViewScaleCallback = std::function<bool(float&, float&)>;
    using SurfaceChangedCallbackMap =
        std::unordered_map<int32_t, std::function<void(int32_t, int32_t, int32_t, int32_t)>>;

    PipelineContext(std::unique_ptr<Window> window, RefPtr<TaskExecutor> taskExecutor,
        RefPtr<AssetManager> assetManager, RefPtr<PlatformResRegister> platformResRegister,
        const RefPtr<Frontend>& frontend, int32_t instanceId);
    PipelineContext(std::unique_ptr<Window> window, RefPtr<TaskExecutor>& taskExecutor,
        RefPtr<AssetManager> assetManager, const RefPtr<Frontend>& frontend);

    ~PipelineContext() override;

    RefPtr<Element> SetupRootElement();

    // This is used for subwindow, when the subwindow is created,a new subrootElement will be built
    RefPtr<Element> SetupSubRootElement();
    RefPtr<DialogComponent> ShowDialog(const DialogProperties& dialogProperties, bool isRightToLeft,
        const std::string& inspectorTag = "");
    void CloseContextMenu();
    void GetBoundingRectData(int32_t nodeId, Rect& rect);

    RefPtr<OffscreenCanvas> CreateOffscreenCanvas(int32_t width, int32_t height);

    void PushVisibleCallback(int32_t nodeId, double ratio, std::function<void(bool, double)>&& callback);
    void RemoveVisibleChangeNode(int32_t nodeId);

    void PushPage(const RefPtr<PageComponent>& pageComponent, const RefPtr<StageElement>& stage);
    void PushPage(const RefPtr<PageComponent>& pageComponent);
    void PostponePageTransition();
    void LaunchPageTransition();

    bool CanPushPage();

    bool IsTransitionStop() const;

    void PopPage();

    void PopToPage(int32_t pageId);

    void RestorePopPage(const RefPtr<PageComponent>& pageComponent);

    bool CanPopPage();

    void ReplacePage(const RefPtr<PageComponent>& pageComponent, const RefPtr<StageElement>& stage);
    void ReplacePage(const RefPtr<PageComponent>& pageComponent);

    bool CanReplacePage();

    bool ClearInvisiblePages();

    bool CallRouterBackToPopPage();

    void NotifyAppStorage(const std::string& key, const std::string& value);

    void Dump(const std::vector<std::string>& params) const;

    void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info);

    RefPtr<StackElement> GetLastStack() const;

    RefPtr<PageElement> GetLastPage() const;

    RefPtr<RenderNode> GetLastPageRender() const;

    void AddRouterChangeCallback(const OnRouterChangeCallback& onRouterChangeCallback);

    void onRouterChange(const std::string url);

    void ScheduleUpdate(const RefPtr<ComposedComponent>& composed);

    void AddComposedElement(const ComposeId& id, const RefPtr<ComposedElement>& element);

    void RemoveComposedElement(const ComposeId& id, const RefPtr<ComposedElement>& element);

    void AddDirtyElement(const RefPtr<Element>& dirtyElement);

    void AddNeedRebuildFocusElement(const RefPtr<Element>& focusElement);

    void AddDirtyRenderNode(const RefPtr<RenderNode>& renderNode, bool overlay = false);

    void AddNeedRenderFinishNode(const RefPtr<RenderNode>& renderNode);

    void AddDirtyLayoutNode(const RefPtr<RenderNode>& renderNode);

    void AddPredictLayoutNode(const RefPtr<RenderNode>& renderNode);

    void AddGeometryChangedNode(const RefPtr<RenderNode>& renderNode);

    void AddPreFlushListener(const RefPtr<FlushEvent>& listener);

    void AddPostAnimationFlushListener(const RefPtr<FlushEvent>& listener);

    void AddPostFlushListener(const RefPtr<FlushEvent>& listener);

    void AddPageUpdateTask(std::function<void()>&& task, bool directExecute = false);

    void SetRequestedRotationNode(const WeakPtr<RenderNode>& renderNode);

    void RemoveRequestedRotationNode(const WeakPtr<RenderNode>& renderNode);

    // add schedule task and return the unique mark id.
    uint32_t AddScheduleTask(const RefPtr<ScheduleTask>& task);

    // remove schedule task by id.
    void RemoveScheduleTask(uint32_t id);

    // Called by view when touch event received.
    void OnTouchEvent(const TouchEvent& point, bool isSubPipe = false);

    // Called by container when key event received.
    // if return false, then this event needs platform to handle it.
    bool OnKeyEvent(const KeyEvent& event);

    // Called by view when mouse event received.
    void OnMouseEvent(const MouseEvent& event);

    // Called by view when axis event received.
    void OnAxisEvent(const AxisEvent& event);

    // Called by container when rotation event received.
    // if return false, then this event needs platform to handle it.
    bool OnRotationEvent(const RotationEvent& event) const;

    // Called by window when received vsync signal.
    void OnVsyncEvent(uint64_t nanoTimestamp, uint32_t frameCount);

    // Called by view when idle event.
    void OnIdle(int64_t deadline);

    void OnActionEvent(const std::string& action);

    void OnVirtualKeyboardAreaChange(Rect keyboardArea);

    // Set card position for barrierfree
    void SetCardViewPosition(int id, float offsetX, float offsetY);

    void SetCardViewAccessibilityParams(const std::string& key, bool focus);

    void FlushPipelineImmediately();

    void RegisterEventHandler(const RefPtr<AceEventHandler>& handler)
    {
        eventTrigger_.RegisterEventHandler(handler);
    }

    template<class... Args>
    void FireAsyncEvent(const EventMarker& marker, Args&&... args)
    {
        eventTrigger_.TriggerAsyncEvent(marker, std::forward<Args>(args)...);
    }

    template<class... Args>
    void FireSyncEvent(const EventMarker& marker, Args&&... args)
    {
        eventTrigger_.TriggerSyncEvent(marker, std::forward<Args>(args)...);
    }

    void PostAsyncEvent(TaskExecutor::Task&& task);

    void PostAsyncEvent(const TaskExecutor::Task& task);

    void OnSurfaceChanged(
        int32_t width, int32_t height, WindowSizeChangeReason type = WindowSizeChangeReason::UNDEFINED);

    void WindowSizeChangeAnimate(int32_t width, int32_t height, WindowSizeChangeReason type);

    void OnSurfaceDensityChanged(double density);

    void OnSystemBarHeightChanged(double statusBar, double navigationBar);

    void OnSurfaceDestroyed();

    RefPtr<Frontend> GetFrontend() const;

    FrontendType GetFrontendType() const
    {
        return frontendType_;
    }

    RefPtr<TaskExecutor> GetTaskExecutor() const
    {
        return taskExecutor_;
    }

    RefPtr<AssetManager> GetAssetManager() const
    {
        return assetManager_;
    }

    RefPtr<PlatformResRegister> GetPlatformResRegister() const
    {
        return platformResRegister_;
    }

    Window* GetWindow()
    {
        return window_.get();
    }

    WindowModal GetWindowModal() const
    {
        return windowModal_;
    }

    bool IsFullScreenModal() const
    {
        return windowModal_ == WindowModal::NORMAL || windowModal_ == WindowModal::SEMI_MODAL_FULL_SCREEN ||
               isFullWindow_;
    }

    using FinishEventHandler = std::function<void()>;
    void SetFinishEventHandler(FinishEventHandler&& listener)
    {
        finishEventHandler_ = std::move(listener);
    }

    using StartAbilityHandler = std::function<void(const std::string& address)>;
    void SetStartAbilityHandler(StartAbilityHandler&& listener)
    {
        startAbilityHandler_ = std::move(listener);
    }
    void HyperlinkStartAbility(const std::string& address) const;

    using ActionEventHandler = std::function<void(const std::string& action)>;
    void SetActionEventHandler(ActionEventHandler&& listener)
    {
        actionEventHandler_ = std::move(listener);
    }

    // SemiModal and DialogModal have their own enter/exit animation and will exit after animation done.
    void Finish(bool autoFinish = true) const;

    void RequestFullWindow(int32_t duration);

    using StatusBarEventHandler = std::function<void(const Color& color)>;
    void SetStatusBarEventHandler(StatusBarEventHandler&& listener)
    {
        statusBarBgColorEventHandler_ = std::move(listener);
    }
    void NotifyStatusBarBgColor(const Color& color) const;
    using PopupEventHandler = std::function<void()>;

    void SetPopupEventHandler(PopupEventHandler&& listener)
    {
        popupEventHandler_ = std::move(listener);
    }
    void NotifyPopupDismiss() const;

    using RouterBackEventHandler = std::function<void()>;
    void SetRouterBackEventHandler(RouterBackEventHandler&& listener)
    {
        routerBackEventHandler_ = std::move(listener);
    }
    void NotifyRouterBackDismiss() const;

    using PopPageSuccessEventHandler = std::function<void(const std::string& pageUrl, const int32_t pageId)>;
    void SetPopPageSuccessEventHandler(PopPageSuccessEventHandler&& listener)
    {
        popPageSuccessEventHandler_.push_back(std::move(listener));
    }
    void NotifyPopPageSuccessDismiss(const std::string& pageUrl, const int32_t pageId) const;

    using IsPagePathInvalidEventHandler = std::function<void(bool& isPageInvalid)>;
    void SetIsPagePathInvalidEventHandler(IsPagePathInvalidEventHandler&& listener)
    {
        isPagePathInvalidEventHandler_.push_back(std::move(listener));
    }
    void NotifyIsPagePathInvalidDismiss(bool isPageInvalid) const;

    using DestroyEventHandler = std::function<void()>;
    void SetDestroyHandler(DestroyEventHandler&& listener)
    {
        destroyEventHandler_.push_back(std::move(listener));
    }
    void NotifyDestroyEventDismiss() const;

    using DispatchTouchEventHandler = std::function<void(const TouchEvent& event)>;
    void SetDispatchTouchEventHandler(DispatchTouchEventHandler&& listener)
    {
        dispatchTouchEventHandler_.push_back(std::move(listener));
    }
    void NotifyDispatchTouchEventDismiss(const TouchEvent& event) const;

    float GetViewScale() const
    {
        return viewScale_;
    }

    // Get the dp scale which used to covert dp to logic px.
    double GetDipScale() const
    {
        return dipScale_;
    }

    double GetRootHeight() const
    {
        return rootHeight_;
    }
    RefPtr<RenderNode> DragTestAll(const TouchEvent& point);
    RefPtr<RenderNode> DragTest(const TouchEvent& point, const RefPtr<RenderNode>& renderNode, int32_t deep);

    double GetRootWidth() const
    {
        return rootWidth_;
    }

    void SetRootHeight(double rootHeight)
    {
        if (rootHeight > 0.0) {
            rootHeight_ = rootHeight;
        }
    }

    Rect GetRootRect() const;
    Rect GetStageRect() const;
    Rect GetPageRect() const;

    bool IsSurfaceReady() const
    {
        return isSurfaceReady_;
    }

    void ClearImageCache();

    RefPtr<ImageCache> GetImageCache() const;

    double NormalizeToPx(const Dimension& dimension) const;

    double ConvertPxToVp(const Dimension& dimension) const;

    void ShowFocusAnimation(
        const RRect& rrect, const Color& color, const Offset& offset, bool isIndented = false) const;

    void ShowFocusAnimation(const RRect& rrect, const Color& color, const Offset& offset, const Rect& clipRect) const;

    void ShowShadow(const RRect& rrect, const Offset& offset) const;

    void ShowShadow(const RRect& rrect, const Offset& offset, const Rect& clipRect) const;

    RefPtr<RenderFocusAnimation> GetRenderFocusAnimation() const;

    void PushFocusAnimation(const RefPtr<Element>& element) const;

    void PushShadow(const RefPtr<Element>& element) const;

    void PopFocusAnimation() const;

    void PopRootFocusAnimation() const;

    void PopShadow() const;

    void CancelFocusAnimation() const;

    void CancelShadow() const;

    void SetUseRootAnimation(bool useRoot);

    void AddDirtyFocus(const RefPtr<FocusNode>& node);

    void RefreshStageFocus();

    void ShowContainerTitle(bool isShow);

    void BlurWindowWithDrag(bool isBlur);

    RefPtr<StageElement> GetStageElement() const;

    RefPtr<ComposedElement> GetComposedElementById(const ComposeId& id);

    void SendCallbackMessageToFrontend(const std::string& callbackId, const std::string& data);

    void SendEventToFrontend(const EventMarker& eventMarker);

    void SendEventToFrontend(const EventMarker& eventMarker, const std::string& param);

    bool AccessibilityRequestFocus(const ComposeId& id);

    bool RequestFocus(const RefPtr<Element>& targetElement);

    RefPtr<AccessibilityManager> GetAccessibilityManager() const;

    void SendEventToAccessibility(const AccessibilityEvent& accessibilityEvent);

    BaseId::IdType AddPageTransitionListener(const PageTransitionListenable::CallbackFuncType& funcObject);

    const RefPtr<OverlayElement> GetOverlayElement() const;

    void RemovePageTransitionListener(typename BaseId::IdType id);

    void ClearPageTransitionListeners();

    void Destroy();

    const RefPtr<FontManager>& GetFontManager() const
    {
        return fontManager_;
    }

    void RegisterFont(const std::string& familyName, const std::string& familySrc);

    void TryLoadImageInfo(const std::string& src, std::function<void(bool, int32_t, int32_t)>&& loadCallback);

    void SetAnimationCallback(AnimationCallback&& callback);

    bool IsLastPage();

    void SetRootSize(double density, int32_t width, int32_t height);

    RefPtr<Element> GetDeactivateElement(int32_t componentId) const;

    void ClearDeactivateElements();

    void AddDeactivateElement(const int32_t id, const RefPtr<Element>& element);

    const RefPtr<RenderFactory>& GetRenderFactory() const
    {
        return renderFactory_;
    }

#ifndef WEARABLE_PRODUCT
    void SetMultimodalSubscriber(const RefPtr<MultimodalSubscriber>& multimodalSubscriber);

    const RefPtr<MultiModalManager>& GetMultiModalManager() const
    {
        return multiModalManager_;
    }
#endif

    void SetWindowOnShow();

    void SetWindowOnHide();

    void OnShow();

    void OnHide();

    void MarkForcedRefresh()
    {
        needForcedRefresh_ = true;
    }

    void SetTimeProvider(TimeProvider&& timeProvider);

    uint64_t GetTimeFromExternalTimer();

    void SetFontScale(float fontScale);

    void AddFontNode(const WeakPtr<RenderNode>& node);

    void RemoveFontNode(const WeakPtr<RenderNode>& node);

    float GetFontScale() const
    {
        return fontScale_;
    }

    void UpdateFontWeightScale();

    void LoadSystemFont(const std::function<void()>& onFondsLoaded);

    void NotifyFontNodes();

    const RefPtr<SharedTransitionController>& GetSharedTransitionController() const
    {
        return sharedTransitionController_;
    }

    const RefPtr<CardTransitionController>& GetCardTransitionController() const
    {
        return cardTransitionController_;
    }

    void SetClickPosition(const Offset& position) const;

    void SetTextFieldManager(const RefPtr<ManagerInterface>& manager);

    void RootLostFocus() const;

    void FlushFocus();

    void WindowFocus(bool isFocus) const;

    void SetIsRightToLeft(bool isRightToLeft)
    {
        isRightToLeft_ = isRightToLeft;
    }

    bool IsRightToLeft() const
    {
        return isRightToLeft_;
    }

    const RefPtr<PlatformBridge>& GetMessageBridge() const
    {
        return messageBridge_;
    }
    void SetMessageBridge(const RefPtr<PlatformBridge>& messageBridge)
    {
        messageBridge_ = messageBridge;
    }

    void SetOnPageShow(OnPageShowCallBack&& onPageShowCallBack);

    void OnPageShow();

    double GetStatusBarHeight() const
    {
        return statusBarHeight_;
    }

    double GetNavigationBarHeight() const
    {
        return navigationBarHeight_;
    }

    void SetAppearingDuration(int32_t duration)
    {
        cardAppearingDuration_ = duration;
    }

    int32_t GetAppearingDuration() const
    {
        return cardAppearingDuration_;
    }

    void SetWindowModal(WindowModal modal)
    {
        windowModal_ = modal;
    }

    void SetModalHeight(int32_t height)
    {
        modalHeight_ = height;
    }

    void SetModalColor(uint32_t color)
    {
        modalColor_ = color;
    }

    void MovePage(const Offset& rootRect, double offsetHeight);

    void SetDrawDelegate(std::unique_ptr<DrawDelegate> delegate)
    {
        drawDelegate_ = std::move(delegate);
    }

    void SetBuildAfterCallback(const std::function<void()>& callback)
    {
        buildAfterCallback_.emplace_back(callback);
    }

    RefPtr<ThemeManager> GetThemeManager() const
    {
        return themeManager_;
    }

    void SetThemeManager(RefPtr<ThemeManager> theme)
    {
        themeManager_ = theme;
    }

    void SetIsKeyEvent(bool isKeyEvent);

    bool IsKeyEvent() const
    {
        return isKeyEvent_;
    }

    void SetIsJsCard(bool isJsCard)
    {
        isJsCard_ = isJsCard;
    }

    bool IsJsCard() const
    {
        return isJsCard_;
    }

    void SetIsJsPlugin(bool isJsPlugin)
    {
        isJsPlugin_ = isJsPlugin;
    }

    bool IsJsPlugin() const
    {
        return isJsPlugin_;
    }

    void RefreshRootBgColor() const;
    void AddToHoverList(const RefPtr<RenderNode>& node);

    using UpdateWindowBlurRegionHandler = std::function<void(const std::vector<std::vector<float>>&)>;

    void SetUpdateWindowBlurRegionHandler(UpdateWindowBlurRegionHandler handler)
    {
        updateWindowBlurRegionHandler_ = std::move(handler);
    }

    using DragEventHandler = std::function<void(const std::string&, const RefPtr<PixelMap>& pixmap)>;

    void SetDragEventHandler(DragEventHandler&& callback)
    {
        dragEventHandler_ = callback;
    }

    using InitDragEventListener = std::function<void()>;

    void SetInitDragListener(InitDragEventListener&& callback)
    {
        initDragEventListener_ = callback;
    }

    void UpdateWindowBlurRegion(
        int32_t id, RRect rRect, float progress, WindowBlurStyle style, const std::vector<RRect>& coords);

    void ClearWindowBlurRegion(int32_t id);

    bool IsBuildingFirstPage() const
    {
        return buildingFirstPage_;
    }

    const RefPtr<SharedImageManager>& GetSharedImageManager() const
    {
        return sharedImageManager_;
    }

    void SetSharedImageManager(const RefPtr<SharedImageManager>& sharedImageManager)
    {
        sharedImageManager_ = sharedImageManager;
    }

    using UpdateWindowBlurDrawOpHandler = std::function<void(void)>;

    void SetUpdateWindowBlurDrawOpHandler(UpdateWindowBlurDrawOpHandler handler)
    {
        updateWindowBlurDrawOpHandler_ = std::move(handler);
    }

    void NavigatePage(uint8_t type, const PageTarget& target, const std::string& params);

    void ForceLayoutForImplicitAnimation();

    bool Animate(const AnimationOption& option, const RefPtr<Curve>& curve,
        const std::function<void()>& propertyCallback, const std::function<void()>& finishCallBack = nullptr);

    void OpenImplicitAnimation(const AnimationOption& option, const RefPtr<Curve>& curve,
        const std::function<void()>& finishCallBack = nullptr);

    bool CloseImplicitAnimation();

    void AddKeyFrame(float fraction, const RefPtr<Curve>& curve, const std::function<void()>& propertyCallback);

    void AddKeyFrame(float fraction, const std::function<void()>& propertyCallback);

    void SaveExplicitAnimationOption(const AnimationOption& option);

    void CreateExplicitAnimator(const std::function<void()>& onFinishEvent);

    void ClearExplicitAnimationOption();

    const AnimationOption GetExplicitAnimationOption() const;

    void FlushBuild();

    int32_t GetInstanceId() const
    {
        return instanceId_;
    }

    void SetUseLiteStyle(bool useLiteStyle)
    {
        useLiteStyle_ = useLiteStyle;
    }

    bool UseLiteStyle()
    {
        return useLiteStyle_;
    }

    const Rect& GetDirtyRect() const
    {
        return dirtyRect_;
    }

    bool GetIsDeclarative() const;

    bool IsForbidePlatformQuit() const
    {
        return forbidePlatformQuit_;
    }

    void SetForbidePlatformQuit(bool forbidePlatformQuit);

    void SetRootBgColor(const Color& color);

    const Color& GetRootBgColor() const
    {
        return rootBgColor_;
    }

    void SetPhotoCachePath(const std::string& photoCachePath)
    {
        photoCachePath_ = photoCachePath;
    }

    const std::string& GetPhotoCachePath()
    {
        return photoCachePath_;
    }

    int32_t GetMinPlatformVersion() const
    {
        return minPlatformVersion_;
    }
    void SetMinPlatformVersion(int32_t minPlatformVersion)
    {
        minPlatformVersion_ = minPlatformVersion;
    }

    void SetGetViewScaleCallback(GetViewScaleCallback&& callback)
    {
        getViewScaleCallback_ = callback;
    }

    bool GetViewScale(float& scaleX, float& scaleY)
    {
        if (getViewScaleCallback_) {
            return getViewScaleCallback_(scaleX, scaleY);
        }
        return false;
    }

    void SetScreenOnCallback(std::function<void(std::function<void()>&& func)>&& screenOnCallback)
    {
        screenOnCallback_ = std::move(screenOnCallback);
    }

    void SetScreenOffCallback(std::function<void(std::function<void()>&& func)>&& screenOffCallback)
    {
        screenOffCallback_ = std::move(screenOffCallback);
    }

    const RefPtr<ManagerInterface>& GetTextFieldManager()
    {
        return textFieldManager_;
    }

    void AddScreenOnEvent(std::function<void()>&& func);
    void AddScreenOffEvent(std::function<void()>&& func);
    void AddAlignDeclarationNode(const RefPtr<RenderNode>& node);
    void AddLayoutTransitionNode(const RefPtr<RenderNode>& node);
    std::list<RefPtr<RenderNode>>& GetAlignDeclarationNodeList();
    void SetQueryIfWindowInScreenCallback(std::function<void()>&& func)
    {
        queryIfWindowInScreenCallback_ = std::move(func);
    }
    void SetIsWindowInScreen(bool isWindowInScreen)
    {
        isWindowInScreen_ = isWindowInScreen;
    }
    // This interface posts an async task to do async query and returns the result from previous query.
    bool IsWindowInScreen();
    void NotifyOnPreDraw();
    void AddNodesToNotifyOnPreDraw(const RefPtr<RenderNode>& renderNode);

    void UpdateNodesNeedDrawOnPixelMap();
    void SearchNodesNeedDrawOnPixelMap(const RefPtr<RenderNode>& renderNode);
    void NotifyDrawOnPixelMap();

    const RefPtr<RootElement>& GetRootElement() const
    {
        return rootElement_;
    }

    const RefPtr<DataProviderManagerInterface>& GetDataProviderManager() const
    {
        return dataProviderManager_;
    }
    void SetDataProviderManager(const RefPtr<DataProviderManagerInterface>& dataProviderManager)
    {
        dataProviderManager_ = dataProviderManager;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void SetCurrentUrl(const std::string& url)
    {
        currentUrl_ = url;
    }

    const std::string& GetCurrentUrl()
    {
        return currentUrl_;
    }
#endif

    int32_t GetWindowId() const
    {
        return windowId_;
    }

    void SetWindowId(int32_t windowId)
    {
        windowId_ = windowId;
    }

    void SetAccessibilityEnabled(bool isEnabled)
    {
        isAccessibilityEnabled_ = isEnabled;
    }
    bool IsAccessibilityEnabled() const
    {
        return isAccessibilityEnabled_ || IsVisibleChangeNodeExists(-1);
    }

    bool IsVisibleChangeNodeExists(NodeId index) const;

    int32_t RegisterSurfaceChangedCallback(std::function<void(int32_t, int32_t, int32_t, int32_t)>&& callback)
    {
        if (callback) {
            surfaceChangedCallbackMap_.emplace(++callbackId_, std::move(callback));
            return callbackId_;
        }
        return 0;
    }

    void UnregisterSurfaceChangedCallback(int32_t callbackId)
    {
        surfaceChangedCallbackMap_.erase(callbackId);
    }
    void StartSystemDrag(const std::string& str, const RefPtr<PixelMap>& pixmap);
    void InitDragListener();
    void OnDragEvent(int32_t x, int32_t y, DragEventAction action);
    void SetPreTargetRenderNode(const RefPtr<RenderNode>& preTargetRenderNode);
    const RefPtr<RenderNode> GetPreTargetRenderNode() const;
    void SetInitRenderNode(const RefPtr<RenderNode>& initRenderNode);
    const RefPtr<RenderNode>& GetInitRenderNode() const;

    void SetContextMenu(const RefPtr<Component>& contextMenu)
    {
        contextMenu_ = contextMenu;
    }

    double GetDensity() const
    {
        return density_;
    }

    void RequestFrame();

    void SetClipHole(double left, double top, double width, double height);

    const Rect& GetTransparentHole() const
    {
        return transparentHole_;
    }

    bool GetHasMeetSubWindowNode() const
    {
        return hasMeetSubWindowNode_;
    }

    void SetHasMeetSubWindowNode(bool hasMeetSubWindowNode)
    {
        hasMeetSubWindowNode_ = hasMeetSubWindowNode;
    }

    bool GetHasClipHole() const
    {
        return hasClipHole_;
    }

    void SetHasClipHole(bool hasClipHole)
    {
        hasClipHole_ = hasClipHole;
    }

    bool GetIsHoleValid() const
    {
        return isHoleValid_;
    }

    void SetPluginOffset(const Offset& offset)
    {
        pluginOffset_ = offset;
    }

    Offset GetPluginOffset() const
    {
        return pluginOffset_;
    }

    void SetPluginEventOffset(const Offset& offset)
    {
        pluginEventOffset_ = offset;
    }

    Offset GetPluginEventOffset() const
    {
        return pluginEventOffset_;
    }

    void SetTouchPipeline(WeakPtr<PipelineContext> context);
    void RemoveTouchPipeline(WeakPtr<PipelineContext> context);

    bool IsRebuildFinished() const
    {
        return isRebuildFinished_;
    }

    void SetRSUIDirector(std::shared_ptr<OHOS::Rosen::RSUIDirector> rsUIDirector);

    std::shared_ptr<OHOS::Rosen::RSUIDirector> GetRSUIDirector();

    void SetOnVsyncProfiler(const std::function<void(const std::string&)> callback)
    {
        onVsyncProfiler_ = callback;
    }

    void ResetOnVsyncProfiler()
    {
        onVsyncProfiler_ = nullptr;
    }

    bool IsShiftDown() const
    {
        return isShiftDown_;
    }

    void MarkIsShiftDown(bool isShiftDown)
    {
        isShiftDown_ = isShiftDown;
    }

    bool IsCtrlDown() const
    {
        return isCtrlDown_;
    }

    void MarkIsCtrlDown(bool isCtrlDown)
    {
        isCtrlDown_ = isCtrlDown;
    }

    bool IsKeyboardA() const
    {
        return isKeyboardA_;
    }

    void MarkIsKeyboardA(bool isKeyboardA)
    {
        isKeyboardA_ = isKeyboardA;
    }

    void SetShortcutKey(const KeyEvent& event);

    void SetEventManager(const RefPtr<EventManager> eventManager)
    {
        eventManager_ = eventManager;
    }

    RefPtr<EventManager> GetEventManager() const
    {
        return eventManager_;
    }

    void SubscribeCtrlA(SubscribeCtrlACallback callback)
    {
        subscribeCtrlA_ = callback;
    }

    void SetWindowMinimizeCallBack(std::function<bool(void)>&& callback)
    {
        windowMinimizeCallback_ = std::move(callback);
    }

    void SetWindowMaximizeCallBack(std::function<bool(void)>&& callback)
    {
        windowMaximizeCallback_ = std::move(callback);
    }

    void SetWindowRecoverCallBack(std::function<bool(void)>&& callback)
    {
        windowRecoverCallback_ = std::move(callback);
    }

    void SetWindowCloseCallBack(std::function<bool(void)>&& callback)
    {
        windowCloseCallback_ = std::move(callback);
    }

    void SetWindowSplitCallBack(std::function<bool(void)>&& callback)
    {
        windowSplitCallback_ = std::move(callback);
    }

    void SetWindowGetModeCallBack(std::function<WindowMode(void)>&& callback)
    {
        windowGetModeCallback_ = std::move(callback);
    }

    void SetWindowStartMoveCallBack(std::function<void(void)>&& callback)
    {
        windowStartMoveCallback_ = std::move(callback);
    }

    void SetGetWindowRectImpl(std::function<Rect()>&& callback)
    {
        windowRectImpl_ = std::move(callback);
    }

    Rect GetCurrentWindowRect() const
    {
        Rect rect;
        if (windowRectImpl_) {
            rect = windowRectImpl_();
        }
        return rect;
    }

    bool FireWindowMinimizeCallBack() const
    {
        if (windowMinimizeCallback_) {
            return windowMinimizeCallback_();
        }
        return false;
    }

    bool FireWindowMaximizeCallBack() const
    {
        if (windowMaximizeCallback_) {
            return windowMaximizeCallback_();
        }
        return false;
    }

    bool FireWindowRecoverCallBack() const
    {
        if (windowRecoverCallback_) {
            return windowRecoverCallback_();
        }
        return false;
    }

    bool FireWindowSplitCallBack() const
    {
        if (windowSplitCallback_) {
            return windowSplitCallback_();
        }
        return false;
    }

    bool FireWindowCloseCallBack() const
    {
        if (windowCloseCallback_) {
            return windowCloseCallback_();
        }
        return false;
    }

    void FireWindowStartMoveCallBack() const
    {
        if (windowStartMoveCallback_) {
            windowStartMoveCallback_();
        }
    }

    WindowMode FireWindowGetModeCallBack() const
    {
        if (windowGetModeCallback_) {
            return windowGetModeCallback_();
        }
        return WindowMode::WINDOW_MODE_UNDEFINED;
    }

    void SetAppIconId(int32_t id)
    {
        appIconId_ = id;
    }

    int32_t GetAppIconId() const
    {
        return appIconId_;
    }

    void SetAppLabelId(int32_t id)
    {
        appLabelId_ = id;
    }

    int32_t GetAppLabelId() const
    {
        return appLabelId_;
    }

    void SetClipboardCallback(const std::function<void(const std::string&)>& callback)
    {
        clipboardCallback_ = callback;
    }

    void ProcessDragEvent(
        const RefPtr<RenderNode>& renderNode, const RefPtr<DragEvent>& event, const Point& globalPoint);
    void ProcessDragEventEnd(
        const RefPtr<RenderNode>& renderNode, const RefPtr<DragEvent>& event, const Point& globalPoint);

    void StoreNode(int32_t restoreId, const WeakPtr<RenderElement>& node);

    std::unique_ptr<JsonValue> GetStoredNodeInfo();

    void RestoreNodeInfo(std::unique_ptr<JsonValue> nodeInfo);

    std::string GetRestoreInfo(int32_t restoreId);

    void SetIsSubPipeline(bool isSubPipeline)
    {
        isSubPipeline_ = isSubPipeline;
    }

    bool IsSubPipeline() const
    {
        return isSubPipeline_;
    }

    bool GetIsDragStart() const
    {
        return isDragStart_;
    }

private:
    void FlushVsync(uint64_t nanoTimestamp, uint32_t frameCount);
    void FlushPipelineWithoutAnimation();
    void FlushLayout();
    void FlushGeometryProperties();
    void FlushRender();
    void FlushMessages();
    void FlushRenderFinish();
    void FireVisibleChangeEvent();
    void FlushPredictLayout(int64_t deadline);
    void FlushAnimation(uint64_t nanoTimestamp);
    void FlushPostAnimation();
    void FlushPageUpdateTasks();
    void ProcessPreFlush();
    void ProcessPostFlush();
    void SetRootSizeWithWidthHeight(int32_t width, int32_t height, int32_t offset = 0);
    void SetRootRect(double width, double height, double offset = 0.0) const;
    void FlushBuildAndLayoutBeforeSurfaceReady();
    void FlushAnimationTasks();
    void DumpAccessibility(const std::vector<std::string>& params) const;
    void FlushWindowBlur();
    void MakeThreadStuck(const std::vector<std::string>& params) const;
    void DumpFrontend() const;
    void ExitAnimation();
    void CreateGeometryTransition();
    void CorrectPosition();

    template<typename T>
    struct NodeCompare {
        bool operator()(const T& nodeLeft, const T& nodeRight)
        {
            if (nodeLeft->GetDepth() < nodeRight->GetDepth()) {
                return true;
            } else if (nodeLeft->GetDepth() == nodeRight->GetDepth()) {
                return nodeLeft < nodeRight;
            }
            return false;
        }
    };

    template<typename T>
    struct NodeCompareWeak {
        bool operator()(const T& nodeLeftWeak, const T& nodeRightWeak)
        {
            auto nodeLeft = nodeLeftWeak.Upgrade();
            auto nodeRight = nodeRightWeak.Upgrade();
            if (!nodeLeft || !nodeRight) {
                return false;
            }
            auto compare = NodeCompare<decltype(nodeLeft)>();
            return compare(nodeLeft, nodeRight);
        }
    };

    Rect dirtyRect_;
    uint32_t nextScheduleTaskId_ = 0;
    std::unordered_map<uint32_t, RefPtr<ScheduleTask>> scheduleTasks_;
    std::unordered_map<ComposeId, std::list<RefPtr<ComposedElement>>> composedElementMap_;
    std::set<WeakPtr<Element>, NodeCompareWeak<WeakPtr<Element>>> dirtyElements_;
    std::set<WeakPtr<Element>, NodeCompareWeak<WeakPtr<Element>>> needRebuildFocusElement_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> dirtyRenderNodes_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> dirtyRenderNodesInOverlay_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> dirtyLayoutNodes_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> predictLayoutNodes_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> needPaintFinishNodes_;
    std::set<RefPtr<RenderNode>, NodeCompare<RefPtr<RenderNode>>> geometryChangedNodes_;
    std::set<RefPtr<RenderNode>> nodesToNotifyOnPreDraw_;
    std::set<RefPtr<RenderNode>> nodesNeedDrawOnPixelMap_;
    std::list<RefPtr<FlushEvent>> postFlushListeners_;
    std::list<RefPtr<FlushEvent>> postAnimationFlushListeners_;
    std::list<RefPtr<FlushEvent>> preFlushListeners_;
    std::unique_ptr<Window> window_;
    RefPtr<FocusAnimationManager> focusAnimationManager_;
    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<AssetManager> assetManager_;
    RefPtr<DataProviderManagerInterface> dataProviderManager_;
    RefPtr<PlatformResRegister> platformResRegister_;
    RefPtr<RootElement> rootElement_;
    RefPtr<FocusNode> dirtyFocusNode_;
    RefPtr<FocusNode> dirtyFocusScope_;
    WeakPtr<Frontend> weakFrontend_;
    RefPtr<ImageCache> imageCache_;
    RefPtr<FontManager> fontManager_;
    RefPtr<ThemeManager> themeManager_;
    RefPtr<SharedImageManager> sharedImageManager_;
    std::list<std::function<void()>> buildAfterCallback_;
    RefPtr<RenderFactory> renderFactory_;
    UpdateWindowBlurRegionHandler updateWindowBlurRegionHandler_;
    UpdateWindowBlurDrawOpHandler updateWindowBlurDrawOpHandler_;
    DragEventHandler dragEventHandler_;
    InitDragEventListener initDragEventListener_;
    GetViewScaleCallback getViewScaleCallback_;
    std::stack<bool> pendingImplicitLayout_;

    Rect transparentHole_;
    // use for traversing cliping hole
    bool hasMeetSubWindowNode_ = false;
    // use for judge clip hole status
    bool hasClipHole_ = false;
    // judge hole is valid
    bool isHoleValid_ = false;

#ifndef WEARABLE_PRODUCT
    RefPtr<MultiModalManager> multiModalManager_ = MakeRefPtr<MultiModalManager>();
#endif
    RefPtr<SharedTransitionController> sharedTransitionController_;
    RefPtr<CardTransitionController> cardTransitionController_;
    RefPtr<EventManager> eventManager_;
    EventTrigger eventTrigger_;
    FinishEventHandler finishEventHandler_;
    StartAbilityHandler startAbilityHandler_;
    ActionEventHandler actionEventHandler_;
    StatusBarEventHandler statusBarBgColorEventHandler_;
    PopupEventHandler popupEventHandler_;
    RouterBackEventHandler routerBackEventHandler_;
    std::list<PopPageSuccessEventHandler> popPageSuccessEventHandler_;
    std::list<IsPagePathInvalidEventHandler> isPagePathInvalidEventHandler_;
    std::list<DestroyEventHandler> destroyEventHandler_;
    std::list<DispatchTouchEventHandler> dispatchTouchEventHandler_;

    RefPtr<ManagerInterface> textFieldManager_;
    RefPtr<PlatformBridge> messageBridge_;
    WeakPtr<RenderNode> requestedRenderNode_;
    // Make page update tasks pending here to avoid block receiving vsync.
    std::queue<std::function<void()>> pageUpdateTasks_;
    // strong deactivate element and it's id.
    std::map<int32_t, RefPtr<Element>> deactivateElements_;

    RefPtr<Component> contextMenu_;
    // animation frame callback
    AnimationCallback animationCallback_;

    // window blur region
    std::unordered_map<int32_t, WindowBlurInfo> windowBlurRegions_;

    std::list<RefPtr<RenderNode>> alignDeclarationNodeList_;
    std::set<RefPtr<RenderNode>> layoutTransitionNodeSet_;

    std::function<void()> queryIfWindowInScreenCallback_;
    std::atomic<bool> isWindowInScreen_ = true;

    RefPtr<RenderNode> preTargetRenderNode_;

    bool isRightToLeft_ = false;
    bool isSurfaceReady_ = false;
    float viewScale_ = 1.0f;
    float fontScale_ = 1.0f;
    double density_ = 1.0;
    double dipScale_ = 1.0;
    float designWidthScale_ = 1.0;

    int32_t cardAppearingDuration_ = 0;
    double statusBarHeight_ = 0.0;     // dp
    double navigationBarHeight_ = 0.0; // dp
    double rootHeight_ = 0.0;
    double rootWidth_ = 0.0;
    bool needForcedRefresh_ = false;
    bool isFlushingAnimation_ = false;
    bool hasIdleTasks_ = false;
    bool isMoving_ = false;
    std::atomic<bool> onShow_ = true;
    bool isKeyEvent_ = false;
    bool needWindowBlurRegionRefresh_ = false;
    bool isJsCard_ = false;
    bool isJsPlugin_ = false;
    bool useLiteStyle_ = false;
    bool isFirstLoaded_ = true;
    bool isDragStart_ = false;
    bool isFirstDrag_ = true;
    uint64_t flushAnimationTimestamp_ = 0;
    TimeProvider timeProvider_;
    OnPageShowCallBack onPageShowCallBack_;
    WindowModal windowModal_ = WindowModal::NORMAL;
    int32_t modalHeight_ = 0;
    int32_t hoverNodeId_ = DEFAULT_HOVER_ENTER_ANIMATION_ID;
    uint32_t modalColor_ = 0x00000000;
    bool isFullWindow_ = false;
    std::list<RefPtr<RenderNode>> hoverNodes_;
    std::unique_ptr<DrawDelegate> drawDelegate_;
    std::function<void(std::function<void()>&&)> screenOffCallback_;
    std::function<void(std::function<void()>&&)> screenOnCallback_;
#if defined(ENABLE_NATIVE_VIEW)
    int32_t frameCount_ = 0;
#endif

    Color rootBgColor_ = Color::WHITE;
    int32_t width_ = 0;
    int32_t height_ = 0;
    bool isFirstPage_ = true;
    bool buildingFirstPage_ = false;
    bool forbidePlatformQuit_ = false;
    FrontendType frontendType_;
    int32_t instanceId_ = 0;
    int32_t minPlatformVersion_ = 0;
    std::string photoCachePath_;
    AnimationOption explicitAnimationOption_;
    std::map<int32_t, RefPtr<Animator>> explicitAnimators_;
    OnRouterChangeCallback OnRouterChangeCallback_ = nullptr;
    bool isAccessibilityEnabled_ = false;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string currentUrl_;
#endif
    int32_t windowId_ = 0;

    int32_t callbackId_ = 0;
    SurfaceChangedCallbackMap surfaceChangedCallbackMap_;

    std::vector<WeakPtr<PipelineContext>> touchPluginPipelineContext_;
    Offset pluginOffset_ { 0, 0 };
    Offset pluginEventOffset_ { 0, 0 };

    bool isRebuildFinished_ = false;
    std::shared_ptr<OHOS::Rosen::RSUIDirector> rsUIDirector_;

    std::function<void(const std::string&)> onVsyncProfiler_;

    bool isShiftDown_ = false;
    bool isCtrlDown_ = false;
    bool isKeyboardA_ = false;
    SubscribeCtrlACallback subscribeCtrlA_;

    int32_t appLabelId_ = 0;
    int32_t appIconId_ = 0;
    std::function<bool(void)> windowMinimizeCallback_ = nullptr;
    std::function<bool(void)> windowMaximizeCallback_ = nullptr;
    std::function<bool(void)> windowRecoverCallback_ = nullptr;
    std::function<bool(void)> windowCloseCallback_ = nullptr;
    std::function<bool(void)> windowSplitCallback_ = nullptr;
    std::function<void(void)> windowStartMoveCallback_ = nullptr;
    std::function<WindowMode(void)> windowGetModeCallback_ = nullptr;
    std::function<Rect()> windowRectImpl_ = nullptr;

    std::function<void(const std::string&)> clipboardCallback_ = nullptr;
    Size selectedItemSize_ { 0.0, 0.0 };
    size_t selectedIndex_ = -1;
    size_t insertIndex_ = -1;
    RefPtr<Clipboard> clipboard_;
    RefPtr<RenderNode> initRenderNode_;
    std::string customDragInfo_;
    Offset pageOffset_;
    Offset rootOffset_;

    std::unordered_map<int32_t, WeakPtr<RenderElement>> storeNode_;
    std::unordered_map<int32_t, std::string> restoreNodeInfo_;

    bool isSubPipeline_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(PipelineContext);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_PIPELINE_CONTEXT_H
