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

#ifndef LOG_TIME_STAMP_H
#define LOG_TIME_STAMP_H

#define NS_PER_SEC 1000000000ULL
class LogTimeStamp {

public:
    LogTimeStamp() = default;
    ~LogTimeStamp() = default;
#ifdef __linux__
    explicit LogTimeStamp(clockid_t id)
    {
        timespec time;
        clock_gettime(id, &time);
        tv_sec = static_cast<uint32_t>(time.tv_sec);
        tv_nsec = static_cast<uint32_t>(time.tv_nsec);
    }
#endif
    explicit LogTimeStamp(const timespec& time)
        : tv_sec(static_cast<uint32_t>(time.tv_sec)),
        tv_nsec(static_cast<uint32_t>(time.tv_nsec)) {}
    explicit LogTimeStamp(uint32_t sec, uint32_t nsec = 0)
        : tv_sec(sec), tv_nsec(nsec) {}
    /* LogTimeStamp */
    bool operator == (const LogTimeStamp& time) const
    {
        return (tv_sec == time.tv_sec) && (tv_nsec == time.tv_nsec);
    }
    bool operator != (const LogTimeStamp& time) const
    {
        return !(*this == time);
    }
    bool operator < (const LogTimeStamp& time) const
    {
        return (tv_sec < time.tv_sec) ||
        ((tv_sec == time.tv_sec) && (tv_nsec < time.tv_nsec));
    }
    bool operator >= (const LogTimeStamp& time) const
    {
        return !(*this < time);
    }
    bool operator > (const LogTimeStamp& time) const
    {
        return (tv_sec > time.tv_sec) ||
        ((tv_sec == time.tv_sec) && (tv_nsec > time.tv_nsec));
    }
    bool operator <= (const LogTimeStamp& time) const
    {
        return !(*this > time);
    }
    LogTimeStamp operator -= (const LogTimeStamp& time)
    {
        if (*this <= time) {
            return *this = LogTimeStamp(epoch);
        }
        if (this->tv_nsec < time.tv_nsec) {
            --this->tv_sec;
            this->tv_nsec = NS_PER_SEC + this->tv_nsec - time.tv_nsec;
        } else {
            this->tv_nsec -= time.tv_nsec;
        }
        this->tv_sec -= time.tv_sec;
        return *this;
    }
    LogTimeStamp operator += (const LogTimeStamp& time)
    {
        this->tv_nsec += time.tv_nsec;
        if (this->tv_nsec >= NS_PER_SEC) {
            this->tv_nsec -= NS_PER_SEC;
            ++this->tv_sec;
        }
        this->tv_sec += time.tv_sec;
        return *this;
    }
    void SetTimeStamp(uint32_t sec, uint32_t nsec)
    {
        this->tv_sec = sec;
        this->tv_nsec = nsec;
    }
    static constexpr timespec epoch = {0, 0};
    uint32_t tv_sec = 0;
    uint32_t tv_nsec = 0;
};

#endif
