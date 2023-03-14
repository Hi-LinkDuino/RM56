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
var camera0Input;
var cameraManager;
var previewOutputAsync;
var photoOutputAsync;
var captureSession;
var surfaceId1;
var camerasArray;

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

export default function cameraJSUnitPhotoAsync(surfaceId: any) {

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

  describe('CameraJsUnitPhotoAsync', function () {
    console.info(TAG + '----------CameraJsUnitPhotoAsync--------------')

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

    console.info(TAG + "----------Camera-PhotoMode-Async-------------");
    /**
      * @tc.number    : GET_CAMERA_MANAGER
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER', 0, async function (done) {
      console.info("--------------GET_CAMERA_MANAGER--------------");
      cameraObj.getCameraManager(null, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_CAMERA_MANAGER success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering GET_CAMERA_MANAGER data is not null || undefined");
            cameraManager = data;
            expect(true).assertTrue();
            console.info(TAG + "Entering GET_CAMERA_MANAGER PASSED");
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering GET_CAMERA_MANAGER FAILED: " + err.message);
        }
        console.info(TAG + "Entering GET_CAMERA_MANAGER ends here");
        await sleep(1000);
        done();
      })
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
      if (cameraManager == null || cameraManager == undefined) {
        console.info(TAG + "Entering CAMERA_STATUS_CALLBACK cameraManager == null || undefined")
      } else {
        console.info(TAG + "Entering CAMERA_STATUS_CALLBACK to operate")
        cameraManager.on("cameraStatus", async (err, data) => {
          if (!err) {
            console.info(TAG + "Camera status Callback on cameraManager is success");
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
      }
      await sleep(1000);
      done();
    })


    /**
      * @tc.number    : GET_CAMERAS
      * @tc.name      : Get camera from cameramanager to get array of camera async api
      * @tc.desc      : Get camera from cameramanager to get array of camera async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS', 0, async function (done) {
      console.info("--------------GET_CAMERAS--------------");
      cameraManager.getCameras(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_CAMERAS success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering GET_CAMERAS data is not null || undefined");
            camerasArray = data;
            if (camerasArray != null && camerasArray.length > 0) {
              for (var i = 0; i < camerasArray.length; i++) {
                // Get the variables from camera object
                var cameraId = camerasArray[i].cameraId;
                console.info(TAG + "Entering GET_CAMERAS camera" + i + "Id: " + cameraId);
                var cameraPosition = camerasArray[i].cameraPosition;
                console.info(TAG + "Entering GET_CAMERAS camera" + i + "Position: " + cameraPosition);
                var cameraType = camerasArray[i].cameraType;
                console.info(TAG + "Entering GET_CAMERAS camera" + i + "Type: " + cameraType);
                var connectionType = camerasArray[i].connectionType
                console.info(TAG + "Entering GET_CAMERAS connection" + i + "Type: " + connectionType);
              }
              expect(true).assertTrue();
              console.info(TAG + "Entering GET_CAMERAS PASSED");
            } else {
              expect().assertFail();
              console.info(TAG + "Entering GET_CAMERAS FAILED cameraArray is null || undefined");
            }
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering GET_CAMERAS FAILED: " + err.message);
        }
        console.info(TAG + "Entering GET_CAMERAS ends here");
        await sleep(1000);
        done();
      })
      await sleep(1000);
      done();
    })


    /*CAMERA-0 Scripts*/
    /**
      * @tc.number    : CREATE_CAMERA_INPUT
      * @tc.name      : Create camerainput from camera-0 cameraId async api
      * @tc.desc      : Create camerainput from camera-0 cameraId async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT', 0, async function (done) {
      cameraManager.createCameraInput(camerasArray[0].cameraId, async (err, data) => {
        if (!err) {
          if (data != null && data != undefined) {
            console.info(TAG + "Entering CREATE_CAMERA_INPUT data is not null || undefined");
            camera0Input = data;
            expect(true).assertTrue();
            console.info(TAG + "Entering CREATE_CAMERA_INPUT PASSED with CameraID :" + camerasArray[0].cameraId);
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering CREATE_CAMERA_INPUT FAILED: " + err.message);
        }
        console.info(TAG + "Entering CREATE_CAMERA_INPUT ends here");
        await sleep(1000);
        done();
      })
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
      if (camera0Input == null || camera0Input == undefined) {
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR cameraInput == null || undefined");
      } else {
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR to operate");
        camera0Input.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "cameraInput error callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during cameraInput with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1000);
          done();
        })
      }
      await sleep(1000);
      done();
    })

    /*PreviewOutput APIs test script*/
    /**
      * @tc.number    : CREATE_PREVIEW_OUTPUT_SUCCESS
      * @tc.name      : Create PreviewOutput instance api
      * @tc.desc      : Create PreviewOutput instance api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */

    it('CREATE_PREVIEW_OUTPUT_SUCCESS', 0, async function (done) {
      console.info(TAG + " Entering CREATE_PREVIEW_OUTPUT_SUCCESS to operate");
      cameraObj.createPreviewOutput(surfaceId, async (err, data) => {
        if (!err) {
          console.info(TAG + " Entering  createPreviewOutput success");
          if (data != null || data != undefined) {
            console.info(TAG + " Entering createPreviewOutput data is not null || undefined");
            previewOutputAsync = data;
            expect(true).assertTrue();
            console.info(TAG + "Entering CREATE_PREVIEW_OUTPUT_SUCCESS PASSED" + previewOutputAsync);
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering CREATE_PREVIEW_OUTPUT_SUCCESS FAILED" + err.message);
        }
        console.info(TAG + "Entering CREATE_PREVIEW_OUTPUT_SUCCESS ends here");
        await sleep(1000);
        done();
      })
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
      if (previewOutputAsync == null || previewOutputAsync == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR to operate");
        previewOutputAsync.on("error", async (err, data) => {
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
      * @tc.number    : CREATE_PHOTO_OUTPUT_SUCCESS
      * @tc.name      : Create PhotoOutput instance api
      * @tc.desc      : Create PhotoOutput instance api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */

    it('CREATE_PHOTO_OUTPUT_SUCCESS', 0, async function (done) {
      console.info(TAG + "Entering CREATE_PHOTO_OUTPUT_SUCCESS to operate");
      console.info(TAG + 'Entering getImageReceiverSurfaceId')
      await getImageReceiverSurfaceId()
      await sleep(1000)
      cameraObj.createPhotoOutput(surfaceId1, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering  createPhotoOutput success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering  createPhotoOutput data is not null || undefined");
            photoOutputAsync = data;
            expect(true).assertTrue();
            console.info(TAG + "Entering  CREATE_PHOTO_OUTPUT_SUCCESS PASSED");
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering CREATE_PHOTO_OUTPUT_SUCCESS FAILED");
          console.info(TAG + "Entering  createPhotoOutput ends here");
        }
        await sleep(1000);
        done();
      })
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTO_OUTPUT_CALLBACK_ON_ERROR photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_OUTPUT_CALLBACK_ON_ERROR to operate");
        photoOutputAsync.on("error", async (err, data) => {
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
      * @tc.number    : CREATE_CAPTURE_SESSION_SUCCESS
      * @tc.name      : Create CaptureSession instance api
      * @tc.desc      : Create CaptureSession instance api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAPTURE_SESSION_SUCCESS', 0, async function (done) {
      console.info(TAG + "Entering CREATE_CAPTURE_SESSION_SUCCESS to operate");
      cameraObj.createCaptureSession(null, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering createCaptureSession success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering createCaptureSession data is not null || undefined");
            captureSession = data;
            expect(true).assertTrue();
            console.info(TAG + "Entering createCaptureSession PASSED");
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering CREATE_CAPTURE_SESSION_SUCCESS FAILED" + err.message);
          console.info(TAG + "Entering createCaptureSession ends here");
        }
        await sleep(1000);
        done();
      })
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR to operate");
        captureSession.on("error", async (err, data) => {
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
      * @tc.number    : CREATE_BEGIN_CONFIG_SUCCESS
      * @tc.name      : CaptureSession_Begin config api
      * @tc.desc      : CaptureSession_Begin config api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_BEGIN_CONFIG_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS to operate");
        captureSession.beginConfig(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering  beginConfig success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering CREATE_BEGIN_CONFIG beginConfig data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS beginConfig PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS FAILED" + err.message);
            console.info(TAG + "Entering CREATE_BEGIN_CONFIG_SUCCESS ends here");
          }
          await sleep(1000);
          done();
        })

        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : ADD_INPUT_SUCCESS
      * @tc.name      : Add Input with camera0Input api
      * @tc.desc      : Add Input with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_INPUT_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS to operate");
        captureSession.addInput(camera0Input, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_INPUT_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering ADD_INPUT_SUCCESS ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_SUCCESS
      * @tc.name      : Add output with camera0Input api
      * @tc.desc      : Add output with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PREVIEW_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS to operate");
        captureSession.addOutput(previewOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS : Success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS FAILED" + err.message);
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS ends here");
          }
          await sleep(1000);
          done();
        })
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS to operate");
        captureSession.addOutput(photoOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS  success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS FAILED: " + err.message);
          }
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS ends here");
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_INPUT_SUCCESS to operate");
        captureSession.removeInput(camera0Input, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering remove input success");
            expect(true).assertTrue();
            console.info(TAG + "Entering REMOVE_INPUT_SUCCESS PASSED");
          }
          else {
            expect().assertFail();
            console.info(TAG + "Entering Remove Input FAILED" + err.message);
            console.info(TAG + "Entering Remove Input ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering REMOVE_PREVIEW_OUTPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_PREVIEW_OUTPUT_SUCCESS to operate");
        captureSession.removeOutput(previewOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering remove preview Output success");
            expect(true).assertTrue();
            console.info(TAG + "Entering REMOVE_PREVIEW_OUTPUT_SUCCESS PASSED");
          }
          else {
            expect().assertFail();
            console.info(TAG + "Entering Remove preview Output FAILED" + err.message);
            console.info(TAG + "Entering Remove Preview Output ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering REMOVE_PHOTO_OUTPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_PHOTO_OUTPUT_SUCCESS to operate");
        captureSession.removeOutput(photoOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering remove photo Output success");
            expect(true).assertTrue();
            console.info(TAG + "Entering REMOVE_PHOTO_OUTPUT_SUCCESS PASSED");
          }
          else {
            expect().assertFail();
            console.info(TAG + "Entering Remove photo Output FAILED" + err.message);
            console.info(TAG + "Entering Remove photo Output ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
      }
      done();
    })

    /**
      * @tc.number    : ADD_INPUT_SUCCESS
      * @tc.name      : Add Input with camera0Input api
      * @tc.desc      : Add Input with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_INPUT_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_INPUT_SUCCESS to operate");
        captureSession.addInput(camera0Input, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_INPUT_SUCCESS addInput PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_INPUT_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering ADD_INPUT_SUCCESS ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_SUCCESS
      * @tc.name      : Add output with camera0Input api
      * @tc.desc      : Add output with camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('ADD_OUTPUT_PREVIEW_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS to operate");
        captureSession.addOutput(previewOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS : Success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS FAILED" + err.message);
            console.info(TAG + "Entering ADD_OUTPUT_PREVIEW_SUCCESS ends here");
          }
          await sleep(1000);
          done();
        })
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS to operate");
        captureSession.addOutput(photoOutputAsync, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS  success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS FAILED: " + err.message);
          }
          console.info(TAG + "Entering ADD_OUTPUT_PHOTO_SUCCESS ends here");
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS to operate");
        captureSession.commitConfig(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig FAILED" + err.message);
            console.info(TAG + "Entering COMMIT_CONFIG_SUCCESS commitConfig ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

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
      if (previewOutputAsync == null || previewOutputAsync == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START to operate");
        previewOutputAsync.on("frameStart", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewStart frameStart Callback is success");
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
      if (previewOutputAsync == null || previewOutputAsync == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END to operate");
        previewOutputAsync.on("frameEnd", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewStop frameEnd Callback is success");
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_START photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_START to operate");
        photoOutputAsync.on("captureStart", async (err, data) => {
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_END photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_CAPTURE_END to operate");
        photoOutputAsync.on("captureEnd", async (err, data) => {
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTO_CAP_CALLBACK_ON_FRAME_SHUTTER to operate");
        photoOutputAsync.on("frameShutter", async (err, data) => {
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
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS to operate");
        captureSession.start(async (err, data) => {
          console.info(TAG + "Entering captureSession.start success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS PASSED");
          }
          console.info(TAG + "Entering CAPTURE_SESSION_START_SUCCESS ends here");
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      console.info(TAG + "hasFlash called.")
      camera0Input.hasFlash(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering HAS_FLASH success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering HAS_FLASH data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering HAS_FLASH PASSED with HAS_FLASH is: " + data);
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering HAS_FLASH FAILED" + err.message);
        }
        console.info(TAG + "Entering HAS_FLASH ends here");
      })
      await sleep(1000);
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED data is not null || undefined");
            console.info(TAG + "FLASH_MODE_OPEN supported is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED PASSED");
          }
        } else {
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_OPEN);
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN PASSED")
          expect(true).assertTrue()
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_OPEN success");
          if (data == 1) {
            console.info(TAG + "GET_FLASH_MODE_OPEN data is not null || undefined: ");
            console.info(TAG + "Current FlashMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FLASH_MODE_OPEN PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_OPEN FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_OPEN ends here");
        }
        await sleep(1000);
        done();

      })
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering FLASH_MODE_ALWAYS_OPEN SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "FLASH_MODE_ALWAYS_OPEN supported is: " + data);
            console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED PASSED");
          }
        } else {
          console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN PASSED")
          expect(true).assertTrue()
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN success");
          if (data == 3) {
            console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN data is not null || undefined: ");
            expect(true).assertTrue();
            console.info(TAG + "Current FlashMode is: " + data);
            console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN PASSED");
          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN ends here");
        }
        await sleep(1000);
        done();
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS
      * @tc.name      : Photo output capture without photosettings api
      * @tc.desc      : Photo output capture without photosettings api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS', 0, async function (done) {
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS to operate");
        photoOutputAsync.capture(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutput capture without photosettings success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering photoOutput capture without photosettings data is not null || undefined");
              console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS PASSED");
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS FAILED" + err.message);
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITHOUT_PHOTOSETTINGS ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_CLOSE, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "FLASH_MODE_CLOSE supported is: " + data);
            console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED PASSED");
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_CLOSE, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_CLOSE);
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE PASSED")
          expect(true).assertTrue()
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_CLOSE success");
          if (data == 0) {
            console.info(TAG + "GET_FLASH_MODE_CLOSE data is not null || undefined: ");
            console.info(TAG + "Current FlashMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FLASH_MODE_CLOSE PASSED");
          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_CLOSE FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_CLOSE ends here");
        }
        await sleep(1000)
        done();

      })
      await sleep(1000)
      done()
    })

    /**
      * @tc.number    : IS_FLASH_MODE_AUTO_SUPPORTED
      * @tc.name      : check if flash mode auto is supported-camera0Input api
      * @tc.desc      : check if flash mode auto is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_AUTO_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED to operate");
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED data is not null || undefined");
            console.info(TAG + "IS_FLASH_MODE_AUTO_SUPPORTED supported is: " + data);
            expect(data).assertEqual(false);
            console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED PASSED");
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })

      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_AUTO
      * @tc.name      : set flash mode auto camera0 api
      * @tc.desc      : set flash mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_AUTO', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_AUTO to operate");
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_AUTO SUCCESS which is not supported: ");
          console.info(TAG + "Entering SET_FLASH_MODE_AUTO FAILED")
          expect().assertFail();
        } else {
          expect(true).assertTrue()
          console.info(TAG + "Entering SET_FLASH_MODE_AUTO PASSED: " + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_AUTO ends here");
        }
        await sleep(1000);
        done();
      })

      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_LOCKED_SUPPORTED
      * @tc.name      : check if focus mode locked is supported-camera0Input api
      * @tc.desc      : check if focus mode locked is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_LOCKED_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED to operate");
      camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_LOCKED, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED SUCCESS: " + data);
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED data is not null || undefined");
            console.info(TAG + "FOCUS_MODE_LOCKED_SUPPORTED is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FOCUS_MODE_LOCKED_SUPPORTED PASSED: ");
          }
        } else {
          console.info(TAG + "IS_FOCUS_MODE_LOCKED_SUPPORTED FAILED" + err.message);
          expect().assertFail();
          console.info(TAG + "IS_FOCUS_MODE_LOCKED_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })
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
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_LOCKED, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_LOCKED);
          console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED PASSED")
          expect(true).assertTrue()
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_LOCKED ends here");
        }
        await sleep(1000);
        done();
      })

      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_LOCKED
      * @tc.name      : get focus mode locked camera0 api
      * @tc.desc      : get focus mode locked camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FOCUS_MODE_LOCKED', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_LOCKED to operate");
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_LOCKED SUCCESS: " + data);
          if (data == 0) {
            console.info(TAG + "GET_FOCUS_MODE_LOCKED data is not null || undefined: ");
            console.info(TAG + "Current FocusMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_LOCKED PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_LOCKED FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_LOCKED ends here");
        }
        await sleep(1000);
        done();

      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_MANUAL_SUPPORTED
      * @tc.name      : check if focus mode manual is supported-camera0Input api
      * @tc.desc      : check if focus mode manual is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_MANUAL_SUPPORTED', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED to operate");
      camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_MANUAL, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED data is not null || undefined");
            console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED PASSED: ");
          }
        } else {
          console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED FAILED" + err.message);
          expect().assertFail();
          console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })
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
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_MANUAL, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_MANUAL);
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL PASSED")
          expect(true).assertTrue()
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL SUCCESS");
          if (data == 0) {
            console.info(TAG + "GET_FOCUS_MODE_MANUAL data is not null || undefined: ");
            console.info(TAG + "Current FocusMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_MANUAL PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_MANUAL FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_MANUAL ends here");
        }
        await sleep(1000);
        done();

      })
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
      camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED data is not null || undefined");
            console.info(TAG + "FOCUS_MODE_CONTINOUS_SUPPORTED is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED PASSED: ");
          }
        } else {
          console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED FAILED" + err.message);
          expect().assertFail();
          console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })
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
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
          expect(true).assertTrue()
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS PASSED")
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS SUCCESS");
          if (data == 1) {
            console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS data is not null || undefined: ");
            console.info(TAG + "Current FocusMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS ends here");
        }
        await sleep(1000);
        done();

      })
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
      camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED data is not null || undefined");
            console.info(TAG + "FOCUS_MODE_AUTO_SUPPORTED is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED PASSED: ");
          }
        } else {
          console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED FAILED" + err.message);
          expect().assertFail();
          console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED ends here");
        }
        await sleep(1000);
        done();
      })
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
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_AUTO);
          if (data != null || data != undefined) {
            expect(true).assertTrue()
            console.info(TAG + "Entering SET_FOCUS_MODE_AUTO PASSED")
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO ends here");
        }
        await sleep(1000);
        done();
      })

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
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_AUTO SUCCESS");
          if (data == 2) {
            console.info(TAG + "GET_FOCUS_MODE_AUTO data is not null || undefined: ");
            console.info(TAG + "Current FocusMode is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_AUTO PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_AUTO FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_AUTO ends here");
        }
        await sleep(1000);
        done();

      })
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        photoOutputAsync.capture(photosettings1, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutput capture with Rotation-0 & Quality-0 success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering photoOutput capture with photosettings1 data is not null || undefined");
              console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS  Rotation-0 & Quality-0 PASSED");
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-0 & Quality-0 FAILED" + err.message);
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-0 & Quality-0 ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        photoOutputAsync.capture(photosettings2, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutput capture with Rotation-90 & Quality-1 success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering photoOutput capture with photosettings2 data is not null || undefined");
              console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS  Rotation-90 & Quality-1 PASSED");
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-90 & Quality-1 FAILED" + err.message);
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-90 & Quality-1 ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        photoOutputAsync.capture(photosettings3, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutput capture with Rotation-180 & Quality-2 success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering photoOutput capture with photosettings3 data is not null || undefined");
              console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS  Rotation-180 & Quality-2 PASSED");
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-180 & Quality-2 FAILED" + err.message);
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-180 & Quality-2 ends here");
          }
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : GET_ZOOM_RATIO
      * @tc.name      : get zoom ratio camera-0 cameraId api
      * @tc.desc      : get zoom ratio camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_ZOOM_RATIO', 0, async function (done) {
      console.info("--------------GET_ZOOM_RATIO--------------");
      camera0Input.getZoomRatioRange(async (err, data) => {
        if (!err) {
          if (data != null && data != undefined) {
            console.info(TAG + "Entering GET_ZOOM_RATIO data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering GET_ZOOM_RATIO Success " + data)
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering GET_ZOOM_RATIO FAILED: " + err.message);
        }
        console.info(TAG + "Entering GET_ZOOM_RATIO ends here");
        await sleep(1000);
        done();
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_1_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_1_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(1, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 1");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(1);
              console.info(TAG + "SET_GET_ZOOM_1_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_1_ASYNC FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_2_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_2_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(2, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 2");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(2);
              console.info(TAG + "SET_GET_ZOOM_2_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_2_ASYNC FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_3_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_3_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(3, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 3");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(3);
              console.info(TAG + "SET_GET_ZOOM_3_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_3_ASYNC FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_4_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_4_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(4, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 4");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(4);
              console.info(TAG + "SET_GET_ZOOM_4_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_4_ASYNC FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_5_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_5_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(5, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 5");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(5);
              console.info(TAG + "SET_GET_ZOOM_5_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_5_ASYNC FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : SET_GET_ZOOM_6_ASYNC
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_6_ASYNC', 0, async function (done) {
      camera0Input.setZoomRatio(6, (err, data) => {
        if (!err) {
          console.info(TAG + "setZoomRatio success: 6");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err, data1) => {
            if (!err) {
              console.info(TAG + "getZoomRatio success : " + data1);
              expect(data1).assertEqual(6);
              console.info(TAG + "SET_GET_ZOOM_6_ASYNC PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_6_ASYNC FAILED" + err.message);
            }
          })
        }
      })
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
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS photoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS to operate");
        photoOutputAsync.capture(photosettings4, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutput capture with Rotation-270 success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering photoOutput capture with photosettings4 data is not null || undefined");
              console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-270 PASSED");
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-270 FAILED" + err.message);
            console.info(TAG + "Entering PHOTOOUTPUT_CAPTURE_WITH_PHOTOSETTINGS Rotation-270 ends here");
          }
        })
        await sleep(1000);
        done();
      }
    })

    /*CaptureSession APIs test script*/
    /**
      * @tc.number    : CAPTURE_SESSION_STOP
      * @tc.name      : capture session stop api
      * @tc.desc      : capture session stop api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAPTURE_SESSION_STOP', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_STOP captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_STOP to operate");
        captureSession.stop(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering CAPTURE_SESSION_STOP captureSession.stop success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering  captureSession.stop data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering CAPTURE_SESSION_STOP captureSession.stop PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering CAPTURE_SESSION_STOP FAILED" + err.message);
            console.info(TAG + "Entering CAPTURE_SESSION_STOP ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_RELEASE
      * @tc.name      : capture session release api
      * @tc.desc      : capture session release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAPTURE_SESSION_RELEASE', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_RELEASE captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_RELEASE to operate");
        captureSession.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering captureSession.release success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering captureSession.release data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering CAPTURE_SESSION_RELEASE PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering CAPTURE_SESSION_RELEASE FAILED: " + err.message);
            console.info(TAG + "Entering CAPTURE_SESSION_RELEASE ends here");
            await sleep(1000);
            done();
          }
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : PHOTOOUPUT_RELEASE
      * @tc.name      : photoOutput release api
      * @tc.desc      : photoOutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PHOTOOUPUT_RELEASE', 0, async function (done) {
      if (photoOutputAsync == null || photoOutputAsync == undefined) {
        console.info(TAG + "Entering PHOTOOUPUT_RELEASE photoOutputAsync == null || undefined");
      } else {
        console.info(TAG + "Entering PHOTOOUPUT_RELEASE to operate");
        photoOutputAsync.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering photoOutputAsync.release success");
            expect(true).assertTrue();
            console.info(TAG + "Entering PHOTOOUPUT_RELEASE PASSED");
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PHOTOOUPUT_RELEASE FAILED: " + err.message);
            console.info(TAG + "Entering  photoOutputAsync.release ends here");
          }
          await sleep(1000);
          done();
        })
        await sleep(1000);
        done();
      }
    })

    /**
      * @tc.number    : PREVIEWOUPUT_RELEASE
      * @tc.name      : previewOutput release api
      * @tc.desc      : previewOutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEWOUPUT_RELEASE', 0, async function (done) {
      if (previewOutputAsync == null || previewOutputAsync == undefined) {
        console.info(TAG + "Entering PREVIEWOUPUT_RELEASE previewOutputAsync == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEWOUPUT_RELEASE to operate");
        previewOutputAsync.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering previewOutputAsync.release success");
            console.info(TAG + "Entering previewOutputAsync.release data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering PREVIEWOUPUT_RELEASE PASSED");
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PREVIEWOUPUT_RELEASE FAILED: " + err.message);
            console.info(TAG + "Entering  PREVIEWOUPUT_RELEASE ends here");
            await sleep(1000);
            done();
          }
        })
        await sleep(1000);
        done();
      }
      await sleep(1000);
      done();
    })

    /**
      * @tc.number    : CAMERAINPUT_RELEASE_SUCCESS
      * @tc.name      : camera Input release api
      * @tc.desc      : camera Input release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('CAMERAINPUT_RELEASE_SUCCESS', 0, async function (done) {
      if (camera0Input == null || camera0Input == undefined) {
        console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS camera0Input == null || undefined");
      } else {
        console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS to operate");
        camera0Input.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering camera0Input.release success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering camera0Input.release data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS ends here");
            await sleep(1000);
            done();
          }
        })
        await sleep(1000);
        done();
      }
    })
  })
}