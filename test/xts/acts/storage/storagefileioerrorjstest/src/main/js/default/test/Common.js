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

import fileio from '@system.fileio'

export const FILE_CONTENT = 'hello world'

export function prepareFile(fpath, content) {
  try {
    let fd = fileio.openSync(fpath, 0o102, 0o666)
    fileio.ftruncateSync(fd)
    fileio.writeSync(fd, content)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to prepareFile for ' + e)
    return false
  }
}
export function prepareFile1(fpath, content) {
  try {
    let fd = fileio.openSync(fpath, 0o102, 0o777)
    fileio.ftruncateSync(fd)
    fileio.writeSync(fd, content)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to prepareFile for ' + e)
    return false
  }
}
export function prepareEmptyFile(fpath) {
  try {
    let fd = fileio.openSync(fpath, 0o102, 0o777)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to prepareFile for ' + e)
    return false
  }
}
export function fileToReadOnly(fpath) {
  try {
    let fd = fileio.openSync(fpath, 0o1)
    fileio.fchmodSync(fd, 0o444)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to fileToReadOnly for ' + e);
    return false
  }
}
export function fileToWriteOnly(fpath) {
  try {
    let fd = fileio.openSync(fpath, 0o2)
    fileio.fchmodSync(fd, 0o222)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to fileToWriteOnly ' + e)
    return false
  }
}
export function fileToReadAndWrite(fpath) {
  try {
    let fd = fileio.openSync(fpath, 0o1)
    fileio.fchmodSync(fd, 0o777)
    fileio.fsyncSync(fd)
    fileio.closeSync(fd)
    return true
  } 
  catch (e) {
    console.log('Failed to fileToReadAndWrite ' + e);
    return false
  }
}
export function appName(testName) {
  const BASE_PATH = '/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/'
  return BASE_PATH + testName
}
export function nextFileName(testName) {
  const BASE_PATH = '/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/cache/'
  return BASE_PATH + testName
}
export function fileName(testName) {
  const BASE_PATH = '/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/files/'
  return BASE_PATH + testName
}
export function cacheFileName(testName) {
  const BASE_PATH = '/data/accounts/account_0/appdata/ohos.acts.stroage.fileio/files/cache/'
  return BASE_PATH + testName
}
export function differentFileName(testName) {
  const BASE_PATH = '/data/accounts/account_0/ohos.acts.distributeddatamgr.distributedfile/'
  return BASE_PATH + testName
}
export function differentCacheName(testName) {
  const BASE_PATH = '/data/accounts/account_0/ohos.acts.distributeddatamgr.distributedfile/cache/'
  return BASE_PATH + testName
}

export function getFileTextLen(fpath) {
  let ss
  try {
    ss = fileio.Stream.createStreamSync(fpath, 'r+')
    expect(ss !== null).assertTrue()
    let len = ss.readSync(new ArrayBuffer(4096))
    console.log('file:' + fpath)
    console.log('file length:' + len)
    expect(ss.closeSync() !== null).assertTrue()
    return len
  } 
  catch (e) {
    console.log('Failed to getFileTextLen ' + e)
    expect(ss.closeSync() !== null).assertTrue()
    return null
  }
}
export function isFileExist(fpath) {
  try {
    expect(fileio.accessSync(fpath) !== null).assertTrue()
    console.log('file:' + fpath)
    console.log('status:exist')
    return true
  } 
  catch (e) {
    console.log('file:' + fpath)
    console.log('status:non-existen')
    return false
  }
}
export function sleep(n) {
  var start = new Date().getTime();
  while (true) {
    if (new Date().getTime() - start > n) {
      break;
    }
  }
}
export function randomString(num) {
  let len= num;
  var $chars = 'aaaabbbbcccc';
  var maxPos = $chars.length;
  var pwd = '';
  for (var i = 0; i < len; i++) {
    pwd += $chars.charAt(Math.floor(Math.random() * maxPos));
  }
  return pwd;
}
