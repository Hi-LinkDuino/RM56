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

describe('PlayerLocalTestAudioAPI', function () {
    let audioPlayer = media.createAudioPlayer();
    let loopValue = false;
    let isTimeOut = false;
    const AUDIO_SOURCE = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audioplayer/01.mp3';
    const PLAY_TIME = 3000;
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
    const DURATION_TIME = 219600;
    const SEEK_TIME = 5000;
    const DELTA_TIME  = 1000;
    const SECOND_INDEX = 1;
    const TIME_OUT = 20000;
    const VOLUME_VALUE = 0.5;
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
        for(let t = Date.now();Date.now() - t <= time;);
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
                nextStep(mySteps,done);
                break;
            case LOOP_STATE:
                loopValue = mySteps[SECOND_INDEX];
                mySteps.shift();
                mySteps.shift();
                audioPlayer.loop = loopValue;
                nextStep(mySteps,done);
                break;
            default:
                break;
        }
    }

    function setCallback(mySteps, done) {
        console.info(`case setCallback`);
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
                nextStep(mySteps,done);
            } else if (mySteps[0] == ERROR_STATE) {
                mySteps.shift();
            } else if (mySteps[0] == END_STATE) {
                console.info('case release player error');
            } else {
                mySteps.shift();
                mySteps.shift();
                nextStep(mySteps,done);
            }
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0100
        * @tc.name      : 01.pause->play
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0100', 0, async function (done) {
        console.info(`case update`);
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, PLAY_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0200
        * @tc.name      : 02.stop->play
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, PLAY_STATE, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0300
        * @tc.name      : 03.seek->play
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, SEEK_TIME, PLAY_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0400
        * @tc.name      : 04.reset->play
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Play_API_0400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, RESET_STATE, PLAY_STATE, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0100
        * @tc.name      : 01.createAudioPlayer->play
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0100', 0, async function (done) {
        let mySteps = new Array(PAUSE_STATE, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.pause();
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0200
        * @tc.name      : 02.play->pause
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0300
        * @tc.name      : 03.stop->pause
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0300', 0, async function (done) {
        let mySteps = new Array(PLAY_STATE, STOP_STATE, PAUSE_STATE, ERROR_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0400
        * @tc.name      : 04.seek->pause
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Pause_API_0400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, SEEK_TIME, PAUSE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0100
        * @tc.name      : 01.play->stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0200
        * @tc.name      : 02.pause->stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0300
        * @tc.name      : 03.seek->stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Stop_API_0300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, SEEK_TIME, STOP_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0100
        * @tc.name      : 01.play->seek
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, SEEK_TIME, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0200
        * @tc.name      : 02.pause->seek
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, SEEK_STATE, SEEK_TIME, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0300
        * @tc.name      : 03.seek(0)
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Seek_API_0300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, 0, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Reset_API_0100
        * @tc.name      : 01.play->reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Reset_API_0100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Reset_API_0200
        * @tc.name      : 02.pause->reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Reset_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, RESET_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0100
        * @tc.name      : 01.createAudioPlayer->setVolume
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0100', 0, async function (done) {
        var mySteps = new Array(VOLUME_STATE, VOLUME_VALUE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.setVolume(VOLUME_VALUE);
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0200
        * @tc.name      : 02.play->setVolume
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0200', 0, async function (done) {
        var mySteps = new Array(SRC_STATE, PLAY_STATE, VOLUME_STATE, VOLUME_VALUE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0300
        * @tc.name      : 03.pause->setVolume
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_SetVolume_API_0300', 0, async function (done) {
        var mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, VOLUME_STATE, VOLUME_VALUE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0100
        * @tc.name      : 01.play->release
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0100', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, RELEASE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0200
        * @tc.name      : 02.pause->release
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0200', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, PAUSE_STATE, RELEASE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0300
        * @tc.name      : 03.stop->release
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0300', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0400
        * @tc.name      : 04.seek->release
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, SEEK_STATE, SEEK_TIME, RELEASE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0400
        * @tc.name      : 05.reset->release
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Release_API_0400', 0, async function (done) {
        let mySteps = new Array(SRC_STATE, PLAY_STATE, RESET_STATE, RELEASE_STATE, END_STATE);
        initAudioPlayer();
        setCallback(mySteps, done);
        audioPlayer.src = fdPath;
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Time_API_0100
        * @tc.name      : 01.get parameters after createAudioPlayer
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Time_API_0100', 0, async function (done) {
        initAudioPlayer();
        expect(audioPlayer.src).assertEqual(undefined);
        expect(audioPlayer.duration).assertEqual(undefined);
        expect(audioPlayer.currentTime).assertEqual(0);
        expect(audioPlayer.state).assertEqual('idle');
        expect(audioPlayer.loop).assertEqual(false);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_AudioPlayer_Time_API_0200
        * @tc.name      : 02.get parameters after src
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_AudioPlayer_Time_API_0200', 0, async function (done) {
        initAudioPlayer();
        audioPlayer.src = fdPath;
        sleep(PLAY_TIME);
        expect(audioPlayer.src).assertEqual(fdPath);
        expect(audioPlayer.currentTime).assertEqual(0);
        expect(audioPlayer.duration).assertEqual(DURATION_TIME);
        expect(audioPlayer.state).assertEqual('paused');
        expect(audioPlayer.loop).assertEqual(false);
        done();
    })
})
