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

import cameraObj from '@ohos.multimedia.camera';
import image from '@ohos.multimedia.image';
import fileio from '@ohos.fileio';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

const TAG = "CameraModuleTest: ";

// Define global variables

var cameraManager;
var surfaceId1;

// CAMERA-0 Variables
var camera0Input, camera0InputPosBack, camera0InputPosFront;
var camera0InputPromise, camera0InputPromisePosBack, camera0InputPromisePosFront;
// CAMERA-1 Variables
var camera1Input, camera1InputPosBack, camera1InputPosFront;
var camera1InputPromise, camera1InputPromisePosBack, camera1InputPromisePosFront;
// CAMERA-2 Variables
var camera2Input, camera2InputPosBack, camera2InputPosFront;
var camera2InputPromise, camera2InputPromisePosBack, camera2InputPromisePosFront;
// CAMERA-3 Variables
var camera3Input, camera3InputPosBack, camera3InputPosFront;
var camera3InputPromise, camera3InputPromisePosBack, camera3InputPromisePosFront;

var PermissionFlag = {
    PERMISSION_USER_SET: 1,
    PERMISSION_USER_FIXED: 2,
    PERMISSION_SYSTEM_FIXED: 3
};

const RESULT_FAIL = -1
const TIMEOUT = 1000;
const DEFAULT_PERMISSION_FALG = 0
var permissionNameUser = "ohos.permission.CAMERA";
var tokenID = undefined


