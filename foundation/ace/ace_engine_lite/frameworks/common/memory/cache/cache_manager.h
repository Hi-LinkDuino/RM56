/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_CACHE_MANAGER_H
#define OHOS_ACELITE_CACHE_MANAGER_H

#include "cache_config.h"

#include <cstddef>
#include <cstdint>
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
/**
 * The structure used for recording the cache memory detail information.
 */
struct CacheMemInfo {
    // the cache memory start address, including the magic number at beginning
    uintptr_t cacheStartAddr;
    // count in bytes, and the max capability is 4M, including the maginc number at the beginning and ending
    size_t cacheLength;
};

enum CacheSetupState : uint8_t {
    STATE_NORMAL, // the cache is not initialized ever, or no user is configured
    STATE_SUCCESS, // the cache was initialized and distributed to all users successfully
    STATE_FAILURE, // the cache was tried be initialized but failed to distributed to all users
};

/**
 * This is one singleton instance for cache memory management. It can be used
 * out of the applications' lifecycle. It records the memory area for all kinds
 * of caching purpose in the frameworks. If the cache memory rang is not setup,
 * the cache functionality will not work.
 */
class CacheManager final {
public:
    static CacheManager &GetInstance();
    void SetupCacheMemInfo(uintptr_t startAddr, size_t length);
    // return the start addrss of the assigned buffer the unit can use, which doesn't includ the starting magic number
    uintptr_t GetCacheBufAddress(CacheUser user) const;
    // return the total buffer length the unit can use, which doesn't include the starting and ending magic numbers
    size_t GetCacheBufLength(CacheUser user) const;
    bool IsCacheAvailable(CacheUser user) const;
    bool IsCacheOverflow(CacheUser user) const;
    bool IsWholeCacheHealthy() const;
    /**
     * @brief SetConfigTable give the config table from outside, call this function carefully
     * @param table the config table
     * @param count the actual user count
     */
    void SetConfigTable(const CacheUnit *table, size_t count);

private:
    ACE_DISALLOW_COPY_AND_MOVE(CacheManager);
    CacheManager();
    ~CacheManager() = default;
    /**
     * @brief ResetDistributedInfo clean up all assigned buffer info
     */
    void ResetDistributedInfo();
    /**
     * @brief DistributeCacheRange called once to assign proper memory rang to all the user in the config table
     * @param startAddr the entire cache buffer start address
     * @param totalBytes the entire cache length
     * @return the result of the assignment process, if the memory size can not meet the  lowest request, the process
     * will return false as failure
     */
    CacheSetupState DistributeCacheRange(uintptr_t startAddr, size_t totalBytes);
    CacheSetupState PrecheckStatus(uintptr_t startAddr, size_t length) const;
    bool IsEnvReady(CacheUser user) const;
    CacheMemInfo wholeCacheMemInfo_;
    CacheMemInfo cacheUnitInfo_[USER_MAX_COUNT]; // count in bytes
    const CacheUnit *configTable_;
    size_t configTableLen_;
    CacheSetupState cacheState_;
    static const CacheUnit DEFAULT_CONFIG_TABLE[];
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_CACHE_MANAGER_H
