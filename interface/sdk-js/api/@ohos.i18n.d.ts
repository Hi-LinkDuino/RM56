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

/**
 * Provides international settings related APIs.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 7
 */
declare namespace i18n {
/**
 * Obtains the country or region name localized for display on a given locale.
 *
 * @syscap SystemCapability.Global.I18n
 * @param country The locale whose country or region name will be displayed.
 * @param locale The locale used to display the country or region.
 * @param sentenceCase Specifies whether the country or region name is displayed in sentence case.
 * @return Returns the country or region name localized for display on a given locale.
 * @since 7
 */
export function getDisplayCountry(country: string, locale: string, sentenceCase?: boolean): string;

/**
 * Obtains the language name localized for display on a given locale.
 *
 * @syscap SystemCapability.Global.I18n
 * @param language The locale whose language name will be displayed.
 * @param locale The locale used to display the language.
 * @param sentenceCase Specifies whether the language name is displayed in sentence case.
 * @return Returns the language name localized for display on a given locale.
 * @since 7
 */
export function getDisplayLanguage(language: string, locale: string, sentenceCase?: boolean): string;

/**
 * Obtain all languages supported by the system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns all languages supported by the system.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function getSystemLanguages(): Array<string>;

/**
 * Obtain all regions supported by the system in the language.
 *
 * @syscap SystemCapability.Global.I18n
 * @param language The language used to get the list of regions.
 * @return Returns all regions supported by the system in the language.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function getSystemCountries(language: string): Array<string>;

/**
 * Determine whether the current language or region is recommended.
 *
 * @syscap SystemCapability.Global.I18n
 * @param language The language code.
 * @param region The region code.
 * @return Returns whether the current language or region is recommended.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function isSuggested(language: string, region?: string): boolean;

/**
 * Obtain the language currently used by the system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns the language currently used by the system.
 * @since 7
 */
export function getSystemLanguage(): string;

/**
 * Set the language currently used by the system.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param language The language to be used.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function setSystemLanguage(language: string): boolean;

/**
 * Obtain the region currently used by the system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns the region currently used by the system.
 * @since 7
 */
export function getSystemRegion(): string;

/**
 * Set the region currently used by the system.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param region The region to be used.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function setSystemRegion(region: string): boolean;

/**
 * Obtain the locale currently used by the system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns the locale currently used by the system.
 * @since 7
 */
export function getSystemLocale(): string;

/**
 * Set the locale currently used by the system.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param locale The locale to be used.
 * @since 7
 * @systemapi Hide this for inner system use.
 */
export function setSystemLocale(locale: string): boolean;

/**
 * Provides util functions.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface Util {
    /**
     * Convert from unit to to unit and format according to the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @param fromUnit Information of the unit to be converted.
     * @param toUnit Information about the unit to be converted to.
     * @param value Indicates the number to be formatted.
     * @param locale The locale to be used.
     * @param style The style of format.
     * @since 8
     */
    unitConvert(fromUnit: UnitInfo, toUnit: UnitInfo, value: number, locale: string, style?: string): string;
}

/**
 * Provides the options of unit.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface UnitInfo {
    /**
     * Unit name.
     */
    unit: string

    /**
     * The measurement system of the unit.
     */
    measureSystem: string
}

/**
 * Provides the options of PhoneNumberFormat.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface PhoneNumberFormatOptions {
    /**
     * Indicates the type to format phone number.
     */
    type: string;
}

/**
 * Provides the API for formatting phone number strings
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class PhoneNumberFormat {
    /**
     * A constructor used to create a PhoneNumberFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param country Indicates a character string containing the country information for the PhoneNumberFormat object.
     * @param type Indicates the type used to format the phone number, includes: "E164", "RFC3966", "INTERNATIONAL", "NATIONAL".
     * @since 8
     */
    constructor(country: string, options?: PhoneNumberFormatOptions);

    /**
     * Judges whether phone number is valid.
     *
     * @syscap SystemCapability.Global.I18n
     * @param number Indicates the input phone number to be judged.
     * @return Returns a boolean indicates whether the input phone number is valid.
     * @since 8
     */
    isValidNumber(number: string): boolean;

    /**
     * Obtains the formatted phone number strings of number.
     *
     * @syscap SystemCapability.Global.I18n
     * @param number Indicates the input phone number to be formatted.
     * @return Returns the formatted phone number.
     * @since 8
     */
    format(number: string): string;
}

