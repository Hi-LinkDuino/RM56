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

describe('fileio_read', function () {

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0000
   * @tc.name fileio_test_read_sync_000
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(4096));
      expect(len == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0010
   * @tc.name fileio_test_read_sync_001
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_001', 0, async function () {
    let bufLen = 5;
    expect(FILE_CONTENT.length > bufLen).assertTrue();
    let fpath = await nextFileName('fileio_test_read_sync_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(bufLen), {
        offset: 1,
      });
      expect(len == bufLen - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0020
   * @tc.name fileio_test_read_sync_002
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        length: 1,
      });
      expect(len == 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0030
   * @tc.name fileio_test_read_sync_003
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: 1,
      });
      expect(len == FILE_CONTENT.length - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0040
   * @tc.name fileio_test_read_sync_004
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_004', 0, async function () {
    let fd;
    const invalidOffset = 99999;
    let fpath = await nextFileName('fileio_test_read_sync_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        offset: invalidOffset,
      });
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0050
   * @tc.name fileio_test_read_sync_005
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_005', 0, async function () {
    let fd;
    const invalidLength = 9999;
    let fpath = await nextFileName('fileio_test_read_sync_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      fileio.readSync(fd, new ArrayBuffer(4096), {
        length: invalidLength,
      });
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });
  
  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0060
   * @tc.name fileio_test_read_sync_006
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_006');
    let fd;

    try {
      fileio.readSync(-1, new ArrayBuffer(4096));
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_read_sync_006 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0070
   * @tc.name fileio_test_read_sync_007
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_007', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: 1,
      });
      expect(len == FILE_CONTENT.length - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_SYNC_0080
   * @tc.name fileio_test_read_sync_008
   * @tc.desc Test readSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_sync_008', 0, async function () {
    let fpath = await nextFileName('fileio_test_read_sync_008');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let invalidPos = FILE_CONTENT.length + 1;
      let len = fileio.readSync(fd, new ArrayBuffer(4096), {
        position: invalidPos,
      });
      expect(len == 0).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_read_sync_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0000
   * @tc.name fileio_test_read_async_000
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let res = await fileio.read(fd, new ArrayBuffer(4096));
      expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0010
   * @tc.name fileio_test_read_async_001
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      fileio.read(fd, new ArrayBuffer(4096), async function (err, data) {
        expect(data.bytesRead == FILE_CONTENT.length).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_read_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0020
   * @tc.name fileio_test_read_async_002
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      let readout = await fileio.read(fd, new ArrayBuffer(4096));
      expect(readout.buffer.byteLength == 4096).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0030
   * @tc.name fileio_test_read_async_003
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      let readout = await fileio.read(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(readout.bytesRead == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0040
   * @tc.name fileio_test_read_async_004
   * @tc.desc Test readAsync() interface.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      let readout = await fileio.read(fd, new ArrayBuffer(4096), {
        offset: 1,
        length: 3,
      });
      expect(readout.bytesRead == 3).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0050
   * @tc.name fileio_test_read_async_005
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_005', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      let readout = await fileio.read(fd, new ArrayBuffer(5), {
        offset: 1,
      });
      expect(readout.bytesRead == 4).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0060
   * @tc.name fileio_test_read_async_006
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_006', 0, async function (done) {
    let fd;
    const invalidOffset = 999;
    let fpath = await nextFileName('fileio_test_read_async_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      await fileio.read(fd, new ArrayBuffer(4096), {
        offset: invalidOffset,
      });
      expect(null).assertFail();
      done();
    } catch (e) {
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0070
   * @tc.name fileio_test_read_async_007
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_007', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    let fd;

    try {
      fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let invalidPos = FILE_CONTENT.length + 1;
      let err = await fileio.read(fd, new ArrayBuffer(4096), {
        offset: 1,
        position: invalidPos,
      });
      expect(!!err).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_0080
   * @tc.name fileio_test_read_async_008
   * @tc.desc Test readAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_008', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_008');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let err = await fileio.read(-1, new ArrayBuffer(4096));
      expect(!!err).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(null).assertFail();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_008 has failed for ' + e);
      expect(!!e).assertTrue();
      done();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_READ_ASYNC_READOUT_OFFSET_0000
   * @tc.name fileio_test_read_async_readout_offset_000
   * @tc.desc Test readAsync() ReadOut.offset interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_read_async_readout_offset_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_read_async_readout_offset_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      expect(isIntNum(fd)).assertTrue();
      let readout = await fileio.read(fd, new ArrayBuffer(4096), {
        offset: 1,
      });
      expect(readout.offset == 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_read_async_readout_offset_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
