/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import fileio from '@system.fileio'
import file from '@system.file';
import {
  describe,
  it,
  expect
}
  from 'deccjsunit/index'
import {
  FILE_CONTENT,
  prepareFile,
  fileName,
  randomString,
}
  from './Common'

describe('FileError', function () {

  /**
   * @tc.number SUB_STORAGE_File_test_error_0000
   * @tc.name File_test_error_000
   * @tc.desc Function of API, Move files when disk space is full.
   *  Create the file (file_test_error) in advance and give 777 permission,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/files/file_test_error
   */
  it('File_test_error_000', 0, async function (done) {
    file.move({
      srcUri: 'internal://app/file_test_error',
      dstUri: 'internal://cache/file_test_error',
      success: function (uri) {
        console.log('file_test_error_000 pass, uri:' + uri);
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_000 call move fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0100
   * @tc.name File_test_error_001
   * @tc.desc Function of API, Copy files when disk space is full.
   *  Create the file (file_test_error) in advance and give 777 permission,
   *  path:/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/files/file_test_error
   */
  it('File_test_error_001', 0, async function (done) {
    file.copy({
      srcUri: 'internal://app/file_test_error',
      dstUri: 'internal://cache/file_test_error',
      success: function (uri) {
        console.log('file_test_error_001 pass, uri:' + uri);
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_001 call copy fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0200
   * @tc.name File_test_error_002
   * @tc.desc Function of API, Write files when disk space is full.
   */
  it('File_test_error_002', 0, async function (done) {
    file.writeText({
      uri: 'internal://app../file_test_error',
      text: FILE_CONTENT,
      success: function () {
        console.log('file_test_error_002 call writeText success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_002 call writeText fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0300
   * @tc.name File_test_error_003
   * @tc.desc Function of API, Write files when disk space is full.
   */
  it('File_test_error_003', 0, async function (done) {
    let buf = new Uint8Array([48, 49, 50, 51, 65, 66, 67, 68, 32, 33]);
    file.writeArrayBuffer({
      uri: 'internal://app/file_test_error',
      buffer: buf,
      success: function () {
        console.log('file_test_error_003 call writeArrayBuffer success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_003 call writeArrayBuffer fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0400
   * @tc.name File_test_error_004
   * @tc.desc Function of API, When inode is full, verify the folder creation function
   */
  it('File_test_error_004', 0, async function (done) {
    file.mkdir({
      uri: 'internal://app/file_test_error004d',
      success: function () {
        console.log('file_test_error_004 call mkdir success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_004 call mkdir fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0500
   * @tc.name File_test_error_005
   * @tc.desc Function of API, Prerequisites create file 200M,When ram is filled to
   * the remaining 100m, verify the function of reading 200m files
   */
  it('File_test_error_005', 0, async function (done) {
    file.readText({
      uri: 'internal://app/file_test_error_005',
      success: function (data) {
        console.log('file_test_error_005 pass, Content:' + data.text);
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_005 call readText fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0600
   * @tc.name File_test_error_006
   * @tc.desc Function of API, Prerequisites create file 200M,When ram is filled to
   * the remaining 100m, verify the function of reading 200m files
   */
  it('File_test_error_006', 0, async function (done) {
    file.readArrayBuffer({
      uri: 'internal://app/file_test_error_006',
      success: function (data) {
        console.log('file_test_error_006 pass, Content:' + data.buffer);
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_006 call readArrayBuffer fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0700
   * @tc.name File_test_error_007
   * @tc.desc Function of API, delete file
   */
  it('File_test_error_007', 0, async function (done) {
    let fpath = fileName('file_test_error_007');
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    file.rmdir({
      uri: 'internal://app/file_test_error_007',
      recursive: true,
      success: function () {
        console.log('file_test_error_007 call rmdir success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_007 call rmdir fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0800
   * @tc.name File_test_error_008
   * @tc.desc Function of API, Try to delete the folder while the file in the folder is reading and writing
   */
  it('File_test_error_008', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/file_test_error_008',
      text: randomString(4096),
      success: function () {
        console.log('file_test_error_008 call mkdir success.');
        done();
      },
      fail: function (data, code) {
        console.log('file_test_error_008 call mkdir fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.rmdir({
      uri: 'internal://app/file_test_error_008',
      success: function () {
        console.log('file_test_error_008 call rmdir success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_008 call rmdir fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_0900
   * @tc.name File_test_error_009
   * @tc.desc Function of API, Delete non empty folder
   */
  it('File_test_error_009', 0, async function (done) {
    let dpath = fileName('file_test_error_009') + 'd';
    let fpath = dpath + '/file_test_error_009';
    expect(fileio.mkdirSync(dpath) !== null).assertTrue();
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    file.rmdir({
      uri: 'internal://app/file_test_error_009d',
      recursive: true,
      success: function () {
        console.log('file_test_error_009 call rmdir success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_009 call rmdir fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_1000
   * @tc.name File_test_error_010
   * @tc.desc Function of API, Delete non empty folder
   */
  it('File_test_error_010', 0, async function (done) {
    let dpath = fileName('file_test_error_010') + 'd';
    let fpath = dpath + '/file_test_error_010';
    expect(fileio.mkdirSync(dpath) !== null).assertTrue();
    expect(prepareFile(fpath, FILE_CONTENT)).assertTrue();
    file.rmdir({
      uri: 'internal://app/file_test_error_010d',
      recursive: false,
      success: function () {
        console.log('file_test_error_010 call rmdir success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_010 call rmdir fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_1100
   * @tc.name File_test_error_011
   * @tc.desc Function of API, Unsupported same encoding format
   */
  it('File_test_error_011', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/file_test_error_011',
      text: 'hello',
      encoding: 'gbk',
      success: function () {
        console.log('file_test_error_011 call writeText success.');
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_011 call writeText fail, code: ' + code + ', data: ' + data);
        done();
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });

  /**
   * @tc.number SUB_STORAGE_File_test_error_1200
   * @tc.name File_test_error_012
   * @tc.desc Function of API, Unsupported same encoding format
   */
  it('File_test_error_012', 0, async function (done) {
    file.writeText({
      uri: 'internal://app/file_test_error_012',
      text: 'Text that just for test.',
      encoding: 'Utf-8',
      success: function () {
        console.log('file_test_error_012 call writeText success.');
        done();
      },
      fail: function (data, code) {
        console.log('file_test_error_012 call writeText fail, code: ' + code + ', data: ' + data);
        expect(null).assertFail();
      },
    });
    file.readText({
      uri: 'internal://app/file_test_error_012',
      encoding: 'GBK',
      success: function (data) {
        console.log('file_test_error_012 call readText success.' + data.text);
        expect(null).assertFail();
      },
      fail: function (data, code) {
        console.log('file_test_error_012 call readText fail, code: ' + code + ', data: ' + data);
        file.delete({
          uri: 'internal://app/file_test_error_012',
          success: function () {
            console.log('file_test_error_012 call delete success');
            done();
          },
          fail: function (data, code) {
            console.log('file_test_error_012 call delete fail, code: ' + code + ', data: ' + data);
            expect(null).assertFail();
          },
        });
      },
    });
    setTimeout(
      function(){
        expect(null).assertFail();
        done();
      },30)
  });
});