/**
 * Get a Calendar instance specified by locale and type.
 *
 * @syscap SystemCapability.Global.I18n
 * @param locale The locale used to get calendar.
 * @param type If type is not specified, get locale's default Calendar, else get the specified type of Calendar.
 *  such as buddhist, chinese, coptic, ethiopic, hebrew, gregory, indian, islamic_civil, islamic_tbla, islamic_umalqura,
 *  japanese, persion.
 * @since 8
 */
 export function getCalendar(locale: string, type?: string): Calendar;

export class Calendar {
    /**
     * set the date.
     *
     * @syscap SystemCapability.Global.I18n
     * @param date Date object used to set the time and date.
     * @since 8
     */
    setTime(date: Date): void;

    /**
     * set the time.
     *
     * @syscap SystemCapability.Global.I18n
     * @param time Indicates the elapsed milliseconds from 1970.1.1 00:00:00 GMT.
     * @since 8
     */
     setTime(time: number): void;

    /**
     * Set the time
     *
     * @syscap SystemCapability.Global.I18n
     * @param year The year field of the calendar, ranges from 0 to 9999.
     * @param month The month field of the calendar, ranges from 0 to 11.
     * @param date The day field of the calendar, ranges from 1 to 31.
     * @param hour The hour field of the calendar, ranges from 0 to 23.
     * @param minute The minute field of the calendar, ranges from 0 to 59.
     * @param second the second field of the calendar, ranges from 0 to 59.
     * @since 8
     */
    set(year: number, month: number, date:number, hour?: number, minute?: number, second?: number): void;

    /**
     * Set the timezone of this calendar.
     *
     * @syscap SystemCapability.Global.I18n
     * @param timezone The id of a timezone.
     * @since 8
     */
    setTimeZone(timezone: string): void;

    /**
     * Get the timezone id of this calendar instance.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the timezone id of this calendar.
     * @since 8
     */
    getTimeZone(): string;

    /**
     * Get the start day of a week. 1 indicates Sunday, 7 indicates Saturday.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns start day of a week.
     * @since 8
     */
    getFirstDayOfWeek(): number;

    /**
     * Set the start day of a week. 1 indicates Sunday, 7 indicates Saturday.
     *
     * @syscap SystemCapability.Global.I18n
     * @param value Indicates the start day of a week. 1 indicates Sunday, 7 indicates Saturday.
     * @since 8
     */
    setFirstDayOfWeek(value: number): void;

    /**
     * Get the minimal days of a week, which is needed for the first day of a year.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the minimal days of a week.
     * @since 8
     */
    getMinimalDaysInFirstWeek(): number;

    /**
     * Set the minimal days of a week, which is needed for the first week of a year.
     *
     * @syscap SystemCapability.Global.I18n
     * @param value The value to be set.
     * @since 8
     */
    setMinimalDaysInFirstWeek(value: number): void;

    /**
     * Get the associated value with the field.
     *
     * @syscap SystemCapability.Global.I18n
     * @param field Field values such as era, year, month, week_of_year, week_of_month, date, day_of_year, day_of_week
     *  day_of_week_in_month, hour, hour_of_day, minute, second, millisecond, zone_offset, dst_offset, year_woy,
     *  dow_local, extended_year, julian_day, milliseconds_in_day, is_leap_month.
     * @return Return the associated value.
     * @since 8
     */
    get(field: string): number;

    /**
     * Get calendar's name localized for display in the given locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Locale used to get the localized name for this calendar.
     * @return Returns the localized name of this calendar.
     * @since 8
     */
    getDisplayName(locale: string): string;

    /**
     * Returns true if the given date is a weekend day. If the date is not given,
     *  the date object of this calendar is used.
     *
     * @syscap SystemCapability.Global.I18n
     * @param date Date object whose attribute is desired.
     * @return Returns whether the date is a weekend day.
     * @since 8
     */
    isWeekend(date?: Date): boolean;
}

/**
 * Judge whether the locale is RTL locale.
 *
 * @syscap SystemCapability.Global.I18n
 * @param locale The locale to be used.
 * @return Returns true representing the locale is an RTL locale
 *
 * @since 7
 */
export function isRTL(locale: string): boolean;

/**
 * Obtains a BreakIterator object for finding the location of break point in text.
 *
 * @syscap SystemCapability.Global.I18n
 * @param locale the returned BreakIterator will adapt the rule, specified by the locale, to break text.
 * @return Returns a newly constructed BreakIterator object.
 * @since 8
 */
 export function getLineInstance(locale: string): BreakIterator;

/**
 * The BreakIterator class is used for finding the location of break point in text.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class BreakIterator {
    /**
     * Obtains the current position of the BreakIterator instance.
     * 
     * @syscap SystemCapability.Global.I18n
     * @return Returns the current position of the BreakIterator instance.
     * @since 8
     */
    current(): number;

    /**
     * Set the BreakIterator's position to the first break point, the first break point is always the beginning of the
     * processed text.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the index of the first break point.
     * @since 8
     */
    first(): number;

    /**
     * Set the BreakIterator's position to the last break point. the last break point is always the index beyond the
     * last character of the processed text.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the index of the last break point.
     * @since 8
     */
    last(): number;

    /**
     * Set the BreakItertor's position to the nth break point from the current break point.
     *
     * @syscap SystemCapability.Global.I18n
     * @param index indicates the number of break points to advance. If index is not given, n is treated as 1.
     * @return Returns the index of the BreakIterator after moving. If there is not enough break points, returns -1.
     * @since 8
     */
    next(index?: number): number;

    /**
     * Set the BreakItertor's position to the break point preceding the current break point.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the index of the BreakIterator after moving. If there is not enough break points, returns -1.
     * @since 8
     */
    previous(): number;

    /**
     * Set the text to be processed.
     *
     * @syscap SystemCapability.Global.I18n
     * @param text Indicates the text to be processed by the BreakIterator.
     * @since 8
     */
    setLineBreakText(text: string): void;

    /**
     * Set the BreakIterator's position to the first break point following the specified offset.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the index of the BreakIterator after moving. If there is not enough break points, returns -1.
     * @since 8
     */
    following(offset: number): number;

    /**
     * Obtains the text being processed.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the text that is processed by the BreakIterator.
     * @since 8
     */
    getLineBreakText(): string;

    /**
     * Returns true if the position indicated by the offset is a break point, otherwise false. The BreakIterator's
     * position will be set to the position indicated by the offset if it returns true, otherwise the BreakIterator
     * will be moved to the break point following the offset.
     *
     * @syscap SystemCapability.Global.I18n
     * @param offset The offset to be checked.
     * @return Returns true if the offset is a break point.
     * @since 8
     */
    isBoundary(offset: number): boolean;
}

/**
 * Get IndexUtil object.
 *
 * @syscap SystemCapability.Global.I18n
 * @param locale Indicates a character string containing the locale information, including
 *               the language and optionally the script and region, for the NumberFormat object.
 * @return Returns IndexUtil object.
 * @since 8
 */
export function getInstance(locale?:string): IndexUtil;

/**
 * Sequence text can be grouped under the specified area,
 * and grouping index with different lengths can be specified.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class IndexUtil {
    /**
     * Get a list of labels for use as a UI index
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns a list of labels
     * @since 8
     */
    getIndexList(): Array<string>;

    /**
     * Add the index characters from a Locale to the index.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale The locale whose index characters are to be added.
     * @since 8
     */
    addLocale(locale: string): void;

    /**
     * Get corresponding index of the input text.
     *
     * @syscap SystemCapability.Global.I18n
     * @param text input text
     * @since 8
     */
    getIndex(text: string): string;
}

/**
 * Provides the API for accessing unicode character properties, sunch as whether a character is a digit.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class Character {
    /**
     * Determines whether the specified code point is a digit character
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a digit character
     */
    isDigit(char: string): boolean;

    /**
     * Determines if the specified character is a space character or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a space character
     */
    isSpaceChar(char: string): boolean;

    /**
     * Determines if the specified character is a whitespace character
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a whitespace character
     */
    isWhitespace(char: string): boolean;

    /**
     * Determines if the specified character is a RTL character or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a RTL character
     */
    isRTL(char: string): boolean;

    /**
     * Determines if the specified character is a Ideographic character or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a Ideographic character
     */
    isIdeograph(char: string): boolean;

    /**
     * Determines if the specified character is a Letter or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a Letter
     */
    isLetter(char: string): boolean;

    /**
     * Determines if the specified character is a LowerCase character or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a LowerCase character
     */
    isLowerCase(char: string): boolean;

    /**
     * Determines if the specified character is a UpperCase character or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns true if the character is a UpperCase character
     */
    isUpperCase(char: string): boolean;

    /**
     * Get the general category value of the specified character.
     *
     * @syscap SystemCapability.Global.I18n
     * @param char the character to be tested
     * @return Returns the general category of the specified character.
     */
    getType(char: string): string;
}

/**
 * check out whether system is 24-hour system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns a boolean represent whether system is 24-hour system.
 * @since 7
 */
 export function is24HourClock(): boolean;

/**
 * set 24-hour system.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param option represent the boolean to be set.
 * @return Returns a boolean represent whether setting 24-hour system success.
 * @since 7
 */
  export function set24HourClock(option: boolean): boolean;

/**
 * Add one language to preferred language List.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param language the language to be added.
 * @param index the position of preferred language list to be inserted. 
 * @return Returns a boolean represent whether language added success.
 * @since 8
 */
export function addPreferredLanguage(language: string, index?: number): boolean;

/**
 * Remove one language from preferred language list.
 *
 * @permission ohos.permission.UPDATE_CONFIGURATION
 * @syscap SystemCapability.Global.I18n
 * @param index the position of removed language in preferred language list.
 * @return Returns a boolean represent whether removed success.
 * @since 8
 */
export function removePreferredLanguage(index: number): boolean;

/**
 * Access the system preferred language list.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns a string Array represent the preferred language list.
 * @since 8
 */
export function getPreferredLanguageList(): Array<string>;

/**
 * Get the first preferred language of system.
 *
 * @syscap SystemCapability.Global.I18n
 * @return Returns a string represent the first preferred language of system.
 * @since 8
 */
export function getFirstPreferredLanguage(): string;

/**
 * Get the default TimeZone object or the TimeZone object corresponds to zoneID.
 * 
 * @syscap SystemCapability.Global.I18n
 * @param zoneID TimeZone ID used to create TimeZone Object.
 * @return Returns a TimeZone object corresponds to zoneID.
 * @since 7
 */
export function getTimeZone(zoneID?: string): TimeZone;

/**
 * Provides the API for accessing TimeZone name, rawOffset and offset information.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 7
 */
export class TimeZone {
    /**
     * Get the id of the TimeZone object.
     * 
     * @syscap SystemCapability.Global.I18n
     * @return Returns a string represents the timezone id.
     * @since 7
     */
    getID(): string;

    /**
     * Get the displayName of the TimeZone Object under the locale.
     * 
     * @syscap SystemCapability.Global.I18n
     * @param locale the locale tag use to display timezone object's name.
     * @param isDST wether conside daylight saving time when display timezone object's name.
     * @return Returns a string represents the display name.
     * @since 7
     */
    getDisplayName(locale?: string, isDST?: boolean): string;

    /**
     * Get the raw offset of the TimeZone object.
     * 
     * @syscap SystemCapability.Global.I18n
     * @return Returns a number represents the raw offset.
     * @since 7
     */
    getRawOffset(): number;

    /**
     * Get the offset of the TimeZone object.
     * 
     * @syscap SystemCapability.Global.I18n
     * @date Indicates a date use to compute offset.
     * @return Returns a number represents the offset with date.
     * @since 7
     */
    getOffset(date?: number): number;
}
}
export default i18n;