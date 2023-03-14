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
#include "date_time_format_module.h"
#if (FEATURE_DATE_FORMAT == 1)
#include <cstring>
#include "ace_log.h"
#include "locale_util.h"
#include "keys.h"
#include "key_parser.h"
#include "string_util.h"

namespace OHOS {
namespace ACELite {
using namespace I18N;
jerry_object_native_info_t DateTimeFormatModule::GC_CALLBACK = {.free_cb = DateTimeFormatModule::DeleteDateFormat};

DateTimeFormatModule::DateTimeFormatModule()
    : dateFormat_(nullptr),
      info_(nullptr),
      timePattern_(I18N::AvailableDateTimeFormatPattern::SHORT),
      datePattern_(I18N::AvailableDateTimeFormatPattern::SHORT),
      weekStyle_(StyleState::UNKNOWN),
      monthStyle_(StyleState::UNKNOWN),
      numArray_(nullptr),
      isSetTime_(false),
      isSetDate_(false)
{
    if (memset_s(digitArray_, TIME_NUMBER_LEN, 0, TIME_NUMBER_LEN) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "init digit array failed");
    }
}

void DateTimeFormatModule::Init(jerry_value_t intlHandle)
{
    jerry_value_t constructor = jerry_create_external_function(CreateDateTimeFormat);
    jerry_value_t format = jerry_create_external_function(Format);
    jerry_value_t baseObj = jerry_create_object();
    JerrySetNamedProperty(baseObj, "format", format);
    JerrySetNamedProperty(constructor, "prototype", baseObj);
    JerrySetNamedProperty(intlHandle, "DateTimeFormat", constructor);
    ReleaseJerryValue(constructor, format, baseObj, VA_ARG_END_FLAG);
}

DateTimeFormatModule::~DateTimeFormatModule()
{
    ACE_DELETE(dateFormat_);
    ACE_DELETE(info_);
    ReleaseNumArray();
}

void DateTimeFormatModule::ReleaseNumArray()
{
    if (numArray_ != nullptr) {
        for (int i = 0; i < MAX_NUM_LEN; i++) {
            ACE_FREE(numArray_[i]);
        }
        ace_free(numArray_);
        numArray_ = nullptr;
    }
}

bool DateTimeFormatModule::InitNumArray(LocaleInfo info)
{
    ReleaseNumArray();
    int status = 0;
    NumberFormat numFormat(info, status);
    if (status == I18nStatus::IERROR) {
        HILOG_ERROR(HILOG_MODULE_ACE, "init number format failed");
        return false;
    }
    numArray_ = reinterpret_cast<char **>(ace_malloc(MAX_NUM_LEN * sizeof(char *)));
    if (numArray_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc number array failed");
        return false;
    }
    // get the number value 0-9 in specified locale info
    for (uint8_t i = 0; i < MAX_NUM_LEN; i++) {
        int numStatus = 0;
        std::string result = numFormat.Format(i, numStatus);
        if ((numStatus == I18nStatus::IERROR) || (result.empty())) {
            HILOG_ERROR(HILOG_MODULE_ACE, "init array %{public}d failed", i);
            ReleaseNumArray();
            return false;
        }
        numArray_[i] = StringUtil::Copy(result.c_str());
    }
    return true;
}

jerry_value_t DateTimeFormatModule::CreateDateTimeFormat(const jerry_value_t func,
                                                         const jerry_value_t context,
                                                         const jerry_value_t args[],
                                                         const jerry_length_t argsNum)
{
    if (!jerry_value_is_constructor(func)) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("use new to create number format"));
    }
    DateTimeFormatModule *dateModel = new DateTimeFormatModule();
    if (dateModel == nullptr) {
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("create date format model failed"));
    }
    char *value = nullptr;
    if (argsNum >= 1) {
        value = MallocStringOf(args[0]);
    }
    dateModel->info_ = LocaleUtil::GetLocaleInfo(value);
    ACE_FREE(value);
    if (dateModel->info_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "create LocaleInfo failed");
        delete dateModel;
        dateModel = nullptr;
        return jerry_create_error(JERRY_ERROR_COMMON, reinterpret_cast<const jerry_char_t *>("lack of memory"));
    }
    // set the default format pattern year(numeric)-month(numeric)-day(numeric)
    dateModel->dateFormat_ = new DateTimeFormat(AvailableDateTimeFormatPattern::SHORT, *dateModel->info_);
    if ((dateModel->dateFormat_ == nullptr) || (!dateModel->InitNumArray(*dateModel->info_))) {
        delete dateModel;
        dateModel = nullptr;
        return jerry_create_error(JERRY_ERROR_EVAL,
                                  reinterpret_cast<const jerry_char_t *>("create dateTimeFormat failed"));
    }
    if (argsNum > 1) {
        // deal the year month day style
        const char *dateRes = dateModel->SetDateStyle(args[1]);
        // deal the hour minute second style
        const char *timeRes = dateModel->SetTimeStyle(args[1]);
        // if dateRes or timeRes is not nullptr, the date style or time style is invalid
        if ((dateRes != nullptr) || (timeRes != nullptr)) {
            delete dateModel;
            dateModel = nullptr;
            const char *result = (dateRes != nullptr) ? dateRes : timeRes;
            return (result == nullptr)
                       ? jerry_create_null()
                       : jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>(result));
        }
    }
    jerry_set_object_native_pointer(context, dateModel, &GC_CALLBACK);
    return UNDEFINED;
}

