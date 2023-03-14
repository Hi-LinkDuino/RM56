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
import mediademo from '@ohos.multimedia.mediademo'
import Fileio from '@ohos.fileio'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
export
const ENCODE_STEP = {
    WAIT_FOR_EOS : 'encode:waitForEOS',
    CONFIGURE : 'encode:configure',
    GET_INPUTSURFACE : 'encode:getSurface',
    PREPARE : 'encode:prepare',
    START : 'encode:start',
    FLUSH : 'encode:flush',
    STOP : 'encode:stop',
    RESET : 'encode:reset',
    WAIT_FOR_ALL_OUTS : 'encode:waitForAllOuts',
    ERROR : 'encode:error',
    RELEASE : 'encode:release',
}
const STREAM_STEP = {
    CREATE : 'stream:create',
    PREPARE : 'stream:prepare',
    SET_PARAM: 'stream:setParam',
    SET_EOS_FRAME: 'stream:setEOSFrame',
    START : 'stream:start',
    STOP : 'stream:stop',
}
describe('VideoEncoderSoftwareReliCallbackTest', function () {
    let videoEncodeProcessor = null;
    let mediaTest = null;
    let surfaceID = '';
    let frameCountOut = 0;
    let outputQueue = [];
    let frameTotal = 100;
    let finalFrameId = 100;
    let isStreamRunning = false;
    let workdoneAtEOS = false;
    let stopBuffer = false;
    const ROOT = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoencoder/results/';
    const BASIC_PATH = ROOT + 'video_reliability_callback_';
    let mediaDescription = {
        'width': 320,
        'height': 240,
        'pixel_format': 3,
        'frame_rate': 30,
    }
    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(async function() {
        console.info('beforeEach case');
        await msleep(1000).then(() => {
        }, failCallback).catch(failCatch);
        videoEncodeProcessor = null;
        mediaTest = null;
        surfaceID = '';
        frameCountOut = 0;
        outputQueue = [];
        workdoneAtEOS = false;
        frameTotal = 100;
        finalFrameId = 100;
        isStreamRunning = false;
        stopBuffer = false;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (videoEncodeProcessor != null) {
            await videoEncodeProcessor.release().then(() => {
                console.info('in case : videoEncodeProcessor release success');
            }, failCallback).catch(failCatch);
            videoEncodeProcessor = null;
        }
        toStopStream();
    })

    afterAll(function() {
        console.info('afterAll case');
    })
    let failCallback = function(err) {
        console.error(`in case error failCallback called, errMessage is ${err.message}`);
        expect(err == undefined).assertTrue();
    }
    let failCatch = function(err) {
        console.error(`in case error failCatch called,errMessage is ${err.message}`);
        expect(err == undefined).assertTrue();
    }

    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }

    function printError(err, expectFail) {
        expect((err != undefined) == expectFail).assertTrue();
        if (expectFail == false && err != undefined) {
            console.error('in case error failCatch called,err is ' + err);
            console.error(`in case error failCatch called,errMessage is ${err.message}`);
        }
    }
    
    function writeFile(path, buf, len){
        try {
            let writestream = Fileio.createStreamSync(path, 'ab+');
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        } catch(e) {
            console.error('in case error writeFile: ' + e);
        }
    }

    /* get outputbuffers from codec  */
    async function dequeueOutputs(path, nextStep) {
        console.log('outputQueue.length:' + outputQueue.length);
        while (outputQueue.length > 0){
            let outputObject = outputQueue.shift();
            if (outputObject.flags == 1 || frameCountOut == frameTotal) {
                if (workdoneAtEOS) {
                    doneWork(nextStep);
                }
                return;
            }
            frameCountOut++;
            writeFile(path, outputObject.data, outputObject.length);
            videoEncodeProcessor.freeOutputBuffer(outputObject, (err) => {
                if (typeof(err) == 'undefined') {
                    console.debug('in case release output count:' + frameCountOut);
                } else {
                    console.info(`in case release output called,errMessage is ${err.message}`);
                }   
            })
        }
    }
    function printDescription(obj) {
        for (let item in obj) {
            let property = obj[item];
            console.info('video encoder key is ' + item);
            console.info('video encoder value is ' + property);
        }
    }
    function setCallback(path, nextStep) {
        videoEncodeProcessor.on('newOutputData', async (outBuffer) => {
            console.info('in case: outputBufferAvailable outBuffer.index: '+ outBuffer.index);
            if (stopBuffer == false) {
                outputQueue.push(outBuffer);
                dequeueOutputs(path, nextStep);
            }
        });

        videoEncodeProcessor.on('error',(err) => {
            console.info('in case: error called,errName is' + err);
        });

        videoEncodeProcessor.on('streamChanged', (format) => {
            if (typeof(format) != undefined) {
                printDescription(format);   
            }
        });
    }
    function toCreateStream(mySteps, done) {
        mediaTest = mediademo.createMediaTest();
        toNextStep(mySteps, done);
    }
    function toSetStreamParam(mySteps, done) {
        mediaTest.setResolution(mediaDescription.width, mediaDescription.height);
        mediaTest.setFrameRate(mediaDescription.frame_rate);
        toNextStep(mySteps, done);
    }
    function toSetEOSFrame(mySteps, done) {
        mediaTest.setFrameCount(frameTotal);
        toNextStep(mySteps, done);
    }
    function toStartStream(mySteps, done) {
        if (isStreamRunning == false) {
            console.info('in case : toStartStream');
            mediaTest.startStream(surfaceID);
            isStreamRunning = true;
        }
        toNextStep(mySteps, done);
    }

    function toStopStream(mySteps, done) {
        if (isStreamRunning == true) {
            console.info('in case : toStopStream');
            mediaTest.closeStream(surfaceID);
            isStreamRunning = false;
        }
        toNextStep(mySteps, done);
    }
    function toConfigure(mySteps, done, expectFail) {
        videoEncodeProcessor.configure(mediaDescription, (err) => {
            console.info(`case configure callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        })
    }
    function toGetInputSurface(mySteps, done, expectFail) {
        videoEncodeProcessor.getInputSurface((err, inputSurface) => {
            expect(err).assertUndefined();
            expect(inputSurface != undefined).assertTrue();
            printError(err, expectFail);
            surfaceID = inputSurface;
            console.info('in case : getInputSurface success, surfaceID ' + surfaceID);
            toNextStep(mySteps, done);
        })
    }
    function toPrepare(mySteps, done, expectFail) {
        videoEncodeProcessor.prepare((err) => {
            console.info(`case prepare callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
    function toStart(mySteps, done, expectFail) {
        videoEncodeProcessor.start((err) => {
            console.info(`case start callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
        stopBuffer = false;
    }
    function toStop(mySteps, done, expectFail) {
        videoEncodeProcessor.stop((err) => {
            console.info(`case stop callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
        stopBuffer = true;
    }
    function toFlush(mySteps, done, expectFail) {
        videoEncodeProcessor.flush((err) => {
            console.info(`case flush callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
    function toReset(mySteps, done, expectFail) {
        videoEncodeProcessor.reset((err) => {
            console.info(`case reset callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
	function runCase(mySteps, nextCase, done) {
	    if (mySteps[0] == ENCODE_STEP.ERROR) {
			mySteps.shift();
			nextCase(mySteps, done, true);
		} else {
			nextCase(mySteps, done, false);
		}
	}
    function toNextStep(mySteps, done) {
        console.info('case myStep[0]: ' + mySteps[0]);
        if (mySteps[0] == ENCODE_STEP.RELEASE) {
            if (videoEncodeProcessor != null){
                videoEncodeProcessor.release((err) => {
                    printError(err, false);
                    console.info('in case : videoEncodeProcessor release callback');
                    videoEncodeProcessor = null;
                    console.info('case to done');
                    done();
                });
            }
            return;
        }
        switch (mySteps[0]) {
            case ENCODE_STEP.CONFIGURE:
                mySteps.shift();
                console.info(`case to configure`);
				runCase(mySteps, toConfigure, done);
                break;
            case STREAM_STEP.CREATE:
                mySteps.shift();
                console.info(`case to createStream`);
                toCreateStream(mySteps, done);
                break;
            case STREAM_STEP.SET_PARAM:
                mySteps.shift();
                console.info(`case to SetStreamParam`);
                toSetStreamParam(mySteps, done);
                break;
            case STREAM_STEP.SET_EOS_FRAME:
                mySteps.shift();
                console.info(`case to SetEOSFrame`);
                toSetEOSFrame(mySteps, done);
                break;
            case STREAM_STEP.START:
                mySteps.shift();
                console.info(`case to StartStream`);
                toStartStream(mySteps, done);
                break;
            case STREAM_STEP.STOP:
                mySteps.shift();
                console.info(`case to StoptStream`);
                toStopStream(mySteps, done);
                break;
            case ENCODE_STEP.GET_INPUTSURFACE:
                mySteps.shift();
                console.info(`case to setOutputSurface`);
				runCase(mySteps, toGetInputSurface, done);
                break;
            case ENCODE_STEP.PREPARE:
                mySteps.shift();
                console.info(`case to prepare`);
                runCase(mySteps, toPrepare, done);
                break;
            case ENCODE_STEP.START:
                mySteps.shift();
                console.info(`case to start`);
				runCase(mySteps, toStart, done);
                break;
            case ENCODE_STEP.FLUSH:
                mySteps.shift();
                console.info(`case to flush`);
				runCase(mySteps, toFlush, done);
                break;
            case ENCODE_STEP.STOP:
                mySteps.shift();
                console.info(`case to stop`);
				runCase(mySteps, toStop, done);
                break;
            case ENCODE_STEP.RESET:
                mySteps.shift();
                console.info(`case to reset`);
				runCase(mySteps, toReset, done);
                break;
            case ENCODE_STEP.WAIT_FOR_EOS:
                mySteps.shift();
                setTimeout(() =>{
                    toNextStep(mySteps, done);
                }, 5000);   // wait 5000 ms for eos
                break;
            case ENCODE_STEP.WAIT_FOR_ALL_OUTS:
                mySteps.shift();
                console.info(`case wait for all outputs`);
                break;
            default:
                break;
        }
    }

    function toCreateVideoEncoderByMime(mime, path, mySteps, done) {
        media.createVideoEncoderByMime(mime, (err, processor) => {
            console.info(`case createVideoEncoderByMime callback`);
            printError(err, false);
            if (typeof(processor) != 'undefined') {
                videoEncodeProcessor = processor;
                setCallback(path, done);
                toNextStep(mySteps, done);
            } else {
                done();
            }
        })
    }
    function toCreateVideoEncoderByName(name, path, mySteps, done) {
        media.createVideoEncoderByName(name, (err, processor) => {
            printError(err, false);
            if (typeof(processor) != 'undefined') {
                videoEncodeProcessor = processor;
                setCallback(path, done);
                toNextStep(mySteps, done);
            } else {
                done();
            }
        })
    }
    async function doneWork(nextStep) {
        videoEncodeProcessor.stop((err) => {
            printError(err, false);
            console.info('case stop callback');
            mediaTest.closeStream(surfaceID);
            videoEncodeProcessor.reset((err) => {
                printError(err, false);
                console.info('case reset callback');
                videoEncodeProcessor.release((err) => {
                    printError(err, false);
                    console.info('case release callback');
                    videoEncodeProcessor = null;
                    nextStep();
                })
            })
        })
    }

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0100
    * @tc.name      : 001.create -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0200
    * @tc.name      : 002.prepare -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.CONFIGURE, ENCODE_STEP.ERROR, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0300
    * @tc.name      : 003.start -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.CONFIGURE, ENCODE_STEP.ERROR, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0400
    * @tc.name      : 004.flush -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.CONFIGURE, ENCODE_STEP.ERROR, STREAM_STEP.STOP, 
            ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0500
    * @tc.name      : 005.stop -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.CONFIGURE, ENCODE_STEP.ERROR, STREAM_STEP.STOP, 
            ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0600
    * @tc.name      : 006.EOS -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.CONFIGURE, ENCODE_STEP.ERROR, 
            STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0700
    * @tc.name      : 007.reset -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.CONFIGURE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0800
    * @tc.name      : 008.configure -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.CONFIGURE, 
            ENCODE_STEP.ERROR, ENCODE_STEP.PREPARE, ENCODE_STEP.START, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0900
    * @tc.name      : 009.configure -> reset -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0900', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE_CALLBACK_0900.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.CONFIGURE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0100
        * @tc.name      : 001.create -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.PREPARE, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0200
        * @tc.name      : 002.configure -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0300
        * @tc.name      : 003.prepare -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.PREPARE, ENCODE_STEP.ERROR, ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0400
        * @tc.name      : 004.start -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.PREPARE, ENCODE_STEP.ERROR, 
            ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0500
        * @tc.name      : 005.flush -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0600
        * @tc.name      : 006.stop -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0700
        * @tc.name      : 007.EOS -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.ERROR, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0800
        * @tc.name      : 008.reset -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_PREPARE_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RESET, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0100
        * @tc.name      : 001.create -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.START, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0200
        * @tc.name      : 002.configure -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.START, 
            ENCODE_STEP.ERROR, STREAM_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0300
        * @tc.name      : 003.prepare -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0400
        * @tc.name      : 004.start -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.START, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0500
        * @tc.name      : 005.flush -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START, ENCODE_STEP.PREPARE, 
            ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.START, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0600
        * @tc.name      : 006.stop -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0700
        * @tc.name      : 007.EOS -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.START, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0800
        * @tc.name      : 008.reset -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.START, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0100
        * @tc.name      : 001.create -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.FLUSH, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0200
        * @tc.name      : 002.configure -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE,
            STREAM_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0300
        * @tc.name      : 003.prepare -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.FLUSH, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0400
        * @tc.name      : 004.start -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0500
        * @tc.name      : 005.flush -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.FLUSH, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0600
        * @tc.name      : 006.stop -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP , ENCODE_STEP.FLUSH, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0700
        * @tc.name      : 007.EOS -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.FLUSH, 
            ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0800
        * @tc.name      : 008.reset -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_FLUSH_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.FLUSH, 
            ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0100
        * @tc.name      : 001.create -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.STOP, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0200
        * @tc.name      : 002.configure -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, STREAM_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0300
        * @tc.name      : 003.prepare -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.STOP, ENCODE_STEP.ERROR, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0400
        * @tc.name      : 004.start -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0500
        * @tc.name      : 005.flush -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.STOP, 
            ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0600
        * @tc.name      : 006.stop -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.STOP, ENCODE_STEP.ERROR, 
            ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0700
        * @tc.name      : 007.EOS -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.STOP, 
            ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0800
        * @tc.name      : 008.reset -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_STOP_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.STOP, ENCODE_STEP.ERROR, 
            ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0100
        * @tc.name      : 001.create -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0200
        * @tc.name      : 002.configure -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE,
            STREAM_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0300
        * @tc.name      : 003.prepare -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0400
        * @tc.name      : 004.start -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0500
        * @tc.name      : 005.flush -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.FLUSH, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0600
        * @tc.name      : 006.stop -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0600', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0600.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0700
        * @tc.name      : 007.EOS -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0700', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0700.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0800
        * @tc.name      : 008.reset -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0800', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_RESET_CALLBACK_0800.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.RESET, ENCODE_STEP.RESET, ENCODE_STEP.RELEASE);
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0100
        * @tc.name      : 001.EOS -> flush -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0100', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0100.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.FLUSH, 
            ENCODE_STEP.STOP, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0200
        * @tc.name      : 002.EOS -> flush -> EOS
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0200', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0200.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.FLUSH, 
            ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0300
        * @tc.name      : 003.EOS -> reset -> configure
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0300', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0300.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.RESET, 
            ENCODE_STEP.CONFIGURE, STREAM_STEP.STOP, STREAM_STEP.SET_PARAM, STREAM_STEP.SET_EOS_FRAME, 
            STREAM_STEP.START, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0400
        * @tc.name      : 004.EOS -> stop -> start -> EOS
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0400', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0400.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.STOP, 
            ENCODE_STEP.START, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0500
        * @tc.name      : 005.EOS -> stop -> start -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0500', 0, async function (done) {
        let path = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_EOS_CALLBACK_0500.es';
        let mySteps = new Array(ENCODE_STEP.CONFIGURE, STREAM_STEP.CREATE, STREAM_STEP.SET_PARAM, 
            STREAM_STEP.SET_EOS_FRAME, ENCODE_STEP.GET_INPUTSURFACE, STREAM_STEP.START,
            ENCODE_STEP.PREPARE, ENCODE_STEP.START, ENCODE_STEP.WAIT_FOR_EOS, ENCODE_STEP.STOP, 
            ENCODE_STEP.START, ENCODE_STEP.STOP, ENCODE_STEP.RELEASE);
        frameTotal = 50;
        toCreateVideoEncoderByName('avenc_mpeg4', path, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE-RESET_CALLBACK_0100
        * @tc.name      : 001. configure -> reset for 50 times
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CONFIGURE-RESET_CALLBACK_0100', 0, async function (done) {
        let name = 'avenc_mpeg4';
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let loopCnt = 0;
        eventEmitter.on('configure', (mediaDescription) => {
            videoEncodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                eventEmitter.emit('reset');
            })
        });
        eventEmitter.on('reset', () => {
            videoEncodeProcessor.reset((err) => {
                expect(err).assertUndefined();
                console.info(`case reset 1`);
                loopCnt += 1;
                if (loopCnt < 50) {
                    console.info('case configure-reset current loop: ' + loopCnt);
                    eventEmitter.emit('configure', mediaDescription);
                } else {
                    videoEncodeProcessor.release((err) => {
                        expect(err).assertUndefined();
                        console.info('case release callback');
                        videoEncodeProcessor = null;
                        done();
                    })
                }
            })
        });
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                videoEncodeProcessor = processor;
                eventEmitter.emit('configure', mediaDescription);
            } else {
                console.info('in case : createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START-STOP_CALLBACK_0100
        * @tc.name      : 001. start -> stop for 50 times
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_START-STOP_CALLBACK_0100', 0, async function (done) {
        let name = 'avenc_mpeg4';
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let loopCnt = 0;
        eventEmitter.on('configure', (mediaDescription) => {
            videoEncodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                eventEmitter.emit('getInputSurface');
            })
        });
        eventEmitter.on('getInputSurface', () => {
            videoEncodeProcessor.getInputSurface((err, inputSurface) => {
                expect(err).assertUndefined();
                expect(inputSurface != undefined).assertTrue();
                surfaceID = inputSurface;
                console.info('in case : getInputSurface success, surfaceID ' + surfaceID);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            videoEncodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info('in case : prepare success');
                eventEmitter.emit('start');
            });
        });
        eventEmitter.on('start', () => {
            videoEncodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info('in case : start success');
                eventEmitter.emit('stop');
            });
        });
        eventEmitter.on('stop', () => {
            videoEncodeProcessor.stop((err) => {
                expect(err).assertUndefined();
                console.info(`case stop 1`);
                loopCnt += 1;
                if (loopCnt < 50) {
                    console.info('case start-stop current loop: ' + loopCnt);
                    eventEmitter.emit('start');
                } else {
                    videoEncodeProcessor.release((err) => {
                        expect(err).assertUndefined();
                        console.info('case release callback');
                        videoEncodeProcessor = null;
                        done();
                    })
                }
            })
        });
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                videoEncodeProcessor = processor;
                eventEmitter.emit('configure', mediaDescription);
            } else {
                console.info('in case : createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        })
    })
    
    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CREATE-RELEASE_CALLBACK_0100
        * @tc.name      : 001. create -> release for 50 times
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_CREATE-RELEASE_CALLBACK_0100', 0, async function (done) {
        let name = 'avenc_mpeg4';
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let loopCnt = 0;
        eventEmitter.on('create', (name) => {
            media.createVideoEncoderByName(name, (err, processor) => {
                expect(err).assertUndefined();
                if (typeof(processor) != 'undefined') {
                    videoEncodeProcessor = processor;
                    eventEmitter.emit('release');
                } else {
                    console.info('in case : createVideoEncoderByName fail');
                    expect().assertFail();
                    done();
                }
            })
        })
        eventEmitter.on('release', () => {
            videoEncodeProcessor.release((err) => {
                expect(err).assertUndefined();
                console.info(`case release 1`);
                videoEncodeProcessor = null;
                loopCnt += 1;
                if (loopCnt < 50) {
                    console.info('case create-release current loop: ' + loopCnt);
                    eventEmitter.emit('create', name);
                } else {
                    done();
                }
            })
        })
        eventEmitter.emit('create', name);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_TOTALLOOP_CALLBACK_0100
        * @tc.name      : 001. total loop for 50 times
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_API_TOTALLOOP_CALLBACK_0100', 0, async function (done) {
        let name = 'avenc_mpeg4';
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let loopCnt = 0;
        eventEmitter.on('create', (name) => {
            media.createVideoEncoderByName(name, (err, processor) => {
                expect(err).assertUndefined();
                if (typeof(processor) != 'undefined') {
                    videoEncodeProcessor = processor;
                    eventEmitter.emit('configure', mediaDescription);
                } else {
                    console.info('in case : createVideoEncoderByName fail');
                    expect().assertFail();
                    done();
                }
            })
        })
        eventEmitter.on('configure', (mediaDescription) => {
            videoEncodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                eventEmitter.emit('getInputSurface');
            })
        });
        eventEmitter.on('getInputSurface', () => {
            videoEncodeProcessor.getInputSurface((err, inputSurface) => {
                expect(err).assertUndefined();
                expect(inputSurface != undefined).assertTrue();
                surfaceID = inputSurface;
                console.info('in case : getInputSurface success, surfaceID ' + surfaceID);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            videoEncodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info('in case : prepare success');
                eventEmitter.emit('start');
            });
        });
        eventEmitter.on('start', () => {
            videoEncodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info('in case : start success');
                eventEmitter.emit('flush');
            });
        });
        eventEmitter.on('flush', () => {
            videoEncodeProcessor.flush((err) => {
                expect(err).assertUndefined();
                console.info('in case : flush success');
                eventEmitter.emit('stop');
            });
        });
        eventEmitter.on('stop', () => {
            videoEncodeProcessor.stop((err) => {
                expect(err).assertUndefined();
                console.info('in case : stop success');
                eventEmitter.emit('reset');
            });
        });
        eventEmitter.on('reset', () => {
            videoEncodeProcessor.reset((err) => {
                expect(err).assertUndefined();
                console.info('in case : reset success');
                eventEmitter.emit('release');
            });
        });
        eventEmitter.on('release', () => {
            videoEncodeProcessor.release((err) => {
                expect(err).assertUndefined();
                console.info(`case release 1`);
                videoEncodeProcessor = null;
                loopCnt += 1;
                if (loopCnt < 50) {
                    console.info('case create-release current loop: ' + loopCnt);
                    eventEmitter.emit('create', name);
                } else {
                    done();
                }
            })
        })
        eventEmitter.emit('create', name);
    })
})

