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


export default function HttpRequestOptionsJsunit() {
  describe("HttpRequestOptionsJsunitTest", function () {


    /**
     * @tc.number Telephony_Http_HttpRequestOptions_method_0100
     * @tc.name HttpRequestOptions::method
     * @tc.desc Test Test HttpRequestOptions property.
     */
    it("Telephony_http_HttpRequestOptions_method_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0100';
      console.log("-----------------------HttpRequestOptions_method Test is starting-----------------------");
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
          console.log("-----------------------HttpRequestOptions_method Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_method_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_http_HttpRequestOptions_method_0200
     * @tc.name HttpRequestOptions::method
     * @tc.desc Test Test HttpRequestOptions property.
     */
    it("Telephony_http_HttpRequestOptions_method_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0100';
      console.log("-----------------------HttpRequestOptions_method_01 Test is starting-----------------------");
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
          console.log("-----------------------HttpRequestOptions_method_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_http_HttpRequestOptions_method_0200 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpRequestOptions_extraData_0100
     * @tc.name HttpRequestOptions::extraData
     * @tc.desc Test Test HttpRequestOptions property.
     */
    it("Telephony_Http_HttpRequestOptions_extraData_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0200';
      console.log("-----------------------HttpRequestOptions_extraData Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
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
          console.log("-----------------------HttpRequestOptions_extraData Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_extraData_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_Http_HttpRequestOptions_extraData_0200
     * @tc.name HttpRequestOptions::extraData
     * @tc.desc Test Test HttpRequestOptions property.
     */
    it("Telephony_Http_HttpRequestOptions_extraData_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0200';
      console.log("-----------------------HttpRequestOptions_extraData_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
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
          console.log("-----------------------HttpRequestOptions_extraData_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_extraData_0200 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpRequestOptions_header_0100
     * @tc.name HttpRequestOptions::header
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_header_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0300';
      console.log("-----------------------HttpRequestOptions_header Test is starting-----------------------");
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
          try {
            var entity = JSON.parse(data.getResult());
            expect(true).assertTrue();
            console.log("-----------------------HttpRequestOptions_header Test end-----------------------");
            done();
          } catch (error) {
            console.log("Telephony_Http_HttpRequestOptions_header_0100 : JSON parse error = " + error);
            expect().assertFail();
            done();
          }
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_header_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_HttpRequestOptions_header_0200
     * @tc.name HttpRequestOptions::header
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_header_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0300';
      console.log("-----------------------HttpRequestOptions_header_01 Test is starting-----------------------");
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
          try {
            var entity = JSON.parse(data.getResult());
            expect(true).assertTrue();
            console.log("-----------------------HttpRequestOptions_header_01 Test end-----------------------");
            done();
          } catch (error) {
            console.log("Telephony_Http_HttpRequestOptions_header_0200 : JSON parse error = " + error);
            expect().assertFail();
            done();
          }

        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_header_0200 : error = " + error);
        done();
      }
    });




    /**
     * @tc.number Telephony_Http_HttpRequestOptions_header_0300
     * @tc.name HttpRequestOptions::header
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_header_0300", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0300';
      console.log("-----------------------HttpRequestOptions_header_02 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/xml",
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
          try {
            var result = '<?xml version="1.0" encoding="utf-8"?>';
            expect(data.getResult().contains(result)).assertTrue();
            console.log("-----------------------HttpRequestOptions_header_02 Test end-----------------------");
            done();
          } catch (error) {
            console.log("Telephony_Http_HttpRequestOptions_header_0300 : JSON parse error = " + error);
            expect().assertFail();
            done();
          }
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_header_0300 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_HttpRequestOptions_header_0400
     * @tc.name HttpRequestOptions::header
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_header_0400", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0300';
      console.log("-----------------------HttpRequestOptions_header_03 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/xml",
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
          try {
            var result = '<?xml version="1.0" encoding="utf-8"?>';
            expect(data.getResult().contains(result)).assertTrue();
            console.log("-----------------------HttpRequestOptions_header_03 Test end-----------------------");
            done();
          } catch (error) {
            console.log("Telephony_Http_HttpRequestOptions_header_0400 : JSON parse error = " + error);
            expect().assertFail();
            done();
          }

        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_header_0400 : error = " + error);
        done();
      }
    });





    /**
     * @tc.number Telephony_Http_HttpRequestOptions_readTimeout_0100
     * @tc.name HttpRequestOptions::readTimeout
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_readTimeout_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0400';
      console.log("-----------------------HttpRequestOptions_readTimeout Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 0.1,
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
          expect(data.getResponseCode == 408).assertTrue();
          console.log("-----------------------HttpRequestOptions_readTimeout Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_readTimeout_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_HttpRequestOptions_readTimeout_0200
     * @tc.name HttpRequestOptions::readTimeout
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_readTimeout_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0400';
      console.log("-----------------------HttpRequestOptions_readTimeout_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 0.1,
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
          expect(data.getResponseCode == 408).assertTrue();
          console.log("-----------------------HttpRequestOptions_readTimeout_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_readTimeout_0200 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpRequestOptions_connectTimeout_0100
     * @tc.name HttpRequestOptions::connectTimeout
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_connectTimeout_0100", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0500';
      console.log("-----------------------HttpRequestOptions_connectTimeout Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 0.1
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions, (err, data) => {
          console.log(casename + JSON.stringify(err) + " data: " + JSON.stringify(data));
          expect(data.getResponseCode == 504).assertTrue();
          console.log("-----------------------HttpRequestOptions_connectTimeout Test end-----------------------");
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_connectTimeout_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_HttpRequestOptions_connectTimeout_0200
     * @tc.name HttpRequestOptions::connectTimeout
     * @tc.desc Test Test HttpRequestOptions property.
     */

    it("Telephony_Http_HttpRequestOptions_connectTimeout_0200", 0, async function (done) {
      var casename = 'Telephony_Http_HttpRequestOptions_0500';
      console.log("-----------------------HttpRequestOptions_connectTimeout_01 Test is starting-----------------------");
      try {
        let httpRequestOptions = {
          method: 'GET',
          extraData: null,
          header: "content-type': 'application/json",
          readTimeout: 60,
          connectTimeout: 0.1
        }
        var httpRequest = http.createHttp();
        if (utils.notCheck) {
          expect(true).assertTrue();
          done();
          return;
        }
        httpRequest.request("https://www.baidu.com/", httpRequestOptions).then(data => {
          console.log(casename + JSON.stringify(data));
          expect(data.getResponseCode == 504).assertTrue();
          console.log("-----------------------HttpRequestOptions_connectTimeout_01 Test end-----------------------");
          done();
        }).catch(error => {
          console.info(casename+"errocode" + JSON.stringify(error));
          done();
        });
      } catch (error) {
        console.log("Telephony_Http_HttpRequestOptions_connectTimeout_0200 : error = " + error);
        done();
      }
    });

  })

};