void DateTimeFormatModule::ConvertLocalToGMT(time_t &time) const
{
    struct tm gmTime = {0};
    struct tm local = {0};
    gmtime_r(&time, &gmTime);
    localtime_r(&time, &local);
    time += mktime(&local) - mktime(&gmTime);
}

jerry_value_t DateTimeFormatModule::Format(const jerry_value_t func,
                                           const jerry_value_t context,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum)
{
    if (!jerry_is_feature_enabled(JERRY_FEATURE_DATE)) {
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("the date is not enabled"));
    }
    if (argsNum < 1) {
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("date is invalid"));
    }
    DateTimeFormatModule *formatter = nullptr;
    if (!(jerry_get_object_native_pointer(context, reinterpret_cast<void **>(&formatter), &GC_CALLBACK)) ||
        (formatter == nullptr) || (formatter->dateFormat_ == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get format object failed");
        return UNDEFINED;
    }
    const uint16_t msToSec = 1000;
    time_t dateTime = static_cast<time_t>(formatter->GetTimeVal(args[0], "getTime") / msToSec);
    formatter->ConvertLocalToGMT(dateTime);
    if (formatter->isSetDate_ || formatter->isSetTime_) {
        uint8_t maxSize = 128;
        char *res = reinterpret_cast<char *>(ace_malloc(maxSize));
        if (res == nullptr) {
            return UNDEFINED;
        }
        res[0] = '\0';
        uint8_t start = 0;
        if (formatter->isSetDate_) {
            formatter->FormatDate(dateTime, res, maxSize, start);
        }
        if (formatter->isSetTime_) {
            formatter->FormatTime(dateTime, res, maxSize, start);
        }
        jerry_value_t resultProp = jerry_create_string(reinterpret_cast<jerry_char_t *>(res));
        ace_free(res);
        res = nullptr;
        return resultProp;
    }
    // the style group is not support, format the lonely support style week or month
    if (formatter->weekStyle_ != StyleState::UNKNOWN) {
        int8_t weekIndex = static_cast<int8_t>(formatter->GetTimeVal(args[0], "getDay"));
        DateTimeDataType type = (formatter->weekStyle_ == StyleState::LONG) ? FORMAT_WIDE : FORMAT_ABBR;
        std::string weekName = formatter->dateFormat_->GetWeekName(weekIndex, type);
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(weekName.c_str()));
    } else if (formatter->monthStyle_ != StyleState::UNKNOWN) {
        return formatter->GetMonthVal(args[0]);
    }
    // if the style is valid, the style pattern is not support, format the date style as default style
    std::string result, timeZone;
    I18nStatus status = I18nStatus::ISUCCESS;
    formatter->dateFormat_->Format(dateTime, timeZone, result, status);
    if (status == I18nStatus::IERROR) {
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("format date failed"));
    }
    return jerry_create_string(reinterpret_cast<const jerry_char_t *>(result.c_str()));
}