export default function cameraTestCase(surfaceId:any) {

	async function getImageReceiverSurfaceId() {
    console.log(TAG + 'Entering create Image receiver')
    var receiver = image.createImageReceiver(640, 480, 4, 8)
    console.log(TAG + 'before receiver check')
    if (receiver !== undefined) {
      console.log(TAG + 'Receiver is ok')
      surfaceId1 = await receiver.getReceivingSurfaceId()
      console.log(TAG + 'Received id: ' + JSON.stringify(surfaceId1))
    } else {
      console.log(TAG + 'Receiver is not ok')
    }
  }

describe('Enum-Verification', function () {
    var camerasArray;

    console.log('##########start AccessTokenTests');
    beforeAll(async function (done){
        var appInfo = await bundle.getApplicationInfo('com.example.cameramoduletestetsv2',0 ,100);
        tokenID = appInfo.accessTokenId;
        console.info(TAG + "AccessTokenTest accessTokenId:" + appInfo.accessTokenId + ", name:"+ appInfo.name
			+ ", bundleName:" + appInfo.bundleName)
        sleep(TIMEOUT);
        console.info(TAG + "Grant permission start");
        var atManager = abilityAccessCtrl.createAtManager();
        var result = await atManager.grantUserGrantedPermission(tokenID, permissionNameUser,
			PermissionFlag.PERMISSION_USER_FIXED); 
        console.info(TAG + "Grant permission finish, result:" + result + ", TokenID:" + tokenID);
        done();
    })
    afterEach(function(){
    })
	
       function sleep(ms) {
        console.info(TAG + "Entering sleep -> Promise constructor");
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    /**
      * @tc.number    : GET_CAMERA_MANAGER_TC_001
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER_TC_001', 0, async function (done) {
        console.info("--------------GET_CAMERA_MANAGER_TC_001--------------");
        cameraObj.getCameraManager(null, async (err, data) => {
            if (!err) {
                console.info(TAG + "Entering GET_CAMERA_MANAGER_TC_001 success");
                if (data != null || data != undefined) {
                    console.info(TAG + "Entering GET_CAMERA_MANAGER_TC_001 data is not null || undefined");
                    cameraManager = data;
                    expect(true).assertTrue();
                    console.info(TAG + "Entering GET_CAMERA_MANAGER_TC_001 PASSED");
                }
            } else {
                expect().assertFail();
                console.info(TAG + "Entering GET_CAMERA_MANAGER_TC_001 FAILED: " + err.message);
            }
            console.info(TAG + "Entering GET_CAMERA_MANAGER_TC_001 ends here");
            await sleep(1000);
            done();
        })
        await sleep(1000);
        done();
    })

    /**
      * @tc.number    : GET_CAMERAS_TC_003
      * @tc.name      : Get camera from cameramanager to get array of camera async api
      * @tc.desc      : Get camera from cameramanager to get array of camera async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS_TC_003', 0, async function (done) {
        console.info("--------------GET_CAMERAS_TC_003--------------");
        cameraManager.getCameras(async (err, data) => {
            if (!err) {
                console.info(TAG + "Entering GET_CAMERAS_TC_003 success");
                if (data != null || data != undefined) {
                    console.info(TAG + "Entering GET_CAMERAS_TC_003 data is not null || undefined");
                    camerasArray = data;
                    if (camerasArray != null && camerasArray.length > 0) {
                        for (var i = 0; i < camerasArray.length; i++) {
                            // Get the variables from camera object
                            var cameraId = camerasArray[i].cameraId;
                            console.info(TAG + "Entering GET_CAMERAS_TC_003 camera" + i + "Id: " + cameraId);
                            var cameraPosition = camerasArray[i].cameraPosition;
                            console.info(TAG + "Entering GET_CAMERAS_TC_003 camera" + i + "Position: " + cameraPosition);
                            var cameraType = camerasArray[i].cameraType;
                            console.info(TAG + "Entering GET_CAMERAS_TC_003 camera" + i + "Type: " + cameraType);
                            var connectionType = camerasArray[i].connectionType
                            console.info(TAG + "Entering GET_CAMERAS_TC_003 connection" + i + "Type: " + connectionType);
                        }
                        expect(true).assertTrue();
                        console.info(TAG + "Entering GET_CAMERAS_TC_003 PASSED");
                    } else {
                        expect().assertFail();
                        console.info(TAG + "Entering GET_CAMERAS_TC_003 FAILED cameraArray is null || undefined");
                    }
                }
            } else {
                expect().assertFail();
                console.info(TAG + "Entering GET_CAMERAS_TC_003 FAILED: " + err.message);
            }
            console.info(TAG + "Entering GET_CAMERAS_TC_003 ends here");
            await sleep(1000);
            done();
        })
        await sleep(1000);
        done();
    })


    /*CAMERA-0 Scripts*/
    /**
      * @tc.number    : CREATE_CAMERA_INPUT_TC_005
      * @tc.name      : Create camerainput from camera-0 cameraId async api
      * @tc.desc      : Create camerainput from camera-0 cameraId async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_TC_005', 0, async function (done) {
        console.info("--------------CAMERA-0 STARTS HERE--------------");
        console.info("--------------CREATE_CAMERA_INPUT_TC_005--------------");
        cameraManager.createCameraInput(camerasArray[0].cameraId, async (err, data) => {
            if (!err) {
                if (data != null && data != undefined) {
                    console.info(TAG + "Entering CREATE_CAMERA_INPUT_TC_005 data is not null || undefined");
                    camera0Input = data;
                    expect(true).assertTrue();
                    console.info(TAG + "Entering CREATE_CAMERA_INPUT_TC_005 PASSED with CameraID :" + camerasArray[0].cameraId);
                }
            } else {
                expect().assertFail();
                console.info(TAG + "Entering CREATE_CAMERA_INPUT_TC_005 FAILED: " + err.message);
            }
            console.info(TAG + "Entering CREATE_CAMERA_INPUT_TC_005 ends here");
			camera0Input.release();
            await sleep(1000);
            done();
        })
        await sleep(1000);
        done();
    })


	
	/**
      * @tc.number    : CAMERA_STATUS
      * @tc.name      : camera status ENAME
      * @tc.desc      : camera status ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAMERA_STATUS', 0, async function (done) {
		console.info(TAG + "--------------CameraStatus ------------");
        console.info(TAG + "CameraStatus CAMERA_STATUS_APPEAR : " + cameraObj.CameraStatus.CAMERA_STATUS_APPEAR);
		expect(cameraObj.CameraStatus.CAMERA_STATUS_APPEAR).assertEqual(0);
		console.info(TAG + "CameraStatus CAMERA_STATUS_DISAPPEAR : " + cameraObj.CameraStatus.CAMERA_STATUS_DISAPPEAR);
		expect(cameraObj.CameraStatus.CAMERA_STATUS_DISAPPEAR).assertEqual(1);
		console.info(TAG + "CameraStatus CAMERA_STATUS_AVAILABLE : " + cameraObj.CameraStatus.CAMERA_STATUS_AVAILABLE)
		expect(cameraObj.CameraStatus.CAMERA_STATUS_AVAILABLE).assertEqual(2);
		console.info(TAG + "CameraStatus CAMERA_STATUS_UNAVAILABLE : " + cameraObj.CameraStatus.CAMERA_STATUS_UNAVAILABLE)
		expect(cameraObj.CameraStatus.CAMERA_STATUS_UNAVAILABLE).assertEqual(3);
        await sleep(1000);
        done();

    })

    /**
      * @tc.number    : CAMERA_POSITION
      * @tc.name      : Camera position ENAME
      * @tc.desc      : Camera position ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAMERA_POSITION', 0, async function (done) {
      console.info(TAG + "--------------CameraPosition ------------")
      console.info(TAG + "CameraPosition CAMERA_POSITION_BACK : " + cameraObj.CameraPosition.CAMERA_POSITION_BACK);
      expect(cameraObj.CameraPosition.CAMERA_POSITION_BACK).assertEqual(1);
      console.info(TAG + "CameraPosition CAMERA_POSITION_FRONT : " + cameraObj.CameraPosition.CAMERA_POSITION_FRONT);
      expect(cameraObj.CameraPosition.CAMERA_POSITION_FRONT).assertEqual(2);
      console.info(TAG + "CameraPosition CAMERA_POSITION_UNSPECIFIED : " + cameraObj.CameraPosition.CAMERA_POSITION_UNSPECIFIED);
      expect(cameraObj.CameraPosition.CAMERA_POSITION_UNSPECIFIED).assertEqual(0);
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CAMERA_TYPE
      * @tc.name      : camera type ENAME
      * @tc.desc      : camera type ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAMERA_TYPE', 0, async function (done) {
      console.info(TAG + "--------------CameraType ------------")
      console.info(TAG + "CameraType CAMERA_TYPE_UNSPECIFIED : " + cameraObj.CameraType.CAMERA_TYPE_UNSPECIFIED);
      expect(cameraObj.CameraType.CAMERA_TYPE_UNSPECIFIED).assertEqual(0);
      console.info(TAG + "CameraType CAMERA_TYPE_WIDE_ANGLE : " + cameraObj.CameraType.CAMERA_TYPE_WIDE_ANGLE);
      expect(cameraObj.CameraType.CAMERA_TYPE_WIDE_ANGLE).assertEqual(1);
      console.info(TAG + 'CameraType CAMERA_TYPE_ULTRA_WIDE : ' + cameraObj.CameraType.CAMERA_TYPE_ULTRA_WIDE);
      expect(cameraObj.CameraType.CAMERA_TYPE_ULTRA_WIDE).assertEqual(2);
      console.info(TAG + 'CameraType CAMERA_TYPE_TELEPHOTO : ' + cameraObj.CameraType.CAMERA_TYPE_TELEPHOTO);
      expect(cameraObj.CameraType.CAMERA_TYPE_TELEPHOTO).assertEqual(3);
      console.info(TAG + 'CameraType CAMERA_TYPE_TRUE_DEPTH : ' + cameraObj.CameraType.CAMERA_TYPE_TRUE_DEPTH)
      expect(cameraObj.CameraType.CAMERA_TYPE_TRUE_DEPTH).assertEqual(4);
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CONNECTION_TYPE
      * @tc.name      : connection type ENAME
      * @tc.desc      : connection type ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CONNECTION_TYPE', 0, async function (done) {
	   console.info(TAG + "--------------ConnectionType ------------")
       console.info(TAG + "ConnectionType CAMERA_CONNECTION_BUILT_IN : " + cameraObj.ConnectionType.CAMERA_CONNECTION_BUILT_IN);
		   expect(cameraObj.ConnectionType.CAMERA_CONNECTION_BUILT_IN).assertEqual(0);
       console.info(TAG + "ConnectionType CAMERA_CONNECTION_USB_PLUGIN : " + cameraObj.ConnectionType.CAMERA_CONNECTION_USB_PLUGIN);
	   expect(cameraObj.ConnectionType.CAMERA_CONNECTION_USB_PLUGIN).assertEqual(1);
		   console.info(TAG + "ConnectionType CAMERA_CONNECTION_REMOTE : " + cameraObj.ConnectionType.CAMERA_CONNECTION_REMOTE);
	   expect(cameraObj.ConnectionType.CAMERA_CONNECTION_REMOTE).assertEqual(2);
	   await sleep(1000);
	   done();
	})

	
	/**
      * @tc.number    : CAMERA_FORMAT
      * @tc.name      : Camera Format ENAME
      * @tc.desc      : Camera Format ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAMERA_FORMAT', 0, async function (done) {
	console.info(TAG + "--------------CameraFormat ------------")
    console.info(TAG + "CameraFormat CAMERA_FORMAT_YCRCb_420_SP : " + cameraObj.CameraFormat.CAMERA_FORMAT_YCRCb_420_SP);
		expect(cameraObj.CameraFormat.CAMERA_FORMAT_YCRCb_420_SP).assertEqual(1003);
    console.info(TAG + "CameraFormat CAMERA_FORMAT_JPEG : " + cameraObj.CameraFormat.CAMERA_FORMAT_JPEG);
		expect(cameraObj.CameraFormat.CAMERA_FORMAT_JPEG).assertEqual(2000);
		await sleep(1000);
	done();
	})
	
	/**
      * @tc.number    : FLASHMODE
      * @tc.name      : Flash Mode ENAME
      * @tc.desc      : Flash Mode ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('FLASHMODE', 0, async function (done) {
	console.info(TAG + "--------------FlashMode ------------")
    console.info(TAG + "FlashMode FLASH_MODE_CLOSE : " + cameraObj.FlashMode.FLASH_MODE_CLOSE);
		expect(cameraObj.FlashMode.FLASH_MODE_CLOSE).assertEqual(0);
    console.info(TAG + "FlashMode FLASH_MODE_OPEN : " + cameraObj.FlashMode.FLASH_MODE_OPEN);
		expect(cameraObj.FlashMode.FLASH_MODE_OPEN).assertEqual(1);
	console.info(TAG + "FlashMode FLASH_MODE_AUTO : " + cameraObj.FlashMode.FLASH_MODE_AUTO);
		expect(cameraObj.FlashMode.FLASH_MODE_AUTO).assertEqual(2);
	console.info(TAG + "FlashMode FLASH_MODE_ALWAYS_OPEN : " + cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
		expect(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN).assertEqual(3);
		await sleep(1000);
	done();
	})
	
	/**
      * @tc.number    : FOCUSMODE
      * @tc.name      : Focus Mode ENAME
      * @tc.desc      : Focus Mode ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('FOCUSMODE', 0, async function (done) {
	console.info(TAG + "--------------FocusMode ------------")
    console.info(TAG + "FocusMode FOCUS_MODE_MANUAL : " + cameraObj.FocusMode.FOCUS_MODE_MANUAL);
		expect(cameraObj.FocusMode.FOCUS_MODE_MANUAL).assertEqual(0);
    console.info(TAG + "FocusMode FOCUS_MODE_CONTINUOUS_AUTO : " + cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
		expect(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO).assertEqual(1);
	console.info(TAG + "FocusMode FOCUS_MODE_AUTO : " + cameraObj.FocusMode.FOCUS_MODE_AUTO);
		expect(cameraObj.FocusMode.FOCUS_MODE_AUTO).assertEqual(2);
	console.info(TAG + "FocusMode FOCUS_MODE_LOCKED : " + cameraObj.FocusMode.FOCUS_MODE_LOCKED);
		expect(cameraObj.FocusMode.FOCUS_MODE_LOCKED).assertEqual(3);
		await sleep(1000);
	done();
	})
	
	/**
      * @tc.number    : FOCUSSTATE
      * @tc.name      : Focus State ENAME
      * @tc.desc      : Focus State ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('FOCUSSTATE', 0, async function (done) {
	console.info(TAG + "--------------FocusState ------------")
    console.info(TAG + "FocusState FOCUS_STATE_SCAN : " + cameraObj.FocusState.FOCUS_STATE_SCAN);
		expect(cameraObj.FocusState.FOCUS_STATE_SCAN).assertEqual(0);
    console.info(TAG + "FocusState FOCUS_STATE_FOCUSED : " + cameraObj.FocusState.FOCUS_STATE_FOCUSED);
		expect(cameraObj.FocusState.FOCUS_STATE_FOCUSED).assertEqual(1);
	console.info(TAG + "FocusState FOCUS_STATE_UNFOCUSED : " + cameraObj.FocusState.FOCUS_STATE_UNFOCUSED);
		expect(cameraObj.FocusState.FOCUS_STATE_UNFOCUSED).assertEqual(2);
		await sleep(1000);
	done();
	})
	
	/**
      * @tc.number    : ImageRotation
      * @tc.name      : Image Rotation ENAME
      * @tc.desc      : Image Rotation ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ImageRotation', 0, async function (done) {
	console.info(TAG + "--------------ImageRotation ------------")
    console.info(TAG + "ImageRotation ROTATION_0 : " + cameraObj.ImageRotation.ROTATION_0);
		expect(cameraObj.ImageRotation.ROTATION_0).assertEqual(0);
    console.info(TAG + "ImageRotation ROTATION_90 : " + cameraObj.ImageRotation.ROTATION_90);
		expect(cameraObj.ImageRotation.ROTATION_90).assertEqual(90);
	console.info(TAG + "ImageRotation ROTATION_180 : " + cameraObj.ImageRotation.ROTATION_180);
		expect(cameraObj.ImageRotation.ROTATION_180).assertEqual(180);
	console.info(TAG + "ImageRotation ROTATION_270 : " + cameraObj.ImageRotation.ROTATION_270);
		expect(cameraObj.ImageRotation.ROTATION_270).assertEqual(270);
		await sleep(1000);
	done();
	})
	
	/**
      * @tc.number    : QualityLevel
      * @tc.name      : Quality Level ENAME
      * @tc.desc      : Quality Level ENAME
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('QualityLevel', 0, async function (done) {
	console.info(TAG + "--------------QualityLevel ------------")
    console.info(TAG + "QualityLevel QUALITY_LEVEL_HIGH : " + cameraObj.QualityLevel.QUALITY_LEVEL_HIGH);
		expect(cameraObj.QualityLevel.QUALITY_LEVEL_HIGH).assertEqual(0);
    console.info(TAG + "QualityLevel QUALITY_LEVEL_MEDIUM : " + cameraObj.QualityLevel.QUALITY_LEVEL_MEDIUM);
		expect(cameraObj.QualityLevel.QUALITY_LEVEL_MEDIUM).assertEqual(1);
	console.info(TAG + "QualityLevel QUALITY_LEVEL_LOW : " + cameraObj.QualityLevel.QUALITY_LEVEL_LOW);
		expect(cameraObj.QualityLevel.QUALITY_LEVEL_LOW).assertEqual(2);
		await sleep(1000);
	done();
	})	

})
}