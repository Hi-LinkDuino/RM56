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

import cameraJSUnitEnum from './CameraJSUnitEnum.test.ets'
import cameraJSUnitCameraFormat from './CameraJSUnitCameraFormat.test.ets'
import cameraJSUnitPhotoAsync from './CameraJSUnitPhotoAsync.test.ets'
import cameraJSUnitPhotoPromise from './CameraJSUnitPhotoPromise.test.ets'
import cameraJSUnitVideoAsync from './CameraJSUnitVideoAsync.test.ets'
import cameraJSUnitVideoPromise from './CameraJSUnitVideoPromise.test.ets'

let TAG = 'CameraModuleTest: '

export default function cameraKit(surfaceId: any) {
  console.info(TAG + 'Entering cameraKit')
  console.info(TAG + 'surfaceId: ' + surfaceId)

  cameraJSUnitEnum(surfaceId)
  cameraJSUnitCameraFormat(surfaceId)
  cameraJSUnitPhotoAsync(surfaceId)
  cameraJSUnitPhotoPromise(surfaceId)
  cameraJSUnitVideoAsync(surfaceId)
  cameraJSUnitVideoPromise(surfaceId)
}