jerry_value_t DateTimeFormatModule::GetMonthVal(jerry_value_t time) const
{
    int month = static_cast<int>(GetTimeVal(time, "getMonth"));
    // format the long style ans short month style
    if ((monthStyle_ == StyleState::LONG) || (monthStyle_ == StyleState::SHORT)) {
        DateTimeDataType type = (monthStyle_ == StyleState::LONG) ? FORMAT_WIDE : FORMAT_ABBR;
        std::string monthName = dateFormat_->GetMonthName(month, type);
        return jerry_create_string(reinterpret_cast<const jerry_char_t *>(monthName.c_str()));
    }
    const uint8_t monthIndex = 1;
    const uint8_t maxMonthLen = 20;
    uint8_t start = 0;
    char *result = reinterpret_cast<char *>(ace_malloc(maxMonthLen));
    if (result == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc month str failed");
        return jerry_create_error(JERRY_ERROR_EVAL, reinterpret_cast<const jerry_char_t *>("memory is not enough"));
    }
    std::string date = dateFormat_->GetMonthName(month, DateTimeDataType::STANDALONE_ABBR);
    // format the number month to 2-digit or not through the month digit flag
    jerry_value_t resultProp = UNDEFINED;
    if (FormatDigit(date.c_str(), result, maxMonthLen, start, digitArray_[monthIndex]) > 0) {
        resultProp = jerry_create_string(reinterpret_cast<jerry_char_t *>(result));
    }
    ace_free(result);
    result = nullptr;
    return resultProp;
}

uint8_t DateTimeFormatModule::GetNumInDate(const char *date) const
{
    if (date == nullptr) {
        return 0;
    }
    uint8_t len = 0;
    int16_t dateLen = strlen(date);
    bool check = true;
    for (int16_t index = 0; index < dateLen; index++) {
        int16_t numEnd = GetNumberEnd(date, index);
        if (numEnd >= 0) {
            if (check) {
                len++;
                check = false;
            }
            index = numEnd; // check the number from the next character
        } else {
            check = true;
        }
    }
    return len;
}

void DateTimeFormatModule::FormatDate(time_t time, char *res, const uint8_t resSize, uint8_t &start)
{
    if (dateFormat_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "dateFormat is nullptr");
        return;
    }
    std::string result, timeZone, check;
    I18nStatus status = I18nStatus::ISUCCESS;
    dateFormat_->ApplyPattern(datePattern_);
    dateFormat_->Format(time, timeZone, result, status);
    if (status == I18nStatus::IERROR) {
        return;
    }
    bool isMonthFirst = false;
    // if month is numeric, format the date to full year-month-day, if the first digit is number, the format string
    // is day-month-year or year-month-day, else month-day-year
    const char *dateStr = result.c_str();
    const uint8_t numMaxInDate = 3;
    uint8_t numInDate = GetNumInDate(dateStr);
    if (numInDate == numMaxInDate) {
        dateFormat_->ApplyPattern(AvailableDateTimeFormatPattern::YEAR_WIDE_MONTH_DAY);
        dateFormat_->Format(time, timeZone, check, status);
        if (status == I18nStatus::IERROR) {
            return;
        }
        isMonthFirst = (GetNumberEnd(check.c_str(), 0) < 0);
    }
    const uint8_t dayStyleIndex = 2;   // the day number style index
    const uint8_t monthStyleIndex = 1; // the month style index
    uint8_t firstFormatIndex = dayStyleIndex;
    int16_t formatIndex = 0;
    if (numInDate == numMaxInDate) {
        uint8_t secondFormatIndex = monthStyleIndex;
        if (isMonthFirst) {
            // set the format order month-day-year
            firstFormatIndex = monthStyleIndex;
            secondFormatIndex = dayStyleIndex;
        }
        formatIndex = FormatDigit(dateStr, res, resSize, start, digitArray_[firstFormatIndex]);
        if (formatIndex < 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "format the first number in date str failed");
            return;
        }
        formatIndex =
            FormatDigit(dateStr + formatIndex, res, resSize, start, digitArray_[secondFormatIndex]) + formatIndex;
    } else {
        // format the day digit or year digit
        formatIndex = FormatDigit(dateStr, res, resSize, start, digitArray_[dayStyleIndex]);
    }
    if (formatIndex < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "format date string failed");
        return;
    }
    // format the year
    if (FormatDigit(dateStr + formatIndex, res, resSize, start, digitArray_[firstFormatIndex]) < 0) {
        res[0] = '\0';
    }
}

