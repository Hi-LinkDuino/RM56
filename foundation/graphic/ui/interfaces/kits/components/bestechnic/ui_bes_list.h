/**
 * @file ui_bes_list.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-01-18
 * @brief  对 ui_ist.h 的增强。This scrollable list can be used with
 *        the adapter {@link AbstractAdapter} to implement scrolling, inertial scrolling, automatic alignment, and
 *        invoking of a callback when a child view at the preset position is selected as this list scrolls. It is used
 *        when there is a large number of child views with a fixed format.
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_BES_LIST_H
#define GRAPHIC_LITE_UI_BES_LIST_H

#include "components/abstract_adapter.h"
#include "components/ui_abstract_scroll.h"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "gfx_utils/list.h"

/**
 * 滚动条自动隐藏功能开关
 * 2023年2月7日 此功能打开后，频繁使用会导致 JS HEAP OOM。未解决，正式版本暂时屏蔽
 */
#define FEATURE_BES_LIST_SCROLL_BAR_AUTO_HIDE 0
/**
 * 滚动条动态设置自动隐藏功能开关
 * 2023年2月10日 此功能打开后，频繁使用会导致 JS HEAP OOM。未解决，正式版本暂时屏蔽
 */
#define BES_LIST_SET_SCROLL_BAR_SIDE 0
/**
 * 滚动条动态设置启用/禁用功能开关
 * 2023年2月10日 此功能打开后，频繁使用会导致 JS HEAP OOM。未解决，正式版本暂时屏蔽
 */
#define BES_LIST_SET_SCROLL_BAR_ENABLE 0

namespace OHOS {

//----- OHOS::BesListScrollListener

/**
 * @brief Represents a listener that contains a callback to be invoked when the scroll state changes or when a new child
 *        view is selected at the preset position as this list scrolls. The scroll state can be {@link
 *        SCROLL_STATE_STOP} or {@link SCROLL_STATE_MOVE}.
 * @since 1.0
 * @version 1.0
 */
class BesListScrollListener : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>ListScrollListener</b> instance with the default state
     *        {@link SCROLL_STATE_STOP}.
     * @since 1.0
     * @version 1.0
     */
    BesListScrollListener() : state_(SCROLL_STATE_STOP) {}

    /**
     * @brief A destructor used to delete the <b>BesListScrollListener</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~BesListScrollListener() {}

    /**
     * @brief Called when a scroll starts.
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnScrollStart(int16_t index, UIView* view) {}

    /**
     * @brief Called when a scroll ends.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     *
     */
    virtual void OnScrollEnd(int16_t index, UIView* view) {}

    /**
     * @brief Called when move to the top of scroll.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 8
     * @version 8
     *
     */
    virtual void OnScrollTop(int16_t index, UIView* view) {}

    /**
     * @brief Called when move to the bottom of scroll.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 8
     * @version 8
     *
     */
    virtual void OnScrollBottom(int16_t index, UIView* view) {}

    /**
     * @brief Called when a new child view is selected at the preset position as this list scrolls. For details about
     * how to set the position, see {@link SetSelectPosition}.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     *
     */
    virtual void OnItemSelected(int16_t index, UIView* view) {}

    /**
     * @brief Obtains the scroll state of this list.
     *
     * @return Returns the scroll state, either {@link SCROLL_STATE_STOP} or {@link SCROLL_STATE_MOVE}.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetScrollState() const
    {
        return state_;
    }

    void SetScrollState(uint8_t state)
    {
        state_ = state;
    }

    static constexpr uint8_t SCROLL_STATE_STOP = 0;
    static constexpr uint8_t SCROLL_STATE_MOVE = 1;

private:
    friend class UIBesList;
    uint8_t state_;
};

//----- OHOS::UIBesList

/**
 * @brief 滚动状态控制量
 */
typedef union {
    struct {
        /** @brief 1 表示允许负向滚动（从右往左，从下往上） */
        bool backward : 1;
        /** @brief true 1 表示允许正向滚动（从左往右，从上往下） */
        bool forward : 1;
        /** 状态位，1 表示滚动，0 表示禁用滚动且时忽略 backward 和 forward */
        bool enable : 6;
    };
    /** @brief 滚动状态控制量 */
    int8_t state;
} DragState;

/**
 * @brief 滚动条的位置
 */
enum BesScrollBarSide : uint8_t {
    /** @brief 右侧 */
    BES_SCROLL_BAR_RIGHT_SIDE = ScrollBarSide::SCROLL_BAR_RIGHT_SIDE,
    /** @brief 右侧 */
    BES_SCROLL_BAR_LEFT_SIDE,
    /** @brief 顶部 */
    BES_SCROLL_BAR_TOP_SIDE,
    /** @brief 底部 */
    BES_SCROLL_BAR_BOTTOM_SIDE,
    BES_SCROLL_BAR_SIDE_MAX
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 列表滚动条的样式信息结构体
 */
struct ScrollBarStyle {

