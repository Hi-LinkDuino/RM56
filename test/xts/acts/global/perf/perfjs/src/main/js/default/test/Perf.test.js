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

describe('PerfTest', function () {
    console.log('*************start PerfTest*************');

    let EXETIME = 1000;
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
    * execute this step after every testcase
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
        console.log('step before all cases in perf.'
        + ' 24hour: ' + I18n.is24HourClock()
        + ' prelang: ' + I18n.getPreferredLanguageList()
        + ' syslocale: ' + I18n.getSystemLocale());
    })

    /* *
    * execute this step before every testcase
    */
    beforeEach(function(){
        console.log('step before every case in perf.');
        console.log('i18n_test_preferredlanguage_beforeEach ' + getCurrentPreferredLang());
    })

    /* *
    * execute this step after every testcase
    */
    afterEach(function(){
        console.log('step after every case in perf.');
        let currLang = getCurrentPreferredLang();
        console.log('i18n_test_preferredlanguage_afterEach ' + currLang);
        clearLang(currLang);
        restoreLang();
    })

    /* *
    * execute this step after all testcases
    */
    afterAll(function(){
        console.log('step after all cases in perf.'
        + ' 24hour: ' + I18n.is24HourClock()
        + ' prelang: ' + I18n.getPreferredLanguageList()
        + ' syslocale: ' + I18n.getSystemLocale());
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0100
    * @tc.name test the performance of getSystemLanguage
    * @tc.desc check the performance of getSystemLanguage
    */
    it('perf_test_0100', 0, function () {
        console.log('perf_test_0100 ' + 'start');
        let lang = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            lang = I18n.getSystemLanguage();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0100--' 
                    + 'lang: ' + lang 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0200
    * @tc.name test the performance of getSystemRegion
    * @tc.desc check the performance of getSystemRegion
    */
    it('perf_test_0200', 0, function () {
        console.log('perf_test_0200 ' + 'start');
        let region = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            region = I18n.getSystemRegion();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0200--' 
                    + 'region: ' + region 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0300
    * @tc.name test the performance of getSystemLocale
    * @tc.desc check the performance of getSystemLocale
    */
    it('perf_test_0300', 0, function () {
        console.log('perf_test_0300 ' + 'start');
        let locale = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            locale = I18n.getSystemLocale();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0300--' 
                    + 'locale: ' + locale 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0400
    * @tc.name test the performance of setSystemLanguage
    * @tc.desc check the performance of setSystemLanguage
    */
    it('perf_test_0400', 0, function () {
        console.log('perf_test_0400 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.setSystemLanguage('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0400--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0500
    * @tc.name test the performance of setSystemRegion
    * @tc.desc check the performance of setSystemRegion
    */
    it('perf_test_0500', 0, function () {
        console.log('perf_test_0500 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.setSystemRegion('US');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0500--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0600
    * @tc.name test the performance of setSystemLocale
    * @tc.desc check the performance of setSystemLocale
    */
    it('perf_test_0600', 0, function () {
        console.log('perf_test_0600 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.setSystemLocale('en-Latn-US');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0600--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0700
    * @tc.name test the performance of getSystemLanguages
    * @tc.desc check the performance of getSystemLanguages
    */
    it('perf_test_0700', 0, function () {
        console.log('perf_test_0700 ' + 'start');
        let value = new Array();
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getSystemLanguages();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0700--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0800
    * @tc.name test the performance of getSystemCountries
    * @tc.desc check the performance of getSystemCountries
    */
    it('perf_test_0800', 0, function () {
        console.log('perf_test_0800 ' + 'start');
        let value = new Array();
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getSystemCountries('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0800--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_0900
    * @tc.name test the performance of isSuggested
    * @tc.desc check the performance of isSuggested
    */
    it('perf_test_0900', 0, function () {
        console.log('perf_test_0900 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.isSuggested('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_0900--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1000
    * @tc.name test the performance of getDisplayCountry
    * @tc.desc check the performance of getDisplayCountry
    */
    it('perf_test_1000', 0, function () {
        console.log('perf_test_1000 ' + 'start');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getDisplayCountry('zh-Hans-CN', 'en-US', true);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1000--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1100
    * @tc.name test the performance of getDisplayLanguage
    * @tc.desc check the performance of getDisplayLanguage
    */
    it('perf_test_1100', 0, function () {
        console.log('perf_test_1100 ' + 'start');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getDisplayLanguage('zh-Hans-CN', 'en-US', true);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1100--' 
                    + 'value: ' + value 
                    + ' exeTime: ' + exeTime 
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1200
    * @tc.name test the performance of PhoneNumberFormat
    * @tc.desc check the performance of PhoneNumberFormat
    */
    it('perf_test_1200', 0, function () {
        console.log('perf_test_1200 ' + 'start');
        let phonenumber = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            phonenumber = new I18n.PhoneNumberFormat('CN');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1200--'
                    + 'phonenumber: ' + phonenumber
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1300
    * @tc.name test the performance of isValidNumber
    * @tc.desc check the performance of isValidNumber
    */
    it('perf_test_1300', 0, function () {
        console.log('perf_test_1300 ' + 'start');
        let phonenumber = new I18n.PhoneNumberFormat('CN');
        let valid = false;
        let ran = Math.ceil(Math.random(1) * 9);
        console.log('perf_test_1300 ' + ran);
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            valid = phonenumber.isValidNumber('1351057467' + ran);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1300--'
                    + 'valid: ' + valid
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1400
    * @tc.name test the performance of format
    * @tc.desc check the performance of format
    */
    it('perf_test_1400', 0, function () {
        console.log('perf_test_1400 ' + 'start');
        let phonenumber = new I18n.PhoneNumberFormat('CN');
        let value = '1234';
        let ran = Math.ceil(Math.random(1) * 9);
        console.log('perf_test_1400 ' + ran);
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = phonenumber.format('1351057467' + ran);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1500
    * @tc.name test the performance of getCalendar
    * @tc.desc check the performance of getCalendar
    */
    it('perf_test_1500', 0, function () {
        console.log('perf_test_1500 ' + 'start');
        let calendar = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar = I18n.getCalendar('zh');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1500--'
                    + 'calendar: ' + calendar
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1600
    * @tc.name test the performance of isWeekend
    * @tc.desc check the performance of isWeekend
    */
    it('perf_test_1600', 0, function () {
        console.log('perf_test_1600 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = calendar.isWeekend(new Date(2021, 10, 1, 10, 0, 0, 0));
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1600--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1700
    * @tc.name test the performance of getDisplayName
    * @tc.desc check the performance of getDisplayName
    */
    it('perf_test_1700', 0, function () {
        console.log('perf_test_1700 ' + 'start');
        let calendar = I18n.getCalendar('zh', 'chinese');
        let name = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            name = calendar.getDisplayName('zh');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1700--'
                    + 'name: ' + name
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1800
    * @tc.name test the performance of calendar.get
    * @tc.desc check the performance of calendar.get
    */
    it('perf_test_1800', 0, function () {
        console.log('perf_test_1800 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = calendar.get('year');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_1900
    * @tc.name test the performance of calendar.setTime with date param
    * @tc.desc check the performance of calendar.setTime
    */
    it('perf_test_1900', 0, function () {
        console.log('perf_test_1900 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let date1 = new Date(2021, 8, 8, 8, 8, 8, 8);
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.setTime(date1);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_1900--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2000
    * @tc.name test the performance of calendar.setTime with time param
    * @tc.desc check the performance of calendar.setTime
    */
    it('perf_test_2000', 0, function () {
        console.log('perf_test_2000 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.setTime(10540800000);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2000--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2100
    * @tc.name test the performance of calendar.set
    * @tc.desc check the performance of calendar.set
    */
    it('perf_test_2100', 0, function () {
        console.log('perf_test_2100 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.set(2021, 11, 11, 10, 10, 10);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2100--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2200
    * @tc.name test the performance of calendar.setTimeZone
    * @tc.desc check the performance of calendar.setTimeZone
    */
    it('perf_test_2200', 0, function () {
        console.log('perf_test_2200 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.setTimeZone('Asia/Shanghai');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2200--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2300
    * @tc.name test the performance of calendar.getTimeZone
    * @tc.desc check the performance of calendar.getTimeZone
    */
    it('perf_test_2300', 0, function () {
        console.log('perf_test_2300 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = calendar.getTimeZone();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2300--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2400
    * @tc.name test the performance of calendar.setFirstDayOfWeek
    * @tc.desc check the performance of calendar.setFirstDayOfWeek
    */
    it('perf_test_2400', 0, function () {
        console.log('perf_test_2400 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.setFirstDayOfWeek(1);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2400--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2500
    * @tc.name test the performance of calendar.getFirstDayOfWeek
    * @tc.desc check the performance of calendar.getFirstDayOfWeek
    */
    it('perf_test_2500', 0, function () {
        console.log('perf_test_2500 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = calendar.getFirstDayOfWeek();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2500--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2600
    * @tc.name test the performance of calendar.setMinimalDaysInFirstWeek
    * @tc.desc check the performance of calendar.setMinimalDaysInFirstWeek
    */
    it('perf_test_2600', 0, function () {
        console.log('perf_test_2600 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            calendar.setMinimalDaysInFirstWeek(1);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2600--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2700
    * @tc.name test the performance of calendar.getMinimalDaysInFirstWeek
    * @tc.desc check the performance of calendar.getMinimalDaysInFirstWeek
    */
    it('perf_test_2700', 0, function () {
        console.log('perf_test_2700 ' + 'start');
        let calendar = I18n.getCalendar('zh');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = calendar.getMinimalDaysInFirstWeek();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2700--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2800
    * @tc.name test the performance of unitConvert
    * @tc.desc check the performance of unitConvert
    */
    it('perf_test_2800', 0, function () {
        console.log('perf_test_2800 ' + 'start');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Util.unitConvert({unit: 'hour', measureSystem: 'SI'},
                                        {unit: 'second', measureSystem: 'SI'},
                                        10,
                                        'zh-CN',
                                        'medium');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_2900
    * @tc.name test the performance of isRTL
    * @tc.desc check the performance of isRTL
    */
    it('perf_test_2900', 0, function () {
        console.log('perf_test_2900 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.isRTL('zh-CN');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_2900--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3000
    * @tc.name test the performance of getLineInstance
    * @tc.desc check the performance of getLineInstance
    */
    it('perf_test_3000', 0, function () {
        console.log('perf_test_3000 ' + 'start');
        let iterator = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            iterator = I18n.getLineInstance('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3000--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3100
    * @tc.name test the performance of setLineBreakText
    * @tc.desc check the performance of setLineBreakText
    */
    it('perf_test_3100', 0, function () {
        console.log('perf_test_3100 ' + 'start');
        let iterator = I18n.getLineInstance('en');
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3100--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3200
    * @tc.name test the performance of current
    * @tc.desc check the performance of current
    */
    it('perf_test_3200', 0, function () {
        console.log('perf_test_3200 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.current();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3200--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3300
    * @tc.name test the performance of first
    * @tc.desc check the performance of first
    */
    it('perf_test_3300', 0, function () {
        console.log('perf_test_3300 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.first();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3300--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3400
    * @tc.name test the performance of last
    * @tc.desc check the performance of last
    */
    it('perf_test_3400', 0, function () {
        console.log('perf_test_3400 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.last();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3500
    * @tc.name test the performance of next
    * @tc.desc check the performance of next
    */
    it('perf_test_3500', 0, function () {
        console.log('perf_test_3500 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.next();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3500--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3600
    * @tc.name test the performance of previous
    * @tc.desc check the performance of previous
    */
    it('perf_test_3600', 0, function () {
        console.log('perf_test_3600 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.previous();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3600--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3700
    * @tc.name test the performance of following
    * @tc.desc check the performance of following
    */
    it('perf_test_3700', 0, function () {
        console.log('perf_test_3700 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.following(1);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3700--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3800
    * @tc.name test the performance of getLineBreakText
    * @tc.desc check the performance of getLineBreakText
    */
    it('perf_test_3800', 0, function () {
        console.log('perf_test_3800 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.getLineBreakText();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_3900
    * @tc.name test the performance of isBoundary
    * @tc.desc check the performance of isBoundary
    */
    it('perf_test_3900', 0, function () {
        console.log('perf_test_3900 ' + 'start');
        let iterator =I18n.getLineInstance('en');
        iterator.setLineBreakText('My name is Tom.Welcome to the sky world.');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = iterator.isBoundary(10);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_3900--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4000
    * @tc.name test the performance of getInstance
    * @tc.desc check the performance of getInstance
    */
    it('perf_test_4000', 0, function () {
        console.log('perf_test_4000 ' + 'start');
        let index = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            index = I18n.getInstance();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4000--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4100
    * @tc.name test the performance of getIndex
    * @tc.desc check the performance of getIndex
    */
    it('perf_test_4100', 0, function () {
        console.log('perf_test_4100 ' + 'start');
        let index = I18n.getInstance();
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = index.getIndex('a');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4100--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4200
    * @tc.name test the performance of getIndexList
    * @tc.desc check the performance of getIndexList
    */
    it('perf_test_4200', 0, function () {
        console.log('perf_test_4200 ' + 'start');
        let index = I18n.getInstance();
        let value = new Array();
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = index.getIndexList();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4200--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4300
    * @tc.name test the performance of addLocale
    * @tc.desc check the performance of addLocale
    */
    it('perf_test_4300', 0, function () {
        console.log('perf_test_4300 ' + 'start');
        let index = I18n.getInstance();
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            index.addLocale('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4300--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4400
    * @tc.name test the performance of isDigit
    * @tc.desc check the performance of isDigit
    */
    it('perf_test_4400', 0, function () {
        console.log('perf_test_4400 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isDigit('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4500
    * @tc.name test the performance of isSpaceChar
    * @tc.desc check the performance of isSpaceChar
    */
    it('perf_test_4500', 0, function () {
        console.log('perf_test_4500 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isSpaceChar('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4500--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })


    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4600
    * @tc.name test the performance of isWhitespace
    * @tc.desc check the performance of isWhitespace
    */
    it('perf_test_4600', 0, function () {
        console.log('perf_test_4600 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isWhitespace('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4600--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4700
    * @tc.name test the performance of isRTL
    * @tc.desc check the performance of isRTL
    */
    it('perf_test_4700', 0, function () {
        console.log('perf_test_4700 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isRTL('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4700--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4800
    * @tc.name test the performance of isIdeograph
    * @tc.desc check the performance of isIdeograph
    */
    it('perf_test_4800', 0, function () {
        console.log('perf_test_4800 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isIdeograph('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_4900
    * @tc.name test the performance of isLetter
    * @tc.desc check the performance of isLetter
    */
    it('perf_test_4900', 0, function () {
        console.log('perf_test_4900 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isLetter('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_4900--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5000
    * @tc.name test the performance of isLowerCase
    * @tc.desc check the performance of isLowerCase
    */
    it('perf_test_5000', 0, function () {
        console.log('perf_test_5000 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isLowerCase('abc');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5000--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5100
    * @tc.name test the performance of isUpperCase
    * @tc.desc check the performance of isUpperCase
    */
    it('perf_test_5100', 0, function () {
        console.log('perf_test_5100 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.isUpperCase('ABC');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5100--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5200
    * @tc.name test the performance of getType
    * @tc.desc check the performance of getType
    */
    it('perf_test_5200', 0, function () {
        console.log('perf_test_5200 ' + 'start');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.Character.getType('a');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5200--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5300
    * @tc.name test the performance of is24HourClock
    * @tc.desc check the performance of is24HourClock
    */
    it('perf_test_5300', 0, function () {
        console.log('perf_test_5300 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.is24HourClock();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5300--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5400
    * @tc.name test the performance of set24HourClock
    * @tc.desc check the performance of set24HourClock
    */
    it('perf_test_5400', 0, function () {
        console.log('perf_test_5400 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.set24HourClock(true);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
        I18n.set24HourClock(hour);
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5500
    * @tc.name test the performance of getPreferredLanguageList
    * @tc.desc check the performance of getPreferredLanguageList
    */
    it('perf_test_5500', 0, function () {
        console.log('perf_test_5500 ' + 'start');
        let value = new Array();
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getPreferredLanguageList();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5500--'
                    + 'value.length: ' + value.length
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5600
    * @tc.name test the performance of getFirstPreferredLanguage
    * @tc.desc check the performance of getFirstPreferredLanguage
    */
    it('perf_test_5600', 0, function () {
        console.log('perf_test_5600 ' + 'start');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.getFirstPreferredLanguage();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5600--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 20){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5700
    * @tc.name test the performance of addPreferredLanguage
    * @tc.desc check the performance of addPreferredLanguage
    */
    it('perf_test_5700', 0, function () {
        console.log('perf_test_5700 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.addPreferredLanguage('it');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5700--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5800
    * @tc.name test the performance of removePreferredLanguage
    * @tc.desc check the performance of removePreferredLanguage
    */
    it('perf_test_5800', 0, function () {
        console.log('perf_test_5800 ' + 'start');
        let value = false;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = I18n.removePreferredLanguage(0);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_5900
    * @tc.name test the performance of Locale
    * @tc.desc check the performance of Locale
    */
    it('perf_test_5900', 0, function () {
        console.log('perf_test_5900 ' + 'start');
        let locale = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            locale = new Intl.Locale('en-Latn-GB');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_5900--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6000
    * @tc.name test the performance of toString
    * @tc.desc check the performance of toString
    */
    it('perf_test_6000', 0, function () {
        console.log('perf_test_6000 ' + 'start');
        let locale = new Intl.Locale('zh');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = locale.toString();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6000--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6100
    * @tc.name test the performance of minimize
    * @tc.desc check the performance of minimize
    */
    it('perf_test_6100', 0, function () {
        console.log('perf_test_6100 ' + 'start');
        let locale = new Intl.Locale('zh-Hans-CN');
        let locale2 = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            locale2 = locale.minimize();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        let value = locale2.toString();
        console.log('perf_test_6100--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6200
    * @tc.name test the performance of maximize
    * @tc.desc check the performance of maximize
    */
    it('perf_test_6200', 0, function () {
        console.log('perf_test_6200 ' + 'start');
        let locale = new Intl.Locale('zh-CN');
        let locale2 = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            locale2 = locale.maximize();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        let value = locale2.toString();
        console.log('perf_test_6200--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6300
    * @tc.name test the performance of DateTimeFormat
    * @tc.desc check the performance of DateTimeFormat
    */
    it('perf_test_6300', 0, function () {
        console.log('perf_test_6300 ' + 'start');
        let datefmt = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            datefmt = new Intl.DateTimeFormat('en-GB');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6300--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6400
    * @tc.name test the performance of DateTimeFormat.format
    * @tc.desc check the performance of DateTimeFormat.format
    */
    it('perf_test_6400', 0, function () {
        console.log('perf_test_6400 ' + 'start');
        let date = new Date(2021, 11, 17, 3, 24, 0);
        let datefmt = new Intl.DateTimeFormat('zh');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = datefmt.format(date);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6500
    * @tc.name test the performance of DateTimeFormat.formatRange
    * @tc.desc check the performance of DateTimeFormat.formatRange
    */
    it('perf_test_6500', 0, function () {
        console.log('perf_test_6500 ' + 'start');
        let startdate = new Date(2020, 11, 20, 14, 23, 16);
        let enddate = new Date(2020, 11, 21, 14, 23, 16);
        let datefmt = new Intl.DateTimeFormat('en-GB');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = datefmt.formatRange(startdate, enddate);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6500--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6600
    * @tc.name test the performance of DateTimeFormat.resolvedOptions
    * @tc.desc check the performance of DateTimeFormat.resolvedOptions
    */
    it('perf_test_6600', 0, function () {
        console.log('perf_test_6600 ' + 'start');
        let date = new Date(2020, 11, 20, 14, 23, 16);
        let option = { dateStyle: 'short', timeStyle: 'short' };
        let datefmt = new Intl.DateTimeFormat('zh-CN', option);
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            datefmt.resolvedOptions();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6600--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6700
    * @tc.name test the performance of NumberFormat
    * @tc.desc check the performance of NumberFormat
    */
    it('perf_test_6700', 0, function () {
        console.log('perf_test_6700 ' + 'start');
        let numfmt = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            numfmt = new Intl.NumberFormat('zh');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6700--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6800
    * @tc.name test the performance of NumberFormat.format
    * @tc.desc check the performance of NumberFormat.format
    */
    it('perf_test_6800', 0, function () {
        console.log('perf_test_6800 ' + 'start');
        let numfmt = new Intl.NumberFormat('zh-u-nu-latn');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = numfmt.format(123456.789);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_6900
    * @tc.name test the performance of NumberFormat.resolvedOptions
    * @tc.desc check the performance of NumberFormat.resolvedOptions
    */
    it('perf_test_6900', 0, function () {
        console.log('perf_test_6900 ' + 'start');
        let numfmt = new Intl.NumberFormat('ar', { style: 'percent',
                                                    minimumFractionDigits: 1,
                                                    maximumFractionDigits: 2,
                                                    minimumIntegerDigits: 7,
                                                    minimumSignificantDigits: 5,
                                                    maximumSignificantDigits: 10,
                                                    useGrouping: true  });
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            numfmt.resolvedOptions();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_6900--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7000
    * @tc.name test the performance of PluralRules
    * @tc.desc check the performance of PluralRules
    */
    it('perf_test_7000', 0, function () {
        console.log('perf_test_7000 ' + 'start');
        let pl = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            pl = new Intl.PluralRules('zh');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7000--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7100
    * @tc.name test the performance of PluralRules.select
    * @tc.desc check the performance of PluralRules.select
    */
    it('perf_test_7100', 0, function () {
        console.log('perf_test_7100 ' + 'start');
        let pl = new Intl.PluralRules('zh', {'type': 'ordinal'});
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = pl.select(0);
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7100--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7200
    * @tc.name test the performance of Collator
    * @tc.desc check the performance of Collator
    */
    it('perf_test_7200', 0, function () {
        console.log('perf_test_7200 ' + 'start');
        let coll = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            coll = new Intl.Collator('zh');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7200--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7300
    * @tc.name test the performance of Collator.compare with same param
    * @tc.desc check the performance of Collator.compare
    */
    it('perf_test_7300', 0, function () {
        console.log('perf_test_7300 ' + 'start');
        let coll = new Intl.Collator('zh', {'sensitivity': 'base'});
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = coll.compare('a', 'a');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7300--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7400
    * @tc.name test the performance of Collator.compare
    * @tc.desc check the performance of Collator.compare
    */
    it('perf_test_7400', 0, function () {
        console.log('perf_test_7400 ' + 'start');
        let coll = new Intl.Collator('zh', {'sensitivity': 'base'});
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = coll.compare('a', 'b');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7400--'
        + 'value: ' + value
        + ' exeTime: ' + exeTime
        + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7500
    * @tc.name test the performance of Collator.resolvedOptions
    * @tc.desc check the performance of Collator.resolvedOptions
    */
    it('perf_test_7500', 0, function () {
        console.log('perf_test_7500 ' + 'start');
        let coll = new Intl.Collator('zh', {'sensitivity': 'base'});
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            coll.resolvedOptions();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7500--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7600
    * @tc.name test the performance of RelativeTimeFormat
    * @tc.desc check the performance of RelativeTimeFormat
    */
    it('perf_test_7600', 0, function () {
        console.log('perf_test_7600 ' + 'start');
        let relativetimefmt = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            relativetimefmt = new Intl.RelativeTimeFormat('en');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7600--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7700
    * @tc.name test the performance of RelativeTimeFormat.format
    * @tc.desc check the performance of RelativeTimeFormat.format
    */
    it('perf_test_7700', 0, function () {
        console.log('perf_test_7700 ' + 'start');
        let relativetimefmt = new Intl.RelativeTimeFormat('en');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = relativetimefmt.format(100,'second');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7700--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7800
    * @tc.name test the performance of RelativeTimeFormat.formatToParts
    * @tc.desc check the performance of RelativeTimeFormat.formatToParts
    */
    it('perf_test_7800', 0, function () {
        console.log('perf_test_7800 ' + 'start');
        let relativetimefmt = new Intl.RelativeTimeFormat('en');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = relativetimefmt.formatToParts(100,'second');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7800--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_7900
    * @tc.name test the performance of RelativeTimeFormat.resolvedOptions
    * @tc.desc check the performance of RelativeTimeFormat.resolvedOptions
    */
    it('perf_test_7900', 0, function () {
        console.log('perf_test_7900 ' + 'start');
        let relativetimefmt = new Intl.RelativeTimeFormat('en', { localeMatcher: 'best fit',
                                                                    numeric: 'auto',
                                                                    style: 'narrow' });
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            relativetimefmt.resolvedOptions();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_7900--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8000
    * @tc.name test the performance of getTimeZone
    * @tc.desc check the performance of getTimeZone
    */
    it('perf_test_8000', 0, function () {
        console.log('perf_test_8000 ' + 'start');
        let timezone = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            timezone = I18n.getTimeZone();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8000--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8100
    * @tc.name test the performance of getTimeZone with zoneID param
    * @tc.desc check the performance of getTimeZone
    */
    it('perf_test_8100', 0, function () {
        console.log('perf_test_8100 ' + 'start');
        let timezone = null;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            timezone = I18n.getTimeZone('ACT');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8100--'
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8200
    * @tc.name test the performance of getID
    * @tc.desc check the performance of getID
    */
    it('perf_test_8200', 0, function () {
        console.log('perf_test_8200 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = timezone.getID();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8200--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8300
    * @tc.name test the performance of getDisplayName
    * @tc.desc check the performance of getDisplayName
    */
    it('perf_test_8300', 0, function () {
        console.log('perf_test_8300 ' + 'start');
        let timezone = I18n.getTimeZone('Asia/Shanghai');
        let value = 'test';
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = timezone.getDisplayName('zh-CN');
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8300--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8400
    * @tc.name test the performance of getRawOffset
    * @tc.desc check the performance of getRawOffset
    */
    it('perf_test_8400', 0, function () {
        console.log('perf_test_8400 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = timezone.getRawOffset();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8400--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    /* *
    * @tc.number SUB_GLOBAL_PERF_JS_8500
    * @tc.name test the performance of getOffset
    * @tc.desc check the performance of getOffset
    */
    it('perf_test_8500', 0, function () {
        console.log('perf_test_8500 ' + 'start');
        let timezone = I18n.getTimeZone('ACT');
        let value = 0;
        let startTime = new Date().getTime();
        for(let i = 0; i < EXETIME; i++){
            value = timezone.getOffset();
        }
        let exeTime = new Date().getTime() - startTime;
        let avgTime = exeTime/EXETIME;
        console.log('perf_test_8500--'
                    + 'value: ' + value
                    + ' exeTime: ' + exeTime
                    + ' avgTime: ' + avgTime);
        if(avgTime < 10){
            expect(true).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }
    })

    console.log('*************end PerfTest*************');
})