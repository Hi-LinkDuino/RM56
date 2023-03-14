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

describe('AudioEncoderReliabilityPromise', function () {
    const RESOURCEPATH = '/data/accounts/account_0/appdata/ohos.acts.multimedia.audio.audioencoder/'
    const AUDIOPATH = RESOURCEPATH + 'S16LE.pcm';
    const BASIC_PATH = RESOURCEPATH + 'results/encode_reliability_promise_';
    const END = 0;
    const CONFIGURE = 1;
    const PREPARE = 2;
    const START = 3;
    const FLUSH = 4;
    const STOP = 5;
    const RESET = 6;
    const HOLDON = 7;
    const WAITFORALLOUTS = 8;
    const CONFIGURE_ERROR = 9;
    const PREPARE_ERROR = 10;
    const START_ERROR = 11;
    const FLUSH_ERROR = 12;
    const STOP_ERROR = 13;
    const JUDGE_EOS = 14;
    const WAITTIME = 3000;
    let audioEncodeProcessor;
    let readStreamSync;
    let EOSFrameNum = 0;
    let flushAtEOS = false;
    let workdoneAtEOS = false;
    let needGetMediaDes = false;
    let needRelease = false;
    let frameCnt = 1;
    let timestamp = 0;
    let sawInputEOS = false;
    let sawOutputEOS = false;
    let inputQueue = [];
    let outputQueue = [];
    const ES_SIZE = [0, 4096];
    let ES_LENGTH = 200;
    let mime = 'audio/mp4a-latm';
    let mediaDescription = {
                "channel_count": 2,
                "sample_rate": 44100,
                "audio_sample_format": 1,
    };
    let expectError = false;

    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
        audioEncodeProcessor = null;
        readStreamSync = undefined;
        EOSFrameNum = 0;
        flushAtEOS = false;
        workdoneAtEOS = false;
        needGetMediaDes = false;
        needRelease = false
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
        ES_LENGTH = 200;
        expectError = false;
    })

    afterEach(async function() {
        console.info('afterEach case');
        if (audioEncodeProcessor != null) {
            await audioEncodeProcessor.release().then(() => {
                console.info('audioEncodeProcessor release success');
            }, failCallback).catch(failCatch);
            audioEncodeProcessor = null;
        }
    })

    afterAll(function() {
        console.info('afterAll case');
    })

    let failCallback = function(err) {
        console.info('case callback err : ' + err);
        expect(err).assertUndefined();
    }

    let failCallbackTrue = function(err, mySteps, done) {
        console.info('case callback err : ' + err);
        expect(err != undefined).assertTrue();
        nextStep(mySteps, done);
    }

    let failCatch = function(err) {
        console.info('case catch err : ' + err);
        expect(err).assertUndefined();
    }

    function resetParam() {
        readStreamSync = undefined;
        EOSFrameNum = 0;
        flushAtEOS = false;
        workdoneAtEOS = false;
        needGetMediaDes = false;
        needRelease = false
        frameCnt = 1;
        timestamp = 0;
        sawInputEOS = false;
        sawOutputEOS = false;
        inputQueue = [];
        outputQueue = [];
    }

    function createAudioEncoder(savepath, mySteps, done) {
        media.createAudioEncoderByMime(mime).then((processor) => {
            console.info(`case createAudioEncoder 1`);
            audioEncodeProcessor = processor;
            setCallback(savepath, done);
            console.info("case start api test");
            nextStep(mySteps, done);
        }, failCallback).catch(failCatch);
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

    async function doneWork() {
        await audioEncodeProcessor.stop().then(() => {
            console.info("case stop success");
        }, failCallback).catch(failCatch);
        resetParam();
        await audioEncodeProcessor.reset().then(() => {
            console.info("case reset success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.release().then(() => {
            console.info("case release success");
        }, failCallback).catch(failCatch);
        audioEncodeProcessor = null;
    }


    function sleep(time) {
        return new Promise((resolve) => setTimeout(resolve, time));
    }

    function wait(time) {
        for(let t = Date.now();Date.now() - t <= time;);
    }

    function nextStep(mySteps, done) {
        console.info("case myStep[0]: " + mySteps[0]);
        if (mySteps[0] == END) {
            audioEncodeProcessor.release().then(() => {
                console.info("case release success");
                audioEncodeProcessor = null;
                done();
            }, failCallback).catch(failCatch);
        }
        switch (mySteps[0]) {
            case CONFIGURE:
                mySteps.shift();
                console.info(`case to configure`);
                audioEncodeProcessor.configure(mediaDescription).then(() => {
                    console.info(`case configure 1`);
                    readFile(AUDIOPATH);
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case PREPARE:
                mySteps.shift();
                console.info(`case to prepare`);
                audioEncodeProcessor.prepare().then(() => {
                    console.info(`case prepare 1`);
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case START:
                mySteps.shift();
                console.info(`case to start`);
                if (sawOutputEOS) {
                    resetParam();
                    readFile(AUDIOPATH);
                    workdoneAtEOS = true;
                    enqueueInputs(inputQueue);
                }
                audioEncodeProcessor.start().then(() => {
                    console.info(`case start 1`);
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case FLUSH:
                mySteps.shift();
                console.info(`case to flush`);
                inputQueue = [];
                outputQueue = [];
                audioEncodeProcessor.flush().then(() => {
                    console.info(`case flush 1`);
                    if (flushAtEOS) {
                        resetParam();
                        readFile(AUDIOPATH);
                        workdoneAtEOS = true;
                        flushAtEOS = false;
                    }
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case STOP:
                mySteps.shift();
                console.info(`case to stop`);
                audioEncodeProcessor.stop().then(() => {
                    console.info(`case stop 1`);
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case RESET:
                mySteps.shift();
                console.info(`case to reset`);
                resetParam();
                audioEncodeProcessor.reset().then(() => {
                    console.info(`case reset 1`);
                    nextStep(mySteps, done);
                }, failCallback).catch(failCatch);
                break;
            case HOLDON:
                mySteps.shift();
                setTimeout(() =>{
                    nextStep(mySteps, done);
                }, WAITTIME);
                break;
            case WAITFORALLOUTS:
                mySteps.shift();
                console.info(`case wait for all outputs`);
                break;
            case CONFIGURE_ERROR:
                mySteps.shift();
                console.info(`case to configure 2`);
                audioEncodeProcessor.configure(mediaDescription).then(() => {
                    console.info(`case configure error 1`);
                    expect(expectError).assertTrue();
                }, (err) => {failCallbackTrue(err,  mySteps, done)}).catch(failCatch);
                break;
            case PREPARE_ERROR:
                mySteps.shift();
                console.info(`case to prepare 2`);
                audioEncodeProcessor.prepare().then(() => {
                    console.info(`case prepare error 1`);
                    expect(expectError).assertTrue();
                }, (err) => {failCallbackTrue(err,  mySteps, done)}).catch(failCatch);
                break;
            case START_ERROR:
                mySteps.shift();
                console.info(`case to start 2`);
                audioEncodeProcessor.start().then(() => {
                    console.info(`case start error 1`);
                    expect(expectError).assertTrue();
                }, (err) => {failCallbackTrue(err,  mySteps, done)}).catch(failCatch);
                break;
            case FLUSH_ERROR:
                mySteps.shift();
                console.info(`case to flush 2`);
                audioEncodeProcessor.flush().then(() => {
                    console.info(`case flush error 1`);
                    expect(expectError).assertTrue();
                }, (err) => {failCallbackTrue(err,  mySteps, done)}).catch(failCatch);
                break;
            case STOP_ERROR:
                mySteps.shift();
                console.info(`case to stop 2`);
                audioEncodeProcessor.stop().then(() => {
                    console.info(`case stop error 1`);
                    expect(expectError).assertTrue();
                }, (err) => {failCallbackTrue(err,  mySteps, done)}).catch(failCatch);
                break;
            case JUDGE_EOS:
                mySteps.shift();
                console.info(`case judge EOS state`);
                expect(sawOutputEOS).assertTrue();
                nextStep(mySteps, done);
                break;
            default:
                break;
        }
    }

    async function enqueueInputs(queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            console.info("case frameCnt:" + frameCnt);
            if (frameCnt == EOSFrameNum || frameCnt == ES_LENGTH + 1) {
                console.info("case EOS frame seperately")
                inputobject.flags = 1;
                inputobject.timeMs = 0;
                inputobject.length = 0;
                sawInputEOS = true;
            } else {
                console.info("case read frame from file");
                inputobject.timeMs = timestamp;
                inputobject.offset = 0;
                inputobject.length = ES_SIZE[1];
                getContent(inputobject.data, ES_SIZE[1]);
                inputobject.flags = 0;
            }
            timestamp += 23;
            frameCnt += 1;
            audioEncodeProcessor.pushInputData(inputobject).then(() => {
                console.info('case queueInput success');
            });
        }
    }

    async function dequeueOutputs(queue, savepath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                if (workdoneAtEOS) {
                    await doneWork();
                    done();
                } else {
                    console.info('sawOutputEOS = true');
                }
            }
            else{
                console.info("not last frame, continue");
            }
            audioEncodeProcessor.freeOutputBuffer(outputobject).then(() => {
                console.info('release output success');
            });
        }
    }

    function setCallback(savepath, done) {
        console.info('case callback');
        audioEncodeProcessor.on('needInputData', async(inBuffer) => {
            console.info('inputBufferAvailable');
            inputQueue.push(inBuffer);
            await enqueueInputs(inputQueue);
        });
        audioEncodeProcessor.on('newOutputData', async(outBuffer) => {
            console.info('outputBufferAvailable');
            if (needGetMediaDes) {
                audioEncodeProcessor.getOutputMediaDescription().then((MediaDescription) => {
                    console.info("get OutputMediaDescription success");
                    console.info('get outputMediaDescription : ' + MediaDescription);
                    needGetMediaDes=false;
                }, failCallback).catch(failCatch);
            }
            outputQueue.push(outBuffer);
            await dequeueOutputs(outputQueue, savepath, done);
        });
        audioEncodeProcessor.on('error',(err) => {
            console.info('case error called,errName is' + err);
        });
        audioEncodeProcessor.on('streamChanged',(format) => {
            console.info('Output format changed: ' + format);
        });
    }

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0100
        * @tc.name      : 001.create -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0100.es';
        let mySteps = new Array(CONFIGURE, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0200
        * @tc.name      : 002.prepare -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0200.es';
        let mySteps = new Array(CONFIGURE, PREPARE, CONFIGURE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0300
        * @tc.name      : 003.start -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, CONFIGURE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0400
        * @tc.name      : 004.flush -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, CONFIGURE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0500
        * @tc.name      : 005.stop -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP ,CONFIGURE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0600
        * @tc.name      : 006.EOS -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, CONFIGURE_ERROR, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0700
        * @tc.name      : 007.reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0700.es';
        let mySteps = new Array(RESET, CONFIGURE, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0800
        * @tc.name      : 008.configure -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0800.es';
        let mySteps = new Array(CONFIGURE, CONFIGURE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0900
        * @tc.name      : 009.configure -> reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_CONFIGURE_PROMISE_0900', 0, async function (done) {
        let savepath = BASIC_PATH + 'configure_0900.es';
        let mySteps = new Array(CONFIGURE, RESET, CONFIGURE, END);
        createAudioEncoder(savepath, mySteps, done);
    })

   /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0100
        * @tc.name      : 001.create -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0100.es';
        let mySteps = new Array(PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0200
        * @tc.name      : 002.configure -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0200.es';
        let mySteps = new Array(CONFIGURE, PREPARE, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0300
        * @tc.name      : 003.prepare -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0400
        * @tc.name      : 004.start -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0500
        * @tc.name      : 005.flush -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0600
        * @tc.name      : 006.stop -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0700
        * @tc.name      : 007.EOS -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0700.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, PREPARE_ERROR, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0800
        * @tc.name      : 008.reset -> prepare
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_PREPARE_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'prepare_0800.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, PREPARE_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

  /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0100
        * @tc.name      : 001.create -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0100.es';
        let mySteps = new Array(START_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0200
        * @tc.name      : 002.configure -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0200.es';
        let mySteps = new Array(CONFIGURE, START_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0300
        * @tc.name      : 003.prepare -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0400
        * @tc.name      : 004.start -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, START_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0500
        * @tc.name      : 005.flush -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, START_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0600
        * @tc.name      : 006.stop -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, START, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0700
        * @tc.name      : 007.EOS -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0700.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, START_ERROR, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0800
        * @tc.name      : 008.reset -> start
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_START_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'start_0800.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, START_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0100
        * @tc.name      : 001.create -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0100.es';
        let mySteps = new Array(FLUSH_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0200
        * @tc.name      : 002.configure -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0200.es';
        let mySteps = new Array(CONFIGURE, FLUSH_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0300
        * @tc.name      : 003.prepare -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, FLUSH_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0400
        * @tc.name      : 004.start -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0500
        * @tc.name      : 005.flush -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, FLUSH, WAITFORALLOUTS);
        workdoneAtEOS = true;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0600
        * @tc.name      : 006.stop -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP ,FLUSH_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0700
        * @tc.name      : 007.EOS -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0700.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, FLUSH, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0800
        * @tc.name      : 008.reset -> flush
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_FLUSH_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'flush_0800.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, FLUSH_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0100
        * @tc.name      : 001.create -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0100.es';
        let mySteps = new Array(STOP_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0200
        * @tc.name      : 002.configure -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0200.es';
        let mySteps = new Array(CONFIGURE, STOP_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0300
        * @tc.name      : 003.prepare -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, STOP_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0400
        * @tc.name      : 004.start -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0500
        * @tc.name      : 005.flush -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, STOP, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0600
        * @tc.name      : 006.stop -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, STOP_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0700
        * @tc.name      : 007.EOS -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0700.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0800
        * @tc.name      : 008.reset -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_STOP_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'stop_0800.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, STOP_ERROR, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0100
        * @tc.name      : 001.create -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0100.es';
        let mySteps = new Array(RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0200
        * @tc.name      : 002.configure -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0200.es';
        let mySteps = new Array(CONFIGURE, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0300
        * @tc.name      : 003.prepare -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0400
        * @tc.name      : 004.start -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0500
        * @tc.name      : 005.flush -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, FLUSH, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0600
        * @tc.name      : 006.stop -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0600', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0600.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, STOP, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0700
        * @tc.name      : 007.EOS -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0700', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0700.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, RESET, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0800
        * @tc.name      : 008.reset -> reset
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_RESET_PROMISE_0800', 0, async function (done) {
        let savepath = BASIC_PATH + 'reset_0800.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, RESET, RESET, END);
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0100
        * @tc.name      : 001.EOS -> flush -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0100', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0100.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, FLUSH, STOP, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0200
        * @tc.name      : 002.EOS -> flush -> EOS
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0200', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0200.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, FLUSH, WAITFORALLOUTS);
        EOSFrameNum = 2;
        flushAtEOS = true;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0300
        * @tc.name      : 003.EOS -> reset -> configure
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0300', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0300.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, RESET, CONFIGURE, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0400
        * @tc.name      : 004.EOS -> stop -> start -> EOS
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0400', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0400.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, START, WAITFORALLOUTS);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0500
        * @tc.name      : 005.EOS -> stop -> start -> stop
        * @tc.desc      : Reliability Test
        * @tc.size      : MediumTest
        * @tc.type      : Reliability
        * @tc.level     : Level2
    */
    it('SUB_MEDIA_AUDIO_ENCODER_API_EOS_PROMISE_0500', 0, async function (done) {
        let savepath = BASIC_PATH + 'eos_0500.es';
        let mySteps = new Array(CONFIGURE, PREPARE, START, HOLDON, JUDGE_EOS, STOP, START, STOP, END);
        EOSFrameNum = 2;
        createAudioEncoder(savepath, mySteps, done);
    })
})