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

#include "base/i18n/localization.h"

#include <map>
#include <unordered_map>

#include "chnsecal.h"
#include "unicode/calendar.h"
#include "unicode/datefmt.h"
#include "unicode/dtfmtsym.h"
#include "unicode/dtptngen.h"
#include "unicode/locid.h"
#include "unicode/measfmt.h"
#include "unicode/measure.h"
#include "unicode/numberformatter.h"
#include "unicode/plurrule.h"
#include "unicode/reldatefmt.h"
#include "unicode/smpdtfmt.h"
#include "unicode/uclean.h"
#include "unicode/udata.h"

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/resource/internal_resource.h"
#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

using namespace icu;

struct LocaleProxy final {
    LocaleProxy(const char* language, const char* countryOrRegion, const char* variant, const char* keywordsAndValues)
        : instance(language, countryOrRegion, variant, keywordsAndValues)
    {}
    ~LocaleProxy() = default;

    Locale instance;
};

namespace {

#define CHECK_RETURN(status, ret)                                      \
    do {                                                               \
        if ((status) > U_ZERO_ERROR) {                                   \
            LOGE("status = %{public}d", static_cast<int32_t>(status)); \
            return (ret);                                                \
        }                                                              \
    } while (0)

#define CHECK_NO_RETURN(status)                                        \
    do {                                                               \
        if ((status) > U_ZERO_ERROR) {                                   \
            LOGE("status = %{public}d", static_cast<int32_t>(status)); \
        }                                                              \
    } while (0)

const char JSON_PATH_CARVE = '.';
const char DEFAULT_LANGUAGE[] = "en-US";
constexpr uint32_t SEXAGENARY_CYCLE_SIZE = 60;
constexpr uint32_t GUIHAI_YEAR_RECENT = 3;
constexpr uint32_t SECONDS_IN_HOUR = 3600;

const char CHINESE_LEAP[] = u8"\u95f0";
const char CHINESE_FIRST[] = u8"\u521d";
const char CHINESE_TEN[] = u8"\u5341";
const char CHINESE_TWENTY[] = u8"\u5eff";
const char* g_chineseOneToNine[] = {
    u8"\u4e00", u8"\u4e8c", u8"\u4e09", u8"\u56db", u8"\u4e94", u8"\u516d", u8"\u4e03", u8"\u516b", u8"\u4e5d"
};
const std::unordered_map<std::string, std::string> LANGUAGE_CODE_MAP {
    { "he", "iw" },
    { "fil", "tl" },
    { "id", "in" },
};

inline void UnicodeString2String(const UnicodeString& source, std::string& result)
{
    source.toUTF8String(result);
}

DateFormat::EStyle DateTimeStyle2EStyle(DateTimeStyle dateTimeStyle)
{
    switch (dateTimeStyle) {
        case DateTimeStyle::NONE:
            return DateFormat::EStyle::kNone;
        case DateTimeStyle::FULL:
            return DateFormat::EStyle::kFull;
        case DateTimeStyle::LONG:
            return DateFormat::EStyle::kLong;
        case DateTimeStyle::MEDIUM:
            return DateFormat::EStyle::kMedium;
        case DateTimeStyle::SHORT:
            return DateFormat::EStyle::kShort;
        default:
            return DateFormat::EStyle::kNone;
    }
}

UMeasureFormatWidth GetMeasureFormatWidth(MeasureFormatStyle formatStyle)
{
    switch (formatStyle) {
        case MeasureFormatStyle::WIDTH_WIDE:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_WIDE;
        case MeasureFormatStyle::WIDTH_SHORT:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_SHORT;
        case MeasureFormatStyle::WIDTH_NARROW:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_NARROW;
        case MeasureFormatStyle::WIDTH_NUMERIC:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_NUMERIC;
        case MeasureFormatStyle::WIDTH_COUNT:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_COUNT;
        default:
            return UMeasureFormatWidth::UMEASFMT_WIDTH_WIDE;
    }
}

MeasureUnit* GetMeasureUnit(TimeUnitStyle timeUnitStyle, UErrorCode& status)
{
    switch (timeUnitStyle) {
        case TimeUnitStyle::YEAR:
            return MeasureUnit::createYear(status);
        case TimeUnitStyle::MONTH:
            return MeasureUnit::createMonth(status);
        case TimeUnitStyle::DAY:
            return MeasureUnit::createDay(status);
        case TimeUnitStyle::HOUR:
            return MeasureUnit::createHour(status);
        case TimeUnitStyle::MINUTE:
            return MeasureUnit::createMinute(status);
        case TimeUnitStyle::SECOND:
            return MeasureUnit::createSecond(status);
        case TimeUnitStyle::MILLISECOND:
            return MeasureUnit::createMillisecond(status);
        default:
            return MeasureUnit::createYear(status);
    }
}

void GetLocalJsonObject(InternalResource::ResourceId id, std::string language, std::unique_ptr<JsonValue>& indexJson,
    std::unique_ptr<JsonValue>& json)
{
    if (indexJson == nullptr) {
        size_t size = 0;
        const uint8_t* buf = InternalResource::GetInstance().GetResource(id, size);
        if (buf == nullptr) {
            return;
        }

        std::string jsonStr(reinterpret_cast<const char*>(buf), size);
        const char* endMsg = nullptr;
        indexJson = JsonUtil::ParseJsonString(jsonStr, &endMsg);
        if (indexJson == nullptr) {
            LOGE("read indexletter json failed. reason: %{private}s.", endMsg);
            return;
        }
    }

    if (indexJson->Contains(language) && indexJson->GetValue(language)->IsObject()) {
        json = indexJson->GetValue(language);
    } else if (indexJson->Contains(DEFAULT_LANGUAGE) && indexJson->GetValue(DEFAULT_LANGUAGE)->IsObject()) {
        json = indexJson->GetValue(DEFAULT_LANGUAGE);
    }
}

} // namespace

