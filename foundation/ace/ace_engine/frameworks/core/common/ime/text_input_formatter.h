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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_FORMATTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_FORMATTER_H

#include <regex>

namespace OHOS::Ace {

struct TextEditingValue;

class TextInputFormatter {
public:
    virtual ~TextInputFormatter() = default;

    /**
     * @brief Format the input text.
     *
     * @param[in] oldValue If multiple formatters called, this is the original value of the format chain.
     * @param[out] newValue The value after formatting.
     */
    virtual void Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const = 0;
};

class BlackListCharsFormatter : public TextInputFormatter {
public:
    explicit BlackListCharsFormatter(std::wregex&& regex);
    ~BlackListCharsFormatter() override = default;

    void Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const override;

private:
    std::wregex regex_;
};

class WhiteListCharsFormatter : public TextInputFormatter {
public:
    void Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const override {}
};

class NumberFormatter : public BlackListCharsFormatter {
public:
    NumberFormatter();
    ~NumberFormatter() override = default;
};

class PhoneNumberFormatter : public BlackListCharsFormatter {
public:
    PhoneNumberFormatter();
    ~PhoneNumberFormatter() override = default;
};

class UriFormatter : public BlackListCharsFormatter {
public:
    UriFormatter();
    ~UriFormatter() override = default;
};

class EmailFormatter : public BlackListCharsFormatter {
public:
    EmailFormatter();
    ~EmailFormatter() override = default;
};

class SingleLineFormatter : public BlackListCharsFormatter {
public:
    SingleLineFormatter();
    ~SingleLineFormatter() override = default;
};

class LengthLimitingFormatter : public TextInputFormatter {
public:
    explicit LengthLimitingFormatter(uint32_t limit) : limit_(limit) {}
    ~LengthLimitingFormatter() override = default;
    void Format(const TextEditingValue& oldValue, TextEditingValue& newValue) const override;

private:
    const uint32_t limit_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_IME_TEXT_INPUT_FORMATTER_H