void DateTimeFormatModule::FormatTime(time_t time, char *res, const uint8_t resSize, uint8_t &start)
{
    if (res == nullptr) {
        return;
    }
    if (start != 0) {
        res[start] = ' ';
        start++;
    }
    std::string result, timeZone;
    I18nStatus status = I18nStatus::ISUCCESS;
    dateFormat_->ApplyPattern(timePattern_);
    dateFormat_->Format(time, timeZone, result, status);
    if (status == I18nStatus::IERROR) {
        return;
    }
    const uint8_t hourIndex = 3;
    const uint8_t minuteIndex = 4;
    const uint8_t secondIndex = 5;
    const char *timeStr = result.c_str();
    int16_t curIndex = 0;
    curIndex = FormatDigit(timeStr, res, resSize, start, digitArray_[hourIndex]);
    if (curIndex < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "format hour style failed");
        return;
    }
    curIndex = FormatDigit(timeStr + curIndex, res, resSize, start, digitArray_[minuteIndex]) + curIndex;
    if (curIndex < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "format minute style failed");
        return;
    }
    if (FormatDigit(timeStr + curIndex, res, resSize, start, digitArray_[secondIndex]) < 0) {
        res[0] = '\0';
    }
}

int16_t DateTimeFormatModule::FormatDigit(const char *time,
                                          char *res,
                                          const uint8_t resSize,
                                          uint8_t &start,
                                          bool is2Digit) const
{
    if ((time == nullptr) || (strlen(time) == 0)) {
        return 0;
    }
    int16_t index = GetNum(time);
    if (index >= 0) {
        if (strncpy_s(res + start, resSize - start + 1, time, index + 1) != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "copy string before number failed");
            return -1;
        }
        start = start + index + 1;
    }
    index = (index < 0) ? 0 : (index + 1);
    int16_t num1 = GetNumberEnd(time, index);
    if (num1 < 0) {
        return -1;
    }
    int16_t num2 = GetNumberEnd(time, num1 + 1);
    int16_t numEnd = num1;
    if (num2 < 0) {
        if (is2Digit && (numArray_[0] != nullptr)) {
            uint8_t zeroLen = strlen(numArray_[0]);
            if (strncpy_s(res + start, resSize - start + 1, numArray_[0], zeroLen)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "add zero failed");
                return -1;
            }
            start = start + zeroLen;
        }
    } else {
        int num3 = GetNumberEnd(time, num2 + 1);
        // format the 4 digit year
        if (num3 >= 0) {
            // the year style is 2-digit
            if (digitArray_[0]) {
                index = num2 + 1;
            }
            numEnd = GetNumberEnd(time, num3 + 1);
        } else {
            numEnd = num2;
        }
    }
    numEnd = GetNum(time + numEnd + 1) + numEnd + 1;
    if (numEnd >= index) {
        if (strncpy_s(res + start, resSize - start + 1, time + index, numEnd - index + 1) != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "copy string failed");
            return -1;
        }
        start = start + numEnd - index + 1;
    }
    res[start] = '\0';
    return numEnd + 1;
}

