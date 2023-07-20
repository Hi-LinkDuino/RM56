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

#ifndef GRAPHIC_LITE_CACHE_MANAGER_H
#define GRAPHIC_LITE_CACHE_MANAGER_H

#include "file_img_decoder.h"

#include "graphic_config.h"

#if (BES_IMG_PRELOAD_ENABLE == 1)
#include "common/bestechnic/frame_event_manager.h"
#include "hal_tick.h"

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
#include <pthread.h>
#endif

namespace {
//预加载清单文件所在目录
#define PRELOAD_ROOT_DIR FS_ROOT BES_IMG_PRELOAD_DIR "/"
//开机预加载清单文件绝对路径
#define PRELOAD_BOOT_PATH PRELOAD_ROOT_DIR BES_IMG_PRELOAD_MANIFEST_BOOT
//闲时预加载清单文件绝对路径
#define PRELOAD_IDLE_PATH PRELOAD_ROOT_DIR BES_IMG_PRELOAD_MANIFEST_IDLE
//IO 性能评估记录文件路径
#define PRELOAD_EVALUATE_PATH PRELOAD_ROOT_DIR "bes_preload_evaluate"

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
/** @brief 预加载任务中的绘制锁 */
static pthread_mutex_t drawLock_;
/** @brief 预加载任务中的 IO 锁 */
static pthread_mutex_t preloadLock_;
#endif

/** @brief 一个文件可以被重试读取的最大次数 */
constexpr const uint8_t READ_RETRY_COUNT_MAX = 3;
/** @brief 一次文件读取的最小字节数量，单位为 byte */
constexpr const uint32_t IO_READ_BYTES_MIN = 1024;
/** @brief 一次文件读取的最大字节数量，单位为 byte */
constexpr const uint32_t IO_READ_BYTES_MAX = 128 * 1024;
/** @brief 返回码，表示目标不属于预加载资源 */
constexpr const int16_t CODE_NOT_PRELOAD = -1;
/** @brief 闲时预加载的默认连续时长 */
constexpr const uint16_t DEF_PRELOAD_TIME_MS = 30;
/** @brief UI 绘制一次的基准时长（30fps），单位为毫秒 */
constexpr const uint8_t DRAW_FRAME_TIME_MS = 30;
/** @brief 图片资源文件要求的格式后缀 */
constexpr const char PRELOAD_SRC_SUFFIX[4] = {'.', 'b', 'i', 'n'};
/** @brief IO 性能评估列表大小 */
constexpr const uint8_t EVALUATE_IO_COUNT = 20;
} //namespace
#endif //BES_IMG_PRELOAD_ENABLE

namespace OHOS {
#if (BES_IMG_PRELOAD_ENABLE == 1)
/**
 * @author yongxianglai@bestechnic.com
 * @brief 预加载结果
 */
enum class PreloadResult {
    /** @brief 文件不存在读取失败 */
    IO_ERROR,
    /** @brief 被意外终止 */
    TERMINAL,
    /** @brief 正常结束 */
    ENDED
};

/**
 * @anchor yongxianglai@bestechnic.com
 * @brief 预加载列表项
 */
struct PreloadEntry final {
    PreloadEntry(char* path): path(path),
        data(nullptr), 
        seq(CODE_NOT_PRELOAD), 
        loadBytes(0), totalBytes(0), 
        retryCount(READ_RETRY_COUNT_MAX) {}

    virtual ~PreloadEntry() {
        UIFree(path);
        path = nullptr;

        ClearData();
    }

    void ClearData()
    {
        UIFree(data);
        data = nullptr;
        loadBytes = 0;
    }

    /** @brief 预加载资源的绝对路径 */
    char* path;

    /** @brief 临时数据，拼接用途 */
    uint8_t* data;

    /** @brief 资源在缓存池（entryAtr_）中的位置。如果为负数，说明该资源还未完成预加载 */
    int16_t seq;

    /** @brief 已经读取的数据大小，单位为 byte */
    uint32_t loadBytes;

    /** 总数据大小， 单位为 byte */
    uint32_t totalBytes;

    /** @brief 剩余重试次数。当次数变成0，认定此资源无效 */
    int16_t retryCount;
};
#endif //BES_IMG_PRELOAD_ENABLE

class CacheEntry : public HeapBase {
public:
    CacheEntry() : dsc_{0}, life_(0) {}

    ~CacheEntry() {}

    ImageSrcType GetImgSrcType() const
    {
        return dsc_.srcType;
    }

    const uint8_t* GetImgData() const;

    ImageHeader GetImgHeader() const
    {
        return dsc_.imgInfo.header;
    }

    ImageInfo GetImageInfo() const
    {
        return dsc_.imgInfo;
    }

    RetCode ReadLine(const Point& start, int16_t bufSize, uint8_t* buf);

    bool InCache() const
    {
        return dsc_.inCache_;
    }

private:
    static constexpr uint16_t MAX_SRC_LENGTH = 4096;