// for entry.json
static std::unique_ptr<JsonValue> g_indexJsonEntry = nullptr;
static std::unique_ptr<JsonValue> g_indexJsonError = nullptr;

Localization::~Localization() = default;

void Localization::SetLocaleImpl(const std::string& language, const std::string& countryOrRegion,
    const std::string& script, const std::string& selectLanguage, const std::string& keywordsAndValues)
{
    locale_ = std::make_unique<LocaleProxy>(language.c_str(), countryOrRegion.c_str(), "", keywordsAndValues.c_str());

    UErrorCode status = U_ZERO_ERROR;
    std::vector<std::string> keyValuePairs;
    StringUtils::StringSpliter(keywordsAndValues, ';', keyValuePairs);
    for (const auto& pair : keyValuePairs) {
        // [pair] is like "nu=arab" or "nu=" for most occasions, but may be "=" under extreme scenarios
        std::vector<std::string> res;
        StringUtils::StringSpliter(pair, '=', res);
        if (res.size() == 0) {
            continue;
        }
        auto value = (res.size() == 2) ? res[1] : "";
        locale_->instance.setUnicodeKeywordValue(res[0], value, status);
        CHECK_NO_RETURN(status);
    }

    languageTag_ = language;
    if (!script.empty()) {
        languageTag_.append("-").append(script);
    }
    languageTag_.append("-").append(countryOrRegion);
    fontLocale_ = languageTag_;
    // Simple chinese
    if (languageTag_ == "zh-Hans-CN") {
        languageTag_ = "zh-CN";
        fontLocale_ = "";
    }

    selectLanguage_ = selectLanguage;
    // match json of latin
    if (selectLanguage_ == "jv-Latn") {
        selectLanguage_ = "b+jv+Latn";
    } else if (selectLanguage_ == "sr-Latn") {
        selectLanguage_ = "b+sr+Latn";
    }

    LOGI("SetLocale language tag: %{public}s, select language: %{public}s", languageTag_.c_str(),
        selectLanguage_.c_str());
    if (!isPromiseUsed_) {
        promise_.set_value(true);
        isPromiseUsed_ = true;
    }
}

