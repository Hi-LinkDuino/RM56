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

import {Core, ExpectExtend} from "deccjsunit/index"
//import {Core, ExpectExtend, InstrumentLog, ReportExtend} from "deccjsunit/index"
import cameraKit from "../test/Camera.test"
import featureAbility from "@ohos.ability.featureAbility"
//import {PreviewArea} from '../view/PreviewArea.ets'

let TAG = 'CameraModuleTest: '
var mXComponentController: XComponentController = new XComponentController()
var surfaceId: any

@Entry
@Component
struct CameraIndex {
  @State isShowSettings: boolean = false
  @State previewSize: string = '75%'

  aboutToAppear() {
    console.info('--------------aboutToAppear--------------')
//    featureAbility.getWant()
//      .then((Want) => {
//        const core = Core.getInstance()
////        const instrumentLog = new InstrumentLog({
////          'id': 'report', 'unity': 'true'
////        })
////        console.info(TAG + 'Entering instrumentLog')
//        const expectExtend = new ExpectExtend({
//          'id': 'extend'
//        })
//        console.info(TAG + 'Entering expectExtend')
//        core.addService('expect', expectExtend)
////        core.addService('report', instrumentLog)
//        console.info(TAG + 'Entering addService')
//        core.init()
//        console.info(TAG + 'Entering core.init()')
////        core.subscribeEvent('spec', instrumentLog)
////        core.subscribeEvent('suite', instrumentLog)
////        core.subscribeEvent('task', instrumentLog)
//        console.info(TAG + 'Entering subscribeEvent')
//        const configService = core.getDefaultService('config')
//        configService.setConfig(Want.parameters)
//        console.info(TAG + 'Entering configService')
//        cameraKit(surfaceId)
////        console.info(TAG + 'Entering PreviewArea')
////        PreviewArea({
////          previewSize: $previewSize
////        })
//        core.execute()
//        console.info(TAG + 'Operation successful. Data: ' + JSON.stringify(Want));
//      })
//      .catch((error) => {
//        console.error(TAG + 'Operation failed. Cause: ' + JSON.stringify(error));
//      })
  }

  /*build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('Hello World')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
    }
    .width('100%')
    .height('100%')
  }*/
  build() {
    Flex() {
      XComponent({
        id: '',
        type: 'surface',
        libraryname: '',
        source: '',
        controller: mXComponentController
      })
        .onLoad(() => {
          console.info('CameraModuleTest: OnLoad() is called!')
          surfaceId = mXComponentController.getXComponentSurfaceId()
          console.info('CameraModuleTest: XComponent onLoad surfaceId: ' + surfaceId)
          featureAbility.getWant()
            .then((Want) => {
              const core = Core.getInstance()
              const expectExtend = new ExpectExtend({
                'id': 'extend'
              })
              console.info(TAG + 'Entering expectExtend')
              core.addService('expect', expectExtend)
              console.info(TAG + 'Entering addService')
              core.init()
              console.info(TAG + 'Entering core.init()')
              console.info(TAG + 'Entering subscribeEvent')
              const configService = core.getDefaultService('config')
              configService.setConfig(Want.parameters)
              console.info(TAG + 'Entering configService')
              cameraKit(surfaceId)
              core.execute()
              console.info(TAG + 'Operation successful. Data: ' + JSON.stringify(Want));
            })
            .catch((error) => {
              console.error(TAG + 'Operation failed. Cause: ' + JSON.stringify(error));
            })
        })
        .width('640px')
        .height('480px')
    }
  }
}