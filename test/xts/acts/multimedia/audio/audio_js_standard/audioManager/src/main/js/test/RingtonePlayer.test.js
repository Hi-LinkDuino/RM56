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
import mediaLibrary from '@ohos.multimedia.medialibrary';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('audioManager',async function () {
	console.info('AudioFrameworkTest: Create systemSoundManager Object JS Framework');
	const systemSoundManager = audio.getSystemSoundManager();
	const mediaLibInstance = mediaLibrary.getMediaLibrary();
	let ringtonePlayer;
	var minVol = 0;
	var maxVol = 1;
	var lowVol = 0.1;
	var highVol = 0.9;
	var outOfRangeVol = 28;
	var defaultRingtoneType=0;
	var multiSIM=1;
	var volErrorMesg = 'Error, Operation not supported or Failed';

	function sleep (ms) {
		return new Promise(resolve => setTimeout(resolve, ms));
	}

	beforeAll(async function () {
		console.info('AudioFrameworkTest: beforeAll: Prerequisites at the test suite level');
		console.info('AudioFrameworkTest: before get scanner instance');
		let scannerObj = mediaLibrary.getScannerInstance();
		let path = "/storage/media/100/local/files/media";
		console.info('AudioFrameworkTest: before scan file');
		const scanPromise = await scannerObj.scanDir(path);
		console.info('AudioFrameworkTest: before scan file');
		console.info("AudioFrameworkTest: scan promise: " + scanPromise);
		console.info("AudioFrameworkTest: promise status: " + scanPromise.status);
	})

    beforeEach(async function () {
        console.info('AudioFrameworkTest: beforeEach: Prerequisites at the test case level');
		await sleep(1000);
	})

    afterEach(async function () {
        console.info('AudioFrameworkTest: afterEach: Test case-level clearance conditions');
		await sleep(1000);

	})

    afterAll(async function () {
        console.info('AudioFrameworkTest: afterAll: Test suite-level cleanup condition');
		await ringtonePlayer.stop();
		await sleep(100);
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayer.release();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_001
                * @tc.name      : getSystemSoundManager
                * @tc.desc      : Check getSystemSoundManager is not empty
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_001', 0, function (done) {
		const systemSoundManager = audio.getSystemSoundManager();
        if(systemSoundManager!=null){
            console.info('AudioFrameworkTestSSM: get systemSoundManager : PASS');
            expect(true).assertTrue();
        }
        else{
            console.info('AudioFrameworkTestSSM: get systemSoundManager : FAIL');
            expect(false).assertTrue();
        }
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_002
                * @tc.name      : setSystemRingtoneUri -Promise-RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : setSystemRingtoneUri - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager002', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',0).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_003
                * @tc.name      : getSystemRingtoneUri - promise RINGTONE_TYPE_DEFAULT enum=0
                * @tc.desc      : getSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_003', 0,async function(done){
        const promise = systemSoundManager.getSystemRingtoneUri(null, 0);
        promise.then(function (data) {
            if (data != null) {
                console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
				expect(true).assertTrue();
            }
            else {
                console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
				expect(false).assertTrue();
            }
			}).catch((err) => {
                console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
				expect(false).assertTrue();
            });
		await promise;
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_004
                * @tc.name      : setSystemRingtoneUri - Promise RINGTONE_TYPE_MULTISIM enum 1
                * @tc.desc      : setSystemRingtoneUri - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_004', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',multiSIM).then(function(){
			console.info('AudioFrameworkTestSSM: enum: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM: enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_005
                * @tc.name      : getSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM enum 1
                * @tc.desc      : getSystemRingtoneUri - promise enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_005', 0,async function(done){
        const promise = systemSoundManager.getSystemRingtoneUri(null,multiSIM);
        promise.then(function (data) {
            if (data != null) {
                console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
				expect(true).assertTrue();
            }
            else {
                console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
				expect(false).assertTrue();
            }
			}).catch((err) => {
                console.info('AudioFrameworkTestSSM: enum Ringtone Get SRC Path  ERROR: ' + err.message);
				expect(false).assertTrue();
            });
		await promise;
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_006
                * @tc.name      : setSystemRingtoneUri - Promise RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : setSystemRingtoneUri - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_006', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:ENAME: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:ENAME: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
	})

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_007
                * @tc.name      : getSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : getSystemRingtoneUri - promise set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_011', 0,async function(done){
		const promise = systemSoundManager.getSystemRingtoneUri(null,1);
		promise.then(function (data) {
			if (data != null) {
            console.info('AudioFrameworkTestSSM: Get Ringtone Uri : MULTISIM : SUCCESS: ' + data);
			expect(true).assertTrue();
			}
			else {
            console.info('AudioFrameworkTestSSM: Get Ringtone Uri :MULTISIM: FAIL: ' + data);
			expect(false).assertTrue();
			}
			}).catch((err) => {
                console.info('AudioFrameworkTestSSM: Ringtone Get Ringtone uri: ERROR: ' + err.message);
				expect(false).assertTrue();
            });
		await promise;
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_008
                * @tc.name      : getSystemRingtoneUri - Callback RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : getSystemRingtoneUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_008', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Uri');
        systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT,async(err,data) => {
            if(err) {
			console.error('AudioFrameworkTestRM: Callback :enum: Get System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		}
		else if(data!= null){
			console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : SUCCESS: '+data);
			expect(true).assertTrue();
		}
		else{
			console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : FAIL: '+data);
			expect(false).ssertTrue();
			}
			await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_009
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM ENAME
                * @tc.desc      : setSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_009', 0,async function(done){
		console.log('AudioFrameworkTestSSM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_MULTISIM).then( function() {
			console.info('AudioFrameworkTestSSM:Callback : ENAME: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
			}).catch((err) => {
            console.info('AudioFrameworkTestSSM:Callback : ENAME: Set Ringtone Uri : ERROR:' + err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                    * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_010
                    * @tc.name      : getSystemRingtonePlayer - promise  RINGTONE_TYPE_MULTISIM enum
                    * @tc.desc      : getSystemRingtonePlayer - promise
                    * @tc.size      : MEDIUM
                    * @tc.type      : Function
                    * @tc.level     : Level 0
                */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_010', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
        await systemSoundManager.getSystemRingtonePlayer(null,1).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
				expect(true).assertTrue();
			}
        }).catch((err) => {
            console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_011
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : getSystemRingtonePlayer - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_011', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
        await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' +(err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_012
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : getSystemRingtonePlayer - promise set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_012', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,1).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:enum: get Ringtone Player : SUCCESS: ' + data.state);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:enum: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_013
                * @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : setSystemRingtoneUri - Callback set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_013', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',1,async(err) => {
            if(err) {
			console.error('AudioFrameworkTestSSM :enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		}
		else{
			console.info('AudioFrameworkTestSSM :enum: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
		}
		await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_014
                * @tc.name      : setSystemRingtoneUri - Callback
                * @tc.desc      : RINGTONE_TYPE_DEFAULT enum 0
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_014', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',0,async(err) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		}
		else{
			console.info('AudioFrameworkTestSSM:Callback :enum: Set Ringtone Uri : SUCCESS: ');
			expect(true).assertTrue();
		}
		await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_015
                * @tc.name      : getSystemRingtoneUri - Callback
                * @tc.desc      : getSystemRingtoneUri - Callback set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_015', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Uri');
        systemSoundManager.getSystemRingtoneUri(null,1,async(err,data) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :enum: get System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		}
		else if(data!= null){
			console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : SUCCESS: '+data);
			expect(true).assertTrue();
		}
		else{
			console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : FAIL: '+data);
			expect(false).assertTrue();
			}
			await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_016
                * @tc.name      : getSystemRingtoneUri - Callback RINGTONE_TYPE_DEFAULT
                * @tc.desc      : getSystemRingtoneUri - Callback set enum 0
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_016', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.getSystemRingtoneUri(null,defaultRingtoneType,async(err,data) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :enum: get System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
			}
			else if(data!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : SUCCESS: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : FAIL: '+data);
				expect(false).assertTrue();
			}
			await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_017
                * @tc.name      : getSystemRingtoneUri - Callback RINGTONE_TYPE_MULTISIM
                * @tc.desc      : getSystemRingtoneUri - Callback ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_017', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err,data) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :enum: get System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
			}
			else if(data!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : SUCCESS: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Uri : FAIL: '+data);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_018
                * @tc.name      : getSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM
                * @tc.desc      : getSystemRingtoneUri - promise ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_018', 0,async function(done){
		const promise = systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM);
		promise.then(function (data) {
			if (data != null) {
				console.info('AudioFrameworkTestSSM: Get Ringtone Uri : MULTISIM : SUCCESS: ' + data);
				expect(true).assertTrue();
			}
			else {
				console.info('AudioFrameworkTestSSM: Get Ringtone Uri :MULTISIM: FAIL: ' + data);
				expect(false).assertTrue();
			}
			}).catch((err) => {
                console.info('AudioFrameworkTestSSM:  Get Ringtone Uri: ERROR: ' + err.message);
				expect(false).assertTrue();
		});
		await promise;
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_019
                * @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE_MULTISIM
                * @tc.desc      : setSystemRingtoneUri - Callback ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_019', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :ENAME: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :ENAME: Set Ringtone Uri : SUCCESS: ');
				expect(true).assertTrue();
			}
			await sleep(1000);
        });
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_020
                * @tc.name      : getSystemRingtonePlayer -Callback RINGTONE_TYPE_MULTISIM
                * @tc.desc      : getSystemRingtonePlayer -Callback ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_020', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
        systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err,data)=> {
			ringtonePlayer=data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
			console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Fail: '+data.state);
			expect(false).assertTrue();
			}
			await sleep(1000);
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_021
                * @tc.name      : getSystemRingtonePlayer -Callback RINGTONE_TYPE_DEFAULT
                * @tc.desc      : getSystemRingtonePlayer -Callback ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_021', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT,async(err,data)=> {
			ringtonePlayer=data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Pass: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Fail: '+data);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_022
                * @tc.name      : getSystemRingtonePlayer -Callback RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : getSystemRingtonePlayer -Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_SystemSoundManager_022', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		systemSoundManager.getSystemRingtonePlayer(null,multiSIM,async(err,data)=> {
			ringtonePlayer=data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_023
                * @tc.name      : setSystemNotificationUri - promise - ENAME
                * @tc.desc      : setSystemNotificationUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_023', 0,async function(done){
		console.log('AudioFrameworkTestSSM: setSystemNotificationUri');
		await systemSoundManager.setSystemNotificationUri(null,'/storage/media/100/local/files/media/test.mp3').then( function() {
			console.info('AudioFrameworkTestSSM:Callback : setSystemNotificationUri : SUCCESS');
			expect(true).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:Callback : setSystemNotificationUri : ERROR:' + err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_024
                * @tc.name      : getSystemNotificationUri - promise
                * @tc.desc      : getSystemNotificationUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_024', 0,async function(done){
		console.log('AudioFrameworkTestRM: getSystemNotificationUri');
		await systemSoundManager.getSystemNotificationUri(null).then(async function(data) {
			if (data == '/storage/media/100/local/files/media/test.mp3') {
				console.info('AudioFrameworkTestSSM:enum: getSystemNotificationUri : SUCCESS : '+data);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:enum: getSystemNotificationUri : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_025
                * @tc.name      : setSystemNotificationUri - promise - NULL
                * @tc.desc      : setSystemNotificationUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_025', 0,async function(done){
		console.log('AudioFrameworkTestSSM: setSystemNotificationUri');
		await systemSoundManager.setSystemNotificationUri(null,null).then( function() {
			console.info('AudioFrameworkTestSSM:Callback : setSystemNotificationUri : SUCCESS');
			expect(false).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:Callback : setSystemNotificationUri : ERROR:' + err.message);
			expect(true).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_026
                * @tc.name      : setSystemNotificationUri - Callback
                * @tc.desc      : setSystemNotificationUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_026', 0,async function(done){
		console.log('AudioFrameworkTestRM: setSystemNotificationUri');
		systemSoundManager.setSystemNotificationUri(null,'/storage/media/100/local/files/media/test.mp4',async(err) => {
			if(err) {
				console.error('AudioFrameworkTestSSM : setSystemNotificationUri :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM : setSystemNotificationUri : SUCCESS: ');
				expect(true).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_027
                * @tc.name      : getSystemNotificationUri - Callback
                * @tc.desc      : getSystemNotificationUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_027', 0,async function(done){
		console.log('AudioFrameworkTestRM: getSystemNotificationUri');
		systemSoundManager.getSystemNotificationUri(null,async(err,data) => {
			if(err) {
				console.error('AudioFrameworkTestSSM: Callback : getSystemNotificationUri :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else if(data == '/storage/media/100/local/files/media/test.mp4'){
				console.info('AudioFrameworkTestSSM:Callback : getSystemNotificationUri : SUCCESS: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback : getSystemNotificationUri : FAIL: '+data);
				expect(false).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_028
                * @tc.name      : setSystemNotificationUri - Callback - NULL
                * @tc.desc      : setSystemNotificationUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_028', 0,async function(done){
		console.log('AudioFrameworkTestRM: setSystemNotificationUri');
		systemSoundManager.setSystemNotificationUri(null,null,async(err) => {
			if(err) {
				console.error('AudioFrameworkTestSSM : setSystemNotificationUri :ERROR: '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM : setSystemNotificationUri : SUCCESS');
				expect(false).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_029
                * @tc.name      : setSystemAlarmUri - promise - ENAME
                * @tc.desc      : setSystemAlarmUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_029', 0,async function(done){
		console.log('AudioFrameworkTestSSM: setSystemAlarmUri');
		await systemSoundManager.setSystemAlarmUri(null,'/storage/media/100/local/files/media/test.mp3').then( function() {
			console.info('AudioFrameworkTestSSM:Callback : setSystemAlarmUri : SUCCESS');
			expect(true).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:Callback : setSystemAlarmUri : ERROR:' + err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_030
                * @tc.name      : getSystemNotificationUri - promise
                * @tc.desc      : getSystemNotificationUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_030', 0,async function(done){
		console.log('AudioFrameworkTestRM: getSystemAlarmUri');
		await systemSoundManager.getSystemAlarmUri(null).then(async function(data) {
			if (data == '/storage/media/100/local/files/media/test.mp3') {
				console.info('AudioFrameworkTestSSM:enum: getSystemAlarmUri : SUCCESS :'+data);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:enum: getSystemAlarmUri : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_031
                * @tc.name      : setSystemAlarmUri - promise - NULL
                * @tc.desc      : setSystemAlarmUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_031', 0,async function(done){
		console.log('AudioFrameworkTestSSM: setSystemAlarmUri');
		await systemSoundManager.setSystemAlarmUri(null,null).then( function() {
			console.info('AudioFrameworkTestSSM:Callback : setSystemAlarmUri : SUCCESS ');
			expect(false).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:Callback : setSystemAlarmUri : ERROR:' + err.message);
			expect(true).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_032
                * @tc.name      : setSystemAlarmUri - Callback
                * @tc.desc      : setSystemAlarmUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_032', 0,async function(done){
		console.log('AudioFrameworkTestRM: setSystemAlarmUri');
		systemSoundManager.setSystemAlarmUri(null,'/storage/media/100/local/files/media/test.mp4',async(err) => {
			if(err) {
				console.error('AudioFrameworkTestSSM : setSystemAlarmUri :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM : setSystemAlarmUri : SUCCESS: ');
				expect(true).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_033
                * @tc.name      : getSystemAlarmUri - Callback
                * @tc.desc      : getSystemAlarmUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_033', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
		systemSoundManager.getSystemAlarmUri(null,async(err,data) => {
			if(err) {
				console.error('AudioFrameworkTestSSM: Callback : getSystemAlarmUri :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else if(data == '/storage/media/100/local/files/media/test.mp4'){
				console.info('AudioFrameworkTestSSM:Callback : getSystemAlarmUri : SUCCESS: '+data);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback : getSystemAlarmUri : FAIL: '+data);
				expect(false).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_034
                * @tc.name      : setSystemAlarmUri - Callback - NULL
                * @tc.desc      : setSystemAlarmUri - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_SystemSoundManager_034', 0,async function(done){
		console.log('AudioFrameworkTestRM: setSystemAlarmUri');
		systemSoundManager.setSystemAlarmUri(null,null,async(err) => {
			if(err) {
				console.error('AudioFrameworkTestSSM : setSystemAlarmUri :ERROR: '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM : setSystemAlarmUri : SUCCESS: ');
				expect(false).assertTrue();
			}
			await sleep(100);
		});
		done();
	})

	/* *
			* @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_035
			* @tc.name      : getSystemRingtonePlayer - ERROR - Negative
			* @tc.desc      : getSystemRingtonePlayer - ERROR - Negative
			* @tc.size      : MEDIUM
			* @tc.type      : Function
			* @tc.level     : Level 0
		*/
	it('SUB_AUDIO_MANAGER_SystemSoundManager_035', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,-1).then(async function(data) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
				expect(false).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(true).assertTrue();
		});
		done();
	})

	/* *
			* @tc.number    : SUB_AUDIO_MANAGER_SystemSoundManager_036
			* @tc.name      : getSystemRingtonePlayer - ERROR - OutOfRange
			* @tc.desc      : getSystemRingtonePlayer - ERROR - OutOfRange
			* @tc.size      : MEDIUM
			* @tc.type      : Function
			* @tc.level     : Level 0
		*/
	it('SUB_AUDIO_MANAGER_SystemSoundManager_036', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,2).then(async function(data) {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			expect(false).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(true).assertTrue();
		});
		done();
	})

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_001
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : setSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_001', 0,async function(done){
		console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_DEFAULT);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set FAIL: ' + data);
                    expect(false).assertTrue();
                }
				}).catch((err) => {
                console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path : ERROR: ' + err.message);
				expect(false).assertTrue();
            });
        }).catch((err) => {
            console.info('AudioFrameworkTestSSM: Ringtone set SRC Path : ERROR: ' + err.message);
			expect(false).assertTrue();
		});
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_002
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : getSystemRingtonePlayer - promise get state ,enum 0
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_002', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,0).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM :enum: get Ringtone Player : SUCCESS: ' + data.state);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:enum: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_003
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM ENAME
                * @tc.desc      : setSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_003', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_DEFAULT);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set FAIL: ' + data);
                    expect(false).assertTrue();
                }
            }).catch((err) => {
                console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path ERROR: ' + err.message);
                expect(false).assertTrue();
            });
        }).catch((err) => {
            console.info('AudioFrameworkTestSSM: Ringtone SRC Path set  ERROR: ' + err.message);
            expect(false).assertTrue();
        });
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_004
                * @tc.name      : Configure - promise - Volume=maxVol ,Disable loop
                * @tc.desc      : Configure - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_004', 0,async function(done){
        let ringtoneOptions = {
            volume: maxVol,
            loop: false
        };
		sleep(100);
        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : lowVol: Loop False');
        await ringtonePlayer.configure(ringtoneOptions).then(function() {
			console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : ' +ringtonePlayer.state);
			expect(true).assertTrue();
		}).catch((err) => {
            console.info('AudioFrameworkTestRM: Configure RingtonePlayer : FAIL:' + err.message);
            expect(false).assertTrue();
        });
		sleep(100);
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_005
                * @tc.name      : configure- promise Volume=maxVol ,enable loop
                * @tc.desc      : configure- promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_005', 0,async function(done){

        let ringtoneOptions = {
            volume: maxVol,
            loop: true
        };
		await sleep(100);
        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : maxVol : Loop True');
        await ringtonePlayer.configure(ringtoneOptions).then(function() {
			console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS: ' +ringtonePlayer.state);
			expect(true).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestRM: Configure RingtonePlayer : FAIL:' + err.message);
			expect(false).assertTrue();
		});
		sleep(100);
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_006
                * @tc.name      : stop - promise
                * @tc.desc      : stop - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_006', 0,async function(done){

		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayer.start().then(async function () {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(1000);

		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
        await ringtonePlayer.stop().then(function () {
            console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_007
                * @tc.name      : release - promise
                * @tc.desc      : release - promise - Get State_RELEASED
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_007', 0,async function(done){
        console.log('AudioFrameworkTestRM: Release RingtonePlayer');
        await ringtonePlayer.release().then(function () {
            console.log('AudioFrameworkTestRM: Release RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Release RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_008
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : getSystemRingtonePlayer - promise -NEW set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_008', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,1).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:enum: get Ringtone Player : SUCCESS: ' + data.state);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:enum: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_009
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_DEFAULT enum 0
                * @tc.desc      : setSystemRingtoneUri - promise set enum 0
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_009', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',0);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,0).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum FAIL: ' + data);
                    expect(false).assertTrue();
                }
				}).catch((err) => {
                console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path :enum  ERROR: ' + err.message);
				expect(false).assertTrue();
            });
        }).catch((err) => {
            console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : enum ERROR: ' + err.message);
			expect(false).assertTrue();
            });
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_010
                * @tc.name      : configure -start-Stop - Promise -Volume=highVol ,Enable loop
                * @tc.desc      : configure -start-Stop - promise- Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_010', 0,async function(done){

        let ringtoneOptions = {
            volume:highVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol : Loop true');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            await sleep(1000);

		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		ringtonePlayer.stop().then(function () {
                console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
                expect(true).assertTrue();
            }).catch((err) => {
                console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
                expect(false).assertTrue();
            });
        }).catch((err) => {
            console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_011
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE enum
                * @tc.desc      : setSystemRingtoneUri - promise enum -1 Negative value
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_011',0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',-1).then(function () {
			console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : enum : SUCCESS: ');
			expect(false).assertTrue();
				}).catch((err) => {
					console.info('AudioFrameworkTestSSM: Ringtone SRC Path set enum : ERROR: ' + err.message);
					expect(true).assertTrue();
            });
        await Promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_012
                * @tc.name      : setSystemRingtoneUri - promise - RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : setSystemRingtoneUri - promise - Null Uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_012', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        await systemSoundManager.setSystemRingtoneUri(null,null,0).then(function () {
             console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum : SUCCESS: ' );
             expect(false).assertTrue();
		}).catch((err) => {
		console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum : ERROR: ' + err.message);
		expect(true).assertTrue();
		});
        await Promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_013
                * @tc.name      : setSystemRingtoneUri - promise - RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : setSystemRingtoneUri - promise - Invalid uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_013', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'xyz@123',0);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,0).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum SUCCESS: ' + data);
                    expect(false).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum FAIL: ' + data);
                    expect(false).assertTrue();
                }
			}).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path :enum : ERROR: ' + err.message);
				expect(true).assertTrue();
			});
			}).catch((err) => {
			console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : enum : ERROR: ' + err.message);
			expect(false).assertTrue();
            });
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_014
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_DEFAULT -ENAME
                * @tc.desc      : setSystemRingtoneUri - promise DEFAULT ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_014', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_DEFAULT);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :ENAME SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :ENAME FAIL: ' + data);
                    expect(false).assertTrue();
                }
				}).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path :ENAME : ERROR: ' + err.message);
				expect(false).assertTrue();
					});
			}).catch((err) => {
			console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : ENAME :ERROR: ' + err.message);
			expect(false).assertTrue();
            });
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_015
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : getSystemRingtonePlayer - promise -NEW
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_015', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,0).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : SUCCESS: ' + data.state);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_016
                * @tc.name      : Configure - Volume=highVol ,Disable loop start - promise
                * @tc.desc      : Configure-Start - promise -Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_016', 0,async function(done){

        let ringtoneOptions = {
            volume:highVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol : Loop False');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        ringtonePlayer.start().then(function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_017
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM -ENAME
                * @tc.desc      : setSystemRingtoneUri - promise ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_017', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_MULTISIM);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM).then(function (data) {
				if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
				{
					console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :ENAME SUCCESS: ' + data);
					expect(true).assertTrue();
				}
				else{
					console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :ENAME FAIL: ' + data);
					expect(false).assertTrue();
				}
			}).catch((err) => {
			console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path :ENAME  ERROR: ' + err.message);
			expect(false).assertTrue();
			});
			}).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : ENAME ERROR: ' + err.message);
				expect(false).assertTrue();
			});
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_018
                * @tc.name      : configure -start - promise - Volume=minVol ,Disable loop
                * @tc.desc      : Configure-Start -promise -Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_018', 0,async function(done){

        let ringtoneOptions = {
            volume:minVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : minVol : Loop False');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        ringtonePlayer.start().then(function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_019
                * @tc.name      : Stop - promise
                * @tc.desc      : Stop  - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
	it('SUB_AUDIO_MANAGER_RingtoneManager_019', 0,async function(done){

        console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
         await ringtonePlayer.stop().then(async function () {
            console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
        }).catch((err) => {
            console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_020
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE enum (2)
                * @tc.desc      : setSystemRingtoneUri - promise -enum (2) invalid value
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_020', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',2).then(function (){
			}).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone set SRC Path set: enum  ERROR: ' + err.message);
				expect(true).assertTrue();
            });
        await Promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_021
                * @tc.name      : setSystemRingtoneUri - promise - uri Null,MULTISIM ENAME
                * @tc.desc      : setSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_021', 0,async function(done){
        await systemSoundManager.setSystemRingtoneUri(null,null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM).then(function () {
			console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :Success : FAIL');
			expect(false).assertTrue();

		}).catch((err) => {
            console.info('AudioFrameworkTestSSM: Ringtone SRC Path set : ENAME ERROR: ' +err.message);
			expect(true).assertTrue();
            });
        await Promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_022
                * @tc.name      : getSystemRingtoneUri - promise - RINGTONE_TYPE enum 1
                * @tc.desc      : getSystemRingtoneUri - promise set enum 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_022', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',1);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,1).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum FAIL: ' + data);
                    expect(false).assertTrue();
                }
			}).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path set :enum : ERROR: ' + err.message);
				expect(false).assertTrue();
            });
        }).catch((err) => {
				console.info('AudioFrameworkTestSSM: Ringtone set SRC Path set:enum : ERROR: ' + err.message);
				expect(false).assertTrue();
            });
        await promise;
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_023
                * @tc.name      : Configure - promise - Volume=highVol ,Enable loop
                * @tc.desc      : Configure - promise - Get state -PREPARED
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_023', 0,async function(done){

        let ringtoneOptions = {
        volume: highVol,
        loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : Vol highVol : Loop True');
        await ringtonePlayer.configure(ringtoneOptions).then(async function() {
			await sleep(500);
			if (ringtonePlayer.state == 1){
				console.log('AudioFrameworkTestRM: Configure RingtonePlayer : State: PASS : '+ringtonePlayer.state);
				expect(true).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Configure RingtonePlayer : State: Fail : '+ringtonePlayer.state);
				expect(false).assertTrue();
			}

        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
        done();
    })
	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_024
                * @tc.name      : setSystemRingtoneUri - promise RINGTONE_TYPE_MULTISIM enum
                * @tc.desc      : setSystemRingtoneUri - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_024', 0,async function(done){
        console.info('AudioFrameworkTestSSM: Starting Set RM path');
        const promise = systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',multiSIM);
        promise.then(function () {
            systemSoundManager.getSystemRingtoneUri(null,multiSIM).then(function (data) {
                if(data == '/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav')
                {
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum SUCCESS: ' + data);
                    expect(true).assertTrue();
                }
                else{
                    console.info('AudioFrameworkTestSSM: Ringtone SRC Path set :enum FAIL: ' + data);
                    expect(false).assertTrue();
                }
				}).catch((err) => {
                console.info('AudioFrameworkTestSSM: Ringtone Get SRC Path set :enum : ERROR: ' + err.message);
				expect(false).assertTrue();
           		 });
			}).catch((err) => {
					console.info('AudioFrameworkTestSSM: Ringtone SRC Path set:  enum : ERROR: ' + err.message);
					expect(false).assertTrue();
            });
        await promise;
		await sleep(100);
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_025
                * @tc.name      : start-stop - promise Configure-highVol,Enable loop
                * @tc.desc      : start-stop - promise ,Configure- Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */

	it('SUB_AUDIO_MANAGER_RingtoneManager_025', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: true
        };

		await sleep(100);
        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : Vol highVol : Loop True');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        await ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            await sleep(2000);

			console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
			await ringtonePlayer.stop().then(async function () {
				await sleep(100);
				console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
			}).catch((err) => {
				console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			});
		}).catch((err) => {
			console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_026
                * @tc.name      : Configure minVol ,disable loop - promise - Get state (START -STOP)
                * @tc.desc      : Configure-Start-Stop -Promise -Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
	it('SUB_AUDIO_MANAGER_RingtoneManager_026', 0,async function(done){

        let ringtoneOptions = {
            volume: minVol,
            loop: false
        };
		await sleep(100);
        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : minVol : Loop false');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        await ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            await sleep(2000);

		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		await ringtonePlayer.stop().then(function () {
                console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
                expect(true).assertTrue();
            }).catch((err) => {
                console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
                expect(false).assertTrue();
            });
        }).catch((err) => {
            console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_027
                * @tc.name      : Configure Start -Promise highVol ,Disable loop
                * @tc.desc      : Configure - Start -Promise -Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_027', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: false
        };
		await sleep(100);
        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol: Loop false');
        await ringtonePlayer.configure(ringtoneOptions);
		await sleep(100);
        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayer.start().then(async function () {
			await sleep(2000);
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
			}).catch((err) => {
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			});
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_028
                * @tc.name      : getSystemRingtonePlayer - promise RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : getSystemRingtonePlayer - promise - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_028', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM: ENAME: get Ringtone Player : SUCCESS: ' + data.state);
				expect(true).assertTrue();
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_029
                * @tc.name      : Configure - lowVol ,Disable loop- promise
                * @tc.desc      : Configure - Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_029', 0,async function(done){

        let ringtoneOptions = {
            volume: lowVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : lowVol: Loop False');
        await ringtonePlayer.configure(ringtoneOptions).then(function() {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer :ERROR: '+err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_030
                * @tc.name      : Configure - promise outOfRangeVol ,Disable loop
                * @tc.desc      : Configure - promise Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_030', 0,async function(done){

        let ringtoneOptions = {
            volume:outOfRangeVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : Vol outOfRangeVol : Loop False');
        await ringtonePlayer.configure(ringtoneOptions).then(function() {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS :FAIL '+ringtonePlayer.state);
			expect(false).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer :ERROR: '+err.message);
            expect(volErrorMesg).assertEqual(err.message);
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_031
                * @tc.name      : Configure - promise outOfRangeVol ,Enable loop
                * @tc.desc      : Configure - promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_031', 0,async function(done){

        let ringtoneOptions = {
            volume:outOfRangeVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : Vol outOfRangeVol : Loop True');
        await ringtonePlayer.configure(ringtoneOptions).then(function() {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            expect(false).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Configure RingtonePlayer :ERROR: '+err.message);
            expect(volErrorMesg).assertEqual(err.message);
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_032
                * @tc.name      : Configure - Start-Stop-Start - promise minVol ,Enable loop
                * @tc.desc      : Configure - (Start-Stop-Start) promise - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_032', 0,async function(done){

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        await ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            await sleep(2000);

			console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
			await ringtonePlayer.stop().then(async function () {
				console.log('AudioFrameworkTestRM: Stop RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
				await sleep(1000);

				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				await ringtonePlayer.start().then(async function () {
					console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
					expect(true).assertTrue();
					await sleep(1000);
					}).catch((err) => {
						console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					});
				}).catch((err) => {
					console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
					expect(false).assertTrue();
				});
			}).catch((err) => {
			console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_033
                * @tc.name      : Stop -Promise -STOPPED
                * @tc.desc      : Stop-Promise-STOPPED
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_033', 0,async function(done){
		console.log('AudioFrameworkTestRM: STOP RingtonePlayer '+ringtonePlayer.state);
		await ringtonePlayer.stop().then(function(){
			console.log('AudioFrameworkTestRM: Get RingtonePlayer State :SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
		}).catch((err) => {
            console.log('AudioFrameworkTestRM: Get RingtonePlayer State :ERROR: '+err.message);
			expect(false).assertTrue();
        });
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_034
                * @tc.name      : start - promise - Without Configure
                * @tc.desc      : Start -Promise - Get State - Without configure
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_034', 0,async function(done){
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayer.start().then(async function () {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(true).assertTrue();
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(2000);
		done();
    })

	/* *
                    * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_035
                    * @tc.name      : Configure -maxVol ,enable loop -start
                    * @tc.desc      : Configure-Start-Promise (Start in running state)
                    * @tc.size      : MEDIUM
                    * @tc.type      : Function
                    * @tc.level     : Level 0
                */
    it('SUB_AUDIO_MANAGER_RingtoneManager_035', 0,async function(done){

        let ringtoneOptions = {
            volume:maxVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : maxVol : Loop True');
        await ringtonePlayer.configure(ringtoneOptions);

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        ringtonePlayer.start().then(function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            expect(false).assertTrue();

        }).catch((err) => {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
            expect(true).assertTrue();
        });
        done();
		await sleep(1000);
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_036
                * @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE_DEFAULT
                * @tc.desc      : setSystemRingtoneUri - Callback ENAME
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_036', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_DEFAULT,(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT, async(err,data)=>{
					if (err){
						console.error('AudioFrameworkTestRM :Callback: Failed to get system Ringtone Uri: ERROR '+err.message);
						expect(false).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav') {
						console.info('AudioFrameworkTestRM :Callback: Get system Ringtone Uri : SUCCESS');
						expect(true).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
					await sleep(1000);
					done();
				});
            }
        });
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_037
                * @tc.name      : getSystemRingtonePlayer -Callback RINGTONE_TYPE_DEFAULT
                * @tc.desc      : getSystemRingtonePlayer -Callback enum
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_037', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,0,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_038
                * @tc.name      : Configure -maxVol ,Disable loop - Callback -Get state -PREPARED
                * @tc.desc      : Configure -Callback - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_038', 0,async function(done){

        let ringtoneOptions = {
            volume: maxVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : lowVol: Loop False');
        ringtonePlayer.configure(ringtoneOptions,async(err)=> {
			if (err){
				console.error('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : ERROR:' + err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : SUCCESS');
				await sleep(500)
				if (ringtonePlayer.state == 1){
					console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : PREPARED:PASS: State : '+ringtonePlayer.state);
					expect(true).assertTrue();
				}
				else{
					console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : PREPARED:FAIL: State : '+ringtonePlayer.state);
					expect(false).assertTrue();
				}
			}
		});
		await sleep(1000);
		done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_039
                * @tc.name      : Configure - maxVol ,enable loop Callback - START
                * @tc.desc      : Configure-Start-Callback - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_039', 0,async function(done){

		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async (err) => {
			await sleep(2000);
			if (err){
				console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				if (ringtonePlayer.state == 2){
					console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : START:PASS: State : '+ringtonePlayer.state);
					expect(true).assertTrue();
				}
				else{
					console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : START:FAIL: State : '+ringtonePlayer.state);
					expect(false).assertTrue();
				}
				await sleep(1000);
				console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
				await ringtonePlayer.stop();
			}
			done();
		});
		await sleep(1000);
		done();
	});


	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_040
				* @tc.name      : STOP Callback -Get state
                * @tc.desc      : STOP -Callback -STOPPED
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_040', 0,async function(done){

		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async (err) => {
			await sleep(2000);
			if (err){
				console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
			}
			await sleep(1000);
			console.log('AudioFrameworkTestRM: STOP RingtonePlayer');
			ringtonePlayer.stop(async (err) => {
				await sleep(1000);
				if (err){
					console.error('AudioFrameworkTestRM: STOP RingtonePlayer :ERROR: '+err.message);
					expect(false).assertTrue();
				}
				else{
					console.log('AudioFrameworkTestRM: STOP RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
					if (ringtonePlayer.state == 3){
						console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : STOP:PASS: State : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM:Callback: Configure RingtonePlayer : STOP:FAIL: State : '+ringtonePlayer.state);
						expect(false).assertTrue();
					}
				}
			done();
			});
		});
		await sleep(2000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_041
				* @tc.name      : RELEASE Callback -Get state
                * @tc.desc      : RELEASE - Callback -RELEASED
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_041', 0,async function(done){
        console.log('AudioFrameworkTestRM: RingtonePlayer - Release');
        await ringtonePlayer.release(async(err) => {
			if(err){
				console.error('AudioFrameworkTestRM: Release RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Release RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
			}
			await sleep(1000);
			done();
		});
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_042
                * @tc.name      : getSystemRingtonePlayer -Callback  RINGTONE_TYPE_MULTISIM
                * @tc.desc      : getSystemRingtonePlayer -Callback -NEW- enum
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_042', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,1,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_043
                * @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE_DEFAULT - enum
                * @tc.desc      : setSystemRingtoneUri - Callback - enum
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_043', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',0,(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				systemSoundManager.getSystemRingtoneUri(null,0,async(err,data) => {
					if (err){
						console.error('AudioFrameworkTestRM :Callback: enum: Get system Ringtone Uri: ERROR '+err.message);
						expect(false).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav') {
						console.info('AudioFrameworkTestRM :Callback:enum: Get system Ringtone Uri : SUCCESS');
						expect(true).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: enum: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
					await sleep(100);
					done();
				});
            }
        });
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_044
                * @tc.name      : Configure-highVol,Enable loop- Callback -Get state -(START -STOP)
                * @tc.desc      : Configure-START-STOP - Callback - Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_044', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol: Loop False');
        ringtonePlayer.configure(ringtoneOptions);

		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				await sleep(1000);
				console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
				ringtonePlayer.stop((err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback: Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
					done();
				});
			}
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_045
                * @tc.name      : setSystemRingtoneUri - Callback  RINGTONE_TYPE_DEFAULT - enum - Negative value
                * @tc.desc      : setSystemRingtoneUri - Callback enum - Negative value
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_045', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',-1,async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: enum: Get system Ringtone Uri : FAIL');
				expect(false).assertTrue();
			}
			await sleep(1000);
			done();
		});
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_046
                * @tc.name      : setSystemRingtoneUri - Callback- Null uri -RINGTONE_TYPE_DEFAULT
                * @tc.desc      : setSystemRingtoneUri - Callback set null uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_046', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT,async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				expect(false).assertTrue();
				}
			await sleep(1000);
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_047
                * @tc.name      : getSystemRingtonePlayer -Callback Get State RINGTONE_TYPE_MULTISIM
                * @tc.desc      : getSystemRingtonePlayer -Callback- enum Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_047', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,1,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_048
                * @tc.name      : getSystemRingtonePlayer -Callback Get State RINGTONE_TYPE_DEFAULT
                * @tc.desc      : getSystemRingtonePlayer -Callback- enum Get State
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_048', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,0,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_049
                * @tc.name      : getSystemRingtonePlayer -Callback Get State -NEW RINGTONE_TYPE_MULTISIM ENAME
                * @tc.desc      : getSystemRingtonePlayer -Callback -NEW
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_049', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : ERROR :' + err.message);
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_050
                * @tc.name      : Configure minVol ,Disable loop - Start - callback
                * @tc.desc      : Configure - Start - callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_050', 0,async function(done){

        let ringtoneOptions = {
            volume: minVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : minVol: Loop False');
        ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player');
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer SUCCESS');

				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
                    await sleep(1000);
                    done();
				});
			}
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_051
                * @tc.name      : Stop - Callback -Get state
                * @tc.desc      : Stop - Callback Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_051', 0,async function(done){
		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		ringtonePlayer.stop(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
				}
			await sleep(1000);
			done();
		});
	});

			/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_052
                * @tc.name      : setSystemRingtoneUri - Callback  RINGTONE_TYPE enum 2
                * @tc.desc      : setSystemRingtoneUri - Callback Ename
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_052', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',2,async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback:enum: Set system Ringtone Uri : SUCCESS');
				expect(false).assertTrue();
			}
			await sleep(1000);
			done();
		});
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_053
                * @tc.name      : setSystemRingtoneUri - Callback null RINGTONE_TYPE_MULTISIM -ENAME
                * @tc.desc      : setSystemRingtoneUri - Callback set null uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_053', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback :ENAME: Failed to set system ringtone '+err.message);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: ENAME: Get system Ringtone Uri : SUCCESS');
				expect(false).assertTrue();
				}
			await sleep(1000);
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_054
                * @tc.name      : setSystemRingtoneUri - Callback Invalid RINGTONE_TYPE_MULTISIM
                * @tc.desc      : setSystemRingtoneUri - Callback set Invalid uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_054', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'xyz123',audio.RingtoneType.RINGTONE_TYPE_MULTISIM,(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async(err,data) => {
					if (err){
						console.error('AudioFrameworkTestRM :Callback: Failed to get system Ringtone Uri: ERROR '+err.message);
						expect(true).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav') {
						console.info('AudioFrameworkTestRM :Callback: Get system Ringtone Uri : PASS');
						expect(false).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
					await sleep(1000);
					done();
				});
            }
            done();
        });
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_055
                * @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE enum 1
                * @tc.desc      : setSystemRingtoneUri - Callback set enum = 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_055', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',1,(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				systemSoundManager.getSystemRingtoneUri(null,1,async(err,data) => {
					if (err){
						console.error('AudioFrameworkTestRM :Callback: enum: Failed to get system Ringtone Uri: ERROR '+err.message);
						expect(false).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav'){
						console.info('AudioFrameworkTestRM :Callback:enum: Get system Ringtone Uri : SUCCESS');
						expect(true).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: enum: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
                    await sleep(100);
					done();
				});
            }
			done();
        });
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_056
                * @tc.name      : Configure - minVol ,Enable loop  - Callback
                * @tc.desc      : Configure - Callback -Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_056', 0,async function(done){

        let ringtoneOptions = {
            volume: minVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : minVol: Loop true');
        ringtonePlayer.configure(ringtoneOptions,async(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player');
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer SUCCESS');
				expect(true).assertTrue();
			}
			await sleep(1000);
            done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_057
                * @tc.name      : getSystemRingtonePlayer -Callback Get State -NEW RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : getSystemRingtonePlayer -Callback Get State -NEW
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_057', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :enum: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_058
                * @tc.name      : Configure highVol ,Disable loop Start - Callback
                * @tc.desc      : Configure - Start - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_058', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol: Loop false');
        ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player');
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer SUCCESS');

				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
                    await sleep(1000);
					await ringtonePlayer.stop();
                    done();
				});
			}
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_059
                * @tc.name      : Configure - highVol ,Enable loop - start Callback
                * @tc.desc      : Configure - start - callback -
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_059', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol: Loop true');
        ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player');
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer SUCCESS');

				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
                    await sleep(1000);
					await ringtonePlayer.stop();
                    done();
				});
			}
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_060
                * @tc.name      : getSystemRingtonePlayer -Callback Get State -NEW RINGTONE_TYPE_DEFAULT ENAME
                * @tc.desc      : getSystemRingtonePlayer -Callback Get State -NEW
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_060', 0,async function(done){
		console.log('AudioFrameworkTestRM: get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT,async(err,data)=> {
			ringtonePlayer = data;
			if(err){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : ERROR :' + (err.message));
				expect(false).assertTrue();
			}
			else if(ringtonePlayer!= null){
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Pass: '+data.state);
				expect(true).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestSSM:Callback :ENAME: get Ringtone Player : Fail: '+data.state);
				expect(false).assertTrue();
			}
			await sleep(1000);
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_061
                * @tc.name      : Configure outOfRangeVol ,Enable loop -Get STATE- Callback
                * @tc.desc      : Configure - Start -Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_061', 0,async function(done){

        let ringtoneOptions = {
            volume: outOfRangeVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : outOfRangeVol: Loop true');
        ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player' +err.message);
				expect(volErrorMesg).assertEqual(err.message);
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer Out Of Range SUCCESS : FAIL');
				expect(false).assertTrue();
			}
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_062
                * @tc.name      : Configure outOfRangeVol ,disable loop-Get STATE- Callback
                * @tc.desc      : Configure - Start - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_062', 0,async function(done){

        let ringtoneOptions = {
            volume: outOfRangeVol,
            loop: false
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : outOfRangeVol: Loop false');
		ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player' +err.message);
				expect(volErrorMesg).assertEqual(err.message);
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer Out Of Range SUCCESS : FAIL');
				expect(false).assertTrue();
			}
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_063
                * @tc.name      : setSystemRingtoneUri - Callback -RINGTONE_TYPE_MULTISIM -ENAME
                * @tc.desc      : setSystemRingtoneUri - Callback -Ename
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_063', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_MULTISIM,async (err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback :ENAME: Failed to set system ringtone '+err.message);
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');
				systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM, async(err,data) => {
					if (err){
						console.error('AudioFrameworkTestRM :Callback: ENAME: Failed to get system Ringtone Uri: ERROR '+err.message);
						expect(false).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav') {
						console.info('AudioFrameworkTestRM :Callback: ENAME: Get system Ringtone Uri : PASS');
						expect(true).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: ENAME: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
                    await sleep(1000);
					done();
				});
            }
			await sleep(1000);
			done();
        });
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_064
                * @tc.name      : Configure - Callback - Get state (Start-Stop-Start)
                * @tc.desc      : Volume=minVol ,Enable loop
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_064', 0,async function(done){

		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
			}
			await sleep(2000);
			console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
			ringtonePlayer.stop(async(err) => {
				if (err){
					console.error('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :ERROR: '+err.message);
					expect(false).assertTrue();
				}
				else{
					console.log('AudioFrameworkTestRM: Callback: Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				}
				await sleep(1000);
				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Start1 RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
					await sleep(1000);
				});
			});
		});
		await sleep(6000)
		done();
	})
	await sleep(1000);
	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_065
                * @tc.name      : Get State Stop -STOPPED
                * @tc.desc      : Stop - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_065', 0,async function(done){

            console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
            ringtonePlayer.stop(async(err) => {
                if (err){
                    console.error('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :ERROR: '+err.message);
                    expect(false).assertTrue();
                }
                else{
                    console.log('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
                    expect(true).assertTrue();
                }
				await sleep(1000);
                done();
            });
		})
	await sleep(1000);
	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_066
                * @tc.name      : start - Callback without configure
                * @tc.desc      : START - Callback - without configure - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_066', 0,async function(done){
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer without configure :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer without configure :SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
			}
			await sleep(2000);
			done();
		});
	})
	await sleep(1000);
	/* *
				* @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_067
				* @tc.name      : Configure maxVol ,enable loop - start - Callback
				* @tc.desc      : Configure - Start - Callback (Start when in running)
				* @tc.size      : MEDIUM
				* @tc.type      : Function
				* @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_067', 0,async function(done){

        let ringtoneOptions = {
            volume: highVol,
            loop: true
        };

        console.log('AudioFrameworkTestRM: Configure RingtonePlayer : highVol: Loop true');
        ringtonePlayer.configure(ringtoneOptions,(err) => {
			if(err){
				console.error('AudioFrameworkTestRM :Callback : Failed to Configure Ringtone Player');
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback : Configure RingtonePlayer SUCCESS');

				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(true).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(false).assertTrue();
					}
                    await sleep(1000);
					await ringtonePlayer.stop();
					done();
				});
			}
			done();
		});
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_068
                * @tc.name      : Start - stop promise - (START -STOP)
                * @tc.desc      : Start - Stop - Promise
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_068', 0,async function(done){

        console.log('AudioFrameworkTestRM: Start RingtonePlayer');
        ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
            await sleep(1000);
				ringtonePlayer.stop().then(function () {
				console.log('AudioFrameworkTestRM : Stop RingtonePlayer : SUCCESS :'+ringtonePlayer.state);
				expect(true).assertTrue();
			}).catch((err) => {
				console.log('AudioFrameworkTestRM : Stop RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}).catch((err) => {
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			});
			done();
		});
    })

    /* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_069
                * @tc.name      : Start -Stop - Callback -Get state -(START -STOP)
                * @tc.desc      : Start -Stop - Callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_069', 0,async function(done){
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		ringtonePlayer.start(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Callback: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
			await sleep(1000);
				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.stop(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(true).assertTrue();
					}
					await sleep(1000);
					done();
				});
			}
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_070
                * @tc.name      : Get State Stop promise
                * @tc.desc      : Stop - promise (negative scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_070', 0,async function(done){

            console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
            await ringtonePlayer.stop().then(async function() {
				console.error('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
                }).catch((err) => {
                    console.log('AudioFrameworkTestRM: Callback : Stop RingtonePlayer :error :'+err.message);
					expect(false).assertTrue();
			});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_071
                * @tc.name      : Stop-Callback - Get state
                * @tc.desc      : Stop- Callback (Negative scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_071', 0,async function(done){
		console.log('AudioFrameworkTestRM: stop RingtonePlayer');
		ringtonePlayer.stop(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Stop RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Callback: Stop RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				expect(true).assertTrue();
			}
			await sleep(4000);
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_072
                * @tc.name      : Release - Start - promise - Get state (Release -Start)
                * @tc.desc      : Release - Start - promise - Get state (Negative Scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_072', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS');
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' +err.message);
			expect(false).assertTrue();
		});

        console.log('AudioFrameworkTestRM: Release RingtonePlayer');
        await ringtonePlayer.release().then(async function () {
            console.log('AudioFrameworkTestRM: Release RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			await sleep(2000);
				await ringtonePlayer.start().then(function () {
					console.log('AudioFrameworkTestRM : Start RingtonePlayer : SUCCESS :'+ringtonePlayer.state);
					expect(false).assertTrue();
				}).catch((err) => {
					console.log('AudioFrameworkTestRM : Start RingtonePlayer :ERROR: '+err.message);
					expect(true).assertTrue();
			}).catch((err) => {
				console.log('AudioFrameworkTestRM: Release RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			});
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_073
                * @tc.name      : Release - Start - Callback - Get state (Release -Start)
                * @tc.desc      : Release - Start - Callback - Get state (Negative Scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
        */
    it('SUB_AUDIO_MANAGER_RingtoneManager_073', 0,async function(done){
		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayer = data;
			if (ringtonePlayer !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		await sleep(100);
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		ringtonePlayer.release(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Release RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Callback: release RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
				await sleep(1000);
				console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
						expect(true).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(false).assertTrue();
					}
					await sleep(1000);
					done();
				});
			}
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_074
                * @tc.name      : setSystemRingtoneUri - Promise RINGTONE_TYPE_ enum 1
                * @tc.desc      : setSystemRingtoneUri - Promise set enum value 1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_074', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',1).then(function(){
			console.info('AudioFrameworkTestSSM:enum:1:Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum:1:Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_075
                * @tc.name      : getRendererInfo()- promise -  Content
                * @tc.desc      : getRendererInfo()- promise - Content Get Renderer info
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_075', 0,async function(done){
        await ringtonePlayer.getAudioRendererInfo().then(function(data){
        console.info('AudioFrameworkTestSSM : Ringtone info content : '+data.content);
           expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM : Ringtone info content: '+err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_076
                * @tc.name      : getRendererInfo()- Get Renderer info- callback
                * @tc.desc      : getRendererInfo()- Content - callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_076', 0,async function(done){
        ringtonePlayer.getAudioRendererInfo(async(err,data) => {
            if(err){
                console.error('AudioFrameworkTestRM : Ringtone info content :ERROR: '+err.message);
                expect(false).assertTrue();
            }
            else if (data!==null){
             	console.log('AudioFrameworkTestRM : Ringtone info content: Pass :'+data.content);
             	expect(true).assertTrue();
            }
            else{
             	expect(false).assertTrue();
			 	await sleep(100);
            }
        })
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_077
                * @tc.name      : getRendererInfo()- promise - Get Renderer info - usage
                * @tc.desc      : getRendererInfo()- promise - Usage
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_077', 0,async function(done){
        await ringtonePlayer.getAudioRendererInfo().then(function(data){
        console.info('AudioFrameworkTestSSM : Ringtone info usage : '+data.usage);
           expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM : Ringtone info usage: '+err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_078
                * @tc.name      : getRendererInfo()- Get Renderer info- callback -Usage
                * @tc.desc      : getRendererInfo()- Usage - callback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_078', 0,async function(done){
        ringtonePlayer.getAudioRendererInfo(async(err,data) => {
            if(err){
                console.error('AudioFrameworkTestRM : Ringtone info usage :ERROR: '+err.message);
                expect(false).assertTrue();
            }
            else if (data!==null){
				console.log('AudioFrameworkTestRM : Ringtone info usage: Pass :'+data.content);
				expect(true).assertTrue();
            }
            else{
             	expect(false).assertTrue();
			 	await sleep(100);
            }
        })
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_079
                * @tc.name      : getRendererInfo()- promise - Get Renderer info - RendererFlags
                * @tc.desc      : getRendererInfo()- promise - RendererFlags
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_079', 0,async function(done){
        await ringtonePlayer.getAudioRendererInfo().then(function(data){
        	console.info('AudioFrameworkTestRM : Ringtone info rendererFlags : '+data.rendererFlags);
           	expect(true).assertTrue();
        }).catch((err) => {
            console.log('AudioFrameworkTestRM : Ringtone info rendererFlags : '+err.message);
            expect(false).assertTrue();
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_080
                * @tc.name      : getRendererInfo()- Get Renderer info- callback -RendererFlags
                * @tc.desc      : getRendererInfo()- Callback - RendererFlags
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_080', 0,async function(done){
        ringtonePlayer.getAudioRendererInfo(async(err,data) => {
            if(err){
                console.error('AudioFrameworkTestRM : Ringtone info rendererFlags :ERROR: '+err.message);
                expect(false).assertTrue();
            }
            else if (data!==null){
			 	console.log('AudioFrameworkTestRM : Ringtone info rendererFlags: Pass :'+data.rendererFlags);
			 	expect(true).assertTrue();
            }
            else{
             	expect(false).assertTrue();
			 	await sleep(100);
            }
        });
        done();
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_083
                * @tc.name      : setSystemRingtoneUri - Promise invalid uri RINGTONE_TYPE_DEFAULT enum 0
                * @tc.desc      : setSystemRingtoneUri - Promise  set invalid uri - Start() (Negative Scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_083', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'data',0).then(async function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');

			await ringtonePlayer.start().then(async function () {
				console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
				expect(false).assertTrue();
			}).catch((err) => {
				console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
				expect(true).assertTrue();
			});
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM: enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(1000);
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_084
                * @tc.name      : setSystemRingtoneUri - Promise RINGTONE_TYPE_ enum negative value
                * @tc.desc      : setSystemRingtoneUri - Promise set negative enum value -1
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_084', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',-1).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Negative Value :Set Ringtone Uri : SUCCESS: ');

			ringtonePlayer.start().then(async function () {
            console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayer.state);
			expect(false).assertTrue();
				}).catch((err) => {
					console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
					expect(false).assertTrue();
				});
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum:Negative Value :Set System Ringtone Uri :ERROR: '+err.message);
			expect(true).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_085
                * @tc.name      : setSystemRingtoneUri - Callback
                * @tc.desc      : setSystemRingtoneUri - Callback enum negative value
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_085', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',-1,(err) => {
            if(err) {
			console.error('AudioFrameworkTestRM: Callback :enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(true).assertTrue();
		}
		else{
			console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(false).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(false).assertTrue();
					}
					await sleep(1000);
				});
			}
			done();
		})
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_085
                * @tc.name      : setSystemRingtoneUri - Promise
                * @tc.desc      : setSystemRingtoneUri - Promise enum negative value
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
	it('SUB_AUDIO_MANAGER_RingtoneManager_085', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',-1).then(function () {
			console.log('AudioFrameworkTestRM: Set System Ringtone Uri : SUCCESS');
			expect(false).assertTrue();
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: Ringtone set SRC Path : ERROR: ' + err.message);
			expect(true).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_086
				* @tc.name      : setSystemRingtoneUri - Callback RINGTONE_TYPE_DEFAULT enum
                * @tc.desc      : setSystemRingtoneUri - Callback invalid uri
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_086', 0,async function(done){
		console.log('AudioFrameworkTestRM: Set System Ringtone Uri');
        systemSoundManager.setSystemRingtoneUri(null,'xyz@123',0,(err) => {
            if(err) {
			console.error('AudioFrameworkTestSSM: Callback :enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		}
		else{
			console.info('AudioFrameworkTestSSM:Callback :enum: Set Ringtone Uri : SUCCESS: ');
			console.log('AudioFrameworkTestRM: Start RingtonePlayer');
				ringtonePlayer.start(async(err) => {
					if (err){
						console.error('AudioFrameworkTestRM: Callback : Start RingtonePlayer :ERROR: '+err.message);
						expect(true).assertTrue();
					}
					else{
						console.log('AudioFrameworkTestRM: Callback : Start RingtonePlayer :SUCCESS : '+ringtonePlayer.state);
						expect(false).assertTrue();
					}
					await sleep(1000);
				});
			}
			done();
		});
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_087
                * @tc.name      : getSystemRingtoneUri - Callback  RINGTONE_TYPE_MULTISIM  ENAME
                * @tc.desc      : getSystemRingtoneUri - Callback Ename (negative scenario)
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_087', 0,async function(done){
        systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',audio.RingtoneType.RINGTONE_TYPE_MULTISIM,(err) => {
			if (err){
				console.error('AudioFrameworkTestRM :Callback : Failed to set system ringtone '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.info('AudioFrameworkTestRM :Callback: Set system Ringtone SUCCESS');

				systemSoundManager.getSystemRingtoneUri(null,-1,async(err,data) => {
					if (err){
						console.error('AudioFrameworkTestRM :Callback: enum: Failed to get system Ringtone Uri: ERROR '+err.message);
						expect(true).assertTrue();
					}
					else if(data=='/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav') {
						console.info('AudioFrameworkTestRM :Callback:enum: Get system Ringtone Uri : SUCCESS');
						expect(false).assertTrue();
					}
					else{
						console.info('AudioFrameworkTestRM :Callback: enum: Get system Ringtone Uri : FAIL');
						expect(false).assertTrue();
					}
					await sleep(1000);
					done();
				});
            }
			done();
        });
    })

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_088
                * @tc.name      : setSystemRingtoneUri - Promise RINGTONE_TYPE_ enum 0
                * @tc.desc      : setSystemRingtoneUri - Promise set enum value 0
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
            */
    it('SUB_AUDIO_MANAGER_RingtoneManager_088', 0,async function(done){
		console.log('AudioFrameworkTestSSMM: Set System Ringtone Uri');
        await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/StarWars10s-1C-44100-2SW.wav',0).then(function(){
			console.info('AudioFrameworkTestSSM:enum:0:Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum:0:Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_089
                * @tc.name      : Configure- Stop-Release  Callback
                * @tc.desc      : Configure -Stop-Release -Callback - Get state
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
    it('SUB_AUDIO_MANAGER_RingtoneManager_089', 0,async function(done){
		var ringtonePlayerTemp;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_MULTISIM).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});

		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		ringtonePlayerTemp.release(async(err) => {
			if (err){
				console.error('AudioFrameworkTestRM: Callback : Release RingtonePlayer :ERROR: '+err.message);
				expect(false).assertTrue();
			}
			else{
				console.log('AudioFrameworkTestRM: Callback: release RingtonePlayer :SUCCESS');

				if (ringtonePlayerTemp.state == 4) {
					console.log('AudioFrameworkTestRM: Callback: Player Status : PASS : '+ringtonePlayerTemp.state);
					expect(true).assertTrue();
				}
				else{
					console.log('AudioFrameworkTestRM: Callback: Player Status : FAIL : '+ringtonePlayerTemp.state);
					expect(false).assertTrue();
				}
			}
			await sleep(1000);
			done();
		});

		done();
	})

		/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_090
                * @tc.name      : Rintgtone-Player - mp3
                * @tc.desc      : Configure start-Stop-Release
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_090', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS');
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' +err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp3',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp3') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		let ringtoneOptions = {
			volume: maxVol,
			loop: true
		};
		console.log('AudioFrameworkTestRM: Configure RingtonePlayer : maxVol: Loop true');
		await ringtonePlayerTemp.configure(ringtoneOptions).then(function() {
			console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : ' +ringtonePlayerTemp.state);
		}).catch((err) => {
			console.info('AudioFrameworkTestRM: Configure RingtonePlayer : FAIL:' + err.message);
			expect(false).assertTrue();
		});
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayerTemp.start().then(async function () {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayerTemp.state);
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(2000);

		if (ringtonePlayerTemp.state == 2) {
			console.log('AudioFrameworkTestRM: Player Status RUNNING: PASS : '+ringtonePlayerTemp.state);
			resultFlag = true;
		}
		else{
			console.log('AudioFrameworkTestRM: Player Status RUNNING: FAIL : '+ringtonePlayerTemp.state);
			resultFlag = false;
		}
		await sleep(1000);

		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		await ringtonePlayerTemp.stop();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		await sleep(1000);
		expect(resultFlag).assertTrue();

		done();
	});

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_091
                * @tc.name      : Rintgtone-Player - mp4
                * @tc.desc      : Configure start-Stop-Release
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_091', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS');
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' +err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp4',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp4') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		let ringtoneOptions = {
			volume: maxVol,
			loop: true
		};
		console.log('AudioFrameworkTestRM: Configure RingtonePlayer : maxVol: Loop true');
		await ringtonePlayerTemp.configure(ringtoneOptions).then(function() {
			console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : ' +ringtonePlayerTemp.state);
		}).catch((err) => {
			console.info('AudioFrameworkTestRM: Configure RingtonePlayer : FAIL:' + err.message);
			expect(false).assertTrue();
		});
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayerTemp.start().then(async function () {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayerTemp.state);
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(2000);

		if (ringtonePlayerTemp.state == 2) {
			console.log('AudioFrameworkTestRM: Player Status RUNNING: PASS : '+ringtonePlayerTemp.state);
			resultFlag = true;
		}
		else{
			console.log('AudioFrameworkTestRM: Player Status RUNNING: FAIL : '+ringtonePlayerTemp.state);
			resultFlag = false;
		}
		await sleep(1000);

		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		await ringtonePlayerTemp.stop();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		await sleep(1000);
		expect(resultFlag).assertTrue();

		done();
	})

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_092
                * @tc.name      : Ringtone-Player - mp3 -Get Title
                * @tc.desc      : Configure & get title during playback
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_092', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp3',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp3') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		let ringtoneOptions = {
			volume: maxVol,
			loop: true
		};
		console.log('AudioFrameworkTestRM: Configure RingtonePlayer : maxVol: Loop true');
		await ringtonePlayerTemp.configure(ringtoneOptions).then(function() {
			console.log('AudioFrameworkTestRM: Configure RingtonePlayer : SUCCESS : ' +ringtonePlayerTemp.state);
		}).catch((err) => {
			console.info('AudioFrameworkTestRM: Configure RingtonePlayer : FAIL:' + err.message);
			expect(false).assertTrue();
		});
		console.log('AudioFrameworkTestRM: Start RingtonePlayer');
		await ringtonePlayerTemp.start().then(async function () {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer : SUCCESS : '+ringtonePlayerTemp.state);
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: Start RingtonePlayer :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await sleep(2000);

		await ringtonePlayerTemp.getTitle().then(async function (data) {
			console.log('AudioFrameworkTestRM: RingtonePlayer : STATE : '+ringtonePlayerTemp.state);
			if (data == 'Test MP3') {
				console.log('AudioFrameworkTestRM: TITLE : PASS : '+data);
				resultFlag = true;
			}
			else{
				console.log('AudioFrameworkTestRM: TITLE : FAIL : '+data);
				resultFlag = false;
			}
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: getTitle :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		//		await sleep(1000);
		console.log('AudioFrameworkTestRM: Stop RingtonePlayer');
		await ringtonePlayerTemp.stop();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		await sleep(1000);
		expect(resultFlag).assertTrue();
		done();
	});

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_093
                * @tc.name      : Ringtone-Player - mp3 -Get Title
                * @tc.desc      : Configure & get title
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_093', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp3',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp3') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		await ringtonePlayerTemp.getTitle().then(async function (data) {
			console.log('AudioFrameworkTestRM: RingtonePlayer : STATE : '+ringtonePlayerTemp.state);
			if (data == 'Test MP3') {
				console.log('AudioFrameworkTestRM: TITLE : PASS : '+data);
				resultFlag = true;
			}
			else{
				console.log('AudioFrameworkTestRM: TITLE : FAIL : '+data);
				resultFlag = false;
			}
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: getTitle :ERROR: '+err.message);
			expect(false).assertTrue();
		});

		await sleep(100);
		expect(resultFlag).assertTrue();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		done();

	});

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_094
                * @tc.name      : Ringtone-Player - mp4 -Get Title
                * @tc.desc      : Configure & get title
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_094', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp4',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp4') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		await ringtonePlayerTemp.getTitle().then(async function (data) {
			console.log('AudioFrameworkTestRM: RingtonePlayer : STATE : '+ringtonePlayerTemp.state);
			if (data == 'Galway') {
				console.log('AudioFrameworkTestRM: TITLE : PASS : '+data);
				resultFlag = true;
			}
			else{
				console.log('AudioFrameworkTestRM: TITLE : FAIL : '+data);
				resultFlag = false;
			}
		}).catch((err) => {
			console.log('AudioFrameworkTestRM: getTitle :ERROR: '+err.message);
			expect(false).assertTrue();
		});

		await sleep(100);
		expect(resultFlag).assertTrue();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		done();
	});

	/* *
                * @tc.number    : SUB_AUDIO_MANAGER_RingtoneManager_095
                * @tc.name      : Ringtone-Player - mp4 -Get Title - Callback
                * @tc.desc      : Configure & get title
                * @tc.size      : MEDIUM
                * @tc.type      : Function
                * @tc.level     : Level 0
			*/
	it('SUB_AUDIO_MANAGER_RingtoneManager_095', 0,async function(done){
		var ringtonePlayerTemp;
		var resultFlag;

		console.log('AudioFrameworkTestRM: Get System Ringtone Player');
		await systemSoundManager.getSystemRingtonePlayer(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(async function(data) {
			ringtonePlayerTemp = data;
			if (ringtonePlayerTemp !=null) {
				console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : SUCCESS: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM:ENAME: get Ringtone Player : ERROR :' + (err.message));
			expect(false).assertTrue();
		});
		await systemSoundManager.setSystemRingtoneUri(null,'/storage/media/100/local/files/media/test.mp4',audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function(){
			console.info('AudioFrameworkTestSSM:enum: Set Ringtone Uri : SUCCESS: ');
		}).catch((err) => {
			console.error('AudioFrameworkTestSSM:enum: Set System Ringtone Uri :ERROR: '+err.message);
			expect(false).assertTrue();
		});
		await systemSoundManager.getSystemRingtoneUri(null,audio.RingtoneType.RINGTONE_TYPE_DEFAULT).then(function (data) {
			if (data == '/storage/media/100/local/files/media/test.mp4') {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri : DEFAULT : SUCCESS: ' + data);
			}
			else {
				console.info('AudioFrameworkTestSSM: enum Get Ringtone Uri :DEFAULT : FAIL: ' + data);
			}
		}).catch((err) => {
			console.info('AudioFrameworkTestSSM: enum Ringtone Get Ringtone  ERROR: ' + err.message);
			expect(false).assertTrue();
		});

		ringtonePlayerTemp.getTitle((err, data) => {
			if (err) {
				console.error(`AudioFrameworkTestRM: getTitle : ERROR : ${err.message}`);
				expect(false).assertTrue();
			}
			else {
				if (data == 'Galway') {
					console.log('AudioFrameworkTestRM: TITLE : PASS : ' + data);
					resultFlag = true;
				}
				else {
					console.log('AudioFrameworkTestRM: TITLE : FAIL : ' + data);
					resultFlag = false;
				}
			}
		});

		await sleep(100);
		expect(resultFlag).assertTrue();
		console.log('AudioFrameworkTestRM: Release RingtonePlayer');
		await ringtonePlayerTemp.release();
		done();
	});

});





