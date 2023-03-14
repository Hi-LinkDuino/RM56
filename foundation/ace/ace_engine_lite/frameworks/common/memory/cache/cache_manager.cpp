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

#include "cache_manager.h"

#include "ace_log.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
/**
 * This table will decide the cache usage distribution. If one module want to use cache functionality,
 * it must config here first. The cache manager will assign buffer according to the actual whole
 * buffer rang, if the buffer supplement is rich, every cache user might be assigned one buffer
 * rang bigger than its lowest requirement.
 *
 * NOTE: the user must not be duplicated in this list
 */
const CacheUnit CacheManager::DEFAULT_CONFIG_TABLE[] = {
    {USER_LOCALICATION, 16}, // localication key-value cache, lowest 16KB
};

CacheManager::CacheManager() : cacheState_(STATE_NORMAL)
{
    configTable_ = DEFAULT_CONFIG_TABLE;
    configTableLen_ = sizeof(DEFAULT_CONFIG_TABLE) / sizeof(CacheUnit);
    ResetDistributedInfo();
}

CacheManager &CacheManager::GetInstance()
{
    static CacheManager cacheManager;
    return cacheManager;
}

void CacheManager::ResetDistributedInfo()
{
    wholeCacheMemInfo_.cacheStartAddr = 0;
    wholeCacheMemInfo_.cacheLength = 0;
    auto ret = memset_s(cacheUnitInfo_, sizeof(CacheMemInfo) * USER_MAX_COUNT,
        0, sizeof(CacheMemInfo) * USER_MAX_COUNT);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "reset cache info failed");
    }
}

void CacheManager::SetConfigTable(const CacheUnit *table, size_t count)
{
    if (cacheState_ == STATE_SUCCESS) {
        // reset
        cacheState_ = STATE_NORMAL;
        ResetDistributedInfo();
    }
    configTable_ = table;
    configTableLen_ = count;
}

void CacheManager::SetupCacheMemInfo(uintptr_t startAddr, size_t length)
{
    // only can be setup once
    if (cacheState_ == STATE_SUCCESS) {
        return;
    }
    // try to distribute the whole buffer, and if the buffer length can not meet the lowest requirement, just give up
    cacheState_ = DistributeCacheRange(startAddr, length);
    if (cacheState_ != STATE_SUCCESS) {
        return;
    }
    wholeCacheMemInfo_.cacheStartAddr = startAddr;
    wholeCacheMemInfo_.cacheLength = length;
}

CacheSetupState CacheManager::PrecheckStatus(uintptr_t startAddr, size_t totalBytes) const
{
    // check basic info
    if (configTableLen_ == 0 || configTable_ == nullptr) {
        // no user, return normal as no one cares
        return STATE_NORMAL;
    }
    if (configTableLen_ > CACHE_USER_MAX_COUNT) {
        HILOG_ERROR(HILOG_MODULE_ACE, "too many user!");
        return STATE_FAILURE;
    }
    if (startAddr == 0 || totalBytes == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error cache buffer rang!");
        return STATE_FAILURE;
    }

    // calculate the lowest requirement
    size_t lowestRequiredKBs = 0;
    size_t index = 0;
    while (index < configTableLen_) {
        lowestRequiredKBs = lowestRequiredKBs + configTable_[index].minLength_;
        index++;
    }
    if (lowestRequiredKBs > CACHE_REQUIREMENT_MAX_KBS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the requirement KBs is too much!");
        return STATE_FAILURE;
    }

    constexpr uint16_t bytesOneKB = 1024;
    // convert the required KBs to bytes
    size_t totalRequiredBytes = lowestRequiredKBs * bytesOneKB;
    // consider the magic number length for each unit
    totalRequiredBytes += (configTableLen_ * MAGIC_NUMBER_TOTAL_LENGTH_FOR_EACH);
    if (totalRequiredBytes > totalBytes) {
        // the given buffer is less than the lowest requirement
        HILOG_ERROR(HILOG_MODULE_ACE, "the cache buffer length can not meet the lowest requirement!");
        return STATE_FAILURE;
    }
    return STATE_SUCCESS;
}