int16_t DateTimeFormatModule::GetNum(const char *format) const
{
    uint8_t len = strlen(format);
    int16_t res = -1;
    for (uint8_t index = 0; index < len; index++) {
        int16_t numIndex = GetNumberEnd(format, index);
        if (numIndex >= 0) {
            break;
        } else {
            res = index;
        }
    }
    return res;
}

int16_t DateTimeFormatModule::GetNumberEnd(const char *compare, uint8_t start) const
{
    for (uint8_t index = 0; index < MAX_NUM_LEN; index++) {
        const char *str = compare + start;
        if (StringUtil::StartsWith(str, numArray_[index])) {
            return start + strlen(numArray_[index]) - 1;
        }
    }
    return -1;
}

double DateTimeFormatModule::GetTimeVal(jerry_value_t time, const char *funcName) const
{
    jerry_value_t funcProp = jerryx_get_property_str(time, funcName);
    if (IS_UNDEFINED(funcProp)) {
        return 0;
    }
    jerry_value_t result = CallJSFunction(funcProp, time, nullptr, 0);
    double timeVal = jerry_get_number_value(result);
    ReleaseJerryValue(funcProp, result, VA_ARG_END_FLAG);
    return timeVal;
}

void DateTimeFormatModule::DeleteDateFormat(void *pointer)
{
    DateTimeFormatModule *formatter = reinterpret_cast<DateTimeFormatModule *>(pointer);
    if (formatter != nullptr) {
        delete formatter;
        formatter = nullptr;
    }
}

const char *DateTimeFormatModule::SetDateStyle(jerry_value_t style)
{
    weekStyle_ = GetWeekdayStyle(style);
    if (weekStyle_ == StyleState::ERROR) {
        return "the weekday style value is out of range";
    }
    const uint8_t yearIndex = 0;
    StyleState yearStyle = GetStyle(style, "year", yearIndex);
    if (yearStyle == StyleState::ERROR) {
        return "the year style value is out of range";
    }
    monthStyle_ = GetMonthStyle(style);
    if (monthStyle_ == StyleState::ERROR) {
        return "the month style value is out of range";
    }
    const uint8_t dayIndex = 2;
    StyleState dayStyle = GetStyle(style, "day", dayIndex);
    if (dayStyle == StyleState::ERROR) {
        return "the day style value is out of range";
    }
    isSetDate_ = SetDatePattern(weekStyle_, yearStyle, monthStyle_, dayStyle);
    return nullptr;
}

DateTimeFormatModule::StyleState DateTimeFormatModule::GetWeekdayStyle(jerry_value_t style) const
{
    if (!jerryx_has_property_str(style, "weekday")) {
        return StyleState::UNKNOWN;
    }
    StyleState weekdayStyle = StyleState::UNKNOWN;
    jerry_value_t weekProp = jerryx_get_property_str(style, "weekday");
    uint16_t len = 0;
    char *weekStyle = MallocStringOf(weekProp, &len);
    jerry_release_value(weekProp);
    uint16_t weekId = KeyParser::ParseKeyId(weekStyle, len);
    ACE_FREE(weekStyle);
    if (!KeyParser::IsKeyValid(weekId)) {
        return StyleState::ERROR;
    }
    switch (weekId) {
        case K_SHORT: {
            weekdayStyle = StyleState::SHORT;
            break;
        }
        case K_LONG: {
            weekdayStyle = StyleState::LONG;
            break;
        }
        default: {
            weekdayStyle = StyleState::ERROR;
            break;
        }
    }
    return weekdayStyle;
}

