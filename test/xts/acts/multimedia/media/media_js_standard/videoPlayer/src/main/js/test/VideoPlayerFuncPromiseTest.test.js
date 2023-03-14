/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import media from '@ohos.multimedia.media'
import router from '@system.router'
import fileIO from '@ohos.fileio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('VideoPlayerFuncPromiseTest', function () {
    const AUDIO_SOURCE = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoplayer/H264_AAC.mp4';
    const PLAY_TIME = 3000;
    const SEEK_TIME = 5000;
    const SEEK_CLOSEST = 3;
    const WIDTH_VALUE = 720;
    const HEIGHT_VALUE = 480;
    const DURATION_TIME = 10034;
    const DELTA_TIME = 1000;
    const NEXT_FRAME_TIME = 8333;
    const PREV_FRAME_TIME = 4166;
    let surfaceID = '';
    let fdPath;
    let fdValue;
    let temp = 0;
    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(async function() {
        console.info('case flush surfaceID start');
        await toNewPage();
        console.info('case flush surfaceID end');
        await msleep(1000).then(() => {
        }, failureCallback).catch(catchCallback);
        console.info('beforeEach case');
    })

    afterEach(async function() {
        await router.clear();
        console.info('afterEach case');
    })

    afterAll(async function() {
        await fileIO.close(fdValue);
        console.info('afterAll case');
    })

    function sleep(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }

    function failureCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    function catchCallback(error) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
    }

    async function toNewPage() {
        let path = '';
        if (temp == 0) {
            path = 'pages/surfaceTest/surfaceTest';
            temp = 1;
        } else {
            path = 'pages/surfaceTest/surfaceTest2';
            temp = 0;
        }
        let options = {
            uri: path,
        }
        try {
            let result = await router.push(options);
        } catch {
            console.info('case route failed');
        }
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

    function printfDescription(obj) { 
        let description = ""; 
        for(let i in obj) { 
            let property = obj[i];
            console.info('case key is  '+ i);
            console.info('case value is  '+ property);
            description += i + " = " + property + "\n"; 
        }
    }

    function checkSpeedTime(videoPlayer, speedValue, startTime) {
        let newTime = videoPlayer.currentTime;
        if (videoPlayer.state == 'playing') {
            switch (speedValue) {
                case media.PlaybackSpeed.SPEED_FORWARD_0_75_X:
                    expect(newTime - startTime).assertClose(0.75 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_00_X:
                    expect(newTime - startTime).assertClose(1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_25_X:
                    expect(newTime - startTime).assertClose(1.25 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_1_75_X:
                    expect(newTime - startTime).assertClose(1.75 * 1000, DELTA_TIME);
                    break;
                case media.PlaybackSpeed.SPEED_FORWARD_2_00_X:
                    expect(newTime - startTime).assertClose(2 * 1000, DELTA_TIME);
                    break;
            }
        } else {
            console.info('case speed not in play');
        }
    }

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_MULTIPLE_0100
        * @tc.name      : 001.Multi-instance (2 videoPlayer promise) 
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_PLAYER_MULTIPLE_0100', 0, async function (done) {
        await getFd();
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let testVideoPlayer1 = null;
        let testVideoPlayer2 = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                testVideoPlayer1 = video
                console.info('case createVideoPlayer success ');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        testVideoPlayer1.url = fdPath;
        await testVideoPlayer1.setDisplaySurface(surfaceID).then(() => {
            console.info('case setDisplaySurface success');
            expect(testVideoPlayer1.state).assertEqual('idle');
        }, failureCallback).catch(catchCallback);

        await testVideoPlayer1.prepare().then(() => {
            console.info('case prepare called!!');
            expect(testVideoPlayer1.state).assertEqual('prepared');
            expect(testVideoPlayer1.duration).assertEqual(DURATION_TIME);
            expect(testVideoPlayer1.width).assertEqual(WIDTH_VALUE);
            expect(testVideoPlayer1.height).assertEqual(HEIGHT_VALUE);
        }, failureCallback).catch(catchCallback);

        let startTime = testVideoPlayer1.currentTime;
        await testVideoPlayer1.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(testVideoPlayer1.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = testVideoPlayer1.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await testVideoPlayer1.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        testVideoPlayer1 = null;

        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                testVideoPlayer2 = video
                console.info('case createVideoPlayer success ');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        testVideoPlayer2.url = fdPath;
        await testVideoPlayer2.setDisplaySurface(surfaceID).then(() => {
            console.info('case setDisplaySurface success');
            expect(testVideoPlayer2.state).assertEqual('idle');
        }, failureCallback).catch(catchCallback);

        await testVideoPlayer2.prepare().then(() => {
            console.info('case prepare called!!');
            expect(testVideoPlayer2.state).assertEqual('prepared');
            expect(testVideoPlayer2.duration).assertEqual(DURATION_TIME);
            expect(testVideoPlayer2.width).assertEqual(WIDTH_VALUE);
            expect(testVideoPlayer2.height).assertEqual(HEIGHT_VALUE);
        }, failureCallback).catch(catchCallback);

        startTime = testVideoPlayer2.currentTime;
        await testVideoPlayer2.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(testVideoPlayer2.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        endTime = testVideoPlayer2.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await testVideoPlayer2.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        testVideoPlayer2 = null;
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0100
        * @tc.name      : 001.play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                console.info('case createVideoPlayer success');
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            console.info('case setDisplaySurface success');
            expect(videoPlayer.state).assertEqual('idle');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            console.info('case prepare called!!');
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
        }, failureCallback).catch(catchCallback);

        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0200
        * @tc.name      : 002.pause (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            console.info('case setDisplaySurface success');
            expect(videoPlayer.state).assertEqual('idle');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            console.info('case prepare called!!');
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();        
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0300
        * @tc.name      : 003.pause->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0400
        * @tc.name      : 004.stop (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0500
        * @tc.name      : 005.reset (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.reset().then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case reset called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0600
        * @tc.name      : 006.seek (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(SEEK_TIME).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0700
        * @tc.name      : 007.seek CLOSEST(promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(SEEK_TIME, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(SEEK_TIME, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(PREV_FRAME_TIME - 100, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(0);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(PREV_FRAME_TIME + 100, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(NEXT_FRAME_TIME - 100, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(NEXT_FRAME_TIME + 100, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME + 100);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done(); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0800
        * @tc.name      : 008.setVolume (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.setVolume(1).then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case setVolume called');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0900
        * @tc.name      : 009.setSpeed (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_0900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);
        
        startTime = videoPlayer.currentTime;
        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_2_00_X).then((speedMode) => {
            expect(videoPlayer.state).assertEqual('playing');
            sleep(1000);
            expect(speedMode).assertEqual(media.PlaybackSpeed.SPEED_FORWARD_2_00_X);
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(2 * 1000, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1000
        * @tc.name      : 010.getTrackDescription (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        let arrayDescription = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.getTrackDescription().then((arrayList) => {
            console.info('case getTrackDescription called!!');
            if (typeof (arrayList) != 'undefined') {
                arrayDescription = arrayList;
            } else {
                console.info('case getTrackDescription is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        for (let i = 0; i < arrayDescription.length; i++) {
            printfDescription(arrayDescription[i]);
        }

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1100
        * @tc.name      : 011.Loop true (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            videoPlayer.loop = true;
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.loop).assertEqual(true);
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(DURATION_TIME, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(DURATION_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1200
        * @tc.name      : 012.play->pause->stop (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1300
        * @tc.name      : 013.play->pause->reset (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.reset().then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case reset called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();      
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1400
        * @tc.name      : 014.play->pause->seek->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.seek(SEEK_TIME).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('paused');
            expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);
        
        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);
        
        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1500
        * @tc.name      : 015.play->pause->seek(mode)->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.seek(SEEK_TIME, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('paused');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(SEEK_TIME, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('paused');
            expect(seekDoneTime).assertEqual(PREV_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);
        
        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);
        
        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1600
        * @tc.name      : 016.play->pause->setvolume->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.setVolume(1).then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case setVolume called');
        }, failureCallback).catch(catchCallback);
        
        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1700
        * @tc.name      : 017.play->pause->setspeed->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_2_00_X).then((speedMode) => {
            expect(videoPlayer.state).assertEqual('paused');
            expect(speedMode).assertEqual(media.PlaybackSpeed.SPEED_FORWARD_2_00_X);
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);
        
        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            expect(videoPlayer.state).assertEqual('playing');
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1800
        * @tc.name      : 018.play->stop->reset (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1800', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.reset().then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case reset called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();       
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1900
        * @tc.name      : 019.play->stop->reset->prepare->play (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_1900', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.reset().then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case reset called!!');
        }, failureCallback).catch(catchCallback);
        videoPlayer.url = fdPath;
        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2000
        * @tc.name      : 020.play->seek(0)->pause->stop (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2000', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(0).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(0);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();             
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2100
        * @tc.name      : 021.play->seek(0, mode)->pause->stop (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2100', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(0, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(0);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(0, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(0);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);
        
        await videoPlayer.pause().then(() => {
            expect(videoPlayer.state).assertEqual('paused');
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.stop().then(() => {
            expect(videoPlayer.state).assertEqual('stopped');
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2200
        * @tc.name      : 022.play->seek(duration)->finish (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2200', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.on('playbackCompleted', () => {
            console.info('case playbackCompleted called!!');
            expect(videoPlayer.state).assertEqual('stopped');
            videoPlayer.release().then(() => {
                console.info('case release called!!');
                done();
            }, failureCallback).catch(catchCallback);
        });

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(DURATION_TIME).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2300
        * @tc.name      : 023.play->seek(duration, mode)->finish (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2300', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.on('playbackCompleted', () => {
            console.info('case playbackCompleted called!!');
            expect(videoPlayer.state).assertEqual('stopped');
            videoPlayer.release().then(() => {
                console.info('case release called!!');
                done();
            }, failureCallback).catch(catchCallback);
        });
        
        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(DURATION_TIME, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(DURATION_TIME, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(DURATION_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();           
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2400
        * @tc.name      : 024.play->seek(out of duration) (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2400', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.on('playbackCompleted', () => {
            console.info('case playbackCompleted called!!');
            expect(videoPlayer.state).assertEqual('stopped');
            videoPlayer.release().then(() => {
                console.info('case release called!!');
                done();
            }, failureCallback).catch(catchCallback);
        });

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(DURATION_TIME + 1).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, (err) => {
            console.info('case seek out of duration called');
            done();
        }).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2500
        * @tc.name      : 025.play->seek(out of duration, mode) (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2500', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.on('playbackCompleted', () => {
            console.info('case playbackCompleted called!!');
            expect(videoPlayer.state).assertEqual('stopped');
            videoPlayer.release().then(() => {
                console.info('case release called!!');
                done();
            }, failureCallback).catch(catchCallback);
        });

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.seek(DURATION_TIME + 1, media.SeekMode.SEEK_PREV_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(NEXT_FRAME_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(DURATION_TIME + 1, media.SeekMode.SEEK_NEXT_SYNC).then((seekDoneTime) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(seekDoneTime).assertEqual(DURATION_TIME);
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2600
        * @tc.name      : 026. play->setvolume(0~1) (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2600', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);
        
        for (let i = 0; i < 6; i++) {
            await videoPlayer.setVolume(i * 0.2).then(() => {
                expect(videoPlayer.state).assertEqual('playing');
                console.info('case setVolume called');
            }, failureCallback).catch(catchCallback);   
        }

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2700
        * @tc.name      : 028.play->setspeed(0~4) (promise)
        * @tc.desc      : Video playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_PLAYER_FUNCTION_PROMISE_2700', 0, async function (done) {
        surfaceID = globalThis.value;
        console.info('case new surfaceID is ' + surfaceID);
        let videoPlayer = null;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                expect(videoPlayer.state).assertEqual('idle');
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.url = fdPath;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            expect(videoPlayer.state).assertEqual('idle');
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            expect(videoPlayer.state).assertEqual('prepared');
            expect(videoPlayer.duration).assertEqual(DURATION_TIME);
            expect(videoPlayer.width).assertEqual(WIDTH_VALUE);
            expect(videoPlayer.height).assertEqual(HEIGHT_VALUE);
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(1000);
            expect(videoPlayer.state).assertEqual('playing');
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        startTime = videoPlayer.currentTime;
        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_2_00_X).then((speedMode) => {
            expect(videoPlayer.state).assertEqual('playing');
            expect(speedMode).assertEqual(media.PlaybackSpeed.SPEED_FORWARD_2_00_X);
            sleep(1000);
            checkSpeedTime(videoPlayer, media.PlaybackSpeed.SPEED_FORWARD_2_00_X, startTime);
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
        done();            
    })
})
