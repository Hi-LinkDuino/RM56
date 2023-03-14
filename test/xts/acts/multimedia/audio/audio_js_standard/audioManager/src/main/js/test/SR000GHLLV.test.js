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

describe('audioCapturer', function () {


    var dateTime = new Date().getTime();
    //const dirPath = '/data/accounts/account_0/appdata/RecTest'+dateTime;
    //var fpath = dirPath+'/capture_js.pcm';
    var audioCapCallBack;
    var audioCapPromise;
    var dirPath;
    var mediaDir;
    var fpath;
    //console.info('AudioFrameworkRecLog: Recording files Path: '+dirPath);

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

    const audioManager = audio.getAudioManager();
    console.info('AudioFrameworkRenderLog: Create AudioManger Object JS Framework');

    const audioManagerRec = audio.getAudioManager();
    console.info('AudioFrameworkRecLog: Create AudioManger Object JS Framework');

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
        await sleep(1000);
    })

    afterEach(function () {
        console.info('AudioFrameworkTest: afterEach: Test case-level clearance conditions');
    })

    afterAll(async function () {
        fileio.rmdir(dirPath, function(err){
            if (!err) {
                console.info('AudioFrameworkTest: Recorded files are deleted successfully');
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkTest: afterAll: Test suite-level cleanup condition');
    })

    function sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }


    async function recPromise (AudioCapturerOptions, fpath, AudioScene) {

        var resultFlag = 'new';
        console.info('AudioFrameworkRecLog: Promise : Audio Recording Function');

        var audioCap;

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCap = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return resultFlag;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : Path : '+fpath);

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioManagerRec.setAudioScene(AudioScene).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                resultFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioCap.getStreamInfo().then(async function (audioParamsGet) {
            if (audioParamsGet != undefined) {
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
                console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
                console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
                console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
                console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
            } else {
                console.info('AudioFrameworkRecLog: audioParamsGet is : '+audioParamsGet);
                console.info('AudioFrameworkRecLog: audioParams getStreamInfo are incorrect: ');
                resultFlag = false;
            }
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: getStreamInfo  :ERROR: '+err.message);
            resultFlag = false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioCap.getCapturerInfo().then(async function (audioParamsGet) {
            if (audioParamsGet != undefined) {
                console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
                console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
                console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
            }else {
                console.info('AudioFrameworkRecLog: audioParamsGet is : '+audioParamsGet);
                console.info('AudioFrameworkRecLog: audioParams getCapturerInfo are incorrect: ');
                resultFlag = false;
            }
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
            resultFlag = false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        await audioCap.start().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            resultFlag=false;
        });
        if (resultFlag == false){
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            return resultFlag;
        }

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        var bufferSize = await audioCap.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);

        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }
        else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            resultFlag=false;
            return resultFlag;
        }

        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        }
        else{
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            resultFlag=false;
            return resultFlag;
        }
        await sleep(100);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------READ BUFFER---------');
            var buffer = await audioCap.read(bufferSize, true);
            await sleep(50);
            console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
            var number = fileio.writeSync(fd, buffer);
            console.info('BufferRecLog: data written: ' + number);
            await sleep(50);
            numBuffersToCapture--;
        }
        await sleep(1000);
        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioCap.stop().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            resultFlag=true;
            console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            resultFlag=false;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        await audioCap.release().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
            resultFlag=false;
        });

        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCap.state);

        return resultFlag;

    }


    async function recCallBack (AudioCapturerOptions, fpath, AudioScene) {

        var resultFlag = true;
        console.info('AudioFrameworkRecLog: CallBack : Audio Recording Function');
        //var audioCap;

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioManagerRec.getAudioScene(async(err,getValue) => {
            console.info('AudioFrameworkRecLog: ---------GET AUDIO SCENE---------');
            if(err){
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                resultFlag=false;
            }else {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+getValue);
            }
        });
        await sleep(1000);
        await audioManagerRec.setAudioScene(AudioScene,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                resultFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioCapCallBack.getStreamInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET STREAM INFO---------');
            console.log('AudioFrameworkRecLog: Entered getStreamInfo');
            if (err){
                console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
                resultFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
                console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
                console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
                console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
                console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
            }
        });
        await sleep(1000);
        audioCapCallBack.getCapturerInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET CAPTURER INFO---------');
            if (err){
                console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
                resultFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
                console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
                console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                resultFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR ');
            resultFlag=false;
            return resultFlag;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            resultFlag=false;
            return resultFlag;
        }

        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------READ BUFFER---------');
            var buffer = await audioCapCallBack.read(bufferSize, true);
            await sleep(50);
            console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
            var number = fileio.writeSync(fd, buffer);
            console.info('BufferRecLog: data written: ' + number);
            await sleep(50);
            numBuffersToCapture--;
        }
        //await sleep(3000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                resultFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                resultFlag=true;
                console.info('AudioFrameworkRecLog: resultFlag : '+resultFlag);
            }
        });
        await sleep(1000);

        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                resultFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                resultFlag=true;
                console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
                //return resultFlag;
            }
        });
        //await sleep(3000);
        console.info('AudioFrameworkRenderLog: After all check resultFlag : '+resultFlag);
        return resultFlag;
    }

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_PREPARED_STATE_001
               * @tc.name      : AudioCapturer-Check-STATE-PREPARED
               * @tc.desc      : AudioCapturer with state prepared
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_PREPARED_STATE_001', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO PREPARED STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == audio.AudioState.STATE_PREPARED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO PREPARED STATE : PASS---------');
                    stateFlag = true;
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });

    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_PREPARED_STATE_ENUM_002
               * @tc.name      : AudioCapturer-Check-STATE-PREPARED-ENUM
               * @tc.desc      : AudioCapturer with state prepared
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_PREPARED_STATE_ENUM_002', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO PREPARED STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO PREPARED STATE : PASS---------');
                    stateFlag = true;
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });

    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_RUNNING_STATE_003
               * @tc.name      : AudioCapturer-Check-STATE-RUNNING
               * @tc.desc      : AudioCapturer with state running
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_RUNNING_STATE_003', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RUNNING STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == audio.AudioState.STATE_RUNNING)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RUNNING STATE : PASS---------');
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        await audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
                //return resultFlag;
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_RUNNING_STATE_EUNM_004
               * @tc.name      : AudioCapturer-Check-STATE-RUNNING-ENUM
               * @tc.desc      : AudioCapturer with state running
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_RUNNING_STATE_EUNM_004', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RUNNING STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RUNNING STATE : PASS---------');
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        await audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
                //return resultFlag;
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_STOPPED_STATE_005
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_STOPPED_STATE_005', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO STOPPED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == audio.AudioState.STATE_STOPPED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO STOPPED STATE---------');
                    stateFlag = true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
                //return resultFlag;
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_STOPPED_STATE_ENUM_006
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED-ENUM
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_STOPPED_STATE_ENUM_006', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                stateFlag = true;
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                stateFlag == true;
            }
        });
        await sleep(1000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO STOPPED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 3)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO STOPPED STATE---------');
                    stateFlag = true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
                //return resultFlag;
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_RELEASED_STATE_007
               * @tc.name      : AudioCapturer-Check-STATE-RELEASED
               * @tc.desc      : AudioCapturer with state released
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_RELEASED_STATE_007', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 3)) {
                    stateFlag = true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == audio.AudioState.STATE_RELEASED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_RELEASED_STATE_ENUM_008
               * @tc.name      : AudioCapturer-Check-STATE-RELEASED
               * @tc.desc      : AudioCapturer with state released
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_RELEASED_STATE_ENUM_008', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 3)) {
                    stateFlag = true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_GET_BUFFER_SIZE_009
               * @tc.name      : AudioCapturer-get_buffer_size
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_GET_BUFFER_SIZE_009', 0, async function (done) {
        var stateFlag ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize(async(err,cbbufferSize) => {
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK BUFFER SIZE---------');
                console.info('AudioFrameworkRecLog: buffer size: ' + cbbufferSize);
                stateFlag = true
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                console.info('AudioFrameworkRecLog: ---------AFTER CHECK BUFFER SIZE : PASS---------')
            }
        });
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
                //return resultFlag;
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_010
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_010', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMOISE_PREPARED_STATE_011
               * @tc.name      : AudioCapturer-Check-STATE-PREPARED
               * @tc.desc      : AudioCapturer with state prepared
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMOISE_PREPARED_STATE_011', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO PREPARED STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapPromise.state == audio.AudioState.STATE_PREPARED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO PREPARED STATE : PASS---------');
                    stateFlag = true;
                    expect(stateFlag).assertTrue();
                    done();
                }
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });

    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMOISE_PREPARED_STATE_ENUM_012
               * @tc.name      : AudioCapturer-Check-STATE-PREPARED-ENUM
               * @tc.desc      : AudioCapturer with state prepared
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMOISE_PREPARED_STATE_ENUM_012', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO PREPARED STATE---------');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapPromise.state == 1)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO PREPARED STATE : PASS---------');
                    stateFlag = true;
                    expect(stateFlag).assertTrue();
                    done();
                }
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });

    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_RUNNING_STATE_013
               * @tc.name      : AudioCapturer-Check-STATE-RUNNING
               * @tc.desc      : AudioCapturer with state running
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_RUNNING_STATE_013', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == audio.AudioState.STATE_RUNNING)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            stateFlag=true;
            console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
            expect(stateFlag).assertTrue();
            done();
            //return resultFlag;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_RUNNING_STATE_ENUM_014
               * @tc.name      : AudioCapturer-Check-STATE-RUNNING-ENUM
               * @tc.desc      : AudioCapturer with state running
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_RUNNING_STATE_ENUM_014', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == 2)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            stateFlag=true;
            console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
            expect(stateFlag).assertTrue();
            done();
            //return resultFlag;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_STOPPED_STATE_015
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_STOPPED_STATE_015', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == audio.AudioState.STATE_STOPPED)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.stop().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            if ((audioCapPromise.state == audioCapPromise.AudioState.STATE_STOPPED)){
                stateFlag=true;
                console.info('AudioFrameworkRecLog: resultFlag : '+stateFlag);
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            stateFlag=true;
            console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
            expect(stateFlag).assertTrue();
            done();
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_STOPPED_STATE_ENUM_016
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_STOPPED_STATE_ENUM_016', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == audio.AudioState.STATE_STOPPED)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.stop().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            if ((audioCapPromise.state == 3)){
                stateFlag=true;
                console.info('AudioFrameworkRecLog: resultFlag : '+stateFlag);
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            stateFlag=true;
            console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
            expect(stateFlag).assertTrue();
            done();
            //return resultFlag;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_RELEASED_STATE_017
               * @tc.name      : AudioCapturer-Check-STATE-RELEASED
               * @tc.desc      : AudioCapturer with state released
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_RELEASED_STATE_017', 0, async function (done) {
        var stateFlag;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : ' + err.message);
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : ' + data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : ' + err.message);
                stateFlag = false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : ' + err.message);
            stateFlag = false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            stateFlag = true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ' + err.message);
            stateFlag = false;
        });
        await sleep(1000);

        await audioCapPromise.stop().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            stateFlag = true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : ' + err.message);
            stateFlag = false;
        });
        await sleep(1000);
        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
            if ((audioCapPromise.state == audio.AudioState.STATE_RELEASED)) {
                console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
                await sleep(1000);
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : ' + err.message);
            stateFlag = false;
        });
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_RELEASED_STATE_ENUM_018
               * @tc.name      : AudioCapturer-Check-STATE-RELEASED-ENUM
               * @tc.desc      : AudioCapturer with state released
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_RELEASED_STATE_ENUM_018', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });


        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            stateFlag = true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.stop().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
            stateFlag = true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        await audioCapPromise.release().then(async function (){
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
            if ((audioCapPromise.state == 4)) {
                console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_GET_BUFFER_SIZE_019
               * @tc.name      : AudioCapturer-get_buffer_size
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_GET_BUFFER_SIZE_019', 0, async function (done) {
        var stateFlag ;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });
        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == 2)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapPromise.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        stateFlag = true;
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);

        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
            if ((audioCapPromise.state == 4)) {
                console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_READ_BUFFER_020
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_READ_BUFFER_020', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkpromisereadbuffer.pcm';
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            return stateFlag;
        });
        await sleep(1000);

        await audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT).then(async function () {
            console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
            await audioManagerRec.getAudioScene().then(async function (data) {
                console.info('AudioFrameworkRecLog: getAudioScene : Value : '+data);
            }).catch((err) => {
                console.info('AudioFrameworkRecLog: getAudioScene : ERROR : '+err.message);
                stateFlag=false;
            });
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        await audioCapPromise.start().then(async function () {
            console.info('AudioFrameworkRecLog: ---------START---------');
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapPromise.state);
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
            if ((audioCapPromise.state == 2)) {
                stateFlag = true;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);

        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapPromise.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        stateFlag = true;

        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        //await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE PROMISE READ ---------');
            console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
            var buffer = await audioCapPromise.read(bufferSize, true);
            console.info('AudioFrameworkRecLog: ---------AFTER PROMISE READ ---------');
            //await sleep(50);
            var number = fileio.writeSync(fd, buffer);
            console.info('BufferRecLog: data written: ' + number);
            console.info('AudioFrameworkRecLog: ---------AFTER PROMISE WRITE ---------');
            //await sleep(100);
            numBuffersToCapture--;
        }
        //await sleep(3000);

        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
            console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
            console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
            if ((audioCapPromise.state == 4)) {
                console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_CB_021
               * @tc.name      : AudioCapturer-Set1-Media
               * @tc.desc      : AudioCapturer with parameter set 1
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_CB_021', 0, async function (done) {

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

        var resultFlag = await recCallBack(AudioCapturerOptions, dirPath+'/capture_CB_js-44100-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(1000);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
        expect(resultFlag).assertTrue();
        done();
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_CB_ENUM_022
               * @tc.name      : AudioCapturer-Set1-Media
               * @tc.desc      : AudioCapturer with parameter set 1
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_CB_ENUM_022', 0, async function (done) {

        var AudioStreamInfo = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0
        }

        var AudioCapturerInfo = {
            source: 1,
            capturerFlags: 1
        }

        var AudioCapturerOptions = {
            streamInfo: AudioStreamInfo,
            capturerInfo: AudioCapturerInfo
        }

        var resultFlag = await recCallBack(AudioCapturerOptions, dirPath+'/capture_CB_js-44100-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(1000);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);
        expect(resultFlag).assertTrue();
        done();
    })

    /*                   *
               * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_023
               * @tc.name      : AudioCapturer-Set1-Media
               * @tc.desc      : AudioCapturer with parameter set 1
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
               * */


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_023', 0, async function (done) {

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

    /*                   *
               * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_024
               * @tc.name      : AudioCapturer-Set1-Media
               * @tc.desc      : AudioCapturer with parameter set 1
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0
               * */


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_024', 0, async function (done) {

        var AudioStreamInfo = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0
        }

        var AudioCapturerInfo = {
            source: 1,
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

    /*               *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_025
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_025', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions44100 = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        var resultFlag = await recPromise(audioCapturerOptions44100, dirPath+'/capture_js-44100-1C-16LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*               *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_026
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_026', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions44100 = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        var resultFlag = await recPromise(audioCapturerOptions44100, dirPath+'/capture_js-44100-1C-16LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_027
                * @tc.name      : AudioRec-Set2
                * @tc.desc      : record audio with parameter set 2
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_027', 0, async function (done) {
        var audioStreamInfo96000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_96000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S24LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo96000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions96000 = {
            streamInfo: audioStreamInfo96000,
            capturerInfo: audioCapturerInfo96000,
        }

        var resultFlag = await recPromise(audioCapturerOptions96000, dirPath+'/capture_js-96000-1C-S24LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_028
                * @tc.name      : AudioRec-Set2
                * @tc.desc      : record audio with parameter set 2
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_028', 0, async function (done) {
        var audioStreamInfo96000 = {
            samplingRate: 96000,
            channels: 1,
            sampleFormat: 2,
            encodingType: 0,
        };
        var audioCapturerInfo96000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions96000 = {
            streamInfo: audioStreamInfo96000,
            capturerInfo: audioCapturerInfo96000,
        }

        var resultFlag = await recPromise(audioCapturerOptions96000, dirPath+'/capture_js-96000-1C-S24LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_029
                * @tc.name      : AudioRec-Set3
                * @tc.desc      : record audio with parameter set 3
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_029', 0, async function (done) {
        var audioStreamInfo48000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S32LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo48000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions48000 = {
            streamInfo: audioStreamInfo48000,
            capturerInfo: audioCapturerInfo48000,
        }

        var resultFlag = await recPromise(audioCapturerOptions48000, dirPath+'/capture_js-48000-2C-1S32LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_30
                * @tc.name      : AudioRec-Set3
                * @tc.desc      : record audio with parameter set 3
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_30', 0, async function (done) {
        var audioStreamInfo48000 = {
            samplingRate: 48000,
            channels: 2,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo48000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions48000 = {
            streamInfo: audioStreamInfo48000,
            capturerInfo: audioCapturerInfo48000,
        }

        var resultFlag = await recPromise(audioCapturerOptions48000, dirPath+'/capture_js-48000-2C-1S32LE.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_031
                * @tc.name      : AudioRec-Set4
                * @tc.desc      : record audio with parameter set 4
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_031', 0, async function (done) {
        var audioStreamInfo8000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_8000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_U8,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo8000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions8000 = {
            streamInfo: audioStreamInfo8000,
            capturerInfo: audioCapturerInfo8000,
        }

        var resultFlag = await recPromise(audioCapturerOptions8000, dirPath+'/capture_js-8000-1C-8B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_032
                * @tc.name      : AudioRec-Set4
                * @tc.desc      : record audio with parameter set 4
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_032', 0, async function (done) {
        var audioStreamInfo8000 = {
            samplingRate: 8000,
            channels: 1,
            sampleFormat: 0,
            encodingType: 0,
        };
        var audioCapturerInfo8000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions8000 = {
            streamInfo: audioStreamInfo8000,
            capturerInfo: audioCapturerInfo8000,
        }

        var resultFlag = await recPromise(audioCapturerOptions8000, dirPath+'/capture_js-8000-1C-8B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_33
                * @tc.name      : AudioRec-Set5
                * @tc.desc      : record audio with parameter set 5
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_33', 0, async function (done) {
        var audioStreamInfo11025 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_11025,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo11025 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions11025 = {
            streamInfo: audioStreamInfo11025,
            capturerInfo: audioCapturerInfo11025,
        }

        var resultFlag = await recPromise(audioCapturerOptions11025, dirPath+'/capture_js-11025-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_034
                * @tc.name      : AudioRec-Set5
                * @tc.desc      : record audio with parameter set 5
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_034', 0, async function (done) {
        var audioStreamInfo11025 = {
            samplingRate: 11025,
            channels: 2,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo11025 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions11025 = {
            streamInfo: audioStreamInfo11025,
            capturerInfo: audioCapturerInfo11025,
        }

        var resultFlag = await recPromise(audioCapturerOptions11025, dirPath+'/capture_js-11025-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_035
                * @tc.name      : AudioRec-Set6
                * @tc.desc      : record audio with parameter set 6
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_028', 0, async function (done) {
        var audioStreamInfo12000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_12000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S24LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo12000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions12000 = {
            streamInfo: audioStreamInfo12000,
            capturerInfo: audioCapturerInfo12000,
        }

        var resultFlag = await recPromise(audioCapturerOptions12000, dirPath+'/capture_js-12000-1C-24B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_036
                * @tc.name      : AudioRec-Set6
                * @tc.desc      : record audio with parameter set 6
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_036', 0, async function (done) {
        var audioStreamInfo12000 = {
            samplingRate: 12000,
            channels: 1,
            sampleFormat: 2,
            encodingType: 0
        };
        var audioCapturerInfo12000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions12000 = {
            streamInfo: audioStreamInfo12000,
            capturerInfo: audioCapturerInfo12000,
        }

        var resultFlag = await recPromise(audioCapturerOptions12000, dirPath+'/capture_js-12000-1C-24B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_037
                * @tc.name      : AudioRec-Set7
                * @tc.desc      : record audio with parameter set 7
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_037', 0, async function (done) {
        var audioStreamInfo16000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_16000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S32LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo16000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions16000 = {
            streamInfo: audioStreamInfo16000,
            capturerInfo: audioCapturerInfo16000,
        }

        var resultFlag = await recPromise(audioCapturerOptions16000, dirPath+'/capture_js-16000-2C-32B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_038
                * @tc.name      : AudioRec-Set7
                * @tc.desc      : record audio with parameter set 7
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_038', 0, async function (done) {
        var audioStreamInfo16000 = {
            samplingRate: 16000,
            channels: 2,
            sampleFormat: 3,
            encodingType: 0,
        };
        var audioCapturerInfo16000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions16000 = {
            streamInfo: audioStreamInfo16000,
            capturerInfo: audioCapturerInfo16000,
        }

        var resultFlag = await recPromise(audioCapturerOptions16000, dirPath+'/capture_js-16000-2C-32B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_039
                * @tc.name      : AudioRec-Set8
                * @tc.desc      : record audio with parameter set 8
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_039', 0, async function (done) {
        var audioStreamInfo22050 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_22050,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_U8,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo22050 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions22050 = {
            streamInfo: audioStreamInfo22050,
            capturerInfo: audioCapturerInfo22050,
        }

        var resultFlag = await recPromise(audioCapturerOptions22050, dirPath+'/capture_js-22050-1C-8B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_040
                * @tc.name      : AudioRec-Set8
                * @tc.desc      : record audio with parameter set 8
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_040', 0, async function (done) {
        var audioStreamInfo22050 = {
            samplingRate: 22050,
            channels: 1,
            sampleFormat: 0,
            encodingType: 0,
        };
        var audioCapturerInfo22050 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions22050 = {
            streamInfo: audioStreamInfo22050,
            capturerInfo: audioCapturerInfo22050,
        }

        var resultFlag = await recPromise(audioCapturerOptions22050, dirPath+'/capture_js-22050-1C-8B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_041
                * @tc.name      : AudioRec-Set9
                * @tc.desc      : record audio with parameter set 9
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_041', 0, async function (done) {
        var audioStreamInfo24000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_24000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo24000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions24000 = {
            streamInfo: audioStreamInfo24000,
            capturerInfo: audioCapturerInfo24000,
        }

        var resultFlag = await recPromise(audioCapturerOptions24000, dirPath+'/capture_js-24000-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_042
                * @tc.name      : AudioRec-Set9
                * @tc.desc      : record audio with parameter set 9
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_042', 0, async function (done) {
        var audioStreamInfo24000 = {
            samplingRate: 24000,
            channels: 2,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo24000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions24000 = {
            streamInfo: audioStreamInfo24000,
            capturerInfo: audioCapturerInfo24000,
        }

        var resultFlag = await recPromise(audioCapturerOptions24000, dirPath+'/capture_js-24000-2C-16B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_043
                * @tc.name      : AudioRec-Set10
                * @tc.desc      : record audio with parameter set 010
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_043', 0, async function (done) {
        var audioStreamInfo32000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_32000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S24LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo32000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions32000 = {
            streamInfo: audioStreamInfo32000,
            capturerInfo: audioCapturerInfo32000,
        }

        var resultFlag = await recPromise(audioCapturerOptions32000, dirPath+'/capture_js-32000-1C-24B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*         *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_044
                * @tc.name      : AudioRec-Set10
                * @tc.desc      : record audio with parameter set 010
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_044', 0, async function (done) {
        var audioStreamInfo32000 = {
            samplingRate: 32000,
            channels: 1,
            sampleFormat: 2,
            encodingType: 0,
        };
        var audioCapturerInfo32000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions32000 = {
            streamInfo: audioStreamInfo32000,
            capturerInfo: audioCapturerInfo32000,
        }

        var resultFlag = await recPromise(audioCapturerOptions32000, dirPath+'/capture_js-32000-1C-24B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })


    /*
     *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_045
                * @tc.name      : AudioRec-Set11
                * @tc.desc      : record audio with parameter set 011
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
*/


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_045', 0, async function (done) {
        var audioStreamInfo64000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_64000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S32LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo64000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions64000 = {
            streamInfo: audioStreamInfo64000,
            capturerInfo: audioCapturerInfo64000,
        }

        var resultFlag = await recPromise(audioCapturerOptions64000, dirPath+'/capture_js-64000-2C-32B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })


    /*     *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_046
                * @tc.name      : AudioRec-Set11
                * @tc.desc      : record audio with parameter set 011
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_046', 0, async function (done) {
        var audioStreamInfo64000 = {
            samplingRate: 64000,
            channels: 2,
            sampleFormat: 3,
            encodingType: 0,
        };
        var audioCapturerInfo64000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions64000 = {
            streamInfo: audioStreamInfo64000,
            capturerInfo: audioCapturerInfo64000,
        }

        var resultFlag = await recPromise(audioCapturerOptions64000, dirPath+'/capture_js-64000-2C-32B.pcm', audio.AudioScene.AUDIO_SCENE_VOICE_CHAT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*        *
                * @tc.number    : SUB_AUDIO_RECORD_Promise_AUDIO_SCENE_DEFAULT_047
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_RECORD_Promise_AUDIO_SCENE_DEFAULT_047', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions44100 = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        var resultFlag = await recPromise(audioCapturerOptions44100, dirPath+'/capture_js-44100-1C-16LE.pcm', audio.AudioScene.AUDIO_SCENE_DEFAULT);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*        *
                * @tc.number    : SUB_AUDIO_RECORD_Promise_AUDIO_SCENE_DEFAULT_ENUM_048
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_RECORD_Promise_AUDIO_SCENE_DEFAULT_ENUM_048', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions44100 = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        var resultFlag = await recPromise(audioCapturerOptions44100, dirPath+'/capture_js-44100-1C-16LE.pcm', 0);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*             *
                * @tc.number    : SUB_AUDIO_PROMISE_RECORD_AUDIO_SCENE_RINGING_049
                * @tc.name      : AudioRec-Set2
                * @tc.desc      : record audio with parameter set 2
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_PROMISE_RECORD_AUDIO_SCENE_RINGING_049', 0, async function (done) {
        var audioStreamInfo96000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_96000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S24LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo96000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions96000 = {
            streamInfo: audioStreamInfo96000,
            capturerInfo: audioCapturerInfo96000,
        }

        var resultFlag = await recPromise(audioCapturerOptions96000, dirPath+'/capture_js-96000-1C-S24LE.pcm', audio.AudioScene.AUDIO_SCENE_RINGING);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*             *
                * @tc.number    : SUB_AUDIO_PROMISE_RECORD_AUDIO_SCENE_RINGING_ENUM_050
                * @tc.name      : AudioRec-Set2
                * @tc.desc      : record audio with parameter set 2
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_PROMISE_RECORD_AUDIO_SCENE_RINGING_ENUM_050', 0, async function (done) {
        var audioStreamInfo96000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_96000,
            channels: audio.AudioChannel.CHANNEL_1,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S24LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo96000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions96000 = {
            streamInfo: audioStreamInfo96000,
            capturerInfo: audioCapturerInfo96000,
        }

        var resultFlag = await recPromise(audioCapturerOptions96000, dirPath+'/capture_js-96000-1C-S24LE.pcm', 1);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*             *
                * @tc.number    : SUB_AUDIO_RECORD_PROMISE_AUDIO_SCENE_PHONE_CALL_Audio_051
                * @tc.name      : AudioRec-Set3
                * @tc.desc      : record audio with parameter set 3
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_RECORD_PROMISE_AUDIO_SCENE_PHONE_CALL_Audio_051', 0, async function (done) {
        var audioStreamInfo48000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S32LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo48000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions48000 = {
            streamInfo: audioStreamInfo48000,
            capturerInfo: audioCapturerInfo48000,
        }

        var resultFlag = await recPromise(audioCapturerOptions48000, dirPath+'/capture_js-48000-2C-1S32LE.pcm', audio.AudioScene.AUDIO_SCENE_PHONE_CALL);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*             *
                * @tc.number    : SUB_AUDIO_RECORD_PROMISE_AUDIO_SCENE_PHONE_CALL_Audio_ENUM_052
                * @tc.name      : AudioRec-Set3
                * @tc.desc      : record audio with parameter set 3
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/

    it('SUB_AUDIO_RECORD_PROMISE_AUDIO_SCENE_PHONE_CALL_Audio_ENUM_052', 0, async function (done) {
        var audioStreamInfo48000 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_48000,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S32LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW,
        };
        var audioCapturerInfo48000 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags : 1,
        }
        var audioCapturerOptions48000 = {
            streamInfo: audioStreamInfo48000,
            capturerInfo: audioCapturerInfo48000,
        }

        var resultFlag = await recPromise(audioCapturerOptions48000, dirPath+'/capture_js-48000-2C-1S32LE.pcm', audio.AudioScene.AUDIO_SCENE_PHONE_CALL);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*     *
                * @tc.number    : SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_053
                * @tc.name      : AudioRec-Set11
                * @tc.desc      : record audio with parameter set 011
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_VOIP_Rec_VOICE_CHAT_Promise_ENUM_053', 0, async function (done) {
        var audioStreamInfo64000 = {
            samplingRate: 64000,
            channels: 2,
            sampleFormat: 3,
            encodingType: 0,
        };
        var audioCapturerInfo64000 = {
            source: 1,
            capturerFlags : 1,
        }
        var audioCapturerOptions64000 = {
            streamInfo: audioStreamInfo64000,
            capturerInfo: audioCapturerInfo64000,
        }

        var resultFlag = await recPromise(audioCapturerOptions64000, dirPath+'/capture_js-64000-2C-32B.pcm', 3);
        await sleep(100);
        console.info('AudioFrameworkRenderLog: resultFlag : '+resultFlag);

        expect(resultFlag).assertTrue();

        done();
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_START_055
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_START_055', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime().then(async function (audioTime) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER START : Success' + audioTime );
            if (audioTime != 0) {
                stateFlag = true;
                expect(stateFlag).assertTrue();
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime().then(async function (audioTime) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime : Success' + audioTime );
            if (audioTime != 0) {
                stateFlag = true;
                expect(stateFlag).assertTrue();
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_READ_WRITE_056
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_READ_WRITE_056', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime().then(async function (audioTime) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime );
            if (audioTime != 0) {
                stateFlag = true;
            } else {
                stateFlag = false;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    await audioCapCallBack.getAudioTime().then(async function (audioTime1) {
                        console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime1 );
                        if (audioTime1 != 0) {
                            stateFlag = true;
                        } else {
                            stateFlag = false;
                        }
                    }).catch((err) => {
                        console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
                    });
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);

        await audioCapCallBack.getAudioTime().then(async function (audioTime2) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime2 );
            if (audioTime2 != 0) {
                stateFlag = true;
            } else {
                stateFlag = false;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_STOP_057
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_GET_AUDIO_TIME_AFTER_STOP_057', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime().then(async function (audioTime) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime );
            if (audioTime != 0) {
                stateFlag = true;
            } else {
                stateFlag = false;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    await audioCapCallBack.getAudioTime().then(async function (audioTime1) {
                        console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime1 );
                        if (audioTime1 != 0) {
                            stateFlag = true;
                        } else {
                            stateFlag = false;
                        }
                    }).catch((err) => {
                        console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
                    });
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO STOP STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 3)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO STOP STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        done();
                    }
            }
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime().then(async function (audioTime2) {
            console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime AFTER WRITE : Success' + audioTime2 );
            if (audioTime2 != 0) {
                stateFlag == true;
            } else {
                stateFlag == false;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                } else {
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_GET_AUDIO_TIME_058
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_GET_AUDIO_TIME_058', 0, async function (done) {
        var stateFlag ;
        var audioCapCallBack ;
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        await audioCapCallBack.start().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
            stateFlag=false;
        });
        await sleep(1000);
        await audioCapCallBack.getAudioTime(async(err,audioTime) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer getAudioTime : Success' + audioTime);
                if (audioTime != 0) {
                    stateFlag = true;
                } else {
                    stateFlag = false;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                } else {
                    stateFlag = false;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });


    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_STOP_BEFORE_START_059
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_STOP_BEFORE_START_059', 0, async function (done) {
        var stateFlag;
        var audioCapPromise;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO NEW STATE---------');
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
            stateFlag == true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : ' + err.message);
        });
        await sleep(1000);
        await audioCapPromise.stop().then(async function () {
            console.info('AudioFrameworkRecLog: AudioCapturer STOPED : UNSUCCESS' + audioCapCallBack.state);
            if (audioCapCallBack.state == 1) {
                stateFlag = true;
            } else {
                stateFlag = false;
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop :ERROR : ' + err.message);
            stateFlag = false;
        });
        await sleep(1000);
        audioCapPromise.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                if ((audioCapCallBack.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })

    /**
               * @tc.number    : SUB_AUDIO_VOIP_CAP_PROMISE_RELEASE_BEFORE_START_060
               * @tc.name      : AudioCapturer-GET_AUDIO_TIME
               * @tc.desc      : AudioCapturer GET_AUDIO_TIME
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_PROMISE_RELEASE_BEFORE_START_060', 0, async function (done) {
        var stateFlag ;
        var audioCapPromise;
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

        await audio.createAudioCapturer(AudioCapturerOptions).then(async function (data) {
            audioCapPromise = data;
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS');
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO NEW STATE---------');
            console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            stateFlag = true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        await sleep(1000);
        await audioCapPromise.release().then(async function () {
            console.info('AudioFrameworkRecLog: Capturer released :SUCCESS ');
            stateFlag=true;
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: Capturer stop :ERROR : '+err.message);
            stateFlag=false;
            expect(stateFlag).assertTrue();
        });
        await sleep(1000);
        audioCapPromise.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapPromise.state);
                if ((audioCapPromise.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })


    /*           *
                * @tc.number    : SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_STREAM_INFO_061
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_STREAM_INFO_061', 0, async function (done) {
        var audioCapGetgetStreamInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags: 1
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetStreamInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        await audioCapGetgetStreamInfo.getStreamInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
            console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
            console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
            setFlag = true;
            if (setFlag){
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo: PASS' );
            }
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
            setFlag = false
        });
        await sleep(1000);
        audioCapGetgetStreamInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetStreamInfo.state);
                if ((audioCapGetgetStreamInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);

    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_STREAM_INFO_ENUM_062
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_STREAM_INFO_ENUM_062', 0, async function (done) {
        var audioCapGetgetStreamInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetStreamInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        await audioCapGetgetStreamInfo.getStreamInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
            console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
            console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
            console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
            console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
            setFlag = true ;
            if (setFlag){
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo: PASS' );
            }
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
            setFlag = false ;
            expect(setFlag).assertTrue();
        });
        await sleep(1000);
        audioCapGetgetStreamInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetStreamInfo.state);
                if ((audioCapGetgetStreamInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);


    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_063
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_063', 0, async function (done) {
        var audioCapGetgetStreamInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags: 1
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetStreamInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        audioCapGetgetStreamInfo.getStreamInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET STREAM INFO---------');
            console.log('AudioFrameworkRecLog: Entered getStreamInfo');
            if (err){
                console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
                console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
                console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
                console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
                console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
                setFlag = true;
            }
            await sleep(100);
            done();
        });

        audioCapGetgetStreamInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetStreamInfo.state);
                if ((audioCapGetgetStreamInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);

    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_ENUM_064
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_ENUM_064', 0, async function (done) {
        var audioCapGetgetStreamInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetStreamInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        audioCapGetgetStreamInfo.getStreamInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET STREAM INFO---------');
            console.log('AudioFrameworkRecLog: Entered getStreamInfo');
            if (err){
                console.log('AudioFrameworkRecLog: getStreamInfo :ERROR: '+err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer getStreamInfo:');
                console.info('AudioFrameworkRecLog: Capturer sampleFormat:' + audioParamsGet.sampleFormat);
                console.info('AudioFrameworkRecLog: Capturer samplingRate:' + audioParamsGet.samplingRate);
                console.info('AudioFrameworkRecLog: Capturer channels:' + audioParamsGet.channels);
                console.info('AudioFrameworkRecLog: Capturer encodingType:' + audioParamsGet.encodingType);
                setFlag = true;
            }
            await sleep(1000);
        });

        audioCapGetgetStreamInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetStreamInfo.state);
                if ((audioCapGetgetStreamInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);

    })



    /*           *
                * @tc.number    : SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_CAPTURER_INFO_065
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_CAPTURER_INFO_065', 0, async function (done) {
        var audioCapGetgetCapturerInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags: 1
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetCapturerInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        await audioCapGetgetCapturerInfo.getCapturerInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
            console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
            console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
            setFlag = true;
        }).catch((err) => {
            console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
            setFlag = false;
        });
        await sleep(1000);
        audioCapGetgetCapturerInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetCapturerInfo.state);
                if ((audioCapGetgetCapturerInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);


    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_CAPTURER_INFO_ENUM_066
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_PR_VOICE_CHAT_GET_CAPTURER_INFO_ENUM_65', 0, async function (done) {
        var audioCapGetgetCapturerInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetCapturerInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        await audioCapGetgetCapturerInfo.getCapturerInfo().then(async function (audioParamsGet) {
            console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
            console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
            console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
            setFlag = true;
        }).catch((err) => {
            setFlag = false;
            console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
        });
        await sleep(1000);
        audioCapGetgetCapturerInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetCapturerInfo.state);
                if ((audioCapGetgetCapturerInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);


    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_CAPTURER_INFO_067
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_CAPTURER_INFO_067', 0, async function (done) {
        var audioCapGetgetCapturerInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: audio.AudioSamplingRate.SAMPLE_RATE_44100,
            channels: audio.AudioChannel.CHANNEL_2,
            sampleFormat: audio.AudioSampleFormat.SAMPLE_FORMAT_S16LE,
            encodingType: audio.AudioEncodingType.ENCODING_TYPE_RAW
        }

        var audioCapturerInfo44100 = {
            source: audio.SourceType.SOURCE_TYPE_MIC,
            capturerFlags: 1
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetCapturerInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        audioCapGetgetCapturerInfo.getCapturerInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET CAPTURER INFO---------');
            if (err){
                console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
                console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
                console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
                setFlag = true;
                await sleep(100);
                done();
            }
        });
        await sleep(1000);
        audioCapGetgetCapturerInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetCapturerInfo.state);
                if ((audioCapGetgetCapturerInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);

    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_ENUM_068
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_CB_VOICE_CHAT_GET_STREAM_INFO_ENUM_068', 0, async function (done) {
        var audioCapGetgetCapturerInfo;
        var setFlag;
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data != undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
                audioCapGetgetCapturerInfo = data;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
            done();
        });
        await sleep(1000);
        audioCapGetgetCapturerInfo.getCapturerInfo(async(err,audioParamsGet) => {
            console.info('AudioFrameworkRecLog: ---------GET CAPTURER INFO---------');
            if (err){
                console.log('AudioFrameworkRecLog: CapturerInfo :ERROR: '+err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer CapturerInfo:');
                console.info('AudioFrameworkRecLog: Capturer SourceType:' + audioParamsGet.source);
                console.info('AudioFrameworkRecLog: Capturer capturerFlags:' + audioParamsGet.capturerFlags);
                setFlag = true;
                await sleep(1000);
                done();
            }
        });

        audioCapGetgetCapturerInfo.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                setFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapGetgetCapturerInfo.state);
                if ((audioCapGetgetCapturerInfo.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    setFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : ' + setFlag);
                    expect(setFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_VOICE_CHAT_PR_ENUM_AUDIO_STREAM_INFO_INVALID_069
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_VOICE_CHAT_PR_ENUM_AUDIO_STREAM_INFO_INVALID_069', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: 0,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        var audioCap;

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data == undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data);
                expect(true).assertTrue();
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
            }
        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        done();
    })

    /*           *
                * @tc.number    : SUB_AUDIO_Rec_VOICE_CHAT_PR_ENUM_AUDIO_CAPTURER_INFO_INVALID_070
                * @tc.name      : AudioRec-Set1
                * @tc.desc      : record audio with parameter set 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0*/


    it('SUB_AUDIO_Rec_VOICE_CHAT_PR_ENUM_AUDIO_CAPTURER_INFO_INVALID_070', 0, async function (done) {
        var audioStreamInfo44100 = {
            samplingRate: 44100,
            channels: 1,
            sampleFormat: 1,
            encodingType: 0,
        };
        var audioCapturerInfo44100 = {
            source: 1000,
            capturerFlags : 1,
        }
        var AudioCapturerOptionsInvalid = {
            streamInfo: audioStreamInfo44100,
            capturerInfo: audioCapturerInfo44100,
        }

        await audio.createAudioCapturer(AudioCapturerOptionsInvalid).then(async function (data) {
            if (data == undefined) {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Unsuccess :'+ data );
                expect(true).assertTrue();
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success:'+ data.state );
            }

        }).catch((err) => {
            console.info('AudioFrameworkRecLog: AudioCapturer Created : ERROR : '+err.message);
        });
        done();
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_ON_ALL_CASES_071
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_ON_ALL_CASES_070', 0, async function (done) {
        var stateFlag ;
        var audioCapCallBack;
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
        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('stateChange',(AudioState) => {
            console.info('AudioCapturerLog: Changed State to : ' + AudioState)
            switch (AudioState) {
                case audio.AudioState.STATE_NEW:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------NEW--------------');
                    console.info('AudioFrameworkTest: Audio State is : New');
                    break;
                case audio.AudioState.STATE_PREPARED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------PREPARED--------------');
                    console.info('AudioFrameworkTest: Audio State is : Prepared');
                    break;
                case audio.AudioState.STATE_RUNNING:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RUNNING--------------');
                    console.info('AudioFrameworkTest: Audio State is : Running');
                    break;
                case audio.AudioState.STATE_STOPPED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------STOPPED--------------');
                    console.info('AudioFrameworkTest: Audio State is : stopped');
                    break;
                case audio.AudioState.STATE_RELEASED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RELEASED--------------');
                    console.info('AudioFrameworkTest: Audio State is : released');
                    break;
                default:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------INVALID--------------');
                    console.info('AudioFrameworkTest: Audio State is : invalid');
                    break;
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);

        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO STOPPED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == audio.AudioState.STATE_STOPPED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO STOPPED STATE---------');
                    stateFlag == true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                stateFlag=true;
                console.info('AudioFrameworkRenderLog: stateFlag : '+stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
    })

    /*         *
                   * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_ON_PREPARED_072
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_ON_PREPARED_072', 0, async function (done) {
        var stateFlag;
        var audioCapCallBack;
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
        audio.createAudioCapturer(AudioCapturerOptions, async (err, value) => {
            if (err) {
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('stateChange', (AudioState) => {
            console.info('AudioCapturerLog: Changed State to : ' + AudioState)
            switch (AudioState) {
                case audio.AudioState.STATE_NEW:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------NEW--------------');
                    console.info('AudioFrameworkTest: Audio State is : New');
                    break;
                case audio.AudioState.STATE_PREPARED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------PREPARED--------------');
                    console.info('AudioFrameworkTest: Audio State is : Prepared');
                    break;
                case audio.AudioState.STATE_RUNNING:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RUNNING--------------');
                    console.info('AudioFrameworkTest: Audio State is : Running');
                    break;
                case audio.AudioState.STATE_STOPPED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------STOPPED--------------');
                    console.info('AudioFrameworkTest: Audio State is : stopped');
                    break;
                case audio.AudioState.STATE_RELEASED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RELEASED--------------');
                    console.info('AudioFrameworkTest: Audio State is : released');
                    break;
                default:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------INVALID--------------');
                    console.info('AudioFrameworkTest: Audio State is : invalid');
                    break;
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT, async (err, setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err) {
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : ' + setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: stateFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_ON_START_073
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_ON_START_073', 0, async function (done) {
        var stateFlag;
        var audioCapCallBack;
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
        audio.createAudioCapturer(AudioCapturerOptions, async (err, value) => {
            if (err) {
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('stateChange', (AudioState) => {
            console.info('AudioCapturerLog: Changed State to : ' + AudioState)
            switch (AudioState) {
                case audio.AudioState.STATE_NEW:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------NEW--------------');
                    console.info('AudioFrameworkTest: Audio State is : New');
                    break;
                case audio.AudioState.STATE_PREPARED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------PREPARED--------------');
                    console.info('AudioFrameworkTest: Audio State is : Prepared');
                    break;
                case audio.AudioState.STATE_RUNNING:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RUNNING--------------');
                    console.info('AudioFrameworkTest: Audio State is : Running');
                    break;
                case audio.AudioState.STATE_STOPPED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------STOPPED--------------');
                    console.info('AudioFrameworkTest: Audio State is : stopped');
                    break;
                case audio.AudioState.STATE_RELEASED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RELEASED--------------');
                    console.info('AudioFrameworkTest: Audio State is : released');
                    break;
                default:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------INVALID--------------');
                    console.info('AudioFrameworkTest: Audio State is : invalid');
                    break;
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT, async (err, setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err) {
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : ' + setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: stateFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
    })

    /*         *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_ON_STOP_074
               * @tc.name      : AudioCapturer-Check-STATE-STOPPED
               * @tc.desc      : AudioCapturer with state stopped
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_ON_STOP_074', 0, async function (done) {
        var stateFlag;
        var audioCapCallBack;
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
        audio.createAudioCapturer(AudioCapturerOptions, async (err, value) => {
            if (err) {
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('stateChange', (AudioState) => {
            console.info('AudioCapturerLog: Changed State to : ' + AudioState)
            switch (AudioState) {
                case audio.AudioState.STATE_NEW:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------NEW--------------');
                    console.info('AudioFrameworkTest: Audio State is : New');
                    break;
                case audio.AudioState.STATE_PREPARED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------PREPARED--------------');
                    console.info('AudioFrameworkTest: Audio State is : Prepared');
                    break;
                case audio.AudioState.STATE_RUNNING:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RUNNING--------------');
                    console.info('AudioFrameworkTest: Audio State is : Running');
                    break;
                case audio.AudioState.STATE_STOPPED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------STOPPED--------------');
                    console.info('AudioFrameworkTest: Audio State is : stopped');
                    break;
                case audio.AudioState.STATE_RELEASED:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------RELEASED--------------');
                    console.info('AudioFrameworkTest: Audio State is : released');
                    break;
                default:
                    console.info('AudioFrameworkTest:--------CHANGE IN AUDIO STATE----------INVALID--------------');
                    console.info('AudioFrameworkTest: Audio State is : invalid');
                    break;
            }
        });
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT, async (err, setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err) {
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : ' + setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS' + audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                await sleep(1000);
                if ((audioCapCallBack.state == 2)) {
                    stateFlag == true;
                }
            }
        });
        await sleep(1000);

        audioCapCallBack.stop(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------STOP RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer stop:ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO STOPPED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer stopped : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == audio.AudioState.STATE_STOPPED)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO STOPPED STATE---------');
                    stateFlag == true;
                }
                console.info('AudioFrameworkRecLog: stateFlag : '+stateFlag);
            }
        });

        await sleep(1000);
        audioCapCallBack.release(async (err, value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err) {
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : ' + err.message);
                stateFlag = false;
            } else {
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : ' + audioCapCallBack.state);
                stateFlag = true;
                console.info('AudioFrameworkRenderLog: stateFlag : ' + stateFlag);
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_5000_REACH_075
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_5000_REACH_075', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 5000, (position) => {
            if (position == 5000) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                if ((audioCapCallBack.state == 4)) {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                    stateFlag = true;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                } else {
                    stateFlag = false;
                    console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                    expect(stateFlag).assertTrue();
                    done();
                }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_1000_REACH_076
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_1000_REACH_076', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 1000, (position) => {
            if (position == 1000) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_10000_REACH_077
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_10000_REACH_077', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 10000, (position) => {
            if (position == 10000) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_100_REACH_078
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_100_REACH_078', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 100, (position) => {
            if (position == 100) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_1_REACH_079
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_1_REACH_079', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 1, (position) => {
            if (position == 1) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_0_REACH_080
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_0_REACH_080', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 0, (position) => {
            if (position == 0) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_LARGEVALUE_REACH_081
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_LARGEVALUE_REACH_081', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', 1234567890, (position) => {
            if (position == 1234567890) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_NEGATIVEVALUE_REACH_082
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_NEGATIVEVALUE_REACH_082', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', -2, (position) => {
            if (position == -2) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_STRINGVALUE_REACH_083
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_MARK_STRINGVALUE_REACH_083', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('markReach', "HARSHA", (position) => {
            if (position == "HARSHA") {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true;
            } else {
                stateFlag = false;
                expect(stateFlag).assertTrue();
                done();
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('markReach');
        await sleep(1000);
        audioCapCallBack.release(async(err,value) => {
            console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                stateFlag=false;
            } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                    console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                    console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                    if ((audioCapCallBack.state == 4)) {
                        console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                        stateFlag = true;
                        console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                        expect(stateFlag).assertTrue();
                        done();
                    }
            }
        });
        await sleep(1000);
    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_1000_084
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_1000_084', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('periodReach',1000, (position) => {
            if (position == "1000") {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true ;
            } else {
                stateFlag = false;
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('periodReach');
        await sleep(1000);
        if (stateFlag == true) {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            stateFlag = true;
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        } else {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                    expect(stateFlag).assertTrue();
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            stateFlag=false;
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        }

    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_1_085
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_1_085', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('periodReach',1, (position) => {
            if (position == "1") {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true ;
            } else {
                stateFlag = false;
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('periodReach');
        await sleep(1000);
        if (stateFlag == true) {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            stateFlag = true;
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        } else {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                    expect(stateFlag).assertTrue();
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            stateFlag=false;
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        }

    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_NEGATIVE_086
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_NEGATIVE_086', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('periodReach',-2, (position) => {
            if (position == -2) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: mark reached: ' + position);
                stateFlag = true ;
            } else {
                stateFlag = false;
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('periodReach');
        await sleep(1000);
        if (stateFlag == true) {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            stateFlag = true;
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        } else {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                    expect(stateFlag).assertTrue();
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            stateFlag=false;
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        }

    })


    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_223750_087
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_223750_087', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('periodReach',223750, (position) => {
            if (position == 223750) {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: periodReach: ' + position);
                stateFlag = true ;
            } else {
                stateFlag = false;
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('periodReach');
        await sleep(1000);
        if (stateFlag == true) {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            stateFlag = true;
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        } else {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                    expect(stateFlag).assertTrue();
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            stateFlag=false;
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        }

    })

    /*     *
               * @tc.number    : SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_STRING_088
               * @tc.name      : AudioCapturer-Check-READ_BUFFER
               * @tc.desc      : AudioCapturer with read buffer
               * @tc.size      : MEDIUM
               * @tc.type      : Function
               * @tc.level     : Level 0*/

    it('SUB_AUDIO_VOIP_CAP_CB_READ_BUFFER_PERIOD_REACH_STRING_088', 0, async function (done) {
        var stateFlag ;
        var fpath = dirPath+'/capture_CB_js-44100-2C-S16LE-checkcbreadbuffer.pcm';
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

        audio.createAudioCapturer(AudioCapturerOptions,async(err,value) => {
            if (err){
                console.info('AudioFrameworkRecLog: AudioCapturer Not Created : Fail : Stream Type: FAIL');
            } else {
                audioCapCallBack = value;
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
            }
        });
        await sleep(1000);
        audioCapCallBack.on('periodReach',"Harsha", (position) => {
            if (position == "Harsha") {
                console.info('AudioFrameworkRecLog: ---------ON TRIGGERED SUCCESSFULLY---------');
                console.info('AudioRenderLog: periodReach: ' + position);
                stateFlag = true ;
            } else {
                stateFlag = false;
            }
        });
        await sleep(1000);
        audioManagerRec.setAudioScene(audio.AudioScene.AUDIO_SCENE_VOICE_CHAT,async(err,setValue) => {
            console.info('AudioFrameworkRecLog: ---------SET AUDIO SCENE---------');
            if (err){
                console.info('AudioFrameworkRecLog: setAudioScene : ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: setAudioScene : SUCCESS ');
                console.info('AudioFrameworkRecLog: setAudioScene : Value : '+setValue);
                console.info('AudioFrameworkRecLog: getAudioScene : Value : startawait');
                console.info('AudioFrameworkRecLog: getAudioScene : Value : initaitestart');
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: AudioCapturer Created : Success : Stream Type: SUCCESS'+audioCapCallBack.state);
                if ((audioCapCallBack.state == 1)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        audioCapCallBack.start(async (err,value) => {
            console.info('AudioFrameworkRecLog: AudioCapturer : START SUCCESS');
            console.info('AudioFrameworkRecLog: ---------START---------');
            if (err){
                console.info('AudioFrameworkRecLog: Capturer start :ERROR : '+err.message);
                stateFlag=false;
            } else {
                console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                console.info('AudioFrameworkRecLog: Capturer started :SUCCESS ');
                if ((audioCapCallBack.state == 2)) {
                    stateFlag = true;
                }
            }
        });
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------GET BUFFER SIZE---------');
        var bufferSize = await audioCapCallBack.getBufferSize();
        console.info('AudioFrameworkRecLog: buffer size: ' + bufferSize);
        await sleep(1000);
        console.info('AudioFrameworkRecLog: ---------OPEN FILE---------');
        var fd = fileio.openSync(fpath, 0o102, 0o777);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd created');
        }else{
            console.info('AudioFrameworkRecLog: Capturer start :ERROR : ');
            stateFlag=false;
        }
        console.info('AudioFrameworkRecLog: ---------OPEN FILE IN APPEND MODE---------');
        fd = fileio.openSync(fpath, 0o2002, 0o666);
        if (fd !== null) {
            console.info('AudioFrameworkRecLog: file fd opened : Append Mode :PASS');
        } else {
            console.info('AudioFrameworkRecLog: file fd Open: Append Mode : FAILED');
            stateFlag=false;
        }
        await sleep(1000);
        var numBuffersToCapture = 100;
        while (numBuffersToCapture) {
            console.info('AudioFrameworkRecLog: ---------BEFORE CHECK CB READ BUFFER---------');
            audioCapCallBack.read(bufferSize, true,async(err,buffer) => {
                if (err) {
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {
                    console.info('AudioFrameworkRecLog: ---------AFTER CHECK CB READ BUFFER---------');
                    console.info('AudioFrameworkRecLog: AudioCapturer : readvalue : '+buffer);
                    await sleep(50);
                    console.info('AudioFrameworkRecLog: ---------WRITE BUFFER---------');
                    var number = fileio.writeSync(fd, buffer);
                    console.info('BufferRecLog: data written: ' + number);
                    await sleep(100);
                    stateFlag=true;
                }
            });
            numBuffersToCapture--;
        }
        await sleep(3000);
        audioCapCallBack.off('periodReach');
        await sleep(1000);
        if (stateFlag == true) {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            stateFlag = true;
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        } else {
            audioCapCallBack.release(async(err,value) => {
                console.info('AudioFrameworkRecLog: ---------RELEASE RECORD---------');
                if (err){
                    console.info('AudioFrameworkRecLog: Capturer release :ERROR : '+err.message);
                    stateFlag=false;
                    expect(stateFlag).assertTrue();
                } else {console.info('AudioFrameworkRecLog: ---------BEFORE CHECK AUDIO RELASED STATE---------');
                        console.info('AudioFrameworkRecLog: Capturer release : SUCCESS');
                        console.info('AudioFrameworkRecLog: AudioCapturer : STATE : '+audioCapCallBack.state);
                        if ((audioCapCallBack.state == 4)) {
                            console.info('AudioFrameworkRecLog: ---------AFTER CHECK AUDIO RELEASED STATE---------');
                            console.info('AudioFrameworkRenderLog: resultFlag : '+stateFlag);
                            stateFlag=false;
                            expect(stateFlag).assertTrue();
                            done();
                        }
                }
            });
            await sleep(1000);
        }

    })

})