DateTimeFormatModule::StyleState DateTimeFormatModule::GetMonthStyle(jerry_value_t style)
{
    if (!jerryx_has_property_str(style, "month")) {
        return StyleState::UNKNOWN;
    }
    jerry_value_t monthProp = jerryx_get_property_str(style, "month");
    uint16_t len = 0;
    char *monthStyle = MallocStringOf(monthProp, &len);
    jerry_release_value(monthProp);
    uint16_t monthId = KeyParser::ParseKeyId(monthStyle, len);
    ACE_FREE(monthStyle);
    if (!KeyParser::IsKeyValid(monthId)) {
        return StyleState::ERROR;
    }
    StyleState monthStyleState = StyleState::UNKNOWN;
    switch (monthId) {
        case K_LONG: {
            monthStyleState = StyleState::LONG;
            break;
        }
        case K_SHORT: {
            monthStyleState = StyleState::SHORT;
            break;
        }
        case K_NUMERIC: {
            monthStyleState = StyleState::NUMERIC;
            break;
        }
        case K_DIGIT2: {
            const uint8_t monthIndex = 1;
            digitArray_[monthIndex] = true;
            monthStyleState = StyleState::NUMERIC;
            break;
        }
        default: {
            monthStyleState = StyleState::ERROR;
            break;
        }
    }
    return monthStyleState;
}

DateTimeFormatModule::StyleState DateTimeFormatModule::GetStyle(jerry_value_t style,
                                                                const char *attrName,
                                                                const uint8_t index)
{
    if (!jerryx_has_property_str(style, attrName)) {
        return StyleState::UNKNOWN;
    }
    jerry_value_t attrProp = jerryx_get_property_str(style, attrName);
    uint16_t len = 0;
    char *attrVal = MallocStringOf(attrProp, &len);
    jerry_release_value(attrProp);
    uint16_t attrId = KeyParser::ParseKeyId(attrVal, len);
    ACE_FREE(attrVal);
    if (!KeyParser::IsKeyValid(attrId)) {
        return StyleState::ERROR;
    }
    StyleState styleState = StyleState::UNKNOWN;
    switch (attrId) {
        case K_NUMERIC: {
            styleState = StyleState::NUMERIC;
            break;
        }
        case K_DIGIT2: {
            digitArray_[index] = true;
            styleState = StyleState::NUMERIC;
            break;
        }
        default: {
            styleState = StyleState::ERROR;
            break;
        }
    }
    return styleState;
}

const char *DateTimeFormatModule::SetTimeStyle(jerry_value_t style)
{
    const uint8_t hourIndex = 3;
    StyleState hourStyle = GetStyle(style, "hour", hourIndex);
    if (hourStyle == StyleState::ERROR) {
        return "the hour style value is out of range";
    }
    const uint8_t minuteIndex = 4;
    StyleState minuteStyle = GetStyle(style, "minute", minuteIndex);
    if (minuteStyle == StyleState::ERROR) {
        return "the minute style value is out of range";
    }
    const uint8_t secondIndex = 5;
    StyleState secondStyle = GetStyle(style, "second", secondIndex);
    if (secondStyle == StyleState::ERROR) {
        return "the second style value is out of range";
    }
    // the hour12 style is invalid when hour style is not set
    if (hourStyle != StyleState::UNKNOWN) {
        if (jerryx_has_property_str(style, "hour12")) {
            jerry_value_t hour12Style = jerryx_get_property_str(style, "hour12");
            bool isHour12 = BoolOf(hour12Style);
            jerry_release_value(hour12Style);
            hourStyle = isHour12 ? StyleState::HOUR12 : StyleState::HOUR24;
        } else {
            hourStyle = StyleState::HOUR;
        }
    }
    // find the most suitable mode for hour minute second style
    isSetTime_ = GetTimePattern(hourStyle, minuteStyle, secondStyle);
    if ((!isSetTime_) && (weekStyle_ == StyleState::UNKNOWN) && (monthStyle_ == StyleState::UNKNOWN)) {
        isSetTime_ = true;
        timePattern_ = AvailableDateTimeFormatPattern::SHORT;
    }
    return nullptr;
}

