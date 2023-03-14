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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_I18N_LOCALIZATION_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_I18N_LOCALIZATION_H

#include <future>
#include <string>
#include <vector>

#include "base/utils/date_util.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

struct LocaleProxy;

struct LunarDate : Date {
    bool isLeapMonth = false;
};

struct DateTime final : Date {
    uint32_t hour = 0;
    uint32_t minute = 0;
    uint32_t second = 0;
};

enum DateTimeStyle { NONE, FULL, LONG, MEDIUM, SHORT };

enum MeasureFormatStyle { WIDTH_WIDE, WIDTH_SHORT, WIDTH_NARROW, WIDTH_NUMERIC, WIDTH_COUNT };

enum TimeUnitStyle { YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MILLISECOND };

class ACE_FORCE_EXPORT_WITH_PREVIEW Localization : public NonCopyable {
public:
    /**
     * Get language list to select the best language.
     * @return language list which is supported
     */
    virtual ~Localization();
    static const std::vector<std::string>& GetLanguageList(const std::string& language);

    static std::shared_ptr<Localization> GetInstance();

    static void SetLocale(const std::string& language, const std::string& countryOrRegion, const std::string& script,
        const std::string& selectLanguage, const std::string& keywordsAndValues);

    static std::string ComputeScript(const std::string& language, const std::string& region);

    static void ParseLocaleTag(
        const std::string& languageTag, std::string& language, std::string& script, std::string& region,
        bool needAddSubtags);

    void SetOnChange(const std::function<void()>& value) {
        onChange_ = value;
    }

    void HandleOnChange()
    {
        if (onChange_) {
            onChange_();
        }
    }

    void SetOnMymrChange(const std::function<void(bool)>& value)
    {
        onMymrChange_ = value;
    }

    const std::function<void(bool)>& GetOnMymrChange() const
    {
        return onMymrChange_;
    }

    void HandleOnMymrChange(bool isZawgyiMyanmar)
    {
        if (onMymrChange_) {
            onMymrChange_(isZawgyiMyanmar);
        }
    }

    bool GetDateColumnFormatOrder(std::vector<std::string>& outOrder);

    bool IsAmPmHour()
    {
        bool isAmPm = false;
        bool hasZero = true;
        GetHourFormat(isAmPm, hasZero);
        return isAmPm;
    }

    bool HasZeroHour()
    {
        bool isAmPm = false;
        bool hasZero = true;
        GetHourFormat(isAmPm, hasZero);
        return hasZero;
    }

    std::string GetLanguage();
    std::string GetLanguageTag();
    std::string GetFontLocale();

    /**
     * For formatting local format duration. Cannot be formatted correctly when duration greater than 24
     * hours. For example: 10:00.
     * @param duration    The value used to set the duration, the range is 0 to 90,000.
     * @return local format duration.
     */
    const std::string FormatDuration(uint32_t duration, bool needShowHour = false);

    /**
     * For formatting local format duration. Cannot be formatted correctly when duration greater than 24
     * hours. For example: 10:00.
     * @param duration    The value used to set the duration, the range is 0 to 90,000.
     * @param format      the pattern for the format.For example: HH-mm-ss-SS.
     * @return local format duration.
     */
    std::string FormatDuration(uint32_t duration, const std::string& format);

    /**
     * For formatting date time. For example: 2020/03/30 08:00:00.
     * @param dateTime    The value of date time.
     * @param format      the pattern for the format.
     * @return local format date time.
     */
    const std::string FormatDateTime(DateTime dateTime, const std::string& format);

    /**
     * For formatting date time.
     * @param dateTime     The value of date time.
     * @param dateStyle    The value of date style.
     * @param timeStyle    The value of time style.
     * @return local format date time.
     */
    const std::string FormatDateTime(DateTime dateTime, DateTimeStyle dateStyle, DateTimeStyle timeStyle);

