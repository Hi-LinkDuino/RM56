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
#include "event_logger_catcher_test.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>

#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "securec.h"

#include "common_utils.h"
#include "file_util.h"

#include "event_log_task.h"
#include "event_logger.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void EventloggerCatcherTest::SetUp()
{
    /**
     * @tc.setup: create an event loop and multiple event handlers
     */
    printf("SetUp.\n");
    printf("path_ is %s\n", path_.c_str());

    sleep(1);
    isSelinuxEnabled_ = false;
    char buffer[BUF_SIZE_64] = {'\0'};
    FILE* fp = popen("getenforce", "r");
    if (fp != nullptr) {
        fgets(buffer, sizeof(buffer), fp);
        std::string str = buffer;
        printf("buffer is %s\n", str.c_str());
        if (str.find("Enforcing") != str.npos) {
            printf("Enforcing %s\n", str.c_str());
            isSelinuxEnabled_ = true;
        } else {
            printf("This isn't Enforcing %s\n", str.c_str());
        }
        pclose(fp);
    } else {
        printf("fp == nullptr\n");
    }
    system("setenforce 0");

    constexpr mode_t defaultLogDirMode = 0770;
    if (!FileUtil::FileExists(path_)) {
        FileUtil::ForceCreateDirectory(path_);
        FileUtil::ChangeModeDirectory(path_, defaultLogDirMode);
    }
}

void EventloggerCatcherTest::TearDown()
{
    /**
     * @tc.teardown: destroy the event loop we have created
     */
    if (isSelinuxEnabled_) {
        system("setenforce 1");
        isSelinuxEnabled_ = false;
    }

    printf("TearDown.\n");
}

int EventloggerCatcherTest::GetFdSize(int32_t fd)
{
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        return 0;
    }
    return fileStat.st_size;
}

std::string EventloggerCatcherTest::GetFormatTime(unsigned long timestamp)
{
    struct tm tm;
    time_t ts;
    /* 20: the length of 'YYYYmmddHHMMSS' */
    int strLen = 20;
    ts = timestamp;
    localtime_r(&ts, &tm);
    char buf[strLen];

    (void)memset_s(buf, strLen, 0, strLen);
    strftime(buf, strLen - 1, "%Y%m%d%H%M%S", &tm);
    return std::string(buf, strlen(buf));
}

int EventloggerCatcherTest::JudgmentsFileSize(int minQuantity, const std::string sender)
{
    constexpr mode_t defaultLogFileMode = 0644;
    std::string logPath = path_ + "/" + logFile_;
    auto fd2 = open(logPath.c_str(), O_RDONLY, defaultLogFileMode);
    if (fd2 < 0) {
        printf("second, Fail to create %s. fd2 == %d\n", logFile_.c_str(), fd2);
        return RETURN_OPEN_FAIL2;
    }
    auto size = GetFdSize(fd2);
    printf("%s, file size %d\n", sender.c_str(), size);
    if (size < minQuantity) {
        printf("error %s, less than size %d\n", sender.c_str(), minQuantity);
        close(fd2);
        return RETURN_LESS_THAN_SIZE;
    }
    return fd2;
}

int EventloggerCatcherTest::StartCreate(const std::string sender, const std::string name, const std::string action,
    int pid, const std::string packageName, int interval, int minQuantity)
{
    auto eventlogger = std::make_unique<EventLogger>();
    std::string jsonStr = R"~({"domain_":"demo","name_":")~" + name + R"~(","pid_":)~" +
        std::to_string(pid) + R"~(,"tid_":6527,"PACKAGE_NAME":")~" + packageName + R"~("})~";
    auto event = std::make_shared<SysEvent>("sender", nullptr, jsonStr);
    event->PaserJson();
    std::time_t timeTmp = 0;
    time(&timeTmp);
    event->happenTime_ = timeTmp;
    event->SetValue("eventLog_action", action);
    event->SetValue("eventLog_interval", interval);

    printf("pid is %d, pid_ %d; packageName is %s, PACKAGE_NAME %s\n",
        pid, event->GetPid(), packageName.c_str(), event->GetEventValue("PACKAGE_NAME").c_str());

    std::string idStr = event->eventName_;
    logFile_ = idStr + "-" + GetFormatTime(event->happenTime_) + ".log";

    constexpr mode_t defaultLogFileMode = 0644;
    std::string logPath = path_ + "/" + logFile_;
    printf("logPath is %s\n", logPath.c_str());
    auto fd = open(logPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, defaultLogFileMode);
    if (fd < 0) {
        printf("Fail to create %s. fd == %d\n", logFile_.c_str(), fd);
        return RETURN_OPEN_FAIL;
    }

    auto logTask = std::make_unique<EventLogTask>(fd, event);
    logTask->AddLog(event->GetValue("eventLog_action"));
    auto ret = logTask->StartCompose();
    if (ret != EventLogTask::TASK_SUCCESS) {
        printf("capture fail %d", ret);
        close(fd);
        return RETURN_TASK_NO_SUCCESS;
    }
    close(fd);

    printf("logTask LogSize is %ld\n", logTask->GetLogSize());
    if (logTask->GetLogSize() < minQuantity) {
        printf("error LogSize less than %d\n", minQuantity);
        return RETURN_TASK_LESS_THAN_SIZE;
    }
    return 0;
}