    ScrollBarStyle()
        : enable(false), isArc(false), size(10),
            bgColor(Color::ColorTo32(Color::White())), bgOpa(OPA_OPAQUE / 2),
            foreColor(Color::ColorTo32(Color::Orange())), foreOpa(OPA_OPAQUE),
            side(BES_SCROLL_BAR_SIDE_MAX), anim(0), autoHide(false), lenFactor(0.5f)
    {
    }

    /** @brief 滚动条启用状态 */
    bool enable;
    /** @brief 滚动条是否为弧形， false 表示普通矩形 */
    bool isArc;
    /** @brief 滚动条大小（Y 轴方向上的滚动条表现为宽度，X 轴方向上的滚动条表现为高度） */
    int16_t size;
    /** @brief 滚动条背景 rgb 颜色 */
    uint32_t bgColor;
    /** @brief 滚动条背景不透明度 */
    uint8_t bgOpa;
    /** @brief 滚动条滑块 rgb 颜色 */
    uint32_t foreColor;
    /** @brief 滚动条滑块不透明度 */
    uint8_t foreOpa;
    /** @brief 滚动条位置，取值见 {@link BesScrollBarSide} */
    uint8_t side;
    /** @brief 滚动条动画 */
    uint16_t anim;
    /** @brief 滚动条是否自动隐藏 */
    bool autoHide;
    /** @brief 滚动条的长度因子 */
    float lenFactor;
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 对 {@link UIList} 的重构，用于增加新特性以及修改一些 UI 实现
 * @date 2023年1月18日
 * 
 * @since 1.0
 * @version 1.0
 * 
 * @see UIList
 */
class UIBesList : public UIAbstractScroll {

 public:
    /**
     * @brief Construct a new UIBesList object
     * @since 1.0
     * @version 1.0
     */
    UIBesList();

    /**
     * @brief 构造函数
     * 
     * @param direction 列表方向，取值为 VERTICAL(0) 或 HORIZONTAL(1)
     */
    explicit UIBesList(uint8_t direction);

    /**
     * @brief A destructor used to delete the <b>UIBesList</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIBesList();

    /**
     * @brief 覆盖基类函数，做自己的实现。设置是否允许用户拖动列表（drag 事件）
     * @param draggable 是否允许拖动列表
     */
    void SetDraggable(bool draggable);

    /**
     * @brief 覆盖基类函数，做自己的实现。是否允许用户拖动列表（drag 事件）
     * 
     * @return true 
     * @return false 
     */
    bool IsDraggable() const;

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    virtual UIViewType GetViewType() const override
    {
        return UI_LIST;
    }

    virtual bool OnDragStartEvent(const DragEvent& event) override;

    /**
     * @brief 列表拖拽事件（滑动）回调函数
     * 
     * @param event 列表拖拽事件
     * @return true 消耗此事件，事件将不再传递到此 view 的容器组件。
     * @return false 不消耗此事件，事件将会传递到此 view 的容器组件。
     */
    virtual bool OnDragEvent(const DragEvent& event) override;

    /**
     * @brief 列表滑动（拖拽）结束事件的回调函数
     * 
     * @param event 列表滑动事件
     * @return true 消耗此事件，事件将不再传递到此 view 的容器组件。
     * @return false 不消耗此事件，事件将会传递到此 view 的容器组件。
     */
    virtual bool OnDragEndEvent(const DragEvent& event) override;

    /**
     * @brief 列表接收到触摸（触摸屏幕或按下鼠标键）事件的回调函数
     * 
     * @param event 列表触摸事件
     * @return true 消耗此事件，事件将不再传递到此 view 的容器组件。
     * @return false 不消耗此事件，事件将会传递到此 view 的容器组件。
     */
    virtual bool OnPressEvent(const PressEvent& event) override;

