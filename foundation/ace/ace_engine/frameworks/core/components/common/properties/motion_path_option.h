/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_OPTION_H

#include <memory>
#include <string>

namespace OHOS::Ace {

class MotionPathOption final {
public:
    MotionPathOption(const std::string& path = "", float begin = 0.0f, float end = 1.0f, bool rotate = false)
        : path_(path), begin_(begin), end_(end), rotate_(rotate)
    {}
    ~MotionPathOption() = default;

    void SetPath(const std::string& path)
    {
        path_ = path;
    }
    const std::string& GetPath() const
    {
        return path_;
    }

    void SetBegin(float value)
    {
        begin_ = value;
    }

    float GetBegin() const
    {
        return begin_;
    }

    void SetEnd(float value)
    {
        end_ = value;
    }

    float GetEnd() const
    {
        return end_;
    }

    void SetRotate(bool value)
    {
        rotate_ = value;
    }

    bool GetRotate() const
    {
        return rotate_;
    }

    bool IsValid() const
    {
        return !path_.empty();
    }

private:
    std::string path_;
    float begin_ = 0.0f;
    float end_ = 1.0f;
    bool rotate_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_MOTION_PATH_OPTION_H
