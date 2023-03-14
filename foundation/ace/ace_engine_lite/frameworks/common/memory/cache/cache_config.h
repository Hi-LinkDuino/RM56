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

#ifndef OHOS_ACELITE_CACHE_CONFIG_H
#define OHOS_ACELITE_CACHE_CONFIG_H

#include <cstdint>

namespace OHOS {
namespace ACELite {
/**
 * The magic number used at the beginning of every cache memory area.
 */
constexpr uint8_t MAGIC_NUMBER_COUNT = 2; // will put one magic number at the begin and end of every buffer for each
constexpr uint8_t MAGIC_NUMBER_LENGTH = 4; // count in bytes
constexpr uint8_t MAGIC_NUMBER_TOTAL_LENGTH_FOR_EACH = MAGIC_NUMBER_LENGTH * MAGIC_NUMBER_COUNT;
constexpr uint32_t CACHE_MEM_MAGIC_NUMBER = 0xCCCCCCCC;

constexpr uint8_t ALIGNMENT_SIZE = 8;

constexpr uint8_t CACHE_USER_MAX_COUNT = 16;

/**
 * Every cache user can not use more than the max length buffer.
 * Count in KB.
 */
constexpr uint16_t CACHE_REQUIREMENT_MAX_KBS = 4096; // the cache max total size, 4M
constexpr uint8_t CACHE_UNIT_MAX_KBS = 128; // max 128KB
constexpr uint32_t CACHE_UNIT_MAX_BYTES = CACHE_UNIT_MAX_KBS * 1024;

/**
 * Define the purpose of the specific cache memory range.
 */
enum CacheUser : uint8_t {
    USER_LOCALICATION,
    USER_PAGE_FILE,
    USER_MAX_COUNT
};

/**
 * Records the cache requester info.
 */
struct CacheUnit {
    // the cache unit owner
    CacheUser cacheUser_;
    // the required min buffer length, count in KB
    uint8_t minLength_;
    // constructors
    CacheUnit() : cacheUser_(USER_MAX_COUNT), minLength_(CACHE_UNIT_MAX_KBS) {}
    CacheUnit(CacheUser user, uint8_t length) : cacheUser_(user), minLength_(length) {}
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_CACHE_CONFIG_H
