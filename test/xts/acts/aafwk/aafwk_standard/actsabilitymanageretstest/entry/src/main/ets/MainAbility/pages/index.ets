/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import file from '@system.file';
import {Core, ExpectExtend, ReportExtend} from "deccjsunit/index.ets";
import testsuite from "../test/List.test.ets";
import featureAbility from "@ohos.ability.featureAbility";

async function startService2() {
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility2",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0100 startAbility succeeded: ' + JSON.stringify(data));
}
async function startService3() {
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility3",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0200 startAbility succeeded: ' + JSON.stringify(data));
}
async function startService4() {
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility4",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0300 startAbility succeeded: ' + JSON.stringify(data));
}
async function startService5() {
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility5",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0400 startAbility succeeded: ' + JSON.stringify(data));
}
async function startService6(){
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility6",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0500 startAbility succeeded: ' + JSON.stringify(data));
}
async function startService7(){
  var str = {
    "want": {
      "bundleName": "com.ohos.acecollaboration",
      "abilityName": "com.ohos.acecollaboration.ServiceAbility7",
    },
  };
  var data = featureAbility.startAbility(str);
  console.info('particleAbility_connectAbility_test_0600 startAbility succeeded: ' + JSON.stringify(data));
}

@Entry
@Component
struct MyComponent {
  aboutToAppear() {
    console.info("start run testcase!!!!")
    featureAbility.getWant()
      .then((Want) => {
        const core = Core.getInstance();
        const expectExtend = new ExpectExtend({
          'id': 'extend'
        });
        core.addService('expect', expectExtend);
        const reportExtend = new ReportExtend(file);
        core.addService('report', reportExtend);
        core.init();
        core.subscribeEvent('task', reportExtend);
        const configService = core.getDefaultService('config');
        console.info('parameters---->' + JSON.stringify(Want.parameters));
        configService.setConfig(Want.parameters);
        testsuite();
        core.execute();
        console.info('Operation successful. Data: ' + JSON.stringify(Want));
      })
      .catch((error) => {
        console.error('Operation failed. Cause: ' + JSON.stringify(error));
      })
  }

  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Text('ACE ETS TEST 组件协同')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      Button('disconnectAbility_promise')
        .fontSize(40)
        .height(40)
        .margin({ top: 20 })
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService2();
        });
      Button('disconnectAbility_callback')
        .fontSize(40)
        .height(40)
        .margin({ top: 20 })
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService3();
        });
      Button('connectotherAbility')
        .fontSize(40)
        .height(40)
        .margin({ top: 20 })
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService4();
        });
      Button('connectnull')
        .fontSize(40)
        .height(40)
        .margin({ top: 20 })
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService5();
        });
      Button('connecterror')
        .fontSize(40)
        .height(40)
        .margin({top:20})
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService6();
        });
      Button('connectAPI8')
        .fontSize(40)
        .height(40)
        .margin({top:20})
        .fontWeight(FontWeight.Bold)
        .onClick(() => {
          startService7();
        });

    }
    .width('100%')
    .height('100%')
  }
}

