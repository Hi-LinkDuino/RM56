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
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'
import mediaLibrary from '@ohos.multimedia.mediaLibrary'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('RecorderLocalTestAudioFUNC', function () {
    let audioRecorder = media.createAudioRecorder();
    const BASIC_PATH = 'file:///data/accounts/account_0/appdata/recorder/'
    const END_STATE = 0;
    const PRE_STATE = 1;
    const START_STATE = 2;
    const PAUSE_STATE = 3;
    const RESUME_STATE = 4;
    const STOP_STATE = 5;
    const RESET_STATE = 6;
    const RELEASE_STATE = 7;
    const ERROR_STATE = 8;
    const RECORDER_TIME = 3000;
    const CHANNEL_ONE = 1;
    const CHANNEL_TWO = 2;
    const FORMAT_MP4 = 2;
    const FORMAT_M4A = 6;
    const SOURCE_TYPE = 1;
    const ENCORDER_AACLC = 3;
    let fdPath;
    let fileAsset;
    let fdNumber;
    let audioConfig = {
        audioSourceType : SOURCE_TYPE,
        audioEncoder : ENCORDER_AACLC,
        audioEncodeBitRate : 22050,
        audioSampleRate : 22050,
        numberOfChannels : CHANNEL_TWO,
        format : FORMAT_M4A,
        uri : 'file:///data/accounts/account_0/appdata/appdata/recorder/test.m4a',
        location : { latitude : 1, longitude : 1 },
    }

    function sleep(time) {
        for(let t = Date.now();Date.now() - t <= time;);
    }

    function initAudioRecorder() {
        if (typeof (audioRecorder) != 'undefined') {
            audioRecorder.release();
            audioRecorder = undefined;
        }
        audioRecorder = media.createAudioRecorder();
    }

    beforeAll(async function () {
        await applyPermission();
        console.info('beforeAll case');
    })

    beforeEach(function () {
        console.info('beforeEach case');
    })

    afterEach(async function () {
        await closeFd();
        console.info('afterEach case');
    })

    afterAll(function () {
        console.info('afterAll case');
    })

    async function applyPermission() {
        let appInfo = await bundle.getApplicationInfo('ohos.acts.multimedia.audio.audiorecorder', 0, 100);
        let atManager = abilityAccessCtrl.createAtManager();
        if (atManager != null) {
            let tokenID = appInfo.accessTokenId;
            console.info('[permission] case accessTokenID is ' + tokenID);
            let permissionName1 = 'ohos.permission.MICROPHONE';
            let permissionName2 = 'ohos.permission.MEDIA_LOCATION';
            let permissionName3 = 'ohos.permission.READ_MEDIA';
            let permissionName4 = 'ohos.permission.WRITE_MEDIA';
            await atManager.grantUserGrantedPermission(tokenID, permissionName1, 1).then((result) => {
                console.info('[permission] case grantUserGrantedPermission success :' + result);
            }).catch((err) => {
                console.info('[permission] case grantUserGrantedPermission failed :' + err);
            });
            await atManager.grantUserGrantedPermission(tokenID, permissionName2, 1).then((result) => {
                console.info('[permission] case grantUserGrantedPermission success :' + result);
            }).catch((err) => {
                console.info('[permission] case grantUserGrantedPermission failed :' + err);
            });
            await atManager.grantUserGrantedPermission(tokenID, permissionName3, 1).then((result) => {
                console.info('[permission] case grantUserGrantedPermission success :' + result);
            }).catch((err) => {
                console.info('[permission] case grantUserGrantedPermission failed :' + err);
            });
            await atManager.grantUserGrantedPermission(tokenID, permissionName4, 1).then((result) => {
                console.info('[permission] case grantUserGrantedPermission success :' + result);
            }).catch((err) => {
                console.info('[permission] case grantUserGrantedPermission failed :' + err);
            });
        } else {
            console.info('[permission] case apply permission failed, createAtManager failed');
        }
    }

    async function getFd(pathName) {
        let displayName = pathName;
        const mediaTest = mediaLibrary.getMediaLibrary();
        let fileKeyObj = mediaLibrary.FileKey;
        let mediaType = mediaLibrary.MediaType.VIDEO;
        let publicPath = await mediaTest.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
        let dataUri = await mediaTest.createAsset(mediaType, displayName, publicPath);
        if (dataUri != undefined) {
            let args = dataUri.id.toString();
            let fetchOp = {
                selections : fileKeyObj.ID + "=?",
                selectionArgs : [args],
            }
            let fetchFileResult = await mediaTest.getFileAssets(fetchOp);
            fileAsset = await fetchFileResult.getAllObject();
            fdNumber = await fileAsset[0].open('Rw');
            fdPath = "fd://" + fdNumber.toString();
        }
    }

    async function closeFd() {
        if (fileAsset != null) {
            await fileAsset[0].close(fdNumber).then(() => {
                console.info('[mediaLibrary] case close fd success');
            }).catch((err) => {
                console.info('[mediaLibrary] case close fd failed');
            });
        } else {
            console.info('[mediaLibrary] case fileAsset is null');
        }
    }

    function nextStep(mySteps,done) {
        if (mySteps[0] == END_STATE) {
            done();
            console.info('case to done');
            return;
        }
        switch (mySteps[0]) {
            case PRE_STATE:
                console.info('case to prepare');
                audioRecorder.prepare(audioConfig);
                break;
            case START_STATE:
                console.info('case to start');
                audioRecorder.start();
                break;
            case PAUSE_STATE:
                console.info('case to pause');
                audioRecorder.pause();
                break;
            case RESUME_STATE:
                console.info('case to resume');
                audioRecorder.resume();
                break;
            case STOP_STATE:
                console.info('case to stop');
                audioRecorder.stop();
                break;
            case RESET_STATE:
                console.info('case to reset');
                audioRecorder.reset();
                break;
            case RELEASE_STATE:
                console.info('case to release');
                audioRecorder.release();
                audioRecorder = undefined;
                break;
            case ERROR_STATE:
                console.info('case to wait error callback');
                break;
            default:
                break;
        }
    }

    function setCallback(mySteps, done) {
        audioRecorder.on('prepare', () => {
            console.info('setCallback prepare() case callback is called');
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('start', () => {
            console.info('setCallback start() case callback is called');
            sleep(RECORDER_TIME);
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('pause', () => {
            console.info('setCallback pause() case callback is called');
            sleep(RECORDER_TIME);
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('resume', () => {
            console.info('setCallback resume() case callback is called');
            sleep(RECORDER_TIME);
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('stop', () => {
            console.info('setCallback stop() case callback is called');
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('reset', () => {
            console.info('setCallback reset() case callback is called');
            mySteps.shift();
            nextStep(mySteps,done);
        });

        audioRecorder.on('release', () => {
            console.info('setCallback release() case callback is called');
            mySteps.shift();
            nextStep(mySteps,done);
        });
        audioRecorder.on('error', (err) => {
            console.info(`case error called,errName is ${err.name}`);
            console.info(`case error called,errCode is ${err.code}`);
            console.info(`case error called,errMessage is ${err.message}`);
            mySteps.shift();
            expect(mySteps[0]).assertEqual(ERROR_STATE);
            mySteps.shift();
            nextStep(mySteps,done);
        });  
    }

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0100
        * @tc.name      : 01.AAC
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0100', 0, async function (done) {
        await getFd('test1.m4a');
        audioConfig.uri = fdPath;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0210
        * @tc.name      : 02.AAC_Different Channels 1
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0210', 0, async function (done) {
        await getFd('test2.m4a');
        audioConfig.uri = fdPath;
        audioConfig.numberOfChannels = CHANNEL_ONE;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0220
        * @tc.name      : 02.AAC_Different Channels 2
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0220', 0, async function (done) {
        await getFd('test3.m4a');
        audioConfig.uri = fdPath;
        audioConfig.numberOfChannels = CHANNEL_TWO;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0310
        * @tc.name      : 03.AAC_DifferentSampleRate 8000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0310', 0, async function (done) {
        await getFd('test4.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 8000; // samplerate 8000
        audioConfig.numberOfChannels = CHANNEL_TWO;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0320
        * @tc.name      : 03.AAC_DifferentSampleRate 32000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0320', 0, async function (done) {
        await getFd('test5.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 32000; // samplerate 32000
        audioConfig.numberOfChannels = CHANNEL_ONE;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0330
        * @tc.name      : 03.AAC_DifferentSampleRate 44100
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0330', 0, async function (done) {
        await getFd('test6.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 44100; // samplerate 44100
        audioConfig.numberOfChannels = CHANNEL_TWO;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0340
        * @tc.name      : 03.AAC_DifferentSampleRate 64000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0340', 0, async function (done) {
        await getFd('test7.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 64000; // samplerate 64000
        audioConfig.numberOfChannels = CHANNEL_ONE;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0350
        * @tc.name      : 03.AAC_DifferentSampleRate 96000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0340', 0, async function (done) {
        await getFd('test8.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 96000; // samplerate 96000
        audioConfig.numberOfChannels = CHANNEL_TWO;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0410
        * @tc.name      : 04.AAC_DifferentBitrate 8000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0410', 0, async function (done) {
        await getFd('test9.mp4');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 22050; // samplerate 22050
        audioConfig.audioEncodeBitRate = 8000; // encodeBitrate 8000
        audioConfig.numberOfChannels = CHANNEL_TWO;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0420
        * @tc.name      : 04.AAC_DifferentBitrate 16000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0420', 0, async function (done) {
        await getFd('test10.mp4');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 44100; // samplerate 44100
        audioConfig.audioEncodeBitRate = 16000; // encodeBitrate 16000
        audioConfig.numberOfChannels = CHANNEL_ONE;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0430
        * @tc.name      : 04.AAC_DifferentBitrate 32000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0430', 0, async function (done) {
        await getFd('test11.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 44100; // samplerate 44100
        audioConfig.audioEncodeBitRate = 32000; // encodeBitrate 32000
        audioConfig.numberOfChannels = CHANNEL_ONE;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0440
        * @tc.name      : 04.AAC_DifferentBitrate 64000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0440', 0, async function (done) {
        await getFd('test12.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 44100; // samplerate 44100
        audioConfig.audioEncodeBitRate = 64000; // encodeBitrate 64000
        audioConfig.numberOfChannels = CHANNEL_TWO;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_CODEC_AAC_0450
        * @tc.name      : 04.AAC_DifferentBitrate 112000
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_CODEC_AAC_0450', 0, async function (done) {
        await getFd('test13.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioSampleRate = 44100; // samplerate 44100
        audioConfig.audioEncodeBitRate = 112000; // encodeBitrate 112000
        audioConfig.numberOfChannels = CHANNEL_TWO;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_FORMAT_MP4_0200
        * @tc.name      : 02.AAC,mp4
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_RECORDER_FORMAT_MP4_0200', 0, async function (done) {
        await getFd('test14.mp4');
        audioConfig.uri = fdPath;
        audioConfig.audioEncodeBitRate = 22050; // samplerate 22050
        audioConfig.audioSampleRate = 22050; // encodeBitrate 22050
        audioConfig.fileFormat = FORMAT_MP4;
        audioConfig.numberOfChannels = CHANNEL_TWO;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_Format_M4A_Function_0200
        * @tc.name      : 02.AAC,mp4
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_FORMAT_MP4_0200', 0, async function (done) {
        await getFd('test15.m4a');
        audioConfig.uri = fdPath;
        audioConfig.audioEncodeBitRate = 22050; // samplerate 22050
        audioConfig.audioSampleRate = 22050; // encodeBitrate 22050
        audioConfig.fileFormat = FORMAT_M4A;
        audioConfig.numberOfChannels = CHANNEL_TWO;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0100
        * @tc.name      : 001.start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0100', 0, async function (done) {
        await getFd('test16.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0200
        * @tc.name      : 002.pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0200', 0, async function (done) {
        await getFd('test17.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0300
        * @tc.name      : 003.pause->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0300', 0, async function (done) {
        await getFd('test18.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, RESUME_STATE,
            STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0500
        * @tc.name      : 005.reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0500', 0, async function (done) {
        await getFd('test19.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, RESET_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0600
        * @tc.name      : 006.pause->resume->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0600', 0, async function (done) {
        await getFd('test20.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, RESUME_STATE, PAUSE_STATE,
            STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0700
        * @tc.name      : 007.pause->stop->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0700', 0, async function (done) {
        await getFd('test21.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, STOP_STATE,
            RESET_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0800
        * @tc.name      : 008.pause->resume->stop->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0800', 0, async function (done) {
        await getFd('test22.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, RESUME_STATE, STOP_STATE,
            RESET_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0900
        * @tc.name      : 009.stop->reset->pause->resume->stop->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_0900', 0, async function (done) {
        await getFd('test23.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, RESET_STATE, PRE_STATE, START_STATE,
            PAUSE_STATE, RESUME_STATE, STOP_STATE, RESET_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1000
        * @tc.name      : 010.stop->reset->pause->stop->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1000', 0, async function (done) {
        await getFd('test24.m4a');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_M4A;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, RESET_STATE, PRE_STATE, START_STATE,
            PAUSE_STATE, STOP_STATE, RESET_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1100
        * @tc.name      : 011.start->reset->start->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1100', 0, async function (done) {
        await getFd('test25.mp4');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, RESET_STATE, PRE_STATE,
            START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1200
        * @tc.name      : 012.start->pause->start(error)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1200', 0, async function (done) {
        await getFd('test26.mp4');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, PAUSE_STATE, START_STATE, ERROR_STATE,
            STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1300
        * @tc.name      : 013.start->stop->pause(error)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1300', 0, async function (done) {
        await getFd('test27.mp4');
        audioConfig.uri = fdPath;
        audioConfig.fileFormat = FORMAT_MP4;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, PAUSE_STATE, ERROR_STATE,
            RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(audioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1400
        * @tc.name      : 014. API8 audioEncoderMime: audio/mp4a-latm,
        *                           fileFormat:mp4
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1400', 0, async function (done) {
        await getFd('test28.mp4');
        let newAudioConfig = {
            audioSourceType : SOURCE_TYPE,
            audioEncodeBitRate : 22050,
            audioSampleRate : 22050,
            numberOfChannels : CHANNEL_TWO,
            uri : 'fd://',
            location : { latitude : 1, longitude : 1 },
            audioEncoderMime : media.CodecMimeType.AUDIO_AAC,
            fileFormat : media.ContainerFormatType.CFT_MPEG_4,
        }
        newAudioConfig.uri = fdPath;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(newAudioConfig);
    })

    /* *
        * @tc.number    : SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1500
        * @tc.name      : 014. API8 audioEncoderMime: audio/mp4a-latm,
        *                           fileFormat:m4a
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_RECORDER_AUDIO_FUNCTION_06_1500', 0, async function (done) {
        await getFd('test29.mp4');
        let newAudioConfig = {
            audioSourceType : SOURCE_TYPE,
            audioEncodeBitRate : 22050,
            audioSampleRate : 22050,
            numberOfChannels : CHANNEL_TWO,
            uri : 'fd://',
            location : { latitude : 1, longitude : 1 },
            audioEncoderMime : media.CodecMimeType.AUDIO_AAC,
            fileFormat : media.ContainerFormatType.CFT_MPEG_4A,
        }
        newAudioConfig.uri = fdPath;
        initAudioRecorder();
        let mySteps = new Array(PRE_STATE, START_STATE, STOP_STATE, RELEASE_STATE, END_STATE);
        setCallback(mySteps, done);
        audioRecorder.prepare(newAudioConfig);
    })
})
