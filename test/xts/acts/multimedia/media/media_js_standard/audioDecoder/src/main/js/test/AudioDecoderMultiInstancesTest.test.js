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

describe('AudioDecoderMultiInstances', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audiodecoder/'
    const AUDIOPATH =  RESOURCEPATH + 'AAC_48000_32_1.aac';
    let readStreamSync;
    let eosframenum = 0;
    let samplerate = 44.1;
    let stopAtEOS = false;
    let resetAtEOS = false;
    let flushAtEOS = false;
    let workdoneAtEOS = false;
    let needGetMediaDes = false;
    let needrelease = false;
    let frameCnt = 1;
    let timestamp = 0;
    let sawInputEOS = false;
    let sawOutputEOS = false;
    let inputQueue = [];
    let outputQueue = [];
    let ES = [];
    let ES_LENGTH = 1500;

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
    })

    afterEach(function() {
        console.info('afterEach case');
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

    function resetParam() {
        readStreamSync = undefined;
        eosframenum = 0;
        samplerate = 44.1;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        workdoneAtEOS = false;
        needGetMediaDes = false;
        needrelease = false;
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
    }

    function writeFile(path, buf, len) {
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        }catch(e) {
            console.info(e)
        }
    }

    function readFile(path) {
        console.info('read file start execution');
        try{
            console.info('filepath: ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        }catch(e) {
            console.info(e);
        }
    }

    function getContent(buf, len) {
        console.info("case start get content");
        let lengthreal = -1;
        lengthreal = readStreamSync.readSync(buf,{length:len});
    }

    async function stopWork(audioDecodeProcessor) {
        await audioDecodeProcessor.stop().then(() => {
            console.info("case stop success")
        }, failCallback).catch(failCatch);
    }

    async function resetWork(audioDecodeProcessor) {
        resetParam();
        await audioDecodeProcessor.reset().then(() => {
            console.info("case reset success");
            if (needrelease) {
                audioDecodeProcessor = null;
            }
        }, failCallback).catch(failCatch);
    }

    async function flushWork(audioDecodeProcessor) {
        inputQueue = [];
        outputQueue = [];
        await audioDecodeProcessor.flush().then(() => {
            console.info("case flush at inputeos success");
            resetParam();
            readFile(AUDIOPATH);
            workdoneAtEOS =true;
        }, failCallback).catch(failCatch);
    }

    async function doneWork(audioDecodeProcessor) {
        await audioDecodeProcessor.stop().then(() => {
            console.info("case stop success");
        }, failCallback).catch(failCatch);
        resetParam();
        await audioDecodeProcessor.reset().then(() => {
            console.info("case reset success");
        }, failCallback).catch(failCatch);
        audioDecodeProcessor = null;
    }

    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }

    function wait(time) {
        for(let t = Date.now(); Date.now() - t <= time;);
    }

    async function enqueueAllInputs(audioDecodeProcessor, queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            if (frameCnt == eosframenum || frameCnt == ES_LENGTH + 1) {
                inputobject.flags = 1;
                inputobject.timeMs = 0;
                inputobject.length = 0;
                sawInputEOS = true;
            } else {
                console.info("case read frame from file");
                inputobject.timeMs = timestamp;
                inputobject.offset = 0;
                inputobject.length = ES[frameCnt];
                getContent(inputobject.data, ES[frameCnt]);
                inputobject.flags = 0;
            }
            timestamp += ES[frameCnt]/samplerate;
            frameCnt += 1;
            audioDecodeProcessor.pushInputData(inputobject).then(() => {
                console.info('case queueInput success');
            });
        }
    }

    async function dequeueAllOutputs(audioDecodeProcessor, queue, savapath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                console.info("sawOutputEOS == true");
                if (stopAtEOS) {
                    await stopWork(audioDecodeProcessor);
                } else if (resetAtEOS) {
                    await resetWork(audioDecodeProcessor);
                } else if (flushAtEOS) {
                    await flushWork(audioDecodeProcessor);
                } else if (workdoneAtEOS) {
                    await doneWork(audioDecodeProcessor);
                    done();
                } else {
                    console.info("saw output EOS");
                }
            }
            else{
                writeFile(savapath, outputobject.data, outputobject.length);
                console.info("write to file success");
            }
            audioDecodeProcessor.freeOutputBuffer(outputobject).then(() => {
                console.info('release output success');
            });
        }
    }

    function setCallback(audioDecodeProcessor, savepath, done) {
        console.info('case callback');
        audioDecodeProcessor.on('needInputData', async(inBuffer) => {
            console.info('inputBufferAvailable');
            inputQueue.push(inBuffer);
            await enqueueAllInputs(audioDecodeProcessor, inputQueue);
        });
        audioDecodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            console.info("outputbuffer.flags: " + outBuffer.flags);
            if (needGetMediaDes) {
                audioDecodeProcessor.getOutputMediaDescription().then((MediaDescription) => {
                    console.info("get OutputMediaDescription success");
                    console.info('get outputMediaDescription : ' + MediaDescription);
                    needGetMediaDes=false;
                }, failCallback).catch(failCatch);}
            outputQueue.push(outBuffer);
            await dequeueAllOutputs(audioDecodeProcessor, outputQueue, savepath, done);
        });
        audioDecodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        audioDecodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_DECODER_MULTIINSTANCE_0100
        * @tc.name      : 001.create multiple decoders
        * @tc.desc      : basic decode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_DECODER_MULTIINSTANCE_0100', 0, async function (done) {
        console.info("case test multiple instances");
        let array = new Array();
        for (let i = 0; i < 2; i += 1) {
            await media.createAudioDecoderByMime('audio/mp4a-latm').then((processor) => {
                if (typeof(processor) != 'undefined') {
                    console.info("case create createAudioDecoder success: " + i);
                    array[i] = processor;
                } else {
                    console.info("case create createAudioDecoder failed: " + i);
                }
            }, failCallback).catch(failCatch);
        }
        console.info('case has created multiple decoders');
        for (let j = 0; j < 2; j++) {
            resetParam();
            await array[j].reset().then(() => {
                console.info("reset decoder " + j);
            }, failCallback).catch(failCatch);
            await array[j].release().then(() => {
                console.info('release success');
                array[j] = null;
            }, failCallback).catch(failCatch);
        }
        done();
    })
})