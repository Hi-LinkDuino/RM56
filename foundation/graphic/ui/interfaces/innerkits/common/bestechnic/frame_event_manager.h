/**
 * @file frame_event_manager.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-03-21
 * @brief UI 帧管理器
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_FRAME_EVENT_MANAGER_H
#define GRAPHIC_LITE_FRAME_EVENT_MANAGER_H

#include "gfx_utils/list.h"

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#endif

namespace OHOS {
/**
 * @author yongxianglai@bestechnic.com
 * @brief 绘制事件监听器
 */
class OnFrameListener {
public:
    OnFrameListener() = default;
    ~OnFrameListener() = default;

    /**
     * @brief UI 帧开始前，回调一次此函数
     */
    virtual void OnFrameStart() {}

    /**
     * @brief UI 帧处理完成后，回调一次此函数
     * @param duration 一帧消耗的时间，单位为毫秒
     */
    virtual void OnFrameEnd(uint32_t duration) {}
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief UI 帧事件管理类
 */
class FrameEventManager final {
public:
    static FrameEventManager& GetInstance();

    /**
     * @brief 注册绘制事件监听器
     * @param listener 
     */
    void RegisterFrameListener(OnFrameListener* listener);

    /**
     * @brief 取消注册绘制监听器
     * @param listener 
     */
    void UnregisterFrameListener(OnFrameListener* listener);

    /** @brief UI 的一帧开始前，分发通知 */
    void DispatchFrameStart();

    /** 
     * @brief UI 的一帧完成后，分发通知
     * @param duration 一帧耗时，单位为毫秒
     */
    void DispatchFrameEnd(uint32_t duration);

private:
    FrameEventManager();

    virtual ~FrameEventManager();

    FrameEventManager(const FrameEventManager&) = delete;
    FrameEventManager& operator=(const FrameEventManager&) = delete;
    FrameEventManager(FrameEventManager&&) = delete;
    FrameEventManager& operator=(FrameEventManager&&) = delete;

    /** 列表的遍历会面临多线程修改问题，必须做同步 */
    List<OnFrameListener*> list_;

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_t lock_;
#endif
};

} //namespace OHOS
#endif //GRAPHIC_LITE_FRAME_EVENT_MANAGER_H