std::string Localization::GetLanguage()
{
    WaitingForInit();
    if (locale_) {
        return locale_->instance.getLanguage();
    }
    return "";
}

std::string Localization::GetLanguageTag()
{
    WaitingForInit();
    return languageTag_;
}

std::string Localization::GetFontLocale()
{
    WaitingForInit();
    return fontLocale_;
}

const std::string Localization::FormatDuration(uint32_t duration, bool needShowHour)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    // duration greater than 1 hour, use hh:mm:ss;
    if (!needShowHour && duration > SECONDS_IN_HOUR) {
        needShowHour = true;
    }
    const char* engTimeFormat = needShowHour ? "hh:mm:ss" : "mm:ss";
    auto simpleDateFormat = std::make_unique<SimpleDateFormat>(UnicodeString(engTimeFormat), locale_->instance, status);
    CHECK_RETURN(status, "");

    UnicodeString simpleStr;
    simpleDateFormat->format(1000.0 * duration, simpleStr, status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(simpleStr, ret);
    return ret;
}

std::string Localization::FormatDuration(uint32_t duration, const std::string& format)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;

    const char* engTimeFormat = format.c_str();
    auto simpleDateFormat = std::make_unique<SimpleDateFormat>(UnicodeString(engTimeFormat), locale_->instance, status);
    CHECK_RETURN(status, "");

    UnicodeString simpleStr;
    simpleDateFormat->format(1.0 * duration, simpleStr, status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(simpleStr, ret);
    return ret;
}

const std::string Localization::FormatDateTime(DateTime dateTime, const std::string& format)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    auto cal = Calendar::createInstance(locale_->instance, status);
    CHECK_RETURN(status, "");
    cal->set(dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);

    UDate date = cal->getTime(status);
    delete cal;
    CHECK_RETURN(status, "");

    auto patternGenerator = DateTimePatternGenerator::createInstance(locale_->instance, status);
    CHECK_RETURN(status, "");
    UnicodeString pattern = patternGenerator->getBestPattern(UnicodeString(format.c_str()), status);
    delete patternGenerator;
    CHECK_RETURN(status, "");

    auto dateFormat = std::make_unique<SimpleDateFormat>(pattern, locale_->instance, status);
    CHECK_RETURN(status, "");

    UnicodeString dateTimeStr;
    dateFormat->format(date, dateTimeStr, status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(dateTimeStr, ret);
    return ret;
}

bool Localization::GetDateColumnFormatOrder(std::vector<std::string>& outOrder)
{
    outOrder.clear();
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;

    auto patternGenerator = DateTimePatternGenerator::createInstance(locale_->instance, status);
    CHECK_RETURN(status, false);
    std::string format = "yyyyMMdd";
    UnicodeString pattern = patternGenerator->getBestPattern(UnicodeString(format.c_str()), status);
    delete patternGenerator;
    CHECK_RETURN(status, false);

    std::string result;
    UnicodeString2String(pattern, result);
    LOGD("order of date format is %{public}s", result.c_str());

    std::map<std::size_t, std::string> order;
    std::size_t position = result.find("yyyy");
    if (position == std::string::npos) {
        return false;
    }
    order[position] = "year";

    position = result.find("MM");
    if (position == std::string::npos) {
        return false;
    }
    order[position] = "month";

    position = result.find("dd");
    if (position == std::string::npos) {
        return false;
    }
    order[position] = "day";

    for (auto it = order.begin(); it != order.end(); ++it) {
        outOrder.emplace_back(it->second);
    }

    return true;
}

bool Localization::Contain(const std::string& str, const std::string& tag)
{
    auto pos = str.find(tag);
    return (pos != std::string::npos);
}