    friend class CacheManager;

    void Clear();
    void ClearSrc();
    RetCode SetSrc(const char* path);

    FileImgDecoder::ImgResDsc dsc_;

#if (BES_IMG_PRELOAD_ENABLE == 1)
    /** @brief 缓存的存活年龄标记（毫秒值） */
    uint32_t life_;

    /** @brief 是否属于持久缓存。如果为true，表示此项永远不会被覆盖或清理 */
    bool persistent_ = false;
#else
    int32_t life_;
#endif //BES_IMG_PRELOAD_ENABLE
};

class CacheManager : public HeapBase {
public:
    static CacheManager& GetInstance();

    RetCode Init(uint16_t size);

    uint16_t GetSize()
    {
        return size_;
    }

    RetCode Open(const char* path, const Style& style, CacheEntry& entry);

    RetCode Close(const char* path);

    RetCode Reset();

    RetCode ReadToCache(CacheEntry& entry);

    bool GetImageHeader(const char* path, ImageHeader& header);

#if (BES_IMG_PRELOAD_ENABLE == 1)
    /**
     * @brief 在 UI 线程启动之前，完成指定清单的图片预加载。这会在一定程度上导致 UI 线程的启动时机被延后
     * @return 加载结果
     */
    PreloadResult PreloadBeforeUiLaunch();

    /** @brief 图片资源的（UI 绘制）空闲时预加载 */
    void PreloadWhenIdle();
#endif //BES_IMG_PRELOAD_ENABLE

private:
#if (BES_IMG_PRELOAD_ENABLE == 1)
    /** @brief ui绘制空闲状态监听器 */
    class UiIdleListener final : public OnFrameListener {
    private:
        friend class CacheManager;
        UiIdleListener(CacheManager* cacheManager): cm_(cacheManager) {}
        virtual ~UiIdleListener() = default;
        virtual void OnFrameStart() override;
        virtual void OnFrameEnd(uint32_t duration) override;

        CacheManager* cm_ = nullptr;
    };

    friend class CacheManager::UiIdleListener;

    CacheManager();

    virtual ~CacheManager();

    /**
     * @brief 在指定的时间内读取数据，数据量大小由 {@link #Evaluate()} 统计得出。
     *           当目标时长大于统计列表的最大时长，则认为是读取时长无限大（整个目标读取完成或读取失败）
     * 
     * @param target 目标预加载信息
     * @param duration 指定的时长，单位为毫秒
     * @return 读取结果
     */
    PreloadResult PreloadInTime(PreloadEntry *target, int32_t duration);

    /**
     * @brief 预加载指定本地路径的资源到缓存
     * 
     * @param path 本地资源的绝对路径
     * @param cacheIndex 在缓存池中的目标位置
     * @return 完成预加载的资源在缓存池中的索引，如果为 {@link #CODE_NOT_PRELOAD} 则表示预加载失败
     * @see CacheManager#LoadPreloadRes
     */
    int16_t DoPreloadFully(const char* path, uint16_t cacheIndex, const Style& style);

    /**
     * @brief 将预加载清单文件中的资源路径记录下来
     * 
     * @param list 目标列表
     * @param path 资源绝对路径
     * @param len 路径长度
     * @see CacheManager#SavePreloadResPath
     * @return 记录在列表中的数据对象
     */
    PreloadEntry* DoSavePreloadPath(List<PreloadEntry*>& list, const char* path, uint16_t len);

    /**
     * @brief 修正资源文件格式后缀，支持将如 .xxx 等格式后缀修改为 .bin
     * @param path 目标资源文件的绝对路径
     * @param startPos 检查的起始位置
     * @param len 路径总长度（不包括结尾符 '\0'）
     * @return 新的路径，如果返回 nullptr 意味着目标路径处理失败或不需要修正
     */
    char* FixSrcSuffix(const char* path, uint16_t startPos, uint16_t& len);

    /**
     * @brief 处理从预加载清单文件中读取到的一条路径
     * 
     * @param path 
     * @param pathLen 
     * @param onRead 
     */
    void OnPathRead(const char* path, uint16_t pathLen, void (*onRead)(const char* path, uint16_t len));

    /**
     * @brief 解析指定的预加载清单文件
     * 
     * @param filePath 目标清单文件绝对路径
     * @param onRead 处理函数，函数参数 1 为从清单中解析得到的一个资源绝对路径，参数 2 为 路径长度
     * @return 处理结果
     */
    PreloadResult ParsePreloadManifest(const char* filePath, void (*onRead)(const char* path, uint16_t len));

    /**
     * @brief 对于预加载资源，在 UI 线程中进行直接同步加载
     * 
     * @param path 预加载目标绝对路径
     * @param style 
     * @param entry 
     * @return 如果返回 {@link PreloadResult#ENDED}，表示为预加载资源的处理任务，并不再需要外部继续处理，否则交由外部处理
     */
    PreloadResult MaybePreloadSync(const char* path, const Style& style, CacheEntry& entry);