bool DateTimeFormatModule::SetMonthPattern(StyleState monthStyle,
                                           AvailableDateTimeFormatPattern widePattern,
                                           AvailableDateTimeFormatPattern shortPattern,
                                           AvailableDateTimeFormatPattern numberPattern)
{
    bool result = true;
    switch (monthStyle) {
        case StyleState::LONG: {
            datePattern_ = widePattern;
            break;
        }
        case StyleState::SHORT: {
            datePattern_ = shortPattern;
            break;
        }
        case StyleState::NUMERIC: {
            datePattern_ = numberPattern;
            break;
        }
        default: {
            result = false;
            break;
        }
    }
    return result;
}

bool DateTimeFormatModule::SetDatePattern(StyleState weekdayStyle,
                                          StyleState yearStyle,
                                          StyleState monthStyle,
                                          StyleState dayStyle)
{
    // the pattern support is (weekday)-year-month-day
    if ((yearStyle == StyleState::UNKNOWN) || (dayStyle == StyleState::UNKNOWN) ||
        (monthStyle == StyleState::UNKNOWN)) {
        return false;
    }
    bool result = true;
    switch (weekdayStyle) {
        case StyleState::LONG: {
            result = SetMonthPattern(monthStyle, AvailableDateTimeFormatPattern::FULL,
                                     AvailableDateTimeFormatPattern::YEAR_ABBR_MONTH_WIDE_WEEKDAY_DAY,
                                     AvailableDateTimeFormatPattern::YEAR_SHORT_MONTH_WIDE_WEEKDAY_DAY);
            break;
        }
        case StyleState::SHORT: {
            result = SetMonthPattern(monthStyle, AvailableDateTimeFormatPattern::YEAR_WIDE_MONTH_ABBR_WEEKDAY_DAY,
                                     AvailableDateTimeFormatPattern::YEAR_ABBR_MONTH_ABBR_WEEKDAY_DAY,
                                     AvailableDateTimeFormatPattern::YEAR_SHORT_MONTH_ABBR_WEEKDAY_DAY);
            break;
        }
        default: {
            result = SetMonthPattern(monthStyle, AvailableDateTimeFormatPattern::YEAR_WIDE_MONTH_DAY,
                                     AvailableDateTimeFormatPattern::MEDIUM, AvailableDateTimeFormatPattern::SHORT);
            break;
        }
    }
    return result;
}

bool DateTimeFormatModule::GetTimePattern(StyleState hourStyle, StyleState minuteStyle, StyleState secondStyle)
{
    // the support pattern is hour-minute-(second)
    if ((hourStyle == StyleState::UNKNOWN) || (minuteStyle == StyleState::UNKNOWN)) {
        return false;
    }
    bool result = true;
    switch (hourStyle) {
        case StyleState::HOUR12: {
            timePattern_ = (secondStyle == StyleState::UNKNOWN) ? AvailableDateTimeFormatPattern::HOUR12_MINUTE
                                                                : AvailableDateTimeFormatPattern::HOUR12_MINUTE_SECOND;
            break;
        }
        case StyleState::HOUR24: {
            timePattern_ = (secondStyle == StyleState::UNKNOWN) ? AvailableDateTimeFormatPattern::HOUR24_MINUTE
                                                                : AvailableDateTimeFormatPattern::HOUR24_MINUTE_SECOND;
            break;
        }
        case StyleState::HOUR: {
            timePattern_ =
                (secondStyle == StyleState::UNKNOWN) ? AvailableDateTimeFormatPattern::HOUR_MINUTE : HOUR_MINUTE_SECOND;
            break;
        }
        default: {
            result = false;
            break;
        }
    }
    return result;
}
} // namespace ACELite
} // namespace OHOS
#endif