bool Localization::GetHourFormat(bool& isAmPm, bool& hasZero)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;

    auto patternGenerator = DateTimePatternGenerator::createInstance(locale_->instance, status);
    CHECK_RETURN(status, false);
    std::string format = "J:mm";
    UnicodeString pattern = patternGenerator->getBestPattern(UnicodeString(format.c_str()), status);
    delete patternGenerator;
    CHECK_RETURN(status, false);

    std::string result;
    UnicodeString2String(pattern, result);
    LOGI("hour format is %{public}s", result.c_str());

    if (Contain(result, "hh") || Contain(result, "KK")) {
        isAmPm = true;
        hasZero = true;
        return true;
    }

    if (Contain(result, "h") || Contain(result, "K")) {
        isAmPm = true;
        hasZero = false;
        return true;
    }

    if (Contain(result, "HH") || Contain(result, "kk")) {
        isAmPm = false;
        hasZero = true;
        return true;
    }

    if (Contain(result, "H") || Contain(result, "k")) {
        isAmPm = false;
        hasZero = false;
        return true;
    }

    LOGE("hour format is unknown[%{public}s]", result.c_str());
    return false;
}

const std::string Localization::FormatDateTime(DateTime dateTime, DateTimeStyle dateStyle, DateTimeStyle timeStyle)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    auto cal = Calendar::createInstance(locale_->instance, status);
    CHECK_RETURN(status, "");
    cal->set(dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, dateTime.second);

    UDate date = cal->getTime(status);
    delete cal;
    CHECK_RETURN(status, "");

    auto dateFormat = DateFormat::createDateTimeInstance(
        DateTimeStyle2EStyle(dateStyle), DateTimeStyle2EStyle(timeStyle), locale_->instance);
    if (dateFormat == nullptr) {
        return "";
    }

    UnicodeString dateTimeStr;
    dateFormat->format(date, dateTimeStr, status);
    delete dateFormat;
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(dateTimeStr, ret);
    return ret;
}

std::vector<std::string> Localization::GetMonths(bool isShortType, const std::string& calendarType)
{
    WaitingForInit();
    std::vector<std::string> months;
    UErrorCode status = U_ZERO_ERROR;
    DateFormatSymbols dateformat(locale_->instance, calendarType.c_str(), status);
    CHECK_RETURN(status, months);

    int32_t count = 0;

    auto monthsUniStr = dateformat.getMonths(count, DateFormatSymbols::DtContextType::STANDALONE,
        isShortType ? DateFormatSymbols::DtWidthType::SHORT : DateFormatSymbols::DtWidthType::WIDE);
    if (count > 0) {
        std::string month;
        for (int32_t i = 0; i < count; i++) {
            month.clear();
            UnicodeString2String(monthsUniStr[i], month);
            months.push_back(month);
        }
    }
    return months;
}

std::vector<std::string> Localization::GetWeekdays(bool isShortType)
{
    WaitingForInit();
    std::vector<std::string> weekdays;
    UErrorCode status = U_ZERO_ERROR;
    DateFormatSymbols dateformat(locale_->instance, status);
    CHECK_RETURN(status, weekdays);

    int32_t count = 0;

    auto language = locale_->instance.getLanguage();
    auto widthType = isShortType ? (strcmp(language, "zh") == 0 || strcmp(language, "bo") == 0)
                                       ? DateFormatSymbols::DtWidthType::NARROW
                                       : DateFormatSymbols::DtWidthType::ABBREVIATED
                                 : DateFormatSymbols::DtWidthType::WIDE;
    auto weekdaysUniStr = dateformat.getWeekdays(count, DateFormatSymbols::DtContextType::STANDALONE, widthType);
    if (count > 0) {
        std::string weekday;
        for (int32_t i = 0; i < count; i++) {
            weekday.clear();
            UnicodeString2String(weekdaysUniStr[i], weekday);
            if (!weekday.empty()) {
                weekdays.push_back(weekday);
            }
        }
    }
    return weekdays;
}

std::vector<std::string> Localization::GetAmPmStrings()
{
    WaitingForInit();
    std::vector<std::string> amPms;
    UErrorCode status = U_ZERO_ERROR;
    DateFormatSymbols dateformat(locale_->instance, status);
    CHECK_RETURN(status, amPms);

    int32_t count = 0;

    auto amPmUniStr = dateformat.getAmPmStrings(count);
    if (count > 0) {
        std::string amPm;
        for (int32_t i = 0; i < count; i++) {
            amPm.clear();
            UnicodeString2String(amPmUniStr[i], amPm);
            amPms.push_back(amPm);
        }
    }
    return amPms;
}

