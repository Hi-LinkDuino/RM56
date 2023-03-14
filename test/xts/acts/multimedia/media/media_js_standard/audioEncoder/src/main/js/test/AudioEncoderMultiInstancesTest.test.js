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

describe('AudioEncoderFuncPromise', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audioencoder/'
    const AUDIOPATH = RESOURCEPATH + 'S16LE.pcm';
    let readStreamSync;
    let eosframenum = 0;
    let stopAtEOS = false;
    let resetAtEOS = false;
    let flushAtEOS = false;
    let workdoneAtEOS = false;
    let needgetMediaDes = false;
    let needrelease = false;
    let frameCnt = 1;
    let timestamp = 0;
    let sawInputEOS = false;
    let sawOutputEOS = false;
    let inputQueue = [];
    let outputQueue = [];
    const ES = [0, 4096];
    let ES_LENGTH = 1500;

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
        readStreamSync = undefined;
        eosframenum = 0;
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        workdoneAtEOS = false;
        needgetMediaDes = false;
        needrelease = false
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
        ES_LENGTH = 1500;
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
        stopAtEOS = false;
        resetAtEOS = false;
        flushAtEOS = false;
        workdoneAtEOS = false;
        needgetMediaDes = false;
        needrelease = false
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
    }

    function writeHead(path, len) {
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let head = new ArrayBuffer(7);
            addADTStoPacket(head, len);
            let num = writestream.writeSync(head, {length:7});
            writestream.flushSync();
            writestream.closeSync();
        } catch(e) {
            console.info(e)
        }
    }

    function writeFile(path, buf, len) {
        try{
            let writestream = Fileio.createStreamSync(path, "ab+");
            let num = writestream.writeSync(buf, {length:len});
            writestream.flushSync();
            writestream.closeSync();
        } catch(e) {
            console.info(e);
        }
    }

    function readFile(path) {
        try{
            console.info('filepath: ' + path);
            readStreamSync = Fileio.createStreamSync(path, 'rb');
        } catch(e) {
            console.info(e);
        }
    }

    function getContent(buf, len) {
        console.info("case start get content");
        let lengthreal = -1;
        lengthreal = readStreamSync.readSync(buf,{length:len});
        console.info('lengthreal: ' + lengthreal);
    }

    function addADTStoPacket(head, len) {
        let view = new Uint8Array(head);
        console.info("start add ADTS to Packet");
        let packetLen = len + 7; // 7: head length
        let profile = 2; // 2: AAC LC  
        let freqIdx = 4; // 4: 44100HZ 
        let chanCfg = 2; // 2: 2 channel
        view[0] = 0xFF;
        view[1] = 0xF9;
        view[2] = ((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2);
        view[3] = ((chanCfg & 3) << 6) + (packetLen >> 11);
        view[4] = (packetLen & 0x7FF) >> 3;
        view[5] = ((packetLen & 7) << 5) + 0x1F;
        view[6] = 0xFC;
    }

    async function stopWork(audioEncodeProcessor) {
        await audioEncodeProcessor.stop().then(() => {
            console.info("case stop success")
        }, failCallback).catch(failCatch);
    }

    async function resetWork(audioEncodeProcessor) {
        resetParam();
        await audioEncodeProcessor.reset().then(async() => {
            console.info("case reset success");
            if (needrelease) {
                await audioEncodeProcessor.release().then(() => {
                    console.info("case release success");
                }, failCallback).catch(failCatch);
                audioEncodeProcessor = null;
            }
        }, failCallback).catch(failCatch);
    }

    async function flushWork(audioEncodeProcessor) {
        inputQueue = [];
        outputQueue = [];
        await audioEncodeProcessor.flush().then(() => {
            console.info("case flush at inputeos success");
            resetParam();
            readFile(AUDIOPATH);
            workdoneAtEOS =true;
        }, failCallback).catch(failCatch);
    }

    async function doneWork(audioEncodeProcessor) {
        await audioEncodeProcessor.stop().then(() => {
            console.info("case stop success");
        }, failCallback).catch(failCatch);
        resetParam();
        await audioEncodeProcessor.reset().then(() => {
            console.info("case reset success");
        }, failCallback).catch(failCatch);
        audioEncodeProcessor = null;
    }


    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }

    function wait(time) {
        for(let t = Date.now();Date.now() - t <= time;);
    }

    async function enqueueInputs(audioEncodeProcessor, queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            console.info("case frameCnt:" + frameCnt);
            if (frameCnt == eosframenum || frameCnt == ES_LENGTH + 1) {
                console.info("case EOS frame separately")
                inputobject.flags = 1;
                inputobject.timeMs = 0;
                inputobject.length = 0;
                sawInputEOS = true;
            } else {
                console.info("case read frame from file");
                inputobject.timeMs = timestamp;
                inputobject.offset = 0;
                inputobject.length = ES[1];
                getContent(inputobject.data, ES[1]);
                inputobject.flags = 0;
            }
            timestamp += 23;
            frameCnt += 1;
            audioEncodeProcessor.pushInputData(inputobject).then(() => {
                console.info('case queueInput success');
            });
        }
    }

    async function dequeueOutputs(audioEncodeProcessor, queue, savepath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                if (stopAtEOS) {
                    await stopWork(audioEncodeProcessor);
                } else if (resetAtEOS) {
                    await resetWork(audioEncodeProcessor);
                } else if (flushAtEOS) {
                    await flushWork(audioEncodeProcessor);
                } else if (workdoneAtEOS) {
                    await doneWork(audioEncodeProcessor);
                    done();
                } else {
                    console.info('sawOutputEOS = true');
                }
            }
            else{
                writeHead(savepath, outputobject.length);
                writeFile(savepath, outputobject.data, outputobject.length);
                console.info("write to file success");
            }
            audioEncodeProcessor.freeOutputBuffer(outputobject).then(() => {
                console.info('release output success');
            });
        }
    }

    function setCallback(audioEncodeProcessor, savepath, done) {
        console.info('case callback');
        audioEncodeProcessor.on('needInputData', async(inBuffer) => {
            console.info('inputBufferAvailable');
            inputQueue.push(inBuffer);
            await enqueueInputs(audioEncodeProcessor, inputQueue);
        });
        audioEncodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            if (needgetMediaDes) {
                audioEncodeProcessor.getOutputMediaDescription().then((MediaDescription) => {
                    console.info("get OutputMediaDescription success");
                    console.info('get outputMediaDescription : ' + MediaDescription);
                    needgetMediaDes=false;
                }, failCallback).catch(failCatch);
            }
            outputQueue.push(outBuffer);
            await dequeueOutputs(audioEncodeProcessor, outputQueue, savepath, done);
        });
        audioEncodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        audioEncodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_MULTIINSTANCE_0100
        * @tc.name      : 001.create multiple encoder2
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_MULTIINSTANCE_0100', 0, async function (done) {
        console.info("case test multiple encoder instances");
        let array = new Array();
        for (let i = 0; i < 2; i += 1) {
            await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
                if (typeof(processor) != 'undefined') {
                    console.info("case create createAudioEncoder success: " + i);
                    array[i] = processor;
                } else {
                    console.info("case create createAudioEncoder failed: " + i);
                }
            }, failCallback).catch(failCatch);
        }
        console.info('case has created multiple encoders');
        for (let j = 0; j < 2; j++) {
            resetParam();
            await array[j].reset().then(() => {
                console.info("reset encoder " + j);
            }, failCallback).catch(failCatch);
            await array[j].release().then(() => {
                console.info("case release success");
            }, failCallback).catch(failCatch);
            array[j] = null;
        }
        done();
    })
})

  