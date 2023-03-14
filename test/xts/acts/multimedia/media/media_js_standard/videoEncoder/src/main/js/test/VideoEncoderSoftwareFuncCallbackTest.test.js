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

describe('VideoEncoderSoftwareFuncCallbackTest', function () {
    let videoEncodeProcessor = null;
    let mediaTest;
    let surfaceID = '';
    let frameCountOut = 0;
    let outputQueue = [];
    let frameTotal = 100;
    let finalFrameId = 100;
    let reStart = false;
    let isStreamRunning = false;
    let stopBuffer = false;
    const events = require('events');
    const eventEmitter = new events.EventEmitter();
    const ROOT = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoencoder/results/';
    const BASIC_PATH = ROOT + 'video_func_callback_';
    
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
        frameTotal = 100;
        finalFrameId = 100;
        reStart = false;
        isStreamRunning = false;
        stopBuffer = false;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (videoEncodeProcessor != null) {
            await videoEncodeProcessor.release().then(() => {
                console.info('case videoEncodeProcessor release');
            }, failCallback).catch(failCatch);
            videoEncodeProcessor = null;
        }
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

    async function dequeueOutputs(path, nextStep) {
        while (outputQueue.length > 0) {
            let outputObject = outputQueue.shift();
            if (outputObject.flags == 1 || frameCountOut == frameTotal || frameCountOut == finalFrameId) {
                nextStep();
                return;
            }
            frameCountOut++;
            writeFile(path, outputObject.data, outputObject.length);
            videoEncodeProcessor.freeOutputBuffer(outputObject, (err) => {
                if (typeof(err) == 'undefined') {
                    console.log('in case release output count:' + frameCountOut);
                } else {
                    console.info(`in case release output called,errMessage is ${err.message}`);
                }                
            });
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
        console.info('case callback');
        videoEncodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            if (stopBuffer == false) {
                outputQueue.push(outBuffer);
                dequeueOutputs(path, nextStep);
            }
        });

        videoEncodeProcessor.on('error',(err) => {
            console.info('in case error called, errName is' + err);
        });
        videoEncodeProcessor.on('streamChanged',(format) => {
            if (typeof(format) != undefined) {
                printDescription(format);   
            }
        });
    }
    function toCreateStream() {
        mediaTest = mediademo.createMediaTest();
    }
    function toSetStreamParam(width, height, framerate, frameTotal) {
        mediaTest.setResolution(width, height);
        mediaTest.setFrameRate(framerate);
        mediaTest.setFrameCount(frameTotal);
    }

    function toStartStream() {
        if (isStreamRunning == false) {
            console.info('in case : toStartStream');
            mediaTest.startStream(surfaceID);
            isStreamRunning = true;
        }
    }

    function toStopStream() {
        if (isStreamRunning == true) {
            console.info('in case : toStopStream');
            mediaTest.closeStream(surfaceID);
            isStreamRunning = false;
        }
    }

    eventEmitter.on('configure', (mediaDescription, decPath, nextStep, done) => {
        console.info('in case : configure in');
        videoEncodeProcessor.configure(mediaDescription, (err) => {
            expect(err).assertUndefined();
            console.info('in case : configure success');
            setCallback(decPath, nextStep);
            eventEmitter.emit('getVideoEncoderCaps', done);
        });
    });
    eventEmitter.on('getVideoEncoderCaps', (done) => {
        videoEncodeProcessor.getVideoEncoderCaps((err, videoCaps) => {
            expect(err).assertUndefined();
            console.info("case get getVideoEncoderCaps success");
            console.info(`print videoCaps: 
            codecInfo.name ${videoCaps.codecInfo.name}
            codecInfo.type ${videoCaps.codecInfo.type}
            codecInfo.mimeType ${videoCaps.codecInfo.mimeType}
            codecInfo.isHardwareAccelerated ${videoCaps.codecInfo.isHardwareAccelerated}
            codecInfo.isSoftwareOnly ${videoCaps.codecInfo.isSoftwareOnly}
            codecInfo.isVendor ${videoCaps.codecInfo.isVendor}
            supportedBitrate [${videoCaps.supportedBitrate.min},  ${videoCaps.supportedBitrate.max}]
            supportedFormats ${videoCaps.supportedFormats}
            supportedHeightAlignment ${videoCaps.supportedHeightAlignment}
            supportedWidthAlignment ${videoCaps.supportedWidthAlignment}
            supportedWidth [${videoCaps.supportedWidth.min},  ${videoCaps.supportedWidth.max}]
            supportedHeight [${videoCaps.supportedHeight.min},  ${videoCaps.supportedHeight.max}]
            supportedProfiles ${videoCaps.supportedProfiles}
            supportedLevels ${videoCaps.supportedLevels}
            supportedBitrateMode ${videoCaps.supportedBitrateMode}
            supportedQuality [${videoCaps.supportedQuality.min},  ${videoCaps.supportedQuality.max}]
            supportedComplexity [${videoCaps.supportedComplexity.min},  ${videoCaps.supportedComplexity.max}]
            `);
            eventEmitter.emit('getInputSurface', done);
        });
    });
    eventEmitter.on('getInputSurface', (done) => {
        videoEncodeProcessor.getInputSurface((err, inputSurface) => {
            expect(err).assertUndefined();
            expect(inputSurface != undefined).assertTrue();
            surfaceID = inputSurface;
            console.info('in case : getInputSurface success, surfaceID ' + surfaceID);
            eventEmitter.emit('prepare', done);
        })
    });
    eventEmitter.on('prepare', (done) => {
        videoEncodeProcessor.prepare((err) => {
            expect(err).assertUndefined();
            console.info('in case : prepare success');
            toStartStream();
            eventEmitter.emit('start', done);
        });
    });
    eventEmitter.on('start', (done) => {
        toStartStream();
        videoEncodeProcessor.start((err) => {
            expect(err).assertUndefined();
            console.info('in case : start success');
        });
        stopBuffer = false;
    });
    eventEmitter.on('stop', (done) => {
        videoEncodeProcessor.stop((err) => {
            expect(err).assertUndefined();
            toStopStream();
            console.info('in case : stop success');
            if (reStart == true) {
                frameCountOut = 0;
                outputQueue = [];
                reStart = false;
                eventEmitter.emit('start', done);
            } else {
                eventEmitter.emit('reset', done);
            }
        });
        stopBuffer = true;
    });
    eventEmitter.on('flush', (done) => {
        videoEncodeProcessor.flush((err) => {
            expect(err).assertUndefined();
            console.info('in case : flush success');
            eventEmitter.emit('reset', done);
        });
    });
    eventEmitter.on('reset', (done) => {
        videoEncodeProcessor.reset((err) => {
            toStopStream();
            expect(err).assertUndefined();
            console.info('in case : reset success');
            eventEmitter.emit('release', done);
        });
    });
    eventEmitter.on('release', (done) => {
        videoEncodeProcessor.release((err) => {
            toStopStream();
            expect(err).assertUndefined();
            console.info('in case : release success');
            videoEncodeProcessor = null;
            done();
        });
    });

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_00_0100
        * @tc.name      : 000.test stop after last frame and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_00_0100', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_00_0100.es';
        let mime = 'video/mp4v-es';
        let mediaDescription = {
            "codec_mime": 'video/mp4v-es',
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30,
        }
        media.getMediaCapability((err, mediaCaps) => {
            expect(err).assertUndefined();
            console.info(`case getMediaCapability 1`);
            mediaCaps.getVideoEncoderCaps((err, videoCapsArray) => {
                expect(err).assertUndefined();
                console.info('getVideoDecoderCaps success');
                if (typeof (videoCapsArray) != 'undefined') {
                    console.info("case videoCapsArray" + videoCapsArray);
                } else {
                    console.info("case videoCapsArray is not defined");
                    expect().assertFail();
                    done();
                }
            })
            mediaCaps.findVideoEncoder(mediaDescription, (err, codecname) => {
                expect(err).assertUndefined();
                console.info('findVideoEncoder success');
                if (typeof (codecname) != 'undefined') {
                    console.info("case codecname " + codecname);
                } else {
                    console.info("case codecname is not defined");
                    expect().assertFail();
                    done();
                }
            })
        })
        media.createVideoEncoderByMime(mime, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('stop', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0100
        * @tc.name      : 000.test stop at running state and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0100', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0100.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        finalFrameId = 50;
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('stop', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0200
        * @tc.name      : 000.test stop at end of stream and restart
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0200', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0200.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        reStart = true;
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('stop', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0300
        * @tc.name      : 000.test stop at running state and restart
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0300', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0300.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        finalFrameId = 50;
        reStart = true;
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('stop', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0400
        * @tc.name      : 000.test flush at running state
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0400', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0400.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        finalFrameId = 50;
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('flush', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0500
        * @tc.name      : 000.test flush at end of stream
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0500', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0500.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('flush', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0600
        * @tc.name      : 000.test reconfigure
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0600', 0, async function (done) {
        let decPath = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0600.es';
        let name= 'avenc_mpeg4';
        let mediaDescription = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        let decPath2 = BASIC_PATH + 'SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_CALLBACK_01_0600_2.es';
        let mediaDescription2 = {
            'width': 320, 
            'height': 240,
            'pixel_format': 3,
            'frame_rate': 30.00,
        }
        eventEmitter.on('reset_for_callback_01_0600', (done) => {
            videoEncodeProcessor.reset((err) => {
                expect(err).assertUndefined();
                console.info('in case : reset_for_callback_01_0600 success');
                toStopStream();
                surfaceID = '';
                frameCountOut = 0;
                outputQueue = [];
                isStreamRunning = false;
                toSetStreamParam(mediaDescription2.width, mediaDescription2.height, 
                                mediaDescription2.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription2, decPath2, 
                                function(){eventEmitter.emit('stop', done);}, done);
            })
        })
        media.createVideoEncoderByName(name, (err, processor) => {
            expect(err).assertUndefined();
            if (typeof(processor) != 'undefined') {
                console.info('case create createVideoEncoderByName success');
                videoEncodeProcessor = processor;
                toCreateStream();
                toSetStreamParam(mediaDescription.width, mediaDescription.height, 
                                mediaDescription.frame_rate, frameTotal);
                eventEmitter.emit('configure', mediaDescription, decPath, 
                                function(){eventEmitter.emit('reset_for_callback_01_0600', done);}, done);
            } else {
                console.info('case create createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        });
    })
})
