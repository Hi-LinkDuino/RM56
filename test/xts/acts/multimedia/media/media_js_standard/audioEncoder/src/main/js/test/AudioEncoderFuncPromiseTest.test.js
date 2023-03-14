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
    const BASIC_PATH = RESOURCEPATH + 'results/encode_func_promise_';
    let audioEncodeProcessor;
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
        audioEncodeProcessor = null;
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

    afterEach(async function() {
        console.info('afterEach case');
        if (audioEncodeProcessor != null) {
            await audioEncodeProcessor.release().then(() => {
                console.info('audioEncodeProcessor release success');
                audioEncodeProcessor = null;
            }, failCallback).catch(failCatch);
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

    async function stopWork() {
        await audioEncodeProcessor.stop().then(() => {
            console.info("case stop success")
        }, failCallback).catch(failCatch);
    }

    async function resetWork() {
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

    async function flushWork() {
        inputQueue = [];
        outputQueue = [];
        await audioEncodeProcessor.flush().then(() => {
            console.info("case flush at inputeos success");
            resetParam();
            readFile(AUDIOPATH);
            workdoneAtEOS =true;
        }, failCallback).catch(failCatch);
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

    async function enqueueInputs(queue) {
        while (queue.length > 0 && !sawInputEOS) {
            let inputobject = queue.shift();
            console.info("case frameCnt:" + frameCnt);
            if (frameCnt == eosframenum || frameCnt == ES_LENGTH + 1) {
                console.info("case EOS frame seperately")
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

    async function dequeueOutputs(queue, savepath, done) {
        while (queue.length > 0 && !sawOutputEOS) {
            let outputobject = queue.shift();
            if (outputobject.flags == 1) {
                sawOutputEOS = true;
                if (stopAtEOS) {
                    await stopWork();
                } else if (resetAtEOS) {
                    await resetWork();
                } else if (flushAtEOS) {
                    await flushWork();
                } else if (workdoneAtEOS) {
                    await doneWork();
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

    function setCallback(savepath, done) {
        console.info('case callback');
        audioEncodeProcessor.on('needInputData', async(inBuffer) => {
            console.info('inputBufferAvailable');
            inputQueue.push(inBuffer);
            await enqueueInputs(inputQueue);
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
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_00_0100
        * @tc.name      : 000.test set EOS after last frame and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_00_0100', 0, async function (done) {
        console.info("case test set EOS after last frame and reset");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let mediaDescription2 = {
            "codec_mime": 'audio/mp4a-latm',
        }
        let savepath = BASIC_PATH + '0000.aac';
        needgetMediaDes = true;
        workdoneAtEOS = true;
        await media.getMediaCapability().then((mediaCaps)  => {
            console.info('getMediaCapability success');
            if (typeof (mediaCaps) != 'undefined') {
                mediaCaps.getAudioEncoderCaps().then((audioCaps)  => {
                    console.info('getAudioEncoderCaps success');
                    if (typeof (audioCaps) != 'undefined') {
                        console.info("case audioCaps " + audioCaps);
                    } else {
                        console.info("case audioCaps is not defined");
                    }
                }, failCallback).catch(failCatch);
                mediaCaps.findAudioEncoder(mediaDescription2).then((codecname)  => {
                    console.info('getAudioEncoderCaps success');
                    if (typeof (codecname) != 'undefined') {
                        console.info("case codecname " + codecname);
                    } else {
                        console.info("case codecname is not defined");
                    }
                }, failCallback).catch(failCatch);
            } else {
                console.info('mediaCaps is not defined');
            }
        }, failCallback).catch(failCatch);
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps)
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success")
        }, failCallback).catch(failCatch);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0100
        * @tc.name      : 001.test set EOS manually before last frame and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0100', 0, async function (done) {
        console.info("case test set EOS manually before last frame and reset");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0100.aac';
        eosframenum = 500;
        workdoneAtEOS = true;
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps);
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success");
        }, failCallback).catch(failCatch);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0200
        * @tc.name      : 002.test flush at running state
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0200', 0, async function (done) {
        console.info("case test flush at running state");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0200.aac';
        workdoneAtEOS = true;
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps)
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH)
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success")
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success")
        }, failCallback).catch(failCatch);
        await sleep(5000).then(() => {
            inputQueue = [];
            outputQueue = [];
            audioEncodeProcessor.flush().then(() => {
                console.info("case flush after 5s")
            }, failCallback).catch(failCatch);
        });
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0300
        * @tc.name      : 003.test flush at EOS state
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0300', 0, async function (done) {
        console.info("case test flush at EOS state");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0300.aac';
        eosframenum = 500;
        flushAtEOS = true;
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps);
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success");
        }, failCallback).catch(failCatch);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0400
        * @tc.name      : 004.test stop at running state and reset
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0400', 0, async function (done) {
        console.info("case test stop at running state and reset");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0400.aac';
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps);
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success");
        }, failCallback).catch(failCatch);
        await sleep(5000).then(() => {
            audioEncodeProcessor.stop().then(() => {
                console.info("stop after 5s success");
            }, failCallback).catch(failCatch);});
        resetParam();
        await audioEncodeProcessor.reset().then(() => {
            console.info("reset success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.release().then(() => {
            console.info("case release success");
        }, failCallback).catch(failCatch);
        audioEncodeProcessor = null;
        done();
    })

     /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0500
        * @tc.name      : 005.test stop and restart
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0500', 0, async function (done) {
        console.info("case test stop and restart");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0500.aac';
        eosframenum = 100;
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps);
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success");
        }, failCallback).catch(failCatch);
        await sleep(5000).then(() => {
            console.info("stop encoding after 5s");
        });
        await audioEncodeProcessor.stop().then(() => {
            console.info("case stop after 5s success");
        }, failCallback).catch(failCatch);
        await sleep(2000).then(() => {
            console.info("restart encoding after 2s");
            resetParam();
            readFile(AUDIOPATH);
        });
        await audioEncodeProcessor.start().then(() => {
            console.info("case restart after 3s success");
            workdoneAtEOS = true;
            enqueueInputs(inputQueue);
        }, failCallback).catch(failCatch);
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0600
        * @tc.name      : 006.test reconfigure for new file with the same format
        * @tc.desc      : basic encode function
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level1
    */
    it('SUB_MEDIA_AUDIO_ENCODER_FUNCTION_PROMISE_01_0600', 0, async function (done) {
        console.info("case test reconfigure for new file with the same format");
        let mediaDescription = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        let savepath = BASIC_PATH + '0600.aac';
        eosframenum = 100;
        resetAtEOS = true;
        await media.createAudioEncoderByMime('audio/mp4a-latm').then((processor) => {
            console.info("case create createAudioEncoder success");
            audioEncodeProcessor = processor;
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.getAudioEncoderCaps().then((AudioCaps) => {
            console.info("case get AudioEncoderCaps success");
            console.info("print AudioCaps: " + AudioCaps);
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.configure(mediaDescription).then(() => {
            console.info("case configure success");
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        setCallback(savepath, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("case prepare success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("case start success");
        }, failCallback).catch(failCatch);
        let mediaDescription2 = {
            "channel_count": 2,
            "sample_rate": 44100,
            "audio_sample_format": 1,
        }
        await sleep(10000).then(() => {
            console.info("start configure 2");
        });
        await audioEncodeProcessor.configure(mediaDescription2).then(() => {
            console.info("configure 2 success");
            resetParam();
            readFile(AUDIOPATH);
        }, failCallback).catch(failCatch);
        let savepath2 = BASIC_PATH + '0601.aac';
        workdoneAtEOS = true;
        setCallback(savepath2, done);
        await audioEncodeProcessor.prepare().then(() => {
            console.info("prepare2 success");
        }, failCallback).catch(failCatch);
        await audioEncodeProcessor.start().then(() => {
            console.info("start2 success");
        }, failCallback).catch(failCatch);
    })
})