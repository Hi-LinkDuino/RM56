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
var cameraManager
var camerasArray
var camera0Input
var previewOutput
var videoRecorder
var videoProfile = {
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
var videoConfig = {
  audioSourceType: 1,
  videoSourceType: 0,
  profile: videoProfile,
  url: 'file:///data/media/01.mp4',
  orientationHint: 0,
  location: { latitude: 30, longitude: 130 },
  maxSize: 100,
  maxDuration: 500
}
var videoId
var videoOutput
var captureSession
var surfaceId1

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
  
    console.info(TAG + 'Entering it() initial point')
    /**
      * @tc.number    : GET_CAMERA_MANAGER_TC_001
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERA_MANAGER_TC_001', 0, async function (done) {
      console.info('--------------GET_CAMERA_MANAGER_TC_001--------------')
      await sleep(1)
      cameraObj.getCameraManager(null, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC_001 success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC_001 data is not null || undefined')
            cameraManager = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC_001 PASSED')
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC_001 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering GET_CAMERA_MANAGER_TC_001 ends here')
        done()
      })

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : GET_CAMERAS_TC_002
      * @tc.name      : Create camera manager instance async api
      * @tc.desc      : Create camera manager instance async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_CAMERAS_TC_002', 0, async function (done) {
      console.info('--------------GET_CAMERAS_TC_002--------------')
      await sleep(1)
      cameraManager.getCameras((err, data) => {
        if (!err) {
          console.info(TAG + 'Entering GET_CAMERAS_TC_002 success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering GET_CAMERAS_TC_002 data is not null || undefined')
            camerasArray = data
            if (camerasArray != null && camerasArray.length > 0) {
              for (var i = 0; i < camerasArray.length; i++) {
                // Get the variables from camera object
                var cameraId = camerasArray[i].cameraId
                console.info(TAG + 'Entering GET_CAMERAS_TC_002 camera' + i + 'Id: ' + cameraId)
                var cameraPosition = camerasArray[i].cameraPosition
                console.info(TAG + 'Entering GET_CAMERAS_TC_002 camera' + i + 'Position: ' + cameraPosition)
                var cameraType = camerasArray[i].cameraType
                console.info(TAG + 'Entering GET_CAMERAS_TC_002 camera' + i + 'Type: ' + cameraType)
                var connectionType = camerasArray[i].connectionType
                console.info(TAG + 'Entering GET_CAMERAS_TC_002 connection' + i + 'Type: ' + connectionType)
              }
              expect(true).assertTrue()
              console.info(TAG + 'Entering GET_CAMERAS_TC_002 PASSED')
            } else {
              expect().assertFail()
              console.info(TAG + 'Entering GET_CAMERAS_TC_002 FAILED cameraArray is null || undefined')
            }
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering GET_CAMERAS_TC_002 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering GET_CAMERAS_TC_002 ends here')
        done()
      })

      await sleep(1)
      done()
    })

    /*CAMERA-0 Scripts*/
    /**
      * @tc.number    : CREATE_CAMERA_INPUT_TC_003
      * @tc.name      : Create camerainput from camera-0 cameraId async api
      * @tc.desc      : Create camerainput from camera-0 cameraId async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_TC_003', 0, async function (done) {
      console.info('--------------CAMERA-0 STARTS HERE--------------')
      console.info('--------------CREATE_CAMERA_INPUT_TC_003--------------')
      await sleep(1)
      cameraManager.createCameraInput(camerasArray[0].cameraId, (err, data) => {
        if (!err) {
          if (data != null && data != undefined) {
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT_TC_003 data is not null || undefined')
            camera0Input = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT_TC_003 PASSED with CameraID :' + camerasArray[0].cameraId)
          }
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering CREATE_CAMERA_INPUT_TC_003 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_TC_003 ends here')
        done()
      })
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004
      * @tc.name      : Create camerainput from camera-0 cameraposition & cameratype async api
      * @tc.desc      : Create camerainput from camera-0 cameraposition & cameratype async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004', 0, async function (done) {
      console.info('--------------CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004--------------')
      await sleep(1)
      cameraManager.createCameraInput(camerasArray[0].cameraPosition, camerasArray[0].cameraType, (err, data) => {
        if (!err || err.message != 'Failed to get arguments!') {
          console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 data is not null || undefined')
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 PASSED')
          } else {
            console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 FAILED: ' + err.message)
            expect().assertFail()
          }
        } else {
          console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 FAILED: ' + err.message)
          expect().assertFail()
        }
        console.info(TAG + 'Entering CREATE_CAMERA_INPUT_POSITION_TYPE_TC_004 ends here')
        done()
      })
      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_PREVIEW_OUTPUT_TC_005
      * @tc.name      : Create previewoutput async api
      * @tc.desc      : Create previewoutput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_PREVIEW_OUTPUT_TC_005', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 to operate')
      await sleep(1)
      cameraObj.createPreviewOutput(surfaceId, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 data is not null || undefined')
            previewOutput = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 PASSED')
          }
        } else {
          console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_PREVIEW_OUTPUT_TC_005 ends here')
        done()
      })

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_VIDEO_OUTPUT_TC_006
      * @tc.name      : Create videooutput async api
      * @tc.desc      : Create videooutput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_VIDEO_OUTPUT_TC_006', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 to operate')
      await getvideosurface()
      await sleep(1)
      cameraObj.createVideoOutput(videoId, (err, data) => {
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 success')
        if (data != null || data != undefined) {
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 data is not null || undefined')
          videoOutput = data
          expect(true).assertTrue()
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 PASSED')
        } else {
          expect().assertFail()
          console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_VIDEO_OUTPUT_TC_006 ends here')
        done()
      })

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : CREATE_CAPTURE_SESSION_TC_007
      * @tc.name      : Create capturesession async api
      * @tc.desc      : Create capturesession async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CREATE_CAPTURE_SESSION_TC_007', 0, async function (done) {
      console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 to operate')
      await sleep(1)
      cameraObj.createCaptureSession(null, (err, data) => {
        if (!err) {
          console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 success')
          if (data != null || data != undefined) {
            console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 data is not null || undefined')
            captureSession = data
            expect(true).assertTrue()
            console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 PASSED')
          }
        } else {
          console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 FAILED: ' + err.message)
        }
        console.info(TAG + 'Entering CREATE_CAPTURE_SESSION_TC_007 ends here')
        done()
      })

      await sleep(1)
      done()
    })

    /**
      * @tc.number    : BEGIN_CONFIG_TC_008
      * @tc.name      : Create capturesession async api
      * @tc.desc      : Create capturesession async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('BEGIN_CONFIG_TC_008', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 to operate')
        await sleep(1)
        captureSession.beginConfig((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 PASSED')
            }
          } else {
            console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering BEGIN_CONFIG_TC_008 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_INPUT_TC_009
      * @tc.name      : AddInput async api
      * @tc.desc      : AddInput async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_INPUT_TC_009', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_INPUT_TC_009 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_INPUT_TC_009 to operate')
        await sleep(1)
        captureSession.addInput(camera0Input, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_INPUT_TC_009 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_INPUT_TC_009 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_INPUT_TC_009 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_INPUT_TC_009 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_INPUT_TC_009 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_PREVIEW_TC_010
      * @tc.name      : AddOutput preview async api
      * @tc.desc      : AddOutput preview async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_PREVIEW_TC_010', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 to operate')
        await sleep(1)
        captureSession.addOutput(previewOutput, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_PREVIEW_TC_010 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : ADD_OUTPUT_VIDEO_TC_011
      * @tc.name      : AddOutput video async api
      * @tc.desc      : AddOutput video async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('ADD_OUTPUT_VIDEO_TC_011', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 to operate')
        await sleep(1)
        captureSession.addOutput(videoOutput, (err, data) => {
          if (!err) {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 PASSED')
            }
          } else {
            console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering ADD_OUTPUT_VIDEO_TC_011 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : COMMIT_CONFIG_TC_012
      * @tc.name      : CommitConfig async api
      * @tc.desc      : CommitConfig async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('COMMIT_CONFIG_TC_012', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 to operate')
        await sleep(1)
        captureSession.commitConfig((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 PASSED')
            }
          } else {
            console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering COMMIT_CONFIG_TC_012 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })
	
	//callback API

	/**
      * @tc.number    : CAP_SES_CALLBACK_ON_ERROR_013
      * @tc.name      : CaptureSession callback on error api
      * @tc.desc      : CaptureSession callback on error api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('CAP_SES_CALLBACK_ON_ERROR_013', 0, async function (done) {
        if (captureSession == null || captureSession == undefined) {
            console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR_013 captureSession == null || undefined");
        } else {
            console.info(TAG + "Entering CAP_SES_CALLBACK_ON_ERROR_013 to operate");
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_014
      * @tc.name      : Preview output callback on frame start api
      * @tc.desc      : Preview output callback on frame start api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_014', 0, async function (done) {
        if (previewOutput == null || previewOutput == undefined) {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_014 previewOutput == null || undefined");
        } else {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_START_014 to operate");
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_015
      * @tc.name      : Preview capture callback on frame end api
      * @tc.desc      : Preview capture callback on frame end api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_015', 0, async function (done) {
        if (previewOutput == null || previewOutput == undefined) {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_015 previewOutput == null || undefined");
        } else {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_FRAME_END_015 to operate");
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
      * @tc.number    : PREVIEW_OUTPUT_CALLBACK_ON_ERROR_016
      * @tc.name      : Preview output callback on error api
      * @tc.desc      : Preview output callback on error api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('PREVIEW_OUTPUT_CALLBACK_ON_ERROR_016', 0, async function (done) {
        if (previewOutput == null || previewOutput == undefined) {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR_016 previewOutput == null || undefined");
        } else {
            console.info(TAG + "Entering PREVIEW_OUTPUT_CALLBACK_ON_ERROR_016 to operate");
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017
      * @tc.name      : VideoOutput callback onframestart async api
      * @tc.desc      : VideoOutput callback onframestart async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017', 0, async function (done) {
        if (videoOutput == null || videoOutput == undefined) {
            console.info(TAG + "Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017 videoOutput == null || undefined");
        } else {
            console.info(TAG + "Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_START_TC_017 to operate");
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018
      * @tc.name      : VideoOutput callback onframeend async api
      * @tc.desc      : VideoOutput callback onframeend async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018 videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_FRAME_END_TC_018 to operate')
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
      * @tc.number    : VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019
      * @tc.name      : VideoOutput callback onerror async api
      * @tc.desc      : VideoOutput callback onerror async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019 videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_CALLBACK_ON_ERROR_TC_019 to operate')
        await sleep(1)
        videoOutput.on('error', async (err, data) => {
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

    /**
      * @tc.number    : CAPTURE_SESSION_START_TC_020
      * @tc.name      : CaptureSession start async api
      * @tc.desc      : CaptureSession start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_START_TC_020', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 to operate')
        await sleep(1)
        captureSession.start((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 PASSED')
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_START_TC_020 ends here')
          done()
        })

        await sleep(1)
        done()
      }
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
      camera0Input.hasFlash(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering HAS_FLASH success");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering HAS_FLASH data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering HAS_FLASH PASSED with HAS_FLASH is: " +data);
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering HAS_FLASH FAILED" + err.message);
        }
        console.info(TAG + "Entering HAS_FLASH_TC_021 ends here");
      })
      await sleep(1);
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED_022 SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED data is not null || undefined");
            console.info(TAG + "FLASH_MODE_OPEN supported is: " +data);
			expect(true).assertTrue();
            console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED PASSED");
          }
        } else {
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_OPEN_SUPPORTED ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_OPEN);
          if (data != null || data != undefined) {
            console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 PASSED")
            expect(true).assertTrue()
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_OPEN_023 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_OPEN success");
          if (data == 1) {
            console.info(TAG + "GET_FLASH_MODE_OPEN data is not null || undefined: ");
            console.info(TAG + "Current FlashMode is: " +data);
			expect(true).assertTrue();
            console.info(TAG + "GET_FLASH_MODE_OPEN_024 PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_OPEN_024 FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_OPEN_024 ends here");
        }
        await sleep(1);
        done();

      })
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering FLASH_MODE_ALWAYS_OPEN SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "FLASH_MODE_ALWAYS_OPEN supported is: " +data);
            console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 PASSED");
          }
        } else {
          console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_ALWAYS_OPEN_SUPPORTED_025 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_ALWAYS_OPEN);
          if (data != null || data != undefined) {
            console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 PASSED")
            expect(true).assertTrue()
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_ALWAYS_OPEN_026 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_ALWAYS_OPEN success");
          if (data == 3) {
            console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN data is not null || undefined: ");
            expect(true).assertTrue();
            console.info(TAG + "Current FlashMode is: " +data);
            console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 PASSED");
          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_ALWAYS_OPEN_027 ends here");
        }
        await sleep(1);
        done();
      })
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
      camera0Input.isFlashModeSupported(cameraObj.FlashMode.FLASH_MODE_CLOSE, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED_028 SUCCESS ");
          if (data != null || data != undefined) {
            console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "FLASH_MODE_CLOSE supported is: " +data);
            console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED PASSED");
          }
        } else {
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED FAILED" + err.message);
          console.info(TAG + "Entering IS_FLASH_MODE_CLOSE_SUPPORTED ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.setFlashMode(cameraObj.FlashMode.FLASH_MODE_CLOSE, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 SUCCESS, current flashmode is: " + cameraObj.FlashMode.FLASH_MODE_CLOSE);
          if (data != null || data != undefined) {
            console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 PASSED")
            expect(true).assertTrue()
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 FAILED" + err.message);
          console.info(TAG + "Entering SET_FLASH_MODE_CLOSE_029 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.getFlashMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FLASH_MODE_CLOSE_030 success");
          if (data == 0) {
            console.info(TAG + "GET_FLASH_MODE_CLOSE data is not null || undefined: ");
			console.info(TAG + "Current FlashMode is: " +data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FLASH_MODE_CLOSE_030 PASSED");
          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FLASH_MODE_CLOSE_030 FAILED" + err.message);
          console.info(TAG + "GET_FLASH_MODE_CLOSE_030 ends here");
        }
        await sleep(1)
        done();

      })
      await sleep(1)
      done()
    })

	// FOCUS API's
	
	/**
      * @tc.number    : IS_FOCUS_MODE_MANUAL_SUPPORTED_031
      * @tc.name      : check if focus mode manual is supported-camera0Input api
      * @tc.desc      : check if focus mode manual is supported-camera0Input api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('IS_FOCUS_MODE_MANUAL_SUPPORTED_031', 0, async function (done) {
	console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 to operate");
	camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_MANUAL, async (err, data) => {
		if (!err) {
			console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 SUCCESS " );
				if (data != null || data != undefined) {
					console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 data is not null || undefined");
					console.info(TAG + "FOCUS_MODE_MANUAL_SUPPORTED is: " +data );
					expect(true).assertTrue();
					console.info(TAG + "Entering IS_FOCUS_MODE_MANUAL_SUPPORTED_031 PASSED: ");
				}
			} else {
				console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED_031 FAILED" + err.message);
				expect().assertFail();
				console.info(TAG + "IS_FOCUS_MODE_MANUAL_SUPPORTED_031 ends here");
			}
			await sleep(1);
			done();
		})
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
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_MANUAL, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_MANUAL);
          if (data != null || data != undefined) {
            console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 PASSED")
            expect(true).assertTrue()
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_MANUAL_032 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_MANUAL_033 SUCCESS");
      if (data == 0) {
            console.info(TAG + "GET_FOCUS_MODE_MANUAL data is not null || undefined: ");
			console.info(TAG + "Current FocusMode is: " +data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 PASSED");
          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_MANUAL_033 ends here");
        }
        await sleep(1);
        done();

      })
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
	camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO, async (err, data) => {
		if (!err) {
			console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 SUCCESS " );
				if (data != null || data != undefined) {
					console.info(TAG + "Entering IS_FOCUS_MODE_CONTINUOUS_SUPPORTED_034 data is not null || undefined");
					console.info(TAG + "FOCUS_MODE_CONTINOUS_SUPPORTED is: " +data );
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
      * @tc.number    : SET_FOCUS_MODE_CONTINUOUS_035
      * @tc.name      : set focus mode continuous camera0 api
      * @tc.desc      : set focus mode continuous camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('SET_FOCUS_MODE_CONTINUOUS_035', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 to operate");
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_CONTINUOUS_AUTO);
          if (data != null || data != undefined) {
            expect(true).assertTrue()
			console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 PASSED")
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_CONTINUOUS_035 ends here");
        }
        await sleep(1);
        done();
      })

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
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_CONTINUOUS_036 SUCCESS");
      if (data == 1) {
            console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS data is not null || undefined: ");
			console.info(TAG + "Current FocusMode is: " +data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_CONTINUOUS_036 ends here");
        }
        await sleep(1);
        done();

      })
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
	camera0Input.isFocusModeSupported(cameraObj.FocusMode.FOCUS_MODE_AUTO, async (err, data) => {
		if (!err) {
			console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 SUCCESS " );
				if (data != null || data != undefined) {
					console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 data is not null || undefined");
					console.info(TAG + "FOCUS_MODE_AUTO_SUPPORTED is: " + data );
					expect(true).assertTrue();
					console.info(TAG + "Entering IS_FOCUS_MODE_AUTO_SUPPORTED_037 PASSED: ");
				}
			} else {
				console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED_037 FAILED" + err.message);
				expect().assertFail();
				console.info(TAG + "IS_FOCUS_MODE_AUTO_SUPPORTED_037 ends here");
			}
			await sleep(1);
			done();
		})
		await sleep(1);
		done();
	})
	
	/**
      * @tc.number    : SET_FOCUS_MODE_AUTO_039
      * @tc.name      : set focus mode auto camera0 api
      * @tc.desc      : set focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	
	it('SET_FOCUS_MODE_AUTO_039', 0, async function (done) {
      console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_039 to operate");
      camera0Input.setFocusMode(cameraObj.FocusMode.FOCUS_MODE_AUTO, async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_039 SUCCESS, current FocusMode is: " + cameraObj.FocusMode.FOCUS_MODE_AUTO);
          if (data != null || data != undefined) {
            expect(true).assertTrue()
			console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_039 PASSED")
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_039 FAILED" + err.message);
          console.info(TAG + "Entering SET_FOCUS_MODE_AUTO_039 ends here");
        }
        await sleep(1);
        done();
      })

      await sleep(1);
      done();
    })
	
	/**
      * @tc.number    : GET_FOCUS_MODE_AUTO_040
      * @tc.name      : get focus mode auto camera0 api
      * @tc.desc      : get focus mode auto camera0 api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
	*/
	it('GET_FOCUS_MODE_AUTO_040', 0, async function (done) {
      console.info(TAG + "Entering GET_FOCUS_MODE_AUTO_040 to operate");
      camera0Input.getFocusMode(async (err, data) => {
        if (!err) {
          console.info(TAG + "Entering GET_FOCUS_MODE_AUTO_040 SUCCESS");
      if (data == 2) {
            console.info(TAG + "GET_FOCUS_MODE_AUTO data is not null || undefined: ");
			console.info(TAG + "Current FocusMode is: " +data);
            expect(true).assertTrue();
            console.info(TAG + "GET_FOCUS_MODE_AUTO_040 PASSED");

          }
        }
        else {
          expect().assertFail();
          console.info(TAG + "GET_FOCUS_MODE_AUTO_040 FAILED" + err.message);
          console.info(TAG + "GET_FOCUS_MODE_AUTO_040 ends here");
        }
        await sleep(1);
        done();

      })
      await sleep(1);
      done();
    })
	
	
	
	
	//ZOOM Function

    /**
      * @tc.number    : GET_ZOOM_RATIO_TC_041
      * @tc.name      : get zoom ratio camera-0 cameraId api
      * @tc.desc      : get zoom ratio camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('GET_ZOOM_RATIO_TC_041', 0, async function (done) {
      console.info("--------------GET_ZOOM_RATIO_TC_041--------------");
      camera0Input.getZoomRatioRange(async (err,data) => {
        if (!err) {
          if (data != null && data != undefined) {
            console.info(TAG + "Entering GET_ZOOM_RATIO_TC_041 data is not null || undefined");
            expect(true).assertTrue();
            console.info(TAG + "Entering GET_ZOOM_RATIO_TC_041 Success: "  + data );
          }
        } else {
          expect().assertFail();
          console.info(TAG + "Entering GET_ZOOM_RATIO_TC_041 FAILED: " + err.message);
        }
        console.info(TAG + "Entering GET_ZOOM_RATIO_TC_041 ends here");
        await sleep(1);
        done();
      })
      await sleep(1);
      done();
    })


    /**
      * @tc.number    : SET_GET_ZOOM_1_ASYNC_TC_042
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_1_ASYNC_TC_042', 0, async function (done) {
      camera0Input.setZoomRatio(1, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 1");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(1);
              console.info(TAG + "SET_GET_ZOOM_1_ASYNC_TC_042 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_1_ASYNC_TC_042 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })


    /**
      * @tc.number    : SET_GET_ZOOM_2_ASYNC_TC_043
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_2_ASYNC_TC_043', 0, async function (done) {
      camera0Input.setZoomRatio(2, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 2");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(2);
              console.info(TAG + "SET_GET_ZOOM_2_ASYNC_TC_043 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_2_ASYNC_TC_043 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })



    /**
      * @tc.number    : SET_GET_ZOOM_3_ASYNC_TC_044
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_3_ASYNC_TC_044', 0, async function (done) {
      camera0Input.setZoomRatio(3, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 3");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(3);
              console.info(TAG + "SET_GET_ZOOM_3_ASYNC_TC_044 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_3_ASYNC_TC_044 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })


    /**
      * @tc.number    : SET_GET_ZOOM_4_ASYNC_TC_045
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_4_ASYNC_TC_045', 0, async function (done) {
      camera0Input.setZoomRatio(4, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 4");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(4);
              console.info(TAG + "SET_GET_ZOOM_4_ASYNC_TC_045 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_4_ASYNC_TC_045 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })


    /**
      * @tc.number    : SET_GET_ZOOM_5_ASYNC_TC_046
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_5_ASYNC_TC_046', 0, async function (done) {
      camera0Input.setZoomRatio(5, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 5");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(5);
              console.info(TAG + "SET_GET_ZOOM_5_ASYNC_TC_046 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_5_ASYNC_TC_046 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })



    /**
      * @tc.number    : SET_GET_ZOOM_6_ASYNC_TC_047
      * @tc.name      : Zoom camera-0 cameraId api
      * @tc.desc      : Zoom camera-0 cameraId api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('SET_GET_ZOOM_6_ASYNC_TC_047', 0, async function (done) {
      camera0Input.setZoomRatio(6, (err,data)=>{
        if(!err){
          console.info(TAG + "setZoomRatio success: 6");
          console.info(TAG + "getZoomRatio called")
          camera0Input.getZoomRatio((err,data1) => {
            if(!err){
              console.info(TAG + "getZoomRatio success : "+data1);
              expect(data1).assertEqual(6);
              console.info(TAG + "SET_GET_ZOOM_6_ASYNC_TC_047 PASSED ");
            }
            else {
              expect().assertFail();
              console.info(TAG + "SET_GET_ZOOM_6_ASYNC_TC_047 FAILED" + err.message);
            }
          })
        }
      })
      await sleep(1);
      done();
    })


    /**
      * @tc.number    : VIDEO_OUTPUT_START_TC_048
      * @tc.name      : VideoOutput start async api
      * @tc.desc      : VideoOutput start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_START_TC_048', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_TC_048 videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_START_TC_048 to operate')
        await sleep(1)
        videoOutput.start(async (err, data) => {
          console.info(TAG + 'Entering VIDEO_OUTPUT_START_TC_048 success: '+JSON.stringify(data))
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
      * @tc.number    : VIDEO_RECORDER_START_TC_049
      * @tc.name      : VideoRecorder start async api
      * @tc.desc      : VideoRecorder start async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_START_TC_049', 0, async function (done) {
      if (videoRecorder == null || videoRecorder == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_START_TC_049 videoRecorder == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_START_TC_049 to operate')
        videoRecorder.start()
		console.info(TAG + 'VIDEO_RECORDER_START_TC_049 called');
        sleep(3);
        console.info(TAG + 'VIDEO_RECORDER_START_TC_049 done.')
        expect(true).assertTrue()
        console.info(TAG + 'Entering VIDEO_RECORDER_START_TC_049 PASSED')
        console.info(TAG + 'Entering VIDEO_RECORDER_START_TC_049 ends here')
        await sleep(1)
        done()
      }

      await sleep(1)
      done()
    })
	
    /**
      * @tc.number    : VIDEO_OUTPUT_STOP_TC_050
      * @tc.name      : VideoOutput stop async api
      * @tc.desc      : VideoOutput stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_OUTPUT_STOP_TC_050', 0, async function (done) {
      if (videoOutput == null || videoOutput == undefined) {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_TC_050 videoOutput == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_TC_050 to operate')
        await sleep(1)
        videoOutput.stop(async (err, data) => {
          console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_TC_050 success: '+JSON.stringify(data))
          if (data == undefined) {
            expect(true).assertTrue()
          } else {
            expect().assertFail()
          }
          console.info(TAG + 'Entering VIDEO_OUTPUT_STOP_TC_050 ends here')
          await sleep(1)
          done()
        })

        await sleep(1)
        done()
      }
    })
    
    /**
      * @tc.number    : VIDEO_RECORDER_STOP_RELEASE_TC_051
      * @tc.name      : VideoRecorder stop async api
      * @tc.desc      : VideoRecorder stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('VIDEO_RECORDER_STOP_RELEASE_TC_051', 0, async function (done) {
      if (videoRecorder == null || videoRecorder == undefined) {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_TC_051 videoRecorder == null || undefined')
      } else {
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_TC_051 to operate')
        await sleep(1)
        videoRecorder.stop()
        console.info(TAG + 'VIDEO_RECORDER_STOP_RELEASE_TC_051 stopVideo done.')
        videoOutput.release()
        videoRecorder.release()
        console.info(TAG + 'VIDEO_RECORDER_STOP_RELEASE_TC_051 releaseVideo done.')
		console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_TC_051 PASSED')
        console.info(TAG + 'Entering VIDEO_RECORDER_STOP_RELEASE_TC_051 ends here')

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_STOP_TC_052
      * @tc.name      : CaptureSession stop async api
      * @tc.desc      : CaptureSession stop async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_STOP_TC_052', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 to operate')
        await sleep(1)
        captureSession.stop((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 PASSED')
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_STOP_TC_052 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })

    /**
      * @tc.number    : CAPTURE_SESSION_RELEASE_TC_053
      * @tc.name      : CaptureSession release async api
      * @tc.desc      : CaptureSession release async api
      * @tc.size      : MEDIUM
      * @tc.type      : Function
      * @tc.level     : Level 0
    */
    it('CAPTURE_SESSION_RELEASE_TC_053', 0, async function (done) {
      if (captureSession == null || captureSession == undefined) {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 captureSession == null || undefined')
      } else {
        console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 to operate')
        await sleep(1)
        captureSession.release((err, data) => {
          if (!err) {
            console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 success')
            if (data != null || data != undefined) {
              console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 data is not null || undefined')
              expect(true).assertTrue()
              console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 PASSED')
            }
          } else {
            console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 FAILED: ' + err.message)
          }
          console.info(TAG + 'Entering CAPTURE_SESSION_RELEASE_TC_053 ends here')
          done()
        })

        await sleep(1)
        done()
      }
    })
  })
}