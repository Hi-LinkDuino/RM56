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
import Intl from '@ohos.intl';
import {describe,beforeAll,beforeEach,afterEach,afterAll,it,expect}from 'deccjsunit/index'
describe('intlDateTimeFormatTest_1', function () {
   console.log('******test start******')

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_001
    * @tc.name format the datetime with en-GB locale
    * @tc.desc check the datetime is not null
    */
   it('dateTimeFormat_test_001', 0, function () {
       let datefmt = new Intl.DateTimeFormat('en-GB');
       expect(datefmt !== null).assertTrue();
   })

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_002
    * @tc.name format the datetime with en-GB locale
    * @tc.desc check the datetime is not null
    */
   it('dateTimeFormat_test_002', 0, function () {
       let datefmt = new Intl.DateTimeFormat();
       expect(datefmt !== null).assertTrue();
   })

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_003
    * @tc.name format the date with zh locale
    * @tc.desc check the date
    */
   it('dateTimeFormat_test_003', 0, function () {
       let date = new Date(2021, 11, 17, 3, 24, 0);
       let datefmt = new Intl.DateTimeFormat('zh');
       console.log('dateTimeFormat_test_003 ' + datefmt.format(date));
       expect(datefmt.format(date)).assertEqual('2021/12/17');
   })

})

describe('intlDateTimeFormatTest_2', function () {
    console.log('******test start******')

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_005
    * @tc.name format the date with da locale
    * @tc.desc check the date
    */
   it('dateTimeFormat_test_005', 0, function () {
    let date = new Date(2021, 11, 17, 3, 24, 0);
    let datefmt = new Intl.DateTimeFormat('da');
    console.log('dateTimeFormat_test_005 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('17.12.2021');
})

/* *
 * @tc.number ARK_INTL_JS_DATETIME_006
 * @tc.name format the date with en-GB locale
 * @tc.desc check the date
 */
it('dateTimeFormat_test_006', 0, function () {
    let date = new Date(2021, 11, 17, 3, 24, 0);
    let datefmt = new Intl.DateTimeFormat('en-GB');
    console.log('dateTimeFormat_test_006 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('17/12/2021');
})
})

describe('intlDateTimeFormatTest_3', function () {
    console.log('******test start******')

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_007
    * @tc.name format the date with aa locale
    * @tc.desc check the date
    */
   it('dateTimeFormat_test_007', 0, function () {
    let date = new Date(2021, 11, 17, 3, 24, 0);
    let datefmt = new Intl.DateTimeFormat('aa');
    let value = datefmt.format(date);
    console.log('dateTimeFormat_test_007 ' + value);
    expect(value).assertInstanceOf('String');
})

/* *
 * @tc.number ARK_INTL_JS_DATETIME_008
 * @tc.name format the hour with ban and zh locale
 * @tc.desc check the hour with ban and zh locale
 */
it('dateTimeFormat_test_008', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { dateStyle: 'full' };
    let datefmt = new Intl.DateTimeFormat(['ban', 'zh'],option);
    console.log('dateTimeFormat_test_008 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('2020年12月20日星期日');
})

/* *
 * @tc.number ARK_INTL_JS_DATETIME_009
 * @tc.name format the hour with en and zh locale
 * @tc.desc check the hour with en and zh locale
 */
it('dateTimeFormat_test_009', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { dateStyle: 'full' };
    let datefmt = new Intl.DateTimeFormat(['en', 'zh'], option);
    console.log('dateTimeFormat_test_009 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('Sunday, December 20, 2020');
})
})

describe('intlDateTimeFormatTest_4', function () {
    console.log('******test start******')

/* *
 * @tc.number ARK_INTL_JS_DATETIME_010
 * @tc.name format the hour with en and ban locale
 * @tc.desc check the hour with en and ban locale
 */
it('dateTimeFormat_test_010', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { dateStyle: 'full' };
    let datefmt = new Intl.DateTimeFormat(['en', 'ban'], option);
    console.log('dateTimeFormat_test_010 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('Sunday, December 20, 2020');
})

/* *
 * @tc.number ARK_INTL_JS_DATETIME_013
 * @tc.name format the date dateStyle with long style
 * @tc.desc check the dateStyle with long style
 */
it('dateTimeFormat_test_013', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { dateStyle: 'long', timeStyle: 'long' };
    let datefmt = new Intl.DateTimeFormat('zh-CN', option);
    console.log('dateTimeFormat_test_013 ' + datefmt.resolvedOptions().dateStyle);
    expect(datefmt.resolvedOptions().dateStyle).assertEqual('long');
})

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_014
    * @tc.name format the date dateStyle with medium style
    * @tc.desc check the dateStyle with medium style
    */
   it('dateTimeFormat_test_014', 0, function () {
       let date = new Date(2020, 11, 20, 14, 23, 16);
       let option = { dateStyle: 'medium', timeStyle: 'medium' };
       let datefmt = new Intl.DateTimeFormat('zh-CN', option);
       console.log('dateTimeFormat_test_014 ' + datefmt.resolvedOptions().dateStyle);
       expect(datefmt.resolvedOptions().dateStyle).assertEqual('medium');
       expect(datefmt.format(date)).assertEqual('2020年12月20日 下午2:23:16');
   })
})

describe('intlDateTimeFormatTest_5', function () {
    console.log('******test start******')
 
    /* *
    * @tc.number ARK_INTL_JS_DATETIME_017
    * @tc.name format the date with daterange when the start and end date is equal
    * @tc.desc check the daterange when the start and end date is equal
    */
 it('dateTimeFormat_test_017', 0, function () {
    let startdate = new Date(2020, 11, 20, 14, 23, 16);
    let enddate = new Date(2020, 11, 20, 14, 23, 16);
    let datefmt = new Intl.DateTimeFormat('en-GB');
    console.log('dateTimeFormat_test_017 ' + datefmt.formatRange(startdate, enddate));
    expect(datefmt.formatRange(startdate, enddate)).assertEqual('20/12/2020');
})

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_016
    * @tc.name format the date with daterange
    * @tc.desc check the daterange
    */
   it('dateTimeFormat_test_016', 0, function () {
    let startdate = new Date(2020, 11, 20, 14, 23, 16);
    let enddate = new Date(2020, 11, 21, 14, 23, 16);
    let datefmt = new Intl.DateTimeFormat('en-GB');
    console.log('dateTimeFormat_test_016 ' + datefmt.formatRange(startdate, enddate));
    expect(datefmt.formatRange(startdate, enddate)).assertEqual('20/12/2020 – 21/12/2020');
})

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_015
    * @tc.name format the date dateStyle with short style
    * @tc.desc check the dateStyle with short style
    */
   it('dateTimeFormat_test_015', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { dateStyle: 'short', timeStyle: 'short' };
    let datefmt = new Intl.DateTimeFormat('zh-CN', option);
    console.log('dateTimeFormat_test_015 ' + datefmt.resolvedOptions().dateStyle);
    expect(datefmt.resolvedOptions().dateStyle).assertEqual('short');
})

})

