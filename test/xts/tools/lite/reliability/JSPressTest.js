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
var endFlag = false;
class PressureTest {
    constructor(core) {
        this.core = core;
        this.rootSuite = this.core.getDefaultService("suite").rootSuite;
        this.suites = [];
        this.its = [];
        this.times = 5;
        this.timeout = 10;
        this.init();
    }

    init() {
        if (this.rootSuite.childSuites.length > 0) {
            for (var i in this.rootSuite.childSuites) {
                this.getAllIt(this.rootSuite.childSuites[i]);
            }
        }
    }

    getAllIt(suite) {
        if (suite.specs.length > 0) {
            for (var i in suite.specs) {
                var oneIt = {
                    it: suite.specs[i],
                    suite: suite
                }
                this.its.push(oneIt);
            }
        }
        if (suite.childSuites.length > 0) {
            for (var i in suite.childSuites) {
                this.getAllIt(suite.childSuites[i]);
            }
        }
    }

    run() {
        this.core.fireEvents('task', 'taskStart');
        let date = new Date();
        let start = date.getTime();
        let end = start + this.timeout * 1000;
        if (this.core.getDefaultService('config').isSupportAsync()) {
            while(this.times > 0 || start < end) {
                let asyncExecute = async () => {
                    await this.runIts();
                }
                this.times--;
                date = new Date();
                start = date.getTime();
            }
        } else {
            while(this.times > 0 || start < end) {
                this.runIts();
                this.times--;
                date = new Date();
                start = date.getTime();
            }
        }
        var res = this.core.getDefaultService("suite").getSummary();
        this.core.fireEvents('task', 'taskDone');
    }

    runIts() {
        var itsTmp = [];
        itsTmp = Array.from(this.its);
        var lastSuite = null;
        while (itsTmp.length > 0) {
            var index = this.random(0, itsTmp.length - 1);
            var it = itsTmp[index];
            this.runIt(it, lastSuite);
            lastSuite = it.suite;
            itsTmp.splice(index, 1);
        }
        lastSuite.runHookFunc('afterAll');
    }

    runIt(it, lastSuite) {
        var suite = it.suite;
        if (lastSuite !== suite) {
            if (lastSuite !== null) {
                lastSuite.runHookFunc('afterAll');
            }
            suite.runHookFunc('beforeAll');
        }
        suite.runHookFunc('beforeEach');
        it.it.run(this.core);
        suite.runHookFunc('afterEach');
    }

    random(min, max) {
        return parseInt(Math.random()*(max - min) + min);
    }

    setPressTimes(times) {
        this.times = times;
    }

    setTimeout(timeout) {
        this.timeout = timeout;
    }
}

export {PressureTest}