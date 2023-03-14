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

export default function requestMethodJsunit() {
  describe("requestMethodJsunit", function () {

    /**
     * @tc.number Telephony_http_HttpRequestMethod_options_0100
     * @tc.name HttpRequestMethod::options
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_options_0100", 0, async function (done) {
      var casename = 'Telephony_http_HttpRequestMethod_0100';
      console.log("-----------------------HttpRequestMethod_options Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "OPTIONS",
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
          console.log("-----------------------HttpRequestMethod_options Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestMethod_options_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_options_0200
     * @tc.name HttpRequestMethod::options
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_options_0200", 0, async function (done) {
      var casename = 'Telephony_http_HttpRequestMethod_0100';
      console.log("-----------------------HttpRequestMethod_options_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "OPTIONS",
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
          console.log("-----------------------HttpRequestMethod_options_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_options_0200 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_GET_0100
     * @tc.name HttpRequestMethod::GET
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_GET_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0200"
      console.log("-----------------------HttpRequestMethod_GET Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "GET",
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
          console.log("-----------------------HttpRequestMethod_GET Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestMethod_GET_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_GET_0200
     * @tc.name HttpRequestMethod::GET
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_GET_0200", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0200"
      console.log("-----------------------HttpRequestMethod_GET_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "GET",
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
          console.log("-----------------------HttpRequestMethod_GET_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_GET_0200 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_http_HttpRequestMethod_POST_0100
     * @tc.name HttpRequestMethod::POST
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_POST_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0300"
      console.log("-----------------------HttpRequestMethod_POST Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "POST",
          extraData: {
            //tn=albumsdetail&word=渐变风格插画&fr=albumslist&album_tab=设计素材&album_id=409&rn=30
            tn: "albumsdetail",
            work: "渐变风格插画",
            fr: "albumslist",
            album_tab: "设计素材",
            album_id: "409",
            rn: "30"
          },
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
        httpRequest.request("https://image.baidu.com/search/albumsdetail/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResponseCode = 200).assertTrue();
          console.log("-----------------------HttpRequestMethod_POST Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestMethod_POST_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_POST_0100
     * @tc.name HttpRequestMethod::POST
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_POST_0200", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0300"
      console.log("-----------------------HttpRequestMethod_POST_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "POST",
          extraData: {
            //tn=albumsdetail&word=渐变风格插画&fr=albumslist&album_tab=设计素材&album_id=409&rn=30
            tn: "albumsdetail",
            work: "渐变风格插画",
            fr: "albumslist",
            album_tab: "设计素材",
            album_id: "409",
            rn: "30"
          },
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
        httpRequest.request("https://image.baidu.com/search/albumsdetail/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getResponseCode = 200).assertTrue();
          console.log("-----------------------HttpRequestMethod_POST_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_POST_0200 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_TRACE_0100
     * @tc.name HttpRequestMethod::TRACE
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_TRACE_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0400"
      console.log("-----------------------HttpRequestMethod_TRACE Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "TRACE",
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
          console.log("-----------------------HttpRequestMethod_TRACE Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestMethod_TRACE_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_http_HttpRequestMethod_TRACE_0200
     * @tc.name HttpRequestMethod::TRACE
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_TRACE_0200", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0400"
      console.log("-----------------------HttpRequestMethod_TRACE_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "TRACE",
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
          console.log("-----------------------HttpRequestMethod_TRACE_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_TRACE_0200 : error = " + error);
        done();
      }
    });





    /**
     * @tc.number Telephony_http_HttpRequestMethod_PUT_0100
     * @tc.name HttpRequestMethod::PUT
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_PUT_0100", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0500"
      console.log("-----------------------HttpRequestMethod_PUT Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "PUT",
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
          console.log("-----------------------HttpRequestMethod_PUT Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_PUT_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestMethod_PUT_0200
     * @tc.name HttpRequestMethod::PUT
     * @tc.desc Test Test HttpRequestMethod property.
     */

    it("Telephony_http_HttpRequestMethod_PUT_0200", 0, async function (done) {
      var casename = "Telephony_http_HttpRequestMethod_0500"
      console.log("-----------------------HttpRequestMethod_PUT_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: "PUT",
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
          console.log("-----------------------HttpRequestMethod_PUT_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestMethod_PUT_0200 : error = " + error);
        done();
      }
    });






  })

};