describe('intlDateTimeFormatTest_6', function () {
    console.log('******test start******')
 
/* *
    * @tc.number ARK_INTL_JS_DATETIME_018
    * @tc.name format the year、month、day、weekday
    * @tc.desc check the year、month、day、weekday
    */
it('dateTimeFormat_test_018', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { year: 'numeric', month: 'long', day: 'numeric', weekday: 'long' };
    let datefmt = new Intl.DateTimeFormat('ja', option);
    console.log('dateTimeFormat_test_018 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('2020年12月20日日曜日');
     })

     /* *
    * @tc.number ARK_INTL_JS_DATETIME_019
    * @tc.name format the timerange with year、month、day、weekday
    * @tc.desc check the timerange with year、month、day、weekday
    */
   it('dateTimeFormat_test_019', 0, function () {
    let startdate = new Date(2020, 11, 20, 14, 23, 16);
    let enddate = new Date(2021, 4, 5, 10, 5, 3);
    let option = { year: 'numeric', month: 'long', day: 'numeric', weekday: 'long' };
    let datefmt = new Intl.DateTimeFormat('en', option);
    console.log('dateTimeFormat_test_019 ' + datefmt.formatRange(startdate, enddate));
    expect(datefmt.formatRange(startdate, enddate)).assertEqual(
        'Sunday, December 20, 2020 – Wednesday, May 5, 2021');
     })

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_020
    * @tc.name format the hour&minute&second
    * @tc.desc check the hour&minute&second
    */
   it('dateTimeFormat_test_020', 0, function () {
    let date = new Date(2020, 11, 20, 14, 23, 16);
    let option = { year: 'numeric', month: 'long', day: 'numeric',
        hour: 'numeric', minute: '2-digit', second: 'numeric', weekday: 'long', era: 'short' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_020 ' + datefmt.format(date));
    expect(datefmt.format(date)).assertEqual('公元2020年12月20日星期日 下午2:23:16');
    })
})

describe('intlDateTimeFormatTest_7', function () {
    console.log('******test start******')

/* *
    * @tc.number ARK_INTL_JS_DATETIME_021
    * @tc.name format the timeZone
    * @tc.desc check the timeZone
    */
it('dateTimeFormat_test_021', 0, function () {
    let date = new Date(2020, 3, 14, 15, 5, 3);
    let option = { timeZone: 'America/Los_Angeles', timeZoneName: 'long' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_021 ' + datefmt.format(date));
    expect(datefmt.format(date).indexOf('北美太平洋夏令时间') !== -1).assertTrue();
    })

   /* *
    * @tc.number ARK_INTL_JS_DATETIME_022
    * @tc.name format the America/Los_Angeles timeZone
    * @tc.desc check the America/Los_Angeles timeZone
    */
   it('dateTimeFormat_test_022', 0, function () {
    let option = { timeZone: 'America/Los_Angeles', timeZoneName: 'long' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_022 ' + datefmt.resolvedOptions().timeZone);
    expect(datefmt.resolvedOptions().timeZone).assertEqual('America/Los_Angeles');
    })

     /* *
    * @tc.number ARK_INTL_JS_DATETIME_023
    * @tc.name format the America/Los_Angeles timeZoneName
    * @tc.desc check the America/Los_Angeles timeZoneName
    */
     it('dateTimeFormat_test_023', 0, function () {
        let option = { timeZone: 'America/Los_Angeles', timeZoneName: 'long' };
        let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
        console.log('dateTimeFormat_test_023 ' + datefmt.resolvedOptions().timeZoneName);
        expect(datefmt.resolvedOptions().timeZoneName).assertEqual('long');
    })
        
 /* *
    * @tc.number ARK_INTL_JS_DATETIME_024
    * @tc.name format the year
    * @tc.desc check the year
    */
 it('dateTimeFormat_test_024', 0, function () {
    let option = { year: 'numeric', month: 'long', day: 'numeric',
        hour: 'numeric', minute: '2-digit', second: 'numeric', weekday: 'long', era: 'short' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_024 ' + datefmt.resolvedOptions().year);
    expect(datefmt.resolvedOptions().year).assertEqual('numeric');
    })
})

describe('intlDateTimeFormatTest_7', function () {
    console.log('******test start******')
  
/* *
    * @tc.number ARK_INTL_JS_DATETIME_025
    * @tc.name format the weekday
    * @tc.desc check the weekday
    */
it('dateTimeFormat_test_025', 0, function () {
    let option = { year: 'numeric', month: 'long', day: 'numeric',
        hour: 'numeric', minute: '2-digit', second: 'numeric', weekday: 'long', era: 'short' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_025 ' + datefmt.resolvedOptions().weekday);
    expect(datefmt.resolvedOptions().weekday).assertEqual('long');
   })

     /* *
    * @tc.number ARK_INTL_JS_DATETIME_026
    * @tc.name format the hour in zh-CN-u-hc-h12
    * @tc.desc check the hour in zh-CN-u-hc-h12
    */
 it('dateTimeFormat_test_026', 0, function () {
    let option = { year: 'numeric', month: 'long', day: 'numeric',
        hour: 'numeric', minute: '2-digit', second: 'numeric', weekday: 'long', era: 'short' };
    let datefmt = new Intl.DateTimeFormat('zh-CN-u-hc-h12', option);
    console.log('dateTimeFormat_test_026 ' + datefmt.resolvedOptions().hour);
    expect(datefmt.resolvedOptions().hour).assertEqual('numeric');
   })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_027
    * @tc.name format the hour in fr-Latn-FR-u-nu-mong
    * @tc.desc check the hour in fr-Latn-FR-u-nu-mong
    */
     it('dateTimeFormat_test_027', 0, function () {
        let date = new Date(2020, 11, 20, 14, 23, 16);
        let option = { dateStyle: 'full' };
        let datefmt = new Intl.DateTimeFormat('fr-Latn-FR-u-nu-mong', option);
        console.log('dateTimeFormat_test_027 ' + datefmt.format(date));
        expect(datefmt.format(date)).assertEqual('dimanche ᠒᠐ décembre ᠒᠐᠒᠐');
    })
})