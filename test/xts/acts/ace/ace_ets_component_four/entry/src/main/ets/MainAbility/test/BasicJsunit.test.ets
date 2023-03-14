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

export default function basicJsunit() {
  describe('basicTest', function () {
    let callback: Callback<void> = (data) => {
      console.log(data);
      expect(true).assertTrue();
    }

    function callbackTest(callback: Callback<void>): void {
      callback("callback");
    }

    let errorCallback: ErrorCallback<void> = (error) => {
      console.log(error);
      expect(true).assertTrue();
    }

    function errorCallbackTest(errorCallback: ErrorCallback<void>): void {
      errorCallback("errorCallback");
    }

    let asyncCallback: AsyncCallback<void> = (error, data) => {
      console.log(data);
      expect(true).assertTrue();
    }

    function asyncCallbackTest(asyncCallback: AsyncCallback<void>): void {
      asyncCallback({
        code: 1
      }, "asyncCallback");
    }

    it('testBasic01', 0, async function (done) {
      console.info('[testBasic01] START');
      callbackTest(callback);
      console.info('[testBasic01] END');
      done();
    });

    it('testBasic02', 0, async function (done) {
      console.info('[testBasic02] START');
      errorCallbackTest(errorCallback);
      console.info('[testBasic02] END');
      done();
    });

    it('testBasic03', 0, async function (done) {
      console.info('[testBasic03] START');
      asyncCallbackTest(asyncCallback);
      console.info('[testBasic03] END');
      done();
    });
  });
}