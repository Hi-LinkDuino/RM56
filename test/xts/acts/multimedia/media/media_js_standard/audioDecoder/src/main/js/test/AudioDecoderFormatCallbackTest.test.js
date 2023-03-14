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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('AudioDecoderFormatCallback', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audiodecoder/'
    const AUDIOPATH1 = RESOURCEPATH + 'AAC_48000_32_1.aac';
    const AUDIOPATH2 = RESOURCEPATH + 'FLAC_48000_32_1.flac'
    const AUDIOPATH3 = RESOURCEPATH + 'mp3.es';
    const AUDIOPATH4 = RESOURCEPATH + 'vorbis.es';
    const BASIC_PATH = RESOURCEPATH + 'results/decode_format_callback_';
    let audioDecodeProcessor;
    let readStreamSync;
    let needGetMediaDes = false;
    let frameCnt = 1;
    let timestamp = 0;
    let sawInputEOS = false;
    let sawOutputEOS = false;
    let inputQueue = [];
    let outputQueue = [];
    let ES = [];
    let ES_LENGTH = 0;
    let samplerate = 44.1;
    let isMp3 = false;
    let isVorbis = false;

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
        audioDecodeProcessor = null;
        readStreamSync = undefined;
        needGetMediaDes = false;
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
        ES = [];
        ES_LENGTH = 0;
        samplerate = 44.1;
        isMp3 = false;
        isVorbis = false;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (audioDecodeProcessor != null) {
            await audioDecodeProcessor.release().then(() => {
                console.info('audioDecodeProcessor release success');
                audioDecodeProcessor = null;
            }, failCallback).catch(failCatch);
        }
    })

    afterAll(function() {
        console.info('afterAll case');
    })

    function writeFile(path, buf, len) {
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        } catch(e) {
            console.log(e);
        }
    }

    function readFile(path) {
        console.log('read file start execution');
        try{
            console.log('filepath: ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        } catch(e) {
            console.log(e);
        }
    }

    function getContent(buf, len) {
        console.log("start get content");
        let lengthreal = -1;
        lengthreal = readStreamSync.readSync(buf,{length:len});
        console.log('lengthreal: ' + lengthreal);
    }

    async function enqueueAllInputs(queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            if (frameCnt == ES_LENGTH + 1) {
                inputobject.flags = 1;
                inputobject.timeMs = 0;
                inputobject.length = 0;
                sawInputEOS = true;
            } else {
                inputobject.timeMs = timestamp;
                inputobject.offset = 0;
                if (frameCnt==1 && isVorbis) {
                    inputobject.length = ES[frameCnt];
                    getContent(inputobject.data, ES[frameCnt]);
                    inputobject.flags = 8;
                } else if (isMp3) {
                    inputobject.length = ES[1];
                    getContent(inputobject.data, ES[1]);
                    inputobject.flags = 0;
                } else {
                    inputobject.length = ES[frameCnt];
                    getContent(inputobject.data, ES[frameCnt]);
                    inputobject.flags = 0;
                }
            }
            if (isMp3) {
                timestamp += ES[1]/samplerate;
            } else {
                timestamp += ES[frameCnt]/samplerate;
            }
            frameCnt += 1;
            audioDecodeProcessor.pushInputData(inputobject, () => {
                console.info('queueInput success');
            })
        }
    }

    async function dequeueAllOutputs(queue, savepath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                audioDecodeProcessor.stop((err) => {
                    expect(err).assertUndefined();
                    console.log("stop success");
                    inputQueue = [];
                    outputQueue = [];
                    audioDecodeProcessor.reset((err) => {
                        expect(err).assertUndefined();
                        console.log("reset success");
                        audioDecodeProcessor.release((err) => {
                            expect(err).assertUndefined();
                            console.log("case release success");
                            audioDecodeProcessor = null;
                            done();
                        })
                    })
                })
            } else {
                writeFile(savepath, outputobject.data, outputobject.length);
                console.log("write to file success");
            }
            audioDecodeProcessor.freeOutputBuffer(outputobject, () => {
                console.info('release output success');
            })
        }
    }

    function setCallback(savepath, done) {
        console.info('case callback');
        audioDecodeProcessor.on('needInputData', async(inBuffer) => {
            console.info("inputBufferAvailable");
            inputQueue.push(inBuffer);
            await enqueueAllInputs(inputQueue);
        });
        audioDecodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info("outputBufferAvailable");
            if (needGetMediaDes) {
                audioDecodeProcessor.getOutputMediaDescription((err, MediaDescription) => {
                    console.log("get OutputMediaDescription success");
                    console.log('get outputMediaDescription : ' + MediaDescription);
                    needGetMediaDes=false;
                })
            }
            outputQueue.push(outBuffer);
            await dequeueAllOutputs(outputQueue, savepath, done);
        });
        audioDecodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        audioDecodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0100
        * @tc.name      : 001.test aac format(createbymime)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0100', 0, async function (done) {
        console.log("case test aac format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'aac_01.pcm';
        needGetMediaDes = true;
        ES = [0, 283, 336, 291, 405, 438, 411, 215, 215, 313, 270, 342, 641, 554, 545, 545, 546,
             541, 540, 542, 552, 537, 533, 498, 472, 445, 430, 445, 427, 414, 386, 413, 370, 380,
             401, 393, 369, 391, 367, 395, 396, 396, 385, 391, 384, 395, 392, 386, 388, 384, 379,
             376, 381, 375, 373, 349, 391, 357, 384, 395, 384, 380, 386, 372, 386, 383, 378, 385,
             385, 384, 342, 390, 379, 387, 386, 393, 397, 362, 393, 394, 391, 383, 385, 377, 379,
             381, 369, 375, 379, 346, 382, 356, 361, 366, 394, 393, 385, 362, 406, 399, 384, 377,
             385, 389, 375, 346, 396, 388, 381, 383, 352, 357, 397, 382, 395, 376, 388, 373, 374,
             353, 383, 384, 393, 379, 348, 364, 389, 380, 381, 388, 423, 392, 381, 368, 351, 391,
             355, 358, 395, 390, 385, 382, 383, 388, 388, 389, 376, 379, 376, 384, 369, 354, 390,
             389, 396, 393, 382, 385, 353, 383, 381, 377, 411, 387, 390, 377, 349, 381, 390, 378,
             373, 375, 381, 351, 392, 381, 380, 381, 378, 387, 379, 383, 348, 386, 364, 386, 371,
             399, 399, 385, 380, 355, 397, 395, 382, 380, 386, 352, 387, 390, 373, 372, 388, 378,
             385, 368, 385, 370, 378, 373, 383, 368, 373, 388, 351, 384, 391, 387, 389, 383, 355,
             361, 392, 386, 354, 394, 392, 397, 392, 352, 381, 395, 349, 383, 390, 392, 350, 393,
             393, 385, 389, 393, 382, 378, 384, 378, 375, 373, 375, 389, 377, 383, 387, 373, 344,
             388, 379, 391, 373, 384, 358, 361, 391, 394, 363, 350, 361, 395, 399, 389, 398, 375,
             398, 400, 381, 354, 363, 366, 400, 400, 356, 370, 400, 394, 398, 385, 378, 372, 354,
             359, 393, 381, 363, 396, 396, 355, 390, 356, 355, 371, 399, 367, 406, 375, 377, 405,
             401, 390, 393, 392, 384, 386, 374, 358, 397, 389, 393, 385, 345, 379, 357, 388, 356,
             381, 389, 367, 358, 391, 360, 394, 396, 357, 395, 388, 394, 383, 357, 383, 392, 394,
             376, 379, 356, 386, 395, 387, 377, 377, 389, 377, 385, 351, 387, 350, 388, 384, 345,
             358, 368, 399, 394, 385, 384, 395, 378, 387, 386, 386, 376, 375, 382, 351, 359, 356,
             401, 388, 363, 406, 363, 374, 435, 366, 400, 393, 392, 371, 391, 359, 359, 397, 388,
             390, 420, 411, 369, 384, 382, 383, 383, 375, 381, 361, 380, 348, 379, 386, 379, 379,
             386, 371, 352, 378, 378, 388, 384, 385, 352, 355, 387, 383, 379, 362, 386, 399, 376,
             390, 350, 387, 357, 403, 398, 397, 360, 351, 394, 400, 399, 393, 388, 395, 370, 377,
             395, 360, 346, 381, 370, 390, 380, 391, 387, 382, 384, 383, 354, 349, 394, 358, 387,
             400, 386, 402, 354, 396, 387, 391, 365, 377, 359, 361, 365, 395, 388, 388, 384, 388,
             378, 374, 382, 376, 377, 389, 378, 341, 390, 376, 381, 375, 414, 368, 369, 387, 411,
             396, 391, 378, 389, 349, 383, 344, 381, 387, 380, 353, 361, 391, 365, 390, 396, 382,
             386, 385, 385, 409, 387, 386, 378, 372, 372, 374, 349, 388, 389, 348, 395, 380, 382,
             388, 375, 347, 383, 359, 389, 368, 361, 405, 398, 393, 395, 359, 360, 395, 395, 362,
             354, 388, 348, 388, 386, 390, 350, 388, 356, 369, 364, 404, 404, 391, 394, 385, 439,
             432, 375, 366, 441, 362, 367, 382, 374, 346, 391, 371, 354, 376, 390, 373, 382, 385,
             389, 378, 377, 347, 414, 338, 348, 385, 352, 385, 386, 381, 388, 387, 364, 465, 405,
             443, 387, 339, 376, 337, 379, 387, 370, 374, 358, 354, 357, 393, 356, 381, 357, 407,
             361, 397, 362, 394, 394, 392, 394, 391, 381, 386, 379, 354, 351, 392, 408, 393, 389,
             388, 385, 375, 388, 375, 388, 375, 354, 384, 379, 386, 394, 383, 359, 405, 395, 352,
             345, 403, 427, 373, 380, 350, 415, 378, 434, 385, 388, 387, 400, 405, 329, 391, 356,
             419, 358, 359, 375, 367, 391, 359, 369, 361, 376, 378, 379, 348, 390, 345, 388, 390,
             406, 349, 368, 364, 391, 384, 401, 384, 391, 361, 399, 359, 386, 392, 382, 386, 380,
             383, 345, 376, 393, 400, 395, 343, 352, 354, 381, 388, 357, 393, 389, 384, 389, 388,
             384, 404, 372, 358, 381, 352, 355, 485, 393, 371, 376, 389, 377, 391, 387, 376, 342,
             390, 375, 379, 396, 376, 402, 353, 392, 382, 383, 387, 386, 372, 377, 382, 388, 381,
             387, 357, 393, 385, 346, 389, 388, 357, 362, 404, 398, 397, 402, 371, 351, 370, 362,
             350, 388, 399, 402, 406, 377, 396, 359, 372, 390, 392, 368, 383, 346, 384, 381, 379,
             367, 384, 389, 381, 371, 358, 422, 372, 382, 374, 444, 412, 369, 362, 373, 389, 401,
             383, 380, 366, 365, 361, 379, 372, 345, 382, 375, 376, 375, 382, 356, 395, 383, 384,
             391, 361, 396, 407, 365, 351, 385, 378, 403, 344, 352, 387, 397, 399, 377, 371, 381,
             415, 382, 388, 368, 383, 405, 390, 386, 384, 374, 375, 381, 371, 372, 374, 377, 346,
             358, 381, 377, 359, 385, 396, 385, 390, 389, 391, 375, 357, 389, 390, 377, 370, 379,
             351, 381, 381, 380, 371, 386, 389, 389, 383, 362, 393, 388, 355, 396, 383, 352, 384,
             352, 383, 362, 396, 385, 396, 357, 388, 382, 377, 373, 379, 383, 386, 350, 393, 355,
             380, 401, 392, 391, 402, 391, 427, 407, 394, 332, 398, 367, 373, 343, 381, 383, 386,
             382, 349, 353, 393, 378, 386, 375, 390, 356, 392, 384, 387, 380, 381, 385, 386, 383,
             378, 379, 359, 381, 382, 388, 357, 357, 397, 358, 424, 382, 352, 409, 374, 368, 365,
             399, 352, 393, 389, 385, 352, 380, 398, 389, 385, 387, 387, 353, 402, 396, 386, 357,
             395, 368, 369, 407, 394, 383, 362, 380, 385, 368, 375, 365, 379, 377, 388, 380, 346,
             383, 381, 399, 359, 386, 455, 368, 406, 377, 339, 381, 377, 373, 371, 338];
        ES_LENGTH = 1000;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH1);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByMime('audio/mp4a-latm', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

   /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0101
        * @tc.name      : 001.test aac format(createbyname)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0101', 0, async function (done) {
        console.log("case test aac format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 2,
                    "sample_rate": 44100,
                    "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'aac_02.pcm';
        needGetMediaDes = true;
        ES = [0, 283, 336, 291, 405, 438, 411, 215, 215, 313, 270, 342, 641, 554, 545, 545, 546,
             541, 540, 542, 552, 537, 533, 498, 472, 445, 430, 445, 427, 414, 386, 413, 370, 380,
             401, 393, 369, 391, 367, 395, 396, 396, 385, 391, 384, 395, 392, 386, 388, 384, 379,
             376, 381, 375, 373, 349, 391, 357, 384, 395, 384, 380, 386, 372, 386, 383, 378, 385,
             385, 384, 342, 390, 379, 387, 386, 393, 397, 362, 393, 394, 391, 383, 385, 377, 379,
             381, 369, 375, 379, 346, 382, 356, 361, 366, 394, 393, 385, 362, 406, 399, 384, 377,
             385, 389, 375, 346, 396, 388, 381, 383, 352, 357, 397, 382, 395, 376, 388, 373, 374,
             353, 383, 384, 393, 379, 348, 364, 389, 380, 381, 388, 423, 392, 381, 368, 351, 391,
             355, 358, 395, 390, 385, 382, 383, 388, 388, 389, 376, 379, 376, 384, 369, 354, 390,
             389, 396, 393, 382, 385, 353, 383, 381, 377, 411, 387, 390, 377, 349, 381, 390, 378,
             373, 375, 381, 351, 392, 381, 380, 381, 378, 387, 379, 383, 348, 386, 364, 386, 371,
             399, 399, 385, 380, 355, 397, 395, 382, 380, 386, 352, 387, 390, 373, 372, 388, 378,
             385, 368, 385, 370, 378, 373, 383, 368, 373, 388, 351, 384, 391, 387, 389, 383, 355,
             361, 392, 386, 354, 394, 392, 397, 392, 352, 381, 395, 349, 383, 390, 392, 350, 393,
             393, 385, 389, 393, 382, 378, 384, 378, 375, 373, 375, 389, 377, 383, 387, 373, 344,
             388, 379, 391, 373, 384, 358, 361, 391, 394, 363, 350, 361, 395, 399, 389, 398, 375,
             398, 400, 381, 354, 363, 366, 400, 400, 356, 370, 400, 394, 398, 385, 378, 372, 354,
             359, 393, 381, 363, 396, 396, 355, 390, 356, 355, 371, 399, 367, 406, 375, 377, 405,
             401, 390, 393, 392, 384, 386, 374, 358, 397, 389, 393, 385, 345, 379, 357, 388, 356,
             381, 389, 367, 358, 391, 360, 394, 396, 357, 395, 388, 394, 383, 357, 383, 392, 394,
             376, 379, 356, 386, 395, 387, 377, 377, 389, 377, 385, 351, 387, 350, 388, 384, 345,
             358, 368, 399, 394, 385, 384, 395, 378, 387, 386, 386, 376, 375, 382, 351, 359, 356,
             401, 388, 363, 406, 363, 374, 435, 366, 400, 393, 392, 371, 391, 359, 359, 397, 388,
             390, 420, 411, 369, 384, 382, 383, 383, 375, 381, 361, 380, 348, 379, 386, 379, 379,
             386, 371, 352, 378, 378, 388, 384, 385, 352, 355, 387, 383, 379, 362, 386, 399, 376,
             390, 350, 387, 357, 403, 398, 397, 360, 351, 394, 400, 399, 393, 388, 395, 370, 377,
             395, 360, 346, 381, 370, 390, 380, 391, 387, 382, 384, 383, 354, 349, 394, 358, 387,
             400, 386, 402, 354, 396, 387, 391, 365, 377, 359, 361, 365, 395, 388, 388, 384, 388,
             378, 374, 382, 376, 377, 389, 378, 341, 390, 376, 381, 375, 414, 368, 369, 387, 411,
             396, 391, 378, 389, 349, 383, 344, 381, 387, 380, 353, 361, 391, 365, 390, 396, 382,
             386, 385, 385, 409, 387, 386, 378, 372, 372, 374, 349, 388, 389, 348, 395, 380, 382,
             388, 375, 347, 383, 359, 389, 368, 361, 405, 398, 393, 395, 359, 360, 395, 395, 362,
             354, 388, 348, 388, 386, 390, 350, 388, 356, 369, 364, 404, 404, 391, 394, 385, 439,
             432, 375, 366, 441, 362, 367, 382, 374, 346, 391, 371, 354, 376, 390, 373, 382, 385,
             389, 378, 377, 347, 414, 338, 348, 385, 352, 385, 386, 381, 388, 387, 364, 465, 405,
             443, 387, 339, 376, 337, 379, 387, 370, 374, 358, 354, 357, 393, 356, 381, 357, 407,
             361, 397, 362, 394, 394, 392, 394, 391, 381, 386, 379, 354, 351, 392, 408, 393, 389,
             388, 385, 375, 388, 375, 388, 375, 354, 384, 379, 386, 394, 383, 359, 405, 395, 352,
             345, 403, 427, 373, 380, 350, 415, 378, 434, 385, 388, 387, 400, 405, 329, 391, 356,
             419, 358, 359, 375, 367, 391, 359, 369, 361, 376, 378, 379, 348, 390, 345, 388, 390,
             406, 349, 368, 364, 391, 384, 401, 384, 391, 361, 399, 359, 386, 392, 382, 386, 380,
             383, 345, 376, 393, 400, 395, 343, 352, 354, 381, 388, 357, 393, 389, 384, 389, 388,
             384, 404, 372, 358, 381, 352, 355, 485, 393, 371, 376, 389, 377, 391, 387, 376, 342,
             390, 375, 379, 396, 376, 402, 353, 392, 382, 383, 387, 386, 372, 377, 382, 388, 381,
             387, 357, 393, 385, 346, 389, 388, 357, 362, 404, 398, 397, 402, 371, 351, 370, 362,
             350, 388, 399, 402, 406, 377, 396, 359, 372, 390, 392, 368, 383, 346, 384, 381, 379,
             367, 384, 389, 381, 371, 358, 422, 372, 382, 374, 444, 412, 369, 362, 373, 389, 401,
             383, 380, 366, 365, 361, 379, 372, 345, 382, 375, 376, 375, 382, 356, 395, 383, 384,
             391, 361, 396, 407, 365, 351, 385, 378, 403, 344, 352, 387, 397, 399, 377, 371, 381,
             415, 382, 388, 368, 383, 405, 390, 386, 384, 374, 375, 381, 371, 372, 374, 377, 346,
             358, 381, 377, 359, 385, 396, 385, 390, 389, 391, 375, 357, 389, 390, 377, 370, 379,
             351, 381, 381, 380, 371, 386, 389, 389, 383, 362, 393, 388, 355, 396, 383, 352, 384,
             352, 383, 362, 396, 385, 396, 357, 388, 382, 377, 373, 379, 383, 386, 350, 393, 355,
             380, 401, 392, 391, 402, 391, 427, 407, 394, 332, 398, 367, 373, 343, 381, 383, 386,
             382, 349, 353, 393, 378, 386, 375, 390, 356, 392, 384, 387, 380, 381, 385, 386, 383,
             378, 379, 359, 381, 382, 388, 357, 357, 397, 358, 424, 382, 352, 409, 374, 368, 365,
             399, 352, 393, 389, 385, 352, 380, 398, 389, 385, 387, 387, 353, 402, 396, 386, 357,
             395, 368, 369, 407, 394, 383, 362, 380, 385, 368, 375, 365, 379, 377, 388, 380, 346,
             383, 381, 399, 359, 386, 455, 368, 406, 377, 339, 381, 377, 373, 371, 338];
        ES_LENGTH = 1000;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH1);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByName('avdec_aac', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0200
        * @tc.name      : 002.test flac format(createbymime)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0200', 0, async function (done) {
        console.log("case test flac format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 1,
                    "sample_rate": 48000,
                    "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'flac_01.pcm';
        needGetMediaDes = true;
        samplerate = 48;
        ES = [0, 2116, 2093, 2886, 2859, 2798, 2778, 2752, 2752, 2754, 2720, 2898, 2829, 2806,
            2796,2786, 2774, 2758, 2741, 3489, 3342, 3272, 3167, 3048, 3060, 2919, 2839, 2794,
            2770, 2763,2756, 2722, 2950, 2876, 2842, 2827, 2792, 3121, 3075, 2969, 3240, 3199,
            3082, 2975, 2895, 3135, 3024, 2970, 2902, 2860, 2802, 2778, 2779, 2963, 2893, 2870,
            2831, 2800, 2762, 2752, 2760, 2759, 2726, 2889, 2960, 2876, 3223, 3040, 3006, 3042,
            2980, 3097, 3069, 3215, 3365, 3296, 3653, 3480, 3300, 3140, 3014, 2975, 2952, 2896,
            2897, 2842, 2929, 3567, 3929, 3485, 3082, 3625, 4953, 5239, 5094, 4993, 4821, 4431,
            5195, 5542, 5557, 4894, 4414, 5085, 5496, 5089, 4459, 4532, 4778, 5460, 5372, 5224,
            4395, 4701, 5512, 5304, 4798, 7128, 6534, 5704, 5562, 5539, 5675, 5780, 6058, 6194,
            6076, 5971, 5758, 5202, 4438, 3821, 4267, 5550, 4896, 3841, 5166, 4904, 4504, 5530,
            5354, 5047, 5695, 5422, 5168, 5338, 5154, 5385, 5322, 4934, 4512, 4001, 3793, 3624,
            3589, 3562, 4346, 5407, 5582, 5645, 5581, 5605, 5520, 5399, 5164, 4991, 4556, 4058,
            3662, 3757, 4627, 5183, 5061, 4114, 5840, 5051, 5126, 4573, 5269, 4437, 5021, 5508,
            5289, 5171, 5153, 5118, 5721, 5375, 4863, 6353, 5477, 5608, 5614, 5737, 5178, 4614,
            4671, 5204, 5027, 4977, 3922, 5904];
        ES_LENGTH = 200;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH2);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByMime('audio/flac', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0201
        * @tc.name      : 002.test flac format(createbyname)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0201', 0, async function (done) {
        console.log("case test flac format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
                    "channel_count": 1,
                    "sample_rate": 48000,
                    "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'flac_02.pcm';
        needGetMediaDes = true;
        samplerate = 48;
        ES = [0, 2116, 2093, 2886, 2859, 2798, 2778, 2752, 2752, 2754, 2720, 2898, 2829, 2806,
            2796,2786, 2774, 2758, 2741, 3489, 3342, 3272, 3167, 3048, 3060, 2919, 2839, 2794,
            2770, 2763,2756, 2722, 2950, 2876, 2842, 2827, 2792, 3121, 3075, 2969, 3240, 3199,
            3082, 2975, 2895, 3135, 3024, 2970, 2902, 2860, 2802, 2778, 2779, 2963, 2893, 2870,
            2831, 2800, 2762, 2752, 2760, 2759, 2726, 2889, 2960, 2876, 3223, 3040, 3006, 3042,
            2980, 3097, 3069, 3215, 3365, 3296, 3653, 3480, 3300, 3140, 3014, 2975, 2952, 2896,
            2897, 2842, 2929, 3567, 3929, 3485, 3082, 3625, 4953, 5239, 5094, 4993, 4821, 4431,
            5195, 5542, 5557, 4894, 4414, 5085, 5496, 5089, 4459, 4532, 4778, 5460, 5372, 5224,
            4395, 4701, 5512, 5304, 4798, 7128, 6534, 5704, 5562, 5539, 5675, 5780, 6058, 6194,
            6076, 5971, 5758, 5202, 4438, 3821, 4267, 5550, 4896, 3841, 5166, 4904, 4504, 5530,
            5354, 5047, 5695, 5422, 5168, 5338, 5154, 5385, 5322, 4934, 4512, 4001, 3793, 3624,
            3589, 3562, 4346, 5407, 5582, 5645, 5581, 5605, 5520, 5399, 5164, 4991, 4556, 4058,
            3662, 3757, 4627, 5183, 5061, 4114, 5840, 5051, 5126, 4573, 5269, 4437, 5021, 5508,
            5289, 5171, 5153, 5118, 5721, 5375, 4863, 6353, 5477, 5608, 5614, 5737, 5178, 4614,
            4671, 5204, 5027, 4977, 3922, 5904];
        ES_LENGTH = 200;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH2);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByName('avdec_flac', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0300
        * @tc.name      : 003.test mp3 format(createbymime)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0300', 0, async function (done) {
        console.log("test mp3 format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'mp3_01.pcm';
        needGetMediaDes = true;
        isMp3 = true;
        isVorbis = false;
        ES = [0, 1044];
        ES_LENGTH = 1000;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH3);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByMime('audio/mpeg', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0301
        * @tc.name      : 003.test mp3 format(createbyname)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0301', 0, async function (done) {
        console.log("test mp3 format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'mp3_02.pcm';
        needGetMediaDes = true;
        isMp3 = true;
        isVorbis = false;
        ES = [0, 1044];
        ES_LENGTH = 1000;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH3);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByName('avdec_mp3', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0400
        * @tc.name      : 004.test vorbis format(createbymime)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0400', 0, async function (done) {
        console.log("test vorbis format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 1,
            "sample_rate": 48000,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'vorbis_01.pcm';
        needGetMediaDes = true;
        samplerate = 48;
        isVorbis = true;
        ES = [0, 3251, 33, 160, 162, 159, 148, 160, 160, 159, 167, 156, 162, 173, 135, 137, 133,
            126, 126, 130, 127, 133, 138, 132, 124, 129, 134, 136, 146, 135, 137, 144, 137, 128,
            124, 127, 129, 127, 130, 127, 134, 130, 135, 146, 142, 140, 134, 134, 128, 119, 122,
            122, 123, 121, 128, 125, 128, 122, 137, 141, 136, 135, 127, 128, 125, 128, 122, 126,
            132, 127, 127, 135, 143, 134, 148, 133, 131, 140, 143, 135, 140, 141, 135, 149, 27,
            30, 36, 32, 157, 155, 158, 154, 136, 150, 149, 142, 150, 142, 149, 139, 142, 146, 138,
            138, 144, 138, 139, 132, 120, 130, 134, 124, 127, 130, 130, 123, 133, 141, 137, 135,
            140, 147, 139, 141, 134, 131, 146, 139, 136, 136, 130, 143, 147, 144, 138, 145, 137,
            137, 138, 142, 148, 149, 138, 141, 148, 139, 139, 139, 134, 135, 131, 124, 131, 131,
            120, 121, 132, 128, 127, 130, 136, 129, 130, 123, 133, 132, 141, 147, 147, 139, 127,
            149, 157, 165, 154, 143, 153, 141, 157, 144, 146, 135, 142, 133, 134, 140, 142, 133,
            136, 133, 141, 143, 128, 127, 136, 144, 138, 141, 133, 127, 123, 138, 131, 126, 130,
            131, 135, 131, 137, 136, 125, 127, 126, 143, 142, 127, 134, 140, 144, 128, 125, 139,
            144, 135, 141, 123, 130, 127, 126, 140, 136, 140, 139, 140, 133, 132, 146, 142, 138,
            134, 130, 125, 130, 130, 129, 137, 133, 128, 130, 130, 132, 135, 126, 129, 142, 129,
            151, 150, 146, 130, 130, 144, 137, 145, 140, 126, 123, 128, 138, 136, 140, 137, 137,
            147, 146, 141, 137, 139, 117, 133, 140, 126, 137, 138, 143, 141, 141, 132, 128, 132,
            145, 148, 151, 160, 162, 154, 140, 147, 30, 28, 28, 26, 27, 27, 31, 33, 32, 145, 132,
            130, 124, 142, 140, 128, 124, 131, 127, 130, 128, 135, 125, 137, 137, 127, 128, 127,
            125, 127, 130, 124, 134, 137, 129, 127, 146, 136, 131, 124, 125, 130, 131, 135, 147,
            148, 151, 147, 143, 148, 145, 158, 142, 137, 129, 127, 156, 147, 143, 137, 135, 142,
            141, 140, 137, 136, 132, 134, 130, 124, 127, 130, 127, 126, 127, 129, 132, 134, 141,
            147, 136, 141, 141, 134, 126, 127, 136, 135, 134, 122, 130, 128, 140, 133, 124, 128,
            137, 128, 141, 135, 141, 147, 153, 151, 151, 154, 150, 147, 157, 153, 153, 153, 151,
            155, 154, 153, 150, 143, 157, 146, 150, 154, 152, 153, 28, 35, 33, 34, 37, 36, 30, 31,
            27, 37, 38, 36, 38, 37, 36, 39, 38, 36, 172, 172, 179, 178, 172, 171, 180, 173, 163,
            164, 159, 159, 155, 163, 184, 187, 176, 176, 184, 179, 174, 167, 165, 166, 160, 166,
            169, 169, 159, 165, 166, 168, 162, 180, 174, 186, 179, 180, 186, 187, 178, 156, 158,
            156, 161, 165, 163, 160, 169, 160, 160, 168, 161, 159, 167, 173, 177, 170, 172, 172,
            169, 157, 161, 155, 165, 164, 154, 161, 160, 31, 30, 28, 31, 31, 29, 38, 36, 40, 31,
            33, 37, 39, 179, 30, 28, 27, 30, 28, 30, 30, 37, 42, 39, 184, 163, 158, 172, 166, 161,
            160, 167, 174, 175, 183, 176, 192, 176, 170, 179, 173, 164, 156, 160, 162, 152, 155,
            157, 174, 176, 184, 173, 180, 177, 182, 190, 170, 167, 154, 156, 171, 169, 172, 177,
            190, 192, 186, 188, 192, 192, 176, 179, 175, 191, 174, 197, 194, 203, 198, 190, 185,
            202, 179, 169, 179, 184, 193, 185, 185, 181, 197, 188, 189, 188, 189, 193, 197, 198,
            203, 188, 189, 198, 190, 196, 191, 191, 197, 202, 182, 178, 186, 194, 192, 186, 187,
            188, 186, 179, 179, 168, 165, 173, 165, 160, 161, 145, 162, 149, 155, 154, 145, 154,
            152, 152, 163, 174, 175, 170, 181, 177, 162, 167, 164, 162, 147, 147, 157, 159, 27,
            28, 28, 36, 36, 37, 35, 35, 35, 35, 37, 37, 35, 35, 36, 171, 40, 40, 176, 178, 171,
            174, 165, 161, 157, 160, 154, 167, 163, 174, 172, 155, 158, 165, 166, 181, 190, 160,
            158, 160, 30, 39, 36, 174, 157, 180, 174, 167, 178, 177, 171, 175, 173, 166, 28, 28,
            28, 29, 30, 34, 35, 36, 29, 36, 37, 38, 32, 29, 41, 38, 38, 35, 34, 34, 36, 34, 35,
            30, 30, 31, 31, 28, 30, 31, 36, 36, 36, 40, 38, 44, 41, 34, 38, 38, 38, 39, 184, 157,
            155, 167, 178];
        ES_LENGTH = 757;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH4);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByMime('audio/vorbis', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })

        /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0401
        * @tc.name      : 004.test vorbis format(createbyname)
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_DECODER_FORMAT_CALLBACK_01_0401', 0, async function (done) {
        console.log("test vorbis format");
        let events = require('events');
        let eventEmitter = new events.EventEmitter();
        let mediaDescription = {
            "channel_count": 1,
            "sample_rate": 48000,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + 'vorbis_02.pcm';
        needGetMediaDes = true;
        samplerate = 48;
        isVorbis = true;
        ES = [0, 3251, 33, 160, 162, 159, 148, 160, 160, 159, 167, 156, 162, 173, 135, 137, 133,
            126, 126, 130, 127, 133, 138, 132, 124, 129, 134, 136, 146, 135, 137, 144, 137, 128,
            124, 127, 129, 127, 130, 127, 134, 130, 135, 146, 142, 140, 134, 134, 128, 119, 122,
            122, 123, 121, 128, 125, 128, 122, 137, 141, 136, 135, 127, 128, 125, 128, 122, 126,
            132, 127, 127, 135, 143, 134, 148, 133, 131, 140, 143, 135, 140, 141, 135, 149, 27,
            30, 36, 32, 157, 155, 158, 154, 136, 150, 149, 142, 150, 142, 149, 139, 142, 146, 138,
            138, 144, 138, 139, 132, 120, 130, 134, 124, 127, 130, 130, 123, 133, 141, 137, 135,
            140, 147, 139, 141, 134, 131, 146, 139, 136, 136, 130, 143, 147, 144, 138, 145, 137,
            137, 138, 142, 148, 149, 138, 141, 148, 139, 139, 139, 134, 135, 131, 124, 131, 131,
            120, 121, 132, 128, 127, 130, 136, 129, 130, 123, 133, 132, 141, 147, 147, 139, 127,
            149, 157, 165, 154, 143, 153, 141, 157, 144, 146, 135, 142, 133, 134, 140, 142, 133,
            136, 133, 141, 143, 128, 127, 136, 144, 138, 141, 133, 127, 123, 138, 131, 126, 130,
            131, 135, 131, 137, 136, 125, 127, 126, 143, 142, 127, 134, 140, 144, 128, 125, 139,
            144, 135, 141, 123, 130, 127, 126, 140, 136, 140, 139, 140, 133, 132, 146, 142, 138,
            134, 130, 125, 130, 130, 129, 137, 133, 128, 130, 130, 132, 135, 126, 129, 142, 129,
            151, 150, 146, 130, 130, 144, 137, 145, 140, 126, 123, 128, 138, 136, 140, 137, 137,
            147, 146, 141, 137, 139, 117, 133, 140, 126, 137, 138, 143, 141, 141, 132, 128, 132,
            145, 148, 151, 160, 162, 154, 140, 147, 30, 28, 28, 26, 27, 27, 31, 33, 32, 145, 132,
            130, 124, 142, 140, 128, 124, 131, 127, 130, 128, 135, 125, 137, 137, 127, 128, 127,
            125, 127, 130, 124, 134, 137, 129, 127, 146, 136, 131, 124, 125, 130, 131, 135, 147,
            148, 151, 147, 143, 148, 145, 158, 142, 137, 129, 127, 156, 147, 143, 137, 135, 142,
            141, 140, 137, 136, 132, 134, 130, 124, 127, 130, 127, 126, 127, 129, 132, 134, 141,
            147, 136, 141, 141, 134, 126, 127, 136, 135, 134, 122, 130, 128, 140, 133, 124, 128,
            137, 128, 141, 135, 141, 147, 153, 151, 151, 154, 150, 147, 157, 153, 153, 153, 151,
            155, 154, 153, 150, 143, 157, 146, 150, 154, 152, 153, 28, 35, 33, 34, 37, 36, 30, 31,
            27, 37, 38, 36, 38, 37, 36, 39, 38, 36, 172, 172, 179, 178, 172, 171, 180, 173, 163,
            164, 159, 159, 155, 163, 184, 187, 176, 176, 184, 179, 174, 167, 165, 166, 160, 166,
            169, 169, 159, 165, 166, 168, 162, 180, 174, 186, 179, 180, 186, 187, 178, 156, 158,
            156, 161, 165, 163, 160, 169, 160, 160, 168, 161, 159, 167, 173, 177, 170, 172, 172,
            169, 157, 161, 155, 165, 164, 154, 161, 160, 31, 30, 28, 31, 31, 29, 38, 36, 40, 31,
            33, 37, 39, 179, 30, 28, 27, 30, 28, 30, 30, 37, 42, 39, 184, 163, 158, 172, 166, 161,
            160, 167, 174, 175, 183, 176, 192, 176, 170, 179, 173, 164, 156, 160, 162, 152, 155,
            157, 174, 176, 184, 173, 180, 177, 182, 190, 170, 167, 154, 156, 171, 169, 172, 177,
            190, 192, 186, 188, 192, 192, 176, 179, 175, 191, 174, 197, 194, 203, 198, 190, 185,
            202, 179, 169, 179, 184, 193, 185, 185, 181, 197, 188, 189, 188, 189, 193, 197, 198,
            203, 188, 189, 198, 190, 196, 191, 191, 197, 202, 182, 178, 186, 194, 192, 186, 187,
            188, 186, 179, 179, 168, 165, 173, 165, 160, 161, 145, 162, 149, 155, 154, 145, 154,
            152, 152, 163, 174, 175, 170, 181, 177, 162, 167, 164, 162, 147, 147, 157, 159, 27,
            28, 28, 36, 36, 37, 35, 35, 35, 35, 37, 37, 35, 35, 36, 171, 40, 40, 176, 178, 171,
            174, 165, 161, 157, 160, 154, 167, 163, 174, 172, 155, 158, 165, 166, 181, 190, 160,
            158, 160, 30, 39, 36, 174, 157, 180, 174, 167, 178, 177, 171, 175, 173, 166, 28, 28,
            28, 29, 30, 34, 35, 36, 29, 36, 37, 38, 32, 29, 41, 38, 38, 35, 34, 34, 36, 34, 35,
            30, 30, 31, 31, 28, 30, 31, 36, 36, 36, 40, 38, 44, 41, 34, 38, 38, 38, 39, 184, 157,
            155, 167, 178];
        ES_LENGTH = 757;
        eventEmitter.on('getAudioDecoderCaps', () => {
            audioDecodeProcessor.getAudioDecoderCaps((err, Audiocaps) => {
                expect(err).assertUndefined();
                console.info(`case getAudioDecoderCaps 1`);
                console.log("AudioCaps: " + Audiocaps);
                eventEmitter.emit('configure', mediaDescription);
            })
        });
        eventEmitter.on('configure', (mediaDescription) => {
            audioDecodeProcessor.configure(mediaDescription, (err) => {
                expect(err).assertUndefined();
                console.info(`case configure 1`);
                readFile(AUDIOPATH4);
                eventEmitter.emit('prepare');
            })
        });
        eventEmitter.on('prepare', () => {
            audioDecodeProcessor.prepare((err) => {
                expect(err).assertUndefined();
                console.info(`case prepare 1`);
                setCallback(savepath, done);
                eventEmitter.emit('start');
            })
        });
        eventEmitter.on('start', () => {
            audioDecodeProcessor.start((err) => {
                expect(err).assertUndefined();
                console.info(`case start 1`);
            })
        });
        media.createAudioDecoderByName('avdec_vorbis', (err, processor) => {
            expect(err).assertUndefined();
            console.info(`case createAudioDecoder 1`);
            audioDecodeProcessor = processor;
            eventEmitter.emit('getAudioDecoderCaps');
        })
    })
})