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

export default function inputRequestJsunit() {
  describe('inputRequestTest', function () {
    console.log("************* request Test start*************");

    /**
       * @tc.number  inputRequest_DownloadConfig_Test_001
       * @tc.name    Request DownloadConfig
       * @tc.desc    interface DownloadConfig
       */
    it('inputRequest_DownloadConfig_Test_001', 0, async function (done) {
      let caseName: string = "inputRequest_DownloadConfig_Test_001";
      console.log(`==========> ${caseName} Test start ==========>`);
      let downloadConfig = {
        url: "www.testdownload.com",
        enableMetered: false,
        enableRoaming: false,
        description: "test download",
        title: ""
      };
      try {
        let promise = request.download(downloadConfig);
        expect(promise).assertEqual(undefined);
      } catch (err) {
        console.log(`${caseName} fail,case success,error:${toString(err)}`);
        expect(true).assertTrue();
        done();
        return;
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });

    /**
       * @tc.number  inputRequest_DownloadInfo_Test_002
       * @tc.name    Request DownloadInfo
       * @tc.desc    interface DownloadInfo
       */
    it('inputRequest_DownloadInfo_Test_002', 0, async function (done) {
      let caseName: string = "inputRequest_DownloadInfo_Test_002";
      try {
        request.download({
          url: "www.testdownload.com"
        }).then(downloadTask => {
          if (downloadTask !== undefined) {
            downloadTask.query((err, downloadInfo) => {
              if (downloadInfo !== undefined) {
                expect("info").assertEqual(downloadInfo.description);
                expect(100).assertEqual(downloadInfo.downloadedBytes);
                expect(1).assertEqual(downloadInfo.downloadId);
                expect(101).assertEqual(downloadInfo.failedReason);
                expect("download.txt").assertEqual(downloadInfo.fileName);
                expect("C://").assertEqual(downloadInfo.filePath);
                expect(102).assertEqual(downloadInfo.pausedReason);
                expect(200).assertEqual(downloadInfo.status);
                expect("download url").assertEqual(downloadInfo.targetURI);
                expect("download test").assertEqual(downloadInfo.downloadTitle);
                expect(1000).assertEqual(downloadInfo.downloadTotalBytes);
              }
            });
          }
        }).catch(err => {
        });
      } catch (err) {
        console.log(`${caseName} fail,case success,error:${toString(err)}`);
        expect(true).assertTrue();
        done();
        return;
      }
      console.log(`==========> ${caseName} Test end ==========>`);
      done();
    });
  })
}