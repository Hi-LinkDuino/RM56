/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
  fileio, FILE_CONTENT, prepareFile, nextFileName,
  describe, it, expect,
} from '../../Common';

describe('fileio_createStream', function () {

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATE_STREAM_ASYNC_0000
   * @tc.name fileio_test_stream_create_stream_async_000
   * @tc.desc Test createStreamAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_create_stream_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = await fileio.createStream(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_stream_create_stream_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATE_STREAM_ASYNC_0010
   * @tc.name fileio_test_stream_create_stream_async_001
   * @tc.desc Test createStreamAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_create_stream_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.createStream(fpath, 'r+', function (err, stream) {
        expect(stream.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_stream_create_stream_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATE_STREAM_ASYNC_0020
   * @tc.name fileio_test_stream_create_stream_async_002
   * @tc.desc Test createStreamAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_create_stream_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.createStream(fpath, 'r+', function (err, stream) {
        expect(stream.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
        expect(stream.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_stream_create_stream_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATE_STREAM_ASYNC_0030
   * @tc.name fileio_test_stream_create_stream_async_003
   * @tc.desc Test createStreamAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_create_stream_async_003');

    try {
      fileio.createStream(fpath, 'r+', function (err) {
        done();
      });
    } catch (e) {
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATE_STREAM_ASYNC_0030
   * @tc.name fileio_test_stream_create_stream_async_003
   * @tc.desc Test createStreamSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_create_stream_async_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.createStream(fpath, 'ohos', function (err) {
        done();
      });
    } catch (e) {
      expect(!!e).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    }
  });
});