CacheSetupState CacheManager::DistributeCacheRange(uintptr_t startAddr, size_t totalBytes)
{
    CacheSetupState setupResult = PrecheckStatus(startAddr, totalBytes);
    if (setupResult != STATE_SUCCESS) {
        return setupResult;
    }

    size_t index = 0;
    constexpr uint16_t bytesOneKB = 1024;
    size_t offset = 0;
    while (index < configTableLen_) {
        size_t requestBytes = (configTable_[index].minLength_ * bytesOneKB) + MAGIC_NUMBER_TOTAL_LENGTH_FOR_EACH;
        uintptr_t startPos = startAddr + offset;
        cacheUnitInfo_[configTable_[index].cacheUser_].cacheStartAddr = startPos;
        cacheUnitInfo_[configTable_[index].cacheUser_].cacheLength = requestBytes;
        // set magic number
        *(reinterpret_cast<uint32_t *>(startPos)) = CACHE_MEM_MAGIC_NUMBER;
        *(reinterpret_cast<uint32_t *>(startPos + requestBytes - MAGIC_NUMBER_LENGTH)) = CACHE_MEM_MAGIC_NUMBER;
        offset = offset + requestBytes;
        index++;
    }
    return STATE_SUCCESS;
}

bool CacheManager::IsEnvReady(CacheUser user) const
{
    if (cacheState_ != STATE_SUCCESS) {
        return false;
    }
    return ((user < USER_MAX_COUNT) && (cacheUnitInfo_[user].cacheStartAddr != 0));
}

uintptr_t CacheManager::GetCacheBufAddress(CacheUser user) const
{
    if (!IsEnvReady(user)) {
        return 0;
    }
    return cacheUnitInfo_[user].cacheStartAddr + MAGIC_NUMBER_LENGTH;
}

size_t CacheManager::GetCacheBufLength(CacheUser user) const
{
    if (!IsEnvReady(user)) {
        return 0;
    }
    return cacheUnitInfo_[user].cacheLength - MAGIC_NUMBER_TOTAL_LENGTH_FOR_EACH;
}

bool CacheManager::IsCacheOverflow(CacheUser user) const
{
    if (!IsEnvReady(user)) {
        return false;
    }
    // get head and tail magic number
    uint32_t *bufferHead = reinterpret_cast<uint32_t *>(cacheUnitInfo_[user].cacheStartAddr);
    uint32_t headMagicNumber = reinterpret_cast<uint32_t>(*(bufferHead));
    uint32_t *bufferTail = reinterpret_cast<uint32_t *>(cacheUnitInfo_[user].cacheStartAddr +
                                                        cacheUnitInfo_[user].cacheLength - MAGIC_NUMBER_LENGTH);
    uint32_t tailMagicNumber = reinterpret_cast<uint32_t>(*(bufferTail));
    // if the head magic number was over write, it means someone else overflow this area
    if (headMagicNumber != CACHE_MEM_MAGIC_NUMBER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the cache buffer[%{public}d] was overflown by someone else?", user);
        return true;
    }
    // if the tail magic number was over write, it means the user itself overflow this area
    if (tailMagicNumber != CACHE_MEM_MAGIC_NUMBER) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the cache buffer[%{public}d] was overflown by self?", user);
        return true;
    }
    return false;
}

bool CacheManager::IsWholeCacheHealthy() const
{
    if (cacheState_ != STATE_SUCCESS) {
        return false;
    }
    for (int i = 0; i < USER_MAX_COUNT; i++) {
        if (IsCacheOverflow(CacheUser(i))) {
            return false;
        }
    }
    return true;
}
} // namespace ACELite
} // namespace OHOS
