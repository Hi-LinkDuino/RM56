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

export default function httpRequestJsunit() {
  describe("HttpRequestJsunitTest", function () {
    /**
     * @tc.number Telephony_http_HttpRequest_request_0100
     * @tc.name httprequest::request
     * @tc.desc Test Test httprequest property.
     */
    it("Telephony_http_HttpRequest_request_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0100"
      console.log("-----------------------HttpRequest_request Test is starting-----------------------");
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
          expect(data.getResponseCode = 200).assertTrue();
          console.log("-----------------------HttpRequest_request Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequest_request_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequest_request_0200
     * @tc.name httprequest::request
     * @tc.desc Test Test httprequest property.
     */
    it("Telephony_http_HttpRequest_request_0200", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0100"
      console.log("-----------------------HttpRequest_request_01 Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResponseCode = 200).assertTrue();
          console.log("-----------------------HttpRequest_request_01 Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequest_request_0200 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequest_request_0300
     * @tc.name httprequest::request
     * @tc.desc Test Test httprequest property.
     */
    it("Telephony_http_HttpRequest_request_0300", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0100"
      console.log("-----------------------HttpRequest_request_02 Test is starting-----------------------");
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
          expect(data.getResponseCode = 200).assertTrue();
          console.log("-----------------------HttpRequest_request_02 Test end-----------------------");
          done();
        }).catch(error => {
          console.info('HttpRequest_request_0300 asyncCallback error : ' + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequest_request_0300 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_http_HttpRequest_destroy_0100
     * @tc.name httprequest::destroy
     * @tc.desc Test Test httprequest property.
     */

    it("Telephony_http_HttpRequest_destroy_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0200"
      console.log("-----------------------HttpRequest_destroy Test is starting-----------------------");
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
          try{
            httpRequest.destroy();
            expect(true).assertTrue();
            console.log("-----------------------HttpRequest_destroy Test end-----------------------");
            done();
          }catch(error){
            expect().assertFail();
            done();
          }
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequest_destroy_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequest_on_headerReceive_0100
     * @tc.name httprequest::on_headerReceive
     * @tc.desc Test Test httprequest property.
     */

    it("Telephony_http_HttpRequest_on_headerReceive_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0300"
      console.log("-----------------------HttpRequest_on_headerReceive Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.on_headerReceive((data) => {
          console.log(casename + " data: " + JSON.stringify(data));
          expect(true).assertTrue();
          console.log("-----------------------HttpRequest_on_headerReceive Test end-----------------------");
          done();
        });

      } catch (error) {
        console.log("Telephony_http_HttpRequest_on_headerReceive_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequest_off_headerReceive_0100
     * @tc.name httprequest::off_headerReceive
     * @tc.desc Test Test httprequest property.
     */
    it("Telephony_http_HttpRequest_off_headerReceive_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequest_0400"
      console.log("-----------------------HttpRequest_off_headerReceive Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.off_headerReceive((data) => {
          console.log(casename + " data: " + JSON.stringify(data));
          expect(true).assertTrue();
          console.log("-----------------------HttpRequest_off_headerReceive Test end-----------------------");
          done();
        });

      } catch (error) {
        console.log("Telephony_http_HttpRequest_off_headerReceive_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_http_HttpRequest_on_headersReceive_0100
     * @tc.name httprequest::on_headersReceive
     * @tc.desc Test Test httprequest property.
     */

    it("Telephony_http_HttpRequest_on_headersReceive_0100", 0, function (done) {
      var casename = "Telephony_http_HttpRequest_0500"
      console.log("-----------------------HttpRequest_on_headersReceive Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.on("headersReceive", (data) => {
          console.log(casename + " data: " + JSON.stringify(data));
          expect(true).assertTrue();
          console.log("-----------------------HttpRequest_on_headersReceive Test end-----------------------");
          done();
        });

      } catch (error) {
        console.log("Telephony_http_HttpRequest_on_headersReceive_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequest_off_headersReceive_0100
     * @tc.name httprequest::off_headersReceive
     * @tc.desc Test Test httprequest property.
     */
    it("Telephony_http_HttpRequest_off_headersReceive_0100", 0, function (done) {
      var casename = "Telephony_http_HttpRequest_0600"
      console.log("-----------------------HttpRequest_off_headersReceive Test is starting-----------------------");
      try {
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.off("headersReceive", (data) => {
          console.log(casename + " data: " + JSON.stringify(data));
          expect(true).assertTrue();
          console.log("-----------------------HttpRequest_off_headersReceive Test end-----------------------");
          done();
        });

      } catch (error) {
        console.log("Telephony_http_HttpRequest_off_headersReceive_0100 : error = " + error);
        done();
      }
    });


  })

};

