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
import inputMethodEngine from '@ohos.inputMethodEngine';
import Utils from './Utils';
import router from '@system.router';
import events_emitter from '@ohos.events.emitter';

export default function inputMethodEngineJsunit() {
  describe('appInfoTest', function () {
    var mKeyboardDelegate = null;
    var inputMethodEngineObject = inputMethodEngine.getInputMethodEngine();
    var textInputClient = null;
    var kbController = null;

    console.log("************* inputMethodEngine Test start*************");
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/input',
      }
      try {
        router.clear();
        let pages = router.getState();
        if (!("input" == pages.name)) {
          let result = await router.push(options);
          await Utils.sleep(1000);
        }
      } catch (err) {
        console.error("push input page error: " + err);
      }
      done();
    });

    afterEach(async function () {
      console.log("inputMethodEngine afterEach start:" + inputMethodEngineObject);
      await Utils.sleep(1000);
    });

    it('inputMethodEngine_test_000', 0, async function (done) {
      inputMethodEngineObject.on('inputStart', (kbController, textInputClient) => {
        console.log("inputMethodEngine beforeEach inputStart:" + JSON.stringify(kbController));
        console.log("inputMethodEngine beforeEach inputStart:" + JSON.stringify(textInputClient));
        textInputClient = textInputClient;
        kbController = kbController;
      });
      inputMethodEngineObject.on('keyboardShow', (err) => {
        console.log("inputMethodEngine beforeEach keyboardShow:" + err);
      });
      inputMethodEngineObject.on('keyboardHide', (err) => {
        console.log("inputMethodEngine beforeEach keyboardHide:" + err);
      });
      mKeyboardDelegate = inputMethodEngine.createKeyboardDelegate();
      mKeyboardDelegate.on('keyDown', (keyEvent) => {
        console.log("inputMethodEngine beforeEach keyDown:" + keyEvent);
        expect(keyEvent.keyCode).assertEqual('1');
      });
      mKeyboardDelegate.on('keyUp', (keyEvent) => {
        console.log("inputMethodEngine beforeEach keyUp:" + keyEvent.keyCode);
        expect(keyEvent.keyCode).assertEqual('1');
      });
      mKeyboardDelegate.on('cursorContextChange', (x, y, height) => {
        console.log("inputMethodEngine beforeEach cursorContextChange x:" + x);
        console.log("inputMethodEngine beforeEach cursorContextChange y:" + y);
        console.log("inputMethodEngine beforeEach cursorContextChange height:" + height);
      });
      mKeyboardDelegate.on('selectionChange', (oldBegin, oldEnd, newBegin, newEnd) => {
        console.log("inputMethodEngine beforeEach selectionChange oldBegin:" + oldBegin);
        console.log("inputMethodEngine beforeEach selectionChange oldEnd:" + oldEnd);
        console.log("inputMethodEngine beforeEach selectionChange newBegin:" + newBegin);
        console.log("inputMethodEngine beforeEach selectionChange newEnd:" + newEnd);
      });
      mKeyboardDelegate.on('textChange', (text) => {
        console.log("inputMethodEngine beforeEach textChange:" + text);
      });
      done();
    });

    it('inputMethodEngine_test_001', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_UNSPECIFIED;
      console.log("inputMethodEngine_test_001 result:" + keyType);
      expect(keyType).assertEqual(0);
      done();
    });

    it('inputMethodEngine_test_002', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_GO;
      console.log("inputMethodEngine_test_002 result:" + keyType);
      expect(keyType).assertEqual(2);
      done();
    });

    it('inputMethodEngine_test_003', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_SEARCH;
      console.log("inputMethodEngine_test_003 result:" + keyType);
      expect(keyType).assertEqual(3);
      done();
    });

    it('inputMethodEngine_test_004', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_SEND;
      console.log("inputMethodEngine_test_004 result:" + keyType);
      expect(keyType).assertEqual(4);
      done();
    });

    it('inputMethodEngine_test_005', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_NEXT;
      console.log("inputMethodEngine_test_005 result:" + keyType);
      expect(keyType).assertEqual(5);
      done();
    });

    it('inputMethodEngine_test_006', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_DONE;
      console.log("inputMethodEngine_test_006 result:" + keyType);
      expect(keyType).assertEqual(6);
      done();
    });

    it('inputMethodEngine_test_007', 0, async function (done) {
      let keyType = inputMethodEngine.ENTER_KEY_TYPE_PREVIOUS;
      console.log("inputMethodEngine_test_007 result:" + keyType);
      expect(keyType).assertEqual(7);
      done();
    });

    it('inputMethodEngine_test_008', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_NULL;
      console.log("inputMethodEngine_test_008 result:" + keyType);
      expect(keyType).assertEqual(-1);
      done();
    });

    it('inputMethodEngine_test_009', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_TEXT;
      console.log("inputMethodEngine_test_009 result:" + keyType);
      expect(keyType).assertEqual(0);
      done();
    });

    it('inputMethodEngine_test_010', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_NUMBER;
      console.log("inputMethodEngine_test_010 result:" + keyType);
      expect(keyType).assertEqual(2);
      done();
    });

    it('inputMethodEngine_test_011', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_PHONE;
      console.log("inputMethodEngine_test_011 result:" + keyType);
      expect(keyType).assertEqual(3);
      done();
    });

    it('inputMethodEngine_test_012', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_DATETIME;
      console.log("inputMethodEngine_test_012 result:" + keyType);
      expect(keyType).assertEqual(4);
      done();
    });

    it('inputMethodEngine_test_013', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_EMAIL;
      console.log("inputMethodEngine_test_013 result:" + keyType);
      expect(keyType).assertEqual(5);
      done();
    });

    it('inputMethodEngine_test_014', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_URI;
      console.log("inputMethodEngine_test_014 result:" + keyType);
      expect(keyType).assertEqual(6);
      done();
    });

    it('inputMethodEngine_test_015', 0, async function (done) {
      let keyType = inputMethodEngine.PATTERN_PASSWORD;
      console.log("inputMethodEngine_test_015 result:" + keyType);
      expect(keyType).assertEqual(7);
      done();
    });

    it('inputMethodEngine_test_016', 0, async function (done) {
      let keyType = inputMethodEngine.FLAG_SELECTING;
      console.log("inputMethodEngine_test_016 result:" + keyType);
      expect(keyType).assertEqual(2);
      done();
    });

    it('inputMethodEngine_test_017', 0, async function (done) {
      let keyType = inputMethodEngine.FLAG_SINGLE_LINE;
      console.log("inputMethodEngine_test_017 result:" + keyType);
      expect(keyType).assertEqual(1);
      done();
    });

    it('inputMethodEngine_test_018', 0, async function (done) {
      let keyType = inputMethodEngine.DISPLAY_MODE_PART;
      console.log("inputMethodEngine_test_018 result:" + keyType);
      expect(keyType).assertEqual(0);
      done();
    });

    it('inputMethodEngine_test_019', 0, async function (done) {
      let keyType = inputMethodEngine.DISPLAY_MODE_FULL;
      console.log("inputMethodEngine_test_019 result:" + keyType);
      expect(keyType).assertEqual(1);
      done();
    });

    it('inputMethodEngine_test_020', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_ASCII;
      console.log("inputMethodEngine_test_020 result:" + keyType);
      expect(keyType).assertEqual(20);
      done();
    });

    it('inputMethodEngine_test_021', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_NONE;
      console.log("inputMethodEngine_test_021 result:" + keyType);
      expect(keyType).assertEqual(0);
      done();
    });

    it('inputMethodEngine_test_022', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_AUTO_CAP_CHARACTERS;
      console.log("inputMethodEngine_test_022 result:" + keyType);
      expect(keyType).assertEqual(2);
      done();
    });

    it('inputMethodEngine_test_023', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_AUTO_CAP_SENTENCES;
      console.log("inputMethodEngine_test_023 result:" + keyType);
      expect(keyType).assertEqual(8);
      done();
    });

    it('inputMethodEngine_test_024', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_AUTO_WORDS;
      console.log("inputMethodEngine_test_024 result:" + keyType);
      expect(keyType).assertEqual(4);
      done();
    });

    it('inputMethodEngine_test_025', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_MULTI_LINE;
      console.log("inputMethodEngine_test_025 result:" + keyType);
      expect(keyType).assertEqual(1);
      done();
    });

    it('inputMethodEngine_test_026', 0, async function (done) {
      let keyType = inputMethodEngine.OPTION_NO_FULLSCREEN;
      console.log("inputMethodEngine_test_026 result:" + keyType);
      expect(keyType).assertEqual(10);
      done();
    });

    it('inputMethodEngine_test_027', 0, async function (done) {
      let rect = await Utils.getComponentRect('TextInput')
      console.info("[inputMethodEngine_test_027] rectInfo is " + JSON.stringify(rect));
      let x_value = rect.left + (rect.right - rect.left) / 10
      let y_value = rect.top + (rect.bottom - rect.top) / 10
      console.info("[inputMethodEngine_test_027] onTouch location is: " + "[x]=== " + x_value + "   [y]===" + y_value);
      let point: TouchObject = {
        id: 1,
        x: x_value,
        y: y_value,
        type: TouchType.Move,
      }
      console.info('[inputMethodEngine_test_027] testSendTouchEvent ' + sendTouchEvent(point));
      await Utils.sleep(1000)
      console.info('[inputMethodEngine_test_027] END');
      done();
    });

    it('inputMethodEngine_test_028', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.sendKeyFunction(0, (value) => {
          console.log("inputMethodEngine_test_028 textInputClient sendKeyFunction:" + value);
          expect(value).assertEqual(true);
        });
      }
      done();
    });

    it('inputMethodEngine_test_029', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.sendKeyFunction(0);
        promise.then(res => {
          console.info("inputMethodEngine_test_029 listInputMethod promise result-----" + JSON.stringify(res));
          expect(res).assertEqual(true);
        }).catch(err => {
          console.info("inputMethodEngine_test_029 listInputMethod promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_030', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.deleteForward(1, (value) => {
          console.log("inputMethodEngine_test_030 deleteForward:" + value);
          expect(value).assertEqual(true);
        });
      }
      done();
    });

    it('inputMethodEngine_test_031', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.deleteForward(1);
        promise.then(res => {
          console.info("inputMethodEngine_test_031 deleteForward promise result-----" + JSON.stringify(res));
          expect(res).assertEqual(true);
        }).catch(err => {
          console.info("inputMethodEngine_test_031 deleteForward promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_032', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.deleteBackward(1, (value) => {
          console.log("inputMethodEngine_test_032 deleteBackward:" + value);
          expect(value).assertEqual(true);
        });
      }
      done();
    });

    it('inputMethodEngine_test_033', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.deleteBackward(1);
        promise.then(res => {
          console.info("inputMethodEngine_test_033 deleteBackward promise result-----" + JSON.stringify(res));
          expect(res).assertEqual(true);
        }).catch(err => {
          console.info("inputMethodEngine_test_033 deleteBackward promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_034', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.InsertText('test', (value) => {
          console.log("inputMethodEngine_test_034 InsertText:" + value);
          expect(value).assertEqual(true);
        });
      }
      done();
    });

    it('inputMethodEngine_test_035', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.InsertText('test');
        promise.then(res => {
          console.info("inputMethodEngine_test_035 InsertText promise result-----" + JSON.stringify(res));
          expect(res).assertEqual(true);
        }).catch(err => {
          console.info("inputMethodEngine_test_035 InsertText promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_036', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.getForward(1, (value) => {
          console.log("inputMethodEngine_test_036 getForward:" + value);
          expect(value).assertEqual(true);
        });
      }
      done();
    });

    it('inputMethodEngine_test_037', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.getForward(1);
        promise.then(res => {
          console.info("inputMethodEngine_test_037 getForward promise result-----" + JSON.stringify(res));
          expect(res).assertEqual(true);
        }).catch(err => {
          console.info("inputMethodEngine_test_037 getForward promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_038', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        textInputClient.getEditorAttribute(1, (editorAttribute) => {
          console.log("inputMethodEngine_test_036 getEditorAttribute:" + value);
          expect(editorAttribute.inputPattern).assertEqual(1);
          expect(editorAttribute.enterKeyType).assertEqual(1);
        });
      }
      done();
    });

    it('inputMethodEngine_test_039', 0, async function (done) {
      if (textInputClient == null) {
        expect(textInputClient == null).assertEqual(true);
      } else {
        let promise = textInputClient.getEditorAttribute();
        promise.then(res => {
          console.info("inputMethodEngine_test_037 getEditorAttribute promise result-----" + JSON.stringify(res));
          expect(res.inputPattern).assertEqual(1);
          expect(res.enterKeyType).assertEqual(1);
        }).catch(err => {
          console.info("inputMethodEngine_test_037 getEditorAttribute promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

    it('inputMethodEngine_test_040', 0, async function (done) {
      if (kbController == null) {
        expect(kbController == null).assertEqual(true);
      } else {
        kbController.hideKeyboard(() => {
          console.log("inputMethodEngine_test_040 hideKeyboard:" + value);
          expect(1 == 1).assertTrue();
        });
      }
      done();
    });

    it('inputMethodEngine_test_041', 0, async function (done) {
      if (kbController == null) {
        expect(kbController == null).assertEqual(true);
      } else {
        let promise = kbController.hideKeyboard();
        promise.then(res => {
          console.info("inputMethodEngine_test_037 hideKeyboard promise result-----" + JSON.stringify(res));
          expect(1 == 1).assertTrue();
        }).catch(err => {
          console.info("inputMethodEngine_test_037 hideKeyboard promise error----" + JSON.stringify(err));
          expect().assertFail();
        });
      }
      done();
    });

  })
}
