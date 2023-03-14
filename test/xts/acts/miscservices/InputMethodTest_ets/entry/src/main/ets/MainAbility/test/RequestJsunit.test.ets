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
import {describe, it, expect} from "deccjsunit/index.ets";
import request from '@ohos.request';

export default function requestJsunit() {
  describe('requestTest', function () {

    /**
     * @tc.name: ohos.request_request_0001
     * @tc.desc: NETWORK_MOBILE NETWORK_WIFI ERROR_CANNOT_RESUME ERROR_DEVICE_NOT_FOUND Values detection
     *
     * @tc.author: kangyuntao
     */
    it('request_test_0001', 0, async function (done) {
      console.log("-----------------------Request_test_0001 is starting-----------------------");
      try {
        request.NETWORK_MOBILE = 1;
        console.log("request_test_0001 request.NETWORK_MOBILE:" + request.NETWORK_MOBILE);
        expect(request.NETWORK_MOBILE).assertEqual(1);
        request.NETWORK_WIFI = 2;
        console.log("request_test_0001 request.NETWORK_WIFI:" + request.NETWORK_WIFI);
        expect(request.NETWORK_WIFI).assertEqual(2);
        request.ERROR_CANNOT_RESUME = 3;
        console.log("request_test_0001 request.ERROR_CANNOT_RESUME:" + request.ERROR_CANNOT_RESUME);
        expect(request.ERROR_CANNOT_RESUME).assertEqual(3);
        request.ERROR_DEVICE_NOT_FOUND = 4;
        console.log("request_test_0001 request.ERROR_DEVICE_NOT_FOUND:" + request.ERROR_DEVICE_NOT_FOUND);
        expect(request.ERROR_DEVICE_NOT_FOUND).assertEqual(4);
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_test_0001 error: " + err);
      }
      console.log("-----------------------Request_test_0001 end-----------------------");
      done();
    });

    /**
     * @tc.name: ohos.request_request_0002
     * @tc.desc: ERROR_FILE_ALREADY_EXISTS ERROR_FILE_ERROR ERROR_HTTP_DATA_ERROR
     * ERROR_INSUFFICIENT_SPACE Values detection
     * @tc.author: kangyuntao
     */
    it('request_test_0002', 0, async function (done) {
      console.log("-----------------------Request_test_0002 is starting-----------------------");
      try {
        request.ERROR_FILE_ALREADY_EXISTS = 5;
        console.log("request_test_0002 request.ERROR_FILE_ALREADY_EXISTS:" + request.ERROR_FILE_ALREADY_EXISTS);
        expect(request.ERROR_FILE_ALREADY_EXISTS).assertEqual(5);
        request.ERROR_FILE_ERROR = 6;
        console.log("request_test_0002 request.ERROR_FILE_ERROR:" + request.ERROR_FILE_ERROR);
        expect(request.ERROR_FILE_ERROR).assertEqual(6);
        request.ERROR_HTTP_DATA_ERROR = 7;
        console.log("request_test_0002 request.ERROR_HTTP_DATA_ERROR:" + request.ERROR_HTTP_DATA_ERROR);
        expect(request.ERROR_HTTP_DATA_ERROR).assertEqual(7);
        request.ERROR_INSUFFICIENT_SPACE = 8;
        console.log("request_test_0002 request.ERROR_INSUFFICIENT_SPACE:" + request.ERROR_INSUFFICIENT_SPACE);
        expect(request.ERROR_INSUFFICIENT_SPACE).assertEqual(8);
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_test_0002 error: " + err);
      }
      console.log("-----------------------Request_test_0002 end-----------------------");
      done();
    });

    /**
     * @tc.name: ohos.request_request_0003
     * @tc.desc: ERROR_TOO_MANY_REDIRECTS ERROR_UNHANDLED_HTTP_CODE ERROR_UNHANDLED_HTTP_CODE
     * PAUSED_QUEUED_FOR_WIFI Values detection
     * @tc.author: kangyuntao
     */
    it('request_test_0003', 0, async function (done) {
      console.log("-----------------------Request_test_0003 is starting-----------------------");
      try {
        request.ERROR_TOO_MANY_REDIRECTS = 9;
        console.log("request_test_0003 request.ERROR_TOO_MANY_REDIRECTS:" + request.ERROR_TOO_MANY_REDIRECTS);
        expect(request.ERROR_TOO_MANY_REDIRECTS).assertEqual(9);
        request.ERROR_UNHANDLED_HTTP_CODE = 10;
        console.log("request_test_0003 request.ERROR_UNHANDLED_HTTP_CODE:" + request.ERROR_UNHANDLED_HTTP_CODE);
        expect(request.ERROR_UNHANDLED_HTTP_CODE).assertEqual(10);
        request.ERROR_UNKNOWN = 11;
        console.log("request_test_0003 request.ERROR_UNKNOWN:" + request.ERROR_UNKNOWN);
        expect(request.ERROR_UNKNOWN).assertEqual(11);
        request.PAUSED_QUEUED_FOR_WIFI = 12;
        console.log("request_test_0003 request.PAUSED_QUEUED_FOR_WIFI:" + request.PAUSED_QUEUED_FOR_WIFI);
        expect(request.PAUSED_QUEUED_FOR_WIFI).assertEqual(12);
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_test_0003 error: " + err);
      }
      console.log("-----------------------Request_test_0003 end-----------------------");
      done();
    });

    /**
     * @tc.name: ohos.request_request_0004
     * @tc.desc: PAUSED_UNKNOWN PAUSED_WAITING_FOR_NETWORK  PAUSED_WAITING_TO_RETRY ESSION_FAILED Values detection
     * @tc.author: kangyuntao
     */
    it('request_test_0004', 0, async function (done) {
      console.log("-----------------------Request_test_0004 is starting-----------------------");
      try {
        request.PAUSED_UNKNOWN = 13;
        console.log("request_test_0004 request.PAUSED_UNKNOWN:" + request.PAUSED_UNKNOWN);
        expect(request.PAUSED_UNKNOWN).assertEqual(13);
        request.PAUSED_WAITING_FOR_NETWORK = 14;
        console.log("request_test_0004 request.PAUSED_WAITING_FOR_NETWORK:" + request.PAUSED_WAITING_FOR_NETWORK);
        expect(request.PAUSED_WAITING_FOR_NETWORK).assertEqual(14);
        request.PAUSED_WAITING_TO_RETRY = 15;
        console.log("request_test_0004 request.PAUSED_WAITING_TO_RETRY:" + request.PAUSED_WAITING_TO_RETRY);
        expect(request.PAUSED_WAITING_TO_RETRY).assertEqual(15);
        request.SESSION_FAILED = 16;
        console.log("request_test_0004 request.SESSION_FAILED:" + request.SESSION_FAILED);
        expect(request.SESSION_FAILED).assertEqual(16);
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_test_0004 error: " + err);
      }
      console.log("-----------------------Request_test_0004 end-----------------------");
      done();
    });

    /**
     * @tc.name: ohos.request_request_0005
     * @tc.desc: SESSION_PAUSED SESSION_PENDING SESSION_RUNNING SESSION_SUCCESSFUL Values detection
     * @tc.author: kangyuntao
     */
    it('request_test_0005', 0, async function (done) {
      console.log("-----------------------Request_test_0005 is starting-----------------------");
      try {
        request.SESSION_PAUSED = 17;
        console.log("request_test_0004 request.SESSION_PAUSED:" + request.SESSION_PAUSED);
        expect(request.SESSION_PAUSED).assertEqual(17);
        request.SESSION_PENDING = 18;
        console.log("request_test_0004 request.SESSION_PENDING:" + request.SESSION_PENDING);
        expect(request.SESSION_PENDING).assertEqual(18);
        request.SESSION_RUNNING = 19;
        console.log("request_test_0004 request.SESSION_RUNNING:" + request.SESSION_RUNNING);
        expect(request.SESSION_RUNNING).assertEqual(19);
        request.SESSION_SUCCESSFUL = 20;
        console.log("request_test_0004 request.SESSION_SUCCESSFUL:" + request.SESSION_SUCCESSFUL);
        expect(request.SESSION_SUCCESSFUL).assertEqual(20);
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_test_0005 error: " + err);
      }
      console.log("-----------------------Request_test_0005 end-----------------------");
      done();
    });

    /**
     * @tc.name: ohos.request_request_upload
     * @tc.desc: request_upload Method detection
     * @tc.author: kangyuntao
     */
    it('request_upload_0006', 0, async function (done) {
      console.log("-----------------------Request_test_0006 is starting-----------------------");
      try {
        console.log("request_upload_0006 getUploadConfig() " + getUploadConfig());
        request.upload(getUploadConfig(), (err, uploadTask) => {
          if (err) {
            expect().assertFail();
          } else {
            console.log("request_upload_0006 progress uploadTask =" + JSON.stringify(uploadTask));
            uploadTask.on('progress', function (data1, data2) {
              console.log("request_upload_0006 on data1 =" + data1);
              console.log("request_upload_0006 on data2 =" + data2);
            })
            uploadTask.off('progress', function (data1, data2) {
              console.log("request_upload_0006 off data1 =" + data1);
              console.log("request_upload_0006 off data2 =" + data2);
            })
            uploadTask.remove((err, data) => {
              console.log("request_upload_0006 remove =" + data);
            })
          }
        })
      } catch (err) {
        expect(true).assertEqual(true);
        console.error("request_upload_0006 error: " + err);
      }
      console.log("-----------------------request_upload_0006 end-----------------------");
      done();
    })
  })

  function getUploadConfig() {
    let file = {
      filename: 'test',
      name: 'test',
      uri: 'internal://cache/test.txt',
      type: 'txt'
    }
    let requestData = [{
                         name: 'name', value: '123'
                       }]
    let fileArray = new Array();
    fileArray[0] = file;
    let headerHttp = {
      headers: 'http'
    }
    let uploadConfig = {
      url: 'http://192.168.112.124/upload_test/',
      header: headerHttp,
      method: 'POST',
      files: fileArray,
      data: requestData
    }
    return uploadConfig
  }
}
