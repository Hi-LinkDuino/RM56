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
import Fileio from '@ohos.fileio'
import router from '@system.router'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('HttpPlayerFuncPromiseTest', function () {
    const BASE_PATH = 'http://123.57.24.33:8000/'
    const PLAY_TIME = 3000;
    const SEEK_TIME = 5000;
    const MAX_VOLUME = 1;
    const DELTA_TIME = 1000;
    let isToSeek = false;
    let isToDuration = false;
    let surfaceID = '';

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(async function() {
        await toNewPage();
        await msleep(1000).then(() => {
        }, failureCallback).catch(catchCallback);
        isToSeek = false;
        isToDuration = false;
        console.info('beforeEach case');
    })

    afterEach(async function() {
        await router.clear();
        console.info('afterEach case');
    })

    afterAll(function() {
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

    function printfDescription(obj) { 
        let description = ""; 
        for(let i in obj) { 
            let property = obj[i];
            console.info('case key is  '+ i);
            console.info('case value is  '+ property);
            description += i + " = " + property + "\n"; 
        } 
    }

    async function toNewPage() {
        let path = 'pages/surfaceTest/surfaceTest';
        let options = {
            uri: path,
        }
        try {
            let result = await router.push(options);
        } catch {
            console.info('case route failed');
        }
    }

    async function playAudioSource(audioSource, done) {
        let audioPlayer = media.createAudioPlayer();
        audioPlayer.on('dataLoad', () => {
            console.info('case set source success');
            expect(audioPlayer.state).assertEqual('paused');
            expect(audioPlayer.currentTime).assertEqual(0);
            audioPlayer.play();
        });
        audioPlayer.on('play', () => {
            console.info('case start to play');
            expect(audioPlayer.state).assertEqual('playing');
            sleep(PLAY_TIME);
            if (!isToSeek) {
                audioPlayer.pause();
            } else {
                audioPlayer.seek(SEEK_TIME);
            }
        });
        audioPlayer.on('pause', () => {
            console.info('case now is paused');
            expect(audioPlayer.state).assertEqual('paused');
            audioPlayer.setVolume(MAX_VOLUME);
        });
        audioPlayer.on('stop', () => {
            console.info('case stop success');
            expect(audioPlayer.state).assertEqual('stopped');
            audioPlayer.reset();
        });
        audioPlayer.on('reset', () => {
            console.info('case reset success');
            expect(audioPlayer.state).assertEqual('idle');
            audioPlayer.release();
            audioPlayer = undefined;
            done();
        });
        audioPlayer.on('timeUpdate', (seekDoneTime) => {
            if (typeof (seekDoneTime) == "undefined") {
                console.info(`case seek filed,errcode is ${seekDoneTime}`);
                audioPlayer.release();
                expect().assertFail();
                done();
                return;
            }
            console.info('case seek success, and seek time is ' + seekDoneTime);
            if (!isToDuration) {
                expect(SEEK_TIME).assertEqual(seekDoneTime);
                isToDuration = true;
                sleep(PLAY_TIME);
                audioPlayer.seek(audioPlayer.duration);
            } else {
                expect(audioPlayer.duration).assertEqual(seekDoneTime);
            }
        });
        audioPlayer.on('volumeChange', () => {
            console.info('case set volume value to ' + MAX_VOLUME);
            audioPlayer.play();
            isToSeek = true;
        });
        audioPlayer.on('finish', () => {
            console.info('case play end');
            expect(audioPlayer.state).assertEqual('stopped');
            audioPlayer.stop();
        });
        audioPlayer.on('error', (err) => {
            console.info(`case error called,errName is ${err.name}`);
            console.info(`case error called,errCode is ${err.code}`);
            console.info(`case error called,errMessage is ${err.message}`);
            audioPlayer.release();
            expect().assertFail();
            done();
        });
        audioPlayer.src = audioSource;
    }

    async function playVideoSource(path) {
        let videoPlayer = null;
        surfaceID = globalThis.value;
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                console.info('case createVideoPlayer success');
                videoPlayer = video;
            } else {
                console.info('case createVideoPlayer is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        videoPlayer.on('error', (err) => {
            console.info(`case error called,errMessage is ${err.message}`);
            expect().assertFail();
        });
        videoPlayer.url = path;
        await videoPlayer.setDisplaySurface(surfaceID).then(() => {
            console.info('case setDisplaySurface success');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.prepare().then(() => {
            console.info('case prepare called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.getTrackDescription().then((arrayList) => {
            console.info('case getTrackDescription called!!');
            if (typeof (arrayList) != 'undefined') {
                for (let i = 0; i < arrayList.length; i++) {
                    printfDescription(arrayList[i]);
                }
            } else {
                console.info('case getTrackDescription is failed');
                expect().assertFail();
            }
        }, failureCallback).catch(catchCallback);

        let startTime = videoPlayer.currentTime;
        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);
        let endTime = videoPlayer.currentTime;
        expect(endTime - startTime).assertClose(PLAY_TIME, DELTA_TIME);

        await videoPlayer.pause().then(() => {
            console.info('case pause called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.seek(SEEK_TIME).then((seekDoneTime) => {
            console.info('case seek called and seekDoneTime is' + seekDoneTime);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.play().then(() => {
            console.info('case play called!!');
            sleep(PLAY_TIME);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_0_75_X).then((speedMode) => {
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_00_X).then((speedMode) => {
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_25_X).then((speedMode) => {
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_75_X).then((speedMode) => {
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_2_00_X).then((speedMode) => {
            console.info('case setSpeed called and speedMode is ' + speedMode);
        }, failureCallback).catch(catchCallback);

        await videoPlayer.setVolume(1).then(() => {
            console.info('case setVolume called');
        }, failureCallback).catch(catchCallback);  

        await videoPlayer.stop().then(() => {
            console.info('case stop called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.reset().then(() => {
            console.info('case reset called!!');
        }, failureCallback).catch(catchCallback);

        await videoPlayer.release().then(() => {
            console.info('case release called!!');
        }, failureCallback).catch(catchCallback);
    }

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MP4_0100
        * @tc.name      : 001.H264_AAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MP4_0100', 0, async function (done) {
        let path = BASE_PATH + 'H264_AAC.mp4';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MP4_0200
        * @tc.name      : 002.H264_MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MP4_0200', 0, async function (done) {
        let path = BASE_PATH + 'H264_MP3.mp4';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_TS_0100
        * @tc.name      : 001.H264_AAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_TS_0100', 0, async function (done) {
        let path = BASE_PATH + 'H264_AAC.ts';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_TS_0200
        * @tc.name      : 002.H264_MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_TS_0200', 0, async function (done) {
        let path = BASE_PATH + 'H264_MP3.ts';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0100
        * @tc.name      : 001.H264_AAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0100', 0, async function (done) {
        let path = BASE_PATH + 'H264_AAC.mkv';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0200
        * @tc.name      : 002.H264_MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0200', 0, async function (done) {
        let path = BASE_PATH + 'H264_MP3.mkv';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0500
        * @tc.name      : 005.MPEG2_AAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0500', 0, async function (done) {
        let path = BASE_PATH + 'MPEG2_AAC.mkv';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0600
        * @tc.name      : 006.MPEG2_MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0600', 0, async function (done) {
        let path = BASE_PATH + 'MPEG2_MP3.mkv';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0800
        * @tc.name      : 008.MPEG4_MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_MKV_0800', 0, async function (done) {
        let path = BASE_PATH + 'MPEG4_MP3.mkv';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_WEBM_0100
        * @tc.name      : 001.VP8_VORBIS
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_PLAYER_HTTP_FORMAT_WEBM_0100', 0, async function (done) {
        let path = BASE_PATH + 'VP8_VORBIS.webm';
        await playVideoSource(path);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_MP3_0100
        * @tc.name      : 001.MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_MP3_0100', 0, async function (done) {
        let path = BASE_PATH + 'MP3.mp3';
        await playAudioSource(path, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_AAC_0100
        * @tc.name      : 001.AAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_AAC_0100', 0, async function (done) {
        let path = BASE_PATH + 'AAC.aac';
        await playAudioSource(path, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_WAV_0100
        * @tc.name      : 001.MP3
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_WAV_0100', 0, async function (done) {
        let path = BASE_PATH + 'PCM.wav';
        await playAudioSource(path, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_FLAC_0100
        * @tc.name      : 001.FLAC
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_FLAC_0100', 0, async function (done) {
        let path = BASE_PATH + 'FLAC.flac';
        await playAudioSource(path, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_M4A_0100
        * @tc.name      : 001.M4A
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_M4A_0100', 0, async function (done) {
        let path = BASE_PATH + 'AAC.m4a';
        await playAudioSource(path, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_OGG_0100
        * @tc.name      : 001.OGG
        * @tc.desc      : Http playback control test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_PLAYER_HTTP_FORMAT_OGG_0100', 0, async function (done) {
        let path = BASE_PATH + 'OGG.ogg';
        await playAudioSource(path, done);
    })
})
