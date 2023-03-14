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
import settings from '@ohos.settings'
import featureAbility from '@ohos.ability.featureAbility';

export default function settingUiJsunit() {
  describe('appInfoTest', function () {
    console.log("************* settings Test start*************");
    it('settings_uri_test_001', 0,async function (done) {
      var name = 'settings.screen.test';
      var uri = settings.getUriSync(name);
      console.info("[settings_uri_test_001] uri is: " + uri);
      var uri2 = settings.getUriSync(name);
      console.info("[settings_uri_test_001] uri2 is: " + uri2);
      expect(uri).assertEqual(uri2);
      done();
    });

    it('settings_uri_test_002', 0, async function (done) {
      var name = '';
      var uri = settings.getUriSync(name);
      console.info("[settings_uri_test_002] uri is: " + uri);
      expect(uri).assertEqual('dataability:///com.ohos.settingsdata.DataAbility')
      done();
    });

    it('settings_uri_test_003', 0, async function (done) {
      var name = 122.00;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_uri_test_003] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_uri_test_003] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_uri_test_004', 0, async function (done) {
      var name = 'settings.screen.brightness';
      var uri = settings.getUriSync(name);
      console.info("[settings_uri_test_004] uri is: " + uri);
      expect(uri).assertEqual('dataability:///com.ohos.settingsdata.DataAbility/settings.screen.brightness')
      done();
    });

    it('settings_get_value_005', 0, async function (done) {
      var name = 'settings.screen.brightness20';
      var uri = settings.getUriSync(name);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let value = settings.getValueSync(helper, name, "test getValueSync");
      console.info("[settings_get_value_005] value is: " + value);
      expect(value).assertEqual("test getValueSync");
      done();
    });

    it('settings_get_value_006', 0, async function () {
      var name = 'settings.screen.brightness2';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_006] uri is: " + uri);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = {
        "aa": aa
      }
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_006] value is: " + value);
        expect(value).assertEqual("test getValueSync");
      } catch (err) {
        console.error("[settings_get_value_006] error = " + err);
        expect(err == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_007', 0, async function () {
      var name = 'settings.screen.brightness3';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_007] uri is: " + uri);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = '';
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_007] value is: " + value);
        expect(value).assertEqual('');
      } catch (err) {
        console.error("[settings_get_value_007] error = " + err);
        expect(err == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_008', 0, async function (done) {
      var name = 'settings.screen.brightness4';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_008] uri is: " + uri);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = null;
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_008] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_008] error = " + err);
        expect(err == "Error: assertion (valueType == napi_string) failed: Wrong argument[2] type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_009', 0, async function (done) {
      var name = 'settings.screen.brightness5';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_009] uri is: " + uri);
      var helper = "helper";
      try {
        let value = settings.getValueSync(helper, name, "test getValueSync");
        console.info("[settings_get_value_009] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_009] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_010', 0, async function (done) {
      var name = 'settings.screen.brightness6';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_010] uri is: " + uri);
      var helper = null;
      try {
        let value = settings.getValueSync(helper, name, "test getValueSync");
        console.info("[settings_get_value_010] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_010] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_0011', 0, async function (done) {
      var name = 'settings.screen.brightness7';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_0011] uri is: " + uri);
      var helper = "helper";
      let obj = 121;
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_0011] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_0011] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_0012', 0, async function (done) {
      var name = 'settings.screen.brightness8';
      var uri = settings.getUriSync(name);
      console.info("[settings_get_value_0012] uri is: " + uri);
      var helper = null;
      let obj = null;
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_0012] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_0012] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_013', 0, async function (done) {
      var name = 1322.00;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_get_value_013] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_get_value_013] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      try {
        let value = settings.getValueSync(helper, name, "test getValueSync");
        console.info("[settings_get_value_013] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_013] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_014', 0, async function (done) {
      let name = null;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_get_value_014] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_get_value_014] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      try {
        let value = settings.getValueSync(helper, name, "test getValueSync");
        console.info("[settings_get_value_014] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_014] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_0015', 0, async function (done) {
      let name = 1332;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_get_value_0015] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_get_value_0015] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = 22223;
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_0015] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_0015] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_0016', 0, async function () {
      let name = 1332;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_get_value_0016] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_get_value_0016] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = "helper";
      try {
        let value = settings.getValueSync(helper, name, "test getValueSync");
        console.info("[settings_get_value_0016] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_0016] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_get_value_0017', 0, async function (done) {
      let name = 1332;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_get_value_0017] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_get_value_0017] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = "helper";
      let obj = 221323;
      try {
        let value = settings.getValueSync(helper, name, obj);
        console.info("[settings_get_value_0017] value is: " + value);
      } catch (err) {
        console.error("[settings_get_value_0017] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_018', 0, async function (done) {
      let name = 'settings.screen.brightness10';
      var uri = settings.getUriSync(name);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let value = settings.setValueSync(helper, name, "test getValueSync");
      expect(value).assertEqual(true);
      done();
    });

    it('settings_set_value_019', 0, async function (done) {
      let name = 'settings.screen.brightness11';
      var uri = settings.getUriSync(name);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = 32344.00;
      try {
        let value = settings.setValueSync(helper, name, obj);
        console.info("[settings_set_value_019] value is: " + value);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_019] error = " + err);
        expect(err == "Error: assertion (valueType == napi_string) failed: Wrong argument[2] type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_020', 0, async function (done) {
      let name = 'settings.screen.brightness12';
      var uri = settings.getUriSync(name);
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = null;
      try {
        let value = settings.setValueSync(helper, name, obj);
        console.info("[settings_set_value_020] value is: " + value);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_020] error = " + err);
        expect(err == "Error: assertion (valueType == napi_string) failed: Wrong argument[2] type. String expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_021', 0, async function (done) {
      let name = 'settings.screen.brightness13';
      var helper = "helper";
      try {
        let value = settings.setValueSync(helper, name, "test getValueSync");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_021] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_022', 0, async function (done) {
      let name = 'settings.screen.brightness13';
      var helper = null;
      try {
        let value = settings.setValueSync(helper, name, "test getValueSync");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_022] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_023', 0, async function (done) {
      let name = 'settings.screen.brightness14';
      var helper = "helper";
      let obj = 343434.00;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_023] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_024', 0, async function (done) {
      let name = 'settings.screen.brightness14';
      var helper = "helper";
      let obj = null;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_024] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_025', 0, async function (done) {
      let name = 'settings.screen.brightness14';
      var helper = null;
      let obj = 2323.00;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_025] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_026', 0, async function (done) {
      let name = 1332;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_026] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_026] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      try {
        let value = settings.setValueSync(helper, name, "test getValueSync");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_026] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_027', 0, async function (done) {
      let name = null;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_027] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_027] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      try {
        let value = settings.setValueSync(helper, name, "test getValueSync");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_027] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_028', 0, async function (done) {
      let name = 2323;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_028] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_028] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = 232.00;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_028] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_029', 0, async function (done) {
      let name = 2323;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_029] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_029] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = featureAbility.acquireDataAbilityHelper(uri);
      let obj = null;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_029] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_030', 0, async function (done) {
      let name = 2323;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_030] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_030] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = "helper";
      try {
        let value = settings.setValueSync(helper, name, "text value");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_030] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_031', 0, async function (done) {
      let name = 2323;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_031] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_031] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = null;
      try {
        let value = settings.setValueSync(helper, name, "text value");
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_031] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });

    it('settings_set_value_032', 0, async function (done) {
      let name = 2323;
      try {
        var uri = settings.getUriSync(name);
        console.info("[settings_set_value_032] uri is: " + uri);
      } catch (err) {
        let errMsg = err;
        console.error("[settings_set_value_032] error = " + errMsg);
        expect(errMsg == "Error: assertion (valueType == napi_string) failed: Wrong argument type. String expected.")
          .assertTrue();
      }
      var helper = "helper";
      let obj = 2323;
      try {
        let value = settings.setValueSync(helper, name, obj);
        expect(value).assertEqual(true);
      } catch (err) {
        console.error("[settings_set_value_032] error = " + err);
        expect(err == "Error: assertion (valueType == napi_object) failed: Wrong argument[0] type. Object expected.")
          .assertTrue();
      }
      done();
    });
  })
}
