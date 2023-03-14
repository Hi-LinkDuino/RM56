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

describe('fileio_stream', function () {

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_FLUSHSYNC_0000
   * @tc.name fileio_test_stream_flush_sync_000
   * @tc.desc Test flushSync() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_flush_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_flush_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(ss.flushSync() == null).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_flush_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_FLUSHASYNC_0000
   * @tc.name fileio_test_stream_flush_async_000
   * @tc.desc Test flushSync() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_flush_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_flush_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = await fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      ss.flush().then(
        function (err) {
          expect(!err).assertTrue();
          expect(ss.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
          expect(ss.closeSync() == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
        })
      done();
    } catch (e) {
      console.log('fileio_test_stream_flush_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_FLUSHASYNC_0010
   * @tc.name fileio_test_stream_flush_async_001
   * @tc.desc Test flushSync() interface
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_flush_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_stream_flush_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = await fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      ss.flush(function (err) {
        expect(ss.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      })
    } catch (e) {
      console.log('fileio_test_stream_flush_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
