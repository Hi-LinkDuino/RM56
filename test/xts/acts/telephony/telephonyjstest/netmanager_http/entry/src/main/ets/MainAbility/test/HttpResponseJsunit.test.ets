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
import utils from './Utils.ets'

export default function httpResponseJsunit() {
  describe("HttpResponseJsunitTest", function () {

    /**
     * @tc.number Telephony_Http_HttpResponse_result_0100
     * @tc.name HttpResponse::result
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_result_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0100';
      console.log("-----------------------HttpResponse result Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResult() != null).assertTrue();
          console.log("-----------------------HttpResponse result Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_result_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpResponse_result_0200
     * @tc.name HttpResponse::result
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_result_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0100';
      console.log("-----------------------HttpResponse result_01 Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResult() != null).assertTrue();
          console.log("-----------------------HttpResponse result_01 Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_result_0200 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpResponse_result_0300
     * @tc.name HttpResponse::result
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_result_0300", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0100';
      console.log("-----------------------HttpResponse result_02 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getResult() != null).assertTrue();
          console.log("-----------------------HttpResponse result_02 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_result_0300 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpResponse_responseCode_0100
     * @tc.name HttpResponse::responseCode
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_responseCode_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0200';
      console.log("-----------------------HttpResponse responseCode Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResponseCode() != 0).assertTrue();
          console.log("-----------------------HttpResponse responseCode Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_responseCode_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpResponse_responseCode_0200
     * @tc.name HttpResponse::responseCode
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_responseCode_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0200';
      console.log("-----------------------HttpResponse responseCode_01 Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResponseCode() != 0).assertTrue();
          console.log("-----------------------HttpResponse responseCode_01 Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_responseCode_0200 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_Http_HttpResponse_responseCode_0300
     * @tc.name HttpResponse::responseCode
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_responseCode_0300", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0200';
      console.log("-----------------------HttpResponse responseCode_02 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getResponseCode() != 0).assertTrue();
          console.log("-----------------------HttpResponse responseCode_02 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_responseCode_0300 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_HttpResponse_header_0100
     * @tc.name HttpResponse::header
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_header_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0300';
      console.log("-----------------------HttpResponse header Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getHead() != null).assertTrue();
          console.log("-----------------------header result Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_header_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpResponse_header_0200
     * @tc.name HttpResponse::header
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_header_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0300';
      console.log("-----------------------HttpResponse header_01 Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getHead() != null).assertTrue();
          console.log("-----------------------header header_01 Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_header_0200 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_Http_HttpResponse_header_0300
     * @tc.name HttpResponse::header
     * @tc.desc Test Test HttpResponse property.
     */

    it("Telephony_Http_HttpResponse_header_0300", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0300';
      console.log("-----------------------HttpResponse header_02 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getHead() != null).assertTrue();
          console.log("-----------------------HttpResponse header_02 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_header_0300 : error = " + error);
        done();
      }
    });




    /**
     * @tc.number Telephony_Http_HttpResponse_cookies_0100
     * @tc.name HttpResponse::cookies
     * @tc.desc Test Test HttpResponse property.
     */


    it("Telephony_Http_HttpResponse_cookies_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0400';
      console.log("-----------------------HttpResponse cookies Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getCookies != null).assertTrue();
          console.log("-----------------------HttpResponse cookies Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_cookies_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_Http_HttpResponse_cookies_0200
     * @tc.name HttpResponse::cookies
     * @tc.desc Test Test HttpResponse property.
     */


    it("Telephony_Http_HttpResponse_cookies_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0400';
      console.log("-----------------------HttpResponse cookies_01 Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getCookies != null).assertTrue();
          console.log("-----------------------HttpResponse cookies_01 Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_cookies_0200 : error = " + error);
        done();
      }
    });




    /**
     * @tc.number Telephony_Http_HttpResponse_cookies_0300
     * @tc.name HttpResponse::cookies
     * @tc.desc Test Test HttpResponse property.
     */


    it("Telephony_Http_HttpResponse_cookies_0300", 0, async function (done) {
      var casename = 'Telephony_Http_HttpResponse_0400';
      console.log("-----------------------HttpResponse cookies_02 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 60
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getCookies() != null).assertTrue();
          console.log("-----------------------HttpResponse cookies_02 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpResponse_cookies_0300 : error = " + error);
        done();
      }
    });

  })

};

