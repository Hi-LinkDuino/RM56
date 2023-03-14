/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License')
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import {describe, beforeAll, afterAll, it, expect, afterEach} from 'deccjsunit/index.ets';
import http from '@ohos.net.http';

export default function httpJsunit() {
  describe("httpJsunitTest", function () {
    /**
     * @tc.number Telephony_Http_Http_createHttp_0100
     * @tc.name createHttp
     * @tc.desc Test Create HTTP authentication.
     */
    it("Telephony_Http_Http_createHttp_0100", 0, async function (done) {
      console.log("-----------------------http createHttp Test is starting-----------------------");
      var httpRequest = http.createHttp();
      expect(httpRequest != null).assertTrue();
      console.log("-----------------------http createHttp Test end-----------------------");
      done();
    });

  })

};

