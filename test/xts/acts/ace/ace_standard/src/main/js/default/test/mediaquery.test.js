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

import {describe,beforeAll,beforeEach,afterEach,afterAll,it,expect}from 'deccjsunit/index'
import mediaquery from '@system.mediaquery';

describe('mediaquery', function() {
    let testResult;
    let test;
    beforeAll(function() {
        testResult = false;
        test="success"
    });

    it('addListener', 0, function() {
        console.info('MediaQuery addListener start');
        var mMediaQueryList = mediaquery.matchMedia('(min-height:0)');
//        mMediaQueryList.onreadystatechange = function(){
            function minWidthMatch(e){
                if(e.matches){
                    //do something
                    expect(e.matches).assertEqual(true);
                    console.info('MediaQuery addListener success');
                    expect(test).assertEqual('success');
                }
                else{
                    expect(e.matches).assertEqual(true);
                    console.info('MediaQuery addListener fail');
                    expect(test).assertEqual('fail');
                }
            }

            mMediaQueryList.addListener(minWidthMatch)
            console.info('MediaQuery addListener end');
       // }
    });

    it('matchMedia', 0, function() {
        console.info('matchMedia start');
        var mMediaQueryList = mediaquery.matchMedia('(min-height:0)');
        var med = mMediaQueryList.media
        console.info("media:"+med)
        mMediaQueryList.onchange = function(){
            console.info('MediaQuery detonate onchange')
        }
        function minWidthMatch(e){
            if(e.matches){
                console.info("MediaQuery matches:"+e.matches)
                console.info("MediaQuery onchangeMessage:"+e.onchange)
                console.info('MediaQuery matchMedia success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery matchMedia fail');
                expect(test).assertEqual('fail');
            }
            console.info('MediaQuery matchMedia end');
        }
        mMediaQueryList.addListener(minWidthMatch);
    });

    it('orientation', 0, function() {
        console.info('orientation test start');
        var mMediaQueryList = mediaquery.matchMedia('(orientation:landscape)');
        var med = mMediaQueryList.media
        console.info("orientation onchangeMessage:"+mMediaQueryList.onchange)
        function orientationMatch(e){
            if(e.matches){
                //do something
                console.info("MediaQuery orientation matches:"+e.matches)
                console.info('MediaQuery orientation success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery orientation fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(orientationMatch)
        console.info('MediaQuery orientation test end')
    });

    it('rmeoveListener', 0, function() {
        console.info('rmeoveListener start');
        var mMediaQueryList = mediaquery.matchMedia('(max-width:466)');
        function maxWidthMatch(e){
            if(e.matches){
                //do something
                console.info('MediaQuery rmeoveListener success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery rmeoveListener fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(maxWidthMatch)
        mMediaQueryList.removeListener(maxWidthMatch)
        console.info('MediaQuery removeListener end')
    });

    it('maxHeight', 0, function() {
        console.info('maxHeight start');
        var mMediaQueryList = mediaquery.matchMedia('(max-width:10000)');
//        mMediaQueryList.onreadystatechange = function(){
            function maxHeightMatch(e){
                if(e.matches){
                    //do something
                    console.info('MediaQuery maxHeight success');
                    expect(test).assertEqual('success');
                }
                else{
                    console.info('MediaQuery maxHeight fail');
                    expect(test).assertEqual('fail');
                }
            }
       // }

        mMediaQueryList.addListener(maxHeightMatch)
        console.info('MediaQuery maxHeight end')
    });

    it('deviceType', 0, function() {
        console.info('deviceType start');
        var mMediaQueryList = mediaquery.matchMedia('(deviceType:phone)');
        function deviceTypeMatch(e){
            if(e.matches){
                //do something
                console.info('MediaQuery deviceType success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery deviceType fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(deviceTypeMatch)
        console.info('MediaQuery deviceType end')
    });

    it('logicOnly', 0, function() {
        console.info('MediaQuery logicOnly start');
        var mMediaQueryList = mediaquery.matchMedia('only screen and(deviceType:phone)and(max-height:100000)');
//        mMediaQueryList.onreadystatechange = function(){
        function logicOnlyMatch(e){
            if(e.matches){
                //do something
                console.info('MediaQuery logicOnly success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery logicOnly fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicOnlyMatch)
        console.info('MediaQuery logicOnly end')
        //}
    });

    it('logicNot', 0, function() {
        console.info('logicOnly start');
        var mMediaQueryList = mediaquery.matchMedia('not screen and(deviceType:tv)');
//      mMediaQueryList.onreadystatechange = function(){
        function logicNotMatch(e){
            if(e.matches){
                //do something
                console.info('MediaQuery logicNot success');
                expect(test).assertEqual('success');
            }
            else{
                console.info('MediaQuery logicNot fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicNotMatch)
        console.info('MediaQuery logicNot end')
        //}
    });

    it('logicAnd', 0, function() {
        console.info('MediaQuery logicAnd start');
        var mMediaQueryList = mediaquery.matchMedia('screen and(deviceType:phone)');
        function logicAndMatch(e){
            if(e.matches){
                //do something
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicAnd success');
                expect(test).assertEqual('success');
            }
            else{
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicAnd fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicAndMatch)
        console.info('MediaQuery logicAnd end')
    });

    it('logicComma', 0, function() {
        console.info('MediaQuery logicComma start');
        var mMediaQueryList = mediaquery.matchMedia('screen and(min-height:0),(round-screen:true)');
        function logicCommaMatch(e){
            if(e.matches){
                //do something
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicComma success');
                expect(test).assertEqual('success');
            }
            else{
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicComma fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicCommaMatch)
        console.info('MediaQuery logicComma end')
    });

    it('logicOr', 0, function() {
        console.info('MediaQuery logicOr start');
        var mMediaQueryList = mediaquery.matchMedia('screen and(max-device-height:2000)or(round-screen:true)');
        function logicOrMatch(e){
            if(e.matches){
                //do something
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicOr success');
                expect(test).assertEqual('success');
            }
            else{
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logicOr fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicOrMatch)
        console.info('MediaQuery logicOr end')
    });

    it('logic1', 0, function() {
        console.info('MediaQuery logic>= start');
        var mMediaQueryList = mediaquery.matchMedia('screen and(height>=0)');
        function logicOrMatch(e){
            if(e.matches){
                //do something
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logic>= success');
                expect(test).assertEqual('success');
            }
            else{
                expect(e.matches).assertEqual(true)
                console.info('MediaQuery logic>= fail');
                expect(test).assertEqual('fail');
            }
        }
        mMediaQueryList.addListener(logicOrMatch)
        console.info('MediaQuery logic>= end')
    })
});