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
import I18n from '@ohos.i18n'
import Intl from '@ohos.intl'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('LangTest', function () {
    console.log('*************start LangTest*************');

    let initPreferredLang = I18n.getPreferredLanguageList();
    let initLen = initPreferredLang.length;
    let hour = I18n.is24HourClock();

    /* *
    * get the current preferred language list
    */
    function getCurrentPreferredLang(){
        let value = I18n.getPreferredLanguageList();
        return value;
    }

    /* *
    * judge if the lang is in the preferred language list or not
    */
    function isContainLang(langList, lang){
        let len = langList.length;
        for (let i = 0; i < len; i++){
            if(langList[i] == lang){
                return true
            }
        }
        return false
    }

    /* *
    * clear the preferred language list if exists
    */
    function clearLang(langList){
        let len = langList.length;
        while(len > 0){
            console.log('i18n_test_preferredlanguage_clearLang ' + len);
            let par = len - 1;
            console.log('i18n_test_preferredlanguage_clearLang ' + par);
            let value = I18n.removePreferredLanguage(par);
            console.log('i18n_test_preferredlanguage_clearLang ' + value);
            len--;
        }
        console.log('i18n_test_preferredlanguage_clearLang ' + I18n.getPreferredLanguageList());
    }

    /* *
    * restore the init lang list
    */
    function restoreLang(){
        for(let j = 0; j < initLen; j++){
            let value = I18n.addPreferredLanguage(initPreferredLang[j], j);
            console.log('i18n_test_preferredlanguage_restoreLang ' + value);
            expect(value).assertTrue();
        }
        let currLen = getCurrentPreferredLang().length;
        while(currLen > initLen) {
            let rem = I18n.removePreferredLanguage(currLen - 1);
            console.log('i18n_test_preferredlanguage_restoreLang ' + rem);
            currLen--;
        }
        console.log('i18n_test_preferredlanguage_restoreLang ' + I18n.getPreferredLanguageList());
    }

    /* *
    * execute this step before all testcases
    */
    beforeAll(function(){
        console.log('step before all cases in lang.'
        + ' 24hour: ' + I18n.is24HourClock()
        + ' prelang: ' + I18n.getPreferredLanguageList()
        + ' syslocale: ' + I18n.getSystemLocale());
    })

    /* *
    * execute this step before every testcase
    */
    beforeEach(function(){
        console.log('i18n_test_preferredlanguage_beforeEach ' + getCurrentPreferredLang());
    })

    /* *
    * execute this step after every testcase
    */
    afterEach(function(){
        let currLang = getCurrentPreferredLang();
        console.log('i18n_test_preferredlanguage_afterEach ' + currLang);
        clearLang(currLang);
        restoreLang();
        let afterValue = I18n.set24HourClock(hour);
        console.log('step after every cases.' + afterValue);
        console.log('24 hour clock after every cases ' + I18n.is24HourClock());
    })

    /* *
    * execute this step after all testcases
    */
    afterAll(function(){
        console.log('step after all cases.'
        + ' 24hour: ' + I18n.is24HourClock()
        + ' prelang: ' + I18n.getPreferredLanguageList()
        + ' syslocale: ' + I18n.getSystemLocale());
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_CLOCK_0100
    * @tc.name test the is24HourClock interface
    * @tc.desc check the value of is24HourClock method
    */
    it('i18n_test_clock_0100', 0, function () {
        console.log('i18n_test_clock_0100 ' + 'start');
        let value = I18n.is24HourClock();
        console.log('i18n_test_clock_0100 ' + value);
        if(hour)
        {
            expect(value).assertTrue();
        }
        else
        {
            expect(value).assertFalse();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_CLOCK_0200
    * @tc.name test the set24HourClock interface with true param
    * @tc.desc check the value of set24HourClock method
    */
    it('i18n_test_clock_0200', 0, function () {
        console.log('i18n_test_clock_0200 ' + 'start');
        let value = I18n.set24HourClock(true);
        console.log('i18n_test_clock_0200 ' + value);
        expect(value).assertTrue();
        let value2 = I18n.is24HourClock();
        console.log('i18n_test_clock_0200 ' + value2);
        expect(value2).assertTrue();
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_CLOCK_0300
    * @tc.name test the set24HourClock interface with false param
    * @tc.desc check the value of set24HourClock method
    */
    it('i18n_test_clock_0300', 0, function () {
        console.log('i18n_test_clock_0300 ' + 'start');
        let value = I18n.set24HourClock(false);
        console.log('i18n_test_clock_0300 ' + value);
        expect(value).assertTrue();
        let value2 = I18n.is24HourClock();
        console.log('i18n_test_clock_0300 ' + value2);
        expect(value2).assertFalse();
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_CLOCK_0400
    * @tc.name test the set24HourClock and datetime interface
    * @tc.desc check the value of set24HourClock method
    */
    it('i18n_test_clock_0400', 0, function () {
        console.log('i18n_test_clock_0400 ' + 'start');
        let value = I18n.set24HourClock(true);
        console.log('i18n_test_clock_0400 ' + value);
        expect(value).assertTrue();
        let date = new Date(2021, 11, 17, 3, 24, 0);
        let datefmt = new Intl.DateTimeFormat('en');
        console.log('i18n_test_clock_0400 ' + datefmt.format(date));
        expect(datefmt.format(date)).assertEqual('12/17/21');
        expect(datefmt.resolvedOptions().hour12).assertFalse();
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0100
    * @tc.name test the getPreferredLanguageList interface with default value
    * @tc.desc check the value of getPreferredLanguageList method
    */
    it('i18n_test_preferredlanguage_0100', 0, function () {
        console.log('i18n_test_preferredlanguage_0100 ' + 'start');
        let value = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0100 ' + value);
        expect(value.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0200
    * @tc.name test the addPreferredLanguage interface
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0200', 0, function () {
        console.log('i18n_test_preferredlanguage_0200 ' + 'start');
        let value = I18n.addPreferredLanguage('zh');
        console.log('i18n_test_preferredlanguage_0200 ' + value);
        expect(value).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0200 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0220
    * @tc.name test the addPreferredLanguage interface with abc param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0220', 0, function () {
        console.log('i18n_test_preferredlanguage_0220 ' + 'start');
        let value = I18n.addPreferredLanguage('abc');
        console.log('i18n_test_preferredlanguage_0220 ' + value);
        expect(value).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0220 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0300
    * @tc.name test the addPreferredLanguage interface with en and 0 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0300', 0, function () {
        console.log('i18n_test_preferredlanguage_0300 ' + 'start');
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0300 ' + list);
        expect(list.length).assertLarger(0);
        if(list[0] != 'en'){
            let value = I18n.addPreferredLanguage('en', 0);
            console.log('i18n_test_preferredlanguage_0300 ' + value);
            expect(value).assertTrue();
        }
        else{
            let value = I18n.addPreferredLanguage('zh', 0);
            console.log('i18n_test_preferredlanguage_0300 ' + value);
            expect(value).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0320
    * @tc.name test the addPreferredLanguage interface with abc and index 0 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0320', 0, function () {
        console.log('i18n_test_preferredlanguage_0320 ' + 'start');
        let value = I18n.addPreferredLanguage('abc', 0);
        console.log('i18n_test_preferredlanguage_0320 ' + value);
        expect(value).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0320 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0340
    * @tc.name test the addPreferredLanguage interface with en and index 1.5 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0340', 0, function () {
        console.log('i18n_test_preferredlanguage_0340 ' + 'start');
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0340 ' + list);
        expect(list.length).assertLarger(0);
        if(list[0] != 'en'){
            let value = I18n.addPreferredLanguage('en', 1.5);
            console.log('i18n_test_preferredlanguage_0340 ' + value);
            expect(value).assertTrue();
        }
        else{
            let value = I18n.addPreferredLanguage('zh', 1);
            console.log('i18n_test_preferredlanguage_0340 ' + value);
            expect(value).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0400
    * @tc.name test the addPreferredLanguage interface with en and index 1 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0400', 0, function () {
        console.log('i18n_test_preferredlanguage_0400 ' + 'start');
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0400 ' + list);
        expect(list.length).assertLarger(0);
        if(list[0] != 'en'){
            let value = I18n.addPreferredLanguage('en', 1);
            console.log('i18n_test_preferredlanguage_0400 ' + value);
            expect(value).assertTrue();
        }
        else{
            let value = I18n.addPreferredLanguage('zh', 1);
            console.log('i18n_test_preferredlanguage_0400 ' + value);
            expect(value).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0500
    * @tc.name test the addPreferredLanguage interface with ja and index -1 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0500', 0, function () {
        console.log('i18n_test_preferredlanguage_0500 ' + 'start');
        let value = I18n.addPreferredLanguage('ja', -1);
        console.log('i18n_test_preferredlanguage_0500 ' + value);
        expect(value).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0500 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0600
    * @tc.name test the addPreferredLanguage interface with ko and index 100 param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0600', 0, function () {
        console.log('i18n_test_preferredlanguage_0600 ' + 'start');
        let value = I18n.addPreferredLanguage('ko', 100);
        console.log('i18n_test_preferredlanguage_0600 ' + value);
        expect(value).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0600 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0700
    * @tc.name test the removePreferredLanguage interface with 0 param
    * @tc.desc check the value of removePreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0700', 0, function () {
        console.log('i18n_test_preferredlanguage_0700 ' + 'start');
        let value = I18n.removePreferredLanguage(0);
        console.log('i18n_test_preferredlanguage_0700 ' + value);
        expect(value).assertFalse();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0700 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0720
    * @tc.name test the removePreferredLanguage interface with 0.5 param
    * @tc.desc check the value of removePreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0720', 0, function () {
        console.log('i18n_test_preferredlanguage_0720 ' + 'start');
        let value = I18n.removePreferredLanguage(0.5);
        console.log('i18n_test_preferredlanguage_0720 ' + value);
        expect(value).assertFalse();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0720 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0800
    * @tc.name test the removePreferredLanguage interface with -1 param
    * @tc.desc check the value of removePreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0800', 0, function () {
        console.log('i18n_test_preferredlanguage_0800 ' + 'start');
        let value = I18n.removePreferredLanguage(-1);
        console.log('i18n_test_preferredlanguage_0800 ' + value);
        expect(value).assertFalse();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0800 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0820
    * @tc.name test the removePreferredLanguage interface with -1 param after add ja
    * @tc.desc check the value of removePreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0820', 0, function () {
        console.log('i18n_test_preferredlanguage_0820 ' + 'start');
        let value = I18n.addPreferredLanguage('ja');
        console.log('i18n_test_preferredlanguage_0500 ' + value);
        expect(value).assertTrue();
        let value2 = I18n.removePreferredLanguage(-1);
        console.log('i18n_test_preferredlanguage_0820 ' + value2);
        expect(value2).assertTrue();
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0820 ' + list);
        expect(list.length).assertLarger(0);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0900
    * @tc.name test the removePreferredLanguage interface with 0 param
    * @tc.desc check the value of removePreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0900', 0, function () {
        console.log('i18n_test_preferredlanguage_0900 ' + 'start');
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0900 ' + list);
        let len = list.length;
        console.log('i18n_test_preferredlanguage_0900 ' + len);
        expect(len).assertLarger(0);
        let parm = len - 1;
        console.log('i18n_test_preferredlanguage_0900 ' + parm);
        let value = I18n.removePreferredLanguage(parm);
        console.log('i18n_test_preferredlanguage_0900 ' + value);
        expect(value).assertFalse();
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0920
    * @tc.name test the addPreferredLanguage interface with it param
    * @tc.desc check the value of addPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_0920', 0, function () {
        console.log('i18n_test_preferredlanguage_0920 ' + 'start');
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0920 ' + list);
        let isContain = isContainLang(list, 'it')
        console.log('i18n_test_preferredlanguage_0920 ' + isContain);
        let len = list.length;
        console.log('i18n_test_preferredlanguage_0920 ' + len);
        expect(len).assertLarger(0);
        if(!isContain) {
            let value = I18n.addPreferredLanguage('it');
            console.log('i18n_test_preferredlanguage_0920 ' + value);
            expect(value).assertTrue();
        }
        let list2 = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0920 ' + list2);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_0940
    * @tc.name test the mixed interface with preferredlanguage
    * @tc.desc check the value of preferredLanguage method
    */
    it('i18n_test_preferredlanguage_0940', 0, function () {
        console.log('i18n_test_preferredlanguage_0940 ' + 'start');
        let value = I18n.addPreferredLanguage('it');
        console.log('i18n_test_preferredlanguage_0940 ' + value);
        expect(value).assertTrue();
        let value2 = I18n.addPreferredLanguage('ko');
        console.log('i18n_test_preferredlanguage_0940 ' + value2);
        expect(value2).assertTrue();
        let value3 = I18n.addPreferredLanguage('th');
        console.log('i18n_test_preferredlanguage_0940 ' + value3);
        expect(value3).assertTrue();
        let list1 = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0940 ' + list1);
        let value4 = I18n.addPreferredLanguage('it', 3);
        console.log('i18n_test_preferredlanguage_0940 ' + value4);
        expect(value4).assertTrue();
        let value5 = I18n.addPreferredLanguage('ko', 2);
        console.log('i18n_test_preferredlanguage_0940 ' + value5);
        expect(value5).assertTrue();
        let value6 = I18n.addPreferredLanguage('th', 1);
        console.log('i18n_test_preferredlanguage_0940 ' + value6);
        expect(value6).assertFalse();
        let list2 = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0940 ' + list2);
        expect(list2[1]).assertEqual('th');
        expect(list2[2]).assertEqual('ko');
        expect(list2[3]).assertEqual('it');
        let remove1 = I18n.removePreferredLanguage(1);
        console.log('i18n_test_preferredlanguage_0940 ' + remove1);
        expect(remove1).assertTrue();
        let list3 = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0940 ' + list3);
        expect(list3[1]).assertEqual('ko');
        expect(list3[2]).assertEqual('it');
        let remove2 = I18n.removePreferredLanguage(1);
        console.log('i18n_test_preferredlanguage_0940 ' + remove2);
        expect(remove2).assertTrue();
        let list4 = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_0940 ' + list4);
        expect(list4[1]).assertEqual('it');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_PREFERREDLANGUAGE_1000
    * @tc.name test the getFirstPreferredLanguage interface
    * @tc.desc check the value of getFirstPreferredLanguage method
    */
    it('i18n_test_preferredlanguage_1000', 0, function () {
        console.log('i18n_test_preferredlanguage_1000 ' + 'start');
        let value = I18n.getFirstPreferredLanguage();
        console.log('i18n_test_preferredlanguage_1000 ' + value);
        let list = I18n.getPreferredLanguageList();
        console.log('i18n_test_preferredlanguage_1000 ' + list);
        expect(list.length).assertLarger(0);
        if(list[0] == 'zh-Hans'){
            expect(value).assertEqual('zh-Hans');
        }
        else if(list[0] == 'en'){
            expect(value).assertEqual('en');
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_INTL_JS_TRANSFER_0100
    * @tc.name transfer from lower to upper
    * @tc.desc check the transfer result
    */
    it('transfer_test_0100', 0, function () {
        let date = 'hello';
        let value = date.toLocaleUpperCase('zh-CN');
        console.log('transfer_test_0100 ' + value);
        expect(value).assertEqual('HELLO');
    })

    /* *
    * @tc.number SUB_GLOBAL_INTL_JS_TRANSFER_0200
    * @tc.name transfer from upper to lower
    * @tc.desc check the transfer result
    */
    it('transfer_test_0200', 0, function () {
        let date = 'WORLD';
        let value = date.toLocaleLowerCase('zh-CN');
        console.log('transfer_test_0200 ' + value);
        expect(value).assertEqual('world');
    })

    /* *
    * @tc.number SUB_GLOBAL_INTL_JS_TRANSFER_0300
    * @tc.name transfer to upper
    * @tc.desc check the transfer result
    */
    it('transfer_test_0300', 0, function () {
        let date = 'My name is Jack.';
        let value = date.toLocaleUpperCase('en-US');
        console.log('transfer_test_0300 ' + value);
        expect(value).assertEqual('MY NAME IS JACK.');
    })

    /* *
    * @tc.number SUB_GLOBAL_INTL_JS_TRANSFER_0400
    * @tc.name transfer to lower
    * @tc.desc check the transfer result
    */
    it('transfer_test_0400', 0, function () {
        let date = 'The sky is in BLUE-STYLE!';
        let value = date.toLocaleLowerCase('en-US');
        console.log('transfer_test_0400 ' + value);
        expect(value).assertEqual('the sky is in blue-style!');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0100
    * @tc.name test getTimeZone method
    * @tc.desc get the getTimeZone value
    */
    it('timezone_test_0100', 0, function () {
        console.log('timezone_test_0100 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getID();
        console.log('timezone_test_0100 ' + value);
        expect(value).assertEqual('UTC');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0200
    * @tc.name test getDisplayName method
    * @tc.desc get the getDisplayName value
    */
    it('timezone_test_0200', 0, function () {
        console.log('timezone_test_0200 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getDisplayName();
        console.log('timezone_test_0200 ' + value);
        expect(value).assertEqual('GMT');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0300
    * @tc.name test getDisplayName method with zh-CN param
    * @tc.desc get the getDisplayName value
    */
    it('timezone_test_0300', 0, function () {
        console.log('timezone_test_0300 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getDisplayName('zh-CN');
        console.log('timezone_test_0300 ' + value);
        expect(value).assertEqual('GMT');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0400
    * @tc.name test getDisplayName method with true param
    * @tc.desc get the getDisplayName value
    */
    it('timezone_test_0400', 0, function () {
        console.log('timezone_test_0400 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getDisplayName(true);
        console.log('timezone_test_0400 ' + value);
        expect(value).assertEqual('GMT');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0500
    * @tc.name test getDisplayName method with false param
    * @tc.desc get the getDisplayName value
    */
    it('timezone_test_0500', 0, function () {
        console.log('timezone_test_0500 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getDisplayName(false);
        console.log('timezone_test_0500 ' + value);
        expect(value).assertEqual('GMT');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0600
    * @tc.name test getRawOffset method
    * @tc.desc get the getRawOffset value
    */
    it('timezone_test_0600', 0, function () {
        console.log('timezone_test_0600 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getRawOffset();
        console.log('timezone_test_0600 ' + value);
        expect(value >= 0).assertEqual(true);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0700
    * @tc.name test getOffset method
    * @tc.desc get the getOffset value
    */
    it('timezone_test_0700', 0, function () {
        console.log('timezone_test_0700 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getOffset();
        console.log('timezone_test_0700 ' + value);
        expect(value >= 0).assertEqual(true);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0800
    * @tc.name test getOffset method with date param
    * @tc.desc get the getOffset value
    */
    it('timezone_test_0800', 0, function () {
        console.log('timezone_test_0800 ' + 'start');
        let timezone = I18n.getTimeZone();
        let value = timezone.getOffset(10540800000);
        console.log('timezone_test_0800 ' + value);
        expect(value >= 0).assertEqual(true);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_0900
    * @tc.name test getID
    * @tc.desc get the getID value
    */
    it('timezone_test_0900', 0, function () {
        console.log('timezone_test_0900 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = timezone.getID();
        console.log('timezone_test_0900 ' + value);
        expect(value).assertEqual('ACT');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1000
    * @tc.name test getDisplayName with timezone id
    * @tc.desc get the getID value
    */
    it('timezone_test_1000', 0, function () {
        console.log('timezone_test_1000 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = timezone.getDisplayName();
        console.log('timezone_test_1000 ' + value);
        expect(value).assertEqual('澳大利亚中部标准时间');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1100
    * @tc.name test getDisplayName with locale
    * @tc.desc get the getID value
    */
    it('timezone_test_1100', 0, function () {
        console.log('timezone_test_1100 ' + 'start');
        let timezone = I18n.getTimeZone('Asia/Shanghai');
        let value = timezone.getDisplayName('zh-CN');
        console.log('timezone_test_1100 ' + value);
        expect(value).assertEqual('中国标准时间');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1200
    * @tc.name test getDisplayName with locale
    * @tc.desc get the getID value
    */
    it('timezone_test_1200', 0, function () {
        console.log('timezone_test_1200 ' + 'start');
        let timezone = I18n.getTimeZone('Asia/Shanghai');
        let value = timezone.getDisplayName('zh-CN', true);
        console.log('timezone_test_1200 ' + value);
        expect(value).assertEqual('中国标准时间');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1300
    * @tc.name test getDisplayName with en-US and true param
    * @tc.desc get the getID value
    */
    it('timezone_test_1300', 0, function () {
        console.log('timezone_test_1300 ' + 'start');
        let timezone = I18n.getTimeZone('Asia/Shanghai');
        let value = timezone.getDisplayName('en-US', true);
        console.log('timezone_test_1300 ' + value);
        expect(value).assertEqual('China Standard Time');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1400
    * @tc.name test getDisplayName with zh-CN and false param
    * @tc.desc get the getID value
    */
    it('timezone_test_1400', 0, function () {
        console.log('timezone_test_1400 ' + 'start');
        let timezone = I18n.getTimeZone('Asia/Shanghai');
        let value = timezone.getDisplayName('zh-CN', false);
        console.log('timezone_test_1400 ' + value);
        expect(value).assertEqual('中国标准时间');
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1500
    * @tc.name test getRawOffset method with timezone id
    * @tc.desc get the getRawOffset value
    */
    it('timezone_test_1500', 0, function () {
        console.log('timezone_test_1500 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = timezone.getRawOffset();
        console.log('timezone_test_1500 ' + value);
        expect(value > 0).assertEqual(true);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1600
    * @tc.name test getOffset method with timezone id
    * @tc.desc get the getOffset value
    */
    it('timezone_test_1600', 0, function () {
        console.log('timezone_test_1600 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = timezone.getOffset();
        console.log('timezone_test_1600 ' + value);
        expect(value > 0).assertEqual(true);
    })

    /* *
    * @tc.number SUB_GLOBAL_I18N_JS_TIMEZONE_1700
    * @tc.name test getOffset method with date and timezone id param
    * @tc.desc get the getOffset value
    */
    it('timezone_test_1700', 0, function () {
        console.log('timezone_test_1700 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = timezone.getOffset(10540800000);
        console.log('timezone_test_1700 ' + value);
        expect(value > 0).assertEqual(true);
    })

    console.log('*************end LangTest*************');
})