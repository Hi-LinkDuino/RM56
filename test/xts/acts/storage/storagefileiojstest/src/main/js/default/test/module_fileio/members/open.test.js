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

describe('fileio_open', function () {

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_SYNC_0000
   * @tc.name fileio_test_open_sync_000
   * @tc.desc Test openSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_sync_000', 0, function () {
    try {
      fileio.openSync('/', 0o102, 0o666);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_open_sync_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0000
   * @tc.name fileio_test_open_async_000
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.access(fpath).then(function (err) {
        fileio.open(fpath, 0, 0o0400, function (err, fd) {
          fileio.read(fd, new ArrayBuffer(4096))
            .then(function (res) {
              expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
              expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
              fileio.closeSync(fd);
              fileio.unlinkSync(fpath);
              done();
            });
        });
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0010
   * @tc.name fileio_test_open_async_001
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.open(fpath, 0, 0o0400, function (err, fd) {
        fileio.read(fd, new ArrayBuffer(4096))
          .then(function (res) {
            expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
            expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
            fileio.closeSync(fd);
            fileio.unlinkSync(fpath);
            done();
          });
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0020
   * @tc.name fileio_test_open_async_002
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_002', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.open(fpath, 0, function (err, fd) {
        fileio.read(fd, new ArrayBuffer(4096))
          .then(function (res) {
            expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
            expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
            fileio.closeSync(fd);
            fileio.unlinkSync(fpath);
            done();
          });
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0030
   * @tc.name fileio_test_open_async_003
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_003', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_003');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      fileio.open(fpath, 0o2, function (err, fd) {
        fileio.read(fd, new ArrayBuffer(4096))
          .then(function (res) {
            expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
            expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
            fileio.closeSync(fd);
            fileio.unlinkSync(fpath);
            done();
          });
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0040
   * @tc.name fileio_test_open_async_004
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_004', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_004');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      await fileio.open(fpath)
        .then(function (fd) {
          fileio.read(fd, new ArrayBuffer(4096))
            .then(function (res) {
              expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
              expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
              fileio.closeSync(fd);
              fileio.unlinkSync(fpath);
            });
        })
        .catch(function (err) {
          expect(err == null).assertTrue();
        })
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0050
   * @tc.name fileio_test_open_async_005
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_005', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_005');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      await fileio.open(fpath, 0o0)
        .then(function (fd) {
          fileio.read(fd, new ArrayBuffer(4096))
            .then(function (res) {
              expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
              expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
              fileio.closeSync(fd);
              fileio.unlinkSync(fpath);
            })
        })
        .catch(function (err) {
          expect(err == null).assertTrue();
        })
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0060
   * @tc.name fileio_test_open_async_006
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_006', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_006');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      await fileio.open(fpath, 0o2, 0o0200)
        .then(function (fd) {
          fileio.read(fd, new ArrayBuffer(4096))
            .then(function (res) {
              expect((String.fromCharCode.apply(null, new Uint8Array(res.buffer))) == FILE_CONTENT).assertTrue;
              expect(res.bytesRead == FILE_CONTENT.length).assertTrue();
              fileio.closeSync(fd);
              fileio.unlinkSync(fpath);
            });
        })
        .catch(function (err) {
          expect(err == null).assertTrue();
        })
      done();
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0070
   * @tc.name fileio_test_open_async_007
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_007', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_007');
    try {
      fileio.open(fpath, 0o0101, 0o0222, function (err, fd) {
        expect(fd !== null).assertTrue();
        fileio.writeSync(fd, FILE_CONTENT, {
          encoding: 'utf-8',
          offset: 1,
          length: 1,
        });
        expect(fileio.closeSync(fd) !== null).assertTrue();
        expect(fileio.unlinkSync(fpath) !== null).assertTrue();
        done();
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0080
   * @tc.name fileio_test_open_async_008
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_008', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_008');
    try {
      fileio.open(fpath, 0o100, 0o0444, function (err, fd) {
        expect(fd !== null).assertTrue();
        expect(fileio.closeSync(fd) !== null).assertTrue();
        expect(fileio.unlinkSync(fpath) !== null).assertTrue();
        done();
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_OPEN_ASYNC_0090
   * @tc.name fileio_test_open_async_009
   * @tc.desc Test openASync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Functoin
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_open_async_009', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_open_async_009');
    try {
      fileio.open(fpath, 0o2101, 0o0222, function (err, fd) {
        expect(fd !== null).assertTrue();
        let wri = fileio.writeSync(fd, 'hello1', {
          encoding: 'utf-8',
          offset: 1,
          length: 1,
        });
        expect(wri !== null).assertTrue();
        let writ = fileio.writeSync(fd, 'hello2', {
          encoding: 'utf-8',
          offset: 1,
          length: 1,
        });
        expect(writ !== null).assertTrue();
        expect(fileio.closeSync(fd) !== null).assertTrue();
        expect(fileio.unlinkSync(fpath) !== null).assertTrue();
        done();
      });
    } catch (e) {
      expect(null).assertFail();
    }
  });
});