    /**
     * Gets month strings. For example: "January", "February", etc.
     * @param isShortType    The month style.
     * @param calendarType   The calendar style.
     * @return the month string vector.
     */
    std::vector<std::string> GetMonths(bool isShortType = false, const std::string& calendarType = "");

    /**
     * Gets weekdays strings. For example: "Monday", "Tuesday", etc.
     * @param isShortType    The weekday style.
     * @return the weekday string vector.
     */
    std::vector<std::string> GetWeekdays(bool isShortType = false);

    /**
     * Gets AM/PM strings. For example: "AM", "PM".
     * @return the AM/PM string vector.
     */
    std::vector<std::string> GetAmPmStrings();

    /**
     * Gets relative date. For example: "yesterday", "today", "tomorrow", "in 2 days", etc.
     * @param offset    The relative date time offset.
     * @return the relative date string.
     */
    std::string GetRelativeDateTime(double offset);

    /**
     * Gets lunar date.
     * @param date the western calendar date.
     * @return the lunar calendar date.
     */
    LunarDate GetLunarDate(Date date);

    /**
     * Gets lunar month.
     * @param month the western calendar month.
     * @param isLeapMonth is a leap month.
     * @return the lunar calendar month.
     */
    std::string GetLunarMonth(uint32_t month, bool isLeapMonth);

    /**
     * Gets lunar day.
     * @param dayOfMonth the western calendar day.
     * @return the lunar calendar day.
     */
    std::string GetLunarDay(uint32_t dayOfMonth);

    /**
     * For formatting time unit. For example: "8hours", "8min", etc .
     * @param timeValue the format time value.
     * @param timeStyle the time unit style.
     * @param formatStyle the measure format style.
     * @return local format time unit.
     */
    std::string TimeUnitFormat(double timeValue, TimeUnitStyle timeStyle, MeasureFormatStyle formatStyle);

    /**
     * For formatting plural rules.
     * @param number the number to be formatted.
     * @param isCardinal the plural is cardinal type.
     * @return local keyword of the plural rule.
     */
    std::string PluralRulesFormat(double number, bool isCardinal = true);

    /**
     * Gets Letter strings for indexer. For example: "A", "B", etc.
     * @return the letter string vector.
     */
    std::vector<std::u16string> GetIndexLetter();

    /**
     * For formatting number.
     * @param number the number to be formatted.
     * @return local format number.
     */
    std::string NumberFormat(double number);

    /**
     * Gets alphabet strings. For example: "A", "B", etc.
     * @return the alphabet string vector.
     */
    std::vector<std::u16string> GetIndexAlphabet();

    /**
     * Gets entry letters, read from resource/binary/entry.json.
     * @param lettersIndex letters index, like "common.ok"
     * @return letters
     */
    std::string GetEntryLetters(const std::string& lettersIndex);

    /**
     * Gets error description, read from resource/binary/errorcode.json.
     * @param errorIndex error index, like "error_video_000001"
     * @return error description
     */
    std::string GetErrorDescription(const std::string& errorIndex);

private:
    void SetLocaleImpl(const std::string& language, const std::string& countryOrRegion, const std::string& script,
        const std::string& selectLanguage, const std::string& keywordsAndValues);
    std::vector<std::u16string> GetLetters(bool alphabet);
    bool GetHourFormat(bool& isAmPm, bool& hasZero);
    bool Contain(const std::string& str, const std::string& tag);

    std::unique_ptr<LocaleProxy> locale_;
    std::string languageTag_;
    std::string selectLanguage_;
    std::string fontLocale_;

    std::promise<bool> promise_;
    std::shared_future<bool> future_ = promise_.get_future();
    std::function<void()> onChange_;
    std::function<void(bool)> onMymrChange_;
    bool isPromiseUsed_ = false;
    bool isInit_ = false;

    void WaitingForInit()
    {
        if (!isInit_) {
            isInit_ = future_.get();
        }
    }

    static std::mutex mutex_;
    static std::shared_ptr<Localization> instance_;
    static bool firstInstance_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_I18N_LOCALIZATION_H