/**
 * @tc.name: EventloggerCatcherTest001
 * @tc.desc: parse a correct config file and check result
 * @tc.type: FUNC
 * @tc.require: AR000FT62O
 */
HWTEST_F(EventloggerCatcherTest, EventloggerCatcherTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    constexpr int minQuantity = 8000;
    auto ret = StartCreate("EventloggerCatcherTest001", "TEST01_SYS_STACKTRACE",
        "S", -1, "", 0, minQuantity);
    if (ret < 0) {
        printf("EventloggerCatcherTest001 StartCreate is error ret == %d\n", ret);
        FAIL();
    }

    auto fd = JudgmentsFileSize(minQuantity, "EventloggerCatcherTest001");
    if (fd < 0) {
        printf("EventloggerCatcherTest001 JudgmentsFileSize is error ret == %d\n", fd);
        FAIL();
    }

    char readTmp[BUF_SIZE_256];
    ret = -1;
    while (read(fd, readTmp, BUF_SIZE_256)) {
        std::string tmp = readTmp;
        if (tmp.find("system/bin") != tmp.npos) {
            ret = 0;
            break;
        }
    }

    if (ret < 0) {
        printf("not find Key Messages\n");
        close(fd);
        FAIL();
    }
    close(fd);
}

/**
 * @tc.name: EventloggerCatcherTest002
 * @tc.desc: parse a correct config file and check result
 * @tc.type: FUNC
 * @tc.require: AR000FT62O
 */
HWTEST_F(EventloggerCatcherTest, EventloggerCatcherTest002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    int pid = -1;
    constexpr int upperLimit = 20000000; // 20000000us
    if ((pid = fork()) < 0) {
        printf("Fork error, err:%d", errno);
        FAIL();
    }

    // Creating a process with high usage
    if (pid == 0) {
        prctl(PR_SET_NAME, "EventloggerCatcherTest002_Child");
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        while (true) {
            int volatile i = 0;
            while (i < upperLimit) {
                i++;
            }
        }
    }

    constexpr int minQuantity = 500;
    auto ret = StartCreate("EventloggerCatcherTest002", "TEST02_CPU", "c",
        pid, "EventloggerCatcherTest002_Child", 0, minQuantity);
    if (ret < 0) {
        printf("EventloggerCatcherTest002 StartCreate is error ret == %d\n", ret);
        FAIL();
    }

    auto fd = JudgmentsFileSize(minQuantity, "EventloggerCatcherTest002");
    if (fd < 0) {
        printf("EventloggerCatcherTest002 JudgmentsFileSize is error ret == %d\n", fd);
        FAIL();
    }

    char readTmp[BUF_SIZE_256];
    ret = -1;
    while (read(fd, readTmp, BUF_SIZE_256)) {
        std::string tmp = readTmp;
        if (tmp.find("CpuUtilizationCatcher") != tmp.npos) {
            ret = 0;
            break;
        }
    }

    if (ret < 0) {
        printf("not find Key Messages\n");
        close(fd);
        FAIL();
    }
    close(fd);
}

/**
 * @tc.name: EventloggerCatcherTest003
 * @tc.desc: parse a correct config file and check result
 * @tc.type: FUNC
 * @tc.require: AR000FT62O
 */
HWTEST_F(EventloggerCatcherTest, EventloggerCatcherTest003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    int pid = -1;
    const int memSize = 1024*3;
    if ((pid = fork()) < 0) {
        printf("Fork error, err:%d", errno);
        FAIL();
    }

    // Creating a process with high memory
    if (pid == 0) {
        prctl(PR_SET_NAME, "EventloggerCatcherTest003_Child");
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        int volatile temp[memSize] = {0};
        while (true) {
            int i = 0;
            while (i < memSize) {
                i++;
                temp[i] = i & 0x234567;
            }
        }
    }

    constexpr int minQuantity = 500;
    auto ret = StartCreate("EventloggerCatcherTest003", "TEST03_MEM", "m",
        pid, "EventloggerCatcherTest003_Child", 0, minQuantity);
    if (ret < 0) {
        printf("EventloggerCatcherTest003 is error ret == %d\n", ret);
        FAIL();
    }

    auto fd = JudgmentsFileSize(minQuantity, "EventloggerCatcherTest003");
    if (fd < 0) {
        printf("EventloggerCatcherTest003 JudgmentsFileSize is error ret == %d\n", fd);
        FAIL();
    }

    char readTmp[BUF_SIZE_256];
    ret = -1;
    while (read(fd, readTmp, BUF_SIZE_256)) {
        std::string tmp = readTmp;
        if (tmp.find("MemTotal") != tmp.npos) {
            ret = 0;
            break;
        }
    }

    if (ret < 0) {
        printf("not find Key Messages\n");
        close(fd);
        FAIL();
    }
    close(fd);
}
