/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include "no_copyable.h"

namespace OHOS::Camera {
template<typename B, typename... Args>
class RegisterFactoty : public NoCopyable {
public:
    using CreatorFunc = std::function<std::shared_ptr<B>(Args...)>;
    static RegisterFactoty& Instance()
    {
        static RegisterFactoty r;
        return r;
    }

    template<typename T, typename std::enable_if_t<std::is_base_of_v<B, T>>* = nullptr>
    std::string DoRegister(const std::vector<std::string>& names, CreatorFunc f)
    {
        std::string result;
        for (const auto& it : names) {
            result += it;
            builder_.emplace(it, f);
        }
        return result;
    }

    std::shared_ptr<B> CreateShared(const std::string& name, Args... args)
    {
        std::shared_ptr<B> result = nullptr;
        for (auto&& [n, builder] : builder_) {
            if (n == name) {
                result = builder(std::forward<Args>(args)...);
                break;
            }
        }
        return result;
    }
    RegisterFactoty() = default;
    ~RegisterFactoty() = default;
private:
    std::unordered_map<std::string, CreatorFunc> builder_;
};
}
#endif