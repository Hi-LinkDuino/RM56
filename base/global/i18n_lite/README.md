# i18n<a name="EN-US_TOPIC_0000001078056612"></a>

-   [Overview](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Constraints](#section568761318105)
-   [Usage](#section894511013511)
-   [Repositories Involved](#section15583142420413)
-   [Appendix](#section56601824115)

## Overview<a name="section11660541593"></a>

The internationalization module, namely, i18n, provides i18n capabilities such as formatting the date and time, obtaining the month and week names, and formatting numbers.

## Directory Structure<a name="section1464106163817"></a>

The directory structure for the i18n module is as follows:

```
/base/global/
├── i18n_lite                 # Code repository for the i18n framework
│   ├──  frameworks           # Core code of the i18n framework
│   │   ├── i18n              # i18n module
│   │   │   ├── include       # Header files of external APIs
│   │   │   ├── src           # Implementation code
│   │   │   └── test          # Test cases
│   ├──  interfaces           # i18n framework APIs
│   │   ├── kits              # Application APIs
│   │   │   ├── i18n          # C/C++ i18n APIs
│   │   │   └── js            # C/C++ support for JavaScript APIs
```

## Constraints<a name="section568761318105"></a>

**Programming language**: C/C++

**Supported languages**: See the Appendix.

## Usage<a name="section894511013511"></a>

1. Change the date and time formats \(such as the sequence of year, month, and day, month and week names, and 12-hour or 24-hour system\) following the system settings to adapt to the cultural habits of users in different locales. For details, see the API reference. The sample code is as follows:

```
#include "date_time_format.h"
using namespace OHOS::I18N

LocaleInfo locale("zh", "Hans", "CN"); // Obtain the locale.
DateTimeFormat formatter(AvailableDateTimeFormatPattern::HOUR_MINUTE, locale); // Initialize the example date and time and obtain the data required to format the date and time for the specified locale. The first parameter specifies the formatting pattern. For details about the supported formatting patterns, see the types.h file.
time_t time = 3600 * 3; // Obtain the time to be formatted.
std::string zoneInfo = "+1:00"; // Set the time zone to be UTC+0 plus 1 hour.
std::string out; // Store the formatting result in the out field.
Ii8nStatus status = Ii8nStatus::ISUCCESS;
formatter.Format(time, zoneInfo, out, status); // Check the status field for the formatting result.

output:  4:00
```

2.  Change the number format \(such as the numeral system, grouping, decimal point, and percent sign\) following the system settings to adapt to the cultural habits of users in different locales. For details, see the API reference. The sample code is as follows:

```
#include "number_format.h"
using namespace OHOS::I18N

LocaleInfo locale("en", "US");
int status = 0;
NumberFormat formatter(locale, status); // Initialize the number formatting instance and obtain the data required to format numbers for the specified locale. The value of status indicates the initialization result. If the value is 1, the initialization has failed. 
int num = 1234
std::string out = formatter.Format(num, status); // Check the status field for the initialization result.

output: 1,234
```

3. Obtain the month and week names in the format for the specified locale. The sample code is as follows:

```
#include "date_time_format.h"
using namespace OHOS::I18N

LocaleInfo locale("en", "US"); // Obtain the locale.
DateTimeFormat formatter(AvailableDateTimeFormatPattern::HOUR_MINUTE, locale);
std::string month = formatter.GetMonthName(0, DateTimeDataType::FORMAT_WIDE); //: Obtain the month name in the wide format.

output: January
```

4. Change the plural rule type following the application's language to adapt to the cultural habits of users.

Languages vary in how they handle plurals of nouns. For example, there can be "one apple" or "two apples" in English. Singular and plural forms of nouns are generally classified into six plural rule types: zero, one, two, a few, many, and others. Supported plural rule types vary depending on languages. For example, Chinese supports only  **others**, English supports  **one**  and  **others**, and Arabic supports all the six plural rule types. The sample code is as follows:

```
#include "plural_format.h"
using namespace OHOS::I18N

LocaleInfo locale("en", "US"); // Obtain the locale.
Ii8nStatus status = Ii8nStatus::ISUCCESS;
PluralFormatter formatter = PluralFormatter(locale, status); // Check the status field for the i18n status of the locale.
int out = formatter.GetPluralFormatter(1, status); // Check the status field for the plural rule type.

output: 1 // Value 1 indicates plural rule type 1.
```

## Repositories Involved<a name="section15583142420413"></a>

[Globalization subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/globalization.md)

[global\_resmgr\_lite](https://gitee.com/openharmony/global_resmgr_lite/blob/master/README.md)

**global\_i18n\_lite**

## Appendix<a name="section56601824115"></a>

Supported languages

<a name="table1155163353316"></a>
<table><thead align="left"><tr id="row12551103373310"><th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.1"><p id="p2551233113320"><a name="p2551233113320"></a><a name="p2551233113320"></a>Code</p>
</th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.2"><p id="p4551033123313"><a name="p4551033123313"></a><a name="p4551033123313"></a>Name of Language</p>
</th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.3"><p id="p16551113323317"><a name="p16551113323317"></a><a name="p16551113323317"></a>Code</p>
</th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.4"><p id="p5551113317339"><a name="p5551113317339"></a><a name="p5551113317339"></a>Name of Language</p>
</th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.5"><p id="p9551133383310"><a name="p9551133383310"></a><a name="p9551133383310"></a>Code</p>
</th>
<th class="cellrowborder" valign="top" width="16.666666666666664%" id="mcps1.1.7.1.6"><p id="p25511633143312"><a name="p25511633143312"></a><a name="p25511633143312"></a>Name of Language</p>
</th>
</tr>
</thead>
<tbody><tr id="row1155119337333"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1642315461918"><a name="p1642315461918"></a><a name="p1642315461918"></a>am_ET</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p842312418199"><a name="p842312418199"></a><a name="p842312418199"></a>Amharic</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p15959151711194"><a name="p15959151711194"></a><a name="p15959151711194"></a>hr_HR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p795921716199"><a name="p795921716199"></a><a name="p795921716199"></a>Croatian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p18915730141911"><a name="p18915730141911"></a><a name="p18915730141911"></a>or-IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p4915530181918"><a name="p4915530181918"></a><a name="p4915530181918"></a>Odia</p>
</td>
</tr>
<tr id="row8552833113319"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p342314171912"><a name="p342314171912"></a><a name="p342314171912"></a>ar_EG</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1142312413190"><a name="p1142312413190"></a><a name="p1142312413190"></a>Arabic</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p1959181771915"><a name="p1959181771915"></a><a name="p1959181771915"></a>hu_HU</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p295910172195"><a name="p295910172195"></a><a name="p295910172195"></a>Hungarian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p1915930121910"><a name="p1915930121910"></a><a name="p1915930121910"></a>pa_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p491513302195"><a name="p491513302195"></a><a name="p491513302195"></a>Punjabi, Panjabi</p>
</td>
</tr>
<tr id="row155203333310"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p144235415191"><a name="p144235415191"></a><a name="p144235415191"></a>as_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p54231410192"><a name="p54231410192"></a><a name="p54231410192"></a>Assamese</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p1295991711198"><a name="p1295991711198"></a><a name="p1295991711198"></a>in_ID</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p99591417111911"><a name="p99591417111911"></a><a name="p99591417111911"></a>Indonesian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p791516304193"><a name="p791516304193"></a><a name="p791516304193"></a>pl_PL</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p10915230161914"><a name="p10915230161914"></a><a name="p10915230161914"></a>Polish</p>
</td>
</tr>
<tr id="row15531233183310"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p144232481912"><a name="p144232481912"></a><a name="p144232481912"></a>az_AZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p842316410198"><a name="p842316410198"></a><a name="p842316410198"></a>Azerbaijani</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p13959171701919"><a name="p13959171701919"></a><a name="p13959171701919"></a>it_IT</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1895961781916"><a name="p1895961781916"></a><a name="p1895961781916"></a>Italian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p16915133071912"><a name="p16915133071912"></a><a name="p16915133071912"></a>pt_BR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p5915130131912"><a name="p5915130131912"></a><a name="p5915130131912"></a>Portuguese (Brazil)</p>
</td>
</tr>
<tr id="row755353313317"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1042324121912"><a name="p1042324121912"></a><a name="p1042324121912"></a>be_BY</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p154245441911"><a name="p154245441911"></a><a name="p154245441911"></a>Belarusian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p495931741915"><a name="p495931741915"></a><a name="p495931741915"></a>iw_IL</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1495914176198"><a name="p1495914176198"></a><a name="p1495914176198"></a>Hebrew</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p7916193016198"><a name="p7916193016198"></a><a name="p7916193016198"></a>pt_PT</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p1291683021916"><a name="p1291683021916"></a><a name="p1291683021916"></a>Portuguese (Europe)</p>
</td>
</tr>
<tr id="row10553113311338"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p242434121918"><a name="p242434121918"></a><a name="p242434121918"></a>bg_BG</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1442410411193"><a name="p1442410411193"></a><a name="p1442410411193"></a>Bulgarian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p10959191713197"><a name="p10959191713197"></a><a name="p10959191713197"></a>ja_JP</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p149591317131911"><a name="p149591317131911"></a><a name="p149591317131911"></a>Japanese</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p209167305198"><a name="p209167305198"></a><a name="p209167305198"></a>ro_RO</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p15916430181914"><a name="p15916430181914"></a><a name="p15916430181914"></a>Romanian; Moldavian; Moldovan</p>
</td>
</tr>
<tr id="row1255311336338"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p442417412197"><a name="p442417412197"></a><a name="p442417412197"></a>bn_BD</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p6424144195"><a name="p6424144195"></a><a name="p6424144195"></a>Bengali</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p89597175199"><a name="p89597175199"></a><a name="p89597175199"></a>jv_ID</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p895981711191"><a name="p895981711191"></a><a name="p895981711191"></a>Javanese</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p139161130201917"><a name="p139161130201917"></a><a name="p139161130201917"></a>ru_RU</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p991615309194"><a name="p991615309194"></a><a name="p991615309194"></a>Russian</p>
</td>
</tr>
<tr id="row10554103373313"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p19424164191912"><a name="p19424164191912"></a><a name="p19424164191912"></a>bo_CN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p7424944194"><a name="p7424944194"></a><a name="p7424944194"></a>Tibetan</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p995991791915"><a name="p995991791915"></a><a name="p995991791915"></a>ka_GE</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1395951761911"><a name="p1395951761911"></a><a name="p1395951761911"></a>Georgian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p79161630131915"><a name="p79161630131915"></a><a name="p79161630131915"></a>si_LK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p1791613016194"><a name="p1791613016194"></a><a name="p1791613016194"></a>Sinhala</p>
</td>
</tr>
<tr id="row3554113383312"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1642419414190"><a name="p1642419414190"></a><a name="p1642419414190"></a>bs_BA</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1442424191913"><a name="p1442424191913"></a><a name="p1442424191913"></a>Bosnian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p69605176198"><a name="p69605176198"></a><a name="p69605176198"></a>kk_KZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1696051717196"><a name="p1696051717196"></a><a name="p1696051717196"></a>Kazakh (Cyrillic)</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p10916163010192"><a name="p10916163010192"></a><a name="p10916163010192"></a>sk_SK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p791618306194"><a name="p791618306194"></a><a name="p791618306194"></a>Slovak</p>
</td>
</tr>
<tr id="row055420336334"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p164243412193"><a name="p164243412193"></a><a name="p164243412193"></a>ca_ES</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1742484101914"><a name="p1742484101914"></a><a name="p1742484101914"></a>Catalan</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p20960191717199"><a name="p20960191717199"></a><a name="p20960191717199"></a>km_KH</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p89601517121919"><a name="p89601517121919"></a><a name="p89601517121919"></a>Khmer</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p129161930151919"><a name="p129161930151919"></a><a name="p129161930151919"></a>sl_SI</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p119161130131916"><a name="p119161130131916"></a><a name="p119161130131916"></a>Slovenian</p>
</td>
</tr>
<tr id="row455463316336"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p204245415194"><a name="p204245415194"></a><a name="p204245415194"></a>cs_CZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p114240481913"><a name="p114240481913"></a><a name="p114240481913"></a>Czech</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p18960181712197"><a name="p18960181712197"></a><a name="p18960181712197"></a>kn_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p15960181713190"><a name="p15960181713190"></a><a name="p15960181713190"></a>Kannada</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p0916153010199"><a name="p0916153010199"></a><a name="p0916153010199"></a>sr_Latn_RS sr_RS</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p9916103014196"><a name="p9916103014196"></a><a name="p9916103014196"></a>Serbian (Latin)</p>
</td>
</tr>
<tr id="row155523315331"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p4425134141916"><a name="p4425134141916"></a><a name="p4425134141916"></a>da_DK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p5425194111910"><a name="p5425194111910"></a><a name="p5425194111910"></a>Danish</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p119601217171917"><a name="p119601217171917"></a><a name="p119601217171917"></a>ko_KR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p7960191712191"><a name="p7960191712191"></a><a name="p7960191712191"></a>Korean</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p1691618306198"><a name="p1691618306198"></a><a name="p1691618306198"></a>sv_SE</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p89161630121916"><a name="p89161630121916"></a><a name="p89161630121916"></a>Swedish</p>
</td>
</tr>
<tr id="row1355553314338"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p124251149199"><a name="p124251149199"></a><a name="p124251149199"></a>de_DE</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p3425648194"><a name="p3425648194"></a><a name="p3425648194"></a>German</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p179605177198"><a name="p179605177198"></a><a name="p179605177198"></a>lo_LA</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p296041781912"><a name="p296041781912"></a><a name="p296041781912"></a>Lao</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p1891615300198"><a name="p1891615300198"></a><a name="p1891615300198"></a>sw_TZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p29160307191"><a name="p29160307191"></a><a name="p29160307191"></a>Swahili</p>
</td>
</tr>
<tr id="row855573323320"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p94259491916"><a name="p94259491916"></a><a name="p94259491916"></a>el_GR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1425849195"><a name="p1425849195"></a><a name="p1425849195"></a>Greek</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p7960111751911"><a name="p7960111751911"></a><a name="p7960111751911"></a>lt_LT</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p096015176192"><a name="p096015176192"></a><a name="p096015176192"></a>Lithuanian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p159161230111912"><a name="p159161230111912"></a><a name="p159161230111912"></a>ta_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p291653011917"><a name="p291653011917"></a><a name="p291653011917"></a>Tamil</p>
</td>
</tr>
<tr id="row455517330330"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1042594191916"><a name="p1042594191916"></a><a name="p1042594191916"></a>en_GB</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p34256401915"><a name="p34256401915"></a><a name="p34256401915"></a>English (UK)</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p1296071711193"><a name="p1296071711193"></a><a name="p1296071711193"></a>lv_LV</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p49601717101910"><a name="p49601717101910"></a><a name="p49601717101910"></a>Latvian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p6916103081916"><a name="p6916103081916"></a><a name="p6916103081916"></a>te_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p19916133015196"><a name="p19916133015196"></a><a name="p19916133015196"></a>Telugu</p>
</td>
</tr>
<tr id="row7556143312334"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1642504131918"><a name="p1642504131918"></a><a name="p1642504131918"></a>en_US</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p64259441920"><a name="p64259441920"></a><a name="p64259441920"></a>English (US)</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p18960161731910"><a name="p18960161731910"></a><a name="p18960161731910"></a>mai-Deva-IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p296051718193"><a name="p296051718193"></a><a name="p296051718193"></a>Maithili</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p391753011919"><a name="p391753011919"></a><a name="p391753011919"></a>th_TH</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p1291753016194"><a name="p1291753016194"></a><a name="p1291753016194"></a>Thai</p>
</td>
</tr>
<tr id="row115564335332"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1342534191910"><a name="p1342534191910"></a><a name="p1342534191910"></a>es_ES</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1942518419199"><a name="p1942518419199"></a><a name="p1942518419199"></a>Spanish (Europe)</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p1696010177195"><a name="p1696010177195"></a><a name="p1696010177195"></a>mi-NZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p7960121718191"><a name="p7960121718191"></a><a name="p7960121718191"></a>Maori</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p1391718301197"><a name="p1391718301197"></a><a name="p1391718301197"></a>tl_PH</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p7917153081918"><a name="p7917153081918"></a><a name="p7917153081918"></a>Tagalog</p>
</td>
</tr>
<tr id="row14556173314335"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p114251748198"><a name="p114251748198"></a><a name="p114251748198"></a>es_US</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p15425843198"><a name="p15425843198"></a><a name="p15425843198"></a>Spanish (Latin America)</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p139601917191914"><a name="p139601917191914"></a><a name="p139601917191914"></a>mk_MK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1496016178191"><a name="p1496016178191"></a><a name="p1496016178191"></a>Macedonian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p9917530161919"><a name="p9917530161919"></a><a name="p9917530161919"></a>tr_TR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p169171330171914"><a name="p169171330171914"></a><a name="p169171330171914"></a>Turkish</p>
</td>
</tr>
<tr id="row15556333173313"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p14425645195"><a name="p14425645195"></a><a name="p14425645195"></a>et_EE</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p842513431911"><a name="p842513431911"></a><a name="p842513431911"></a>Estonian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p896061791916"><a name="p896061791916"></a><a name="p896061791916"></a>ml_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1096018172190"><a name="p1096018172190"></a><a name="p1096018172190"></a>Malayalam</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p991743017195"><a name="p991743017195"></a><a name="p991743017195"></a>uk_UA</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p991710309199"><a name="p991710309199"></a><a name="p991710309199"></a>Ukrainian</p>
</td>
</tr>
<tr id="row1855613353314"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p542654141917"><a name="p542654141917"></a><a name="p542654141917"></a>eu_ES</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p342613481912"><a name="p342613481912"></a><a name="p342613481912"></a>Basque</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p2096021715195"><a name="p2096021715195"></a><a name="p2096021715195"></a>mn_MN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1396031741915"><a name="p1396031741915"></a><a name="p1396031741915"></a>Mongolia</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p491773041913"><a name="p491773041913"></a><a name="p491773041913"></a>ur_PK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p491753071916"><a name="p491753071916"></a><a name="p491753071916"></a>Urdu</p>
</td>
</tr>
<tr id="row1355743319338"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p94261947191"><a name="p94261947191"></a><a name="p94261947191"></a>fa_IR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p194262041193"><a name="p194262041193"></a><a name="p194262041193"></a>Persian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p109611217131915"><a name="p109611217131915"></a><a name="p109611217131915"></a>mr_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p1696141718197"><a name="p1696141718197"></a><a name="p1696141718197"></a>Marathi</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p2091783015192"><a name="p2091783015192"></a><a name="p2091783015192"></a>uz_UZ</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p209171730151917"><a name="p209171730151917"></a><a name="p209171730151917"></a>Uzbek</p>
</td>
</tr>
<tr id="row555743317339"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p174262471919"><a name="p174262471919"></a><a name="p174262471919"></a>fi_FI</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p144269418197"><a name="p144269418197"></a><a name="p144269418197"></a>Finnish</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p1796161741910"><a name="p1796161741910"></a><a name="p1796161741910"></a>ms_MY</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p89611617161917"><a name="p89611617161917"></a><a name="p89611617161917"></a>Malay</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p9917193017194"><a name="p9917193017194"></a><a name="p9917193017194"></a>vi_VN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p149178302196"><a name="p149178302196"></a><a name="p149178302196"></a>Vietnamese</p>
</td>
</tr>
<tr id="row16557333153316"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p204269417197"><a name="p204269417197"></a><a name="p204269417197"></a>fr_FR</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p15426745195"><a name="p15426745195"></a><a name="p15426745195"></a>French</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p189611917121913"><a name="p189611917121913"></a><a name="p189611917121913"></a>my_MM my_ZG</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p79611117131910"><a name="p79611117131910"></a><a name="p79611117131910"></a>Burmese</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p12917193041917"><a name="p12917193041917"></a><a name="p12917193041917"></a>zh_CN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p19917143071916"><a name="p19917143071916"></a><a name="p19917143071916"></a>Simplified Chinese</p>
</td>
</tr>
<tr id="row1055743383312"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p1742674191920"><a name="p1742674191920"></a><a name="p1742674191920"></a>gl_ES</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p242619414192"><a name="p242619414192"></a><a name="p242619414192"></a>Galician</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p13961717121916"><a name="p13961717121916"></a><a name="p13961717121916"></a>nb_NO</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p2096111751916"><a name="p2096111751916"></a><a name="p2096111751916"></a>Norwegian</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p139171630101910"><a name="p139171630101910"></a><a name="p139171630101910"></a>zh_HK</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p3917330191918"><a name="p3917330191918"></a><a name="p3917330191918"></a>Traditional Chinese (Hong Kong, China)</p>
</td>
</tr>
<tr id="row455873319339"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p34261948195"><a name="p34261948195"></a><a name="p34261948195"></a>gu_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p1842616481918"><a name="p1842616481918"></a><a name="p1842616481918"></a>Gujarati</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p996181717197"><a name="p996181717197"></a><a name="p996181717197"></a>ne_NP</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p69611517101918"><a name="p69611517101918"></a><a name="p69611517101918"></a>Nepali</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 "><p id="p1491714309196"><a name="p1491714309196"></a><a name="p1491714309196"></a>zh_TW</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 "><p id="p14917163011918"><a name="p14917163011918"></a><a name="p14917163011918"></a>Traditional Chinese (Taiwan, China)</p>
</td>
</tr>
<tr id="row9558833183312"><td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.1 "><p id="p134261645197"><a name="p134261645197"></a><a name="p134261645197"></a>hi_IN</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.2 "><p id="p74260415197"><a name="p74260415197"></a><a name="p74260415197"></a>Hindi</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.3 "><p id="p15961417181916"><a name="p15961417181916"></a><a name="p15961417181916"></a>nl_NL</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.4 "><p id="p89611317191919"><a name="p89611317191919"></a><a name="p89611317191919"></a>Dutch; Flemish</p>
</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.5 ">&nbsp;&nbsp;</td>
<td class="cellrowborder" valign="top" width="16.666666666666664%" headers="mcps1.1.7.1.6 ">&nbsp;&nbsp;</td>
</tr>
</tbody>
</table>

