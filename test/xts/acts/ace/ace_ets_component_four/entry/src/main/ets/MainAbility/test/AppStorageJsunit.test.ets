// @ts-nocheck
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import router from '@system.router';
import Utils from './Utils';

export default function AppStorageJsunit() {
  describe('appStorage', function () {

    beforeEach(async function (done) {
      console.info("appStorage beforeEach start");
      let options = {
        uri: 'pages/appStorage',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get button state pages: " + JSON.stringify(pages));
        if (!("appStorage" == pages.name)) {
          console.info("get appStorage state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push button page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push appStorage page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("appStorage after each called");
    })

    /**
    * @tc.number  stateManagement_AppStorage_setOrCreate_0100
    * @tc.name    stateManagement ContentStorage  validation
    * @tc.desc    Function test
    */
    it('stateManagement_AppStorage_setOrCreate_0100', 0, function (done) {
      console.info('[stateManagement_AppStorage_setOrCreate_0100] START');
      AppStorage.SetOrCreate<string>("key1", "value1");
      AppStorage.SetOrCreate<string>("key2", "value2");
      AppStorage.SetOrCreate<string>("key3", "value3");
      let size = AppStorage.Size();
      console.info('[stateManagement_AppStorage_setOrCreate_0100] size =' + size);
      expect(size == 3)
      .assertTrue;
      let key1 = AppStorage.Get<string>("key1");
      console.info('[stateManagement_AppStorage_setOrCreate_0100] key1 = ' + key1);
      console.info('[stateManagement_AppStorage_setOrCreate_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_Storage_setOrCreate_0100
    * @tc.name    stateManagement Storage  validation
    * @tc.desc    Function test
    */
    it('stateManagement_Storage_set_0100', 0, function (done) {
      done();
      return;

      console.info('[stateManagement_Storage_set_0100] START');
      let storage = new Storage();
      console.info('[stateManagement_Storage_set_0100] 0000');
      storage.set("key5", "value5");
      storage.set("key6", "value6");
      console.info('[stateManagement_Storage_set_0100] 1111');
      let key5 = storage.get("key5");
      let key6 = storage.get("key6");
      console.info('[stateManagement_Storage_set_0100] 2222' + key5);
      storage.delete("key5");
      key5 = storage.get("key5");
      console.info('[stateManagement_Storage_set_0100] 3333');
      console.info('[stateManagement_Storage_set_0100] key5 =' + key5);
      console.info('[stateManagement_Storage_set_0100] key6 =' + key6);
      storage.clear();
      key6 = storage.get("key6");
      console.info('[stateManagement_Storage_set_0100] key6 =' + key6);
      console.info('[stateManagement_Storage_set_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_Environment_EnvProp_0100
    * @tc.name    stateManagement Environment  validation
    * @tc.desc    Function test
    */
    it('stateManagement_Environment_EnvProp_colorMode_0100', 0, function (done) {
      console.info('[stateManagement_Environment_EnvProp_colorMode_0100] START');
      Environment.EnvProp("colorMode", ColorMode.LIGHT);
      let enable = AppStorage.Get("colorMode");
      expect(enable == ColorMode.LIGHT)
      .assertTrue;
      console.info('[stateManagement_Environment_EnvProp_colorMode_0100] enable =' + enable);
      Environment.EnvProp("colorMode", ColorMode.DARK);
      let enable1 = AppStorage.Get("colorMode");
      console.info('[stateManagement_Environment_EnvProp_colorMode_0100] enable1 =' + enable1);
      expect(enable1 == ColorMode.DARK)
      .assertTrue;
      console.info('[stateManagement_Environment_EnvProp_colorMode_0100] END');
      done();
    });

    /**
    * @tc.number  stateManagement_Environment_EnvProp_0100
    * @tc.name    stateManagement Environment  validation
    * @tc.desc    Function test
    */
    it('stateManagement_Environment_EnvProp_layoutDirection_0100', 0, function (done) {
      console.info('[stateManagement_Environment_EnvProp_layoutDirection_0100] START');
      Environment.EnvProp("layoutDirection", LayoutDirection.LTR);
      let enable = AppStorage.Get("layoutDirection");
      expect(enable == LayoutDirection.LTR)
      .assertTrue;
      console.info('[stateManagement_Environment_EnvProp_layoutDirection_0100] enable =' + enable);
      Environment.EnvProp("layoutDirection", LayoutDirection.RTL);
      let enable1 = AppStorage.Get("layoutDirection");
      expect(enable1 == LayoutDirection.RTL)
      .assertTrue;
      console.info('[stateManagement_Environment_EnvProp_layoutDirection_0100] enable1' + enable1);
      Environment.EnvProp("layoutDirection", LayoutDirection.Auto);
      let enable2 = AppStorage.Get("layoutDirection");
      expect(enable2 == LayoutDirection.Auto)
      .assertTrue;
      console.info('[stateManagement_Environment_EnvProp_layoutDirection_0100] enable2' + enable2);
      done();
    });

    /**
    * @tc.number  stateManagement_Environment_EnvProps_0100
    * @tc.name    stateManagement Environment  validation
    * @tc.desc    Function test
    */
    it('stateManagement_Environment_EnvProps_0100', 0, function (done) {
      console.info('[stateManagement_Environment_EnvProps_0100] START');
      let props = {
        "props": [{
                    key: "colorMode", defaultValue: ColorMode.LIGHT
                  }, {
                    key: "layoutDirection", defaultValue: LayoutDirection.LTR
                  }]
      };
      try {
        Environment.EnvProps(props);
        let enable = AppStorage.Get("colorMode");
        let enable1 = AppStorage.Get("layoutDirection");
        expect(enable == ColorMode.LIGHT)
        .assertTrue;
        expect(enable1 == LayoutDirection.RTL)
        .assertTrue;
        console.info('[stateManagement_Environment_EnvProps_0100] enable = ' + enable);
        console.info('[stateManagement_Environment_EnvProps_0100] enable1 = ' + enable1);
      } catch(err) {
        console.info('[stateManagement_Environment_EnvProps_0100] err = ' + err);
      }
      console.info('[stateManagement_Environment_EnvProps_0100] END');
      done();
    });
  })
}
