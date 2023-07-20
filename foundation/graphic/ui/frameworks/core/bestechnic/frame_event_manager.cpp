/**
 * @file frame_event_manager.cpp
 * @author yongxianglai@bestechnic.com
 * @date 2023-03-21
 * @brief UI 帧事件管理实现
 * @since 1.0
 * @version 1.0
 */

#include "common/bestechnic/frame_event_manager.h"
#include "hal_tick.h"

namespace OHOS {
FrameEventManager::FrameEventManager()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_init(&lock_, nullptr);
#endif
}

FrameEventManager::~FrameEventManager()
{
    list_.Clear();

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
    pthread_mutex_destroy(&lock_);
#endif
}

FrameEventManager& FrameEventManager::GetInstance()
{
    static FrameEventManager instance;
    return instance;
}

void FrameEventManager::RegisterFrameListener(OnFrameListener* listener)
{
    if (listener == nullptr) return;

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    ListNode<OnFrameListener*>* node = list_.Begin();
    //不接受重复的元素
    while (node != nullptr && node != list_.End()) {
        if (node->data_ == nullptr) {
            node = list_.Remove(node);
            continue;
        }
        if (node->data_ == listener) return;
        node = node->next_;
    }
    list_.PushBack(listener);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void FrameEventManager::UnregisterFrameListener(OnFrameListener* listener)
{
    if (listener == nullptr) return;

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

     ListNode<OnFrameListener*>* node = list_.Begin();
    while (node != nullptr && node != list_.End()) {
        if (node->data_ == nullptr) {
            node = list_.Remove(node);
            continue;
        }
        if (node->data_ == listener) {
            list_.Remove(node);
            break;
        }
        node = node->next_;
    }

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void FrameEventManager::DispatchFrameStart()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    ListNode<OnFrameListener*>* node = list_.Begin();
    OnFrameListener* listener;
    while (node != nullptr && node != list_.End()) {
        listener = node->data_;
        if (listener == nullptr) {
            node = list_.Remove(node);
            continue;
        }

        listener->OnFrameStart();
        node = node->next_;
    }

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void FrameEventManager::DispatchFrameEnd(uint32_t duration)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    uint32_t time = HALTick::GetInstance().GetTime();
    ListNode<OnFrameListener*>* node = list_.Begin();
    OnFrameListener* listener;
    while (node != nullptr && node != list_.End()) {
        listener = node->data_;
        if (listener == nullptr) {
            node = list_.Remove(node);
            continue;
        }

        duration = duration + HALTick::GetInstance().GetTime() - time;
        time = HALTick::GetInstance().GetTime();

        listener->OnFrameEnd(duration);

        node = node->next_;
    }

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}
} // namespace OHOS