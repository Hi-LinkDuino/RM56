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
import ability_featureAbility from '@ohos.ability.featureAbility';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioCall', function () {
    var mediaDir;
    var dateTime;
    var dirPath;
    var fpath;
    var resultFlagRec;
    var resultFlagRen;
    const audioManager = audio.getAudioManager();
    console.info('AudioFrameworkRenderLog: Create AudioManger Object JS Framework');

    const audioManagerRec = audio.getAudioManager();
    console.info('AudioFrameworkRecLog: Create AudioManger Object JS Framework');

    async function getPathName(){
        var path1 = '/data/accounts/account_0/appdata/';
        var packageName;
        var context = ability_featureAbility.getContext();
        await context.getBundleName()
            .then((data) => {
                console.info('AudioFrameworkRenderLog: Cache directory obtained. Data: ' + data);
                packageName = data;
            }).catch((error) => {
                console.error('AudioFrameworkRenderLog: Failed to obtain the cache directory. Cause:' + error.message);
            });
        await sleep(200);
        //var mediaDirTemp = path1 + packageName + '/' + packageName + '/assets/entry/resources/rawfile';
        var mediaDirTemp = path1+packageName
        console.info('AudioFrameworkRenderLog: Resource DIR Path : '+mediaDirTemp);
        return mediaDirTemp;

    }

    beforeAll(async function () {
        console.info('AudioFrameworkTest: beforeAll: Prerequisites at the test suite level');
        mediaDir = await getPathName();
        dateTime = new Date().getTime();
        dirPath = mediaDir+'/files/RecTest'+dateTime;
        console.info('AudioFrameworkRecLog: Recording files Path: '+dirPath);
        fpath = dirPath+'/capture_js.pcm';
        await sleep(100);
        fileio.mkdirSync(dirPath);
        console.info('AudioFrameworkTest: beforeAll: END');
    })

    beforeEach(async function () {
        console.info('AudioFrameworkTest: beforeEach: Prerequisites at the test case level');
        await sleep(500);
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

    async function playbackPromise (AudioRendererOptions, fpath, AudioScene) {
        resultFlagRen = 'new';
        console.info('AudioFrameworkRenderLog: Promise : Audio Playback Function');

        var audioRen;
        await audio.createAudioRenderer(AudioRendererOptions).then(async function (data) {
            audioRen = data;
            console.info('AudioFrameworkRenderLog: AudioRender Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: AudioRender Created : ERROR : '+err.message);
            resultFlagRen=false;
            return resultFlagRen;
        });

        console.info('AudioFrameworkRenderLog: AudioRenderer : Path : '+fpath);

        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        await audioManager.setAudioScene(AudioScene).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            await audioManager.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRenderLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRenderLog: getAudioScene : ERROR : '+err.message);
                resultFlagRen=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            resultFlagRen=false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
        }
        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        await audioRen.getStreamInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRenderLog: Renderer getStreamInfo:');
            console.info('AudioFrameworkRenderLog: Renderer sampleFormat:' + audioParamsGet.sampleFormat);
            console.info('AudioFrameworkRenderLog: Renderer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRenderLog: Renderer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRenderLog: Renderer encodingType:' + audioParamsGet.encodingType);
        }).catch((err) => {
            console.log('AudioFrameworkRenderLog: getStreamInfo :ERROR: '+err.message);
            resultFlagRen = false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
        }

        await audioRen.getRendererInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRenderLog: Renderer RendererInfo:');
            console.info('AudioFrameworkRenderLog: Renderer content type:' + audioParamsGet.content);
            console.info('AudioFrameworkRenderLog: Renderer usage:' + audioParamsGet.usage);
            console.info('AudioFrameworkRenderLog: Renderer rendererFlags:' + audioParamsGet.rendererFlags);
        }).catch((err) => {
            console.log('AudioFrameworkRenderLog: RendererInfo :ERROR: '+err.message);
            resultFlagRen = false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
        }

        await audioRen.start().then(async function () {
            console.info('AudioFrameworkRenderLog: renderInstant started :SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: renderInstant start :ERROR : '+err.message);
            resultFlagRen=false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
        }

        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        var bufferSize;
        await audioRen.getBufferSize().then(async function (data) {
            console.info('AudioFrameworkRenderLog: getBufferSize :SUCCESS '+data);
            bufferSize=data;
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: getBufferSize :ERROR : '+err.message);
            resultFlagRen=false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
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
        while (rlen < totalSize/2) {
            let buf = new ArrayBuffer(bufferSize);
            rlen += ss.readSync(buf);
            console.info('BufferAudioFramework: bytes read from file: ' +rlen);
            await audioRen.write(buf);
            if (rlen > (totalSize/2)){
                await audioManager.getAudioScene().then(async function (data) {
                    console.info('AudioFrameworkAudioScene: getAudioScene : Value : '+data);
                }).catch((err) => {
                    console.info('AudioFrameworkAudioScene: getAudioScene : ERROR : '+err.message);
                    resultFlagRen=false;
                });
            }
        }
        console.info('AudioFrameworkRenderLog: Renderer after read');

        await audioRen.drain().then(async function () {
            console.info('AudioFrameworkRenderLog: Renderer drained : SUCCESS');
        }).catch((err) => {
            console.error('AudioFrameworkRenderLog: Renderer drain: ERROR : '+err.message);
            resultFlagRen=false;
        });
        if (resultFlagRen == false){
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
            return resultFlagRen;
        }

        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        await audioRen.stop().then(async function () {
            console.info('AudioFrameworkRenderLog: Renderer stopped : SUCCESS');
            resultFlagRen=true;
            console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: Renderer stop:ERROR : '+err.message);
            resultFlagRen=false;
        });

        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        await audioRen.release().then(async function () {
            console.info('AudioFrameworkRenderLog: Renderer release : SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: Renderer release :ERROR : '+err.message);
            resultFlagRen=false;
        });

        console.info('AudioFrameworkRenderLog: AudioRenderer : STATE : '+audioRen.state);

        console.info('AudioFrameworkRenderLog: resultFlagRen : '+resultFlagRen);

        return resultFlagRen;
    }

    async function recPromise (AudioCapturerOptions, fpath, AudioScene) {

        resultFlagRec = 'new';
        console.info('AudioFrameworkRecLog: Promise : Audio Recording Function');

        var audioCap;

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCap = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return resultFlagRec;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : Path : '+fpath);

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioManagerRec.setAudioScene(AudioScene).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                resultFlagRec=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            resultFlagRec=false;
        });
        if (resultFlagRec == false){
            console.info('AudioFrameworkRecLog: resultFlagRec : '+resultFlagRec);
            return resultFlagRec;
        }

        await audioCap.getStreamInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
            console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
            console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
            resultFlagRec = false;
        });
        if (resultFlagRec == false){
            console.info('AudioFrameworkRecLog: resultFlagRec : '+resultFlagRec);
            return resultFlagRec;
        }

        await audioCap.getCapturerInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
            console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
            console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
            resultFlagRec = false;
        });
        if (resultFlagRec == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlagRec);
            return resultFlagRec;
        }

        await audioCap.start().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            resultFlagRec=false;
        });
        if (resultFlagRec == false){
            console.info('AudioFrameworkRecLog: resultFlagRec : '+resultFlagRec);
            return resultFlagRec;
        }

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        var bufferSize = await audioCap.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);

        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }
        else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            resultFlagRec=false;
            return resultFlagRec;
        }

        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        }
        else{
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            resultFlagRec=false;
            return resultFlagRec;
        }

        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            var buffer = await audioCap.read(bufferSize, true);
            var number = fileio.writeSync(fd, buffer);
            console.info('BufferRecLog: data written: ' + number);
            numBuffersToCapture--;
        }

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioCap.stop().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            resultFlagRec=true;
            console.info('AudioFrameworkRecLog: resultFlagRec : '+resultFlagRec);
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            resultFlagRec=false;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioCap.release().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
            resultFlagRec=false;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        return resultFlagRec;

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

        var AudioStreamInfo = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var AudioRendererInfo = {
            content: audio.ContentType.CONTENT_TYPE_SPEECH,
            usage: audio.StreamUsage.STREAM_USAGE_VOICE_COMMUNICATION,
            rendererFlags: 1
        }

        var AudioRendererOptions = {
            streamInfo: AudioStreamInfo,
            rendererInfo: AudioRendererInfo
        }

        var resultFlag = await playbackPromise(AudioRendererOptions, mediaDir+'/files/StarWars10s-1C-44100-2SW.wav', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
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

        var AudioStreamInfo = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var AudioCapturerInfo = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags: 1
        }

        var AudioCapturerOptions = {
            streamInfo: AudioStreamInfo,
            capturerInfo: AudioCapturerInfo
        }

        var resultFlag = await recPromise(AudioCapturerOptions, dirPath+'/capture_js-44100-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
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

       var AudioStreamInfoCap = {
           samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
           channels: audio.AudioChannel.CHANNEL_2,
           sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
           encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
       }

       var AudioCapturerInfo = {
           source: audio.SourceType.SOURCE_TYPE_MIC,
           capturerFlags: 1
       }

       var AudioCapturerOptions = {
           streamInfo: AudioStreamInfoCap,
           capturerInfo: AudioCapturerInfo
       }

       var AudioStreamInfoRen = {
           samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
           channels: audio.AudioChannel.CHANNEL_1,
           sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
           encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
       }

       var AudioRendererInfo = {
           content: audio.ContentType.CONTENT_TYPE_SPEECH,
           usage: audio.StreamUsage.STREAM_USAGE_VOICE_COMMUNICATION,
           rendererFlags: 1
       }

       var AudioRendererOptions = {
           streamInfo: AudioStreamInfoRen,
           rendererInfo: AudioRendererInfo
       }
       recPromise(AudioCapturerOptions, dirPath+'/capture_js-44100-2C-16B-2.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
       await sleep(500);
       await playbackPromise(AudioRendererOptions, mediaDir+'/files/StarWars10s-1C-44100-2SW.wav', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
       await sleep(1000);
       console.info('AudioFrameworkRecLog: resultFlag : Capturer : '+resultFlagRec);
       console.info('AudioFrameworkRenderLog: resultFlag : Renderer : '+resultFlagRen);

        if (resultFlagRec == true){
            expect(resultFlagRen).assertTrue();
        }
        else{
            expect(false).assertTrue();
        }

        done();
   })

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_001
               * @tc.name      : setAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_001', 0, async function (done) {

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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_002
               * @tc.name      : getAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_002', 0, async function (done) {

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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_003
               * @tc.name      : getAudioScene as VOIP to Ringtone - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP to Ringtone - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_003', 0, async function (done) {
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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_004
               * @tc.name      : setAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_004', 0, async function (done) {

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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_005
               * @tc.name      : getAudioScene as VOIP - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_005', 0, async function (done) {

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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_006
               * @tc.name      : getAudioScene as VOIP to Ringtone - Promise - ENUM
               * @tc.desc      : getAudioScene as VOIP to Ringtone - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_006', 0, async function (done) {
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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_007
               * @tc.name      : setAudioScene as VOIP - Callback - ENAME
               * @tc.desc      : setAudioScene as VOIP - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_007', 0, async function (done) {

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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_008
           * @tc.name      : getAudioScene as VOIP - Callback - ENAME
           * @tc.desc      : getAudioScene as VOIP - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_008', 0, async function (done) {

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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_009
           * @tc.name      : getAudioScene as VOIP to Ringtone - Callback - ENAME
           * @tc.desc      : getAudioScene as VOIP to Ringtone - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_009', 0, async function (done) {
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
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_010
               * @tc.name      : setAudioScene as VOIP - Callback - ENUM
               * @tc.desc      : setAudioScene as VOIP - ENUM
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_010', 0, async function (done) {

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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_011
           * @tc.name      : getAudioScene as VOIP - Callback - ENUM
           * @tc.desc      : getAudioScene as VOIP - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_011', 0, async function (done) {

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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_012
           * @tc.name      : getAudioScene as VOIP to Ringtone - Callback - ENUM
           * @tc.desc      : getAudioScene as VOIP to Ringtone - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_012', 0, async function (done) {
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

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_013
               * @tc.name      : setAudioScene as VOIP - Promise - Out Of Range
               * @tc.desc      : setAudioScene as VOIP - Out Of Range
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_013', 0, async function (done) {

        await audioManager.setAudioScene(10).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(false).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(true).assertTrue();
        });

        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_014
               * @tc.name      : setAudioScene as VOIP - Callback - Out Of Range
               * @tc.desc      : setAudioScene as VOIP - Out Of Range
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_014', 0, async function (done) {

        audioManager.setAudioScene(-1, (err) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(true).assertTrue();
            }
            else
            {
                console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
                expect(false).assertTrue();
            }
        });

        await sleep(100);
        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_015
               * @tc.name      : setAudioScene as AUDIO_SCENE_RINGING - Promise - ENUM
               * @tc.desc      : setAudioScene as AUDIO_SCENE_RINGING - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_015', 0, async function (done) {

        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_RINGING).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_016
           * @tc.name      : getAudioScene as AUDIO_SCENE_RINGING - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_RINGING - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_016', 0, async function (done) {

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
        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_017
           * @tc.name      : getAudioScene as Ringtone to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as Ringtone to VOIP - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_017', 0, async function (done) {
        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_018
           * @tc.name      : setAudioScene as AUDIO_SCENE_RINGING - Promise - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_RINGING - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_018', 0, async function (done) {

        await audioManager.setAudioScene(1).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_019
           * @tc.name      : getAudioScene as AUDIO_SCENE_RINGING - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_RINGING - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_019', 0, async function (done) {

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
        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_020
           * @tc.name      : getAudioScene as Ringtone to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as Ringtone to VOIP - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_020', 0, async function (done) {
        await audioManager.setAudioScene(3).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_021
           * @tc.name      : setAudioScene as AUDIO_SCENE_RINGING - Callback - ENAME
           * @tc.desc      : setAudioScene as AUDIO_SCENE_RINGING - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_021', 0, async function (done) {

        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_RINGING, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_022
       * @tc.name      : getAudioScene as AUDIO_SCENE_RINGING - Callback - ENAME
       * @tc.desc      : getAudioScene as AUDIO_SCENE_RINGING - ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_022', 0, async function (done) {

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

        done();
    })

    /* *
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_023
       * @tc.name      : getAudioScene as Ringtone to VOIP - Callback - ENAME
       * @tc.desc      : getAudioScene as Ringtone to VOIP- ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_023', 0, async function (done) {
        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async (err) => {
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
            }
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_024
           * @tc.name      : setAudioScene as AUDIO_SCENE_RINGING - Callback - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_RINGING - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_024', 0, async function (done) {

        audioManager.setAudioScene(1, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_025
       * @tc.name      : getAudioScene as AUDIO_SCENE_RINGING - Callback - ENUM
       * @tc.desc      : getAudioScene as AUDIO_SCENE_RINGING - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_025', 0, async function (done) {

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

        done();
    })

    /* *
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_026
       * @tc.name      : getAudioScene as Ringtone to VOIP - Callback - ENUM
       * @tc.desc      : getAudioScene as Ringtone to VOIP - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_026', 0, async function (done) {
        audioManager.setAudioScene(3,async (err) => {
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
            }
        });
        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_027
               * @tc.name      : setAudioScene as AUDIO_SCENE_PHONE_CALL - Promise - ENUM
               * @tc.desc      : setAudioScene as AUDIO_SCENE_PHONE_CALL - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_027', 0, async function (done) {

        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_PHONE_CALL).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_028
           * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_028', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == audio.AudioScene.AUDIO_SCENE_PHONE_CALL){
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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_029
           * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_029', 0, async function (done) {
        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_030
           * @tc.name      : setAudioScene as AUDIO_SCENE_PHONE_CALL - Promise - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_PHONE_CALL - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_030', 0, async function (done) {

        await audioManager.setAudioScene(2).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_031
           * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_031', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == 2){
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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_032
           * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_032', 0, async function (done) {
        await audioManager.setAudioScene(3).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_033
           * @tc.name      : setAudioScene as AUDIO_SCENE_PHONE_CALL - Callback - ENAME
           * @tc.desc      : setAudioScene as AUDIO_SCENE_PHONE_CALL - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_033', 0, async function (done) {

        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_PHONE_CALL, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_034
       * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL - Callback - ENAME
       * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL - ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_034', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == audio.AudioScene.AUDIO_SCENE_PHONE_CALL){
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_035
       * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP - Callback - ENAME
       * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL to VOIP- ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_035', 0, async function (done) {
        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async (err) => {
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
            }
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_036
           * @tc.name      : setAudioScene as AUDIO_SCENE_PHONE_CALL - Callback - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_PHONE_CALL - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_036', 0, async function (done) {

        audioManager.setAudioScene(2, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_037
       * @tc.name      : getAudioScene as AUDIO_SCENE_PHONE_CALL - Callback - ENUM
       * @tc.desc      : getAudioScene as AUDIO_SCENE_PHONE_CALL - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_037', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == 2){
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_038
       * @tc.name      : getAudioScene as Ringtone to VOIP - Callback - ENUM
       * @tc.desc      : getAudioScene as Ringtone to VOIP - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
   */
    it('SUB_AUDIO_CALL_AudioScene_038', 0, async function (done) {
        audioManager.setAudioScene(3,async (err) => {
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
            }
        });
        done();
    })

    /* *
               * @tc.number    : SUB_AUDIO_CALL_AudioScene_039
               * @tc.name      : setAudioScene as AUDIO_SCENE_DEFAULT - Promise - ENUM
               * @tc.desc      : setAudioScene as AUDIO_SCENE_DEFAULT - ENAME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
           */
    it('SUB_AUDIO_CALL_AudioScene_039', 0, async function (done) {

        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_DEFAULT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_040
           * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_040', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == audio.AudioScene.AUDIO_SCENE_DEFAULT){
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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_041
           * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_041', 0, async function (done) {
        await audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_042
           * @tc.name      : setAudioScene as AUDIO_SCENE_DEFAULT - Promise - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_DEFAULT - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_042', 0, async function (done) {

        await audioManager.setAudioScene(0).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
            expect(true).assertTrue();
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_043
           * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_043', 0, async function (done) {

        await audioManager.getAudioScene().then(async function (data) {
            if (data == 0){
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
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_044
           * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP - Promise - ENUM
           * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_044', 0, async function (done) {
        await audioManager.setAudioScene(3).then(async function () {
            console.info('AudioFrameworkRenderLog: setAudioScene : SUCCESS ');
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
        }).catch((err) => {
            console.info('AudioFrameworkRenderLog: setAudioScene : ERROR : '+err.message);
            expect(false).assertTrue();
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_045
           * @tc.name      : setAudioScene as AUDIO_SCENE_DEFAULT - Callback - ENAME
           * @tc.desc      : setAudioScene as AUDIO_SCENE_DEFAULT - ENAME
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_045', 0, async function (done) {

        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_DEFAULT, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_046
       * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT - Callback - ENAME
       * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT - ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
    */
    it('SUB_AUDIO_CALL_AudioScene_046', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == audio.AudioScene.AUDIO_SCENE_DEFAULT){
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_047
       * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP - Callback - ENAME
       * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT to VOIP- ENAME
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
    */
    it('SUB_AUDIO_CALL_AudioScene_047', 0, async function (done) {
        audioManager.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async (err) => {
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
            }
        });

        done();
    })

    /* *
           * @tc.number    : SUB_AUDIO_CALL_AudioScene_048
           * @tc.name      : setAudioScene as AUDIO_SCENE_DEFAULT - Callback - ENUM
           * @tc.desc      : setAudioScene as AUDIO_SCENE_DEFAULT - ENUM
           * @tc.size      : MEDIUM
           * @tc.type      : Function
           * @tc.level     : Level 0
       */
    it('SUB_AUDIO_CALL_AudioScene_048', 0, async function (done) {

        audioManager.setAudioScene(0, (err) => {
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_049
       * @tc.name      : getAudioScene as AUDIO_SCENE_DEFAULT - Callback - ENUM
       * @tc.desc      : getAudioScene as AUDIO_SCENE_DEFAULT - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
    */
    it('SUB_AUDIO_CALL_AudioScene_049', 0, async function (done) {

        audioManager.getAudioScene((err, data) => {
            if (err) {
                console.error(`AudioFrameworkRenderLog: setAudioScene : ERROR : ${err.message}`);
                expect(false).assertTrue();
            }
            else {
                if (data == 0){
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
       * @tc.number    : SUB_AUDIO_CALL_AudioScene_050
       * @tc.name      : getAudioScene as Ringtone to VOIP - Callback - ENUM
       * @tc.desc      : getAudioScene as Ringtone to VOIP - ENUM
       * @tc.size      : MEDIUM
       * @tc.type      : Function
       * @tc.level     : Level 0
    */
    it('SUB_AUDIO_CALL_AudioScene_050', 0, async function (done) {
        audioManager.setAudioScene(3,async (err) => {
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
            }
        });
        done();
    })



})