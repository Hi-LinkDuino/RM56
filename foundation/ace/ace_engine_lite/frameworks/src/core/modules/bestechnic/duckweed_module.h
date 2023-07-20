/**
 * @file duckweed_module.h
 * @author yongxianglai@bestechnic.com
 * @date 2023-02-22
 * @brief 实现全局范围的浮动消息效果
 * @since 1.0
 * @version 1.0
 */

#if (BES_FEATURE_DUCKWEED == 1)
#ifndef OHOS_ACELITE_DUCKWEED_MODULE_H
#define OHOS_ACELITE_DUCKWEED_MODULE_H

#include "acelite_config.h"
#include "non_copyable.h"
#include "ace_log.h"
#include "presets/preset_module.h"

#include "components/bestechnic/ui_duckweed.h"
#include "components/bestechnic/duckweed_manager.h"

namespace OHOS {
namespace ACELite {

/**
 * @brief 
 * duckweed 消息功能模块
 */
class DuckweedModule final : PresetModule {
public:
    ACE_DISALLOW_COPY_AND_MOVE(DuckweedModule);

    static DuckweedModule *GetInstance()
    {
        static DuckweedModule instance;
        return &instance;
    }

    /**
     * @brief 请求显示一个 duckweed 消息。注意，如果当前消息队列中已经存在其他 duckweed 消息，则本次消息将排队等待；如果消息队列已经达到容量上限，则请求将会被拒绝
     * 
     * @param func
     * @param context
     * @param args
     * @param argsNum
     * @return duckweed 消息的唯一识别 id。当显示消息的要求无法完成（如消息队列容量达到上限），则返回 0
     */
    static jerry_value_t Show(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t *args,
                                const jerry_length_t argsNum);

    /**
     * @brief 清除指定的 duckweed 消息
     * @param func
     * @param context
     * @param args
     * @param argsNum
     * @return 未定义
     */
    static jerry_value_t Hide(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t *args,
                                const jerry_length_t argsNum);

    virtual void Init() override;

private:
    /**
     * @brief 解析 duckweed 样式参数
     * 
     * @param obj js 字段
     * @param option 样式配置
     * @param content 文本内容
     */
    static void ParseOption(jerry_value_t obj, DuckweedOption &option, SpannableString *content);

    DuckweedModule(): PresetModule(nullptr) {}

    virtual ~DuckweedModule() {};
};

/**
 * @author yongxianglai@bestechnic.com
 * @brief 代理注册 DuckweedModule
 */
class ProxyDuckweedModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ProxyDuckweedModule);
    ProxyDuckweedModule() = default;
    ~ProxyDuckweedModule() = default;

    /**
     * @brief 加载 Duckweed module
     */
    static void Load()
    {
        DuckweedModule *duckweedModule = const_cast<DuckweedModule *>(DuckweedModule::GetInstance());
        duckweedModule->Init();
    }
};

} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELite_DUCKWEED_MODULE_H
#endif // BES_FEATURE_DUCKWEED
