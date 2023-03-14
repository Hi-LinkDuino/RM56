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
describe('intlDateTimeFormatTest', function () {
    console.log('******test start********');
   
    /* *
    * @tc.number ARK_INTL_JS_DATETIME_001
    * @tc.name format the number in en-GB and zh
    * @tc.desc check the number in en-GB and zh
    */
    it('numberFormat_info_test_001', 0, function () {
        var numfmt = new Intl.NumberFormat(["en-GB", "zh"], { style: 'decimal', notation: "scientific" });
        console.log(numfmt.format(1223));
        expect(numfmt.format(1223)).assertEqual('1.223E3');
        //        console.log(numfmt.resolvedOptions().locale);
        //        console.log(numfmt.resolvedOptions().currency);
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_003
    * @tc.name format the number in ar-EG
    * @tc.desc check the style of number in ar-EG
    */
    it('numberFormat_info_test_003', 0, function () {
        var number = 123456.789;
        var numfmt =new Intl.NumberFormat('ar-EG');
        console.log(numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('١٢٣٬٤٥٦٫٧٨٩');
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_004
    * @tc.name format the number in en-IN
    * @tc.desc check the format of number in en-IN
    */
    it('numberFormat_info_test_004', 0, function () {
        var number = 123456.789;
        var numfmt =new Intl.NumberFormat('en-IN');
        console.log(numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('1,23,456.789');
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_005
    * @tc.name format the number in zh-Hans-CN-u-nu-hanidec
    * @tc.desc check the format of number in zh-Hans-CN-u-nu-hanidec
    */
    it('numberFormat_info_test_005', 0, function () {
        var number = 123456.789;
        // 通过编号系统中的nu扩展键请求, 例如中文十进制数字
        var numfmt =new Intl.NumberFormat('zh-Hans-CN-u-nu-hanidec')
        console.log(numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('一二三,四五六.七八九')
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_006
    * @tc.name format the number in ban and id
    * @tc.desc check the format of number in ban and id
    */
    it('numberFormat_info_test_006', 0, function () {
        var number = 123456.789;
        var numfmt=new Intl.NumberFormat('ban');
        console.log('numberFormat_info_test_006 '+numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('123,456.789')
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_008
    * @tc.name format the number in ja-JP
    * @tc.desc check the format of number in ja-JP
    */
    it('numberFormat_info_test_008', 0, function () {
        var number = 123456.789;
        var numfmt=new Intl.NumberFormat('ja', { style: 'currency', currency: 'JPY' });
        console.log('numberFormat_info_test_008 '+numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('￥123,457');
    })

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_009
    * @tc.name format the number in en-IN
    * @tc.desc check the format of number in en-IN
    */
    it('numberFormat_info_test_009', 0, function () {
        var number = 123456.789;
        var numfmt=new Intl.NumberFormat('en-IN', { maximumSignificantDigits: 3 })
        console.log('numberFormat_info_test_009 '+numfmt.format(number));
        expect(numfmt.format(number)).assertEqual('1,23,000')
    })

})