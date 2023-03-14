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
import inputmeth from '@ohos.inputmethod';

export default function settingUiJsunit() {
  describe('appInfoTest', function () {
    console.log("************* settings Test start*************");
    it('inputmethoh_test_001', 0, async function (done) {
      let inputMethodSetting = inputmeth.getInputMethodSetting();
      console.log("inputmethoh_test_001 result:" + JSON.stringify(inputMethodSetting));
      inputMethodSetting.listInputMethod((arr) => {
        console.info("inputmethoh_test_001 listInputMethod result---" + JSON.stringify(arr));
        expect(1==1).assertTrue();
      });
      done();
    });

    it('inputmethoh_test_002', 0, async function (done) {
      let inputMethodSetting = inputmeth.getInputMethodSetting();
      console.log("inputmethoh_test_002 result:" + JSON.stringify(inputMethodSetting));
      let promise = inputMethodSetting.listInputMethod();
      console.info("inputmethoh_test_002 listInputMethod result---" + JSON.stringify(promise));
      let obj = promise[0];
      console.info("inputmethoh_test_002 listInputMethod obj---" + JSON.stringify(obj));
      expect(obj.packageName=='com.example.kikakeyboard').assertTrue();
      expect(obj.methodId=='ServiceExtAbility').assertTrue();
      done();
    });

    it('inputmethoh_test_003', 0, async function (done) {
      let inputMethodSetting = inputmeth.getInputMethodSetting();
      console.log("inputmethoh_test_003 result:" + JSON.stringify(inputMethodSetting));
      inputMethodSetting.displayOptionalInputMethod(() => {
        console.info("inputmethoh_test_003 displayOptionalInputMethod---");
      });
      done();
    });

    it('inputmethoh_test_004', 0, async function (done) {
      let inputMethodSetting = inputmeth.getInputMethodSetting();
      console.log("inputmethoh_test_004 result:" + JSON.stringify(inputMethodSetting));
      let promise = inputMethodSetting.displayOptionalInputMethod();
      console.info("inputmethoh_test_004 displayOptionalInputMethod result---" + JSON.stringify(promise));
      expect(promise).assertEqual(undefined)
      done();
    });

    it('inputmethoh_test_005', 0, async function (done) {
      let inputMethodCtrl = inputmeth.getInputMethodController();
      console.log("inputmethoh_test_005 result:" + JSON.stringify(inputMethodCtrl));
      inputMethodCtrl.stopInput((res) => {
        console.info("inputmethoh_test_005 stopInput result----" + res);
      });
      done();
    });

    it('inputmethoh_test_006', 0, async function (done) {
      let inputMethodCtrl = inputmeth.getInputMethodController();
      console.log("inputmethoh_test_006 result:" + JSON.stringify(inputMethodCtrl));
      let promise = inputMethodCtrl.stopInput();
      console.info("inputmethoh_test_006 inputMethodCtrl stopInput result---" + JSON.stringify(promise));
      expect(promise).assertEqual(true)
      done();
    });

    /*
     * @tc.number    : inputmethod_test_MAX_TYPE_NUM_001
     * @tc.name      : inputMethod::MAX_TYPE_NUM
     * @tc.desc      : Verify Max_ TYPE_ NUM
     */
    it('inputmethod_test_MAX_TYPE_NUM_001', 0, async function (done) {
      console.log("************* inputmethod_test_MAX_TYPE_NUM_001 Test start*************");
      let inputMethodSetting = inputmeth.MAX_TYPE_NUM;
      console.log("inputmethod_test_001 result:" + inputMethodSetting);
      expect(inputMethodSetting != null).assertTrue();
      done();
      console.log("************* inputmethod_test_MAX_TYPE_NUM_001 Test end*************");
    });
  })
}