    virtual bool OnReleaseEvent(const ReleaseEvent& event) override;
    virtual bool OnCancelEvent(const CancelEvent& event) override;
    virtual bool OnClickEvent(const ClickEvent& event) override;
    virtual bool OnLongPressEvent(const LongPressEvent& event) override;

#if ENABLE_ROTATE_INPUT
    bool OnRotateStartEvent(const RotateEvent& event) override;
    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    /**
     * @brief Sets the adapter for this list. The content of this list is initialized when the adapter is set.
     *
     * @param adapter Indicates the adapter to set. For details, see {@link AbstractAdapter}.
     * @since 1.0
     * @version 1.0
     */
    void SetAdapter(AbstractAdapter* adapter);

    /**
     * @brief Moves the position of all child views.
     *
     * @param offsetX Indicates the offset distance by which a child view is moved on the x-axis.
     * @param offsetY Indicates the offset distance by which a child view is moved on the y-axis.
     * @since 1.0
     * @version 1.0
     */
    virtual void MoveChildByOffset(int16_t x, int16_t y) override;

    /**
     * @brief Scrolls to change the index of the first row or column of the current view.
     *
     * @param index Indicates the new index of the first row or column of the current view.
     * @since 1.0
     * @version 1.0
     */
    void ScrollTo(uint16_t index);

    /**
     * @brief Scrolls the content in this list.
     *
     * @param distance Indicates the distance by which the content is scrolled.
     * @since 1.0
     * @version 1.0
     */
    void ScrollBy(int16_t distance);

    /**
     * @brief Sets the start index for this list.
     *
     * @param index Indicates the start index to set. The default value is <b>0</b>.
     * @since 1.0
     * @version 1.0
     *
     */
    void SetStartIndex(uint16_t index)
    {
        startIndex_ = index;
    }

    /**
     * @brief Obtains the start index of this list. The default value is <b>0</b>.
     *
     * @return Returns the start index.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetStartIndex() const
    {
        return startIndex_;
    }

    /**
     * @brief Sets the loop state for this list, in which a loop scroll is possible since the top and bottom of the list
     *        are connected together.
     *
     * @param state Indicates the loop state to set.
     * @since 1.0
     * @version 1.0
     */
    void SetLoopState(bool state)
    {
        isLoopList_ = state;
    }

    /**
     * @brief Checks whether this list is in a loop state.
     *
     * @return Returns if the list is in the loop state; returns <b>false</b> if the list is in the common state.
     * @since 1.0
     * @version 1.0
     */
    bool GetLoopState() const
    {
        return isLoopList_;
    }

    /**
     * @brief Sets the position where a child view is selected as this list scrolls.
     *
     * When a child view is selected at the specified position, the callback {@link ListScrollListener} is invoked.
     * You can implement the zoom-in and color-change effects in the callback.
     *
     * @param position Indicates the position to set. The default value is <b>0</b>, indicating that no position is
     *                 set.
     * @since 1.0
     * @version 1.0
     */
    void SetSelectPosition(uint16_t position)
    {
        selectPosition_ = position;
    }

    /**
     * @brief Obtains the child view being selected at the preset position.
     *
     * @return Returns the child view if available; returns <b>NULL</b> if no child view is selected or no position is
     *         set.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetSelectView();

    /**
     * @brief Sets the listener that contains a callback to be invoked when a child view is selected as this list
     *        scrolls.
     *
     * @param scrollListener Indicates the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetScrollStateListener(BesListScrollListener* scrollListener)
    {
        scrollListener_ = scrollListener;
    }

    /**
     * @brief Refreshes this list. The number of child views in the current view is fixed and the positions of those
     *        reserved child views as this list scrolls remain unchanged.
     * @since 1.0
     * @version 1.0
     */
    void RefreshList();

    /**
     * @brief Sets the automatic alignment state for this list. When a scroll stops, a child view is selected and its
     *        position is automatically aligned with the preset position.
     *
     * @param state Indicates the automatic alignment state. <b>true</b> indicates this state is enabled, and
     *              <b>false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void EnableAutoAlign(bool state)
    {
        autoAlign_ = state;
    }

    /**
     * @brief
     * 设置自动对齐动画时长，单位为毫秒，默认为100毫秒。该功能依赖EnableAutoAlign()方法，自动对齐设置为true情况下才生效。
     *
     * @param value 自动对齐动画时长,0表示无动画。
     * @since 3.0
     * @version 3.0
     */
    void SetAutoAlignTime(uint16_t time)
    {
        alignTime_ = time;
    }

