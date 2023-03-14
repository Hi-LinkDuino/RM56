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

#include "plugin_meta.h"

#include <cinttypes>
#include <cstring>
#include <memory>
#include <vector>

#include "plugin/common/plugin_audio_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
using PointerPair = std::pair<std::shared_ptr<uint8_t>, size_t>;
Meta::~Meta()
{
    Clear();
}

bool Meta::Empty() const
{
    return items_.empty();
}

bool Meta::SetString(Plugin::MetaID id, const std::string& value)
{
    return SetData<std::string>(id, value);
}

bool Meta::SetInt32(Plugin::MetaID id, int32_t value)
{
    return SetData<int32_t>(id, value);
}

bool Meta::SetUint32(Plugin::MetaID id, uint32_t value)
{
    return SetData<uint32_t>(id, value);
}

bool Meta::SetInt64(Plugin::MetaID id, int64_t value)
{
    return SetData<int64_t>(id, value);
}

bool Meta::SetUint64(Plugin::MetaID id, uint64_t value)
{
    return SetData<uint64_t>(id, value);
}

bool Meta::SetFloat(Plugin::MetaID id, float value)
{
    return SetData<float>(id, value);
}

bool Meta::GetString(Plugin::MetaID id, std::string& value) const
{
    auto ite = items_.find(id);
    if (ite == items_.end()) {
        return false;
    }
    if (ite->second.SameTypeWith(typeid(const char*))) {
        value = Plugin::AnyCast<const char*>(ite->second);
    } else if (ite->second.SameTypeWith(typeid(std::string))) {
        value = Plugin::AnyCast<std::string>(ite->second);
    } else if (ite->second.SameTypeWith(typeid(char*))) {
        value = Plugin::AnyCast<char*>(ite->second);
    } else {
        return false;
    }
    return true;
}

bool Meta::GetInt32(Plugin::MetaID id, int32_t& value) const
{
    return GetData<int32_t>(id, value);
}

bool Meta::GetUint32(Plugin::MetaID id, uint32_t& value) const
{
    return GetData<uint32_t>(id, value);
}

bool Meta::GetInt64(Plugin::MetaID id, int64_t& value) const
{
    return GetData<int64_t>(id, value);
}

bool Meta::GetUint64(Plugin::MetaID id, uint64_t& value) const
{
    return GetData<uint64_t>(id, value);
}

bool Meta::GetFloat(Plugin::MetaID id, float& value) const
{
    return GetData<float>(id, value);
}

void Meta::Clear()
{
    items_.clear();
}

bool Meta::Remove(Plugin::MetaID id)
{
    auto ite = items_.find(id);
    if (ite == items_.end()) {
        return false;
    }
    items_.erase(ite);
    return true;
}

void Meta::Update(const Meta& meta)
{
    for (auto& ptr : meta.items_) {
        // we need to copy memory for pointers
        if (ptr.second.SameTypeWith(typeid(PointerPair))) {
            auto pointerPair = Plugin::AnyCast<PointerPair>(ptr.second);
            SetPointer(ptr.first, pointerPair.first.get(), pointerPair.second);
        } else {
            items_[ptr.first] = ptr.second;
        }
    }
}

bool Meta::SetPointer(Plugin::MetaID id, const void* ptr, size_t size) // NOLINT:void*
{
    if (size == 0) {
        return false;
    }
    auto tmp = new (std::nothrow) uint8_t[size];
    if (tmp == nullptr) {
        return false;
    }
    std::shared_ptr<uint8_t> savePtr(tmp, std::default_delete<uint8_t[]>());
    if (memcpy_s(savePtr.get(), size, ptr, size) != EOK) {
        delete[] tmp;
        return false;
    }
    return SetData<std::pair<std::shared_ptr<uint8_t>, size_t>>(id, std::make_pair(savePtr, size));
}

bool Meta::GetPointer(Plugin::MetaID id, void** ptr, size_t& size) const // NOLINT: void*
{
    std::pair<std::shared_ptr<uint8_t>, size_t> item;
    if (GetData<std::pair<std::shared_ptr<uint8_t>, size_t>>(id, item)) {
        size = item.second;
        if (size == 0) {
            return false;
        }
        auto tmp = new (std::nothrow) uint8_t[size];
        if (tmp == nullptr) {
            return false;
        }
        if (memcpy_s(tmp, size, item.first.get(), size) != EOK) {
            delete[] tmp;
            return false;
        }
        *ptr = tmp;
        return true;
    } else {
        return false;
    }
}

std::vector<MetaID> Meta::GetMetaIDs() const
{
    std::vector<MetaID> ret (items_.size());
    int cnt = 0;
    for (const auto& tmp : items_) {
        ret[cnt++] = tmp.first;
    }
    return ret;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS
