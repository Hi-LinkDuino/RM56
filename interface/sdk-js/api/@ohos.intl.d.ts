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
 * Provides internationalization related APIs.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
declare namespace intl {
/**
 * Provides the options of Locale.
 *
 * @since 6
 * @syscap SystemCapability.Global.I18n
 */
export interface LocaleOptions {
    /**
     * Indicates the calendar.
     *
     * @since 6
     */
    calendar: string;

    /**
     * Indicates the collation.
     *
     * @since 6
     */
    collation: string;
    
    /**
     * Indicates the hourCycle.
     *
     * @since 6
     */
    hourCycle: string;
    
    /**
     * Indicates the numberingSystem.
     *
     * @since 6
     */
    numberingSystem: string;
    
    /**
     * Indicates the numeric.
     *
     * @since 6
     */
    numeric: boolean;
    
    /**
     * Indicates the caseFirst.
     *
     * @since 6
     */
    caseFirst: string;
}

/**
 * Provides APIs for obtaining locale information.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
export class Locale {
    /**
     * A constructor used to create a Locale object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
   constructor();

    /**
     * A constructor used to create a Locale object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region.
     * @param options Indicates Locale option object use to initialize the Locale object.
     * @since 6
     */
   constructor(locale: string, options?: LocaleOptions);

    /**
     * Indicates the language of the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    language: string

    /**
     * Indicates the script of the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    script: string

    /**
     * Indicates the region of the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    region: string

    /**
     * Indicates the basic locale information, which is returned as a substring of
     * a complete locale string.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    baseName: string

    /**
     * Indicates the case first style of the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    caseFirst: string

    /**
     * Indicates the calendar.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    calendar: string

    /**
     * Indicates the collation.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    collation: string

    /**
     * Indicates the hour cycle.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    hourCycle:  string

    /**
     * Indicates the numbering system.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    numberingSystem: string

    /**
     * Indicates whether it is numeric.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    numeric: boolean

    /**
     * Convert the locale information to string.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns locale information in string form.
     * @since 6
     */
    toString(): string;

    /**
     * Maximize the locale's base information.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns maximized locale.
     * @since 6
     */
    maximize(): Locale;

    /**
     * Minimize the locale's base information.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns minimized locale.
     * @since 6
     */
    minimize(): Locale;
}

/**
 * Provides the options of date time format.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
export interface DateTimeOptions {
    /**
     * Indicates the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    locale: string

    /**
     * Indicates the date style.
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    dateStyle: string

    /**
     * Indicates the time style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    timeStyle: string

    /**
     * Indicates the hour cycle.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    hourCycle: string

    /**
     * Indicates the timezone.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    timeZone: string

    /**
     * Indicates the numbering system.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    numberingSystem: string

    /**
     * Indicates whether is 12 hour or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    hour12: boolean

    /**
     * Indicates the weekday style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    weekday: string

    /**
     * Indicates the era style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    era: string

    /**
     * Indicates the year style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    year: string

    /**
     * Indicates the month style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    month: string

    /**
     * Indicates the day style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    day: string

    /**
     * Indicates the hour style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    hour: string

    /**
     * Indicates the minute style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    minute: string

    /**
     * Indicates the second style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    second: string

    /**
     * Indicates the timezone name.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    timeZoneName: string

    /**
     * Indicates the day period format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    dayPeriod: string

    /**
     * Indicates the locale matching algorithm.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    localeMatcher: string

    /**
     * Indicates the format matching algorithm.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    formatMatcher: string
}

/**
 * Provides the API for formatting date strings.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
export class DateTimeFormat {
    /**
     * A constructor used to create a DateTimeFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    constructor();

    /**
     * A constructor used to create a DateTimeFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates character string containing the locale information, including
     *               the language and optionally the script and region, for the DateTimeFormat object.
     * @param options Indicates the options used to format the date.
     * @since 6
     */
    constructor(locale: string | Array<string>, options?: DateTimeOptions);

    /**
     * Obtains the formatted date strings.
     *
     * @syscap SystemCapability.Global.I18n
     * @param date Indicates the Date object to be formatted.
     * @return Returns a date string formatted based on the specified locale.
     * @since 6
     */
    format(date: Date): string;

    /**
     * Obtains the formatted date strings of a date range.
     *
     * @syscap SystemCapability.Global.I18n
     * @param startDate Indicates the start date of the date range.
     * @param endDate Indicates the end date of the date range.
     * @return Returns a date string formatted based on the specified locale.
     * @since 6
     */
    formatRange(startDate: Date, endDate: Date): string;

    /**
     * Obtains the options of the DateTimeFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the options of the DateTimeFormat object.
     * @since 6
     */
    resolvedOptions(): DateTimeOptions;
}

/**
 * Provides the options of number format.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
export interface NumberOptions {
    /**
     * Indicates the locale.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    locale: string

    /**
     * Indicates the currency.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    currency: string

    /**
     * Indicates the currency sign.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    currencySign: string

    /**
     * Indicates the currency display format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    currencyDisplay: string

    /**
     * Indicates the unit.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    unit: string

    /**
     * Indicates the unit display format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    unitDisplay: string

    /**
     * Indicates the unit display format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    unitUsage: string

    /**
     * Indicates the sign display format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    signDisplay: string

    /**
     * Indicates the compact display format.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    compactDisplay: string

    /**
     * Indicates the notation.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    notation: string

    /**
     * Indicates the locale matching algorithm.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    localeMatcher: string

    /**
     * Indicates the style.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    style: string

    /**
     * Indicates the numbering system.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    numberingSystem: string

    /**
     * Indicates whether using grouping or not.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    useGrouping: boolean

    /**
     * Indicates the minimum integer digits.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    minimumIntegerDigits: number

    /**
     * Indicates the minimum fraction digits.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    minimumFractionDigits: number

    /**
     * Indicates the maximum fraction digits.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    maximumFractionDigits: number

    /**
     * Indicates the minimum significant digits.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    minimumSignificantDigits: number

    /**
     * Indicates the maximum significant digits.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 6
     */
    maximumSignificantDigits: number
}

/**
 * Provides the API for formatting number strings.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 6
 */
export class NumberFormat {
    /**
     * A constructor used to create a NumberFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    constructor();

    /**
     * A constructor used to create a NumberFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region, for the NumberFormat object.
     * @param options Indicates the options used to format the number.
     * @since 6
     */
    constructor(locale: string | Array<string>, options?: NumberOptions);

    /**
     * Obtains the formatted number string.
     *
     * @syscap SystemCapability.Global.I18n
     * @param number Indicates the number to be formatted.
     * @return Returns a number string formatted based on the specified locale.
     * @since 6
     */
    format(number: number): string;

    /**
     * Obtains the options of the NumberFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns the options of the NumberFormat object.
     * @since 6
     */
    resolvedOptions(): NumberOptions;
}

/**
 * Provides the options of Collator
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface CollatorOptions {
    /**
     * The locale matching algorithm to use.
     * Possible values are "lookup" and "best fit"; the default is "best fit".
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    localeMatcher: string;

    /**
     * Whether the comparison is for sorting or for searching for matching strings.
     * Possible values are "sort" and "search"; the default is "sort".
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    usage: string;

    /**
     * Which differences in the strings should lead to non-zero result values.
     * Possible values are "base", "accent", "case", "variant".
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    sensitivity: string;

    /**
     * Whether punctuation should be ignored.
     * Possible values are true and false; the default is false.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    ignorePunctuation: boolean;

    /**
     * Variant collations for certain locales.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    collation: string;

    /**
     * Whether numeric collation should be used.
     * Possible values are true and false; the default is false.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    numeric: boolean;

    /**
     * Whether upper case or lower case should sort first.
     * Possible values are "upper", "lower", or "false" (use the locale's default).
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    caseFirst: string;
}

/**
 * Enable language-sensitive string comparison.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class Collator {
    /**
     * A constructor used to create Collator object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    constructor();
    /**
     * A constructor used to create Collator Object;
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region, for the Collator object.
     * @param options Indicates the options used to initialize Collator object.
     * @since 8
     */
    constructor(locale: string | Array<string>, options?: CollatorOptions);

    /**
     * compares two strings according to the sort order of this Collator object
     *
     * @syscap SystemCapability.Global.I18n
     * @param first The first string to compare.
     * @param second The second string to compare.
     * @return Returns a number indicating how first compare to second:
     *         a negative value if string1 comes before string2;
     *         a positive value if string1 comes after string2;
     *         0 if they are considered equal.
     * @since 8
     */
    compare(first: string, second: string): number;

    /**
     * Returns a new object with properties reflecting the locale and collation options computed
     * during initialization of the object.
     *
     * @syscap SystemCapability.Global.I18n
     * @return Returns a CollatorOptions object reflecting the properties of this object.
     * @since 8
     */
    resolvedOptions(): CollatorOptions;
}

/**
 * Provides the options of PluralRules
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface PluralRulesOptions {
    /**
     * The locale matching algorithm to use.
     * Possible values are "lookup" and "best fit"; the default is "best fit".
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    localeMatcher: string;

    /**
     * The type to use. Possible values are: "cardinal", "ordinal"
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    type: string;

    /**
     * The minimum number of integer digits to use.
     * Possible values are from 1 to 21; the default is 1.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    minimumIntegerDigits: number;

    /**
     * The minimum number of fraction digits to use.
     * Possible values are from 0 to 20; the default for plain number and percent formatting is 0;
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    minimumFractionDigits: number;

    /**
     * The maximum number of fraction digits to use.
     * Possible values are from 0 to 20;
     * the default for plain number formatting is the larger of minimumFractionDigits and 3;
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    maximumFractionDigits: number;

    /**
     * The minimum number of significant digits to use.
     * Possible values are from 1 to 21; the default is 1.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    minimumSignificantDigits: number;

    /**
     * The maximum number of significant digits to use.
     * Possible values are from 1 to 21; the default is 21.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    maximumSignificantDigits: number;
}

/**
 * Enables plural-sensitive formatting and plural-related language rules.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class PluralRules {
    /**
     * A constructor used to create PluralRules object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    constructor();

    /**
     * A constructor used to create PluralRules object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region, for the PluralRules object.
     * @param options Indicates the options used to initialize PluralRules object.
     * @since 8
     */
    constructor(locale: string | Array<string>, options?: PluralRulesOptions);

    /**
     * Returns a string indicating which plural rule to use for locale-aware formatting.
     *
     * @syscap SystemCapability.Global.I18n
     * @param n The number to get a plural rule for.
     * @return A string representing the pluralization category of the number,
     *         can be one of zero, one, two, few, many or other.
     * @since 8
     */
    select(n: number): string;
}

/**
 * Provides the input options of RelativeTimeFormat.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
 export interface RelativeTimeFormatInputOptions {
    /**
     * The locale matching algorithm to use.
     * Possible values are: lookup, best fit
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    localeMatcher: string;

    /**
     * The format of output message.
     * Possible values are: always, auto
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    numeric: string;

    /**
     * The length of the internationalized message.
     * Possible values are: long, short, narrow
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    style: string;
}

/**
 * Provides the resolved options of RelativeTimeFormat.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export interface RelativeTimeFormatResolvedOptions {
    /**
     * The BCP 47 language tag for the locale actually used.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    locale: string;

    /**
     * The length of the internationalized message.
     * Possible values are: long, short, narrow
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    style: string;

    /**
     * The format of output message.
     * Possible values are: always, auto
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    numeric: string;

    /**
     * The value requested using the Unicode extension key "nu" or filled in as a default.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    numberingSystem: string;
}

/**
 * Given a Time period length value and a unit, RelativeTimeFormat object enables
 * language-sensitive relative time formatting.
 *
 * @syscap SystemCapability.Global.I18n
 * @since 8
 */
export class RelativeTimeFormat {
    /**
     * A constructor used to create RelativeTimeFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @since 8
     */
    constructor();

    /**
     * A constructor used to create RelativeTimeFormat object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region, for the RelativeTimeFormat object.
     * @param options Indicates the options used to initialize RelativeTimeFormat object.
     * @since 8
     */
    constructor(locale: string | Array<string>, options?: RelativeTimeFormatInputOptions);

    /**
     * formats a value and unit according to the locale and formatting options of this object.
     *
     * @syscap SystemCapability.Global.I18n
     * @param value Numeric value to use in the internationalized relative time message.
     * @param unit Unit to use in the relative time internationalized message.
     *             Possible values are: year, quarter, month, week, day, hour, minute, second.
     * @returns formatted language-sensitive relative time.
     * @since 8
     */
    format(value: number, unit: string): string;

    /**
     * returns an Array of objects representing the relative time format in parts that can be used for
     * custom locale-aware formatting
     *
     * @syscap SystemCapability.Global.I18n
     * @param value Numeric value to use in the internationalized relative time message.
     * @param unit to use in the relative time internationalized message.
     *             Possible values are: year, quarter, month, week, day, hour, minute, second.
     * @returns an Array of objects representing the relative time format in parts
     * @since 8
     */
    formatToParts(value: number, unit: string): Array<object>;

    /**
     * Returns a new object with properties reflecting the locale and formatting options computed during
     * initialization of the object.
     *
     * @syscap SystemCapability.Global.I18n
     * @returns RelativeTimeFormatOptions which reflecting the locale and formatting options of the object.
     * @since 8
     */
    resolvedOptions(): RelativeTimeFormatResolvedOptions;
}
}
export default intl;