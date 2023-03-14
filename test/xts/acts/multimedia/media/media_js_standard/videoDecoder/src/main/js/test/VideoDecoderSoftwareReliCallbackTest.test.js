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
import Fileio from '@ohos.fileio'
import router from '@system.router'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
export
const DECODE_STEP = {
    WAIT_FOR_EOS : 'waitForEOS',
    CONFIGURE : 'configure',
    SETSURFACE : 'setSurface',
    PREPARE : 'prepare',
    START : 'start',
    FLUSH : 'flush',
    STOP : 'stop',
    RESET : 'reset',
    WAIT_FOR_ALL_OUTS : 'waitForAllOuts',
    ERROR : 'error',
    RELEASE : 'release',
}

describe('VideoDecoderSoftwareReliCallbackTest', function () {
    let videoDecodeProcessor = null;
    let readStreamSync = undefined;
    let frameCountIn = 0;
    let frameCountOut = 0;
    let timestamp = 0;
    let position = 0;
    let eosFrameId = -1;
    let inputQueue = [];
    let outputQueue = [];
    let isCodecData = true;
    let inputEosFlag = false;
    let workdoneAtEOS = false;
    let surfaceID = '';
    const BASIC_PATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videodecoder/';
    const SRCPATH = BASIC_PATH + 'out_320_240_10s.h264';
    let mediaDescription = {
        'track_type': 1,
        'codec_mime': 'video/avc',
        'width': 320,
        'height': 240,
        'pixel_format': 3,
        'frame_rate': 60,
        'max_input_size': 150000,
    }
    const H264_FRAME_SIZE_60FPS_320 =
    [ 2106, 11465, 321, 72, 472, 68, 76, 79, 509, 90, 677, 88, 956, 99, 347, 77, 452, 681, 81, 1263, 94, 106, 97,
        998, 97, 797, 93, 1343, 150, 116, 117, 926, 1198, 128, 110, 78, 1582, 158, 135, 112, 1588, 165, 132,
        128, 1697, 168, 149, 117, 1938, 170, 141, 142, 1830, 106, 161, 122, 1623, 160, 154, 156, 1998, 230,
        177, 139, 1650, 186, 128, 134, 1214, 122, 1411, 120, 1184, 128, 1591, 195, 145, 105, 1587, 169, 140,
        118, 1952, 177, 150, 161, 1437, 159, 123, 1758, 180, 165, 144, 1936, 214, 191, 175, 2122, 180, 179,
        160, 1927, 161, 184, 119, 1973, 218, 210, 129, 1962, 196, 127, 154, 2308, 173, 127, 1572, 142, 122,
        2065, 262, 159, 206, 2251, 269, 179, 170, 2056, 308, 168, 191, 2090, 303, 191, 110, 1932, 272, 162,
        122, 1877, 245, 167, 141, 1908, 294, 162, 118, 1493, 132, 1782, 273, 184, 133, 1958, 274, 180, 149,
        2070, 216, 169, 143, 1882, 224, 149, 139, 1749, 277, 184, 139, 2141, 197, 170, 140, 2002, 269, 162,
        140, 1862, 202, 179, 131, 1868, 214, 164, 140, 1546, 226, 150, 130, 1707, 162, 146, 1824, 181, 147,
        130, 1898, 209, 143, 131, 1805, 180, 148, 106, 1776, 147, 141, 1572, 177, 130, 105, 1776, 178, 144,
        122, 1557, 142, 124, 114, 1436, 143, 126, 1326, 127, 1755, 169, 127, 105, 1807, 177, 131, 134, 1613,
        187, 137, 136, 1314, 134, 118, 2005, 194, 129, 147, 1566, 185, 132, 131, 1236, 174, 137, 106, 11049,
        574, 126, 1242, 188, 130, 119, 1450, 187, 137, 141, 1116, 124, 1848, 138, 122, 1605, 186, 127, 140,
        1798, 170, 124, 121, 1666, 157, 128, 130, 1678, 135, 118, 1804, 169, 135, 125, 1837, 168, 124, 124];
    let ES_FRAME_SIZE = H264_FRAME_SIZE_60FPS_320;
    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(async function() {
        console.info('beforeEach case');
        await toDisplayPage().then(() => {
        }, failCallback).catch(failCatch);
        await msleep(1000).then(() => {
        }, failCallback).catch(failCatch);
        frameCountIn = 0;
        frameCountOut = 0;
        timestamp = 0;
        inputQueue = [];
        outputQueue = [];
        isCodecData = true;
        workdoneAtEOS = false;
        eosFrameId = -1;
        inputEosFlag = false;
        position = 0;
        surfaceID = globalThis.value;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (videoDecodeProcessor != null) {
            await videoDecodeProcessor.release().then(() => {
                console.info('in case : videoDecodeProcessor release success');
            }, failCallback).catch(failCatch);
            videoDecodeProcessor = null;
        }
        await router.clear().then(() => {
        }, failCallback).catch(failCatch);
    })

    afterAll(function() {
        console.info('afterAll case');
    })
    let failCallback = function(err) {
        console.info(`in case error failCallback called, errMessage is ${err.message}`);
        expect(err == undefined).assertTrue();
    }
    let failCatch = function(err) {
        console.info(`in case error failCatch called,errMessage is ${err.message}`);
        expect(err == undefined).assertTrue();
    }
    function printError(err, expectFail) {
        expect((err != undefined) == expectFail).assertTrue();
        if (expectFail == false && err != undefined) {
            console.info('in case error failCatch called,err is ' + err);
            console.info(`in case error failCatch called,errMessage is ${err.message}`);
        }
    }
    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }
    async function toDisplayPage() {
        let path = 'pages/display/display';
        let options = {
            uri: path,
        }
        try {
            await router.push(options);
        } catch (e) {
            console.error('in case toDisplayPage' + e);
        }
    }
    function readFile(path){
        console.info('in case : read file start execution');
        try {
            console.info('in case: file path ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        } catch(e) {
            console.info('in case readFile' + e);
        }
    }

    function getContent(buf, pos, len) {
        console.info('start get content, len ' + len + ' buf.byteLength ' + buf.byteLength);
        let lengthReal = -1;
        try {
            if (pos == -1) {
                lengthReal = readStreamSync.readSync(buf, {
                    length: len,
                });
            } else {
                lengthReal = readStreamSync.readSync(buf, {
                    length: len,
                    position: pos,
                });
            }
            console.info('in case: lengthReal: ' + lengthReal);
        } catch(e) {
            console.error('in case error getContent err ' + err);
        }
    }

    /* push inputbuffers into codec  */
    async function enqueueInputs(inputObject) {
        console.log('in case: inputObject.index: ' + inputObject.index);
        if (frameCountIn < ES_FRAME_SIZE.length) {
            getContent(inputObject.data, position, ES_FRAME_SIZE[frameCountIn]);
            inputObject.timeMs = timestamp;
            inputObject.offset = 0;
            inputObject.length = ES_FRAME_SIZE[frameCountIn];
            position = position + ES_FRAME_SIZE[frameCountIn];
            console.info('in case: frameCountIn ' + frameCountIn);
            frameCountIn++;
            timestamp += 1000 / mediaDescription.frame_rate;
        }
        if (isCodecData) {
            inputObject.flags = 8;
            isCodecData = false;
            timestamp = 0;
        } else if (frameCountIn >= ES_FRAME_SIZE.length || frameCountIn == eosFrameId) {
            inputObject.flags = 1;
            inputEosFlag = true;
        } else {
            inputObject.flags = 4;
        }
        videoDecodeProcessor.pushInputData(inputObject, (err) => {
            console.info('in case: queueInput success ');
        })
    }

    /* get outputbuffers from codec  */
    async function dequeueOutputs(nextStep, outputObject) {
        if (outputObject.flags == 1 ) {
            if (workdoneAtEOS) {
                doneWork(nextStep);
            }
            return;
        }
        frameCountOut++;
        videoDecodeProcessor.freeOutputBuffer(outputObject, () => {
            console.log('in case: release output count:' + frameCountOut);
        })
    }
    
    function toConfigure(mySteps, done, expectFail) {
        videoDecodeProcessor.configure(mediaDescription, (err) => {
            console.info(`case configure callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        })            
    }
    function toPrepare(mySteps, done, expectFail) {
        videoDecodeProcessor.prepare((err) => {
            console.info(`case prepare callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
    function toStart(mySteps, done, expectFail) {
        let timeDelay = 0;
        videoDecodeProcessor.start((err) => {
            console.info(`case start callback`);
            printError(err, expectFail);
            if (mySteps[0] == DECODE_STEP.FLUSH) {
                timeDelay = 500;
            }
            setTimeout(() => {
                toNextStep(mySteps, done);
            }, timeDelay);
        });
    }
    function toFlush(mySteps, done, expectFail) {
        videoDecodeProcessor.flush((err) => {
            console.info(`case flush callback`);
            printError(err, expectFail);
            if (expectFail == false) {
                inputEosFlag = false;
                position = ES_FRAME_SIZE[0];
                inputQueue = [];
                timestamp = 0;
                frameCountIn = 1;
            }
            toNextStep(mySteps, done);
        });
    }
    function toStop(mySteps, done, expectFail) {
        videoDecodeProcessor.stop((err) => {
            console.info(`case stop callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
    function toReset(mySteps, done, expectFail) {
        videoDecodeProcessor.reset((err) => {
            console.info(`case reset callback`);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }
    function toSetOutputSurface(mySteps, done, expectFail) {
        videoDecodeProcessor.setOutputSurface(surfaceID, true, (err) => {
            console.info('case setOutputSurface callback, surfaceID ' + surfaceID);
            printError(err, expectFail);
            toNextStep(mySteps, done);
        });
    }

    function toNextStep(mySteps, done) {
        console.info('case myStep[0]: ' + mySteps[0]);
        if (mySteps[0] == DECODE_STEP.RELEASE) {
            if (videoDecodeProcessor != null){
                videoDecodeProcessor.release((err) => {
                    printError(err, false);
                    console.info('in case : videoDecodeProcessor release callback');
                    videoDecodeProcessor = null;
                    console.info('case to done');
                    done();
                });
            }
            return;
        }
        switch (mySteps[0]) {
            case DECODE_STEP.CONFIGURE:
                mySteps.shift();
                console.info(`case to configure`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toConfigure(mySteps, done, true);
                } else {
                    toConfigure(mySteps, done, false);
                }
                break;
            case DECODE_STEP.SETSURFACE:
                mySteps.shift();
                console.info(`case to setOutputSurface`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toSetOutputSurface(mySteps, done, true);
                } else {
                    toSetOutputSurface(mySteps, done, false);
                }
                break;
            case DECODE_STEP.PREPARE:
                mySteps.shift();
                console.info(`case to prepare`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toPrepare(mySteps, done, true);
                } else {
                    toPrepare(mySteps, done, false);
                }
                break;
            case DECODE_STEP.START:
                mySteps.shift();
                console.info(`case to start`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toStart(mySteps, done, true);
                } else {
                    readStreamSync = undefined;
                    readFile(SRCPATH);
                    frameCountIn = 0;
                    frameCountOut = 0;
                    inputQueue = [];
                    outputQueue = [];
                    isCodecData = true;
                    inputEosFlag = false;
                    position = 0;
                    toStart(mySteps, done, false);
                }
                break;
            case DECODE_STEP.FLUSH:
                mySteps.shift();
                console.info(`case to flush`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toFlush(mySteps, done, true);
                } else {
                    toFlush(mySteps, done, false);
                }
                break;
            case DECODE_STEP.STOP:
                mySteps.shift();
                console.info(`case to stop`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toStop(mySteps, done, true);
                } else {
                    toStop(mySteps, done, false);
                }
                break;
            case DECODE_STEP.RESET:
                mySteps.shift();
                console.info(`case to reset`);
                if (mySteps[0] == DECODE_STEP.ERROR) {
                    mySteps.shift();
                    toReset(mySteps, done, true);
                } else {
                    toReset(mySteps, done, false);
                }
                break;
            case DECODE_STEP.WAIT_FOR_EOS:
                mySteps.shift();
                setTimeout(() =>{
                    expect(inputEosFlag).assertTrue();
                    if (inputEosFlag == false) {
                        console.info(`in case error wait for eos: inputEosFlag = false`);
                    }
                    toNextStep(mySteps, done);
                }, 7000);   // wait 7000 ms for eos
                break;
            case DECODE_STEP.WAIT_FOR_ALL_OUTS:
                mySteps.shift();
                console.info(`case wait for all outputs`);
                break;
            default:
                break;
        }
    }

    function setCallback(nextStep){
        videoDecodeProcessor.on('needInputData', async (inBuffer) => {
            console.info('in case: inputBufferAvailable inBuffer.index: '+ inBuffer.index);
            enqueueInputs(inBuffer);
        });

        videoDecodeProcessor.on('newOutputData', async (outBuffer) => {
            console.info('in case: outputBufferAvailable outBuffer.index: '+ outBuffer.index);
            dequeueOutputs(nextStep, outBuffer);
        });

        videoDecodeProcessor.on('error',(err) => {
            console.info('in case: error called,errName is' + err);
        });

        videoDecodeProcessor.on('streamChanged',(format) => {
            console.info('in case: Output format changed: ' + format.toString());
        });
    }

    function toCreateVideoDecoderByMime(mime, mySteps, done) {
        media.createVideoDecoderByMime(mime, (err, processor) => {
            console.info(`case createVideoDecoderByMime callback`);
            printError(err, false);
            videoDecodeProcessor = processor;
            setCallback(done);
            toNextStep(mySteps, done);
        })
    }
    function toCreateVideoDecoderByName(name, mySteps, done) {
        media.createVideoDecoderByName(name, (err, processor) => {
            printError(err, false);
            console.info(`case createVideoDecoderByName callback`);
            videoDecodeProcessor = processor;
            setCallback(done);
            toNextStep(mySteps, done);
        })
    }
    async function doneWork(nextStep) {
        videoDecodeProcessor.stop((err) => {
            printError(err, false);
            console.info('case stop callback');
            videoDecodeProcessor.reset((err) => {
                printError(err, false);
                console.info('case reset callback');
                videoDecodeProcessor.release((err) => {
                    printError(err, false);
                    console.info('case release callback');
                    videoDecodeProcessor = null;
                    nextStep();
                })
            })
        })
    }

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0100
        * @tc.name      : test set EOS manually before last frame and reset
        * @tc.desc      : test basic function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_ALL_OUTS);
        workdoneAtEOS = true;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0200
        * @tc.name      : test flush at running state
        * @tc.desc      : test basic function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.WAIT_FOR_ALL_OUTS);
        workdoneAtEOS = true;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0300
        * @tc.name      : test flush at EOS state
        * @tc.desc      : test basic function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_ALL_OUTS);
        eosFrameId = 50;
        workdoneAtEOS = true;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0400
        * @tc.name      : test stop at running state and reset
        * @tc.desc      : test basic function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0500
        * @tc.name      : test stop and restart
        * @tc.desc      : test basic function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_FUNCTION_CALLBACK_01_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.STOP, DECODE_STEP.START, DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.STOP,
            DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0100
    * @tc.name      : 001.create -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0200
    * @tc.name      : 002.prepare -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE,
            DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0300
    * @tc.name      : 003.start -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0400
    * @tc.name      : 004.flush -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0500
    * @tc.name      : 005.stop -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0600
    * @tc.name      : 006.EOS -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0700
    * @tc.name      : 007.reset -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.RESET, DECODE_STEP.CONFIGURE, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0800
    * @tc.name      : 008.configure -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.CONFIGURE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0900
    * @tc.name      : 009.configure -> reset -> configure
    * @tc.desc      : test for state transition
    * @tc.size      : MediumTest
    * @tc.type      : Reliability test
    * @tc.level     : Level2
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_CONFIGURE_CALLBACK_0900', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.RESET, DECODE_STEP.CONFIGURE, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0100
        * @tc.name      : 001.create -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0200
        * @tc.name      : 002.configure -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE,
            DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0300
        * @tc.name      : 003.prepare -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE,
            DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0400
        * @tc.name      : 004.start -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0500
        * @tc.name      : 005.flush -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0600
        * @tc.name      : 006.stop -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0700
        * @tc.name      : 007.EOS -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS ,
            DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0800
        * @tc.name      : 008.reset -> prepare
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_PREPARE_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.PREPARE, DECODE_STEP.ERROR, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0100
        * @tc.name      : 001.create -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.START, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0200
        * @tc.name      : 002.configure -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.START, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0300
        * @tc.name      : 003.prepare -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0400
        * @tc.name      : 004.start -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.START, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0500
        * @tc.name      : 005.flush -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.START, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0600
        * @tc.name      : 006.stop -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.START, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0700
        * @tc.name      : 007.EOS -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.START,
            DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0800
        * @tc.name      : 008.reset -> start
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_START_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.START, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0100
        * @tc.name      : 001.create -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.FLUSH, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0200
        * @tc.name      : 002.configure -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.FLUSH, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0300
        * @tc.name      : 003.prepare -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.FLUSH,
            DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0400
        * @tc.name      : 004.start -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0500
        * @tc.name      : 005.flush -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.FLUSH, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0600
        * @tc.name      : 006.stop -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP , DECODE_STEP.FLUSH, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0700
        * @tc.name      : 007.EOS -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.FLUSH, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0800
        * @tc.name      : 008.reset -> flush
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_FLUSH_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.FLUSH, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0100
        * @tc.name      : 001.create -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.STOP, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0200
        * @tc.name      : 002.configure -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.STOP, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0300
        * @tc.name      : 003.prepare -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.STOP,
            DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0400
        * @tc.name      : 004.start -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0500
        * @tc.name      : 005.flush -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.STOP, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0600
        * @tc.name      : 006.stop -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.STOP, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0700
        * @tc.name      : 007.EOS -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.STOP, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0800
        * @tc.name      : 008.reset -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_STOP_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.STOP, DECODE_STEP.ERROR, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0100
        * @tc.name      : 001.create -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0200
        * @tc.name      : 002.configure -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0300
        * @tc.name      : 003.prepare -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.RESET,
            DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0400
        * @tc.name      : 004.start -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0500
        * @tc.name      : 005.flush -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.FLUSH, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0600
        * @tc.name      : 006.stop -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0600', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.STOP, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0700
        * @tc.name      : 007.EOS -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0700', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0800
        * @tc.name      : 008.reset -> reset
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_RESET_CALLBACK_0800', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.RESET, DECODE_STEP.RESET, DECODE_STEP.RELEASE);
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0100
        * @tc.name      : 001.EOS -> flush -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0100', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.FLUSH, DECODE_STEP.STOP, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0200
        * @tc.name      : 002.EOS -> flush -> EOS
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0200', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.FLUSH, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0300
        * @tc.name      : 003.EOS -> reset -> configure
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0300', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.RESET, DECODE_STEP.CONFIGURE, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0400
        * @tc.name      : 004.EOS -> stop -> start -> EOS
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0400', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.STOP, DECODE_STEP.START, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0500
        * @tc.name      : 005.EOS -> stop -> start -> stop
        * @tc.desc      : test for state transition
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_VIDEO_SOFTWARE_DECODER_API_EOS_CALLBACK_0500', 0, async function (done) {
        let mySteps = new Array(DECODE_STEP.CONFIGURE, DECODE_STEP.SETSURFACE, DECODE_STEP.PREPARE, DECODE_STEP.START,
            DECODE_STEP.WAIT_FOR_EOS, DECODE_STEP.STOP, DECODE_STEP.START, DECODE_STEP.STOP, DECODE_STEP.RELEASE);
        eosFrameId = 50;
        toCreateVideoDecoderByName('avdec_h264', mySteps, done);
    })
})
