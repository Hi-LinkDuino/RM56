/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

// helpers for test
// place this is the tsconfig.json files section _before_
// any actual test cases

var tcaseCount = 0;
var testCount = 0;

function initTest(): void {
  tcaseCount = 0;
  testCount = 0;
}

function reportTestResults(): void {
  console.debug(`\nPassed all ${tcaseCount} cases / ${testCount} test asertions !`);
  initTest();
}

/*
    How to structure project tests:
    A project can have several tsuites,
    a tsuite can have several tcases,
    a tcase can make several test (asertions)

    An own file per tsuite is recommeneded
    index_test.ts calls all tsuites.
*/

function tsuite(msg: string, testMe: () => void): () => void {
  const msg1 = msg;
  const testMe1 = testMe;
  return function () {
    console.debug(`\n\n====== ${msg1}`);
    testMe1();
    console.debug(`\n`);
  }
}

function tcase(msg: string, testMe: () => void): void {
  console.debug(`\n------ ${msg}`);
  testMe();
  tcaseCount++;
}

function test(msg: string, asertion: boolean): void {
  if (!asertion) {
    console.error(`Failed: '${msg}'.`);
    process.exit(-1);
  }
  console.debug(`Passed: '${msg}'.`);
  testCount++
}

/*
 use to intercept a function call on an object to asert is was called
 use returned object { called: boolean, args: 'arguments' of call }
 to forumate 'test' clauses
 */
function spyOn(source: Object, funcName: string): { called: boolean, args: any[] } {
  const origFunc = source[funcName];
  const source1 = source;
  const sourceDescr = source.constructor ? source.constructor.name : "unknown object";
  let accessRecord = { called: false, args: undefined };
  source[funcName] = function () {
    console.debug(`SpyOn: ----- '${funcName}' on '${sourceDescr}' about to be called, ${arguments.length} arguments ...`);
    origFunc.call(source1, ...arguments);
    accessRecord.called = true;
    accessRecord.args = /* deep copy */ JSON.parse(JSON.stringify(arguments));
    console.debug(`SpyOn: ----- '${funcName}' on '${sourceDescr}' done!`);
  };
  return accessRecord;
}


function eqSet(as, bs) {
  if (as.size !== bs.size) return false;
  for (var a of as) if (!bs.has(a)) return false;
  return true;
}
