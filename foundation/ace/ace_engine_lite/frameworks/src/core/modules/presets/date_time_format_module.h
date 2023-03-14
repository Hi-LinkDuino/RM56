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

#ifndef OHOS_ACELITE_DATE_TIME_FORMAT_MODULE_H
#define OHOS_ACELITE_DATE_TIME_FORMAT_MODULE_H

#include "acelite_config.h"
#if (FEATURE_DATE_FORMAT == 1)
#include "js_fwk_common.h"
#include "date_time_format.h"
namespace OHOS {
namespace ACELite {
class DateTimeFormatModule final : public MemoryHeap {
public:
    /**
     * @brief register dateTimeFormat attribute and register the format method to dateTimeFormat
     */
    static void Init(jerry_value_t intlHandle);

    static void DeleteDateFormat(void *pointer);

    static jerry_object_native_info_t GC_CALLBACK;

private:
    constexpr static uint8_t TIME_NUMBER_LEN = 6; // the num of number in format string
    constexpr static uint8_t MAX_NUM_LEN = 10;
    DateTimeFormatModule();

    ~DateTimeFormatModule();

    /**
     * @brief get the locale number form 0-9
     * @param info the language and region object
     * @return get the number digit result
     */
    bool InitNumArray(I18N::LocaleInfo info);

    enum StyleState : uint8_t {
        SHORT,
        NUMERIC,
        LONG,
        HOUR12,
        HOUR24,
        HOUR,
        UNKNOWN, // the style is not set
        ERROR // the style value is invalid
    };

    static jerry_value_t CreateDateTimeFormat(const jerry_value_t func,
                                              const jerry_value_t context,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t Format(const jerry_value_t func,
                                const jerry_value_t context,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    const char *SetDateStyle(jerry_value_t style);

    const char *SetTimeStyle(jerry_value_t style);

    /**
     * @brief set the date time pattern
     */
    bool SetDatePattern(StyleState weekdayStyle,
                        StyleState yearStyle,
                        StyleState monthStyle,
                        StyleState dayStyle);

    /**
     * @brief get the hour minute second style pattern
     */
    bool GetTimePattern(StyleState hourStyle,
                        StyleState minuteStyle,
                        StyleState secondStyle);

    /**
     * @brief check the specified position in coampared string is number or not
     * @param comapre: the compare string
     * @param start: the specified position
     * @return if the specified string is digit, return the last position of single number,
     * else return -1
     */
    int16_t GetNumberEnd(const char *compare, uint8_t start) const;

    StyleState GetWeekdayStyle(jerry_value_t style) const;

    StyleState GetStyle(jerry_value_t style, const char *attrName, const uint8_t index);

    StyleState GetMonthStyle(jerry_value_t style);

    /**
     * @brief execute the function in date object
     * @param time : the js date object
     *        funcName : the name of executed function
     * @return the number value of execute the function
     */
    double GetTimeVal(jerry_value_t time, const char *funcName) const;

    /**
     * @brief format the time to date through locale info
     * @param time: the time need to format
     *        res : store the format date string
     *        resSize : the size of format result
     *        start: the start storage position in res
     */
    void FormatDate(time_t time, char *res, const uint8_t resSize, uint8_t &start);

    /**
     * @brief get the time zone, and convert local time to GMT time
     * @param time the timestamp of local time
     */
    void ConvertLocalToGMT(time_t &time) const;

    /**
     * @brief format the time to hour-minute-second through locale info
     * @param date: the time need to format
     *        res : store the format date string
     *        resSize : the length of format result
     *        start: the start storage position in res
     */
    void FormatTime(time_t time, char *res, const uint8_t resSize, uint8_t &start);

    /**
     * @brief get the first number in string
     * @param the string to get the first number
     * @return if string starts with number, return the end index of first number in string
     * else return -1
     */
    int16_t GetNum(const char *format) const;

    /**
     * @brief Get the month format value
     * @param time the js time which need to format
     * @return the format js value
     */
    jerry_value_t GetMonthVal(jerry_value_t time) const;

    /**
     * @brief format the first number to 2 digit
     * @param time the string need to format to 2-digit or remain
     *        res the string to store the format result
     *        start the start position to store format result
     *        is2Digit if true, format the first number to 2-digit
     *                 if false remain the first digit
     * @return the next number index
     */
    int16_t FormatDigit(const char *time, char *res, const uint8_t resSize, uint8_t &start, bool is2Digit) const;

    void ReleaseNumArray();

    bool SetMonthPattern(StyleState monthStyle,
                         I18N::AvailableDateTimeFormatPattern widePattern,
                         I18N::AvailableDateTimeFormatPattern shortPattern,
                         I18N::AvailableDateTimeFormatPattern numberPattern);

    uint8_t GetNumInDate(const char *date) const;
    I18N::DateTimeFormat *dateFormat_;
    I18N::LocaleInfo *info_;
    I18N::AvailableDateTimeFormatPattern timePattern_;
    I18N::AvailableDateTimeFormatPattern datePattern_;
    StyleState weekStyle_;
    StyleState monthStyle_;
    char **numArray_;
    bool isSetTime_;
    bool isSetDate_;
    bool digitArray_[TIME_NUMBER_LEN];
};
}
}
#endif
#endif // DATE_TIME_FORMAT_MODEL_H
