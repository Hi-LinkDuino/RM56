/**
 * @file bes_list_component.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-01-16
 * @brief 
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_ACELITE_BES_LIST_COMPONENT_H
#define OHOS_ACELITE_BES_LIST_COMPONENT_H

#include "component.h"
#include "list_adapter.h"
#include "bestechnic/ui_bes_list.h"

namespace OHOS {
namespace ACELite {

/**
 * @author yongxianglai@bestechnic.com
 * @date 2023年1月16日
 * @brief 对 ListComponent 的重构，用于修复基类存在的一些问题，或者新增一些新特性。禁止继承。
 * @version 1.0
 * 
 * @see ListComponent
*/
class BesListComponent : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BesListComponent);

    /** @brief 防止调用无参构造函数 */
    BesListComponent() = delete;

   /**
    * @brief Construct a new Bes List Component object
    * 
    * @param options 
    * @param children 
    * @param styleManager 
    */
    BesListComponent(JSValue options, JSValue children, AppStyleManager *styleManager);

   ~BesListComponent() override {}

protected:
    virtual bool CreateNativeViews() override;
    virtual void ReleaseNativeViews() override;
    virtual UIView *GetComponentRootView() const override;

    /**
     * @brief 解析自定义样式字段
     * 
     * @param style 
     * @return true 
     * @return false 
     */
    virtual bool ApplyPrivateStyle(const AppStyleItem* style) override;

    /**
     * @brief 注册自定义事件监听器
     * 
     * @param eventTypeId 
     * @param funcValue 
     * @param isStopPropagation 
     * @return true 
     * @return false 
     */
    virtual bool RegisterPrivateEventListener(uint16_t eventTypeId, JSValue funcValue, bool isStopPropagation) override;

    virtual bool ProcessChildren() override;
    virtual void OnViewAttached() override;
    virtual bool UpdateForView() override;

    /** @brief render 阶段完成时执行该函数 */
    virtual void PostRender() override;

private:
    static JSValue ListScrollTo(const JSValue func, const JSValue dom, const JSValue args[], const jerry_length_t size);

    /**
     * @brief 设置滚动条（状态，启用状态，样式等）。字段值为对象类型，包含以下字段：
     * @see FIELD_SCROLLBAR_ENABLE
     * @see FIELD_SCROLLBAR_AUTO_HIDE
     * @see FIELD_SCROLLBAR_SIDE
     * 
     * js 函数示例：
     * setScrollBar({
     *     {@link #FIELD_SCROLLBAR_ENABLE}: true,
     *     {@link #FIELD_SCROLLBAR_AUTO_HIDE}: false,
     *     {@link #FIELD_SCROLLBAR_SIDE}: right
     * })
     * 
     * @param func js 函数
     * @param dom
     * @param args js 函数参数列表
     * @param size 参数 args 的大小
     * @return JSValue
     */
    static JSValue SetScrollBar(const JSValue func, const JSValue dom,
                                const JSValue args[], const jerry_length_t size);

    /**
     * @brief 启用 / 禁用列表滚动
     * 
     * js 函数示例：
     * setListScroll({
     *   {@link #FIELD_ENABLE}: true
     * })
     * 
     * @param func 
     * @param dom 
     * @param args 
     * @param size 
     * @return JSValue
     */
    static JSValue SetListScroll(const JSValue func, const JSValue dom,
                                const JSValue args[], const jerry_length_t size);

    static const char * const FUNC_SCROLLTO;
    /** @brief js 函数名称 */
    static const char * const FUNC_SET_SCROLLBAR;
    /** @brief js 函数名称 */
    static const char * const FUNC_SET_LIST_SCROLL;

    static const char * const FIELD_INDEX;
    /** @brief 字段，启用状态，字段值为 bool 类型 */
    static const char * const FIELD_ENABLE;
    /** @brief 字段，是否自动隐藏，字段值为 bool 类型 */
    static const char * const FIELD_AUTO_HIDE;
    /** @brief 字段，位置，字段值为 VAL_SIDE_LEFT | VAL_SIDE_TOP | VAL_SIDE_RIGHT | VAL_SIDE_BOTTOM */
    static const char * const FIELD_SIDE;

    UIBesList list_;
    ListAdapter adapter_;
    BesListEventListener listEventListener_;
    bool hasSetAdaptor_ = false;
};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_BES_LIST_COMPONENT_H