std::string Localization::GetRelativeDateTime(double offset)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    RelativeDateTimeFormatter relativeDateformat(locale_->instance, status);
    CHECK_RETURN(status, "");

    UnicodeString relativeDate;
    relativeDateformat.format(offset, URelativeDateTimeUnit::UDAT_REL_UNIT_DAY, relativeDate, status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(relativeDate, ret);
    return ret;
}

LunarDate Localization::GetLunarDate(Date date)
{
    WaitingForInit();
    LunarDate dateRet;
    UErrorCode status = U_ZERO_ERROR;
    Locale locale("zh", "CN");
    auto cal = Calendar::createInstance(locale, status);
    CHECK_RETURN(status, dateRet);
    // 0 means January,  1 means February, so month - 1
    cal->set(date.year, date.month - 1, date.day);

    UDate udate = cal->getTime(status);
    delete cal;
    CHECK_RETURN(status, dateRet);

    ChineseCalendar chineseCalendar(locale, status);
    CHECK_RETURN(status, dateRet);

    chineseCalendar.setTime(udate, status);
    CHECK_RETURN(status, dateRet);

    int32_t lunarYear = chineseCalendar.get(UCalendarDateFields::UCAL_YEAR, status);
    CHECK_RETURN(status, dateRet);
    int32_t lunarMonth = chineseCalendar.get(UCalendarDateFields::UCAL_MONTH, status);
    CHECK_RETURN(status, dateRet);
    int32_t lunarDate = chineseCalendar.get(UCalendarDateFields::UCAL_DATE, status);
    CHECK_RETURN(status, dateRet);
    int32_t isLeapMonth = chineseCalendar.get(UCalendarDateFields::UCAL_IS_LEAP_MONTH, status);
    CHECK_RETURN(status, dateRet);

    // Sexagenary cycle years convert to Western years
    dateRet.year = static_cast<uint32_t>(lunarYear) + GUIHAI_YEAR_RECENT;
    dateRet.year +=
        ((static_cast<uint32_t>(date.year) - GUIHAI_YEAR_RECENT) / SEXAGENARY_CYCLE_SIZE) * SEXAGENARY_CYCLE_SIZE;
    // 0 means January,  1 means February, so month + 1
    dateRet.month = static_cast<uint32_t>(lunarMonth) + 1;
    dateRet.day = static_cast<uint32_t>(lunarDate);
    dateRet.isLeapMonth = !(isLeapMonth == 0);
    return dateRet;
}

std::string Localization::GetLunarMonth(uint32_t month, bool isLeapMonth)
{
    WaitingForInit();
    std::vector<std::string> months = Localization::GetInstance()->GetMonths(false, "chinese");
    if (month <= months.size() && month > 0) {
        std::string leap;
        if (isLeapMonth) {
            leap += std::string(CHINESE_LEAP);
        }
        return leap + months[month - 1];
    } else {
        LOGE("month parameter is illegal:%{public}d", month);
        return "";
    }
}

std::string Localization::GetLunarDay(uint32_t dayOfMonth)
{
    WaitingForInit();
    if (dayOfMonth > 30 || dayOfMonth == 0) {
        LOGE("dayOfMonth parameter is illegal");
        return "";
    }

    std::string ret;
    if (dayOfMonth < 10) {
        ret = std::string(CHINESE_FIRST) + std::string(g_chineseOneToNine[dayOfMonth - 1]);
    } else if (dayOfMonth == 10) {
        ret = std::string(CHINESE_FIRST) + std::string(CHINESE_TEN);
    } else if (dayOfMonth < 20) {
        ret = std::string(CHINESE_TEN) + std::string(g_chineseOneToNine[dayOfMonth - 11]);
    } else if (dayOfMonth == 20) {
        ret = std::string(CHINESE_TWENTY) + std::string(CHINESE_TEN);
    } else if (dayOfMonth == 30) {
        ret = g_chineseOneToNine[2] + std::string(CHINESE_TEN);
    } else {
        ret = std::string(CHINESE_TWENTY) + std::string(g_chineseOneToNine[dayOfMonth - 21]);
    }

    return ret;
}

