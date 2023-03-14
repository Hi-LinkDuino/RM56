/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef PROFILE_LIST_H
#define PROFILE_LIST_H

#include <string>
#include <vector>

#include "base/base_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents profile list template.
 *
 * @since 6
 */
template<typename T>
class ProfilesList final {
public:
    /**
     * @brief A constructor used to create an <b>ProfilesList</b> instance.
     *
     * @since 6
     */
    ProfilesList() = default;
    /**
     * @brief A destructor used to delete the <b>ProfilesList</b> instance.
     *
     * @since 6
     */
    ~ProfilesList()
    {
        std::lock_guard<std::mutex> lock(lock_);
        profiles_.clear();
    }
    /**
     * @brief Get profile.
     *
     * @param transport Adapter transport.
     * @param name Profile name
     * @return Returns profile by adapter transport and profile name.
     * @since 6
     */
    T Get(const BTTransport transport, const std::string &name)
    {
        std::lock_guard<std::mutex> lock(lock_);
        return profiles_[transport][name];
    }
    /**
     * @brief Clear profile list.
     *
     * @since 6
     */
    void Clear()
    {
        std::lock_guard<std::mutex> lock(lock_);
        profiles_.clear();
    }
    /**
     * @brief Get profiles map by adapter transport.
     *
     * @param transport Adapter transport.
     * @return Returns profiles map by adapter transport.
     * @since 6
     */
    std::map<std::string, T> *GetProfiles(const BTTransport transport)
    {
        std::lock_guard<std::mutex> lock(lock_);

        auto it = profiles_.find(transport);
        if (it != profiles_.end()) {  // find
            return &(profiles_[transport]);
        } else {
            return nullptr;
        }
    }
    /**
     * @brief Set profile value.
     *
     * @param transport Adapter transport.
     * @param name Profile name
     * @param data Used to set profile value.
     * @since 6
     */
    void SetProfile(const BTTransport transport, const std::string &name, const T &data)
    {
        std::lock_guard<std::mutex> lock(lock_);

        profiles_[transport][name] = data;
    }
    /**
     * @brief Check whether profiles map contains this profile by transport and profile name.
     *
     * @param transport Adapter transport.
     * @param name Profile name
     * @return Returns <b>true</b> if profiles map contains this profile;
     *         returns <b>false</b> if profiles map does not contain this profile.
     * @since 6
     */
    bool Contains(const BTTransport transport, const std::string &name)
    {
        std::lock_guard<std::mutex> lock(lock_);

        auto its = profiles_.find(transport);
        if (its != profiles_.end()) {
            auto it = profiles_[transport].find(name);
            if (it != profiles_[transport].end()) {
                return true;
            }
        }
        return false;
    }
    /**
     * @brief Check whether profiles map contains this profile by profile name.
     *
     * @param name Profile name
     * @return Returns <b>true</b> if profiles map contains this profile;
     *         returns <b>false</b> if profiles map does not contain this profile.
     * @since 6
     */
    bool Contains(const std::string &name)
    {
        std::lock_guard<std::mutex> lock(lock_);

        for (auto &its : profiles_) {
            auto it = profiles_[its.first].find(name);
            if (it != profiles_[its.first].end()) {
                return true;
            }
        }
        return false;
    }
    /**
     * @brief Find whether profiles map contains this profile by transport and profile name.
     * Than get this profile pointer.
     *
     * @param transport Adapter transport.
     * @param name Profile name.
     * @param data Get profile pointer.
     * @return Returns <b>true</b> if profiles map contains this profile;
     *         returns <b>false</b> if profiles map does not contain this profile.
     * @since 6
     */
    bool Find(const BTTransport transport, const std::string &name, T &data)
    {
        std::lock_guard<std::mutex> lock(lock_);

        auto its = profiles_.find(transport);
        if (its != profiles_.end()) {
            auto it = profiles_[transport].find(name);
            if (it != profiles_[transport].end()) {
                data = profiles_[transport][name];
                return true;
            }
        }
        return false;
    }
    /**
     * @brief Find whether profiles map contains this profile by profile name.
     * Than get this profile pointer.
     *
     * @param name Profile name
     * @param data Get profile pointer.
     * @return Returns <b>true</b> if profiles map contains this profile;
     *         returns <b>false</b> if profiles map does not contain this profile.
     * @since 6
     */
    bool Find(const std::string &name, T &data)
    {
        std::lock_guard<std::mutex> lock(lock_);

        for (auto its = profiles_.begin(); its != profiles_.end(); its++) {
            auto it = profiles_[its->first].find(name);
            if (it != profiles_[its->first].end()) {
                data = profiles_[its->first][name];
                return true;
            }
        }
        return false;
    }
    /**
     * @brief Whether profiles map is empty.
     *
     * @param @param transport Adapter transport.
     * @return Returns <b>true</b> if profiles map is empty;
     *         returns <b>false</b> if profiles map is not empty.
     * @since 6
     */
    bool IsEmpty(const BTTransport transport)
    {
        std::lock_guard<std::mutex> lock(lock_);

        auto it = profiles_.find(transport);
        if (it != profiles_.end()) {  // find
            return profiles_[transport].empty();
        } else {
            return true;
        }
    }
    /**
     * @brief Get profiles map size.
     *
     * @param @param transport Adapter transport.
     * @return Returns profiles map size
     * @since 6
     */
    int Size(const BTTransport transport)
    {
        std::lock_guard<std::mutex> lock(lock_);

        auto it = profiles_.find(transport);
        if (it != profiles_.end()) {  // find
            return profiles_[transport].size();
        } else {
            return 0;
        }
    }

private:
    std::mutex lock_ = {};
    std::map<BTTransport, std::map<std::string, T>> profiles_ = {};
    BT_DISALLOW_COPY_AND_ASSIGN(ProfilesList);
};

#define FOR_EACH_LIST(it, profileList, transport) for (auto it : *(profileList.GetProfiles(transport)))
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PROFILE_CONFIG_H