    /**
     * @brief Sets the automatic alignment mode for this list when auto align state is enabled.
     *
     * @param dragBack mode of alignment.
     *        true means list item will move back when its position crosses the line of select
     *        position partly (but not all of it).
     *        false means list item will move front when its position crosses the line of select
     *        position (no matter how many it crosses).
     *
     * @since 3.0
     * @version 3.0
     */
    void EnableCrossDragBack(bool dragBack)
    {
        dragBack_ = dragBack;
    }

    /**
     * @brief 获取自动对齐动画时长。
     *
     * @return 自动对齐动画时长。
     * @since 3.0
     * @version 3.0
     */
    uint16_t GetAutoAlignTime() const
    {
        return alignTime_;
    }

    /**
     * @brief 覆盖基类函数，设置 X 轴坐标方向的滚动条的可见性，覆盖基类函数
     * 
     * @param visible
     */
    void SetXScrollBarVisible(bool visible);

    /**
     * @brief 覆盖基类函数，设置 Y 轴坐标方向的滚动条的可见性，覆盖基类函数
     * 
     * @param visible
     */
    void SetYScrollBarVisible(bool visible);

    virtual void RemoveAll() override;

    /**
     * @brief 在控件绘制完成后，触发此回调函数。位于 UIView::OnDraw(...) 之后
     * 
     * @param gfxDstBuffer
     * @param invalidatedArea 刷新区域
     */
    virtual void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief 渲染 ScrollBar
     */
    void RenderScrollBar();

    /**
     * @brief 刷新滚动条
     */
    void RefreshScrollBar();

    void SetScrollBarEnable(bool enable);

    /**
     * @brief 设置滚动条类型（弧形 / 矩形）
     * 
     * @param isArc 是否为圆弧
     */
    void SetScrollBarType(bool isArc);

    /**
     * @brief 设置滚动条显示的目标位置
     * 
     * @param side 取值见 BesScrollBarSide
     */
    void SetScrollBarSide(uint8_t side);

    void SetScrollBarSize(int16_t size);

    /**
     * @brief 设置滚动条背景样式
     * 
     * @param opacity 不透明度
     * @param rgb rgb颜色值，高 8 位的 alpha 位必须置为 0xFF
     */
    void SetScrollBarBackground(uint8_t opacity, uint32_t rgb);

    /**
     * @brief 设置滚动条滑块样式
     * 
     * @param opacity 不透明度
     * @param rgb rgb 颜色，高 8 位的 alpha 位必须置为 0xFF
     */
    void SetScrollBarForeground(uint8_t opacity, uint32_t rgb);

    /**
     * @brief Set the Scroll Bar Anim object
     * 
     * @param anim 
     */
    void SetScrollBarAnim(uint16_t anim);

    /**
     * @brief 设置滚动条是否自动隐藏（在用户操作结束之后）
     * 
     * @param autoHide 
     */
    void SetScrollBarAutoHide(bool autoHide);

    /**
     * @brief 设置滚动条长度因子
     * 
     * @param factor 滚动条的长度因子，即滚动条等效长度与列表显示长度的比率百分值，范围为 (0.0f, 1.0f]
     */
    void SetScrollBarLenFactor(float factor);

    /**
     * @brief 获取列表拖拽的可用状态量
     * 
     * @return DragState 
     */
    virtual DragState GetDragState();

    /**
     * @brief 获取滚动条样式参数对象
     * 
     * @return ScrollBarStyle& 
     */
    ScrollBarStyle& GetScrollBarStyle();

    static constexpr int8_t NULL_SELECT_INDEX = -1;

    UIView* onSelectedView_;

protected:
    static constexpr int16_t RECALCULATE_DRAG_DISTANCE = 10;
    static constexpr int16_t RECALCULATE_DRAG_TIMES = 10;
    static constexpr int16_t DEFAULT_ALIGN_TIMES = 100;

    virtual void StopAnimator() override;

    /** 
     * @brief 在 X 轴方向上滚动指定距离
     * @return true 表示列表在 X 轴方向上成功滚动目标距离。注意，当目标距离为 0 时，也将命中
     * @return false 表示无法滚动目标距离
     */
    virtual bool DragXInner(int16_t distance) override;

    /** 
     * @brief 在 Y 轴方向上滚动指定距离
     * @return true 表示列表在 Y 轴方向上成功滚动目标距离。注意，当目标距离为 0 时，也将命中
     * @return false 表示无法滚动目标距离
     */
    virtual bool DragYInner(int16_t distance) override;

    /** @brief 覆盖基类函数，以添加 X 轴方向上的滚动条支持（基类 UIList 中只处理了 Y 轴方向上的滚动条） */
    virtual void UpdateScrollBar();

    /** 
     * @brief 判断drag 事件是否需要被消费。当列表不再需要响应 drag 事件，应该归还给外层容器
     * @return
     */
    virtual bool IsDragConsumed(const DragEvent& event);

    /** 
     * @brief 设置 X 轴滚动控制状态量
     * @param backward 负向滚动控制位
     * @param forward 正向滚动控制位
     */
    void SetXDragState(bool backward, bool forward);

    /** 
     * @brief 设置 Y 轴滚动控制状态量
     * @param backward 负向滚动控制位
     * @param forward 正向滚动控制位
     */
    void SetYDragState(bool backward, bool forward);

    ScrollBarStyle* style_ = nullptr;

private:
    friend class UIPicker;
    friend class Recycle;
    class Recycle : public HeapBase {
    public:
        explicit Recycle(UIBesList* list) : adapter_(nullptr), listView_(list), hasInitialiszed_(false) {}
        virtual ~Recycle();
        void InitRecycle();
        UIView* GetView(int16_t index);
        void SetAdapter(AbstractAdapter* adapter)
        {
            hasInitialiszed_ = false;
            adapter_ = adapter;
        }

        bool HasInitialiszed()
        {
            return hasInitialiszed_;
        }

        void AddScrapView(UIView* view)
        {
            scrapView_.PushBack(view);
        }

        uint16_t GetAdapterItemCount()
        {
            return (adapter_ == nullptr) ? 0 : adapter_->GetCount();
        }

        void ClearScrapView()
        {
            scrapView_.Clear();
        }

        Rect32 GetAdapterItemsReletiveRect();
        void MoveAdapterItemsRelativeRect(int16_t x, int16_t y);
        void MeasureAdapterRelativeRect();

    private:
        friend class UIBesList;
        void FillActiveView();

        List<UIView*> scrapView_;
        AbstractAdapter* adapter_;
        UIBesList* listView_;
        Rect32 adapterRelativeRect_;
        bool hasInitialiszed_;
    };

    void PushBack(UIView* view);
    void PopItem(UIView* view);
    void PushFront(UIView* view);
    void SetHead(UIView* view);
    bool MoveChildStep(int16_t distance);
    bool MoveChildStepVertical(int16_t distance);
    bool MoveChildStepHorizontal(int16_t distance);
    uint16_t GetIndexInc(uint16_t index);
    uint16_t GetIndexDec(uint16_t index);

    bool MoveOffset(int16_t x, int16_t y);
    bool IsNeedReCalculateDragEnd();
    bool ReCalculateDragEnd();
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void FixDistance(int16_t& distanceX, int16_t& distanceY) override;
    void FixHorDistance(int16_t& distanceX);
    void FixVerDistance(int16_t& distanceY);
    bool isLoopList_;
    bool isReCalculateDragEnd_;
    bool autoAlign_;
    uint16_t alignTime_;
    uint16_t startIndex_;
    uint16_t topIndex_;
    uint16_t bottomIndex_;
    uint16_t selectPosition_;
    int16_t onSelectedIndex_;
    Recycle recycle_;
    BesListScrollListener* scrollListener_;

    /** @brief 是否自动隐藏滚动条。*/
    bool scrollBarAutoHide_;

    /** @brief 记录滚动条的真实启用状态 */
    bool scrollBarEnableMark_;

    /** @brief X 轴方向滚动状态控制量，默认允许正向滚动 */
    DragState xDragState_ = { {false, true, true} };
    /** @brief Y 轴方向状态控制量，默认允许正向滚动 */
    DragState yDragState_ = { {false, true, true} };

    bool isFirstDraw_ = true;

    /** @brief 标记是否处理 drag 事件 */
    bool processDragEvent_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_BES_LIST_H