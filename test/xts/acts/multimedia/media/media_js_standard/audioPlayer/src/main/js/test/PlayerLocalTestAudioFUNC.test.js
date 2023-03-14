/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import media from '@ohos.multimedia.media'
import fileIO from '@ohos.fileio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('PlayerLocalTestAudioFUNC', function () {
    let audioPlayer = media.createAudioPlayer();
    let isTimeOut = false;
    const AUDIO_SOURCE = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audioplayer/01.mp3';
    const PLAY_TIME = 3000;
    const DURATION_TIME = 219600;
    const SEEK_TIME = 5000;
    const DELTA_TIME  = 1000;
    const END_STATE = 0;
    const SRC_STATE = 1;
    const PLAY_STATE = 2;
    const PAUSE_STATE = 3;
    const STOP_STATE = 4;
    const RESET_STATE = 5;
    const SEEK_STATE = 6;
    const VOLUME_STATE = 7;
    const RELEASE_STATE = 8;
    const ERROR_STATE = 9;
    const FINISH_STATE = 10;
    const LOOP_STATE = 11;
    const SECOND_INDEX = 1;
    const RAND_NUM = 5;
    const TIME_OUT = 40000;
    const MAX_VOLUME = 1;
    let fdPath;
    let fdValue;

    beforeAll(async function() {
        await getFd();
        console.info('beforeAll case');
    })

    beforeEach(function() {
        isTimeOut = false;
        console.info('beforeEach case');
    })

    afterEach(function() {
        console.info('afterEach case');
    })

    afterAll(async function() {
        await fileIO.close(fdValue);
        console.info('afterAll case');
    })

    function sleep(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    async function getFd() {
        fdPath = 'fd://';
        await fileIO.open(AUDIO_SOURCE).then((fdNumber) => {
            fdPath = fdPath + '' + fdNumber;
            fdValue = fdNumber;
            console.info('[fileIO]case open fd success,fdPath is ' + fdPath);
        }, (err) => {
            console.info('[fileIO]case open fd failed');
        }).catch((err) => {
            console.info('[fileIO]case catch open fd failed');
        });
    }

    function initAudioPlayer() {
        if (typeof (audioPlayer) != 'undefined') {
            audioPlayer.release();
            audioPlayer = undefined;
        }
        audioPlayer = media.createAudioPlayer();
        if (typeof (audioPlayer) == 'undefined') {
            console.info('case create player is faild');
            expect().assertFail();
        }
    }

    function nextStep(mySteps,done) {
        if (mySteps[0] == END_STATE) {
            done();
            console.info('case to done');
            return;
        }
        switch (mySteps[0]) {
            case SRC_STATE:
                console.info(`case to prepare`);
                audioPlayer.src = fdPath;
                break;
            case PLAY_STATE:
                console.info(`case to play`);
                audioPlayer.play();
                break;
            case PAUSE_STATE:
                console.info(`case to pause`);
                audioPlayer.pause();
                break;
            case STOP_STATE:
                console.info(`case to stop`);
                audioPlayer.stop();
                break;
            case RESET_STATE:
                console.info(`case to reset`);
                audioPlayer.reset();
                break;
            case SEEK_STATE:
                console.info(`case seek to time is ${mySteps[SECOND_INDEX]}`);
                audioPlayer.seek(mySteps[SECOND_INDEX]);
                break;
            case VOLUME_STATE:
                console.info(`case to setVolume`);
                audioPlayer.setVolume(mySteps[SECOND_INDEX]);
                break;
            case RELEASE_STATE:
                console.info(`case to release`);
                mySteps.shift();
                audioPlayer.release();
                audioPlayer = undefined;
                break;
            case LOOP_STATE:
                audioPlayer.loop = mySteps[SECOND_INDEX];
                mySteps.shift();
                mySteps.shift();
                nextStep(mySteps,done);
                break;
            default:
                break;
        }
    }

    function setCallback(mySteps, done) {
        console.info(`case setCallback`);
        audioPlayer.on('bufferingUpdate', (infoType, value) => {
            console.info('case bufferingUpdate success infoType is ' + infoType);
            console.info('case bufferingUpdate success value is ' + value);
        });
        audioPlayer.on('dataLoad', () => {
            mySteps.shift();
            console.info(`case dataLoad called`);
            expect(audioPlayer.currentTime).assertEqual(0);
            expect(audioPlayer.duration).assertEqual(DURATION_TIME);
            expect(audioPlayer.state).assertEqual('paused');
            nextStep(mySteps,done);
        });
        audioPlayer.on('play', () => {
            mySteps.shift();
            console.info(`case play called`);
            sleep(PLAY_TIME);
            console.info(`case play currentTime is ${audioPlayer.currentTime}`);
            expect(audioPlayer.duration).assertEqual(DURATION_TIME);
            if (mySteps[0] == FINISH_STATE) {
                console.info(`case wait for finish`);
                return;
            }
            expect(audioPlayer.state).assertEqual('playing');
            nextStep(mySteps,done);
        });
        audioPlayer.on('pause', () => {
            mySteps.shift();
            console.info(`case pause called`);
            console.info(`case pause currentTime is ${audioPlayer.currentTime}`);
            expect(audioPlayer.duration).assertEqual(DURATION_TIME);
            expect(audioPlayer.state).assertEqual('paused');
            nextStep(mySteps,done);
        });
        audioPlayer.on('reset', () => {
            mySteps.shift();
            console.info(`case reset called`);
            expect(audioPlayer.state).assertEqual('idle');
            nextStep(mySteps,done);
        });
        audioPlayer.on('stop', () => {
            if (mySteps[0] == RESET_STATE) {
                console.info(`case reset stop called`);
                return;
            }
            mySteps.shift();
            console.info(`case stop called`);
            expect(audioPlayer.currentTime).assertEqual(0);
            expect(audioPlayer.duration).assertEqual(DURATION_TIME);
            expect(audioPlayer.state).assertEqual('stopped');
            nextStep(mySteps,done);
        });
        audioPlayer.on('timeUpdate', (seekDoneTime) => {
            if (typeof (seekDoneTime) == 'undefined') {
                console.info(`case seek filed,errcode is ${seekDoneTime}`);
                return;
            }
            if (mySteps[0] != SEEK_STATE) {
                return;
            }
            mySteps.shift();
            mySteps.shift();
            console.info(`case seekDoneTime is ${seekDoneTime}`);
            console.info(`case seek called`);
            expect(audioPlayer.currentTime + DELTA_TIME).assertClose(seekDoneTime + DELTA_TIME, DELTA_TIME);
            console.info(`case loop is ${audioPlayer.loop}`);
            if ((audioPlayer.loop == true) && (seekDoneTime == DURATION_TIME)) {
                console.info('case loop is true');
                sleep(PLAY_STATE);
            }
            if ((seekDoneTime < audioPlayer.duration) || (audioPlayer.state == 'paused')) {
                nextStep(mySteps,done);
            }
        });
        audioPlayer.on('volumeChange', () => {
            console.info(`case setvolume called`);
            mySteps.shift();
            mySteps.shift();
            if (audioPlayer.state == 'playing') {
                sleep(PLAY_TIME);
            }
            nextStep(mySteps,done);
        });
        audioPlayer.on('finish', () => {
            mySteps.shift();
            expect(audioPlayer.state).assertEqual('stopped');
            expect(audioPlayer.currentTime).assertClose(audioPlayer.duration, DELTA_TIME);
            console.info(`case finish called`);
            nextStep(mySteps,done);
        });
        audioPlayer.on('error', (err) => {
            console.info(`case error called,errName is ${err.name}`);
            console.info(`case error called,errCode is ${err.code}`);
            console.info(`case error called,errMessage is ${err.message}`);
            if ((mySteps[0] == SEEK_STATE) || (mySteps[0] == VOLUME_STATE)) {
                mySteps.shift();
                mySteps.shift();
                mySteps.shift();
            } else {
                mySteps.shift();
                mySteps.shift();
            }
            nextStep(mySteps,done);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0100
        * @tc.name      : 001.play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0100', 0, async function (done) {
        console.info(`case update`);
        let mySteps = new Array(SRC_STATE, PLAY_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0200
        * @tc.name      : 002.play->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTestTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0300
        * @tc.name      : 003.play->pause->play->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0500
        * @tc.name      : 005.play->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0500', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0600
        * @tc.name      : 006.play->pause->play->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0600', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, PAUSE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0700
        * @tc.name      : 007.play->pause->stop
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0700', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0800
        * @tc.name      : 008.play->pause->play->stop
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0800', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0900
        * @tc.name      : 009.play->stop->reset->play->pause->play->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_0900', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, RESET_STATE, SRC_STATE, PLAY_STATE,
            PAUSE_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1000
        * @tc.name      : 010.play->pause->stop->reset->play->pause->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1000', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, STOP_STATE, RESET_STATE, SRC_STATE, PLAY_STATE,
            PAUSE_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1100
        * @tc.name      : 011.play->stop->reset->play->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, RESET_STATE,
            SRC_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1300
        * @tc.name      : 013.play->stop->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, PAUSE_STATE, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1400
        * @tc.name      : 014.play->seek->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, 0, PAUSE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1500
        * @tc.name      : 015.play->pause->seek(duration)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1500', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, DURATION_TIME,
            PLAY_STATE, FINISH_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1600
        * @tc.name      : 016.play->pause->play->seek(0)->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1600', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, SEEK_STATE, 0, PAUSE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1700
        * @tc.name      : 017.play->seek(0)->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1700', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, 0, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1800
        * @tc.name      : 018.play->seek->stop->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1800', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, 0, STOP_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1900
        * @tc.name      : 019.play->stop->seek
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_1900', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, SEEK_STATE, 0, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2000
        * @tc.name      : 020.play->pause->seek->reset
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2000', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, 0, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2100
        * @tc.name      : 021.play->pause->seek->stop
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, DURATION_TIME / RAND_NUM,
            STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2200
        * @tc.name      : 022.play->pause->play->seek(0)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, SEEK_STATE, 0, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2300
        * @tc.name      : 023.play->pause->play->seek(0)->stop
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, SEEK_STATE, 0, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2400
        * @tc.name      : 024.play->stop->seek(0)->reset->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, SEEK_STATE, 0, ERROR_STATE,
            RESET_STATE, SRC_STATE, PLAY_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2500
        * @tc.name      : 025.play->seek(0)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2500', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, 0, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2600
        * @tc.name      : 026.play->stop->seek->pause
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level 3
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2600', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, SEEK_STATE, SEEK_TIME, ERROR_STATE,
            PAUSE_STATE, ERROR_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2700
        * @tc.name      : 027.play->stop->seek->reset->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level 3
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2700', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, SEEK_STATE, SEEK_TIME, ERROR_STATE,
            RESET_STATE, SRC_STATE, PLAY_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2800
        * @tc.name      : 028.play->seek(duration)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2800', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, DURATION_TIME, FINISH_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2900
        * @tc.name      : 029.play->seek(out of duration)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level 3
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_2900', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, DURATION_TIME + DELTA_TIME,
            FINISH_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3000
        * @tc.name      : 030.play->->seek(rand)
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3000', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, DURATION_TIME / RAND_NUM, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3200
        * @tc.name      : 032.play->pause->seek(0)->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, 0, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3300
        * @tc.name      : 033.play->pause->seek(duration)->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, DURATION_TIME, PLAY_STATE,
            FINISH_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3400
        * @tc.name      : 034.play->pause->seek(out of duratin)->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level 3
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, DURATION_TIME + DELTA_TIME, PLAY_STATE,
            FINISH_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3500
        * @tc.name      : 035.play->seek(out of duratin)->play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level 3
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3500', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, DURATION_TIME + DELTA_TIME,
            FINISH_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3600
        * @tc.name      : 036.Loop Play
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3600', 0, async function (done) {
        let playCount = 0;
        let seekCount = 0;
        let isTimeDone = false;
        let testAudioPlayer = media.createAudioPlayer();
        testAudioPlayer.on('dataLoad', () => {
            expect(testAudioPlayer.currentTime).assertEqual(0);
            expect(testAudioPlayer.duration).assertEqual(DURATION_TIME);
            expect(testAudioPlayer.state).assertEqual('paused');
            testAudioPlayer.loop = true;
            testAudioPlayer.play();
        });
        testAudioPlayer.on('play', () => {
            console.info(`case play currentTime is ${testAudioPlayer.currentTime}`);
            expect(testAudioPlayer.duration).assertEqual(DURATION_TIME);
            expect(testAudioPlayer.state).assertEqual('playing');
            sleep(PLAY_TIME);
            if (playCount > 0) {
                return;
            }
            playCount++
            testAudioPlayer.seek(DURATION_TIME);
        });
        testAudioPlayer.on('timeUpdate', (seekDoneTime) => {
            if (typeof (seekDoneTime) == 'undefined') {
                console.info(`case seek filed,errcode is ${seekDoneTime}`);
                return;
            }
            if (seekCount > 0) {
                testAudioPlayer.reset();
                return;
            }
            seekCount++
            console.info(`case seekDoneTime is ${seekDoneTime}`);
        });
        testAudioPlayer.on('finish', () => {
            expect(testAudioPlayer.state).assertEqual('playing');
        });
        testAudioPlayer.on('reset', () => {
            expect(testAudioPlayer.state).assertEqual('idle');
            testAudioPlayer.release();
            isTimeDone = true;
        });
        testAudioPlayer.src = fdPath;
        setTimeout(function() {
            if (!isTimeDone) {
                expect(isTimeOut).assertTrue();
            }
            done();
        }, TIME_OUT);
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3700
        * @tc.name      : 037.set volume 0 to 1
        * @tc.desc      : Audio playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_PLAYER_LOCAL_AUDIO_FUNCTION_04_3700', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, VOLUME_STATE, 0,
            VOLUME_STATE, MAX_VOLUME, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })
})
