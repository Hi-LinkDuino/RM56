/**
 * @file duckweed_manager.cpp
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-24
 * @brief duckweed 消息管理器实现
 * @since 1.0
 * @version 1.0
 */

#if (BES_FEATURE_DUCKWEED == 1)
#include "components/bestechnic/duckweed_manager.h"

#include "common/task_manager.h"
#include "gfx_utils/graphic_log.h"

namespace {
const uint16_t TASK_LIST_MAX = 10;
} //namespace

namespace OHOS {
DuckweedManager* DuckweedManager::GetInstance()
{
    static DuckweedManager instance;
    return &instance;
}

void DuckweedManager::Callback()
{
    ListNode<DuckweedTask*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == nullptr) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "Remove nullptr duckweed task=%p", pos->data_);
            pos = list_.Remove(pos);
            continue;
        }
        if (pos->data_->IsDeath()) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "Delete death duckweed task=%p", pos->data_);
            delete pos->data_;
            pos->data_ = nullptr;
            pos = list_.Remove(pos);
            continue;
        }

        pos = pos->next_;
    }

    //只处理队列头的任务
    if (list_.Size() > 0 && !list_.IsEmpty()) {
        ListNode<DuckweedTask*>* task = list_.Begin();
        if (task != nullptr) {
            task->data_->Callback();
        }
    }
}

void DuckweedManager::Init()
{
    Task::Init();
}

void DuckweedManager::Add(DuckweedTask* task)
{
    if (task == nullptr) return;

    //队列限制 10 个任务
    if (list_.Size() >= TASK_LIST_MAX) {
        GRAPHIC_LOGE("Error! The Duckweed limit in the same time is %{public}d.", TASK_LIST_MAX);
        return;
    }

    ListNode<DuckweedTask*>* pos = list_.Begin();
    while (pos != list_.End()) {
        if (pos->data_ == task) {
            GRAPHIC_LOGW("Do not add task multi times");
            return;
        }
        pos = pos->next_;
    }

    list_.PushBack(task);
}

void DuckweedManager::Remove(const DuckweedTask* task)
{
    Remove(task->GetId());
}

void DuckweedManager::Remove(uint32_t id)
{
    ListNode<DuckweedTask*>* pos = list_.Begin();
    DuckweedTask* task = nullptr;
    while (pos != list_.End()) {
        task = pos->data_;
        if (task == nullptr) {
            pos = list_.Remove(pos);
            continue;
        }

        if (task->GetId() == id) {
            task->SetDeath();
            return;
        }
        pos = pos->next_;
    }
}

void DuckweedManager::Clear()
{
    ListNode<DuckweedTask*>* pos = list_.Begin();
    while (pos != list_.End()) {
        pos->data_->SetDeath();
        pos = pos->next_;
    }
}

DuckweedTask* DuckweedManager::GetCurDuckweedTask()
{
    ListNode<DuckweedTask*>* pos = list_.Begin();
    return pos->data_;
}

} // namespace OHOS
#endif //#BES_FEATURE_DUCKWEED
