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
  fileio, FILE_CONTENT, nextFileName, isIntNum,
  describe, it, expect,
} from '../../Common';

describe('fileio_write', function () {

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0000
   * @tc.name fileio_test_write_sync_000
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_000');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(fileio.writeSync(fd, FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0010
   * @tc.name fileio_test_write_sync_001
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_001', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_001');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          encoding: 'utf-8',
        })
        == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0020
   * @tc.name fileio_test_write_sync_002
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_002');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          offset: 1,
        })
        == FILE_CONTENT.length - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0030
   * @tc.name fileio_test_write_sync_003
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_003', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_003');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          length: FILE_CONTENT.length - 1,
        })
        == FILE_CONTENT.length - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0040
   * @tc.name fileio_test_write_sync_004
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_004', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_004');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          offset: 1,
          length: 1,
        })
        == 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0050
   * @tc.name fileio_test_write_sync_005
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_005', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_005');
    const invalidOffset = 999;
    let fd;

    try {
      fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          offset: invalidOffset,
        })
        == 1).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0060
   * @tc.name fileio_test_write_sync_006
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_006', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_006');
    const invalidLength = 999;
    let fd;

    try {
      fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(
        fileio.writeSync(fd, FILE_CONTENT, {
          length: invalidLength,
        })
        == 1).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0070
   * @tc.name fileio_test_write_sync_007
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_007', 0, function () {
    try {
      fileio.writeSync();
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_write_sync_007 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0080
   * @tc.name fileio_test_write_sync_008
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_008', 0, function () {
    try {
      fileio.writeSync(-1, FILE_CONTENT);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_write_sync_008 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_SYNC_0090
   * @tc.name fileio_test_write_sync_009
   * @tc.desc Test writeSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_sync_009', 0, async function () {
    let fpath = await nextFileName('fileio_test_write_sync_009');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      let writeNum = fileio.writeSync(fd, new ArrayBuffer(4096));
      expect(writeNum == 4096).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_write_sync_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0000
   * @tc.name fileio_test_write_async_000
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_write_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_write_async_000');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      expect(await fileio.write(fd, FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_write_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0010
   * @tc.name fileio_test_write_async_001
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_001');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      let writeNum = await fileio.write(fd, FILE_CONTENT, {
        offset: 1,
      });
      expect(writeNum == FILE_CONTENT.length - 1).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_write_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0020
   * @tc.name fileio_test_write_async_002
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_002');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      let writeNum = await fileio.write(fd, FILE_CONTENT, {
        offset: 1,
        length: 3,
      });
      expect(writeNum == 3).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_write_async_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0030
   * @tc.name fileio_test_write_async_003
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_003');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      let writeNum = await fileio.write(fd, FILE_CONTENT, {
        offset: 1,
        length: 3,
        position: 5,
      });
      expect(writeNum == 3).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_write_async_003 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0040
   * @tc.name fileio_test_write_async_004
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_004');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      let writeNum = await fileio.write(fd, FILE_CONTENT, {
        offset: 0,
        length: 4,
        position: 5,
        encoding: 'utf-8',
      });
      expect(writeNum == 4).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_write_async_004 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0050
   * @tc.name fileio_test_write_async_005
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_005', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_005');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      await fileio.write(fd, new ArrayBuffer(4096), function (
          error,
          bytesWritten
      ) {
        expect(bytesWritten == 4096).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
      });
      done();
    } catch (e) {
      console.log('fileio_test_write_async_005 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0060
   * @tc.name fileio_test_write_async_006
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_006', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_006');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      await fileio.write(
        fd,
        FILE_CONTENT,
        {
          offset: 1,
        }, function (error, bytesWritten) {
          expect(bytesWritten == FILE_CONTENT.length - 1).assertTrue();
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        }
      );
    } catch (e) {
      console.log('fileio_test_write_async_006 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0070
   * @tc.name fileio_test_write_async_007
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_007', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_007');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      await fileio.write(
        fd,
        FILE_CONTENT,
        {
          offset: 1,
          length: 5,
        }, function (error, bytesWritten) {
          expect(bytesWritten == 5).assertTrue();
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        }
      );
    } catch (e) {
      console.log('fileio_test_write_async_007 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0080
   * @tc.name fileio_test_write_async_008
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_008', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_008');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      await fileio.write(
        fd,
        FILE_CONTENT,
        {
          offset: 1,
          length: 5,
          position: 5,
        }, function (error, bytesWritten) {
          expect(bytesWritten == 5).assertTrue();
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        }
      );
    } catch (e) {
      console.log('fileio_test_write_async_008 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_WRITE_ASYNC_0090
   * @tc.name fileio_test_write_async_009
   * @tc.desc Test writeAsync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_write_async_009', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_write_async_009');

    try {
      let fd = fileio.openSync(fpath, 0o102, 0o666);
      expect(isIntNum(fd)).assertTrue();
      await fileio.write(
        fd,
        FILE_CONTENT,
        {
          offset: 1,
          length: 5,
          position: 5,
          encoding: 'utf-8',
        }, function (error, bytesWritten) {
          expect(bytesWritten == 5).assertTrue();
          expect(fileio.closeSync(fd) == null).assertTrue();
          expect(fileio.unlinkSync(fpath) == null).assertTrue();
          done();
        }
      );
    } catch (e) {
      console.log('fileio_test_write_async_009 has failed for ' + e);
      expect(null).assertFail();
    }
  });
});
