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
  fileio, FILE_CONTENT, prepareFile, nextFileName, isIntNum,
  describe, it, expect,
} from '../../Common';

describe('fileio_FdOpenStream', async function () {

  /**
   * @tc.number SUB_DF_FILEIO_FDOPENSTREAM_SYNC_0000
   * @tc.name fileio_test_fdopenstream_sync_000
   * @tc.desc Test fdopenStreamSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fdopenstream_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_fdopenstream_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      let mode = 'r+';
      let fp = fileio.fdopenStreamSync(fd, mode);
      let buf = new ArrayBuffer(4096);
      let data = fp.readSync(buf, {});
      expect(data == FILE_CONTENT.length).assertTrue();
      expect(fp !== null).assertTrue();
      expect(fp.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_FDOPENSTREAM_SYNC_0010
   * @tc.name fileio_test_fdopenstream_sync_001
   * @tc.desc Test fdopenStreamSync() interface.
   * This interface shall throw an exception when the fpath is not given.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fdopenstream_sync_001', 0, function () {
    try {
      let fd = -1;
      let mode = 'r+';
      let fp = fileio.fdopenStreamSync(fd, mode);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_fdopenstream_sync_001 has failed for ' + e);
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_FDOPENSTREAM_ASYNC_000
   * @tc.name fileio_test_fdopenstream_async_000
   * @tc.desc Test fdopenStream() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fdopenstream_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fdopenstream_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let mode = 'r+';
      let fp = await fileio.fdopenStream(fd, mode);
      fp.read(new ArrayBuffer(4096), {}, function (err, data) {
        if (!err) {
          expect(fp !== null).assertTrue();
          expect(fp.closeSync() == null).assertTrue();
          expect(data.bytesRead == FILE_CONTENT.length).assertTrue();
        } else {
          console.log('Cannot read from the stream ' + err);
          expect(null).assertFail();
        }

      })
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_fdopenstream_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  })

  /**
   * @tc.number SUB_DF_FILEIO_FDOPENSTREAM_ASYNC_001
   * @tc.name fileio_test_fdopenstream_async_001
   * @tc.desc Test fdopenStream() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fdopenstream_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_fdopenstream_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let mode = 'r+';
      fileio.fdopenStream(fd, mode, function (error, fp) {
        expect(fp !== null).assertTrue();
        fp.read(new ArrayBuffer(4096), {}, function (err, data) {
          if (!err) {
            expect(fp !== null).assertTrue();
            expect(fp.closeSync() == null).assertTrue();
            expect(data.bytesRead == FILE_CONTENT.length).assertTrue();
          } else {
            console.log('Cannot read from the stream ' + err);
            expect(null).assertFail();
          }
        })
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      })
    } catch (e) {
      console.log('fileio_test_fdopenstream_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  })
})