std::string Localization::TimeUnitFormat(double timeValue, TimeUnitStyle timeStyle, MeasureFormatStyle formatStyle)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    MeasureFormat measureFormat(locale_->instance, GetMeasureFormatWidth(formatStyle), status);
    CHECK_RETURN(status, "");

    MeasureUnit* minuteUnit = GetMeasureUnit(timeStyle, status);
    CHECK_RETURN(status, "");

    Formattable formattable(timeValue);
    Measure measure(formattable, minuteUnit, status);
    CHECK_RETURN(status, "");

    UnicodeString timeUnit;
    FieldPosition fieldPosition;
    measureFormat.formatMeasures(&measure, 1, timeUnit, fieldPosition, status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(timeUnit, ret);
    return ret;
}

std::string Localization::PluralRulesFormat(double number, bool isCardinal)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;
    UPluralType pluralType = isCardinal ? UPluralType::UPLURAL_TYPE_CARDINAL : UPluralType::UPLURAL_TYPE_ORDINAL;
    PluralRules* pluralRules = PluralRules::forLocale(locale_->instance, pluralType, status);
    CHECK_RETURN(status, "");

    UnicodeString numberFormat = pluralRules->select(number);
    delete pluralRules;

    std::string ret;
    UnicodeString2String(numberFormat, ret);
    return ret;
}

std::string Localization::NumberFormat(double number)
{
    WaitingForInit();
    UErrorCode status = U_ZERO_ERROR;

    icu::number::LocalizedNumberFormatter formatter = icu::number::NumberFormatter::withLocale(locale_->instance);
    icu::number::FormattedNumber formattedNumber = formatter.formatDouble(number, status);
    CHECK_RETURN(status, "");

    UnicodeString numberFormat = formattedNumber.toString(status);
    CHECK_RETURN(status, "");

    std::string ret;
    UnicodeString2String(numberFormat, ret);
    return ret;
}

std::vector<std::u16string> Localization::GetLetters(bool alphabet)
{
    WaitingForInit();
    std::vector<std::u16string> letters;
    size_t size = 0;
    const uint8_t* buf =
        InternalResource::GetInstance().GetResource(InternalResource::ResourceId::INDEXLETTER_BAR_JSON, size);
    if (buf == nullptr) {
        return letters;
    }

    std::string jsonStr(reinterpret_cast<const char*>(buf), size);
    const char* endMsg = nullptr;
    auto indexLetterJson = JsonUtil::ParseJsonString(jsonStr, &endMsg);
    if (indexLetterJson == nullptr) {
        LOGE("read indexletter json failed. reason: %{private}s.", endMsg);
        return letters;
    }

    std::string language = locale_->instance.getLanguage();
    if (language == "zh") {
        language = language + "-" + std::string(locale_->instance.getCountry());
    }
    auto iter = LANGUAGE_CODE_MAP.find(language);
    if (iter != LANGUAGE_CODE_MAP.end()) {
        language = iter->second;
    }
    LOGI("[alphabet] Localization::GetLetters. language: %{private}s", language.c_str());
    std::unique_ptr<JsonValue> lettersSet;
    if (!indexLetterJson->Contains(language) || !indexLetterJson->GetValue(language)->IsObject()) {
        lettersSet = indexLetterJson->GetValue("default");
    } else {
        lettersSet = indexLetterJson->GetValue(language);
    }

    std::string letterType = alphabet ? "alphabet" : "index";
    std::unique_ptr<JsonValue> lettersArray;
    if (!lettersSet->Contains(letterType) || !lettersSet->GetValue(letterType)->IsArray()) {
        LOGE("read letter array failed. Invalid type. %s", letterType.c_str());
        return letters;
    } else {
        lettersArray = lettersSet->GetValue(letterType)->GetChild();
    }

    while (lettersArray->IsValid()) {
        letters.push_back(StringUtils::Str8ToStr16(lettersArray->GetString()));
        lettersArray = lettersArray->GetNext();
    }
    return letters;
}

