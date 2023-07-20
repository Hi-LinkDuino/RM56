/**
 * @file duckweed_scheduler.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-24
 * @brief duckweed 管理器
 * @since 1.0
 * @version 1.0
 */

#if (BES_FEATURE_DUCKWEED == 1)
#ifndef GRAPHIC_LITE_DUCKWEED_MANAGER_H
#define GRAPHIC_LITE_DUCKWEED_MANAGER_H

#include "common/task.h"
#include "gfx_utils/list.h"

namespace OHOS {

/** @brief 非法的任务 id */
constexpr const uint32_t INVALID_TASK_ID = 0;

 /**
  * @author yonagxianglai@bestechnic.com
  * @brief 定时任务实现
  */
class DuckweedTask {
public:
    DuckweedTask(uint32_t id, uint32_t duration)
        : id_(id), duration_(duration)
    {
    }

    virtual ~DuckweedTask() = default;

    /** @brief 任务的每一个刷新点到来，都将触发一次此函数 */
    virtual void Callback() {}

    uint32_t GetId() const
    {
        return id_;
    }

    uint32_t GetDuration() const
    {
        return duration_;
    }

    /** @brief 标记 task 处于死亡状态，在下一次刷新点将自动清理这些处于死亡状态的任务 */
    void SetDeath()
    {
        id_ = INVALID_TASK_ID;
    }

    bool IsDeath()
    {
        return id_ == INVALID_TASK_ID;
    }

protected:
    /** @brief 任务的唯一识别 id */
    uint32_t id_;

    /** @brief 任务执行时长，单位为毫秒 */
    uint32_t duration_;
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief duckweed 消息管理器
 */
class DuckweedManager : public Task {
public:
    /**
     * @brief 单例实现
     *
     * @return Returns the <b>AnimatorManager</b> instance.
     */
    static DuckweedManager* GetInstance();

    /**
     * @brief 任务回调函数。每一个任务执行的时间点到来，触发一次此函数
     */
    virtual void Callback() override;

    /**
     * @brief 初始化管理器
     */
    virtual void Init() override;

    /**
     * @brief 添加一个任务
     * 
     * @param task 目标任务
     */
    void Add(DuckweedTask* task);

    /**
     * @brief 移除指定的任务
     * 
     * @param id 目标任务的 id
     * 
     * @see Remove(const DuckweedTask*)
     */
    void Remove(uint32_t id);

    /**
     * @brief 移除指定的任务
     * 
     * @param task 目标任务
     */
    void Remove(const DuckweedTask* task);

    /** @brief 清除所有任务 */
    void Clear();

    /**
     * @brief 获取当前正在被执行的任务
     * 
     * @return DuckweedTask* 
     */
    DuckweedTask* GetCurDuckweedTask();

protected:
    List<DuckweedTask*> list_;
    DuckweedManager() {}
    virtual ~DuckweedManager() {}
    DuckweedManager(const DuckweedManager&) = delete;
    DuckweedManager& operator=(const DuckweedManager&) = delete;
    DuckweedManager(DuckweedManager&&) = delete;
    DuckweedManager& operator=(DuckweedManager&&) = delete;
}; // OHOS::DuckweedManager
} // namespace OHOS
#endif
#endif //BES_FEATURE_DUCKWEED
