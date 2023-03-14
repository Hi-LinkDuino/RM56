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
import camera from '@ohos.multimedia.camera'
import mediaLibrary from '@ohos.multimedia.mediaLibrary'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('VideoRecorderAPICallbackTest', function () {
    const RECORDER_TIME = 3000;
    const PAUSE_TIME = 1000;
    const END_EVENT = 'end';
    const CREATE_EVENT = 'create';
    const PREPARE_EVENT = 'prepare';
    const PREPARE_OLNYVIDEO_EVENT = 'prepare_only';
    const GETSURFACE_EVENT = 'getInputSurface';
    const START_EVENT = 'start';
    const PAUSE_EVENT = 'pause';
    const RESUME_EVENT = 'resume';
    const STOP_EVENT = 'stop';
    const RESET_EVENT = 'reset';
    const RELEASE_EVENT = 'release';
    const START_STREEAM = 'start_stream';
    const SETEXIT = 'setExit';
    const SETSTART = 'setStart';
    const SETPAUSE = 'setPause';
    const CLOSE_STREAM = 'close_stream';
    const ERROR_EVENT = 'error';
    let cameraManager;
    let cameras;
    let captureSession;
    let videoOutput;
    let surfaceID;
    let fdPath;
    let fileAsset;
    let fdNumber;
    let events = require('events');
    let eventEmitter = new events.EventEmitter();

    let configFile = {
        audioBitrate : 48000,
        audioChannels : 2,
        audioCodec : 'audio/mp4a-latm',
        audioSampleRate : 48000,
        durationTime : 1000,
        fileFormat : 'mp4',
        videoBitrate : 48000,
        videoCodec : 'video/mp4v-es',
        videoFrameWidth : 640,
        videoFrameHeight : 480,
        videoFrameRate : 10
    }
    // rotation 0, 90, 180, 270
    let videoConfig = {
        audioSourceType : 1,
        videoSourceType : 0,
        profile : configFile,
        url : 'file:///data/media/API.mp4',
        rotation : 0,
        location : { latitude : 30, longitude : 130 },
        maxSize : 100,
        maxDuration : 500
    }

    let onlyVideoProfile = {
        durationTime : 1000,
        fileFormat : 'mp4',
        videoBitrate : 48000,
        videoCodec : 'video/mp4v-es',
        videoFrameWidth : 640,
        videoFrameHeight : 480,
        videoFrameRate : 10
    }

    let onlyVideoConfig = {
        videoSourceType : 0,
        profile : onlyVideoProfile,
        url : 'file:///data/media/API.mp4',
        rotation : 0,
        location : { latitude : 30, longitude : 130 },
        maxSize : 100,
        maxDuration : 500
    }

    function sleep(time) {
        for(let t = Date.now();Date.now() - t <= time;);
    }

    beforeAll(async function () {
        await initCamera();
        await getFd('API.mp4');
        console.info('beforeAll case');
    })

    beforeEach(function () {
        surfaceID = null;
        console.info('beforeEach case');
    })

    afterEach(function () {
        console.info('afterEach case');
    })

    afterAll(async function () {
        await closeFd();
        console.info('afterAll case');
    })

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

    async function initCamera() {
        cameraManager = await camera.getCameraManager(null);
        if (cameraManager != null) {
            console.info('[camera] case getCameraManager success');
        } else {
            console.info('[camera] case getCameraManager failed');
            return;
        }
        cameras = await cameraManager.getCameras();
        if (cameras != null) {
            console.info('[camera] case getCameras success');
        } else {
            console.info('[camera] case getCameras failed');
        }
    }

    async function initCaptureSession(videoOutPut) {
        let cameraInput = await cameraManager.createCameraInput(cameras[0].cameraId);
        if (cameraInput != null) {
            console.info('[camera] case createCameraInput success');
        } else {
            console.info('[camera] case createCameraInput failed');
            return;
        }
        captureSession = await camera.createCaptureSession(null);
        await captureSession.beginConfig();
        await captureSession.addInput(cameraInput);
        await captureSession.addOutput(videoOutPut);
        await captureSession.commitConfig();
        await captureSession.start();
    }

    async function stopCaptureSession() {
        await captureSession.stop();
        await captureSession.release();
    }

    function printfError(error, done) {
        expect().assertFail();
        console.info(`case error called,errMessage is ${error.message}`);
        done();
    }

    function toNextStep(videoRecorder, steps, done) {
        if (steps[0] == END_EVENT) {
            console.info('case success!!');
            done();
        } else {
            eventEmitter.emit(steps[0], videoRecorder, steps, done);
        }
    }

    eventEmitter.on(START_STREEAM, async (videoRecorder, steps, done) => {
        steps.shift();
        videoOutput = await camera.createVideoOutput(surfaceID);
        if (videoOutput == null) {
            console.info('[camera] case createVideoOutput failed');
        } else {
            console.info('[camera] case createVideoOutput success');
        }
        await initCaptureSession(videoOutput);
        await videoOutput.start().then(() => {
            console.info('[camera] case videoOutput start success');
        });
        toNextStep(videoRecorder, steps, done);
    });

    eventEmitter.on(SETSTART, async (videoRecorder, steps, done) => {
        steps.shift();
        toNextStep(videoRecorder, steps, done);
    });

    eventEmitter.on(SETPAUSE, async (videoRecorder, steps, done) => {
        steps.shift();
        toNextStep(videoRecorder, steps, done);
    });

    eventEmitter.on(CLOSE_STREAM, async (videoRecorder, steps, done) => {
        steps.shift();
        await videoOutput.stop().then(() => {
            console.info('[camera] case videoOutput stop success');
        });
        await videoOutput.release().then(() => {
            console.info('[camera] case videoOutput release success');
        });
        videoOutput = undefined;
        await stopCaptureSession();
        toNextStep(videoRecorder, steps, done);
    });

    eventEmitter.on(CREATE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        media.createVideoRecorder((err, recorder) => {
            if (typeof (err) == 'undefined') {
                console.info('case createVideoRecorder success ');
                videoRecorder = recorder;
                expect(videoRecorder.state).assertEqual('idle');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case createVideoRecorder error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PREPARE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.prepare(videoConfig, (err) => {
            if (typeof (err) == 'undefined') {
                console.info('case prepare success');
                expect(videoRecorder.state).assertEqual('prepared');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case prepare error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PREPARE_OLNYVIDEO_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.prepare(onlyVideoConfig, (err) => {
            if (typeof (err) == 'undefined') {
                console.info('case prepare success');
                expect(videoRecorder.state).assertEqual('prepared');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case prepare error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(GETSURFACE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.getInputSurface((err, outPutSurface) => {
            if (typeof (err) == 'undefined') {
                console.info('case getInputSurface success');
                surfaceID = outPutSurface;
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case getInputSurface error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(START_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.start((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case start success');
                expect(videoRecorder.state).assertEqual('playing');
                sleep(RECORDER_TIME);
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case start error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(PAUSE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.pause((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case pause success');
                sleep(PAUSE_TIME);
                expect(videoRecorder.state).assertEqual('paused');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case pause error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RESUME_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.resume((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case resume success');
                sleep(RECORDER_TIME);
                expect(videoRecorder.state).assertEqual('playing');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case resume error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(STOP_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.stop((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case stop success');
                expect(videoRecorder.state).assertEqual('stopped');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case stop error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RESET_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.reset((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case reset success');
                expect(videoRecorder.state).assertEqual('idle');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case reset error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RELEASE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.release((err) => {
            if (typeof (err) == 'undefined') {
                expect(videoRecorder.state).assertEqual('idle');
                console.info('case release success');
                toNextStep(videoRecorder, steps, done);
            } else if ((typeof (err) != 'undefined') && (steps[0] == ERROR_EVENT)) {
                steps.shift();
                console.info('case release error hanpped');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0100
        * @tc.name      : 01.create->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0100', 0, async function (done) {
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0200
        * @tc.name      : 02.start->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0300
        * @tc.name      : 03.pause->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, CLOSE_STREAM, PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0400
        * @tc.name      : 04.resume->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT,
            PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0500
        * @tc.name      : 05.stop->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, STOP_EVENT, CLOSE_STREAM, PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0600
        * @tc.name      : 06.reset->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, CLOSE_STREAM, RESET_EVENT, PREPARE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->prepare
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, PREPARE_EVENT,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0800
        * @tc.name      : 08.prepare 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PREPARE_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, PREPARE_EVENT, ERROR_EVENT,
            PREPARE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0100
        * @tc.name      : 01.create->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, START_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0200
        * @tc.name      : 02.prepare->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0300
        * @tc.name      : 03.pause->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, START_EVENT, ERROR_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0400
        * @tc.name      : 04.resume->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT,
            START_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0500
        * @tc.name      : 05.stop->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, STOP_EVENT, CLOSE_STREAM, START_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0600
        * @tc.name      : 06.reset->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RESET_EVENT, CLOSE_STREAM, START_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->start
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0800
        * @tc.name      : 08.start 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_START_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, START_EVENT, START_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0100
        * @tc.name      : 01.create->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0200
        * @tc.name      : 02.prepare->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0300
        * @tc.name      : 03.start->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0400
        * @tc.name      : 04.resume->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT, SETPAUSE,
            PAUSE_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0500
        * @tc.name      : 05.stop->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, STOP_EVENT, CLOSE_STREAM, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0600
        * @tc.name      : 06.reset->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RESET_EVENT, CLOSE_STREAM, PAUSE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->pause
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, PAUSE_EVENT,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0800
        * @tc.name      : 08.pause 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_PAUSE_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, PAUSE_EVENT, PAUSE_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0100
        * @tc.name      : 01.create->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, RESUME_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0200
        * @tc.name      : 02.prepare->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, RESUME_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0300
        * @tc.name      : 03.start->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RESUME_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0400
        * @tc.name      : 04.pause->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART,
            RESUME_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0500
        * @tc.name      : 05.stop->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, STOP_EVENT, CLOSE_STREAM, RESUME_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0600
        * @tc.name      : 06.reset->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RESET_EVENT, CLOSE_STREAM, RESUME_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->resume
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, RESUME_EVENT,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0800
        * @tc.name      : 08.resume 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESUME_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART,
            RESUME_EVENT, RESUME_EVENT, RESUME_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0100
        * @tc.name      : 01.create->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0200
        * @tc.name      : 02.prepare->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0300
        * @tc.name      : 03.start->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, 
            START_EVENT, STOP_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0400
        * @tc.name      : 04.pause->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, 
            START_EVENT, SETPAUSE, PAUSE_EVENT, STOP_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0500
        * @tc.name      : 05.resume->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, 
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT,
            STOP_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0600
        * @tc.name      : 06.reset->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, RESET_EVENT, STOP_EVENT,
            ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->stop
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT,
            STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0800
        * @tc.name      : 08.stop 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_STOP_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, 
            START_EVENT, STOP_EVENT, CLOSE_STREAM, STOP_EVENT, ERROR_EVENT,
            STOP_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0100
        * @tc.name      : 01.create->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0200
        * @tc.name      : 02.prepare->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0300
        * @tc.name      : 03.start->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, RESET_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0400
        * @tc.name      : 04.pause->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, RESET_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0500
        * @tc.name      : 05.resume->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT,
            RESET_EVENT, CLOSE_STREAM, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0600
        * @tc.name      : 06.stop->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM,
            START_EVENT, STOP_EVENT, CLOSE_STREAM, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0700
        * @tc.name      : 07.getInputSurface->reset
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0800
        * @tc.name      : 08.reset 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_RESET_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, RESET_EVENT,
            RESET_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0100
        * @tc.name      : 01.create->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0100', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, GETSURFACE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0200
        * @tc.name      : 02.prepare->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0200', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0300
        * @tc.name      : 03.start->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0300', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, START_EVENT,
            GETSURFACE_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0400
        * @tc.name      : 04.pause->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0400', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, START_EVENT,
            SETPAUSE, PAUSE_EVENT, GETSURFACE_EVENT, RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0500
        * @tc.name      : 05.resume->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0500', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, START_EVENT,
            SETPAUSE, PAUSE_EVENT, SETPAUSE, SETSTART, RESUME_EVENT, GETSURFACE_EVENT,
            RELEASE_EVENT, CLOSE_STREAM, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0600
        * @tc.name      : 06.stop->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0600', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_STREEAM, START_EVENT,
            STOP_EVENT, CLOSE_STREAM, GETSURFACE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0700
        * @tc.name      : 07.reset->getInputSurface
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0700', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, RESET_EVENT,
            GETSURFACE_EVENT, ERROR_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0800
        * @tc.name      : 08.getInputSurface 3 times
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_RECORDER_GETSURFACE_CALLBACK_0800', 0, async function (done) {
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, 
            GETSURFACE_EVENT, GETSURFACE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })
})