std::vector<std::u16string> Localization::GetIndexLetter()
{
    return GetLetters(false);
}

std::vector<std::u16string> Localization::GetIndexAlphabet()
{
    return GetLetters(true);
}

std::string Localization::GetEntryLetters(const std::string& lettersIndex)
{
    WaitingForInit();
    if (lettersIndex.empty()) {
        return "";
    }

    std::unique_ptr<JsonValue> localJsonEntry;
    auto language = selectLanguage_;
    auto iter = LANGUAGE_CODE_MAP.find(language);
    if (iter != LANGUAGE_CODE_MAP.end()) {
        language = iter->second;
    }
    GetLocalJsonObject(InternalResource::ResourceId::ENTRY_JSON, language, g_indexJsonEntry, localJsonEntry);
    if (localJsonEntry == nullptr) {
        LOGE("read JsonObject fail. language: %{public}s.", selectLanguage_.c_str());
        return "";
    }

    std::vector<std::string> jsonLetterIndex;
    StringUtils::StringSpliter(lettersIndex, JSON_PATH_CARVE, jsonLetterIndex);

    for (const auto& letter : jsonLetterIndex) {
        if (localJsonEntry && localJsonEntry->Contains(letter)) {
            localJsonEntry = localJsonEntry->GetValue(letter);
        } else {
            LOGE("read entry json failed.");
            return "";
        }
    }

    if (localJsonEntry->IsString()) {
        return localJsonEntry->GetString();
    }

    return "";
}

std::string Localization::GetErrorDescription(const std::string& errorIndex)
{
    WaitingForInit();
    if (errorIndex.empty()) {
        return "";
    }

    std::unique_ptr<JsonValue> localJsonError;
    auto language = selectLanguage_;
    auto iter = LANGUAGE_CODE_MAP.find(language);
    if (iter != LANGUAGE_CODE_MAP.end()) {
        language = iter->second;
    }
    GetLocalJsonObject(InternalResource::ResourceId::ERRORINFO_JSON, language, g_indexJsonError, localJsonError);
    if (localJsonError == nullptr) {
        LOGE("read JsonObject fail. language: %{public}s.", selectLanguage_.c_str());
        return "";
    }

    if (localJsonError->Contains(errorIndex)) {
        localJsonError = localJsonError->GetValue(errorIndex);
    } else {
        LOGE("read error json failed. error path: %{private}s.", errorIndex.c_str());
        return "";
    }

    if (localJsonError->IsString()) {
        return localJsonError->GetString();
    }

    return "";
}

