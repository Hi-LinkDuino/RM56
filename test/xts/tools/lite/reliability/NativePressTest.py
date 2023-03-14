#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import sys
from utils import *
import logging

hdcPath = "D:\\tools\\hdc\\hdc_std.exe"
testFilePath = "D:\\test\\testcase\\"
testFile = ["ActsMediaCppStandardTest", "faultloggertest", "hilogtest", "hipluginmoduleapitest", "HiSysEventCPPTest",
            "LibhilogCPPtest", "libhilogCtest", "ZoneUtiltest"]
testPID = []
filenames = []
testTime = 0.1 * 360
processNum = 5


def EnvInit():
    test_fileset = ""
    for test_fileset in testFile:
        exec_cmd(hdcPath + " file send " + testFilePath + test_fileset + " /data/local/tmp/" + test_fileset)
        time.sleep(3)
    exec_cmd(hdcPath + " shell chmod +x /data/local/tmp/" + test_fileset)


def PressTestProcess(testCmd="", testFilename=""):
    print("Start to PressTest Process with cmd " + testCmd)
    for i in range(processNum):
        exec_cmd(testCmd)
    time.sleep(3)
    GetPidOfProcess(testFilename)


def GetPidOfProcess(processName=""):
    cmdRet = exec_cmd(hdcPath + " shell \"ps -elf | grep "
                      + processName + "| grep -v sh" + "| grep -v grep\"", waitOut=True)
    print("cmdRet is :" + cmdRet)
    splitlines = cmdRet.strip('\n\t\r').split("\n")
    for line in splitlines:
        print("Start go get pid with line:" + line)
        if len(line.split()) >= 1:
            print("Start go get pid:" + line)
            testPID.append(line.split()[1])
    for pid in testPID:
        print("PID of each test process is:" + pid)


def PressTestCheck():
    print("Start to PressTestCheck")
    for pid in testPID:
        print("PID of each test process is:" + pid)
        cmdRet = exec_cmd(hdcPath + " shell \"cat /proc/" + pid + "/cmdline\"", waitOut=True)
        if cmdRet.find(testFile) == -1:
            print("PID " + str(pid) + " not exist,will remove it")
            testPID.remove(pid)
        else:
            print("cmdRet is " + cmdRet)
            print("PID " + str(pid) + " still exist")


def ProcessTestEnd():
    for pid in testPID:
        print("Process with pid " + pid + "will be killed.")
        cmdRet = exec_cmd(hdcPath + " shell " + " kill " + pid, waitOut=True)
        print(cmdRet)


def ProcessTestResultCheck(testScriptPath):
    print("Now we will check test result.")
    exec_cmd("md " + testScriptPath + "\\faultlog")
    cmdRet = exec_cmd(hdcPath + " shell \"ls -l /data/log/faultlog/temp/\" ", waitOut=True)
    splitlines = cmdRet.strip('\n\t\r').split("\n")
    for line in splitlines:
        print("Get exception file with line " + line)
        splitlines = line.split()
        if len(splitlines) >= 8:
            filenames.append(splitlines[7])
    for filename in filenames:
        print("Get exception file " + filename)
        print("cmd is " + hdcPath + "  file recv /data/log/faultlog/temp/" +
              filename + " " + testScriptPath + "\\faultlog\\")
        cmdRet = exec_cmd(hdcPath + "  file recv /data/log/faultlog/temp/" +
                          filename + " " + testScriptPath + "\\faultlog\\")
        print("file recv result:" + str(cmdRet))
    if len(filenames) > 0:
        return False
    return True


#######################################################


if __name__ == "__main__":
    logging.info("------------------------NEW TEST---------------------------")
    print("abs path is %s" % (os.path.split(os.path.realpath(__file__))[0]))
    testScriptPath = os.path.split(os.path.realpath(__file__))[0]
    testFileset = ""
    EnvInit()
    start = datetime.datetime.now()
    now = datetime.datetime.now()
    while (now - start).seconds < int(testTime):
        print("Now is " + str((now - start).seconds))
        now = datetime.datetime.now()
        for testFileset in testFile:
            startTestCmd = "\"cd /data/local/tmp/;/data/local/tmp/" + \
                           testFileset + " > /dev/null \""
            PressTestProcess("start " + hdcPath + " shell " + startTestCmd, testFileset)
            PressTestCheck()
            while len(testPID) > 0:
                time.sleep(1)
                PressTestCheck()
    ProcessTestEnd()
    if not ProcessTestResultCheck(testScriptPath):
        print("Exception found after Test,please check faultlog path")
        raise Exception("Exception found after Test,please check faultlog path")
    logging.info("------------------------TEST END---------------------------")
    # main()
