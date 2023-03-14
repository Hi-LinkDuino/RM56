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
  fileio, FILE_CONTENT, prepareFile, nextFileName, isIntNum, isBoolean,
  describe, beforeAll, beforeEach, afterEach, afterAll, it, expect,
} from '../../Common';

describe('fileio_stat_fstat', function () {

  /**
   * @tc.number SUB_DF_FILEIO_STAT_FSTAT_SYNC_0000
   * @tc.name fileio_stat_fstat_sync_000
   * @tc.desc Test fileio.fstatSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_stat_fstat_sync_000', 0, async function () {
    let fpath = await nextFileName('fileio_stat_fstat_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2);
      expect(isIntNum(fd)).assertTrue();
      let stat = fileio.fstatSync(fd);
      expect(stat !== null).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_stat_fstat_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_STAT_FSTAT_SYNC_0010
   * @tc.name fileio_test_stat_fstat_sync_001
   * @tc.desc Test fstatSync() interfaces.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_stat_fstat_sync_001', 0, function () {
    try {
      let invalidFD = -1;
      fileio.fstatSync(invalidFD);
      expect(null).assertFail();
    } catch (e) {
      console.log('fileio_test_stat_fstat_sync_001 has failed for ' + e);
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_DEV_0000
   * @tc.name fileio_fstat_dev_000
   * @tc.desc Test the dev member of class Fstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_dev_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_dev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.dev)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_dev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_INO_0000
   * @tc.name fileio_fstat_ino_000
   * @tc.desc Test the ino member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_ino_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_ino_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.ino)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_ino_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_MODE_0000
   * @tc.name fileio_fstat_mode_000
   * @tc.desc Test the mode member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_mode_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_mode_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.mode)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_mode_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_NLINK_0000
   * @tc.name fileio_fstat_nlink_000
   * @tc.desc Test the nlink member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_nlink_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_nlink_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.nlink)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_nlink_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_UID_0000
   * @tc.name fileio_fstat_uid_000
   * @tc.desc Test the uid member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_uid_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_uid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.uid)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_uid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_GID_0000
   * @tc.name fileio_fstat_gid_000
   * @tc.desc Test the gid member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_gid_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_gid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.gid)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_gid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_RDEV_0000
   * @tc.name fileio_lstat_rdev_000
   * @tc.desc Test the rdev member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_rdev_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_rdev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.rdev)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_rdev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_SIZE_0000
   * @tc.name fileio_fstat_size_000
   * @tc.desc Test the size member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_size_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_size_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.size)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_size_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_BLOCKS_0000
   * @tc.name fileio_fstat_blocks_000
   * @tc.desc Test the blocks member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_blocks_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_blocks_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.blocks)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_blocks_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ATIME_0000
   * @tc.name fileio_fstat_atime_000
   * @tc.desc Test the atime member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_atime_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_atime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.atime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_atime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_MTIME_0000
   * @tc.name fileio_fstat_mtime_000
   * @tc.desc Test the mtime member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_mtime_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_mtime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.mtime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_mtime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_CTIME_0000
   * @tc.name fileio_fstat_ctime_000
   * @tc.desc Test the ctime member of class Fstat
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_ctime_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_ctime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.ctime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_lstat_ctime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_BLOCK_DEVICE_0000
   * @tc.name fileio_fstat_is_block_device_000
   * @tc.desc Test the isBlockDevice() method of class Fstat.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_block_device_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_block_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isBlockDevice())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_block_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_BLOCK_DEVICE_0010
   * @tc.name fileio_fstat_is_block_device_001
   * @tc.desc Test the isBlockDevice() method of class Fstat.
   * This interface shall not treat a normal file as a block special device.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_block_device_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_block_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isBlockDevice() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_block_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_BLOCK_DEVICE_0020
   * @tc.name fileio_test_fstat_is_block_device_002
   * @tc.desc Test Fstat.isBlockDevice() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_block_device_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_is_block_device_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isBlockDevice(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_CHARACTER_DEVICE_0000
   * @tc.name fileio_fstat_is_character_device_000
   * @tc.desc Test Fstat.isCharacterDevice() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_character_device_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_character_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isCharacterDevice())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_character_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_CHARACTER_DEVICE_0010
   * @tc.name fileio_fstat_is_character_device_001
   * @tc.desc Test Fstat.isCharacterDevice() interface.
   * This interface shall not treat a normal file as a character special device.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_character_device_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_character_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isCharacterDevice() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_character_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_CHARACTER_DEVICE_0020
   * @tc.name fileio_test_fstat_is_character_device_002
   * @tc.desc Test Fstat.isCharacterDevice() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_character_device_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_is_character_device_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isCharacterDevice(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_DIRECTORY_0000
   * @tc.name fileio_fstat_is_directory_000
   * @tc.desc Test Fstat.isDirectory() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_directory_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_directory_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isDirectory())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_directory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_DIRECTORY_0010
   * @tc.name fileio_fstat_is_directory_001
   * @tc.desc Test Fstat.isDirectory() interface.
   * This interface shall not treat a normal file as a directory.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_directory_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_directory_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isDirectory() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_directory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_DIRECTORY_0020
   * @tc.name fileio_fstat_is_directory_002
   * @tc.desc Test Fstat.isDirectory() interface.
   * This interface shall treat a directory as a directory.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_directory_002', 0, async function () {
    let dpath = await nextFileName('fileio_fstat_is_directory_002') + 'dd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isDirectory()).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_directory_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_DIRECTORY_0030
   * @tc.name fileio_test_fstat_is_directory_003
   * @tc.desc Test Fstat.isDirectory() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_directory_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_fstat_is_directory_003') + 'd';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isDirectory(-1)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FIFO_0000
   * @tc.name fileio_fstat_is_fifo_000
   * @tc.desc Test Fstat.isFIFO() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_fifo_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_fifo_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isFIFO())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_fifo_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FIFO_0010
   * @tc.name fileio_fstat_is_fifo_001
   * @tc.desc Test Fstat.isFIFO() interface.
   * This interface shall not treat a normal file as a FIFO.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_fifo_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_fifo_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isFIFO() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_fifo_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FIFO_0020
   * @tc.name fileio_test_fstat_is_fifo_002
   * @tc.desc Test Fstat.isFIFO() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_fifo_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_is_fifo_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isFIFO(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FILE_0000
   * @tc.name fileio_fstat_is_file_000
   * @tc.desc Test Fstat.isFile() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_file_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_file_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isFile())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_file_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FILE_0010
   * @tc.name fileio_fstat_is_file_001
   * @tc.desc Test Fstat.isFile() interface.
   * This interface shall treat a normal file as a normal file.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_file_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_file_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isFile()).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_file_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FILE_0020
   * @tc.name fileio_fstat_is_file_002
   * @tc.desc Test Fstat.isFile() interface.
   * This interface shall not treat a directory as a normal file.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_file_002', 0, async function () {
    let dpath = await nextFileName('fileio_fstat_is_file_002') + 'aa';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isFile() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_file_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_FILE_0030
   * @tc.name fileio_test_fstat_is_file_003
   * @tc.desc Test Fstat.isFile() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_file_003', 0, async function () {
    let dpath = await nextFileName('fileio_test_fstat_is_file_003') + 'ab';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isFile(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SOCKET_0000
   * @tc.name fileio_fstat_is_socket_000
   * @tc.desc Test Fstat.isSocket() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_socket_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_socket_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isSocket())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_socket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SOCKET_0010
   * @tc.name fileio_fstat_is_socket_001
   * @tc.desc Test Fstat.isSocket() interface.
   * This interface shall not treat a file as a socket.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_socket_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_socket_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isSocket() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_socket_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SOCKET_0020
   * @tc.name fileio_test_fstat_is_socket_002
   * @tc.desc Test Fstat.isSocket() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_socket_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_is_socket_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isSocket(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SYMBOLIC_LINK_0000
   * @tc.name fileio_fstat_is_symbolic_link_000
   * @tc.desc Test Fstat.isSymbolicLink() interface.
   * This interface shall return a boolean variable.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_symbolic_link_000', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_symbolic_link_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(isBoolean(stat.isSymbolicLink())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_symbolic_link_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SYMBOLIC_LINK_0010
   * @tc.name fileio_fstat_is_symbolic_link_001
   * @tc.desc Test Fstat.isSymbolicLink() interface.
   * This interface shall not treat a normal file as a symbolic link.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_is_symbolic_link_001', 0, async function () {
    let fpath = await nextFileName('fileio_fstat_is_symbolic_link_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isSymbolicLink() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_fstat_is_symbolic_link_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_IS_SYMBOLIC_LINK_0020
   * @tc.name fileio_test_fstat_is_symbolic_link_002
   * @tc.desc Test Fstat.isSymbolicLink() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_fstat_is_symbolic_link_002', 0, async function () {
    let fpath = await nextFileName('fileio_test_fstat_is_symbolic_link_002');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = fileio.fstatSync(fd);
      expect(stat.isSymbolicLink(-1) === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(null).assertFail();
    } catch (e) {
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_0000
   * @tc.name fileio_fstat_async_000
   * @tc.desc Test Stat.FstatAsync() interface.
   * This interface shall work properly in normal case when providing the promise async model.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      fileio.fstat(fd).then((stat) => {
        expect(stat !== null).assertTrue();
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
      });
      done();
    } catch (e) {
      console.log('fileio_fstat_async_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_0000
   * @tc.name fileio_fstat_async_001
   * @tc.desc Test Stat.FstatAsync() interface.
   * This interface shall work properly in normal case when providing the callback async model.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      fileio.fstat(fd, function (error) {
        expect(fileio.closeSync(fd) == null).assertTrue();
        expect(fileio.unlinkSync(fpath) == null).assertTrue();
        done();
      });
    } catch (e) {
      console.log('fileio_fstat_async_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_DEV_0000
   * @tc.name fileio_fstat_async_dev_000
   * @tc.desc Test the dev member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_dev_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_dev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.dev)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_dev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_INO_0000
   * @tc.name fileio_fstat_async_ino_000
   * @tc.desc Test the ino member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_ino_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_ino_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.ino)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_ino_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_MODE_0000
   * @tc.name fileio_fstat_async_mode_000
   * @tc.desc Test the mode member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_mode_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_mode_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.mode)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_mode_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_NLINK_0000
   * @tc.name fileio_fstat_async_nlink_000
   * @tc.desc Test the nlink member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_nlink_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_nlink_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.nlink)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_nlink_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_UID_0000
   * @tc.name fileio_fstat_async_uid_000
   * @tc.desc Test the uid member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_uid_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_uid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.uid)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_uid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_GID_0000
   * @tc.name fileio_fstat_async_gid_000
   * @tc.desc Test the gid member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_gid_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_gid_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.gid)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_gid_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_RDEV_0000
   * @tc.name fileio_fstat_async_rdev_000
   * @tc.desc Test the rdev member of class Lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_rdev_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_rdev_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.rdev)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_rdev_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_SIZE_0000
   * @tc.name fileio_fstat_async_size_000
   * @tc.desc Test the size member of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_size_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_size_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.size)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_size_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_BLOCKS_0000
   * @tc.name fileio_fstat_async_blocks_000
   * @tc.desc Test Fstat.blocks() interface.
   * @tc.desc Test the blocks member of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_blocks_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_blocks_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.blocks)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_blocks_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_ATIME_0000
   * @tc.name fileio_fstat_async_atime_000
   * @tc.desc Test Fstat.atime() interface.
   * @tc.desc Test the atime member of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_atime_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_atime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.atime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_atime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_MTIME_0000
   * @tc.name fileio_fstat_async_mtime_000
   * @tc.desc Test Fstat.mtime() interface.
   * @tc.desc Test the mtime member of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_mtime_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_mtime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.mtime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_mtime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_CTIME_0000
   * @tc.name fileio_fstat_async_ctime_000
   * @tc.desc Test the ctime member of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_ctime_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_ctime_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isIntNum(stat.ctime)).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_ctime_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_BLOCK_DEVICE_0000
   * @tc.name fileio_fstat_async_is_block_device_000
   * @tc.desc Test the isBlockDevice method of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_block_device_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_block_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isBlockDevice())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_block_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_BLOCK_DEVICE_0010
   * @tc.name fileio_fstat_async_is_block_device_001
   * @tc.desc Test the isBlockDevice method of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_block_device_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_block_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isBlockDevice() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_block_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_CHARACTER_DEVICE_0000
   * @tc.name fileio_fstat_async_is_character_device_000
   * @tc.desc Test the isCharacterDevice method of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_character_device_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_character_device_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isCharacterDevice())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_character_device_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_CHARACTER_DEVICE_0010
   * @tc.name fileio_fstat_async_is_character_device_001
   * @tc.desc Test the isCharacterDevice method of class lstat.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_character_device_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_character_device_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isCharacterDevice() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_character_device_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_DIRECTORY_0000
   * @tc.name fileio_fstat_async_is_directory_000
   * @tc.desc Test lstat.isDirectory() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_directory_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_directory_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isDirectory())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_directory_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_DIRECTORY_0010
   * @tc.name fileio_fstat_async_is_directory_001
   * @tc.desc Test lstat.isDirectory() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_directory_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_directory_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isDirectory() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_directory_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_DIRECTORY_0020
   * @tc.name fileio_fstat_async_is_directory_002
   * @tc.desc Test lstat.isDirectory() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_directory_002', 0, async function (done) {
    let dpath = await nextFileName('fileio_fstat_async_is_directory_002') + 'da';

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isDirectory()).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_directory_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_FIFO_0000
   * @tc.name fileio_fstat_async_is_fifo_000
   * @tc.desc Test lstat.isFIFO() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_fifo_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_fifo_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isFIFO())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_fifo_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_FIFO_0010
   * @tc.name fileio_fstat_async_is_fifo_001
   * @tc.desc Test lstat.isFIFO() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_fifo_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_fifo_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isFIFO() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_fifo_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_FILE_0000
   * @tc.name fileio_fstat_async_is_file_000
   * @tc.desc Test lstat.isFile() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_file_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_file_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isFile())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_file_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_FILE_0010
   * @tc.name fileio_fstat_async_is_file_001
   * @tc.desc Test lstat.isFile() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_file_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_file_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isFile()).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_file_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_FILE_0020
   * @tc.name fileio_fstat_async_is_file_002
   * @tc.desc Test lstat.isFile() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_file_002', 0, async function (done) {
    let dpath = await nextFileName('fileio_fstat_async_is_file_002');

    try {
      expect(fileio.mkdirSync(dpath) == null).assertTrue();
      let fd = fileio.openSync(dpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isFile() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.rmdirSync(dpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_file_002 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_SOCKET_0000
   * @tc.name fileio_fstat_async_is_socket_000
   * @tc.desc Test lstat.isSocket() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_socket_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_socket_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isSocket())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_socket_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_SOCKET_0010
   * @tc.name fileio_fstat_async_is_socket_001
   * @tc.desc Test lstat.isSocket() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_socket_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_socket_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isSocket() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_socket_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_SYMBOLIC_LINK_0000
   * @tc.name fileio_fstat_async_is_symbolic_link_000
   * @tc.desc Test fstat.isSymbolicLink() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_symbolic_link_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_symbolic_link_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(isBoolean(stat.isSymbolicLink())).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_symbolic_link_000 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_FSTAT_ASYNC_IS_SYMBOLIC_LINK_0010
   * @tc.name fileio_fstat_async_is_symbolic_link_001
   * @tc.desc Test lstat.isSymbolicLink() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_fstat_async_is_symbolic_link_001', 0, async function (done) {
    let fpath = await nextFileName('fileio_fstat_async_is_symbolic_link_001');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath);
      let stat = await fileio.fstat(fd);
      expect(stat.isSymbolicLink() === false).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
      done();
    } catch (e) {
      console.log('fileio_fstat_async_is_symbolic_link_001 has failed for ' + e);
      expect(null).assertFail();
    }
  });

  /**
   * @tc.number SUB_DF_FILEIO_APPEND_FILE_SYNC_0010
   * @tc.name fileio_test_append_file_sync_000
   * @tc.desc Test lstat.fstatSync() interface.
   * @tc.size MEDIUM
   * @tc.type Function
   * @tc.level Level 0
   * @tc.require
   */
  it('fileio_test_append_file_sync_000', 0, async function (done) {
    let fpath = await nextFileName('fileio_test_append_file_sync_000');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();

    try {
      let fd = fileio.openSync(fpath, 0o2002);
      let stat = fileio.fstatSync(fd);
      expect(isIntNum(stat.size)).assertTrue();
      expect(isIntNum(fd)).assertTrue();
      expect(fileio.writeSync(fd, FILE_CONTENT) == FILE_CONTENT.length).assertTrue();
      expect(fileio.closeSync(fd) == null).assertTrue();

      let fd2 = fileio.openSync(fpath, 0o2002);
      stat = fileio.fstatSync(fd2);
      expect(stat.size == FILE_CONTENT.length * 2).assertTrue();
      expect(fileio.closeSync(fd2) == null).assertTrue();
      expect(fileio.unlinkSync(fpath) == null).assertTrue();
    } catch (e) {
      console.log('fileio_test_append_file_sync_000 has failed for ' + e);
      expect(null).assertFail();
    }
    done();
  });

});