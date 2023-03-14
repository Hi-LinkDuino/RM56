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

import cameraObj from '@ohos.multimedia.camera'
import media from '@ohos.multimedia.media'
import mediaLibrary from '@ohos.multimedia.mediaLibrary'
import fileio from '@ohos.fileio';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

let TAG = 'CameraModuleTest: '
var cameraManager
var camerasArray
var camera0Input
var previewOutput
var videoRecorder
let fdPath;
let fileAsset;
let fdNumber;
let configFile = {
  audioBitrate: 48000,
  audioChannels: 2,
  audioCodec: 'audio/mp4a-latm',
  audioSampleRate: 48000,
  durationTime: 1000,
  fileFormat: 'mp4',
  videoBitrate: 48000,
  videoCodec: 'video/mp4v-es',
  videoFrameWidth: 640,
  videoFrameHeight: 480,
  videoFrameRate: 10
}

let videoConfig = {
  audioSourceType: 1,
  videoSourceType: 0,
  profile: configFile,
  url: 'file:///data/media/01.mp4',
  orientationHint: 0,
  location: { latitude: 30, longitude: 130 },
  maxSize: 100,
  maxDuration: 500
}
var videoId
var videoOutput
var captureSession

export default function cameraJSUnitVideoAsync(surfaceId: any) {
  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve(1)
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
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

  async function getFd(pathName) {
    let displayName = pathName;
    const mediaTest = mediaLibrary.getMediaLibrary();
    let fileKeyObj = mediaLibrary.FileKey;
    let mediaType = mediaLibrary.MediaType.VIDEO;
    let publicPath = await mediaTest.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
    let dataUri = await mediaTest.createAsset(mediaType, displayName, publicPath);
    if (dataUri != undefined) {
      let args = dataUri.id.toString();
      let fetchOp = {
        selections: fileKeyObj.ID + "=?",
        selectionArgs: [args],
      }
      let fetchFileResult = await mediaTest.getFileAssets(fetchOp);
      fileAsset = await fetchFileResult.getAllObject();
      fdNumber = await fileAsset[0].open('Rw');
      fdPath = "fd://" + fdNumber.toString();
    }
  }

  async function closeFd() {
    if (fileAsset != null) {
      await fileAsset[0].close(fdNumber).then(() => {
        console.info('[mediaLibrary] case close fd success');
      }).catch((err) => {
        console.info('[mediaLibrary] case close fd failed');
      });
    } else {
      console.info('[mediaLibrary] case fileAsset is null');
    }
  }

  async function getvideosurface() {
    await getFd('02.mp4');
    videoConfig.url = fdPath;
    media.createVideoRecorder((err, recorder) => {
      console.info(TAG + 'createVideoRecorder called')
      videoRecorder = recorder
      console.info(TAG + 'videoRecorder is :' + JSON.stringify(videoRecorder))

      console.info(TAG + 'videoRecorder.prepare called.')
      videoRecorder.prepare(videoConfig, (err) => {
        console.info(TAG + 'videoRecorder.prepare success.')
      })

      videoRecorder.getInputSurface((err, id) => {
        console.info(TAG + 'getInputSurface called')
        videoId = id
        console.info(TAG + 'getInputSurface surfaceId: ' + JSON.stringify(videoId))
      })
    })
  }

  describe('VideoModeAsync', function () {
    console.info(TAG + '----------Camera-VideoMode-Async--------------')

    beforeAll(async function () {
      await applyPermission();
      console.info('beforeAll case');
    })

    beforeEach(function () {
      sleep(5);
      console.info('beforeEach case');
    })

    afterEach(async function () {
      await closeFd();
      console.info('afterEach case');
    })

    afterAll(function () {
      console.info('afterAll case');
    })

    /**
      * @tc.number    : GET_CAMERA_MANAGER_TC
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER_TC', 0, async function (done) {
      console.info('--------------GET_CAMERA_MANAGER_TC--------------')
      await sleep(1)
      cameraObj.getCameraManager(null, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC data is not null || undefined')
            cameraManager = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC PASSED')
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC ends here')
        done()
      })

      await sleep(1)
      done()
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
        console.info(TAG + 'Entering CAMERA_STATUS_CALLBACK cameraManager == null || undefined')
      } else {
        console.info(TAG + 'Entering CAMERA_STATUS_CALLBACK to operate')
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
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : GET_CAMERAS
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS', 0, async function (done) {
      console.info('--------------GET_CAMERAS--------------')
      await sleep(1)
      cameraManager.getCameras((err, data) => {
        if (!err) {
          console.info(TAG + 'Entering GET_CAMERAS success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering GET_CAMERAS data is not null || undefined')
            camerasArray = data
            if (camerasArray != null && camerasArray.length > 0) {
              for (var i = 0; i < camerasArray.length; i++) {
                // Get the variables from camera object
                var cameraId = camerasArray[i].cameraId
                console.info(TAG + 'Entering GET_CAMERAS camera' + i + 'Id: ' + cameraId)
                var cameraPosition = camerasArray[i].cameraPosition
                console.info(TAG + 'Entering GET_CAMERAS camera' + i + 'Position: ' + cameraPosition)
                var cameraType = camerasArray[i].cameraType
                console.info(TAG + 'Entering GET_CAMERAS camera' + i + 'Type: ' + cameraType)
                var connectionType = camerasArray[i].connectionType
                console.info(TAG + 'Entering GET_CAMERAS connection' + i + 'Type: ' + connectionType)
              }
              expect(true).assertTrue()
              console.info(TAG + 'Entering GET_CAMERAS PASSED')
            } else {
              expect().assertFail()
              console.info(TAG + 'Entering GET_CAMERAS FAILED cameraArray is null || undefined')
            }
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering GET_CAMERAS FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering GET_CAMERAS ends here')
        done()
      })

      await sleep(1)
      done()
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
      console.info('--------------CAMERA-0 STARTS HERE--------------')
      console.info('--------------CREATE_CAMERA_INPUT--------------')
      await sleep(1)
      cameraManager.createCameraInput(camerasArray[0].cameraId, (err, data) => {
        if (!err) {
          if (data != null && data != undefined) {
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT data is not null || undefined')
            camera0Input = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT PASSED with CameraID :' + camerasArray[0].cameraId)
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering CREATE_CAMERA_INPUT FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT ends here')
        done()
      })
      await sleep(1)
      done()
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
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR camera0Input == null || undefined");
      } else {
        console.info(TAG + "Entering CAMERA_INPUT_CALLBACK_ON_ERROR to operate");
        camera0Input.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "camera0Input error callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during camera0Input with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : CREATE_PREVIEW_OUTPUT
      * @tc.name      : Create previewoutput async api
      * @tc.desc      : Create previewoutput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_PREVIEW_OUTPUT', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT to operate')
      await sleep(1)
      cameraObj.createPreviewOutput(surfaceId, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT data is not null || undefined')
            previewOutput = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT PASSED')
          }
        } else {
          console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT ends here')
        done()
      })
      await sleep(1)
      done()
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
      if (previewOutput == null || previewOutput == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR to operate");
        previewOutput.on("error", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewOutputError callback is success");
            if (data != null || data != undefined) {
              console.info(TAG + "Error during PreviewOutput with ErrorCode: " + data.code);
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : CREATE_VIDEO_OUTPUT
      * @tc.name      : Create videooutput async api
      * @tc.desc      : Create videooutput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_VIDEO_OUTPUT', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT to operate')
      await getvideosurface()
      await sleep(2)
      cameraObj.createVideoOutput(videoId, (err, data) => {
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT success')
        if (data != null || data != undefined) {
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT data is not null || undefined')
          videoOutput = data
          expect(true).assertTrue()
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT PASSED')
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT ends here')
        done()
      })
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_ERROR
      * @tc.name      : VideoOutput callback onerror async api
      * @tc.desc      : VideoOutput callback onerror async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_ERROR', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR to operate')
        await sleep(1)
        videoOutput.on('error', async (err, data) => {
          if (!err) {
            console.info(TAG + "VideoOutput Errorcallback is success")
            if (data != null || data != undefined) {
              console.info(TAG + "Error during videoOutput with ErrorCode: " + data.code);
              expect(true).assertTrue()
            }
          } else {
            expect().assertFail()
          }
          await sleep(1)
          done()
        })
        await sleep(1)
        done();
      }
    })

    /**
      * @tc.number    : CREATE_CAPTURE_SESSION
      * @tc.name      : Create capturesession async api
      * @tc.desc      : Create capturesession async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAPTURE_SESSION', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_CAPTURE_SESSION to operate')
      await sleep(1)
      cameraObj.createCaptureSession(null, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering CREATE_CAPTURE_SESSION success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering CREATE_CAPTURE_SESSION data is not null || undefined')
            captureSession = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_CAPTURE_SESSION PASSED')
          }
        } else {
          console.info(TAG + 'Entering CREATE_CAPTURE_SESSION FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_CAPTURE_SESSION ends here')
        done()
      })
      await sleep(1)
      done()
    })

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
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : BEGIN_CONFIG
      * @tc.name      : Create capturesession async api
      * @tc.desc      : Create capturesession async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('BEGIN_CONFIG', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering BEGIN_CONFIG captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering BEGIN_CONFIG to operate')
        await sleep(1)
        captureSession.beginConfig((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering BEGIN_CONFIG success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering BEGIN_CONFIG data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering BEGIN_CONFIG PASSED')
            }
          } else {
            console.info(TAG + 'Entering BEGIN_CONFIG FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering BEGIN_CONFIG ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_INPUT
      * @tc.name      : AddInput async api
      * @tc.desc      : AddInput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_INPUT', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_INPUT captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_INPUT to operate')
        await sleep(1)
        captureSession.addInput(camera0Input, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_INPUT success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_INPUT data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_INPUT PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_INPUT FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_INPUT ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW
      * @tc.name      : AddOutput preview async api
      * @tc.desc      : AddOutput preview async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_PREVIEW', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW to operate')
        await sleep(1)
        captureSession.addOutput(previewOutput, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_VIDEO
      * @tc.name      : AddOutput video async api
      * @tc.desc      : AddOutput video async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_VIDEO', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO to operate')
        await sleep(1)
        captureSession.addOutput(videoOutput, async (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_VIDEO ends here')
          await sleep(1);
          done()
        })
        await sleep(1)
        done()
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
          await sleep(1);
          done();
        })
        await sleep(1);
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
        captureSession.removeOutput(previewOutput, async (err, data) => {
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
          await sleep(1);
          done();
        })
        await sleep(1);
      }
      done();
    })

    /**
      * @tc.number    : REMOVE_VIDEO_OUTPUT_SUCCESS
      * @tc.name      : Remove video Output api
      * @tc.desc      : Remove video Output api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('REMOVE_VIDEO_OUTPUT_SUCCESS', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering REMOVE_VIDEO_OUTPUT_SUCCESS captureSession == null || undefined");
      } else {
        console.info(TAG + "Entering REMOVE_VIDEO_OUTPUT_SUCCESS to operate");
        captureSession.removeOutput(videoOutput, async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering remove video Output success");
            expect(true).assertTrue();
            console.info(TAG + "Entering REMOVE_VIDEO_OUTPUT_SUCCESS PASSED");
          }
          else {
            expect().assertFail();
            console.info(TAG + "Entering Remove video Output FAILED" + err.message);
            console.info(TAG + "Entering Remove video Output ends here");
          }
          await sleep(1);
          done();
        })
        await sleep(1);
      }
      done();
    })

    /**
      * @tc.number    : ADD_INPUT1
      * @tc.name      : AddInput async api
      * @tc.desc      : AddInput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_INPUT1', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_INPUT1 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_INPUT1 to operate')
        await sleep(1)
        captureSession.addInput(camera0Input, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_INPUT1 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_INPUT1 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_INPUT1 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_INPUT1 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_INPUT1 ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW1
      * @tc.name      : AddOutput preview async api
      * @tc.desc      : AddOutput preview async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_PREVIEW1', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 to operate')
        await sleep(1)
        captureSession.addOutput(previewOutput, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW1 ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_VIDEO1
      * @tc.name      : AddOutput video async api
      * @tc.desc      : AddOutput video async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_VIDEO1', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 to operate')
        await sleep(1)
        captureSession.addOutput(videoOutput, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_VIDEO1 ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : COMMIT_CONFIG
      * @tc.name      : CommitConfig async api
      * @tc.desc      : CommitConfig async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('COMMIT_CONFIG', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering COMMIT_CONFIG captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering COMMIT_CONFIG to operate')
        await sleep(1)
        captureSession.commitConfig((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering COMMIT_CONFIG success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering COMMIT_CONFIG data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering COMMIT_CONFIG PASSED')
            }
          } else {
            console.info(TAG + 'Entering COMMIT_CONFIG FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering COMMIT_CONFIG ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    //callback API

    /**
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START
      * @tc.name      : Preview output callback on frame start api
      * @tc.desc      : Preview output callback on frame start api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START', 0, async function (done) {
      if (previewOutput == null || previewOutput == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START to operate");
        previewOutput.on("frameStart", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewStart frameStart Callback is success");
            if (data != null || data != undefined) {
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail()
          }
          await sleep(1);
          done();
        })
        await sleep(1);
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
      if (previewOutput == null || previewOutput == undefined) {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END to operate");
        previewOutput.on("frameEnd", async (err, data) => {
          if (!err) {
            console.info(TAG + "PreviewStop frameEnd Callback is success");
            if (data != null || data != undefined) {
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail();
          }
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_START
      * @tc.name      : VideoOutput callback onframestart async api
      * @tc.desc      : VideoOutput callback onframestart async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_START', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + "Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START videoOutput == null || undefined");
      } else {
        console.info(TAG + "Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START to operate");
        videoOutput.on("frameStart", async (err, data) => {
          if (!err) {
            console.info(TAG + "Video frameStart Callback is success");
            if (data != null || data != undefined) {
              expect(true).assertTrue();
            }
          } else {
            expect().assertFail()
          }
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_END
      * @tc.name      : VideoOutput callback onframeend async api
      * @tc.desc      : VideoOutput callback onframeend async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_END', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END to operate')
        await sleep(1)
        videoOutput.on('frameEnd', async (err, data) => {
          if (!err) {
            console.info(TAG + 'Video frameEnd callback is success')
            if (data != null || data != undefined) {
              expect(true).assertTrue()
            }
          } else {
            expect().assertFail()
          }
          await sleep(1);
          done();
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_START
      * @tc.name      : CaptureSession start async api
      * @tc.desc      : CaptureSession start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_START', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + "Entering CAPTURE_SESSION_START captureSession == null || undefined")
      } else {
        console.info(TAG + "Entering CAPTURE_SESSION_START to operate")
        await sleep(1)
        captureSession.start((err, data) => {
          if (!err) {
            console.info(TAG + "Entering CAPTURE_SESSION_START success")
            if (data != null || data != undefined) {
              console.info(TAG + "Entering CAPTURE_SESSION_START data is not null || undefined")
              expect(true).assertTrue()
              console.info(TAG + "Entering CAPTURE_SESSION_START PASSED")
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_START FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_START ends here')
          done()
        })
        await sleep(1)
        done()
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
      console.info("--------------HAS_FLASH--------------");
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
      await sleep(1);
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
          expect().assertFail();
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
          expect().assertFail();
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
      done();
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
          console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_AUTO_SUPPORTED ends here");
          expect().assertFail();
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED ends here");
          expect().assertFail();
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1)
        done();
      })
      await sleep(1)
      done()
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();

      })
      await sleep(1);
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
    it('IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 to operate");
      camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 data is not null || undefined");
            console.info(TAG + "FOCUS_MODE_CONTINOUS_SUPPORTED is: " + data);
            expect(true).assertTrue();
            console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 PASSED: ");
          }
        } else {
          console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 FAILED" + err.message);
          expect().assertFail();
          console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 ends here");
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
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
          expect(true).assertTrue()
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO PASSED")
        }
        else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO ends here");
        }
        await sleep(1);
        done();
      })
      await sleep(1);
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
        await sleep(1);
        done();
      })
      await sleep(1);
      done();
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
            console.info(TAG + "Entering GET_ZOOM_RATIO Success: " + data);
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering GET_ZOOM_RATIO FAILED: " + err.message);
        }
        console.info(TAG + "Entering GET_ZOOM_RATIO ends here");
        await sleep(1);
        done();
      })
      await sleep(1);
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
      await sleep(1);
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
      await sleep(1);
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
      await sleep(1);
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
      await sleep(1);
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
      await sleep(1);
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
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_START
      * @tc.name      : VideoOutput start async api
      * @tc.desc      : VideoOutput start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_START', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START to operate')
        await sleep(1)
        videoOutput.start(async (err, data) => {
          console.info(TAG + 'Entering VIDEO_OUTPUT_START success: ' + JSON.stringify(data))
          if (data == undefined) {
            expect(true).assertTrue();
          } else {
            expect().assertFail()
          }
        })
        await sleep(1)
        done()
      }
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : VIDEO_RECORDER_START
      * @tc.name      : VideoRecorder start async api
      * @tc.desc      : VideoRecorder start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_START', 0, async function (done) {
      if (videoRecorder == null || videoRecorder == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_START videoRecorder == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_START to operate')
        videoRecorder.start()
        console.info(TAG + 'VIDEO_RECORDER_START called');
        sleep(3);
        console.info(TAG + 'VIDEO_RECORDER_START done.')
        expect(true).assertTrue()
        console.info(TAG + 'Entering VIDEO_RECORDER_START PASSED')
        console.info(TAG + 'Entering VIDEO_RECORDER_START ends here')
        await sleep(1)
        done()
      }
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_STOP
      * @tc.name      : VideoOutput stop async api
      * @tc.desc      : VideoOutput stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_STOP', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP to operate')
        await sleep(1)
        videoOutput.stop(async (err, data) => {
          console.info(TAG + 'Entering VIDEO_OUTPUT_STOP success: ' + JSON.stringify(data))
          if (data == undefined) {
            expect(true).assertTrue()
          } else {
            expect().assertFail()
          }
          console.info(TAG + 'Entering VIDEO_OUTPUT_STOP ends here')
          await sleep(1)
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : VIDEO_RECORDER_STOP
      * @tc.name      : VideoRecorder stop async api
      * @tc.desc      : VideoRecorder stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_STOP', 0, async function (done) {
      if (videoRecorder == null || videoRecorder == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP videoRecorder == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP to operate')
        await videoRecorder.stop()
        console.info(TAG + 'VIDEO_RECORDER_STOP stopVideo done.')
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP PASSED')
        expect(true).assertTrue()
      }
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CAPTURE_SESSION_STOP
      * @tc.name      : CaptureSession stop async api
      * @tc.desc      : CaptureSession stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_STOP', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP to operate')
        await sleep(1)
        captureSession.stop((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering CAPTURE_SESSION_STOP success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering CAPTURE_SESSION_STOP data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering CAPTURE_SESSION_STOP PASSED')
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_STOP FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_STOP ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_RELEASE
      * @tc.name      : CaptureSession release async api
      * @tc.desc      : CaptureSession release async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_RELEASE', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE to operate')
        await sleep(1)
        captureSession.release((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE PASSED')
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE ends here')
          done()
        })
        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : VIDEOOUPUT_RELEASE_SUCCESS
      * @tc.name      : videooutput release api
      * @tc.desc      : videooutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('VIDEOOUPUT_RELEASE_SUCCESS', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + "Entering VIDEOOUPUT_RELEASE_SUCCESS previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering VIDEOOUPUT_RELEASE_SUCCESS to operate");
        videoOutput.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering videooutput.release success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering videooutput.release data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering videooutput.release PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering VIDEOOUPUT_RELEASE_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering  videooutput.release ends here");
            await sleep(1);
            done();
          }
        })
        await sleep(1);
        done();
      }
    })

    /**
      * @tc.number    : PREVIEWOUPUT_RELEASE_SUCCESS
      * @tc.name      : previewOutput release api
      * @tc.desc      : previewOutput release api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('PREVIEWOUPUT_RELEASE_SUCCESS', 0, async function (done) {
      if (previewOutput == null || previewOutput == undefined) {
        console.info(TAG + "Entering PREVIEWOUPUT_RELEASE_SUCCESS previewOutput == null || undefined");
      } else {
        console.info(TAG + "Entering PREVIEWOUPUT_RELEASE_SUCCESS to operate");
        previewOutput.release(async (err, data) => {
          if (!err) {
            console.info(TAG + "Entering previewOutput.release success");
            if (data != null || data != undefined) {
              console.info(TAG + "Entering previewOutput.release data is not null || undefined");
              expect(true).assertTrue();
              console.info(TAG + "Entering previewOutput.release PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering PREVIEWOUPUT_RELEASE_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering  previewOutput.release ends here");
            await sleep(1);
            done();
          }
        })
        await sleep(1);
        done();
      }
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
              console.info(TAG + "Entering camera0Input.release PASSED");
            }
          } else {
            expect().assertFail();
            console.info(TAG + "Entering CAMERAINPUT_RELEASE_SUCCESS FAILED: " + err.message);
            console.info(TAG + "Entering  camera0Input.release ends here");
            await sleep(1);
            done();
          }
        })
        await sleep(1);
        done();
      }
    })
  })
}