    /**
     * @brief 通过 IO 评估结果 {@link #bytesInTime} 获得最合适的读取大小
     * 
     * @param duration 目标读取时长，单位为毫秒
     * @return uint32_t 目标读取数据大小，单位为 byte
     */
    uint32_t GetBestReadSizeByEvaluated(uint32_t duration);

    /** @brief 评估读取本地文件的速度，目标文件的大小，以及缓存池位置分配等 */
    void Evaluate();

    /** @brief 尝试从本地获取 IO 性能评估数据 */
    bool GetEvaluateFromHistory();

    /**
     * @brief 获取最大的可预加载数量
     * @return uint16_t 
     */
    uint16_t GetPreloadLimit();

    PreloadEntry* GetPreloadEntry(const char* path, const List<PreloadEntry*> *list);

    /**
     * @brief 检查是否为已被记录的资源
     * @param path 目标资源路径
     */
    bool IsDuplicatePreload(const char* path);

    /**
     * @brief 检查是否已经预加载完成
     * @param entry 
     */
    bool IsPreloaded(const PreloadEntry* entry);

    /**
     * @brief 清理预加载的相关资源和信息
     * @see CacheManager#Init
     */
    void ClearPreloadCache();

    /** @brief 由于某些原因（预加载无法继续进行或存储容量不足），裁剪预加载列表 */
    void PrunePreload(ListNode<PreloadEntry*>* lastNode);

    /** 
     * @brief 丢弃无法处理的预加载资源 
     * @param seq 预加载资源的序号
     */
    void DiscardPreloadRes(uint32_t seq);

    /** 
     * @brief 裁剪指定数量的非预加载缓存（按照年龄值升序排列，依次裁剪），以腾出内存继续加载资源
     * @param excludePath 例外资源路径，对应的缓存将不会被裁剪
     * @param num 需要裁剪的缓存个数
     */
    bool PruneCache(const char* excludePath, uint32_t num = 1);

    /**
     * @brief 加载资源到缓存
     * 
     * @param path 资源绝对路径
     * @param len 路径长度
     * @see CacheManager#PreLoadBeforeUiLaunch
     */
    static void LoadPreloadRes(const char* path, uint16_t len);

    /**
     * @brief 保存预加载资源绝对路径
     * 
     * @param path 资源绝对路径
     * @param len 路径长度
     * @see CacheManager#PreloadBeforeUiLaunch
     */
    static void SavePreloadResPath(const char* path, uint16_t len);

    /** @brief ui 绘制空闲状态监听器 */
    UiIdleListener* uiIdleListener_ = nullptr;

    /** @brief 开机预加载的资源路径列表 */
    List<PreloadEntry*> preloadBootList_;

    /** @brief 闲时预加载的资源路径列表 */
    List<PreloadEntry*> preloadIdleList_;

    /** 
     * 当前正在进行闲时预加载的节点
     * @see CacheManager#preloadIdleList_
     */
    PreloadEntry* curPreloadEntry_ = nullptr;

    /**
     * @brief 需要预加载资源的数量，包括 UI 线程启动前的预加载和空闲时的预加载。
     *           保持和 {@link CacheManager#preloadBootList_} 和 {@link CacheManager#preloadIdleList_} 的大小之和一致
     */
    uint16_t preloadCount_ = 0;

    /** @brief 标记是否允许进行开机预加载任务，任务只会被执行一次 */
    bool canBootPreload_ = true;

    /** @brief 是否允许进行空闲时预加载任务 */
    bool canIdlePreload_ = true;

    bool isUiDrawStart_ = false;

    /** @brief 记录 UI 线程 id */
    pthread_t uiThreadId_;

    /** @brief IO 性能评估结果列表，每一行的两个元素分别表示耗时（毫秒）和读取的字节数 */
    uint32_t bytesInTime[EVALUATE_IO_COUNT][2];

    static constexpr const uint32_t LIFE_LIMIT = UINT32_MAX;
#else
    CacheManager() : size_(0), entryArr_(nullptr) {}
    ~CacheManager() {}

    static constexpr uint8_t LIFE_GAIN_INTERVAL = 1;
    static constexpr uint16_t LIFE_LIMIT = 1000;
#endif //BES_IMG_PRELOAD_ENABLE

    void Clear(CacheEntry& entry);

    void AgingAll(int32_t time = AGING_INTERVAL);

    RetCode GetIndex(const char* src, uint16_t& hittedIndex);

    RetCode SelectEntryToReplace(uint16_t& selectedIndex);

    RetCode TryDecode(const char* path, const Style& style, CacheEntry& entry);

    uint16_t size_;
    CacheEntry* entryArr_;

    static constexpr uint8_t AGING_INTERVAL = 1;

    /** @brief 缓存池的数量上限 */
    static constexpr uint8_t MAX_CACHE_ENTRY_NUM = UINT8_MAX;
};
} // namespace OHOS

#endif //GRAPHIC_LITE_CACHE_MANAGER_H
