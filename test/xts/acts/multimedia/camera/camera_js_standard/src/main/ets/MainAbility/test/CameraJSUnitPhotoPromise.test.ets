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
var camera0InputPromise;
var cameraManagerPromise;
var previewOutputPromise;
var photoOutputPromise;
var CaptureSessionPromise;
var surfaceId1;
var camerasArrayPromise

var photosettings1 = {
  rotation: 0,
  quality: 0,
}
var photosettings2 = {
  rotation: 90,
  quality: 1,
}
var photosettings3 = {
  rotation: 180,
  quality: 2,
}
var photosettings4 = {
  rotation: 270
}

export default function cameraJSUnitPhotoPromise(surfaceId: any) {

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

  function sleep(ms) {
    console.info(TAG + "Entering sleep -> Promise constructor");
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  async function applyPermission() {
    let appInfo = await bundle.getApplicationInfo('com.open.harmony.multimedia.cameratest', 0, 100);
    let atManager = abilityAccessCtrl.createAtManager();
    if (atManager != null) {
      let tokenID = appInfo.accessTokenId;
      console.info('[permission] case accessTokenID is ' + tokenID);
      let permissionName1 = 'ohos.permission.CAMERA';
      let permissionName2 = 'ohos.permission.MICROPHONE';
      let permissionName3 = 'ohos.permission.MEDIA_LOCATION';
      let permissionName4 = 'ohos.permission.READ_MEDIA';
      let permissionName5 = 'ohos.permission.WRITE_MEDIA';
      await atManager.grantUserGrantedPermission(tokenID, permissionName1, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
      await atManager.grantUserGrantedPermission(tokenID, permissionName2, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
      await atManager.grantUserGrantedPermission(tokenID, permissionName3, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
      await atManager.grantUserGrantedPermission(tokenID, permissionName4, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
      await atManager.grantUserGrantedPermission(tokenID, permissionName5, 1).then((result) => {
        console.info('[permission] case grantUserGrantedPermission success :' + result);
      }).catch((err) => {
        console.info('[permission] case grantUserGrantedPermission failed :' + err);
      });
    } else {
      console.info('[permission] case apply permission failed, createAtManager failed');
    }
  }

  describe('CameraJsUnitPhotoPromise', function () {
    console.info(TAG + '----------CameraJsUnitPhotoPromise--------------')

    beforeAll(async function () {
      await applyPermission();
      console.info('beforeAll case');
    })

    beforeEach(function () {
      sleep(5000);
      console.info('beforeEach case');
    })

    afterEach(async function () {
      console.info('afterEach case');
    })

    afterAll(function () {
      console.info('afterAll case');
    })

    console.info(TAG + "----------Camera-PhotoMode-Promise-------------");
    /**
      * @tc.number    : GET_CAMERA_MANAGER_PROMISE
      * @tc.name      : Create camera manager instance promise api
      * @tc.desc      : Create camera manager instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER_PROMISE', 0, async function (done) {
      console.info("--------------GET_CAMERA_MANAGER_PROMISE--------------");
      cameraManagerPromise = await cameraObj.getCameraManager(null);
      console.info(TAG + "Entering GET_CAMERA_MANAGER_PROMISE cameraManagerPromise: " + JSON.stringify(cameraManagerPromise));
      if (cameraManagerPromise != null && cameraManagerPromise != undefined) {
        expect(true).assertTrue();
        console.info(TAG + "Entering GET_CAMERA_MANAGER_PROMISE PASSED");
      } else {
        expect().assertFail();
        console.info(TAG + "Entering GET_CAMERA_MANAGER_PROMISE FAILED");
      }
      console.info(TAG + "Entering GET_CAMERA_MANAGER_PROMISE ends here");
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CAMERA_STATUS_CALLBACK
      * @tc.name      : camera status callback on CameraManager async api
      * @tc.desc      : camera status callback on CameraManager async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAMERA_STATUS_CALLBACK', 0, async function (done) {
      if (cameraManagerPromise == null || cameraManagerPromise == undefined) {
        console.info(TAG + 'Entering CAMERA_STATUS_CALLBACK cameraManagerPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering CAMERA_STATUS_CALLBACK to operate')
        cameraManagerPromise.on("cameraStatus", async (err, data) => {
          if (!err) {
            console.info(TAG + "Camera status Callback on cameraManagerPromise is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Camera status Callback CameraStatusInfo_Camera: " + data.camera);
              console.info(TAG + "Camera status Callback CameraStatusInfo_Status: " + data.status);

              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : GET_CAMERAS_PROMISE
      * @tc.name      : Get camera from cameramanager to get array of camera promise api
      * @tc.desc      : Get camera from cameramanager to get array of camera promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS_PROMISE', 0, async function (done) {
      console.info("--------------GET_CAMERAS_PROMISE--------------");
      camerasArrayPromise = await cameraManagerPromise.getCameras();
      console.info(TAG + "Entering GET_CAMERAS_PROMISE: " + JSON.stringify(camerasArrayPromise));
      if (camerasArrayPromise != null && camerasArrayPromise.length > 0) {
        console.info(TAG + "Entering GET_CAMERAS_PROMISE success");
        for (var i = 0; i < camerasArrayPromise.length; i++) {
          // Get the variables from camera object
          var cameraId = camerasArrayPromise[i].cameraId;
          console.info(TAG + "Entering GET_CAMERAS_PROMISE camera" + i + "Id: " + cameraId);
          var cameraPosition = camerasArrayPromise[i].cameraPosition;
          console.info(TAG + "Entering GET_CAMERAS_PROMISE camera" + i + "Position: " + cameraPosition);
          var cameraType = camerasArrayPromise[i].cameraType;
          console.info(TAG + "Entering GET_CAMERAS_PROMISE camera" + i + "Type: " + cameraType);
          var connectionType = camerasArrayPromise[i].connectionType
          console.info(TAG + "Entering GET_CAMERAS_PROMISE connection" + i + "Type: " + connectionType);
        }
        expect(true).assertTrue();
        console.info(TAG + "Entering GET_CAMERAS_PROMISE PASSED");
      } else {
        expect().assertFail();
        console.info(TAG + "Entering GET_CAMERAS_PROMISE FAILED");
      }
      console.info(TAG + "Entering GET_CAMERAS_PROMISE ends here");
      await sleep(1000);
      done();
    })

    /*CAMERA-0 Scripts*/
    /**
      * @tc.number    : CREATE_CAMERA_INPUT_PROMISE
      * @tc.name      : Create camerainput from camera-0 cameraId promise api
      * @tc.desc      : Create camerainput from camera-0 cameraId promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_PROMISE', 0, async function (done) {
      console.info("--------------CAMERA-0 STARTS HERE--------------");
      console.info("--------------CREATE_CAMERA_INPUT_PROMISE--------------");
      camera0InputPromise = await cameraManagerPromise.createCameraInput(camerasArrayPromise[0].cameraId);
      console.info(TAG + "Entering CREATE_CAMERA_INPUT_PROMISE camera0InputPromise: " + JSON.stringify(camera0InputPromise));
      if (camera0InputPromise != null && camera0InputPromise != undefined) {
        console.info(TAG + "Entering CREATE_CAMERA_INPUT_PROMISE camera0InputPromise is not null || undefined");
        expect(true).assertTrue();
        console.info(TAG + "Entering CREATE_CAMERA_INPUT_PROMISE PASSED");
      } else {
        expect().assertFail();
        console.info(TAG + "Entering CREATE_CAMERA_INPUT_PROMISE FAILED");
      }
      console.info(TAG + "Entering CREATE_CAMERA_INPUT_PROMISE ends here");
      await sleep(1000);
      done();
    })

    /**
	  * @tc.number    : CAMERA_INPUT_CALLBACK_ON_ERROR
	  * @tc.name      : Photo output callback on error api
	  * @tc.desc      : Photo output callback on error api
	  * @tc.size      : MEDIUM
	  * @tc.type      : Function
	  * @tc.level     : Level 0
	*/
    it('CAMERA_INPUT_CALLBACK_ON_ERROR', 0, async function (done) {
      if (camera0InputPromise == null || camera0InputPromise == undefined) {
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR camera0InputPromise == null || undefined");
      } else {
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR to operate");
        camera0InputPromise.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "camera0InputPromise error callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during camera0InputPromise with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /*PreviewOutput APIs test script*/
    /**
      * @tc.number    : CREATE_PREVIEW_OUTPUT_SUCCESS_PROMISE
      * @tc.name      : Create PreviewOutput instance promise api
      * @tc.desc      : Create PreviewOutput instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_PREVIEW_OUTPUT_SUCCESS_PROMISE', 0, async function (done) {
      console.info(TAG + " Entering CREATE_PREVIEW_OUTPUT_SUCCESS_PROMISE to operate");
      previewOutputPromise = await cameraObj.createPreviewOutput(surfaceId);
      console.info(TAG + " Entering  createPreviewOutput success");
      if (previewOutputPromise != null || previewOutputPromise != undefined) {
        expect(true).assertTrue();
        console.info(TAG + "Entering createPreviewOutput PASSED: " + JSON.stringify(previewOutputPromise));
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering createPreviewOutput FAILED");
        console.info(TAG + "Entering createPreviewOutput ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_ERROR
      * @tc.name      : Preview output callback on error api
      * @tc.desc      : Preview output callback on error api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('PREVIEW_OUTPUT_CALLBACK_ON_ERROR', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR to operate");
        previewOutputPromise.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewOutputError callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during PreviewOutput with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /*PhotoOutput APIs test script*/
    /**
      * @tc.number    : CREATE_PHOTO_OUTPUT_SUCCESS_PROMISE
      * @tc.name      : Create PhotoOutput instance promise api
      * @tc.desc      : Create PhotoOutput instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_PHOTO_OUTPUT_SUCCESS_PROMISE', 0, async function (done) {
      console.info(TAG + "Entering CREATE_PHOTO_OUTPUT_SUCCESS_PROMISE to operate");
      console.info(TAG + 'Entering getImageReceiverSurfaceId')
      await getImageReceiverSurfaceId()
      await sleep(1000)
      photoOutputPromise = await cameraObj.createPhotoOutput(surfaceId1);
      console.info(TAG + "Entering  createPhotoOutput success");
      if (photoOutputPromise != null || photoOutputPromise != undefined) {
        expect(true).assertTrue();
        console.info(TAG + "Entering CREATE_PHOTO_OUTPUT_SUCCESS_PROMISE PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering CREATE_PHOTO_OUTPUT_SUCCESS_PROMISE FAILED");
        console.info(TAG + "Entering createPhotoOutput ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTO_OUTPUT_CALLBACK_ON_ERROR
      * @tc.name      : Photo output callback on error api
      * @tc.desc      : Photo output callback on error api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTO_OUTPUT_CALLBACK_ON_ERROR', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTO_OUTPUT_CALLBACK_ON_ERROR photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_OUTPUT_CALLBACK_ON_ERROR to operate");
        photoOutputPromise.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "PhotoOutputError callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during PhotoOutput with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /*CaptureSession APIs test script*/
    /**
      * @tc.number    : CREATE_CAPTURE_SESSION_SUCCESS_PROMISE
      * @tc.name      : Create CaptureSession instance promise api
      * @tc.desc      : Create Capturesession instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAPTURE_SESSION_SUCCESS_PROMISE', 0, async function (done) {
      console.info(TAG + "Entering CREATE_CAPTURE_SESSION_SUCCESS_PROMISE to operate");
      CaptureSessionPromise = await cameraObj.createCaptureSession(null);
      console.info(TAG + "Entering createCaptureSession success");
      if (CaptureSessionPromise != null || CaptureSessionPromise != undefined) {
        console.info(TAG + "Entering createCaptureSession data is not null || undefined");
        expect(true).assertTrue();
        console.info(TAG + "Entering createCaptureSession PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering CREATE_CAPTURE_SESSION_SUCCESS_PROMISE FAILED");
        console.info(TAG + "Entering createCaptureSession ends here");
      }
      await sleep(1000);
      done();
    })

    //Capturesession callback
    /**
      * @tc.number    : CAP_SES_CALLBACK_ON_ERROR
      * @tc.name      : CaptureSession callback on error api
      * @tc.desc      : CaptureSession callback on error api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAP_SES_CALLBACK_ON_ERROR', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR to operate");
        CaptureSessionPromise.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + " captureSession errorcallback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error in CaptureSessioncallback with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /*CaptureSession APIs*/
    /**
      * @tc.number    : CREATE_BEGIN_CONFIG_SUCCESS_PROMISE
      * @tc.name      : CaptureSession_Begin config promise api
      * @tc.desc      : CaptureSession_Begin config promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_BEGIN_CONFIG_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS_PROMISE to operate");
        const promise = await CaptureSessionPromise.beginConfig();
        console.info(TAG + "Entering  beginConfig success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering CREATE_BEGIN_CONFIG beginConfig data is not null || undefined: " + JSON.stringify(promise));
          expect(true).assertTrue();
          console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS_PROMISE beginConfig PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering beginConfig FAILED");
        }
        console.info(TAG + "Entering beginConfig ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : ADD_INPUT_SUCCESS_PROMISE
      * @tc.name      : Add Input with camera0Input api
      * @tc.desc      : Add Input with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_INPUT_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE to operate");
        const Promise = await CaptureSessionPromise.addInput(camera0InputPromise);
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput success");
        if (Promise != null || Promise != undefined) {
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE FAILED: ");
        }
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE
      * @tc.name      : Add output with camera0Input api
      * @tc.desc      : Add output with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE to operate");
        const promise = await CaptureSessionPromise.addOutput(previewOutputPromise);
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE : Success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE FAILED");
        }
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : ADD_OUTPUT_PHOTO_SUCCESS
      * @tc.name      : Add output with photo output api
      * @tc.desc      : Add output with photo output api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PHOTO_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS to operate");
        const promise = await CaptureSessionPromise.addOutput(photoOutputPromise);
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS  success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS FAILED: ");
        }
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })


    /**
      * @tc.number    : REMOVE_INPUT_SUCCESS
      * @tc.name      : remove input api
      * @tc.desc      : remove input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('REMOVE_INPUT_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS to operate");
        const Promise = await CaptureSessionPromise.removeInput(camera0InputPromise);
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS  success " + Promise);
        if (Promise != null || Promise != undefined) {
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS  data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS  PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS FAILED: ");
        }
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : REMOVE_PREVIEW_OUTPUT_SUCCESS
      * @tc.name      : Remove preview Output api
      * @tc.desc      : Remove preview Output api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('REMOVE_PREVIEW_OUTPUT_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS to operate");
        const Promise = await CaptureSessionPromise.removeOutput(previewOutputPromise);
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput success " + Promise);
        if (Promise != null || Promise != undefined) {
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS FAILED: ");
        }
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : REMOVE_PHOTO_OUTPUT_SUCCESS
      * @tc.name      : Remove photo Output api
      * @tc.desc      : Remove photo Output api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('REMOVE_PHOTO_OUTPUT_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS to operate");
        const Promise = await CaptureSessionPromise.removeOutput(photoOutputPromise);
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput success " + Promise);
        if (Promise != null || Promise != undefined) {
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS addInput PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering REMOVE_INPUT_SUCCESS FAILED: ");
        }
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })


    /**
      * @tc.number    : ADD_INPUT_SUCCESS_PROMISE
      * @tc.name      : Add Input with camera0Input api
      * @tc.desc      : Add Input with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_INPUT_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE to operate");
        const Promise = await CaptureSessionPromise.addInput(camera0InputPromise);
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput success");
        if (Promise != null || Promise != undefined) {
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE addInput PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE FAILED: ");
        }
        console.info(TAG + "Entering ADD_INPUT_SUCCESS_PROMISE ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE
      * @tc.name      : Add output with camera0Input api
      * @tc.desc      : Add output with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE to operate");
        const promise = await CaptureSessionPromise.addOutput(previewOutputPromise);
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE : Success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE FAILED");
        }
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS_PROMISE ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : ADD_OUTPUT_PHOTO_SUCCESS
      * @tc.name      : Add output with photo output api
      * @tc.desc      : Add output with photo output api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PHOTO_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS to operate");
        const promise = await CaptureSessionPromise.addOutput(photoOutputPromise);
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS  success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS FAILED: ");
        }
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : COMMIT_CONFIG_SUCCESS
      * @tc.name      : commit config api
      * @tc.desc      : commit config api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('COMMIT_CONFIG_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS to operate");
        const promise = await CaptureSessionPromise.commitConfig();
        console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS data is not null || undefined");
          expect(true).assertTrue();
          console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig PASSED");
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig FAILED");
          console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig ends here");
        }
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    // callback related API
    //preview callback
    /**
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START
      * @tc.name      : Preview output callback on frame start api
      * @tc.desc      : Preview output callback on frame start api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START to operate");
        previewOutputPromise.on("frameStart", async (err, data) => {
          if (!err) {
            console.info(TAG + "Preview frameStart Callback is success");
            if (data != null || data != undefined) {
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail()
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END
      * @tc.name      : Preview capture callback on frame end api
      * @tc.desc      : Preview capture callback on frame end api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END to operate");
        previewOutputPromise.on("frameEnd", async (err, data) => {
          if (!err) {
            console.info(TAG + "Preview frameEnd Callback is success");
            if (data != null || data != undefined) {
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    //Capture callback
    /**
      * @tc.number    : PHOTO_CAP_CALLBACK_ON_CAPTURE_START
      * @tc.name      : Photo capture callback on capture start api
      * @tc.desc      : Photo capture callback on capture start api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTO_CAP_CALLBACK_ON_CAPTURE_START', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_START photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_START to operate");
        photoOutputPromise.on("captureStart", async (err, data) => {
          if (!err) {
            console.info(TAG + "CaptureStart Callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "captureStart success with captureId: " + data);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail()
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : PHOTO_CAP_CALLBACK_ON_CAPTURE_END
      * @tc.name      : Photo capture callback on capture end api
      * @tc.desc      : Photo capture callback on capture end api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTO_CAP_CALLBACK_ON_CAPTURE_END', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_END photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_END to operate");
        photoOutputPromise.on("captureEnd", async (err, data) => {
          if (!err) {
            console.info(TAG + "captureEnd callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "captureEnd callback with captureId: " + data.captureId);
              console.info(TAG + "captureEnd callback with frameCount: " + data.frameCount);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER
      * @tc.name      : Photo capture callback on frame shutter api
      * @tc.desc      : Photo capture callback on frame shutter api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER to operate");
        photoOutputPromise.on("frameShutter", async (err, data) => {
          if (!err) {
            console.info(TAG + "frameShutter callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "frameShutter callback with captureId: " + data.captureId);
              console.info(TAG + "frameShutter callback with timestamp: " + data.timestamp);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_START_SUCCESS
      * @tc.name      : capture session start api
      * @tc.desc      : capture session start api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAPTURE_SESSION_START_SUCCESS', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS to operate");
        const promise = await CaptureSessionPromise.start();
        console.info(TAG + "Entering captureSession start success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS data is not null || undefined" + promise);
          expect(true).assertTrue();
          console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS PASSED");
        }
        console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    //FLASH Function API scripts
    /**
      * @tc.number    : HAS_FLASH
      * @tc.name      : check if has flash-camera0Input api
      * @tc.desc      : check if has flash-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('HAS_FLASH', 0, async function (done) {
      console.info("--------------HAS_FLASH--------------");
      console.info(TAG + 'hasFlash called.')
      var hasFlashPromise = await camera0InputPromise.hasFlash();
      console.info(TAG + "Entering HAS_FLASH success");
      if (hasFlashPromise != null || hasFlashPromise != undefined) {
        console.info(TAG + "Entering HAS_FLASH data is not null || undefined");
        expect(true).assertTrue();
        console.info(TAG + "Entering HAS_FLASH PASSED with HAS_FLASH is: " + JSON.stringify(hasFlashPromise));
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering HAS_FLASH FAILED");
      }
      console.info(TAG + "Entering HAS_FLASH ends here");
      await sleep(1000)
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_OPEN_SUPPORTED
      * @tc.name      : check if flash mode open is supported-camera0Input api
      * @tc.desc      : check if flash mode open is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_OPEN_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED to operate");
      var isFMOpenSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_OPEN);
      console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED SUCCESS ");
      if (isFMOpenSupported != null || isFMOpenSupported != undefined) {
        console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED data is not null || undefined");
        console.info(TAG + "FLASH_MODE_OPEN supported is: " + JSON.stringify(isFMOpenSupported));
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_OPEN
      * @tc.name      : set flash mode open camera0 api
      * @tc.desc      : set flash mode open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_OPEN', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_OPEN to operate");
      var SetFMOpen = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_OPEN);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMOpen))
      if (SetFMOpen == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_OPEN);
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_OPEN
      * @tc.name      : get flash mode open camera0 api
      * @tc.desc      : get flash mode open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FLASH_MODE_OPEN', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_OPEN to operate");
      var GetFMOpen = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_OPEN success: " + JSON.stringify(GetFMOpen));
      if (GetFMOpen == 1) {
        console.info(TAG + "GET_FLASH_MODE_OPEN data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + JSON.stringify(GetFMOpen));
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_OPEN PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_OPEN FAILED");
        console.info(TAG + "GET_FLASH_MODE_OPEN ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED
      * @tc.name      : check if flash mode always open is supported-camera0Input api
      * @tc.desc      : check if flash mode always open is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED to operate");
      var isFMAlwaysOpenSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
      console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED SUCCESS ");
      if (isFMAlwaysOpenSupported != null || isFMAlwaysOpenSupported != undefined) {
        console.info(TAG + "Entering FLASH_MODE_ALWAYS_OPEN data is not null || undefined");
        console.info(TAG + "FLASH_MODE_OPEN supported is: " + isFMAlwaysOpenSupported);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_ALWAYS_OPEN
      * @tc.name      : set flash mode always open camera0 api
      * @tc.desc      : set flash mode always open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_ALWAYS_OPEN', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN to operate");
      var SetFMAlwaysOpen = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMAlwaysOpen))
      if (SetFMAlwaysOpen == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_ALWAYS_OPEN
      * @tc.name      : get flash mode always open camera0 api
      * @tc.desc      : get flash mode always open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FLASH_MODE_ALWAYS_OPEN', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN to operate");
      var GetFMAlwaysOpen = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN success");
      if (GetFMAlwaysOpen == 3) {
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + GetFMAlwaysOpen);
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN FAILED");
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_AUTO_SUPPORTED
      * @tc.name      : check if flash mode always open is supported-camera0Input api
      * @tc.desc      : check if flash mode always open is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_AUTO_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED to operate");
      var isFMAutoSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_AUTO);
      console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED SUCCESS ");
      if (isFMAutoSupported != null || isFMAutoSupported != undefined) {
        console.info(TAG + "Entering FLASH_MODE_AUTO data is not null || undefined");
        console.info(TAG + "FLASH_MODE_AUTO supported is: " + isFMAutoSupported);
        expect(isFMAutoSupported).assertEqual(false);
        console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-0 & Quality-0
      * @tc.name      : Photo output capture with photosettings api
      * @tc.desc      : Photo output capture with photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        const promise = photoOutputPromise.capture(photosettings1);
        console.info(TAG + "Entering photoOutput capture with photosettings1 success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering photoOutput capture with photosettings1 data is not null || undefined");
          console.info(TAG + "Entering photoOutput capture with  photosettings1 PASSED");
          expect(true).assertTrue();
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering photoOutput capture with photosettings1 FAILED");
          console.info(TAG + "Entering photoOutput capture with photosettings1 ends here");
        }
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_CLOSE_SUPPORTED
      * @tc.name      : check if flash mode close is supported-camera0Input api
      * @tc.desc      : check if flash mode close is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_CLOSE_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED to operate");
      var isFMCloseSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_CLOSE);
      console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED SUCCESS ");
      if (isFMCloseSupported != null || isFMCloseSupported != undefined) {
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED data is not null || undefined");
        console.info(TAG + "FLASH_MODE_CLOSE supported is: " + isFMCloseSupported);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_CLOSE
      * @tc.name      : set flash mode close camera0 api
      * @tc.desc      : set flash mode close camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_CLOSE', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_CLOSE to operate");
      var SetFMClose = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_CLOSE);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMClose))
      if (SetFMClose == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_CLOSE);
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_CLOSE
      * @tc.name      : get flash mode close camera0 api
      * @tc.desc      : get flash mode close camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FLASH_MODE_CLOSE', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_CLOSE to operate");
      var GetFMClose = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_CLOSE success");
      if (GetFMClose == 0) {
        console.info(TAG + "GET_FLASH_MODE_CLOSE data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + GetFMClose);
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_CLOSE PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_CLOSE FAILED");
        console.info(TAG + "GET_FLASH_MODE_CLOSE ends here");
      }
      await sleep(1000);
      done();
    })

    // FOCUS promise API's
    /**
      * @tc.number    : IS_FOCUS_MODE_LOCKED_SUPPORTED
      * @tc.name      : check if flash mode close is supported-camera0Input api
      * @tc.desc      : check if flash mode close is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_LOCKED_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED to operate");
      var isFMLockedSupported = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_LOCKED);
      console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED SUCCESS ");
      if (isFMLockedSupported != null || isFMLockedSupported != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED data is not null || undefined");
        console.info(TAG + "IS_FOCUS_MODE_LOCKED_SUPPORTED: " + isFMLockedSupported);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED FAILED");
        console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_LOCKED
      * @tc.name      : set focus mode locked camera0 api
      * @tc.desc      : set focus mode locked camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FOCUS_MODE_LOCKED', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED to operate");
      var SetFMLocked = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_LOCKED);
      console.info(TAG + "SetFMLocked: " + JSON.stringify(SetFMLocked))
      if (SetFMLocked == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED SUCCESS, current focusmode is: " + cameraObj.FocusMode.FOCUS_MODE_LOCKED);
        console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_LOCKED
      * @tc.name      : get flash mode close camera0 api
      * @tc.desc      : get flash mode close camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FOCUS_MODE_LOCKED', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_LOCKED to operate");
      var GetFMLocked = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_LOCKED success: " + GetFMLocked);
      if (GetFMLocked == 0) {
        console.info(TAG + "Current focusmode is: " + GetFMLocked);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_LOCKED PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_LOCKED FAILED");
        console.info(TAG + "GET_FOCUS_MODE_LOCKED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_MANUAL_SUPPORTED
      * @tc.name      : is focusmode manual supported
      * @tc.desc      : is focusmode manual supported
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('IS_FOCUS_MODE_MANUAL_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED to operate");
      var isFMmanualSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_MANUAL);
      if (isFMmanualSupportedpromise != null || isFMmanualSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMmanualSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_MANUAL
      * @tc.name      : set focus mode manual camera0 api
      * @tc.desc      : set focus mode manual camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('SET_FOCUS_MODE_MANUAL', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL to operate");
      var setFocusManual = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_MANUAL);
      console.info(TAG + "setFocusManual: " + JSON.stringify(setFocusManual))
      if (setFocusManual == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_MANUAL);
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_MANUAL
      * @tc.name      : get focus mode manual camera0 api
      * @tc.desc      : get focus mode manual camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FOCUS_MODE_MANUAL', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL SUCCESS");
      if (getfocusmodepromise == 0) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_MANUAL PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_MANUAL FAILED");
        console.info(TAG + "GET_FOCUS_MODE_MANUAL ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_CONTINUOUS_SUPPORTED
      * @tc.name      : check if focus mode continuous is supported-camera0Input api
      * @tc.desc      : check if focus mode continuous is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_CONTINUOUS_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED to operate");
      var isFMContinuousSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
      if (isFMContinuousSupportedpromise != null || isFMContinuousSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMContinuousSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_CONTINUOUS
      * @tc.name      : set focus mode continuous camera0 api
      * @tc.desc      : set focus mode continuous camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FOCUS_MODE_CONTINUOUS', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS to operate");
      var setFocusCont = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
      console.info(TAG + "setFocusCont: " + JSON.stringify(setFocusCont))
      if (setFocusCont == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_CONTINUOUS
      * @tc.name      : get focus mode continuous camera0 api
      * @tc.desc      : get focus mode continuous camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FOCUS_MODE_CONTINUOUS', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS SUCCESS");
      if (getfocusmodepromise == 1) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS FAILED");
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_AUTO_SUPPORTED
      * @tc.name      : check if focus mode auto is supported-camera0Input api
      * @tc.desc      : check if focus mode auto is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_AUTO_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED to operate");
      var isFMAutoSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_AUTO);
      if (isFMAutoSupportedpromise != null || isFMAutoSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMAutoSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_AUTO
      * @tc.name      : set focus mode auto camera0 api
      * @tc.desc      : set focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FOCUS_MODE_AUTO', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_AUTO to operate");
      var setFocusAuto = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_AUTO);
      console.info(TAG + "setFocusAuto: " + JSON.stringify(setFocusAuto))
      if (setFocusAuto == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_AUTO);
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_AUTO
      * @tc.name      : get focus mode auto camera0 api
      * @tc.desc      : get focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FOCUS_MODE_AUTO', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_AUTO to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_AUTO SUCCESS");
      if (getfocusmodepromise == 2) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_AUTO PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_AUTO FAILED");
        console.info(TAG + "GET_FOCUS_MODE_AUTO ends here");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS with Rotation-90 & Quality-1
      * @tc.name      : Photo output capture with photosettings api
      * @tc.desc      : Photo output capture with photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        const promise = photoOutputPromise.capture(photosettings2);
        console.info(TAG + "Entering photoOutput capture with photosettings2 success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering photoOutput capture with photosettings2 data is not null || undefined");
          console.info(TAG + "Entering photoOutput capture with  photosettings2 PASSED");
          expect(true).assertTrue();
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering photoOutput capture with photosettings2 FAILED");
          console.info(TAG + "Entering photoOutput capture with photosettings2 ends here");
        }
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS with Rotation-180 & Quality-2
      * @tc.name      : Photo output capture with photosettings api
      * @tc.desc      : Photo output capture with photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        const promise = photoOutputPromise.capture(photosettings3);
        console.info(TAG + "Entering photoOutput capture with photosettings3 success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering photoOutput capture with photosettings3 data is not null || undefined");
          console.info(TAG + "Entering photoOutput capture with  photosettings3 PASSED");
          expect(true).assertTrue();
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering photoOutput capture with photosettings3 FAILED");
          console.info(TAG + "Entering photoOutput capture with photosettings3 ends here");
        }
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS with Rotation-270
      * @tc.name      : Photo output capture with photosettings api
      * @tc.desc      : Photo output capture with photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        const promise = photoOutputPromise.capture(photosettings4);
        console.info(TAG + "Entering photoOutput capture with photosettings4 success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering photoOutput capture with photosettings4 data is not null || undefined");
          console.info(TAG + "Entering photoOutput capture with  photosettings4 PASSED");
          expect(true).assertTrue();
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering photoOutput capture with photosettings4 FAILED");
          console.info(TAG + "Entering photoOutput capture with photosettings4 ends here");
        }
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    //ZOOM Function
    /**
      * @tc.number    : GET_ZOOM_RATIO_PROMISE
      * @tc.name      : get zoom ratio camera-0 cameraId api promise api
      * @tc.desc      : get zoom ratio camera-0 cameraId api promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_ZOOM_RATIO_PROMISE', 0, async function (done) {
      console.info("--------------GET_ZOOM_RATIO_PROMISE--------------");
      var getZoomRatioPromise = await camera0InputPromise.getZoomRatioRange();
      console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE getZoomRatioPromise: " + JSON.stringify(getZoomRatioPromise));
      if (getZoomRatioPromise != null && getZoomRatioPromise != undefined) {
        console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE setZoomRatioPromise is not null || undefined");
        expect(true).assertTrue();
        console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE success: " + JSON.stringify(getZoomRatioPromise));
        console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE PASSED");
      } else {
        expect().assertFail();
        console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE FAILED");
      }
      console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE ends here");
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_1_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_1_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(1);
      console.info(TAG + "setZoomRatio success: 1");
      console.info(TAG + "getZoomRatio called")
      var getpromise1 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise1);
      if (getpromise1 != null && getpromise1 != undefined) {
        expect(getpromise1).assertEqual(1);
        console.info(TAG + "SET_GET_ZOOM_1_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_1_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_2_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_2_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(2);
      console.info(TAG + "setZoomRatio success: 2");
      console.info(TAG + "getZoomRatio called")
      var getpromise2 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise2);
      if (getpromise2 != null && getpromise2 != undefined) {
        expect(getpromise2).assertEqual(2);
        console.info(TAG + "SET_GET_ZOOM_2_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_2_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_3_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_3_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(3);
      console.info(TAG + "setZoomRatio success: 3");
      console.info(TAG + "getZoomRatio called")
      var getpromise3 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise3);
      if (getpromise3 != null && getpromise3 != undefined) {
        expect(getpromise3).assertEqual(3);
        console.info(TAG + "SET_GET_ZOOM_3_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_3_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_4_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_4_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(4);
      console.info(TAG + "setZoomRatio success: 4");
      console.info(TAG + "getZoomRatio called")
      var getpromise4 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise4);
      if (getpromise4 != null && getpromise4 != undefined) {
        expect(getpromise4).assertEqual(4);
        console.info(TAG + "SET_GET_ZOOM_4_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_4_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_5_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_5_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(5);
      console.info(TAG + "setZoomRatio success: 5");
      console.info(TAG + "getZoomRatio called")
      var getpromise5 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise5);
      if (getpromise5 != null && getpromise5 != undefined) {
        expect(getpromise5).assertEqual(5);
        console.info(TAG + "SET_GET_ZOOM_5_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_5_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_6_PROMISE
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_6_PROMISE', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(6);
      console.info(TAG + "setZoomRatio success: 6");
      console.info(TAG + "getZoomRatio called")
      var getpromise6 = await camera0InputPromise.getZoomRatio();
      console.info(TAG + "getZoomRatio success: " + getpromise6);
      if (getpromise6 != null && getpromise6 != undefined) {
        expect(getpromise6).assertEqual(6);
        console.info(TAG + "SET_GET_ZOOM_6_PROMISE PASSED ");
      }
      else {
        expect().assertFail();
        console.info(TAG + "SET_GET_ZOOM_6_PROMISE FAILED");
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS
      * @tc.name      : Photo output capture with photosettings api
      * @tc.desc      : Photo output capture with photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        const promise = photoOutputPromise.capture(photosettings1);
        console.info(TAG + "Entering photoOutput capture with photosettings1 success");
        if (promise != null || promise != undefined) {
          console.info(TAG + "Entering photoOutput capture with photosettings1 data is not null || undefined");
          console.info(TAG + "Entering photoOutput capture with  photosettings1 PASSED");
          expect(true).assertTrue();
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering photoOutput capture with photosettings1 FAILED");
        }
        console.info(TAG + "Entering photoOutput capture with photosettings1 ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /*CaptureSession APIs test script*/
    /**
      * @tc.number    : CAPTURE_SESSION_STOP_SUCCESS_PROMISE
      * @tc.name      : capture session stop api
      * @tc.desc      : capture session stop api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAPTURE_SESSION_STOP_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_STOP_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_STOP_SUCCESS_PROMISE to operate");
        const promise = await CaptureSessionPromise.stop();
        console.info(TAG + "Entering CAPTURE_SESSION_STOP_SUCCESS_PROMISE captureSession.stop promise: " + JSON.stringify(promise));
        if (promise != null || promise != undefined) {
          expect(true).assertTrue();
          console.info(TAG + "Entering CAPTURE_SESSION_STOP_SUCCESS_PROMISE captureSession.stop PASSED");
        }
        console.info(TAG + "Entering CAPTURE_SESSION_STOP_SUCCESS_PROMISE captureSession.stop ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CAPTURE_SESSION_RELEASE_SUCCESS_PROMISE
      * @tc.name      : capture session release api
      * @tc.desc      : capture session release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAPTURE_SESSION_RELEASE_SUCCESS_PROMISE', 0, async function (done) {
      if (CaptureSessionPromise == null || CaptureSessionPromise == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_RELEASE_SUCCESS_PROMISE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_RELEASE_SUCCESS_PROMISE to operate");
        const promise = await CaptureSessionPromise.release();
        console.info(TAG + "Entering captureSession.release promise: " + JSON.stringify(promise));
        if (promise != null || promise != undefined) {
          expect(true).assertTrue();
          console.info(TAG + "Entering captureSession.release PASSED");
        }
        console.info(TAG + "Entering captureSession.release ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PREVIEWOUTPUT_RELEASE_SUCCESS_PROMISE
      * @tc.name      : PreviewOutput release api
      * @tc.desc      : PreviewOutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEWOUTPUT_RELEASE_SUCCESS_PROMISE', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + "Entering PREVIEWOUTPUT_RELEASE_SUCCESS_PROMISE previewOutputPromise == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEWOUTPUT_RELEASE_SUCCESS_PROMISE to operate");
        const promise = await previewOutputPromise.release();
        console.info(TAG + "Entering previewOutputPromise.release promise: " + JSON.stringify(promise));
        if (promise != null || promise != undefined) {
          expect(true).assertTrue();
          console.info(TAG + "Entering previewOutputPromise.release PASSED");
        }
        console.info(TAG + "Entering previewOutputPromise.release ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_RELEASE_SUCCESS_PROMISE
      * @tc.name      : PhotoOutput release api
      * @tc.desc      : PhotoOutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_RELEASE_SUCCESS_PROMISE', 0, async function (done) {
      if (photoOutputPromise == null || photoOutputPromise == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_RELEASE_SUCCESS_PROMISE photoOutputPromise == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_RELEASE_SUCCESS_PROMISE to operate");
        const promise = await photoOutputPromise.release();
        console.info(TAG + "Entering photoOutputPromise.release promise: " + JSON.stringify(promise));
        if (promise != null || promise != undefined) {
          expect(true).assertTrue();
          console.info(TAG + "Entering photoOutputPromise.release PASSED");
        }
        console.info(TAG + "Entering photoOutputPromise.release ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CAMERAINPUT_RELEASE_SUCCESS_PROMISE
      * @tc.name      : cameraInput release api
      * @tc.desc      : cameraInput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAMERAINPUT_RELEASE_SUCCESS_PROMISE', 0, async function (done) {
      if (camera0InputPromise == null || camera0InputPromise == undefined) {
        console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS_PROMISE camera0InputPromise == null || undefined");
      } else {
        console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS_PROMISE to operate");
        const promise = await camera0InputPromise.release();
        console.info(TAG + "Entering camera0InputPromise.release promise: " + JSON.stringify(promise));
        if (promise != null || promise != undefined) {
          expect(true).assertTrue();
          console.info(TAG + "Entering camera0InputPromise.release PASSED");
        }
        console.info(TAG + "Entering camera0InputPromise.release ends here");
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })
  });
}