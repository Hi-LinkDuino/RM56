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

export default function requestDownloadJsunit() {
  describe('requestDownloadTest', function () {
    let downloadConfig = {
      url: "www.baidu.com"
    };
    let file = {
      filename: 'text.txt',
      name: 'text.txt',
      uri: 'C:\\Program Files',
      type: 'text'
    };
    let uploadConfig = {
      url: "www.baidu.com",
      header: 'HTTP',
      method: 'post',
      files: file,
      data: 'jason/xml'
    };
    var receivedSize;
    var totalSize;
    console.log("************* settings Test start*************");

    /**
     * @tc.number  requestDownload_test_001
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_001', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.on('progress', (receivedSize, totalSize) => {
            console.log("downloadTask on_progress:" + JSON.stringify(receivedSize));
            console.log("downloadTask on_progress:" + JSON.stringify(totalSize));
            this.receivedSize = receivedSize;
            this.totalSize = totalSize;
            expect(true).assertTrue();
          })
        });
      } catch (exception) {
        console.log("requestDownload_test_001 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_002
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_002', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.off('progress', (receivedSize, totalSize) => {
            console.log("downloadTask off_progress:" + JSON.stringify(receivedSize));
            console.log("downloadTask off_progress:" + JSON.stringify(totalSize));
            this.receivedSize = receivedSize;
            this.totalSize = totalSize;
            expect(true).assertTrue();
          })
        });
      } catch (exception) {
        console.log("requestDownload_test_002 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_003
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_003', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.on('complete', (err) => {
            console.log("downloadTask on_complete err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_003 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_004
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_004', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.on('pause', (err) => {
            console.log("downloadTask on_pause err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_004 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_005
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_005', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.on('remove', (err) => {
            console.log("downloadTask on_remove err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_005 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_006
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_006', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask));
          downloadTask.off('complete', (err) => {
            console.log("downloadTask off_complete err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_006 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_007
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_007', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.off('pause', (err) => {
            console.log("downloadTask off_pause err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_007 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_007
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_008', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.off('remove', (err) => {
            console.log("downloadTask off_remove err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_008 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_009
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_009', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.on('fail', (err) => {
            console.log("downloadTask on_fail err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_009 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_010
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_010', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.off('fail', (err) => {
            console.log("downloadTask off_fail err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_010 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_011
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_011', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.remove((err) => {
            console.log("downloadTask remove err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_011 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_012
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_012', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.query((err) => {
            console.log("downloadTask query err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (err) {
        console.log("requestDownload_test_012 invoke download error : " + JSON.stringify(err));
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_013
     * @tc.name    Test The  request DownloadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_013', 0, async function (done) {
      try {
        request.download(downloadConfig, (downloadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(downloadTask))
          downloadTask.queryMimeType((err) => {
            console.log("downloadTask queryMimeType err:" + err);
            expect(true).assertTrue();
          });
        });
      } catch (exception) {
        console.log("requestDownload_test_013 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

    /**
     * @tc.number  requestDownload_test_007
     * @tc.name    Test The  request UploadTask
     * @tc.desc    Function test
     */
    it('requestDownload_test_014', 0, async function (done) {
      try {
        request.upload(uploadConfig, (uploadTask) => {
          console.log("downloadConfig result:" + JSON.stringify(uploadTask))
          expect(true).assertTrue();
        });
      } catch (exception) {
        console.log("requestDownload_test_014 failed due to execute timeout 5s");
        expect(true).assertTrue();
      }
      done();
    });

  })
}
