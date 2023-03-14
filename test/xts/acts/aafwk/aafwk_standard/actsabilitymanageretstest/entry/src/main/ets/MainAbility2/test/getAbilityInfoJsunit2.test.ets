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
import {describe, it, expect} from "deccjsunit/index.ets";
import Utils from './Utils';
import ability_featureAbility from '@ohos.ability.featureAbility';

export default function getAbilityInfoJsunit() {
  describe('appInfoTest', function () {

    /**
     * @tc.number: context_getAbilityInfo_test_0300
     * @tc.name: getAbilityInfo:Query the current attributionability details.
     * @tc.desc: Check the return type of the interface (by Callback)
     * @tc.level  0
     */
    it('context_getAbilityInfo_test_0300', 0, async function (done) {
      try{
        console.info('[context_getAbilityInfo_test_0300] START2');
        var timeOldStamp = await Utils.getNowTime();
        var context = ability_featureAbility.getContext();
        await context.getAbilityInfo((error, data)=>{
          var timeNewStamp= Utils.getNowTime();
          Utils.getDurationTime('context_getAbilityInfo_test_0300',timeOldStamp,timeNewStamp);
          console.info('[context_getAbilityInfo_test_0300] data is:' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
        });
      } catch (error){
        console.error('[context_getAbilityInfo_test_0300] failed: ' + JSON.stringify(error));
      }
      var callback = await context.getAbilityInfo();
      console.info('---[context_getAbilityInfo_test_0300] check---');
      checkAbilityInfo2('context_getAbilityInfo_test_0300',callback);
      console.info('[context_getAbilityInfo_test_0300] FINISH');
      done();
    })

    function checkAbilityInfo2(msg,data) {
      console.log(msg+ "checkAbilityInfo2 start  " + JSON.stringify(data));
      console.log(msg+ "checkAbilityInfo2 bundleName : " + data.bundleName);
      console.log(msg+ "checkAbilityInfo2 name : " + data.name);
      console.log(msg+ "checkAbilityInfo2 label : " + data.label);
      console.log(msg+ "checkAbilityInfo2 description : " + data.description);
      console.log(msg+ "checkAbilityInfo2 icon : " + data.icon);
      console.log(msg+ "checkAbilityInfo2 descriptionId : " + data.descriptionId);
      console.log(msg+ "checkAbilityInfo2 iconId : " + data.iconId);
      console.log(msg+ "checkAbilityInfo2 moduleName : " + data.moduleName);
      console.log(msg+ "checkAbilityInfo2 process : " + data.process);
      console.log(msg+ "checkAbilityInfo2 targetAbility : " + data.targetAbility);
      console.log(msg+ "checkAbilityInfo2 backgroundModes : " + data.backgroundModes);
      console.log(msg+ "checkAbilityInfo2 isVisible : " + data.isVisible);
      console.log(msg+ "checkAbilityInfo2 formEnabled : " + data.formEnabled);
      console.log(msg+ "checkAbilityInfo2 type : " + data.type)
      console.log(msg+ "checkAbilityInfo2 orientation : " + data.orientation);
      console.log(msg+ "checkAbilityInfo2 launchMode : " + data.launchMode);
      console.log(msg+ "checkAbilityInfo2 permissions length : " + data.permissions.length);
      for (var j = 0; j < data.permissions.length; j++) {
        console.log(msg+ "getAbilityInfo2 data.permissions[" + j + "] : " + data.permissions[j]);
      }
      console.log(msg+ "checkAbilityInfo2 deviceTypes length : " + data.deviceTypes.length);
      for (var k = 0; k < data.deviceTypes.length; k++) {
        console.log(msg+ "getAbilityInfo2 data.deviceTypes[" + k + "] : " + data.deviceTypes[k]);
      }
      console.log(msg+ "checkAbilityInfo2 deviceCapabilities length : " + data.deviceCapabilities.length);
      for (var i = 0; i < data.deviceCapabilities.length; i++) {
        console.log(msg+ "getAbilityInfo2 data.deviceCapabilities[" + i + "] : " + data.deviceCapabilities[i]);
      }
      console.log(msg+ "checkAbilityInfo2 readPermission : " + data.readPermission);
      console.log(msg+ "checkAbilityInfo2 writePermission : " + data.writePermission);
      console.log(msg+ "checkAbilityInfo2 formEntity : " + data.formEntity);
      console.log(msg+ "checkAbilityInfo2 minFormHeight : " + data.minFormHeight);
      console.log(msg+ "checkAbilityInfo2 defaultFormHeight : " + data.defaultFormHeight);
      console.log(msg+ "checkAbilityInfo2 minFormWidth : " + data.minFormWidth);
      console.log(msg+ "checkAbilityInfo2 defaultFormWidth : " + data.defaultFormWidth);
      console.log(msg+ "checkAbilityInfo2 uri : " + data.uri);
      console.log(msg+ "checkAbilityInfo2 customizeData : " + data.customizeData);
      console.log(msg+ "checkAbilityInfo2 labelId : " + data.labelId);
      console.log(msg+ "checkAbilityInfo2 subType : " + data.subType);

      expect(typeof (data)).assertEqual("object");
      expect(typeof (data.bundleName)).assertEqual("string");
      expect(typeof (data.name)).assertEqual("string");
      expect(typeof (data.label)).assertEqual("string");
      expect(typeof (data.description)).assertEqual("string");
      expect(typeof (data.icon)).assertEqual("string");
      expect(typeof (data.labelId)).assertEqual("number");
      expect(typeof (data.descriptionId)).assertEqual("number");
      expect(typeof (data.iconId)).assertEqual("number");
      expect(typeof (data.moduleName)).assertEqual("string");
      expect(typeof (data.process)).assertEqual("string");
      expect(typeof (data.targetAbility)).assertEqual("string");
      expect(typeof (data.backgroundModes)).assertEqual("number");
      expect(typeof (data.isVisible)).assertEqual("boolean");
      expect(typeof (data.formEnabled)).assertEqual("boolean");
      expect(typeof (data.type)).assertEqual("number");
      expect(typeof (data.subType)).assertEqual("number");
      expect(typeof (data.orientation)).assertEqual("number");
      expect(typeof (data.launchMode)).assertEqual("number");
      expect(Array.isArray(data.permissions)).assertEqual(true);
      expect(Array.isArray(data.deviceTypes)).assertEqual(true);
      expect(Array.isArray(data.deviceCapabilities)).assertEqual(true);
      expect(typeof (data.readPermission)).assertEqual("string");
      expect(typeof (data.writePermission)).assertEqual("string");
      expect(typeof (data.applicationInfo)).assertEqual("object");
      expect(typeof (data.formEntity)).assertEqual("number");
      expect(typeof (data.minFormHeight)).assertEqual("number");
      expect(typeof (data.defaultFormHeight)).assertEqual("number");
      expect(typeof (data.minFormWidth)).assertEqual("number");
      expect(typeof (data.defaultFormWidth)).assertEqual("number");
      expect(typeof (data.uri)).assertEqual("string");

      expect(data.bundleName).assertEqual("com.ohos.acecollaboration");
      expect(data.name).assertEqual("com.ohos.acecollaboration.MainAbility2");
      expect(data.label).assertEqual("$string:entry_MainAbility2");
      expect(data.description).assertEqual("$string:description_mainability2");
      expect(data.icon).assertEqual("$media:icon");
      expect(data.descriptionId > 0).assertTrue();
      expect(data.iconId > 0).assertTrue();
      expect(data.moduleName).assertEqual("entry");
      expect(data.process).assertEqual("");
      expect(data.targetAbility).assertEqual("");
      expect(data.backgroundModes).assertEqual(0);
      expect(data.isVisible).assertEqual(false);
      expect(data.formEnabled).assertEqual(false);
      expect(data.type).assertEqual(1);
      expect(data.orientation).assertEqual(0);
      expect(data.launchMode).assertEqual(1);
      expect(data.deviceTypes[0]).assertEqual("phone");
      expect(data.readPermission).assertEqual("");
      expect(data.writePermission).assertEqual("");
      checkApplicationInfo2(msg,data.applicationInfo);
      expect(data.formEntity).assertEqual(0);
      expect(data.minFormHeight).assertEqual(0);
      expect(data.defaultFormHeight).assertEqual(0);
      expect(data.minFormWidth).assertEqual(0);
      expect(data.defaultFormWidth).assertEqual(0);
      expect(data.uri).assertEqual("");
      expect(data.labelId >0).assertTrue();
      expect(data.subType).assertEqual(0);
      console.log(msg+ "---checkAbilityInfo End---  ");
    }

    function checkApplicationInfo2(msg,info) {
      console.log(msg+ "checkApplicationInfo start : " + JSON.stringify(info));
      console.log(msg+ "checkApplicationInfo name : " + info.name);
      console.log(msg+ "checkApplicationInfo description : " + info.description);
      console.log(msg+ "checkApplicationInfo descriptionId : " + info.descriptionId);
      console.log(msg+ "checkApplicationInfo systemApp : " + info.systemApp);
      console.log(msg+ "checkApplicationInfo enabled : " + info.enabled);
      console.log(msg+ "checkApplicationInfo label : " + info.label);
      console.log(msg+ "checkApplicationInfo labelId : " + info.labelId);
      console.log(msg+ "checkApplicationInfo icon : " + info.icon);
      console.log(msg+ "checkApplicationInfo iconId : " + info.iconId);
      console.log(msg+ "checkApplicationInfo process : " + info.process);
      console.log(msg+ "checkApplicationInfo supportedModes : " + info.supportedModes);

      console.log(msg+ "checkApplicationInfo moduleSourceDirs length : " + info.moduleSourceDirs.length);
      for (var j = 0; j < info.moduleSourceDirs.length; j++) {
        console.log(msg+ "checkApplicationInfo info.moduleSourceDirs[" + j + "] : " + info.moduleSourceDirs[j]);
      }
      console.log(msg+ "checkApplicationInfo permissions length : " + info.permissions.length);
      for (var k = 0; k < info.permissions.length; k++) {
        console.log(msg+ "checkApplicationInfo info.permissions[" + k + "] : " + info.permissions[k]);
      }
      console.log(msg+ "checkApplicationInfo moduleInfos length : " + info.moduleInfos.length);
      for (var i = 0; i < info.moduleInfos.length; i++) {
        console.log(msg+ "checkApplicationInfo info.moduleInfos[" + i + "].moduleName : " + info.moduleInfos[i].moduleName);
        console.log(msg+ "checkApplicationInfo info.moduleInfos[" + i + "].moduleSourceDir : " + info.moduleInfos[i].moduleSourceDir);
      }
      console.log(msg+ "checkApplicationInfo entryDir : " + info.entryDir);

      expect(typeof (info)).assertEqual("object");
      expect(typeof (info.name)).assertEqual("string");
      expect(typeof (info.description)).assertEqual("string");
      expect(typeof (info.descriptionId)).assertEqual("number");
      expect(typeof (info.systemApp)).assertEqual("boolean");
      expect(typeof (info.enabled)).assertEqual("boolean");
      expect(typeof (info.label)).assertEqual("string");
      expect(typeof (info.labelId)).assertEqual("string");
      expect(typeof (info.icon)).assertEqual("string");
      expect(typeof (info.iconId)).assertEqual("string");
      expect(typeof (info.process)).assertEqual("string");
      expect(typeof (info.supportedModes)).assertEqual("number");
      expect(Array.isArray(info.moduleSourceDirs)).assertEqual(true);
      expect(Array.isArray(info.permissions)).assertEqual(true);
      expect(Array.isArray(info.moduleInfos)).assertEqual(true);
      expect(typeof (info.entryDir)).assertEqual("string");

      expect(info.name).assertEqual("com.ohos.acecollaboration");
      expect(info.description).assertEqual("$string:mainability_description");
      expect(info.descriptionId > 0).assertTrue();
      expect(info.systemApp).assertEqual(true);
      expect(info.enabled).assertEqual(true);
      expect(info.label).assertEqual("$string:entry_MainAbility");
      expect(info.icon).assertEqual("$media:icon");
      expect(info.process).assertEqual("");
      expect(info.supportedModes).assertEqual(0);
      expect(info.entryDir).assertEqual("/data/app/el1/bundle/public/" +
      "com.ohos.acecollaboration/com.ohos.acecollaboration");
      if (info.moduleSourceDirs.length == 1){
        expect(info.moduleInfos[0].moduleName).assertEqual("entry");
        expect(info.moduleInfos[0].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
        "com.ohos.acecollaboration/com.ohos.acecollaboration");
      }else if(info.moduleSourceDirs.length == 2) {
        if (info.moduleInfos[0].moduleName == "myapplication1") {
          expect(info.moduleInfos[0].moduleName).assertEqual("myapplication1");
          expect(info.moduleInfos[0].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
          "com.ohos.acecollaboration/com.example.myapplication1");
          expect(info.moduleInfos[1].moduleName).assertEqual("entry");
          expect(info.moduleInfos[1].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
          "com.ohos.acecollaboration/com.ohos.acecollaboration");
        }else{
          expect(info.moduleInfos[0].moduleName).assertEqual("myapplication2");
          expect(info.moduleInfos[0].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
          "com.ohos.acecollaboration/com.example.myapplication2");
          expect(info.moduleInfos[1].moduleName).assertEqual("entry");
          expect(info.moduleInfos[1].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
          "com.ohos.acecollaboration/com.ohos.acecollaboration");
        }
      }else {
        expect(info.moduleInfos[0].moduleName).assertEqual("myapplication1");
        expect(info.moduleInfos[0].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
        "com.ohos.acecollaboration/com.example.myapplication1");
        expect(info.moduleInfos[1].moduleName).assertEqual("myapplication2");
        expect(info.moduleInfos[1].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
        "com.ohos.acecollaboration/com.example.myapplication2");
        expect(info.moduleInfos[2].moduleName).assertEqual("entry");
        expect(info.moduleInfos[2].moduleSourceDir).assertEqual("/data/app/el1/bundle/public/" +
        "com.ohos.acecollaboration/com.ohos.acecollaboration");
      }
      console.log(msg+ "---checkApplicationInfo End--- ");
    }
  })
}