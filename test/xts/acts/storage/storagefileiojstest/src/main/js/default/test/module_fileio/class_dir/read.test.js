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
  fileio, FILE_CONTENT, prepareFile, nextFileName, isIntNum, isString, isBoolean,
  describe, it, expect,
} from '../../Common';

describe('fileio_dir_read', function () {

  /**
   * @tc.number SUB_DF_FileIO_ReadSync_0000
   * @tc.name fileio_test_dir_read_file_sync_000
   * @tc.desc Test Dir.readSync() interface.
   * @tc.size MEDIUM(中型)
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_dir_read_file_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_test_dir_read_file_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let stat = fileio.statSync(fpath);
      expect(isIntNum(stat.size)).assertTrue();
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let len = fileio.readSync(fd, new ArrayBuffer(stat.size));
      expect(len == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_dir_read_file_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_open_close_ASync_0000
   * @tc.name fileio_test_dir_open_close_async_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_test_dir_open_close_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_open_close_async_000') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_test_dir_open_close_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_open_close_ASync_0010
   * @tc.name fileio_test_dir_open_close_async_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_test_dir_open_close_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_open_close_async_001') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      fileio.opendir(dpath, function (error, dd) {
        expect(dd !== null).assertTrue();
        expect(dd.closeSync() == null).assertTrue();
        expect(fileio.rmdirSync(dpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_dir_open_close_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_open_close_ASync_0000
   * @tc.name fileio_test_dir_open_close_async_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_test_dir_read_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_read_async_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(await dd.read() != null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    }
    catch (e) {
      console.log('fileio_test_dir_read_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_open_close_ASync_0010
   * @tc.name fileio_test_dir_open_close_async_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_test_dir_read_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_test_dir_read_async_001') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      dd.read(function (err, dirent) {
        expect(dd.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        expect(fileio.rmdirSync(dpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_test_dir_read_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_0000
   * @tc.name fileio_dir_read_sync_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_000', 0, async function () {
    let dpath = await nextFileName('fileio_dir_read_sync_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync() != null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_dir_read_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_0010
   * @tc.name fileio_dir_read_sync_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_001', 0, async function () {
    let dpath = await nextFileName('fileio_dir_read_sync_001') + 'd';
    let fpath = dpath + '/f1';
    let dd;

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(dd.readSync(-1) != null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_Name_0000
   * @tc.name fileio_dir_read_sync_name_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_name_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_name_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isString(dir.name)).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_name_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_Name_0010
   * @tc.name fileio_dir_read_sync_name_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_name_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_name_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(fpath.indexOf(dir.name) > -1).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_name_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isBlockDevice_0000
   * @tc.name fileio_dir_read_sync_isBlockDevice_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isBlockDevice_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isBlockDevice_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isBlockDevice())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isBlockDevice_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isBlockDevice_0010
   * @tc.name fileio_dir_read_sync_isBlockDevice_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isBlockDevice_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isBlockDevice_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isBlockDevice() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isBlockDevice_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isCharacterDevice_0000
   * @tc.name fileio_dir_read_sync_isCharacterDevice_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isCharacterDevice_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isCharacterDevice_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isCharacterDevice())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isCharacterDevice_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isCharacterDevice_0010
   * @tc.name fileio_dir_read_sync_isCharacterDevice_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isCharacterDevice_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isCharacterDevice_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isCharacterDevice() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isCharacterDevice_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isDirectory_0000
   * @tc.name fileio_dir_read_sync_isDirectory_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isDirectory_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isDirectory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isDirectory())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isDirectory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isDirectory_0010
   * @tc.name fileio_dir_read_sync_isDirectory_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isDirectory_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isDirectory_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isDirectory() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isDirectory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isFIFO_0000
   * @tc.name fileio_dir_read_sync_isFIFO_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isFIFO_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isFIFO_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isFIFO())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isFIFO_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isFIFO_0010
   * @tc.name fileio_dir_read_sync_isFIFO_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isFIFO_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isFIFO_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isFIFO() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isFIFO_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isFile_0000
   * @tc.name fileio_dir_read_sync_isFile_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isFile_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isFile_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isFile())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isFile_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isFile_0010
   * @tc.name fileio_dir_read_sync_isFile_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isFile_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isFile_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isFile()).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isFile_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isSocket_0000
   * @tc.name fileio_dir_read_sync_isSocket_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isSocket_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isSocket_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isSocket())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isSocket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isSocket_0010
   * @tc.name fileio_dir_read_sync_isSocket_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isSocket_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isSocket_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isSocket() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isSocket_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isSymbolicLink_0000
   * @tc.name fileio_dir_read_sync_isSymbolicLink_000
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isSymbolicLink_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isSymbolicLink_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isSymbolicLink())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isSymbolicLink_000 has failed for ' + e);
      expect(null);
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadSync_isSymbolicLink_0010
   * @tc.name fileio_dir_read_sync_isSymbolicLink_001
   * @tc.desc Test Dir.readSync() interface.
   */
  it('fileio_dir_read_sync_isSymbolicLink_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_sync_isSymbolicLink_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = dd.readSync();
      expect(dir != null).assertTrue();
      expect(dir.isSymbolicLink() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_sync_isSymbolicLink_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_0000
   * @tc.name fileio_dir_read_async_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_000') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      expect(await dd.read() != null).assertTrue();
      expect(dd.closeSync() == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_0010
   * @tc.name fileio_dir_read_async_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_001') + 'd';
    let fpath = dpath + '/f1';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = fileio.opendirSync(dpath);
      expect(dd !== null).assertTrue();
      dd.read(function (err, dirent) {
        expect(dd.closeSync() == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        expect(fileio.rmdirSync(dpath) == null).assertTrue();
        done();
      })
    } catch (e) {
      console.log('fileio_dir_read_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_Name_0000
   * @tc.name fileio_dir_read_async_name_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_name_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_name_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isString(dir.name)).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_name_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_Name_0010
   * @tc.name fileio_dir_read_async_name_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_name_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_name_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(fpath.indexOf(dir.name) > -1).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_name_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsBlockDevice_0000
   * @tc.name fileio_dir_read_async_isBlockDevice_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isBlockDevice_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isBlockDevice_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isBlockDevice())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isBlockDevice_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsBlockDevice_0010
   * @tc.name fileio_dir_read_async_isBlockDevice_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isBlockDevice_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isBlockDevice_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isBlockDevice() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isBlockDevice_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsCharacterDevice_0000
   * @tc.name fileio_dir_read_async_isCharacterDevice_000
   * @tc.desc Test Dir.readAsync() interface.
   */

  it('fileio_dir_read_async_isCharacterDevice_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isCharacterDevice_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isCharacterDevice())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isCharacterDevice_000 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsCharacterDevice_0010
   * @tc.name fileio_dir_read_async_isCharacterDevice_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isCharacterDevice_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isCharacterDevice_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isCharacterDevice() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isCharacterDevice_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsDirectory_0000
   * @tc.name fileio_dir_read_async_isDirectory_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isDirectory())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsDirectory_0010
   * @tc.name fileio_dir_read_async_isDirectory_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isDirectory() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsFIFO_0000
   * @tc.name fileio_dir_read_async_isFIFO_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isFIFO_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isFIFO_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isFIFO())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isFIFO_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsFIFO_0010
   * @tc.name fileio_dir_read_async_isFIFO_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isFIFO_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isFIFO_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isFIFO() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isFIFO_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsFile_0000
   * @tc.name fileio_dir_read_async_isDirectory_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isFile())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsFile_0010
   * @tc.name fileio_dir_read_async_isDirectory_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isFile()).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsSocket_0000
   * @tc.name fileio_dir_read_async_isDirectory_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isSocket())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsSocket_0010
   * @tc.name fileio_dir_read_async_isDirectory_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isSocket() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsSymbolicLink_0000
   * @tc.name fileio_dir_read_async_isDirectory_000
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_000', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_000') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(isBoolean(dir.isSymbolicLink())).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FileIO_Dir_ReadAsync_IsSymbolicLink_0010
   * @tc.name fileio_dir_read_async_isDirectory_001
   * @tc.desc Test Dir.readAsync() interface.
   */
  it('fileio_dir_read_async_isDirectory_001', 0, async function (done) {
    let dpath = await nextFileName('fileio_dir_read_async_isDirectory_001') + 'd';
    let fpath = dpath + '/f1';
    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
      let dd = await fileio.opendir(dpath);
      expect(dd !== null).assertTrue();
      let dir = await dd.read();
      expect(dir != null).assertTrue();
      expect(dir.isSymbolicLink() === false).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_dir_read_async_isDirectory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });
})