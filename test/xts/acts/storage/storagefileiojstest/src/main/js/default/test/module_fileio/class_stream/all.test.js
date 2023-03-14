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
   * @tc.number SUB_DF_FILEIO_STREAM_CREATESTREAMSYNC_0000
   * @tc.name fileio_test_stream_create_stream_sync_000
   * @tc.desc Test createStreamSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_create_stream_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATESTREAMSYNC_0010
   * @tc.name fileio_test_stream_create_stream_sync_001
   * @tc.desc Test createStreamSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_001');

    try {
      fileio.createStreamSync(fpath, 'r+');
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_stream_create_stream_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_CREATESTREAMSYNC_0020
   * @tc.name fileio_test_stream_create_stream_sync_002
   * @tc.desc Test createStreamSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_create_stream_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_create_stream_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.createStreamSync(fpath, 'ohos');
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0000
   * @tc.name fileio_test_stream_read_sync_000
   * @tc.desc Test readSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      let len = ss.readSync(new ArrayBuffer(4096));
      expect(len == FILE_CONTENT.length).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_read_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0010
   * @tc.name fileio_test_stream_read_sync_001
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_001', 0, async function () {
    let bufLen = 5;
    expect(FILE_CONTENT.length > bufLen).assertTrue();
    let fpath = await nextFileName('fileio_test_stream_read_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      let len = ss.readSync(new ArrayBuffer(bufLen), {
        offset: 1
      });
      expect(len == (bufLen - 1)).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_read_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0020
   * @tc.name fileio_test_stream_read_sync_002
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      let len = ss.readSync(new ArrayBuffer(4096), {
        length: 1
      });
      expect(len == 1).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_read_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0030
   * @tc.name fileio_test_stream_read_sync_003
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      let len = ss.readSync(new ArrayBuffer(4096), {
        position: 1
      });
      expect(len == (FILE_CONTENT.length - 1)).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_read_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0040
   * @tc.name fileio_test_stream_read_sync_004
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_004', 0, async function () {
    let ss;
    const invalidOffset = 99999;
    let fpath = await nextFileName('fileio_test_stream_read_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      ss = fileio.createStreamSync(fpath, 'r+');
      ss.readSync(new ArrayBuffer(4096), {
        offset: invalidOffset
      });
      expect(null).assertFail();
    } catch (e) {
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0050
   * @tc.name fileio_test_stream_read_sync_005
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_005', 0, async function () {
    let ss;
    const invalidLength = 9999;
    let fpath = await nextFileName('fileio_test_stream_read_sync_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      ss = fileio.createStreamSync(fpath, 'r+');
      ss.readSync(new ArrayBuffer(4096), {
        length: invalidLength
      });
      expect(null).assertFail();
    } catch (e) {
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_READSYNC_0060
   * @tc.name fileio_test_stream_read_sync_006
   * @tc.desc Test the readSync method of class Stream.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_read_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_read_sync_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      let invalidPos = FILE_CONTENT.length + 1;
      let len = ss.readSync(new ArrayBuffer(4096), {
        position: invalidPos
      });
      expect(len == 0).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_read_sync_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STREAM_WRITESYNC_0000
   * @tc.name fileio_test_stream_write_sync_000
   * @tc.desc Test writeSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stream_write_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_stream_write_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let ss = fileio.createStreamSync(fpath, 'r+');
      expect(ss !== null).assertTrue();
      expect(ss.writeSync(FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(ss.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_stream_write_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});