const std::vector<std::string>& Localization::GetLanguageList(const std::string& language)
{
    static const LinearMapNode<std::vector<std::string>> multiLanguageMap[] = {
        { "am", { "am" } },
        { "ar", { "ar" } },
        { "as", { "as" } },
        { "az", { "az-AZ" } },
        { "be", { "be" } },
        { "bg", { "bg" } },
        { "bn", { "bn" } },
        { "bo", { "bo-CN" } },
        { "bs", { "bs" } },
        { "ca", { "ca" } },
        { "cs", { "cs" } },
        { "da", { "da" } },
        { "de", { "de" } },
        { "el", { "el" } },
        { "en", { "en-US", "en-GB" } },
        { "es", { "es,es-US" } },
        { "et", { "et" } },
        { "fa", { "fa" } },
        { "fi", { "fi" } },
        { "fil", { "fil" } },
        { "fr", { "fr" } },
        { "gl", { "gl-ES" } },
        { "he", { "he" } },
        { "hi", { "hi" } },
        { "hr", { "hr" } },
        { "hu", { "hu" } },
        { "id", { "id" } },
        { "in", { "in" } },
        { "it", { "it" } },
        { "iw", { "iw" } },
        { "ja", { "ja" } },
        { "jv", { "jv-Latn" } },
        { "ka", { "ka-GE" } },
        { "kk", { "kk-KZ" } },
        { "km", { "km-KH" } },
        { "kn", { "kn" } },
        { "ko", { "ko" } },
        { "lo", { "lo-LA" } },
        { "lt", { "lt" } },
        { "lv", { "lv" } },
        { "mai", { "mai" } },
        { "mi", { "mi" } },
        { "mk", { "mk" } },
        { "ml", { "ml" } },
        { "mn", { "mn" } },
        { "mr", { "mr" } },
        { "ms", { "ms" } },
        { "my", { "my-ZG", "my-MM" } },
        { "nb", { "nb" } },
        { "ne", { "ne" } },
        { "nl", { "nl" } },
        { "or", { "or" } },
        { "pa", { "pa" } },
        { "pl", { "pl" } },
        { "pt", { "pt", "pt-PT" } },
        { "ro", { "ro" } },
        { "ru", { "ru" } },
        { "si", { "si-LK" } },
        { "sk", { "sk" } },
        { "sl", { "sl" } },
        { "sr", { "sr-Latn" } },
        { "sv", { "sv" } },
        { "sw", { "sw" } },
        { "ta", { "ta" } },
        { "te", { "te" } },
        { "th", { "th" } },
        { "tl", { "tl" } },
        { "tr", { "tr" } },
        { "ug", { "ug" } },
        { "uk", { "uk" } },
        { "ur", { "ur" } },
        { "uz", { "uz-UZ" } },
        { "vi", { "vi" } },
        { "zh", { "zh-CN", "zh-HK", "zh-TW" } },
        { "zz", { "zz-ZX" } },
    };
    int64_t list = BinarySearchFindIndex(multiLanguageMap, ArraySize(multiLanguageMap), language.c_str());
    if (list == -1) {
        static const std::vector<std::string> defaultLanguage = { "en-US" };
        return defaultLanguage;
    }
    return multiLanguageMap[list].value;
}

std::mutex Localization::mutex_;
std::shared_ptr<Localization> Localization::instance_;
bool Localization::firstInstance_ = true;

std::shared_ptr<Localization> Localization::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::make_shared<Localization>();
    }
    return instance_;
}

void Localization::SetLocale(const std::string& language, const std::string& countryOrRegion, const std::string& script,
    const std::string& selectLanguage, const std::string& keywordsAndValues)
{
    if (instance_) {
        instance_->HandleOnChange();
        instance_->HandleOnMymrChange(script == "Qaag");
    }
    std::shared_ptr<Localization> instance;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!firstInstance_ || !instance_) {
            if (instance_) {
                auto onMymrChange = instance_->GetOnMymrChange();
                instance_ = std::make_shared<Localization>();
                instance_->SetOnMymrChange(onMymrChange);
            } else {
                instance_ = std::make_shared<Localization>();
            }
        }
        firstInstance_ = false;
        instance = instance_;
    }
    instance->SetLocaleImpl(language, countryOrRegion, script, selectLanguage, keywordsAndValues);
}

std::string Localization::ComputeScript(const std::string& language, const std::string& region)
{
    icu::Locale locale(language.c_str(), region.c_str());
    UErrorCode status = U_ZERO_ERROR;
    locale.addLikelySubtags(status);
    if (status != U_ZERO_ERROR) {
        return std::string();
    }
    return locale.getScript();
}

void Localization::ParseLocaleTag(
    const std::string& localeTag, std::string& language, std::string& script, std::string& region, bool needAddSubtags)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::Locale locale = icu::Locale::forLanguageTag(icu::StringPiece(localeTag.c_str()), status);
    if (needAddSubtags) {
        locale.addLikelySubtags(status);
    }
    if (status != U_ZERO_ERROR) {
        LOGE("This localeTag is not valid.");
        return;
    }
    language = locale.getLanguage();
    script = locale.getScript();
    region = locale.getCountry();
}

} // namespace OHOS::Ace
