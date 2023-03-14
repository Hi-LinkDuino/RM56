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


describe('VideoDecoderEnum', function () {
    let videoDecodeProcessor = null;
    let videoPlayer = null;
    let readStreamSync = undefined;
    let frameCountIn = 0;
    let frameCountOut = 0;
    let timestamp = 0;
    let inputQueue = [];
    let outputQueue = [];
    let isCodecData = false;
    let inputEosFlag = false;
    let surfaceID = '';
    const events = require('events');
    const eventEmitter = new events.EventEmitter();
    const BASIC_PATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.video.videodecoder/';
    let ES_FRAME_SIZE = [];
    const H264_FRAME_SIZE_60FPS_320 =
    [ 2106, 11465];
    beforeAll(function() {
        console.info('beforeAll case');
        // getSurfaceID();
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
        isCodecData = false;
        inputEosFlag = false;
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
        console.info(`in case error failCallback called, errMessage is ${error.message}`);
        expect(err).assertUndefined();
    }
    let failCatch = function(err) {
        console.info(`in case error failCatch called,errMessage is ${error.message}`);
        expect(err).assertUndefined();
    }
    function readFile(path){
        console.info('in case : read file start execution');
        try {
            console.info('in case: filepath ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        } catch(e) {
            console.error('in case readFile' + e);
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

    function getContent(buf, len) {
        console.info('start get content, len ' + len + ' buf.byteLength ' + buf.byteLength);
        let lengthReal = -1;
        try {
            lengthReal = readStreamSync.readSync(
                buf, 
                {length: len}
            );
            console.info('in case: lengthReal: ' + lengthReal);
        } catch(e) {
            console.error('in case error getContent ' + e);
        }
    }
    function getSurfaceID() {
        let surfaceIDTest = new ArrayBuffer(20);
        let readSurfaceID = Fileio.createStreamSync('/data/media/surfaceID.txt', 'rb');
        readSurfaceID.readSync(surfaceIDTest, {length: 13});
        let view2 = new Uint8Array(surfaceIDTest);
        for (let i = 0; i < 13; i++) {
            let value = view2[i] - 48;
            surfaceID = surfaceID + '' + value;
        }
        console.info('in case surfaceID ' + surfaceID);
        readSurfaceID.closeSync();
    }
    
    /* push inputbuffers into codec  */
    async function enqueueInputs(inputObject) {
        console.info('in case: enqueueInputs in');
        console.log('in case: inputObject.index: ' + inputObject.index);
        if (frameCountIn < ES_FRAME_SIZE.length) {
            getContent(inputObject.data, ES_FRAME_SIZE[frameCountIn]);
            inputObject.timeMs = timestamp;
            inputObject.offset = 0;
            inputObject.length = ES_FRAME_SIZE[frameCountIn];
            console.info('in case: frameCountIn ' + frameCountIn);
            frameCountIn++;
            timestamp += 16.67;
        }
        if (isCodecData) {
            inputObject.flags = 8;
            isCodecData = false;
            timestamp = 0;
        } else if (frameCountIn >= ES_FRAME_SIZE.length - 1) {
            inputObject.flags = 1;
            inputEosFlag = true;
        } else {
            inputObject.flags = 4;
        }
        if (frameCountIn == ES_FRAME_SIZE.length / 2) {
            videoDecodeProcessor.setParameter({
                'bitrate': 6400,
            }).then(() => {
                console.info('in case: setParameter success ');
            }, failCallback).catch(failCatch);
        }
        videoDecodeProcessor.pushInputData(inputObject).then(() => {
            console.info('in case: queueInput success ');
        }, failCallback).catch(failCatch);
    }

    /* get outputbuffers from codec  */
    async function dequeueOutputs(nextStep, outputObject){
        if (outputObject.flags == 1) {
            nextStep();
            return;
        }
        frameCountOut++;
        await videoDecodeProcessor.freeOutputBuffer(outputObject).then(() => {
            console.log('in case: release output count:' + frameCountOut);
        }, failCallback).catch(failCatch);
    }

    function setCallback(nextStep){
        console.info('in case:  setCallback in');
        videoDecodeProcessor.on('needInputData', async (inBuffer) => {
            expect(inBuffer.index !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.index: '+ inBuffer.index);
            expect(inBuffer.data !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.data: '+ inBuffer.data);
            expect(inBuffer.offset !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.offset: '+ inBuffer.offset);
            expect(inBuffer.length !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.length: '+ inBuffer.length);
            expect(inBuffer.flags !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.flags: '+ inBuffer.flags);
            expect(inBuffer.timeMs !== undefined).assertTrue();
            console.info('in case: inputBufferAvailable inBuffer.timeMs: '+ inBuffer.timeMs);
            enqueueInputs(inBuffer);
        });

        videoDecodeProcessor.on('newOutputData', async (outBuffer) => {
            console.info('in case: outputBufferAvailable outBuffer.index: '+ outBuffer.index);
            videoDecodeProcessor.getOutputMediaDescription().then((MediaDescription) => {
                console.info('get outputMediaDescription : ' + MediaDescription);
            }, failCallback).catch(failCatch);
            dequeueOutputs(nextStep, outBuffer);
        });

        videoDecodeProcessor.on('error',(err) => {
            console.info('in case: error called,errName is' + err);
            expect(err.code !== undefined).assertTrue();
            console.info('in case: err.code is ' + err.code);
        });

        videoDecodeProcessor.on('streamChanged',(format) => {
            console.info('in case: Output format changed: ' + format.toString());
        });
        console.info('in case:  setCallback out');
    }

    async function toCreateVideoDecoderByMime(mime, done) {
        await media.createVideoDecoderByMime(mime).then((processor) => {
            if (typeof (processor) != 'undefined') {
                videoDecodeProcessor = processor;
                console.info('in case : createVideoDecoderByMime success');
            } else {
                console.info('in case : createVideoDecoderByMime fail');
                expect().assertFail();
                done();
            }
        }, failCallback).catch(failCatch);
    }
    async function toGetVideoDecoderCaps() {
        await videoDecodeProcessor.getVideoDecoderCaps().then((videoCaps) => {
            console.info("case get getVideoDecoderCaps success");
            console.info("print videoCaps: " + videoCaps)
        }, failCallback).catch(failCatch);
    }
    async function toCreateVideoDecoderByName(name, done) {
        await media.createVideoDecoderByName(name).then((processor) => {
            if (typeof (processor) != 'undefined') {
                videoDecodeProcessor = processor;
                console.info('in case : createVideoDecoderByName success');
            } else {
                console.info('in case : createVideoDecoderByName fail');
                expect().assertFail();
                done();
            }
        }, failCallback).catch(failCatch);
    }
    async function toConfigure(mediaDescription, srcPath) {
        await videoDecodeProcessor.configure(mediaDescription).then(() =>{
            console.info('in case : configure success');
            readFile(srcPath);
        }, failCallback).catch(failCatch);
    }
    async function setSurfaceID(done) {
        await media.createVideoPlayer().then((video) => {
            if (typeof (video) != 'undefined') {
                videoPlayer = video;
                console.info('in case : createVideoPlayer success');
            } else {
                expect().assertFail();
                console.info('in case error: createVideoPlayer fail');
                done();
            }
        }, failCallback).catch(failCatch);
        await videoPlayer.getDisplaySurface().then((surface) => {
            console.info('in case :  getDisplaySurface success and surfaceID is ' + surface);
            surfaceID = surface;
        }, failCallback).catch(failCatch);
    }
    async function toSetOutputSurface(isDisplay) {
        await videoDecodeProcessor.setOutputSurface(surfaceID, isDisplay).then(() => {
            console.info('in case : setOutputSurface success. surfaceID ' + surfaceID);
        }, failCallback).catch(failCatch);
    }
    eventEmitter.on('nextStep', async (done) => {
        console.info('in case : nextStep success');
        await videoDecodeProcessor.stop().then(() => {
            console.info('in case : stop success');
        }, failCallback).catch(failCatch);
        await videoDecodeProcessor.reset().then(() => {
            console.info('in case : reset success');
        }, failCallback).catch(failCatch);
        await videoDecodeProcessor.release().then(() => {
            console.info('in case : release success');
        }, failCallback).catch(failCatch);
        videoDecodeProcessor = null;
        console.info('in case : done');
        done();
    });
    async function toPrepare() {
        await videoDecodeProcessor.prepare().then(() => {
            console.info('in case : prepare success');
        }, failCallback).catch(failCatch);
    }
    async function toStart() {
        await videoDecodeProcessor.start().then(() => {
            console.info('in case : start success');
        }, failCallback).catch(failCatch);
    }

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_DECODER_ENUM_CodecBuffer_0100
        * @tc.name      : 001.CodecBuffer
        * @tc.desc      : Test Interface CodecBuffer
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */ 
    it('SUB_MEDIA_VIDEO_DECODER_ENUM_CodecBuffer_0100', 0, async function (done) {
        ES_FRAME_SIZE = H264_FRAME_SIZE_60FPS_320;
        isCodecData = true;
        let srcPath = BASIC_PATH + 'out_320_240_10s.h264';
        let mediaDescription = {
            'track_type': 1,
            'codec_mime': 'video/avc',
            'width': 320,
            'height': 240,
            'pixel_format': 4,
            'frame_rate': 60.00,
            'max_input_size': 150000,
        }
        await toCreateVideoDecoderByMime('video/avc', done);
        await toGetVideoDecoderCaps();
        await toConfigure(mediaDescription, srcPath);
        await toSetOutputSurface(true);
        setCallback(
            function(){eventEmitter.emit('nextStep', done);}
        );
        await toPrepare();
        await toStart();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_DECODER_ENUM_CodecMimeType_0100
        * @tc.name      : 002.CodecMimeType
        * @tc.desc      : Test Enumerate CodecMimeType
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_DECODER_ENUM_CodecMimeType_0100', 0, async function (done) {
        expect(media.CodecMimeType.VIDEO_H263).assertEqual('video/h263');
        expect(media.CodecMimeType.VIDEO_AVC).assertEqual('video/avc');
        expect(media.CodecMimeType.VIDEO_MPEG2).assertEqual('video/mpeg2');
        expect(media.CodecMimeType.VIDEO_MPEG4).assertEqual('video/mp4v-es');
        expect(media.CodecMimeType.VIDEO_VP8).assertEqual('video/x-vnd.on2.vp8');
        done();
    })   
})
    
