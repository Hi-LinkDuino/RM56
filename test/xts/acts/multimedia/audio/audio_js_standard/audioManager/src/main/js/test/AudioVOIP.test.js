// @ts-nocheck
/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import audio from '@ohos.multimedia.audio';
import fileio from '@ohos.fileio';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioRenderer', function () {

    var dateTime = new Date().getTime();
    const dirPath = '/data/media/RecTest'+dateTime;
    var fpath = dirPath+'/capture_js.pcm';
    console.info('AudioFrameworkRecLog: Recording files Path: '+dirPath);

    const audioManager = audio.getAudioManager();
    console.info('AudioFrameworkRenderLog: Create AudioManger Object JS Framework');

    const audioManagerRec = audio.getAudioManager();
    console.info('AudioFrameworkRecLog: Create AudioManger Object JS Framework');

    beforeAll(function () {
        console.info('AudioFrameworkTest: beforeAll: Prerequisites at the test suite level');
        fileio.mkdirSync(dirPath);
    })

    beforeEach(async function () {
        console.info('AudioFrameworkTest: beforeEach: Prerequisites at the test case level');
    })

    afterEach(async function () {
        console.info('AudioFrameworkTest: afterEach: Test case-level clearance conditions');
        await sleep(1000);
    })

    afterAll(async function () {
        console.info('AudioFrameworkTest: afterAll: Test suite-level cleanup condition');
    })

    function sleep (ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }


    async function playbackPromise (audioParams, fpath, streamType) {
        var resultFlag = 'new';
        console.info('AudioFrameworkRenderLog: Promise : Audio Playback Function');

        const audioRen = audio.createAudioRenderer(streamType);
        console.info('AudioFrameworkRenderLog: AudioRenderer instance created: Success : Stream Type: '+streamType);

        await audioRen.setParams(audioParams).then(async function () {
            console.info('AudioFrameworkRenderLog: setParams Success ');
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setParams failed : ERROR : '+err.message);
            resultFlag = false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        console.info('AudioFrameworkRenderLog: AudioRenderer : Path : '+fpath);

        await audioRen.getParams().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRenderLog: Renderer GetParams:');
            console.info('AudioFrameworkRenderLog: Renderer format:' + audioParamsGet.format);
            console.info('AudioFrameworkRenderLog: Renderer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRenderLog: Renderer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRenderLog: Renderer encoding:' + audioParamsGet.encoding);
            console.info('AudioFrameworkRenderLog: Renderer content type:' + audioParamsGet.contentType);
            console.info('AudioFrameworkRenderLog: Renderer usage:' + audioParamsGet.usage);
            console.info('AudioFrameworkRenderLog: Renderer device role:' + audioParamsGet.deviceRole);
            console.info('AudioFrameworkRenderLog: Renderer device type:' + audioParamsGet.deviceType);
        }).catch((err) => {
            console.log('AudioFrameworkRenderLog: getParams :ERROR: '+err.message);
            resultFlag = false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioRen.start().then(async function (started) {
            console.info('AudioFrameworkRenderLog: renderInstant started '+started);
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: renderInstant start failed : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        var bufferSize;
        await audioRen.getBufferSize().then(async function (data) {
            bufferSize = data;
            console.info('AudioFrameworkRenderLog: buffer size: ' + bufferSize);
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: buffer size: ERROR : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        let ss = fileio.createStreamSync(fpath, 'r');
        console.info('AudioFrameworkRenderLog: File Path: ' + fpath);
        let discardHeader = new ArrayBuffer(44);
        ss.readSync(discardHeader);
        let totalSize = fileio.statSync(fpath).size;
        console.info('AudioFrameworkRenderLog: File totalSize size: ' +totalSize);
        totalSize = totalSize-44;
        console.info('AudioFrameworkRenderLog: File size : Removing header: ' +totalSize);
        let rlen = 0;
        while (rlen < (totalSize/3)) {
            let buf = new ArrayBuffer(bufferSize);
            rlen += ss.readSync(buf);
            console.info('BufferAudioFramework: bytes read from file: ' +rlen);
            await audioRen.write(buf);
            if(rlen > (totalSize/4) && rlen < (totalSize/3.5)) {
                await audioManager.getAudioScene().then(async function (data) {
                    if (data == audio.AudioScene.AUDIO_SCENE_VOICE_CHAT) {
                        console.info('AudioFrameworkRenderLog: getAudioScene : PASS : ' + data);
                        resultFlag=true;
                    }
                    else {
                        console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : ' + data);
                        resultFlag=false;
                    }
                }).catch((err) => {
                    console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : ' + err.message);
                    resultFlag = false;
                });
            }
        }
        console.info('AudioFrameworkRenderLog: Renderer after read');

        await audioRen.drain().then(async function (drained) {
            console.info('AudioFrameworkRenderLog: Renderer drained : '+drained);
        }).catch((err) => {
            console.error('AudioFrameworkRenderLog: Renderer drain failed : '+err.message);
            resultFlag=false;
        });

        await audioRen.stop().then(async function (stopped) {
            console.info('AudioFrameworkRenderLog: Renderer stopped : '+stopped);
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: Renderer stop failed : '+err.message);
            resultFlag=false;
        });

        await audioRen.release().then(async function (data) {
            console.info('AudioFrameworkRenderLog: Renderer release : SUCCESS : '+data);
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: Renderer release :ERROR : '+err.message);
            resultFlag=false;
        });

        return resultFlag;
    }

    async function recPromise (audioParams, fpath, streamType) {

        var resultFlag = 'new';
        console.info('AudioFrameworkRecLog: Promise : Audio Recording Function');

        var audioCapturer = audio.createAudioCapturer(streamType);
        console.info('AudioFrameworkRecLog: capturer instance created as : '+streamType);

        console.info('AudioFrameworkRecLog: audio param object created');
        await audioCapturer.setParams(audioParams);
        console.info('AudioFrameworkRecLog: setParams Success ');

        console.info('AudioFrameworkRecLog: Capturer : Path : '+fpath);

        await audioCapturer.getParams().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer GetParams:');
            console.info('AudioFrameworkRecLog: Capturer format:' + audioParamsGet.format);
            console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRecLog: Capturer encoding:' + audioParamsGet.encoding);
            console.info('AudioFrameworkRecLog: Capturer contentType:' + audioParamsGet.contentType);
            console.info('AudioFrameworkRecLog: Capturer usage:' + audioParamsGet.usage);
            console.info('AudioFrameworkRecLog: Capturer deviceRole:' + audioParamsGet.deviceRole);
            console.info('AudioFrameworkRecLog: Capturer deviceType:' + audioParamsGet.deviceType);
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: getParams :ERROR: '+err.message);
            resultFlag = false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioCapturer.start().then(async function (started) {
            console.info('AudioFrameworkRecLog: Capturer started '+started);
        }).catch((err) => {
            console.info('AudioFrameworkRecLog Capturer started failed : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        var bufferSize = await audioCapturer.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);

        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }
        else{
            console.info('AudioFrameworkRecLog: file fd create : FAILED');
            expect(false).assertTrue();
            return;
        }

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        }
        else{
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            expect(false).assertTrue();
            return;
        }

        var numBuffersToCapture = 370;
        while (numBuffersToCapture) {
            var buffer = await audioCapturer.read(bufferSize, true);
            if (typeof(buffer) == undefined) {
                console.info('BufferRecLog: read buffer failed : '+buffer);
            } else {
                var number = fileio.writeSync(fd, buffer);
                console.info('BufferRecLog: data written: ' + number);
                numBuffersToCapture--;
            }
            if(numBuffersToCapture > 250 && numBuffersToCapture < 255) {
                await audioManagerRec.getAudioScene().then(async function (data) {
                    if (data == audio.AudioScene.AUDIO_SCENE_VOICE_CHAT) {
                        console.info('AudioFrameworkRenderLog: getAudioScene : PASS : ' + data);
                        resultFlag=true;
                    }
                    else {
                        console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : ' + data);
                        resultFlag=false;
                    }
                }).catch((err) => {
                    console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : ' + err.message);
                    resultFlag = false;
                });
            }
        }
        console.info('AudioFrameworkRecLog: Capturer after read');

        await audioCapturer.stop().then(async function (data) {
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS : '+data);
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            resultFlag=false;
        });

        await audioCapturer.release().then(async function (data) {
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS : '+data);
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
            resultFlag=false;
        });

        return resultFlag;

    }


    /* *
               * @tc.number    : SUB_AUDIO_VOIP_Play_001
               * @tc.name      : SetAudioScene during playback
               * @tc.desc      : SetAudioScene during playback
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_Play_001', 0, async function (done) {

        var audioParams = {
            format: audio.AudioSampleFormat.SAMPLE_S16LE,
            channels: audio.AudioChannel.STEREO,
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            encoding: audio.AudioEncodingType.ENCODING_PCM,
        };


        var resultFlag = await playbackPromise(audioParams, '/data/Believer60s.wav', audio.AudioVolumeType.VOICE_CALL);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_Rec_001
               * @tc.name      : SetAudioScene during Recording
               * @tc.desc      : SetAudioScene during Recording
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_Rec_001', 0, async function (done) {

        var audioParams = {
            format: audio.AudioSampleFormat.SAMPLE_S32LE,
            channels: audio.AudioChannel.STEREO,
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
            encoding: audio.AudioEncodingType.ENCODING_PCM,
            contentType: audio.ContentType.CONTENT_TYPE_SPEECH,
            usage: audio.StreamUsage.STREAM_USAGE_VOICE_COMMUNICATION,
            deviceRole: audio.DeviceRole.INPUT_DEVICE,
            deviceType: audio.DeviceType.MIC
        };
 
        
        var resultFlag = await recPromise(audioParams, dirPath+'/capture_js-48000-2C-32B.pcm', audio.AudioVolumeType.VOICE_CALL);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
 
        expect(resultFlag).assertTrue();
 
        done();
    })


   /* *
              * @tc.number    : SUB_AUDIO_VOIP_RecPlay_001
              * @tc.name      : Record & play at same time - setAudioScene
              * @tc.desc      : Record & play at same time - setAudioScene
              * @tc.size      : MEDIUM
              * @tc.type      : Function
              * @tc.level     : Level 0
          */
   it('SUB_AUDIO_VOIP_RecPlay_001', 0, async function (done) {

       var audioParamsRen = {
           format: audio.AudioSampleFormat.SAMPLE_S16LE,
           channels: audio.AudioChannel.STEREO,
           samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
           encoding: audio.AudioEncodingType.ENCODING_PCM,
       };

        var audioParamsRec = {
            format: audio.AudioSampleFormat.SAMPLE_S32LE,
            channels: audio.AudioChannel.STEREO,
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
            encoding: audio.AudioEncodingType.ENCODING_PCM,
            contentType: audio.ContentType.CONTENT_TYPE_SPEECH,
            usage: audio.StreamUsage.STREAM_USAGE_VOICE_COMMUNICATION,
            deviceRole: audio.DeviceRole.INPUT_DEVICE,
            deviceType: audio.DeviceType.MIC
        };

        var resultFlagRec = recPromise(audioParamsRec, dirPath+'/capture_js-48000-2C-32B-2.pcm', audio.AudioVolumeType.VOICE_CALL);

        var resultFlagRen = await playbackPromise(audioParamsRen, '/data/Believer60s.wav', audio.AudioVolumeType.VOICE_CALL);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : Renderer : '+resultFlagRec);
        console.info('AudioFrameworkRenderLog: resultFlag : Capturer : '+resultFlagRen);

        if (resultFlagRec = true){
            expect(resultFlagRen).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }

        done();
   })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_001
               * @tc.name      : setAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_001', 0, async function (done) {

        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_002
               * @tc.name      : getAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_002', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == audio.AudioScene.AUDIO_SCENE_VOICE_CHAT){
                console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                expect(false).assertTrue();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : '+err.message);
            resultFlag=false;
        });
        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_003
               * @tc.name      : getAudioScene as VOIP to Ringtone - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP to Ringtone - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_003', 0, async function (done) {
        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_RINGING).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            await audioManager.getAudioScene().then(async function (data) {
                if (data == audio.AudioScene.AUDIO_SCENE_RINGING){
                    console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                    expect(false).assertTrue();
                }
            }).catch((err) => {
                console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : '+err.message);
                resultFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_004
               * @tc.name      : setAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_004', 0, async function (done) {

        await audioManager.setAudioScene(3).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_005
               * @tc.name      : getAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_005', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == 3){
                console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                expect(true).assertTrue();
            }
            else{
                console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                expect(false).assertTrue();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : '+err.message);
            resultFlag=false;
        });
        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_006
               * @tc.name      : getAudioScene as VOIP to Ringtone - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP to Ringtone - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_006', 0, async function (done) {
        await audioManager.setAudioScene(1).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            await audioManager.getAudioScene().then(async function (data) {
                if (data == 1){
                    console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                    expect(false).assertTrue();
                }
            }).catch((err) => {
                console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : '+err.message);
                resultFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_007
               * @tc.name      : setAudioScene as VOIP - Callback - ENAME
               * @tc.desc      : setAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_007', 0, async function (done) {

        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT, (err) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
                expect(true).assertTrue();
            }
        });

        await sleep(100);
        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_VOIP_AudioScene_008
           * @tc.name      : getAudioScene as VOIP - Callback - ENAME
           * @tc.desc      : getAudioScene as VOIP - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_VOIP_AudioScene_008', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == audio.AudioScene.AUDIO_SCENE_VOICE_CHAT){
                    console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                    expect(false).assertTrue();
                }
            }
        });
        await sleep(100);

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_VOIP_AudioScene_009
           * @tc.name      : getAudioScene as VOIP to Ringtone - Callback - ENAME
           * @tc.desc      : getAudioScene as VOIP to Ringtone - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_VOIP_AudioScene_009', 0, async function (done) {
        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_RINGING,async (err) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                await sleep(100);
                console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
                audioManager.getAudioScene((err, data) => {
                    if (err) {
                        console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else {
                        if (data == audio.AudioScene.AUDIO_SCENE_RINGING){
                            console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                            expect(true).assertTrue();
                        }
                        else{
                            console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                            expect(false).assertTrue();
                        }
                    }
                });
                await sleep(100);
            }
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_VOIP_AudioScene_010
               * @tc.name      : setAudioScene as VOIP - Callback - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_VOIP_AudioScene_010', 0, async function (done) {

        audioManager.setAudioScene(3, (err) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
                expect(true).assertTrue();
            }
        });

        await sleep(100);
        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_VOIP_AudioScene_011
           * @tc.name      : getAudioScene as VOIP - Callback - ENUM
           * @tc.desc      : getAudioScene as VOIP - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_VOIP_AudioScene_011', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == 3){
                    console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                    expect(false).assertTrue();
                }
            }
        });
        await sleep(100);

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_VOIP_AudioScene_012
           * @tc.name      : getAudioScene as VOIP to Ringtone - Callback - ENUM
           * @tc.desc      : getAudioScene as VOIP to Ringtone - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_VOIP_AudioScene_012', 0, async function (done) {
        audioManager.setAudioScene(1,async (err) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else
            {
                await sleep(100);
                console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
                audioManager.getAudioScene((err, data) => {
                    if (err) {
                        console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                        expect(false).assertTrue();
                    }
                    else {
                        if (data == 1){
                            console.info('AudioFrameworkRenderLog: getAudioScene : PASS : '+data);
                            expect(true).assertTrue();
                        }
                        else{
                            console.info('AudioFrameworkRenderLog: getAudioScene : FAIL : '+data);
                            expect(false).assertTrue();
                        }
                    }
                });
                await sleep(100);
            }
        });
        done();
    })

})