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

import cameraTestCase from '../test/CameraJSUnitEnum.test.test'
import cameraTestCase from '../test/CameraJSUnitCameraFormat.test'
import cameraTestCase from '../test/CameraJSUnitPhotoPromise.test'
import cameraTestCase from '../test/CameraJSUnitPhotoAsync.test'
import cameraTestCase from '../test/CameraJSUnitVideoPromise.test'
import cameraTestCase from '../test/CameraJSUnitVideoAsync.test'


let TAG = 'CameraModuleTest: '

export default function cameraKit(surfaceId:any) {
  console.info(TAG + 'Entering cameraKit')
  console.info(TAG + 'surfaceId: ' +surfaceId)
  cameraTestCase(surfaceId)
//  cameraService()
}