/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import process from '@ohos.process'
describe('ChildProcessTest', function () {

    /**
     * @tc.name: testRunCmd001
     * @tc.desc: Return a child process object and spawns a new ChildProcess to run the command.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testRunCmd001', 0, async function (done) {
        let child = process.runCmd('echo abc')
        child.wait()
        let array = new Uint8Array([97, 98, 99, 10, 0])
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testRunCmd002
     * @tc.desc: Return a child process object and spawns a new ChildProcess to run the command.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testRunCmd002', 0, async function (done) {
        let child = process.runCmd('echo abc;', { maxBuffer : 2 })
        child.wait()
        let array = new Uint8Array([97, 98, 0])
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testRunCmd003
     * @tc.desc: Return a child process object and spawns a new ChildProcess to run the command.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testRunCmd003', 0, async function (done) {
        let child = process.runCmd('sleep 5; echo abc;', { timeout : 1, killSignal : 9 })
        child.wait()
        let array = new Uint8Array([0])
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        expect(child.exitCode).assertEqual(9)
        done();
    })

    /**
     * @tc.name: testRunCmd004
     * @tc.desc: Return a child process object and spawns a new ChildProcess to run the command.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testRunCmd004', 0, async function (done) {
        let child = process.runCmd('sleep 2; echo abc;', { timeout : 9000, killSignal : 9 })
        child.wait()
        let array = new Uint8Array([97, 98, 99, 10, 0])
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        expect(child.exitCode).assertEqual(0)
        done();
    })

    /**
     * @tc.name: testRunCmd005
     * @tc.desc: Return a child process object and spawns a new ChildProcess to run the command.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testRunCmd005', 0, async function (done) {
        let child = process.runCmd('echo abc', { maxBuffer : 1000 })
        child.wait()
        let array = new Uint8Array([97, 98, 99, 10, 0])
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetOutput001
     * @tc.desc: return it as 'Uint8Array' of the stdout until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetOutput001', 0, async function (done) {
        let child = process.runCmd('echo bcd;')
        let array = new Uint8Array([98, 99, 100, 10, 0])
        child.wait();
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetOutput002
     * @tc.desc: return it as 'Uint8Array' of the stdout until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetOutput002', 0, async function (done) {
        let child = process.runCmd('echo 123;');
        let array = new Uint8Array([49, 50, 51, 10, 0]);
        child.wait();
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i]);
            }
        });
        done();
    })

    /**
     * @tc.name: testGetOutput003
     * @tc.desc: return it as 'Uint8Array' of the stdout until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetOutput003', 0, async function (done) {
        let child = process.runCmd('echo helloWorld;');
        let array = new Uint8Array([104, 101, 108, 108, 111, 87, 111, 114, 108, 100, 10, 0]);
        child.wait();
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i]);
            }
        });
        done();
    })

    /**
     * @tc.name: testGetOutput004
     * @tc.desc: return it as 'Uint8Array' of the stdout until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetOutput004', 0, async function (done) {
        let child = process.runCmd('echo 浣犲�?');
        let array = new Uint8Array([230, 181, 163, 231, 138, 178, 239, 191, 189, 63, 10, 0]);
        child.wait();
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i]);
            }
        });
        done();
    })

    /**
     * @tc.name: testGetOutput005
     * @tc.desc: return it as 'Uint8Array' of the stdout until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetOutput005', 0, async function (done) {
        let child = process.runCmd('echo ~_~;');
        let array = new Uint8Array([126, 95, 126, 10, 0]);
        child.wait();
        await child.getOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i]);
            }
        });
        done();
    })

    /**
     * @tc.name: testGetErrorOutput001
     * @tc.desc: return it as 'Uint8Array of the stderr until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetErrorOutput001', 0, async function (done) {
        let child = process.runCmd('makdir 1.txt')
        child.wait()
        let array = new Uint8Array([115, 104, 58, 32, 109, 97, 107, 100, 105, 114, 58, 32, 105, 110, 97, 99, 99,
        101, 115, 115, 105, 98, 108, 101, 32, 111, 114, 32, 110, 111, 116, 32, 102, 111, 117, 110, 100, 10, 0])
        await child.getErrorOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetErrorOutput002
     * @tc.desc: return it as 'Uint8Array of the stderr until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetErrorOutput002', 0, async function (done) {
        let child = process.runCmd('echo "error" 1>&2')
        child.wait()
        let array = new Uint8Array([101, 114, 114, 111, 114, 10, 0])
        await child.getErrorOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetErrorOutput003
     * @tc.desc: return it as 'Uint8Array of the stderr until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetErrorOutput003', 0, async function (done) {
        let child = process.runCmd('1')
        child.wait()
        let array = new Uint8Array([115, 104, 58, 32, 49, 58, 32, 105, 110, 97, 99, 99, 101, 115, 115, 105, 98,
        108, 101, 32, 111, 114, 32, 110, 111, 116, 32, 102, 111, 117, 110, 100, 10, 0])
        await child.getErrorOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetErrorOutput004
     * @tc.desc: return it as 'Uint8Array of the stderr until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetErrorOutput004', 0, async function (done) {
        let child = process.runCmd('chmod 777 123')
        let array = new Uint8Array([99, 104, 109, 111, 100, 58, 32, 49, 50, 51, 58, 32, 78, 111, 32, 115, 117, 99,
        104, 32, 102, 105, 108, 101, 32, 111, 114, 32, 100, 105, 114, 101, 99, 116, 111, 114, 121, 10, 0]);
        child.wait();
        await child.getErrorOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testGetErrorOutput005
     * @tc.desc: return it as 'Uint8Array of the stderr until EOF.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetErrorOutput005', 0, async function (done) {
        let child = process.runCmd('cp ./1 ./2')
        let array = new Uint8Array([99, 112, 58, 32, 98, 97, 100, 32, 39, 46, 47, 49, 39, 58, 32, 78, 111, 32, 115,
        117, 99, 104, 32, 102, 105, 108, 101, 32, 111, 114, 32, 100, 105, 114, 101, 99, 116, 111, 114, 121, 10, 0])
        child.wait();
        await child.getErrorOutput().then(val=>{
            for (let i = 0; i < array.length; i++) {
                expect(val[i]).assertEqual(array[i])
            }
        });
        done();
    })

    /**
     * @tc.name: testWait001
     * @tc.desc: return 'number' is the targer process exit code.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testWait001', 0, async function (done) {
        let child = process.runCmd('ls')
        await child.wait().then(val=>{
            expect(val).assertEqual(0)
        });
        done();
    })

    /**
     * @tc.name: testWait002
     * @tc.desc: return 'number' is the targer process exit code.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testWait002', 0, async function (done) {
        let child = process.runCmd('ls; sleep 5;')
        child.kill(9);
        await child.wait().then(val=>{
            expect(val).assertEqual(9)
        });
        done();
    })

    /**
     * @tc.name: testWait003
     * @tc.desc: return 'number' is the targer process exit code.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testWait003', 0, async function (done) {
        let child = process.runCmd('echo helloWorld');
        await child.wait().then(val=>{
            expect(val).assertEqual(0);
        });
        done();
    })

    /**
     * @tc.name: testWait004
     * @tc.desc: return 'number' is the targer process exit code.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testWait004', 0, async function (done) {
        let child = process.runCmd('mkdir 123');
        await child.wait().then(val=>{
            expect(val).assertEqual(0);
        });
        done();
    })

    /**
     * @tc.name: testWait005
     * @tc.desc: return 'number' is the targer process exit code.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testWait005', 0, async function (done) {
        let child = process.runCmd('sleep 5; echo abc;', { timeout : 1, killSignal : 9 });
        await child.wait().then(val=>{
            expect(val).assertEqual(9);
        });
        done();
    })

    /**
     * @tc.name: testchildPid001
     * @tc.desc: return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testchildPid001', 0, function () {
        let child = process.runCmd('ls; sleep 5;')
        child.wait()
        let pid_ = child.pid
        expect(pid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPid002
     * @tc.desc: return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid002', 0, function () {
        let child = process.runCmd('mkdir test.cpp')
        child.wait()
        let pid_ = child.pid
        expect(pid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPid003
     * @tc.desc: return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid003', 0, function () {
        let child = process.runCmd('each abc')
        child.wait()
        let pid_ = child.pid
        expect(pid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPid004
     * @tc.desc: return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid004', 0, function () {
        let child = process.runCmd('cd ..')
        child.wait()
        let pid_ = child.pid
        expect(pid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPid005
     * @tc.desc: return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid005', 0, function () {
        let child = process.runCmd('adc')
        child.wait()
        let pid_ = child.pid
        expect(pid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPpid001
     * @tc.desc: return ppid is the pid of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid001', 0, function () {
        let child = process.runCmd('ls; sleep 5;')
        let status = child.wait()
        let ppid_ = child.ppid
        expect(ppid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPpid002
     * @tc.desc: return ppid is the pid of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid002', 0, function () {
        let child = process.runCmd('mkdir test.cpp')
        child.wait()
        let ppid_ = child.ppid
        expect(ppid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPpid003
     * @tc.desc: return ppid is the pid of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid003', 0, function () {
        let child = process.runCmd('each abc')
        child.wait()
        let ppid_ = child.ppid
        expect(ppid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPpid004
     * @tc.desc: return ppid is the pid of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid004', 0, function () {
        let child = process.runCmd('cd ..')
        child.wait()
        let ppid_ = child.ppid
        expect(ppid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testPpid005
     * @tc.desc: return ppid is the pid of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid005', 0, function () {
        let child = process.runCmd('adc')
        child.wait()
        let ppid_ = child.ppid
        expect(ppid_ > 0).assertEqual(true)
    })

    /**
     * @tc.name: testKill001
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill001', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill(13)
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKill002
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill002', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill(1)
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKill003
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill003', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill('SIGHUP')
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKill004
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill004', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill('ABC')
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKill005
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill005', 0, function () {
        let child =  process.runCmd('ls')
        child.wait()
        let result = child.kill(9)
        let temp = child.killed
        expect(temp).assertEqual(false)
    })

    /**
     * @tc.name: testClose001
     * @tc.desc: close the target process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testClose001', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.close()
        expect(child.exitCode).assertEqual(9)
    })

    /**
     * @tc.name: testClose002
     * @tc.desc: close the target process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testClose002', 0, function () {
        let child =  process.runCmd('ls')
        function sleep(ms, callback) {
            setTimeout(callback, ms)
        }
        sleep(1000, () => {} )
        let result = child.close()
        expect(child.exitCode === 0).assertEqual(false)
    })

    /**
     * @tc.name: testClose003
     * @tc.desc: close the target process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testClose003', 0, function () {
        let child =  process.runCmd('ls;')
        let status = child.wait()
        let result = child.close()
        expect(child.exitCode).assertEqual(0)
    })

    /**
     * @tc.name: testClose004
     * @tc.desc: close the target process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testClose004', 0, function () {
        let child =  process.runCmd('ls')
        let result = child.close()
        expect(child.exitCode).assertEqual(9)
    })

    /**
     * @tc.name: testClose005
     * @tc.desc: close the target process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testClose005', 0, function () {
        let child =  process.runCmd('sleep 5; echo A')
        let result = child.close()
        let status = child.wait()
        expect(child.exitCode).assertEqual(9)
    })

    /**
     * @tc.name: testKilled001
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKilled001', 0, function () {
        let child =  process.runCmd('ls; sleep 1s;')
        let result = child.kill(3)
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKilled002
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKilled002', 0, function () {
        let child =  process.runCmd('ls; sleep 1s;')
        let result = child.close()
        let temp = child.killed
        expect(temp).assertEqual(false)
    })

    /**
     * @tc.name: testKilled003
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKilled003', 0, function () {
        let child =  process.runCmd('ls; sleep 1s;')
        let status = child.wait()
        let result = child.kill(9)
        let temp = child.killed
        expect(temp).assertEqual(false)
    })

    /**
     * @tc.name: testKilled004
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKilled004', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill('SIGHUP')
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testKilled005
     * @tc.desc: return boolean is whether the current process signal is sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKilled005', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let result = child.kill('1')
        let temp = child.killed
        expect(temp).assertEqual(true)
    })

    /**
     * @tc.name: testExitCode001
     * @tc.desc: return exitCode is the exit code of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testExitCode001', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        let temp = child.kill(9)
        child.wait()
        let result = child.exitCode
        expect(result).assertEqual(9)
    })

    /**
     * @tc.name: testExitCode002
     * @tc.desc: return exitCode is the exit code of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testExitCode002', 0, function () {
        let child =  process.runCmd('ls;')
        child.wait()
        let result = child.exitCode
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testExitCode003
     * @tc.desc: return exitCode is the exit code of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testExitCode003', 0, function () {
        let child =  process.runCmd('ls; sleep 5s;')
        child.close()
        child.wait()
        let result = child.exitCode
        expect(result).assertEqual(9)
    })

    /**
     * @tc.name: testExitCode004
     * @tc.desc: return exitCode is the exit code of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testExitCode004', 0, function () {
        let child =  process.runCmd('ls')
        let temp = child.kill('ABCD')
        child.close()
        let result = child.exitCode
        expect(result).assertEqual(0)
    })

    /**
     * @tc.name: testExitCode005
     * @tc.desc: return exitCode is the exit code of the current child process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testExitCode005', 0, function () {
        let child =  process.runCmd('echo A')
        let result= child.wait()
        let status = child.close()
        expect(child.exitCode).assertEqual(0)
    })

    /**
     * @tc.name: testGetUid001
     * @tc.desc: returns the digital user id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetUid001', 0, function () {
        let result = process.uid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testGetUid002
     * @tc.desc: returns the digital user id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetUid002', 0, function () {
        for(let i = 0; i < 3; i++){
            let result = process.uid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetUid003
     * @tc.desc: returns the digital user id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetUid003', 0, function () {
        for(let i = 0; i < 6; i++){
            let result = process.uid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetUid004
     * @tc.desc: returns the digital user id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetUid004', 0, function () {
        for(let i = 0; i < 8; i++){
            let result = process.uid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetUid005
     * @tc.desc: returns the digital user id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetUid005', 0, function () {
        for(let i = 0; i < 5; i++){
            let result = process.uid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGid001
     * @tc.desc: returns the numeric group id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGid001', 0, function () {
        let result = process.gid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testGetGid002
     * @tc.desc: returns the numeric group id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGid002', 0, function () {
        for(let i = 0; i < 3; i++){
            let result = process.gid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGid003
     * @tc.desc: returns the numeric group id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGid003', 0, function () {
        for(let i = 0; i < 5; i++){
            let result = process.gid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGid004
     * @tc.desc: returns the numeric group id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGid004', 0, function () {
        for(let i = 0; i < 6; i++){
            let result = process.gid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGid005
     * @tc.desc: returns the numeric group id of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGid005', 0, function () {
        for(let i = 0; i < 8; i++){
            let result = process.gid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEuid001
     * @tc.desc: return the numeric valid user identity of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEuid001', 0, function () {
        let result = process.euid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testGetEuid002
     * @tc.desc: return the numeric valid user identity of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEuid002', 0, function () {
        for(let i = 0; i < 3; i++){
            let result = process.euid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEuid003
     * @tc.desc: return the numeric valid user identity of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEuid003', 0, function () {
        for(let i = 0; i < 5; i++){
            let result = process.euid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEuid004
     * @tc.desc: return the numeric valid user identity of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEuid004', 0, function () {
        for(let i = 0; i < 6; i++){
            let result = process.euid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEuid005
     * @tc.desc: return the numeric valid user identity of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEuid005', 0, function () {
        for(let i = 0; i < 8; i++){
            let result = process.euid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEgid001
     * @tc.desc: returns the numeric valid group ID of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEgid001', 0, function () {
        let result = process.egid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testGetEgid002
     * @tc.desc: returns the numeric valid group ID of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEgid002', 0, function () {
        for(let i = 0; i < 3; i++){
            let result = process.egid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEgid003
     * @tc.desc: returns the numeric valid group ID of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEgid003', 0, function () {
        for(let i = 0; i < 5; i++){
            let result = process.egid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEgid004
     * @tc.desc: returns the numeric valid group ID of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEgid004', 0, function () {
        for(let i = 0; i < 6; i++){
            let result = process.egid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEgid005
     * @tc.desc: returns the numeric valid group ID of the process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetEgid005', 0, function () {
        for(let i = 0; i < 8; i++){
            let result = process.egid
            if(result != null) {
                if(result > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGroups001
     * @tc.desc: return an array with supplementary group id.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGroups001', 0, function () {
        let result = process.groups
        let len = result.length
        if(len > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testGetGroups002
     * @tc.desc: return an array with supplementary group id.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGroups002', 0, function () {
        for(let i = 0; i < 3; i++){
            let result = process.groups
            if(result != null) {
                let len = result.length
                if(len > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGroups003
     * @tc.desc: return an array with supplementary group id.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGroups003', 0, function () {
        for(let i = 0; i < 5; i++){
            let result = process.groups
            if(result != null) {
                let len = result.length
                if(len > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGroups004
     * @tc.desc: return an array with supplementary group id.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGroups004', 0, function () {
        for(let i = 0; i < 6; i++){
            let result = process.groups
            if(result != null) {
                let len = result.length
                if(len > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetGroups005
     * @tc.desc: return an array with supplementary group id.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testGetGroups005', 0, function () {
        for(let i = 0; i < 8; i++){
            let result = process.groups
            if(result != null) {
                let len = result.length
                if(len > 0) {
                    var flag = true
                }
                expect(flag).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testChdir001
     * @tc.desc: Change current directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testChdir001', 0, function () {
        let result = process.chdir('/system')
        let result1 = process.cwd()
        expect(result1).assertEqual('/system')
    })

    /**
     * @tc.name: testChdir002
     * @tc.desc: Change current directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testChdir002', 0, function () {
        let result = process.chdir('/system/lib')
        let result1 = process.cwd()
        expect(result1).assertEqual('/system/lib')
    })

    /**
     * @tc.name: testChdir003
     * @tc.desc: Change current directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testChdir003', 0, function () {
        let result = process.chdir('/..')
        let result1 = process.cwd()
        expect(result1).assertEqual('/')
    })

    /**
     * @tc.name: testChdir004
     * @tc.desc: Change current directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testChdir004', 0, function () {
        let result = process.chdir('/system/app')
        let result1 = process.cwd()
        expect(result1).assertEqual('/system/app')
    })

    /**
     * @tc.name: testChdir005
     * @tc.desc: Change current directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testChdir005', 0, function () {
        let result = process.chdir('/data')
        let result1 = process.cwd()
        expect(result1).assertEqual('/data')
    })

    /**
     * @tc.name: testKill001
     * @tc.desc: Return whether the signal was sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill001', 0, function () {
        let result = process.kill(3, 123)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testKill002
     * @tc.desc: Return whether the signal was sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill002', 0, function () {
        let pres = process.pid
        let result = process.kill(23, pres)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testKill003
     * @tc.desc: Return whether the signal was sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill003', 0, function () {
        let pres = process.pid
        let result = process.kill(28, pres)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testKill004
     * @tc.desc: Return whether the signal was sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill004', 0, function () {
        let pres = process.pid
        let result = process.kill(17, pres)
        expect(result).assertEqual(true)
    })

    /**
     * @tc.name: testKill005
     * @tc.desc: Return whether the signal was sent successfully.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testKill005', 0, function () {
        let result = process.kill(3, 113)
        expect(result).assertEqual(false)
    })

    /**
     * @tc.name: testUptime001
     * @tc.desc: Returns the running time of the system.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testUptime001', 0, function () {
        let result1 = process.uptime()
        function sleep(d){
            while(process.uptime() - result1 <= d);
        }
        sleep(6);
        let result2 = process.uptime()
        var flag = false
        if ((result2 - result1) >= 6)
        {
            flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testUptime002
     * @tc.desc: Returns the running time of the system.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testUptime002', 0, function () {
        let result1 = process.uptime()
        function sleep(d){
            while(process.uptime() - result1 <= d);
        }
        sleep(8);
        let result2 = process.uptime()
        var flag = false
        if ((result2 - result1) >= 8)
        {
            flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testUptime003
     * @tc.desc: Returns the running time of the system.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testUptime003', 0, function () {
        let result1 = process.uptime()
        function sleep(d){
            while(process.uptime() - result1 <= d);
        }
        sleep(10);
        let result2 = process.uptime()
        var flag = false
        if ((result2 - result1) >= 10)
        {
            flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testUptime004
     * @tc.desc: Returns the running time of the system.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testUptime004', 0, function () {
        let result1 = process.uptime()
        function sleep(d){
            while(process.uptime() - result1 <= d);
        }
        sleep(7);
        let result2 = process.uptime()
        var flag = false
        if ((result2 - result1) >= 7)
        {
            flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testUptime005
     * @tc.desc: Returns the running time of the system.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testUptime005', 0, function () {
        let result1 = process.uptime()
        function sleep(d){
            while(process.uptime() - result1 <= d);
        }
        sleep(11);
        let result2 = process.uptime()
        var flag = false
        if ((result2 - result1) >= 11)
        {
            flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testPid001
     * @tc.desc: Return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid001', 0, function () {
        let result = process.pid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testPid002
     * @tc.desc: Return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid002', 0, function () {
        for(let i = 0; i < 3; i++)
        {
            let result = process.pid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPid003
     * @tc.desc: Return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid003', 0, function () {
        for(let i = 0; i < 5; i++)
        {
            let result = process.pid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPid004
     * @tc.desc: Return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid004', 0, function () {
        for(let i = 0; i < 6; i++)
        {
            let result = process.pid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPid005
     * @tc.desc: Return pid is the pid of the current process.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPid005', 0, function () {
        for(let i = 0; i < 8; i++)
        {
            let result = process.pid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPpid001
     * @tc.desc: Return ppid is the pid of the current child process
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid001', 0, function () {
        let result = process.ppid
        if(result > 0) {
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testPpid002
     * @tc.desc: Return ppid is the pid of the current child process
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid002', 0, function () {
        for(let i = 0; i < 3; i++)
        {
            let result = process.ppid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPpid003
     * @tc.desc: Return ppid is the pid of the current child process
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid003', 0, function () {
        for(let i = 0; i < 5; i++)
        {
            let result = process.ppid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPpid004
     * @tc.desc: Return ppid is the pid of the current child process
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid004', 0, function () {
        for(let i = 0; i < 6; i++)
        {
            let result = process.ppid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testPpid005
     * @tc.desc: Return ppid is the pid of the current child process
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testPpid005', 0, function () {
        for(let i = 0; i < 8; i++)
        {
            let result = process.ppid
            if(result > 0) {
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testCwd001
     * @tc.desc: Return the current work directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testCwd001', 0, function () {
        let cwdir = process.cwd()
        if(cwdir){
            var flag = true
        }
        expect(flag).assertEqual(true)
    })

    /**
     * @tc.name: testCwd002
     * @tc.desc: Return the current work directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testCwd002', 0, function () {
        for(let i = 0; i < 3; i++)
        {
            let cwdir = process.cwd()
            if(cwdir){
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testCwd003
     * @tc.desc: Return the current work directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testCwd003', 0, function () {
        for(let i = 0; i < 5; i++)
        {
            let cwdir = process.cwd()
            if(cwdir){
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testCwd004
     * @tc.desc: Return the current work directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testCwd004', 0, function () {
        for(let i = 0; i < 6; i++)
        {
            let cwdir = process.cwd()
            if(cwdir){
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testCwd005
     * @tc.desc: Return the current work directory.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testCwd005', 0, function () {
        for(let i = 0; i < 8; i++)
        {
            let cwdir = process.cwd()
            if(cwdir){
                var flag = true
            }
            expect(flag).assertEqual(true)
        }
    })

    /**
     * @tc.name: testOn001
     * @tc.desc: Register for an event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOn001', 0, function () {
        function add(){
            let value = 3 + 2
        }
        let ontest = process.on(1, add)
        expect(ontest).assertEqual(false)
    })

    /**
     * @tc.name: testOn002
     * @tc.desc: Register for an event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOn002', 0, function () {
        function add1(num){
            let value = num + 3
        }
        let on = process.on("add123", add1)
    })

    /**
     * @tc.name: testOn003
     * @tc.desc: Register for an event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOn003', 0, function () {
        function add2(num0, num1){
            let value = num0 + num1
        }
        let ontest = process.on(879, add2)
        expect(ontest).assertEqual(false)
    })

    /**
     * @tc.name: testOn004
     * @tc.desc: Register for an event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOn004', 0, function () {
        function add3(num0, num1){
            let value = num0 + num1
        }
        let ontest = process.on(12345, add3)
        expect(ontest).assertEqual(false)
    })

    /**
     * @tc.name: testOn005
     * @tc.desc: Register for an event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOn005', 0, function () {
        function add4(num0, num1){
            let value = num0 + num1
        }
        let ontest = process.on(1000, add4)
        expect(ontest).assertEqual(false)
    })

    /**
     * @tc.name: testOff001
     * @tc.desc: Remove registered event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOff001', 0, function () {
        function add1(num){
            let value = num + 3
        }
        let on = process.on("UnHandleRejection", add1)
        let offtest = process.off("UnHandleRejection")
        expect(offtest).assertEqual(true)
    })

    /**
     * @tc.name: testOff002
     * @tc.desc: Remove registered event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOff002', 0, function () {
        let offtest = process.off("123")
        expect(offtest).assertEqual(false)
    })

    /**
     * @tc.name: testOff003
     * @tc.desc: Remove registered event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOff003', 0, function () {
        let offtest = process.off("456")
        expect(offtest).assertEqual(false)
    })

    /**
     * @tc.name: testOff004
     * @tc.desc: Remove registered event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOff004', 0, function () {
        function add1(num){
            let value = num + 3
        }
        let on = process.on("UnHandleRejection", add1)
        let offtest = process.off("UnHandleRejection")
        expect(offtest).assertEqual(true)
    })

    /**
     * @tc.name: testOff005
     * @tc.desc: Remove registered event.
     * @tc.require: AR000GFB2S
     * @tc.author: wangben
     */
    it('testOff005', 0, function () {
        function add1(num){
            let value = num + 3
        }
        let on = process.on("UnHandleRejection", add1)
        let offtest = process.off("UnHandleRejection")
        expect(offtest).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-001
     * @tc.desc: Returns the tid of the current thread.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-001', 0, function () {
        let pres = process.tid
        if(pres > 0)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-002
     * @tc.desc: Returns the tid of the current thread.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-002', 0, function () {
        for(let i=0; i < 5; i++)
        {
            let pres = process.tid
            if(pres > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-003
     * @tc.desc: Returns the tid of the current thread.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-003', 0, function () {
        for(let i=0; i < 3; i++)
        {
            let pres = process.tid
            if(pres > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })


    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-004
     * @tc.desc: Returns a boolean whether the process is isolated.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-004', 0, function () {
        let pres = process.isIsolatedProcess()
        expect(pres).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-005
     * @tc.desc: Returns a boolean whether the process is isolated.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-005', 0, function () {
        for(let i=0; i < 3; i++)
        {
            let pres =process.isIsolatedProcess()
            expect(pres).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-006
     * @tc.desc: Returns a boolean whether the process is isolated.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-006', 0, function () {
        for(let i=0; i < 5; i++)
        {
            let pres =process.isIsolatedProcess()
            expect(pres).assertEqual(true)
        }
    })


    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-007
     * @tc.desc: Returns a boolean whether the specified uid belongs to a particular application.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-007', 0, function () {
        let isorno = process.isAppUid(167)
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-008
     * @tc.desc: Returns a boolean whether the specified uid belongs to a particular application.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-008', 0, function () {
        let isorno = process.isAppUid(123)
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: testIsappuid003
     * @tc.desc: Returns a boolean whether the specified uid belongs to a particular application.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testIsappuid003', 0, function () {
        let isorno = process.isAppUid(80000)
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: testIsappuid004
     * @tc.desc: Returns a boolean whether the specified uid belongs to a particular application.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testIsappuid004', 0, function () {
        let isorno = process.isAppUid(789)
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: testIsappuid005
     * @tc.desc: Returns a boolean whether the specified uid belongs to a particular application.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testIsappuid005', 0, function () {
        let isorno = process.isAppUid(8569)
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-009
     * @tc.desc: Returns a boolean whether the process is running in a 64-bit environment.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-009', 0, function () {
        let isorno = process.is64Bit()
        expect(isorno).assertEqual(false)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-010
     * @tc.desc: Returns a boolean whether the process is running in a 64-bit environment.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-010', 0, function () {
        for(let i=0; i<3; i++)
        {
            let isorno = process.is64Bit()
            expect(isorno).assertEqual(false)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-011
     * @tc.desc: Returns a boolean whether the process is running in a 64-bit environment.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-011', 0, function () {
        for(let i=0; i<5; i++)
        {
            let isorno = process.is64Bit()
            expect(isorno).assertEqual(false)
        }
    })


    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-012
     * @tc.desc: Returns the uid based on the specified user name.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-012',0, function () {
        let pres = process.getUidForName("root")
        if(pres != -1)
        {
            let flag = new Boolean(true)
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-013
     * @tc.desc: Returns the uid based on the specified user name.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-013', 0, function () {
        for(let i=0; i<3; i++)
        {
            let pres = process.getUidForName("12356")
            expect(pres).assertEqual(-1)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-014
     * @tc.desc: Returns the uid based on the specified user name.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-014', 0, function () {
        for(let i=0; i<5; i++)
        {
            let pres = process.getUidForName("12356")
            expect(pres).assertEqual(-1)
        }
    })


    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-015
     * @tc.desc: Returns the thread priority based on the specified tid.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-015', 0, function () {
        let pres = process.tid
        let pri = process.getThreadPriority(pres)
        if(pri)
        {
            let flag = new Boolean(true)
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-016
     * @tc.desc: Returns the thread priority based on the specified tid.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-016', 0, function () {
        let pres = process.tid
        for(let i=0; i<3; i++)
        {
            let pri = process.getThreadPriority(pres)
            if(pri > 0)
            {
                let flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-017
     * @tc.desc: Returns the thread priority based on the specified tid.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-017', 0, function () {
        let pres = process.tid
        for(let i=0; i<5; i++)
        {
            let pri = process.getThreadPriority(pres)
            if(pri > 0)
            {
                let flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-019
     * @tc.desc: Returns the elapsed real time (in milliseconds) taken from the start of
     * @the system to the start of the process.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-019', 0, function () {
        for(let i=0; i<3; i++)
        {
            let pri = process.getStartRealtime()
            if(pri !== null)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-018
     * @tc.desc: Returns the elapsed real time (in milliseconds) taken from the start of
     * @the system to the start of the process.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-018', 0, function () {
        let pri = process.getStartRealtime()
        if(pri !== null)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-020
     * @tc.desc: Returns the elapsed real time (in milliseconds) taken from the start of
     * @the system to the start of the process.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-020', 0, function () {
        for(let i=0; i<5; i++)
        {
            let pri = process.getStartRealtime()
            if(pri !== null)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-021
     * @tc.desc: Returns the elapsed real time (in milliseconds)
     * @taken from the start of the system to the start of the process.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-021', 0, function () {
            let priOne = process.getStartRealtime()
            let priTwo = process.getStartRealtime()
            expect(priOne - priTwo).assertEqual(0)       
    })

    /**
     * @tc.name: testGetstartrealtime004
     * @tc.desc: Returns the elapsed real time (in milliseconds) taken from the start of
     * @the system to the start of the process.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testGetstartrealtime004', 0, function () {
        for(let i=0; i<6; i++)
        {
            let pri = process.getStartRealtime()
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: testGetpastCpuTime001
     * @tc.desc: Returns the cpu time (in milliseconds) from the time when the process starts to the current time.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testGetpastCpuTime001', 0, function () {
        for(let i=0; i<3; i++)
        {
            let pri = process.getPastCpuTime()
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-022
     * @tc.desc: Returns the cpu time (in milliseconds) from the time when the process starts to the current time.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-022', 0, function () {
        let pri = process.getPastCpuTime()
        if(pri > 0)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-023
     * @tc.desc: Returns the cpu time (in milliseconds) from the time when the process starts to the current time.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-023', 0, function () {
        for(let i=0; i<5; i++)
        {
            let pri = process.getPastCpuTime()
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-024
     * @tc.desc: Returns the cpu time (in milliseconds) from the time when the process starts to the current time.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-024', 0, function () {
        for(let i=0; i<8; i++)
        {
            let pri = process.getPastCpuTime()
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-025
     * @tc.desc: Returns the system configuration at runtime.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-025', 0, function () {
        let _SC_ARG_MAX = 0
        let pri = process.getSystemConfig(_SC_ARG_MAX)
        if(pri > 0)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-026
     * @tc.desc: Returns the system configuration at runtime.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-026', 0, function () {
        let _SC_ARG_MAX = 0
        for(let i=0; i<3; i++)
        {
            let pri = process.getSystemConfig(_SC_ARG_MAX)
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-027
     * @tc.desc: Returns the system configuration at runtime.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-027', 0, function () {
        let _SC_ARG_MAX = 0
        for(let i=0; i<5; i++)
        {
            let pri = process.getSystemConfig(_SC_ARG_MAX)
            if(pri > 0)
            {
                var flag = new Boolean(true)
            }
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-028
     * @tc.desc: Returns the system configuration at runtime.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-028', 0, function () {
        let _SC_OPEN_MAX = 4
        let pri = process.getSystemConfig(_SC_OPEN_MAX)
        if(pri > 0)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-029
     * @tc.desc: Returns the system configuration at runtime.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-029', 0, function () {
        let _SC_PAGESIZE = 8
        let pri = process.getSystemConfig(_SC_PAGESIZE)
        if(pri > 0)
        {
            var flag = new Boolean(true)
        }
        expect(Boolean(flag)).assertEqual(true)
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-033
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-033', 0, function () {
        let pri = process.getEnvironmentVar("USER")
        if(pri != null) {
            var flag = new Boolean(true)
            expect(Boolean(flag)).assertEqual(true)
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-036
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-036', 0, function () {
        for(let i=0; i<3; i++)
        {
            let pri = process.getEnvironmentVar("PATH")
            if(pri != null)
            {
                var flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: testGetEnvironmentVar003
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('testGetEnvironmentVar003', 0, function () {
        for(let i=0; i<5; i++)
        {
            let pri = process.getEnvironmentVar("PATH")
            if(pri != null)
            {
                var flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-034
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-034', 0, function () {
        for(let i=0; i<6; i++)
        {
            let pri = process.getEnvironmentVar("USER")
            if(pri != null)
            {
                var flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-035
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-035', 0, function () {
        for(let i=0; i<8; i++)
        {
            let pri = process.getEnvironmentVar("USER")
            if(pri != null)
            {
                var flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })

    /**
     * @tc.name: SUB_Runtime_JSAPI_SR000GGR3G-037
     * @tc.desc: Returns the system value for environment variables.
     * @tc.require: AR000GFB04
     * @tc.author: liwenqiang
     */
    it('SUB_Runtime_JSAPI_SR000GGR3G-037', 0, function () {
        for(let i=0; i<100; i++)
        {
            let pri = process.getEnvironmentVar("i123")
            if(pri == null)
            {
                var flag = new Boolean(true)
                expect(Boolean(flag)).assertEqual(true)
            }
        }
    })
})