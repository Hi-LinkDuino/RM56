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

describe('videoSoftwareEncoderFuncPromise', function () {
    const ROOT = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videoencoder/results/';
    const BASIC_PATH = ROOT + 'video_func_promise_';
    let videoEncodeProcessor;
    let mediaTest;
    let surfaceID = '';
    let outputQueue = [];
    let outputCnt = 0;
    let frameTotal = 100;
    let stopAtEOS = false;
    let resetAtEOS = false;
    let flushAtEOS = false;
    let sawOutputEOS = false;
    let needGetMediaDes = false;

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
        outputQueue = [];
        outputCnt = 0;
        frameTotal = 100;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        sawOutputEOS = false;
        needGetMediaDes = false;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (videoEncodeProcessor != null){
            await videoEncodeProcessor.release().then(() => {
                console.info("case release success"); 
            }, failCallback).catch(failCatch);
            videoEncodeProcessor = null;
        }
    })

    afterAll(function() {
        console.info('afterAll case');
    })

    let failCallback = function(err) {
        console.info('case callback err : ' + err);
        expect(err).assertUndefined();
    }

    let failCatch = function(err) {
        console.info('case catch err : ' + err);
        expect(err).assertUndefined();
    }

    function msleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }

    function resetParam() {
        outputQueue = [];
        outputCnt = 0;
        frameTotal = 100;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        sawOutputEOS = false;
        needGetMediaDes = false;
    }

    function writeFile(path, buf, len){
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        } catch(e) {
            console.info(e)
        }
    }

    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    } 

    function wait(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    async function dequeueOutputs(path, done) {
        while (outputQueue.length > 0) {
            let outputObject = outputQueue.shift();
            outputCnt += 1;
            if (outputObject.flags == 1) {
                console.info("last frame, make choice");
                mediaTest.closeStream(surfaceID);
                sawOutputEOS = true;
                if (stopAtEOS) {
                    await toStop();
                } else if (resetAtEOS) {
                    await toReset();
                } else if (flushAtEOS) {
                    await toFlush();
                } else {
                    await toReset();
                    await toRelease();
                    done();
                }
            } else {
                console.info('not last frame, write data to file');
                writeFile(path, outputObject.data, outputObject.length);
                console.info("write to file success");
                videoEncodeProcessor.freeOutputBuffer(outputObject).then(() => {
                    console.info('release output success');
                });
            }
        } 
    }

    function setCallback(path, done) {
        console.info('case callback');
        videoEncodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            console.info('outBuffer.flags: ' + outBuffer.flags);
            if (needGetMediaDes) {
                videoEncodeProcessor.getOutputMediaDescription().then((MediaDescription) => {
                    console.info("get OutputMediaDescription success");
                    console.info('get outputMediaDescription : ' + MediaDescription);
                    needGetMediaDes = false;
                }, failCallback).catch(failCatch);
            }
            outputQueue.push(outBuffer);
            dequeueOutputs(path, done);
        });

        videoEncodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        videoEncodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    async function toCreateByMime(mime, done) {
        await media.createVideoEncoderByMime(mime).then((processor) => {
            if (typeof (processor) != 'undefined') {
                videoEncodeProcessor = processor;
                console.info('in case : createVideoEncoderByMime success');
            } else {
                console.info('in case : createVideoEncoderByMime fail');
                expect().assertFail();
                done();
            }
        })
    }

    async function toCreateByName(name, done) {
        await media.createVideoEncoderByName(name).then((processor) => {
            if (typeof (processor) != 'undefined') {
                videoEncodeProcessor = processor;
                console.info('in case : createVideoEncoderByName success');
            } else {
                console.info('in case : createVideoEncoderByName fail');
                expect().assertFail();
                done();
            }
        })
    }

    async function toGetVideoEncoderCaps(width, height) {
        await videoEncodeProcessor.getVideoEncoderCaps().then((videoCaps) => {
            console.info("case get getVideoEncoderCaps success");
            printVideoCaps(videoCaps, width, height);
        }, failCallback).catch(failCatch);
    }

    async function printVideoCaps(videoCaps, width, height) {
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
        await videoCaps.getPreferredFrameRate(width, height).then((valueRange) => {
            console.info("case getPreferredFrameRate valueRange success");
            if (typeof (valueRange) != 'undefined') {
                console.info('getPreferredFrameRate.min: ' + valueRange.min);
                console.info('getPreferredFrameRate.max: ' + valueRange.max);
            } else {
                console.info('case getPreferredFrameRate valueRange is not defined');
                expect().assertFail();
            }
        }, failCallback).catch(failCatch);
        await videoCaps.getSupportedFrameRate(width, height).then((valueRange) => {
            console.info("case getSupportedFrameRate valueRange success");
            if (typeof (valueRange) != 'undefined') {
                console.info('getSupportedFrameRate.min: ' + valueRange.min);
                console.info('getSupportedFrameRate.max: ' + valueRange.max);
            } else {
                console.info('case getSupportedFrameRate valueRange is not defined');
                expect().assertFail();
            }
        }, failCallback).catch(failCatch);
        await videoCaps.isSizeSupported(width, height).then((trueORfalse) => {
            console.info("case isSizeSupported valueRange for width:" + width + ", height: " + height);
            if (typeof (trueORfalse) != 'undefined') {
                console.info('videoCaps.isSizeSupported: ' + trueORfalse);
            } else {
                console.info('case isSizeSupported is not defined');
                expect().assertFail();
            }
        }, failCallback).catch(failCatch);
    }

    function toCreateStream() {
        mediaTest = mediademo.createMediaTest();
    }

    function toSetStreamParam(width, height, framerate, frameTotal) {
        console.info('case set stream parameter');
        mediaTest.setResolution(width, height);
        mediaTest.setFrameRate(framerate);
        mediaTest.setFrameCount(frameTotal);
    }

    async function toGetInputSurface() {
        await videoEncodeProcessor.getInputSurface().then((inputSurface) => {
            expect(inputSurface != undefined).assertTrue();
            console.info('case getInputSurface success');
            surfaceID = inputSurface;
        }, failCallback).catch(failCatch);
    }

    function toStartStream() {
        console.info('case start stream');
        mediaTest.startStream(surfaceID);
    }

    function toStopStream() {
        console.info('case stop stream');
        mediaTest.closeStream(surfaceID);
    }

    async function toConfigure(mediaDescription) {
        await videoEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success"); 
        }, failCallback).catch(failCatch);
    }

    async function toPrepare() {
        await videoEncodeProcessor.prepare().then(() => {
            console.info("case prepare success"); 
        }, failCallback).catch(failCatch);
    }

    async function toStart() {
        await videoEncodeProcessor.start().then(() => {
            console.info("case start success"); 
        }, failCallback).catch(failCatch);
    }

    async function toFlush() {
        outputQueue = [];
        await videoEncodeProcessor.flush().then(() => {
            console.info("case flush success"); 
        }, failCallback).catch(failCatch);
    }

    async function toStop() {
        await videoEncodeProcessor.stop().then(() => {
            console.info("case stop success"); 
        }, failCallback).catch(failCatch);
    }

    async function toReset() {
        resetParam();
        await videoEncodeProcessor.reset().then(() => {
            console.info("case reset success"); 
        }, failCallback).catch(failCatch);
    }

    async function toRelease() {
        resetParam();
        await videoEncodeProcessor.release().then(() => {
            console.info("case release success"); 
            videoEncodeProcessor = null;
        }, failCallback).catch(failCatch);
    }

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_00_0100
        * @tc.name      : 000.test stop after last frame and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_00_0100', 0, async function (done) {
        console.info("case test stop after last frame");
        let savepath = BASIC_PATH + '0000.es';
        let name= 'avenc_mpeg4';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "codec_mime": 'video/mp4v-es',
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        await media.getMediaCapability().then((mediaCaps)  => {
            console.info('getMediaCapability success');
            if (typeof (mediaCaps) != 'undefined') {
                mediaCaps.getVideoEncoderCaps().then((videoCapsArray)  => {
                    console.info('getVideoEncoderCaps from media success');
                    if (typeof (videoCapsArray) != 'undefined') {
                        let videoCaps = videoCapsArray[0];
                        console.info('print first videoCaps from videoCapsArray');
                        printVideoCaps(videoCaps, width, height);
                    } else {
                        console.info("case videoCapsArray is not defined");
                        expect().assertFail();
                        done();
                    }
                }, failCallback).catch(failCatch);
                mediaCaps.findVideoEncoder(mediaDescription).then((codecname)  => {
                    console.info('findVideoEncoder success');
                    if (typeof (codecname) != 'undefined') {
                        console.info("case codecname " + codecname);
                    } else {
                        console.info("case codecname is not defined");
                        expect().assertFail();
                        done();
                    }
                }, failCallback).catch(failCatch);
            } else {
                console.info('mediaCaps is not defined');
                expect().assertFail();
                done();
            }
        }, failCallback).catch(failCatch);
        await toCreateByName(name, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0100
    * @tc.name      : 001.test stop at runnning state and reset
    * @tc.desc      : basic encode function
    * @tc.size      : MediumTest
    * @tc.type      : Function test
    * @tc.level     : Level1
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0100', 0, async function (done) {
        console.info("case test stop at runnning state and reset");
        let savepath = BASIC_PATH + '0100.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(1000);
        await toStop();
        toStopStream();
        await toReset();
        await toRelease();
        done();
    })

    /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0200
    * @tc.name      : 002.test stop at end of stream and restart
    * @tc.desc      : basic encode function
    * @tc.size      : MediumTest
    * @tc.type      : Function test
    * @tc.level     : Level1
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0200', 0, async function (done) {
        console.info("test stop at end of stream and restart");
        let savepath = BASIC_PATH + '0200.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        frameTotal = 10;
        stopAtEOS = true;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(5000);
        resetParam();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toStart();
        toStartStream();
    })

        /* *
    * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0300
    * @tc.name      : 003.test stop at running state and restart
    * @tc.desc      : basic encode function
    * @tc.size      : MediumTest
    * @tc.type      : Function test
    * @tc.level     : Level1
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0300', 0, async function (done) {
        console.info("test stop at runnning state and restart");
        let savepath = BASIC_PATH + '0300.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        frameTotal = 1000;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(500);
        toStopStream();
        await toStop();
        resetParam();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toStart();
        toStartStream();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0400
        * @tc.name      : 004.test flush at running state
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */ 
    it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0400', 0, async function (done) {
        console.info("case test flush at running state");
        let savepath = BASIC_PATH + '0400.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        frameTotal = 500;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(500);
        await toFlush();
    })

   /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0500
        * @tc.name      : 005.test flush at end of stream
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */ 
   it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0500', 0, async function (done) {
        console.info("case test flush at end of stream");
        let savepath = BASIC_PATH + '0500.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        flushAtEOS = true;
        frameTotal = 10;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(5000);
        resetParam();
        toSetStreamParam(width, height, framerate, frameTotal);
        toStartStream();
    })
    
  /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0600
        * @tc.name      : 006.test reconfigure
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
   it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0600', 0, async function (done) {
        console.info("case test reconfigure");
        let savepath = BASIC_PATH + '0600.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        resetAtEOS = true;
        frameTotal = 10;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(5000);
        resetParam();
        width = 320;
        height = 240;
        framerate = 30;
        let mediaDescription2 = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        let savepath2 = BASIC_PATH + '0601.es';
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription2);
        setCallback(savepath2, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
    })

  /* *
        * @tc.number    : SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0700
        * @tc.name      : 007.test recreate videoencoder
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
   it('SUB_MEDIA_VIDEO_SOFTWARE_ENCODER_FUNCTION_PROMISE_01_0700', 0, async function (done) {
        console.info("case test recreate videoencoder");
        let savepath = BASIC_PATH + '0700.es';
        let mime = 'video/mp4v-es';
        let width = 320;
        let height = 240;
        let framerate = 30;
        let mediaDescription = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        resetAtEOS = true;
        frameTotal = 10;
        await toCreateByMime(mime, done);
        await toGetVideoEncoderCaps(width, height);
        toCreateStream();
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription);
        setCallback(savepath, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
        await sleep(5000);
        await toRelease();
        resetParam();
        width = 320;
        height = 240;
        framerate = 30;
        let mediaDescription2 = {
            "width": width, 
            "height": height,
            "pixel_format": 3,
            "frame_rate" : framerate,
        }
        let savepath2 = BASIC_PATH + '0701.es';
        await toCreateByMime(mime, done);
        toSetStreamParam(width, height, framerate, frameTotal);
        await toConfigure(mediaDescription2);
        setCallback(savepath2, done);
        await toGetInputSurface();
        await toPrepare();
        toStartStream();
        await toStart();
    })
})
        
