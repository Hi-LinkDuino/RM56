/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_COMPATIBLE_CHECK_H
#define HISTREAMER_PIPELINE_CORE_COMPATIBLE_CHECK_H

#include "type_define.h"
#include "plugin/common/plugin_caps.h"
#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
/**
 * merge otherCap keys with originCap keys:
 * 1. if key exists in only one Cap (e.g. only exists in originCap or otherCap), then put key into resCap directly
 * 2. if key exists in both originCap and otherCap, then intersections will be calculated
 * 2.1 if intersections of any key is empty, return false
 * 2.2 otherwise, put intersections into resCap
 * The mime of resCap is not set.
 *
 * @param originCap originCap
 * @param otherCap otherCap
 * @param resCap out parameter. The merge result is in it.
 * @return success
 */
bool MergeCapabilityKeys(const Capability& originCap, const Capability& otherCap, Capability& resCap);

/**
 * merge otherCap with originCap. The rule of merging keys is the same as MergeCapabilityKeys. Besides, it requires that
 * mime in originCap should be subset of mime in otherCap. The mime of resCap is set as originCap.mime.
 *
 * @param originCap originCap
 * @param otherCap otherCap
 * @param resCap out parameter. The merge result is in it.
 * @return success
 */
bool MergeCapability(const Capability& originCap, const Capability& otherCap, Capability& resCap);


/**
 * change meta info capability
 *
 * @param meta target meta
 * @return result capability
 */
std::shared_ptr<Capability> MetaToCapability(const Plugin::Meta& meta);

/**
 * merge meta with capability. This function firstly change meta into capability metaCap. The mime of metaCap is the
 * the same as cap. Then, merge metaCap and cap.
 * After that, output the resMeta according merge result and origin meta, as the following rules:
 * 1. if one meta key only exist in origin meta, put it into resMeta
 * 2. if one meta key exist in both origin meta and capability merge results, then translate the one in capability merge
 * results into fixed values
 *
 * @param meta origin meta
 * @param cap origin cap
 * @param resMeta result meta
 * @return success to merge
 */
bool MergeMetaWithCapability(const Plugin::Meta& meta, const Capability& cap, Plugin::Meta& resMeta);

bool ApplyCapabilitySet(const Capability& originCap, const CapabilitySet& capabilitySet, Capability& resCap);
}
}
}
#endif // HISTREAMER_PIPELINE_CORE_COMPATIBLE_CHECK_H
