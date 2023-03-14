/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MESSAGE_H
#define MESSAGE_H

namespace utility {
struct Message {
public:
    /**
     * @brief Construct a new Message object.
     *
     * @param what Message Identifier.
     * @param arg1 Message first arg.
     * @param arg2 Message second arg.
     * @since 6
     */
    Message(int what, int arg1 = 0, void *arg2 = nullptr) : what_(what), arg1_(arg1), arg2_(arg2){};

    /**
     * @brief Construct a new Message object.
     *
     * @since 6
     */
    Message() = default;

    /**
     * @brief Destroy the Message object.
     *
     * @since 6
     */
    ~Message() = default;

    int what_ = 0;
    int arg1_ = 0;
    void *arg2_ = nullptr;
};
}  // namespace utility

#endif  // MESSAGE_H