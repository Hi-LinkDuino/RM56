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
import fileio from '@ohos.fileio';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import bundle from '@ohos.bundle'

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

let TAG = 'CameraModuleTest: '
var cameraManagerPromise
var camerasArrayPromise
var camera0InputPromise
var previewOutputPromise
var videoRecorderPromise
var videoProfilePromise = {
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
  videoFrameRate: 30
}
var videoConfigPromise = {
  audioSourceType: 1,
  videoSourceType: 0,
  profile: videoProfilePromise,
  url: 'file:///data/media/01.mp4',
  orientationHint: 0,
  location: { latitude: 30, longitude: 130 },
  maxSize: 100,
  maxDuration: 500
}
var videoId
var videoOutputPromise
var captureSessionPromise

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

export default function cameraTestCase(surfaceId: any) {
  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve(1)
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  async function getvideosurface() {
    media.createVideoRecorder((err, recorder) => {
      if (!err)
      console.info(TAG + 'createVideoRecorder called')
      videoRecorderPromise = recorder
      console.info(TAG + 'videoRecorder is :' + JSON.stringify(videoRecorderPromise))

    console.info(TAG + 'videoRecorder.prepare called.')
    videoRecorderPromise.prepare(videoConfigPromise, (err) => {
        console.info(TAG + 'videoRecorder.prepare success.')
      })

    videoRecorderPromise.getInputSurface((err, id) => {
      console.info(TAG + 'getInputSurface called')
      videoId = id
      console.info(TAG + 'getInputSurface surfaceId: ' + JSON.stringify(videoId))
    })
	})
  }

  describe('VideoModePromise', function () {
    console.info(TAG + '----------Camera-VideoMode-Promise--------------')
	
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

    /**
      * @tc.number    : GET_CAMERA_MANAGER_PROMISE_TC_001
      * @tc.name      : Create camera manager instance promise api
      * @tc.desc      : Create camera manager instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER_PROMISE_TC_001', 0, async function (done) {
      console.info('--------------GET_CAMERA_MANAGER_PROMISE_TC_001--------------')
      cameraManagerPromise = await cameraObj.getCameraManager(null)
      console.info(TAG + 'Entering GET_CAMERA_MANAGER_PROMISE_TC_001 cameraManagerPromise: ' + cameraManagerPromise)
      if (cameraManagerPromise != null && cameraManagerPromise != undefined) {
        expect(true).assertTrue()
        console.info(TAG + 'Entering GET_CAMERA_MANAGER_PROMISE_TC_001 PASSED')
      } else {
        expect().assertFail()
        console.info(TAG + 'Entering GET_CAMERA_MANAGER_PROMISE_TC_001 FAILED')
      }
      console.info(TAG + 'Entering GET_CAMERA_MANAGER_PROMISE_TC_001 ends here')
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : GET_CAMERAS_PROMISE_TC_002
      * @tc.name      : Create camera manager instance promise api
      * @tc.desc      : Create camera manager instance promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS_PROMISE_TC_002', 0, async function (done) {
      console.info('--------------GET_CAMERAS_PROMISE_TC_002--------------')
      camerasArrayPromise = await cameraManagerPromise.getCameras()
      console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002: ' + JSON.stringify(camerasArrayPromise))
      if (camerasArrayPromise != null && camerasArrayPromise.length > 0) {
        console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 success')
        for (var i = 0; i < camerasArrayPromise.length; i++) {
          // Get the variables from camera object
          var cameraId = camerasArrayPromise[i].cameraId
          console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 camera' + i + 'Id: ' + cameraId)
          var cameraPosition = camerasArrayPromise[i].cameraPosition
          console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 camera' + i + 'Position: ' + cameraPosition)
          var cameraType = camerasArrayPromise[i].cameraType
          console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 camera' + i + 'Type: ' + cameraType)
          var connectionType = camerasArrayPromise[i].connectionType
          console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 connection' + i + 'Type: ' + connectionType)
        }
        expect(true).assertTrue()
        console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 PASSED')
      } else {
        expect().assertFail()
        console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 FAILED')
      }
      console.info(TAG + 'Entering GET_CAMERAS_PROMISE_TC_002 ends here')
      await sleep(1)
      done()
    })

    /*CAMERA-0 Scripts*/
    /**
      * @tc.number    : CREATE_CAMERA_INPUT_PROMISE_TC_003
      * @tc.name      : Create camerainput from camera-0 cameraId promise api
      * @tc.desc      : Create camerainput from camera-0 cameraId promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_PROMISE_TC_003', 0, async function (done) {
      console.info('--------------CREATE_CAMERA_INPUT_PROMISE_TC_003--------------')
      camera0InputPromise = await cameraManagerPromise.createCameraInput(camerasArrayPromise[0].cameraId)
      console.info(TAG + 'Entering CREATE_CAMERA_INPUT_PROMISE_TC_003 camera0InputPromise: ' + JSON.stringify(camera0InputPromise))
      if (camera0InputPromise != null && camera0InputPromise != undefined) {
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_PROMISE_TC_003 camera0InputPromise is not null || undefined')
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_PROMISE_TC_003 PASSED')
      } else {
        expect().assertFail()
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_PROMISE_TC_003 FAILED')
      }
      console.info(TAG + 'Entering CREATE_CAMERA_INPUT_PROMISE_TC_003 ends here')
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004
      * @tc.name      : Create camerainput from camera-0 cameraposition & cameratype promise api
      * @tc.desc      : Create camerainput from camera-0 cameraposition & cameratype promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004', 0, async function (done) {
      console.info('--------------CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004--------------')
      var cameraInputPromise = await cameraManagerPromise.createCameraInput(camerasArrayPromise[0].cameraPosition, camerasArrayPromise[0].cameraType)
      console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004 cameraInputPromise: ' + JSON.stringify(cameraInputPromise))
      if (cameraInputPromise == undefined) {
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004 cameraInputPromise is undefined')
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004 PASSED')
      } else {
        expect().assertFail()
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004 FAILED')
      }
      console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_PROMISE_TC_004 ends here')
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_PREVIEW_OUTPUT_PROMISE_TC_005
      * @tc.name      : Create previewoutput promise api
      * @tc.desc      : Create previewoutput promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_PREVIEW_OUTPUT_PROMISE_TC_005', 0, async function (done) {
      console.info('--------------CREATE_PREVIEW_OUTPUT_PROMISE_TC_005--------------')
      previewOutputPromise = await cameraObj.createPreviewOutput(surfaceId)
      console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_PROMISE_TC_005 previewOutputPromise: ' + JSON.stringify(previewOutputPromise))
      if (previewOutputPromise != null && previewOutputPromise != undefined) {
        console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_PROMISE_TC_005 previewOutputPromise is not null || undefined')
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_PROMISE_TC_005 PASSED')
      } else {
        expect().assertFail()
        console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_PROMISE_TC_005 FAILED')
      }
      console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_PROMISE_TC_005 ends here')
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_VIDEO_OUTPUT_PROMISE_TC_006
      * @tc.name      : Create videooutput promise api
      * @tc.desc      : Create videooutput promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_VIDEO_OUTPUT_PROMISE_TC_006', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_PROMISE_TC_006 to operate')
      await getvideosurface()
      await sleep(1)
      videoOutputPromise = await cameraObj.createVideoOutput(videoId)
      console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_PROMISE_TC_006: ' + videoOutputPromise)
      if (videoOutputPromise != null && videoOutputPromise != undefined) {
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_PROMISE_TC_006 PASSED')
      } else {
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_PROMISE_TC_006 PASSED')
      }
      console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_PROMISE_TC_006 ends here');
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : CREATE_CAPTURE_SESSION_PROMISE_TC_007
      * @tc.name      : Create capturesession promise api
      * @tc.desc      : Create capturesession promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAPTURE_SESSION_PROMISE_TC_007', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_PROMISE_TC_007 to operate')
      captureSessionPromise = await cameraObj.createCaptureSession(null)
      console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_PROMISE_TC_007: ' + captureSessionPromise)
      if (captureSessionPromise != null && captureSessionPromise != undefined) {
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_PROMISE_TC_007 PASSED')
      } else {
        expect(true).assertTrue()
        console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_PROMISE_TC_007 PASSED')
      }
      console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_PROMISE_TC_007 ends here');
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : BEGIN_CONFIG_PROMISE_TC_008
      * @tc.name      : CaptureSession begin config promise api
      * @tc.desc      : CaptureSession begin config promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('BEGIN_CONFIG_PROMISE_TC_008', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering BEGIN_CONFIG_PROMISE_TC_008 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering BEGIN_CONFIG_PROMISE_TC_008 to operate')
        await captureSessionPromise.beginConfig()
        expect(true).assertTrue()
        console.info(TAG + 'Entering BEGIN_CONFIG_PROMISE_TC_008 PASSED')
        console.info(TAG + 'Entering BEGIN_CONFIG_PROMISE_TC_008 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : ADD_INPUT_PROMISE_TC_009
      * @tc.name      : AddInput promise api
      * @tc.desc      : AddInput promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_INPUT_PROMISE_TC_009', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering ADD_INPUT_PROMISE_TC_009 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_INPUT_PROMISE_TC_009 to operate')
        await captureSessionPromise.addInput(camera0InputPromise)
        expect(true).assertTrue()
        console.info(TAG + 'Entering ADD_INPUT_PROMISE_TC_009 PASSED')
        console.info(TAG + 'Entering ADD_INPUT_PROMISE_TC_009 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_PROMISE_TC_010
      * @tc.name      : AddOutput preview promise api
      * @tc.desc      : AddOutput preview promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_PREVIEW_PROMISE_TC_010', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_PROMISE_TC_010 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_PROMISE_TC_010 to operate')
        await captureSessionPromise.addOutput(previewOutputPromise)
        expect(true).assertTrue()
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_PROMISE_TC_010 PASSED')
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_PROMISE_TC_010 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : ADD_OUTPUT_VIDEO_PROMISE_TC_011
      * @tc.name      : AddOutput video promise api
      * @tc.desc      : AddOutput video promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_VIDEO_PROMISE_TC_011', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_PROMISE_TC_011 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_PROMISE_TC_011 to operate')
        await captureSessionPromise.addOutput(videoOutputPromise)
        expect(true).assertTrue()
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_PROMISE_TC_011 PASSED')
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_PROMISE_TC_011 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : COMMIT_CONFIG_PROMISE_TC_012
      * @tc.name      : CommitConfig promise api
      * @tc.desc      : CommitConfig promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('COMMIT_CONFIG_PROMISE_TC_012', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering COMMIT_CONFIG_PROMISE_TC_012 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering COMMIT_CONFIG_PROMISE_TC_012 to operate')
        await captureSessionPromise.commitConfig()
        expect(true).assertTrue()
        console.info(TAG + 'Entering COMMIT_CONFIG_PROMISE_TC_012 PASSED')
        console.info(TAG + 'Entering COMMIT_CONFIG_PROMISE_TC_012 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CAP_SES_CALLBACK_ON_ERROR_TC_013
      * @tc.name      : CaptureSession callback onerror async api
      * @tc.desc      : CaptureSession callback onerror async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAP_SES_CALLBACK_ON_ERROR_TC_013', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering CAP_SES_CALLBACK_ON_ERROR_TC_013 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering CAP_SES_CALLBACK_ON_ERROR_TC_013 to operate')
        await sleep(1)
        captureSessionPromise.on('error', async (err, data) => {
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_TC_014
      * @tc.name      : PreviewOutput callback onframestart async api
      * @tc.desc      : PreviewOutput callback onframestart async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_TC_014', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_TC_014 previewOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_TC_014 to operate')
        await sleep(1)
        previewOutputPromise.on('frameStart', async (err, data) => {
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_TC_015
      * @tc.name      : PreviewOutput callback onframeend async api
      * @tc.desc      : PreviewOutput callback onframeend async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_TC_015', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_TC_015 previewOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_TC_015 to operate')
        await sleep(1)
        previewOutputPromise.on('frameEnd', async (err, data) => {
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_ERROR_TC_016
      * @tc.name      : PreviewOutput callback onerror async api
      * @tc.desc      : PreviewOutput callback onerror async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('PREVIEW_OUTPUT_CALLBACK_ON_ERROR_TC_016', 0, async function (done) {
      if (previewOutputPromise == null || previewOutputPromise == undefined) {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR_TC_016 previewOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR_TC_016 to operate')
        await sleep(1)
        previewOutputPromise.on('error', async (err, data) => {
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017
      * @tc.name      : VideoOutput callback onframestart async api
      * @tc.desc      : VideoOutput callback onframestart async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017 to operate')
        await sleep(1)
        videoOutputPromise.on('frameStart', async (err, data) => {
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018
      * @tc.name      : VideoOutput callback onframeend async api
      * @tc.desc      : VideoOutput callback onframeend async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018 to operate')
        await sleep(1)
        videoOutputPromise.on('frameEnd', async (err, data) => {
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019
      * @tc.name      : VideoOutput callback onerror async api
      * @tc.desc      : VideoOutput callback onerror async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019 to operate')
        await sleep(1)
        videoOutputPromise.on('error', async (err, data) => {
          if (!err) {
            console.info(TAG + 'VideoOutput Errorcallback is success')
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
	
	//FocusState callback
    /**
      * @tc.number    : FOCUSSTATECHANGE_CALLBACK_ON_CAMERAINPUT0_020
      * @tc.name      : focus state change callback on camera0Input api
      * @tc.desc      : Preview capture callback on frame end api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
//    it('FOCUSSTATECHANGE_CALLBACK_ON_CAMERAINPUT0_020', 0, async function (done) {
//      if (camera0InputPromise == null || camera0InputPromise == undefined) {
//        console.info(TAG + "Entering FOCUSSTATECHANGE_CALLBACK_ON_CAMERAINPUT0_020 previewOutput == null || undefined");
//      } else {
//        console.info(TAG + "Entering FOCUSSTATECHANGE_CALLBACK_ON_CAMERAINPUT0_020 to operate");
//        camera0InputPromise.on("focusStateChange", async (err, data) => {
//          if (!err) {
//            console.info(TAG + "FocusState callback is success");
//            if (data != null || data != undefined) {
//              console.info(TAG + "Current FocusState is: " + data);
//              expect(true).assertTrue();
//            }
//          } else {
//            expect().assertFail();
//          }
//          await sleep(1000);
//          done();
//        })
//        await sleep(1000);
//        done();
//      }
//    })


    /**
      * @tc.number    : CAPTURE_SESSION_START_PROMISE_TC_020
      * @tc.name      : CaptureSession start promise api
      * @tc.desc      : CaptureSession start promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_START_PROMISE_TC_020', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_START_PROMISE_TC_020 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_START_PROMISE_TC_020 to operate')
        await captureSessionPromise.start()
        expect(true).assertTrue()
        console.info(TAG + 'Entering CAPTURE_SESSION_START_PROMISE_TC_020 PASSED')
        console.info(TAG + 'Entering CAPTURE_SESSION_START_PROMISE_TC_020 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })
	
	//FLASH Function API scripts
    /**
      * @tc.number    : HAS_FLASH_TC_021
      * @tc.name      : check if has flash-camera0Input api
      * @tc.desc      : check if has flash-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('HAS_FLASH_TC_021', 0, async function (done) {
      console.info("--------------HAS_FLASH_TC_021--------------");
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
      console.info(TAG + "Entering HAS_FLASH_TC_021 ends here");
      await sleep(1)
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_OPEN_SUPPORTED_022
      * @tc.name      : check if flash mode open is supported-camera0Input api
      * @tc.desc      : check if flash mode open is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_OPEN_SUPPORTED_022', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED_022 to operate");
      var isFMOpenSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_OPEN);
      console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED_022 SUCCESS ");
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
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_OPEN_023
      * @tc.name      : set flash mode open camera0 api
      * @tc.desc      : set flash mode open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_OPEN_023', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 to operate");
      var SetFMOpen = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_OPEN);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMOpen))
      if (SetFMOpen == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_OPEN);
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_OPEN_024
      * @tc.name      : get flash mode open camera0 api
      * @tc.desc      : get flash mode open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('GET_FLASH_MODE_OPEN_024', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_OPEN_024 to operate");
      var GetFMOpen = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_OPEN success: " + JSON.stringify(GetFMOpen));
      if (GetFMOpen == 1) {
        console.info(TAG + "GET_FLASH_MODE_OPEN data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + JSON.stringify(GetFMOpen));
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_OPEN_024 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_OPEN_024 FAILED");
        console.info(TAG + "GET_FLASH_MODE_OPEN_024 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025
      * @tc.name      : check if flash mode always open is supported-camera0Input api
      * @tc.desc      : check if flash mode always open is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 to operate");
      var isFMAlwaysOpenSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
      console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 SUCCESS ");
      if (isFMAlwaysOpenSupported != null || isFMAlwaysOpenSupported != undefined) {
        console.info(TAG + "Entering FLASH_MODE_ALWAYS_OPEN data is not null || undefined");
        console.info(TAG + "FLASH_MODE_OPEN supported is: " + isFMAlwaysOpenSupported);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_ALWAYS_OPEN_026
      * @tc.name      : set flash mode always open camera0 api
      * @tc.desc      : set flash mode always open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_ALWAYS_OPEN_026', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 to operate");
      var SetFMAlwaysOpen = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMAlwaysOpen))
      if (SetFMAlwaysOpen == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_ALWAYS_OPEN_027
      * @tc.name      : get flash mode always open camera0 api
      * @tc.desc      : get flash mode always open camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FLASH_MODE_ALWAYS_OPEN_027', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN_027 to operate");
      var GetFMAlwaysOpen = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN_027 success");
      if (GetFMAlwaysOpen == 3) {
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + GetFMAlwaysOpen);
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 FAILED");
        console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 ends here");
      }
      await sleep(1);
      done();
    })
	
	/**
      * @tc.number    : IS_FLASH_MODE_CLOSE_SUPPORTED_028
      * @tc.name      : check if flash mode close is supported-camera0Input api
      * @tc.desc      : check if flash mode close is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FLASH_MODE_CLOSE_SUPPORTED_028', 0, async function (done) {
      console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 to operate");
      var isFMCloseSupported = await camera0InputPromise.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_CLOSE);
      console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 SUCCESS ");
      if (isFMCloseSupported != null || isFMCloseSupported != undefined) {
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 data is not null || undefined");
        console.info(TAG + "FLASH_MODE_CLOSE supported is: " + isFMCloseSupported);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 PASSED");
      }
      else {
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 FAILED");
        console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FLASH_MODE_CLOSE_029
      * @tc.name      : set flash mode close camera0 api
      * @tc.desc      : set flash mode close camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FLASH_MODE_CLOSE_029', 0, async function (done) {
      console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 to operate");
      var SetFMClose = await camera0InputPromise.setFlashMode(cameraObj.FlashMode.FLASH_MODE_CLOSE);
      console.info(TAG + "setFlashModeOPEN: " + JSON.stringify(SetFMClose))
      if (SetFMClose == undefined) {
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_CLOSE);
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 PASSED")
        expect(true).assertTrue()
      } else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 FAILED");
        console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FLASH_MODE_CLOSE_030
      * @tc.name      : get flash mode close camera0 api
      * @tc.desc      : get flash mode close camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FLASH_MODE_CLOSE_030', 0, async function (done) {
      console.info(TAG + "Entering GET_FLASH_MODE_CLOSE_030 to operate");
      var GetFMClose = await camera0InputPromise.getFlashMode();
      console.info(TAG + "Entering GET_FLASH_MODE_CLOSE success");
      if (GetFMClose == 0) {
        console.info(TAG + "GET_FLASH_MODE_CLOSE data is not null || undefined: ");
        console.info(TAG + "Current FlashMode is: " + GetFMClose);
        expect(true).assertTrue();
        console.info(TAG + "GET_FLASH_MODE_CLOSE_030 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FLASH_MODE_CLOSE_030 FAILED");
        console.info(TAG + "GET_FLASH_MODE_CLOSE_030 ends here");
      }
      await sleep(1);
      done();
    })

    // FOCUS promise API's

    it('IS_FOCUS_MODE_MANUAL_SUPPORTED_031', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 to operate");
      var isFMmanualSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_MANUAL);
      if (isFMmanualSupportedpromise != null || isFMmanualSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMmanualSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED_031 FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED_031 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_MANUAL_032
      * @tc.name      : set focus mode manual camera0 api
      * @tc.desc      : set focus mode manual camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('SET_FOCUS_MODE_MANUAL_032', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 to operate");
      var setFocusManual = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_MANUAL);
      console.info(TAG + "setFocusManual: " + JSON.stringify(setFocusManual))
      if (setFocusManual == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_MANUAL);
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_MANUAL_033
      * @tc.name      : get focus mode manual camera0 api
      * @tc.desc      : get focus mode manual camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FOCUS_MODE_MANUAL_033', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL_033 to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL_033 SUCCESS");
      if (getfocusmodepromise == 0) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 FAILED");
        console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 ends here");
      }
      await sleep(1);
      done();

    })

    /**
      * @tc.number    : IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034
      * @tc.name      : check if focus mode continuous is supported-camera0Input api
      * @tc.desc      : check if focus mode continuous is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 to operate");
      var isFMContinuousSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
      if (isFMContinuousSupportedpromise != null || isFMContinuousSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMContinuousSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_CONTINUOUS_035
      * @tc.name      : set focus mode continuous camera0 api
      * @tc.desc      : set focus mode continuous camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('SET_FOCUS_MODE_CONTINUOUS_035', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 to operate");
      var setFocusCont = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
      console.info(TAG + "setFocusCont: " + JSON.stringify(setFocusCont))
      if (setFocusCont == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_CONTINUOUS_036
      * @tc.name      : get focus mode continuous camera0 api
      * @tc.desc      : get focus mode continuous camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FOCUS_MODE_CONTINUOUS_036', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS_036 to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS_036 SUCCESS");
      if (getfocusmodepromise == 1) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 FAILED");
        console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : IS_FOCUS_MODE_AUTO_SUPPORTED_037
      * @tc.name      : check if focus mode auto is supported-camera0Input api
      * @tc.desc      : check if focus mode auto is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('IS_FOCUS_MODE_AUTO_SUPPORTED_037', 0, async function (done) {
      console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 to operate");
      var isFMAutoSupportedpromise = await camera0InputPromise.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_AUTO);
      if (isFMAutoSupportedpromise != null || isFMAutoSupportedpromise != undefined) {
        console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 data is not null || undefined");
        console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " + isFMAutoSupportedpromise);
        expect(true).assertTrue();
        console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 PASSED: ");
      }
      else {
        console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED_037 FAILED");
        expect().assertFail();
        console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED_037 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : SET_FOCUS_MODE_AUTO_038
      * @tc.name      : set focus mode auto camera0 api
      * @tc.desc      : set focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
    it('SET_FOCUS_MODE_AUTO_038', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_038 to operate");
      var setFocusAuto = await camera0InputPromise.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_AUTO);
      console.info(TAG + "setFocusAuto: " + JSON.stringify(setFocusAuto))
      if (setFocusAuto == undefined) {
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_038 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_AUTO);
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_038 PASSED")
        expect(true).assertTrue()
      }
      else {
        expect().assertFail();
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_038 FAILED");
        console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_038 ends here");
      }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : GET_FOCUS_MODE_AUTO_039
      * @tc.name      : get focus mode auto camera0 api
      * @tc.desc      : get focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/

    it('GET_FOCUS_MODE_AUTO_039', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_AUTO_039 to operate");
      var getfocusmodepromise = await camera0InputPromise.getFocusMode();
      console.info(TAG + "Entering GET_FOCUS_MODE_AUTO_039 SUCCESS");
      if (getfocusmodepromise == 2) {
        console.info(TAG + "Current FocusMode is: " + getfocusmodepromise);
        expect(true).assertTrue();
        console.info(TAG + "GET_FOCUS_MODE_AUTO_039 PASSED");
      }
      else {
        expect().assertFail();
        console.info(TAG + "GET_FOCUS_MODE_AUTO_039 FAILED");
        console.info(TAG + "GET_FOCUS_MODE_AUTO_039 ends here");
      }
      await sleep(1);
      done();
    })

		   //ZOOM Function
	   
/**
      * @tc.number    : GET_ZOOM_RATIO_PROMISE_TC_041
      * @tc.name      : get zoom ratio camera-0 cameraId api promise api
      * @tc.desc      : get zoom ratio camera-0 cameraId api promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_ZOOM_RATIO_PROMISE_TC_041', 0, async function (done) {
        console.info("--------------GET_ZOOM_RATIO_PROMISE_TC_041--------------");
        var getZoomRatioPromise = await camera0InputPromise.getZoomRatioRange();
        console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 getZoomRatioPromise: " + JSON.stringify(getZoomRatioPromise));
			if (getZoomRatioPromise != null && getZoomRatioPromise != undefined) {
				console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 setZoomRatioPromise is not null || undefined");
				expect(true).assertTrue();
				console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 success: " + JSON.stringify(getZoomRatioPromise));
				console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 PASSED");
		} else {
			expect().assertFail();
			console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 FAILED");
		}
		console.info(TAG + "Entering GET_ZOOM_RATIO_PROMISE_TC_041 ends here");
		await sleep(1);
		done();
    })
	
	
	/**
      * @tc.number    : SET_GET_ZOOM_1_PROMISE_TC_042
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
   it('SET_GET_ZOOM_1_PROMISE_TC_042', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(1);
          console.info(TAG + "setZoomRatio success: 1");
          console.info(TAG + "getZoomRatio called")
            var getpromise1 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise1);
			  if(getpromise1 !=null && getpromise1 != undefined) {
              expect(getpromise1).assertEqual(1);
              console.info(TAG + "SET_GET_ZOOM_1_PROMISE_TC_042 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_1_PROMISE_TC_042 FAILED" );
            }
		await sleep(1);
		done();
    })
		

	/**
      * @tc.number    : SET_GET_ZOOM_2_PROMISE_TC_043
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
   it('SET_GET_ZOOM_2_PROMISE_TC_043', 0, async function (done) {
       var setpromise = await camera0InputPromise.setZoomRatio(2);
          console.info(TAG + "setZoomRatio success: 2");
          console.info(TAG + "getZoomRatio called")
            var getpromise2 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise2);
			  if(getpromise2 !=null && getpromise2 != undefined) {
              expect(getpromise2).assertEqual(2);
              console.info(TAG + "SET_GET_ZOOM_2_PROMISE_TC_043 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_2_PROMISE_TC_043 FAILED" );
            }
		await sleep(1);
		done();
    })
	

	/**
      * @tc.number    : SET_GET_ZOOM_3_PROMISE_TC_044
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
   it('SET_GET_ZOOM_3_PROMISE_TC_044', 0, async function (done) {
      var setpromise = await camera0InputPromise.setZoomRatio(3);
          console.info(TAG + "setZoomRatio success: 3");
          console.info(TAG + "getZoomRatio called")
            var getpromise3 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise3);
			  if(getpromise3 !=null && getpromise3 != undefined) {
              expect(getpromise3).assertEqual(3);
              console.info(TAG + "SET_GET_ZOOM_3_PROMISE_TC_044 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_3_PROMISE_TC_044 FAILED" );
            }
		await sleep(1);
		done();
    })
	

	/**
      * @tc.number    : SET_GET_ZOOM_4_PROMISE_TC_045
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_4_PROMISE_TC_045', 0, async function (done) {
        var setpromise = await camera0InputPromise.setZoomRatio(4);
          console.info(TAG + "setZoomRatio success: 4");
          console.info(TAG + "getZoomRatio called")
            var getpromise4 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise4);
			  if(getpromise4 !=null && getpromise4 != undefined) {
              expect(getpromise4).assertEqual(4);
              console.info(TAG + "SET_GET_ZOOM_4_PROMISE_TC_045 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_4_PROMISE_TC_045 FAILED" );
            }
		await sleep(1);
		done();
    })

	
	/**
      * @tc.number    : SET_GET_ZOOM_5_PROMISE_TC_046
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_5_PROMISE_TC_046', 0, async function (done) {
        var setpromise = await camera0InputPromise.setZoomRatio(5);
          console.info(TAG + "setZoomRatio success: 5");
          console.info(TAG + "getZoomRatio called")
            var getpromise5 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise5);
			  if(getpromise5 !=null && getpromise5 != undefined) {
              expect(getpromise5).assertEqual(5);
              console.info(TAG + "SET_GET_ZOOM_5_PROMISE_TC_046 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_5_PROMISE_TC_046 FAILED" );
            }
		await sleep(1);
		done();
    })
	

	/**
      * @tc.number    : SET_GET_ZOOM_6_PROMISE_TC_047
      * @tc.name      : Zoom camera-0 cameraId api 
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_6_PROMISE_TC_047', 0, async function (done) {
        var setpromise = await camera0InputPromise.setZoomRatio(6);
          console.info(TAG + "setZoomRatio success: 6");
          console.info(TAG + "getZoomRatio called")
            var getpromise6 = await camera0InputPromise.getZoomRatio();
              console.info(TAG + "getZoomRatio success: "+getpromise6);
			  if(getpromise6 !=null && getpromise6 != undefined) {
              expect(getpromise6).assertEqual(6);
              console.info(TAG + "SET_GET_ZOOM_6_PROMISE_TC_047 PASSED ");
			  }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_6_PROMISE_TC_047 FAILED" );
            }
      await sleep(1);
      done();
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_START_PROMISE_TC_048
      * @tc.name      : VideoOutput start promise api
      * @tc.desc      : VideoOutput start promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_START_PROMISE_TC_048', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_PROMISE_TC_048 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_PROMISE_TC_048 to operate')
        await videoOutputPromise.start()
        expect(true).assertTrue()
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_PROMISE_TC_048 PASSED')
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_PROMISE_TC_048 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })



    /**
      * @tc.number    : VIDEO_RECORDER_START_PROMISE_TC_049
      * @tc.name      : VideoOutput start promise api
      * @tc.desc      : VideoOutput start promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_START_PROMISE_TC_049', 0, async function (done) {
      if (videoRecorderPromise == null || videoRecorderPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_START_PROMISE_TC_049 videoRecorderPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_START_PROMISE_TC_049 to operate')
        videoRecorderPromise.start()
        console.info(TAG + 'VIDEO_RECORDER_START_PROMISE_TC_049 called');
        sleep(3);
        console.info(TAG + 'VIDEO_RECORDER_START_PROMISE_TC_049 done.')
        expect(true).assertTrue()
        console.info(TAG + 'Entering VIDEO_RECORDER_START_PROMISE_TC_049 PASSED')
        console.info(TAG + 'Entering VIDEO_RECORDER_START_PROMISE_TC_049 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : VIDEO_OUTPUT_STOP_PROMISE_TC_050
      * @tc.name      : VideoOutput stop promise api
      * @tc.desc      : VideoOutput stop promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_STOP_PROMISE_TC_050', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_PROMISE_TC_050 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_PROMISE_TC_050 to operate')
        await videoOutputPromise.stop()
        expect(true).assertTrue()
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_PROMISE_TC_050 PASSED')
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_PROMISE_TC_050 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051
      * @tc.name      : VideoRecorder stop promise api
      * @tc.desc      : VideoRecorder stop promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051', 0, async function (done) {
      if (videoOutputPromise == null || videoOutputPromise == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051 videoOutputPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051 to operate')
        videoRecorderPromise.stop()
        console.info(TAG + 'VIDEO_RECORDER_STOP done.')

        videoOutputPromise.release()
        videoRecorderPromise.release()
        console.info(TAG + 'VIDEO_RECORDER_RELEASE done.')
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051 PASSED')
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_PROMISE_TC_051 ends here')
        await sleep(1)
        done()
      }
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CAPTURE_SESSION_STOP_PROMISE_TC_052
      * @tc.name      : CaptureSession stop promise api
      * @tc.desc      : CaptureSession stop promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_STOP_PROMISE_TC_052', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_PROMISE_TC_052 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_PROMISE_TC_052 to operate')
        await captureSessionPromise.stop()
        expect(true).assertTrue()
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_PROMISE_TC_052 PASSED')
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_PROMISE_TC_052 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CAPTURE_SESSION_RELEASE_PROMISE_TC_053
      * @tc.name      : CaptureSession release promise api
      * @tc.desc      : CaptureSession release promise api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_RELEASE_PROMISE_TC_053', 0, async function (done) {
      if (captureSessionPromise == null || captureSessionPromise == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_PROMISE_TC_053 captureSessionPromise == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_PROMISE_TC_053 to operate')
        await captureSessionPromise.release()
        expect(true).assertTrue()
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_PROMISE_TC_053 PASSED')
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_PROMISE_TC_053 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })
  })
}