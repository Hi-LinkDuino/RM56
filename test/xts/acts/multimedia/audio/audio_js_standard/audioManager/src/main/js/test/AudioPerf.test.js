/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import audio from '@ohos.multimedia.audio';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioApiResponseTime', function () {
    console.info('AudioFrameworkTestPerf: Running AudioManger API Perfomance Test');
    var AUDIO_MEDIA = 3;
    var AUDIO_RINGTONE = 2;
    var MAX_VOL = 15;
    var LOW_VOL = 5;
    var MAXNUM = 1000;
    var err;

    beforeAll(function () {
        console.info('AudioFrameworkTestPerf: beforeAll: Prerequisites at the test suite level');
    })

    beforeEach(function () {
        console.info('AudioFrameworkTestPerf: beforeEach: Prerequisites at the test case level');
    })

    afterEach(function () {
        console.info('AudioFrameworkTestPerf: afterEach: Test case-level clearance conditions');
    })

    afterAll(function () {
        console.info('AudioFrameworkTestPerf: afterAll: Test suite-level cleanup condition');
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_001
        * @tc.name      : API Response - setVolume - Promise
        * @tc.desc      : Check setVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_001', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setVolume(AUDIO_RINGTONE,MAX_VOL);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;
        console.info(`AudioFrameworkTestPerf: Promise: setVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: setVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: setVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_002
        * @tc.name      : API Response - setVolume - CallBack
        * @tc.desc      : Check setVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_002', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setVolume(AUDIO_MEDIA,LOW_VOL,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;
        console.info(`AudioFrameworkTestPerf: CallBack: setVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: setVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: setVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_003
        * @tc.name      : API Response - getVolume - Promise
        * @tc.desc      : Check getVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_003', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getVolume(AUDIO_RINGTONE);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise: getVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_004
        * @tc.name      : API Response - getVolume - CallBack
        * @tc.desc      : Check getVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_004', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getVolume(AUDIO_MEDIA,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_005
        * @tc.name      : API Response - getMinVolume - Promise
        * @tc.desc      : Check getMinVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_005', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getMinVolume(AUDIO_MEDIA);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : getMinVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getMinVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getMinVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_006
        * @tc.name      : API Response - getMinVolume - CallBack
        * @tc.desc      : Check getMinVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_006', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getMinVolume(AUDIO_RINGTONE,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getMinVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getMinVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getMinVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })
    
    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_007
        * @tc.name      : API Response - getMaxVolume - Promise
        * @tc.desc      : Check getMaxVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_007', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getMaxVolume(AUDIO_MEDIA);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : getMaxVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getMaxVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getMaxVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_008
        * @tc.name      : API Response - getMaxVolume - CallBack
        * @tc.desc      : Check getMaxVolume API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_008', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getMaxVolume(AUDIO_RINGTONE,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getMaxVolume Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getMaxVolume Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getMaxVolume Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_009
        * @tc.name      : API Response - mute - Promise
        * @tc.desc      : Check mute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_009', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.mute(AUDIO_MEDIA,true);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : mute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: mute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: mute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_010
        * @tc.name      : API Response - mute - CallBack
        * @tc.desc      : Check mute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_010', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.mute(AUDIO_RINGTONE,true,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: mute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: mute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: mute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_011
        * @tc.name      : API Response - isMute - Promise
        * @tc.desc      : Check isMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_011', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isMute(AUDIO_MEDIA);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : isMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: isMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: isMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_012
        * @tc.name      : API Response - isMute - CallBack
        * @tc.desc      : Check isMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_012', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isMute(AUDIO_RINGTONE,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: isMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: isMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: isMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_013
        * @tc.name      : API Response - isActive - Promise
        * @tc.desc      : Check isActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_013', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isActive(AUDIO_MEDIA);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : isActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: isActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: isActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_014
        * @tc.name      : API Response - isActive - CallBack
        * @tc.desc      : Check isActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_014', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isActive(AUDIO_RINGTONE,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: isActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: isActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: isActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_015
        * @tc.name      : API Response - setMicrophoneMute - Promise
        * @tc.desc      : Check setMicrophoneMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_015', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setMicrophoneMute(true);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : setMicrophoneMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: setMicrophoneMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: setMicrophoneMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_016
        * @tc.name      : API Response - setMicrophoneMute - CallBack
        * @tc.desc      : Check setMicrophoneMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_016', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setMicrophoneMute(false,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: setMicrophoneMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: setMicrophoneMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: setMicrophoneMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_017
        * @tc.name      : API Response - isMicrophoneMute - Promise
        * @tc.desc      : Check isMicrophoneMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_017', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isMicrophoneMute();
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : isMicrophoneMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: isMicrophoneMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: isMicrophoneMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_018
        * @tc.name      : API Response - isMicrophoneMute - CallBack
        * @tc.desc      : Check isMicrophoneMute API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_018', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isMicrophoneMute((err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: isMicrophoneMute Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: isMicrophoneMute Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: isMicrophoneMute Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_019
        * @tc.name      : API Response - setRingerMode - Promise
        * @tc.desc      : Check setRingerMode API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_019', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setRingerMode(audio.AudioRingMode.RINGER_MODE_NORMAL);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : setRingerMode Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: setRingerMode Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: setRingerMode Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_020
        * @tc.name      : API Response - setRingerMode - CallBack
        * @tc.desc      : Check setRingerMode API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_020', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setRingerMode(audio.AudioRingMode.RINGER_MODE_VIBRATE,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: setRingerMode Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: setRingerMode Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: setRingerMode Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_021
        * @tc.name      : API Response - getRingerMode - Promise
        * @tc.desc      : Check getRingerMode API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_021', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getRingerMode();
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : getRingerMode Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getRingerMode Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getRingerMode Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_022
        * @tc.name      : API Response - getRingerMode - CallBack
        * @tc.desc      : Check getRingerMode API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_022', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getRingerMode((err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getRingerMode Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getRingerMode Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getRingerMode Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_023
        * @tc.name      : API Response - setAudioParameter - Promise
        * @tc.desc      : Check setAudioParameter API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_023', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setAudioParameter('AudioPerTemp1', '8 bit');
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : setAudioParameter Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: setAudioParameter Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: setAudioParameter Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_024
        * @tc.name      : API Response - setAudioParameter - CallBack
        * @tc.desc      : Check setAudioParameter API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_024', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setAudioParameter('AudioPerTemp2', '44100',(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: setAudioParameter Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: setAudioParameter Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: setAudioParameter Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_025
        * @tc.name      : API Response - getAudioParameter - Promise
        * @tc.desc      : Check getAudioParameter API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_025', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getAudioParameter('AudioPerTemp1');
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : getAudioParameter Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getAudioParameter Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getAudioParameter Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_026
        * @tc.name      : API Response - getAudioParameter - CallBack
        * @tc.desc      : Check getAudioParameter API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_026', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getAudioParameter('AudioPerTemp2',(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getAudioParameter Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getAudioParameter Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getAudioParameter Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_027
        * @tc.name      : API Response - getDevices - Promise
        * @tc.desc      : Check getDevices API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_027', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getDevices(audio.DeviceFlag.INPUT_DEVICES_FLAG);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : getDevices Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: getDevices Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: getDevices Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_028
        * @tc.name      : API Response - getDevices - CallBack
        * @tc.desc      : Check getDevices API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_028', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.getDevices(audio.DeviceFlag.OUTPUT_DEVICES_FLAG,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: getDevices Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: getDevices Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: getDevices Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_029
        * @tc.name      : API Response - setDeviceActive - Promise
        * @tc.desc      : Check setDeviceActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_029', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : setDeviceActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: setDeviceActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: setDeviceActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_030
        * @tc.name      : API Response - setDeviceActive - CallBack
        * @tc.desc      : Check setDeviceActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_030', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.setDeviceActive(audio.ActiveDeviceType.SPEAKER,true,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: setDeviceActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: setDeviceActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: setDeviceActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_031
        * @tc.name      : API Response - isDeviceActive - Promise
        * @tc.desc      : Check isDeviceActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_031', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isDeviceActive(audio.ActiveDeviceType.SPEAKER);
            };
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: Promise : isDeviceActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: Promise: isDeviceActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: Promise: isDeviceActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

    /* *
        * @tc.number    : SUB_AUDIO_MANAGER_Performance_032
        * @tc.name      : API Response - isDeviceActive - CallBack
        * @tc.desc      : Check isDeviceActive API Response time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
       
    it('SUB_AUDIO_MANAGER_Performance_032', 0, function (done) {
        let audioManagerPerf = audio.getAudioManager(); 
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            audioManagerPerf.isDeviceActive(audio.ActiveDeviceType.SPEAKER,(err, value) => {
                if (err) {
                    console.error(`AudioFrameworkTestPerf: callback : ERROR : ${err.message}`);
                    expect(false).assertTrue();
                }
            });
        }
        let waitTime = new Date().getTime()-startTime;
        let avgTime = waitTime/MAXNUM;                     
        console.info(`AudioFrameworkTestPerf: CallBack: isDeviceActive Wait Time :  ${waitTime}`);
        if (avgTime<10)
        {
            console.info(`AudioFrameworkTestPerf: CallBack: isDeviceActive Average Time: PASS : ${avgTime}`);
            expect(true).assertTrue();
        }
        else{
            console.info(`AudioFrameworkTestPerf: CallBack: isDeviceActive Average Time: FAIL : ${avgTime}`);
            expect(false).assertTrue();
        }
        done();
    })

})