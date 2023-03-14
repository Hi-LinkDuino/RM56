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
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('VideoRecorderFuncCallbackTest', function () {
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
    let cameraManager;
    let cameras;
    let captureSession;
    let videoOutput;
    let surfaceID;
    let fdPath;
    let fileAsset;
    let fdNumber;
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
        url : 'file:///data/media/01.mp4',
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
        url : 'file:///data/media/01.mp4',
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
        await applyPermission();
        console.info('beforeAll case');
    })

    beforeEach(function () {
        sleep(5000);
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
        let appInfo = await bundle.getApplicationInfo('ohos.acts.multimedia.video.videorecorder', 0, 100);
        let atManager = abilityAccessCtrl.createAtManager();
        if (atManager != null) {
            let tokenID = appInfo.accessTokenId;
            console.info('[permission] case accessTokenID is ' + tokenID);
            let permissionName1 = 'ohos.permission.CAMERA';
            let permissionName2 = 'ohos.permission.MICROPHONE';
            let permissionName3 = 'ohos.permission.MEDIA_LOCATION';
            let permissionName4 = 'ohos.permission.READ_MEDIA';
            let permissionName5 = 'ohos.permission.WRITE_MEDIA';
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
            await atManager.grantUserGrantedPermission(tokenID, permissionName5, 1).then((result) => {
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

    let events = require('events');
    let eventEmitter = new events.EventEmitter();

    function toNextStep(videoRecorder, steps, done) {
        if (steps[0] == END_EVENT) {
            console.info('case success!!');
            done();
        } else {
            eventEmitter.emit(steps[0], videoRecorder, steps, done);
        }
    }

    function setOnCallback(videoRecorder) {
        videoRecorder.on('info', (infoType, extra) => {
            console.info('case info called, infoType is ' + infoType);
            console.info('case info called, extra is ' + extra);
        });

        videoRecorder.on('error', (err) => {
            console.info('case error called, errMessage is ' + err.message);
            expect().assertFail();
        });
    }

    eventEmitter.on(CREATE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        media.createVideoRecorder((err, recorder) => {
            if (typeof (err) == 'undefined') {
                console.info('case createVideoRecorder success ');
                videoRecorder = recorder;
                setOnCallback(videoRecorder);
                expect(videoRecorder.state).assertEqual('idle');
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
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(GETSURFACE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.getInputSurface((err, outPutsurface) => {
            if (typeof (err) == 'undefined') {
                expect(videoRecorder.state).assertEqual('prepared');
                surfaceID = outPutsurface;
                console.info('case getInputSurface success :' + surfaceID);
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(START_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoOutput = await camera.createVideoOutput(surfaceID);
        if (videoOutput == null) {
            console.info('case createVideoOutput failed');
        } else {
            console.info('case createVideoOutput success');
        }
        await initCaptureSession(videoOutput);
        await videoOutput.start().then(() => {
            console.info('case videoOutput start success');
        });
        videoRecorder.start((err) => {
            if (typeof (err) == 'undefined') {
                console.info('case start success');
                expect(videoRecorder.state).assertEqual('playing');
                sleep(RECORDER_TIME);
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
                expect(videoRecorder.state).assertEqual('paused');
                sleep(PAUSE_TIME);
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
            } else {
                printfError(err, done);
            }
        });
    });

    eventEmitter.on(RELEASE_EVENT, async (videoRecorder, steps, done) => {
        steps.shift();
        videoRecorder.release(async (err) => {
            await videoOutput.stop().then(() => {
                console.info('case videoOutput stop success');
            });
            await videoOutput.release().then(() => {
                console.info('case videoOutput release success');
            });
            videoOutput = undefined;
            await stopCaptureSession();
            if (typeof (err) == 'undefined') {
                expect(videoRecorder.state).assertEqual('idle');
                console.info('case release success');
                toNextStep(videoRecorder, steps, done);
            } else {
                printfError(err, done);
            }
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0100
        * @tc.name      : 01.start->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0100', 0, async function (done) {
        await getFd('19.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0200
        * @tc.name      : 02.start->pause->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0200', 0, async function (done) {
        await getFd('20.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0300
        * @tc.name      : 03.start->pause->resume->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0300', 0, async function (done) {
        await getFd('21.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            PAUSE_EVENT, RESUME_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0400
        * @tc.name      : 04.start->stop->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0400', 0, async function (done) {
        await getFd('22.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);    
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0500
        * @tc.name      : 05.start->reset->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0500', 0, async function (done) {
        await getFd('23.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0600
        * @tc.name      : 06.start->pause->stop->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0600', 0, async function (done) {
        await getFd('24.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, PAUSE_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0700
        * @tc.name      : 07.start->pause->reset->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0700', 0, async function (done) {
        await getFd('25.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, PAUSE_EVENT,
            RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0800
        * @tc.name      : 08.start->pause->resume->stop->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0800', 0, async function (done) {
        await getFd('26.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, PAUSE_EVENT,
            RESUME_EVENT, STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0900
        * @tc.name      : 09.start->pause->resume->reset->release (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_0900', 0, async function (done) {
        await getFd('27.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, PAUSE_EVENT,
            RESUME_EVENT, RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1000
        * @tc.name      : 10.start->stop->release
        *                 (audioBitrate 8000,audioSampleRate 8000,videoBitrateRange 8000)(promise)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1000', 0, async function (done) {
        configFile.audioBitrate = 8000;
        configFile.audioSampleRate = 8000;
        configFile.videoBitrate = 8000;
        await getFd('28.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1100
        * @tc.name      : 11.start->stop->release
        *                 (audioBitrate 16000, audioSampleRate 32000, videoBitrateRange 16000)(callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1100', 0, async function (done) {
        configFile.audioBitrate = 16000;
        configFile.audioSampleRate = 32000;
        configFile.videoBitrate = 16000;
        await getFd('29.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1200
        * @tc.name      : 12.start->stop->release
        *                 (audioBitrate 32000, audioSampleRate 44100, videoBitrateRange 32000)(callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1200', 0, async function (done) {
        configFile.audioBitrate = 32000;
        configFile.audioSampleRate = 44100;
        configFile.videoBitrate = 32000;
        await getFd('30.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1300
        * @tc.name      : 13.start->stop->release
        *                 (audioBitrate 112000, audioSampleRate 96000, videoBitrateRange 112000)(callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1300', 0, async function (done) {
        configFile.audioBitrate = 112000;
        configFile.audioSampleRate = 96000;
        configFile.videoBitrate = 112000;
        await getFd('31.mp4');
        videoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done); 
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1400
        * @tc.name      : 14.start->release (only video) (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1400', 0, async function (done) {
        await getFd('32.mp4');
        onlyVideoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_OLNYVIDEO_EVENT, GETSURFACE_EVENT,
            START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1500
        * @tc.name      : 15.start->pause->release (only video) (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1500', 0, async function (done) {
        await getFd('33.mp4');
        onlyVideoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_OLNYVIDEO_EVENT, GETSURFACE_EVENT, START_EVENT,
            PAUSE_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1600
        * @tc.name      : 16.start->pause->resume->releas (only video) (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1600', 0, async function (done) {
        await getFd('34.mp4');
        onlyVideoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_OLNYVIDEO_EVENT, GETSURFACE_EVENT, START_EVENT,
            PAUSE_EVENT, RESUME_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1700
        * @tc.name      : 17.start->stop->release (only video) (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1700', 0, async function (done) {
        await getFd('35.mp4');
        onlyVideoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_OLNYVIDEO_EVENT, GETSURFACE_EVENT, START_EVENT,
            STOP_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1800
        * @tc.name      : 18.start->reset->release (only video) (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1800', 0, async function (done) {
        await getFd('36.mp4');
        onlyVideoConfig.url = fdPath;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_OLNYVIDEO_EVENT, GETSURFACE_EVENT, START_EVENT,
            RESET_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1900
        * @tc.name      : 19.rotation 90 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_1900', 0, async function (done) {
        await getFd('37.mp4');
        videoConfig.url = fdPath;
        videoConfig.rotation = 90;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT,
            START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2000
        * @tc.name      : 20.rotation 180 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2000', 0, async function (done) {
        await getFd('38.mp4');
        videoConfig.url = fdPath;
        videoConfig.rotation = 180;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2100
        * @tc.name      : 21.rotation 270 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2100', 0, async function (done) {
        await getFd('39.mp4');
        videoConfig.url = fdPath;
        videoConfig.rotation = 270;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2200
        * @tc.name      : 22.videoFrameRate 20 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2200', 0, async function (done) {
        await getFd('46.mp4');
        videoConfig.url = fdPath;
        videoConfig.videoFrameRate = 20;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2300
        * @tc.name      : 23.videoFrameRate 30 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2300', 0, async function (done) {
        await getFd('47.mp4');
        videoConfig.url = fdPath;
        videoConfig.videoFrameRate = 30;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2400
        * @tc.name      : 24.videoFrameRate 60 (callback)
        * @tc.desc      : Audio recordr control test
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_VIDEO_RECORDER_FUNCTION_CALLBACK_2400', 0, async function (done) {
        await getFd('48.mp4');
        videoConfig.url = fdPath;
        videoConfig.videoFrameRate = 60;
        let videoRecorder = null;
        let mySteps = new Array(CREATE_EVENT, PREPARE_EVENT, GETSURFACE_EVENT, START_EVENT, RELEASE_EVENT, END_EVENT);
        eventEmitter.emit(mySteps[0], videoRecorder, mySteps, done);
    })
})
