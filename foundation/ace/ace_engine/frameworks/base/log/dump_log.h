/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_DUMP_LOG_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_DUMP_LOG_H

#include <cstdio>
#include <memory>
#include <streambuf>
#include <sstream>
#include <vector>

#include "base/log/log.h"
#include "base/utils/noncopyable.h"
#include "base/utils/singleton.h"

namespace OHOS::Ace {

class DumpLog : public Singleton<DumpLog> {
    DECLARE_SINGLETON(DumpLog);

public:
    class DumpFileBuf : public std::streambuf {
    public:
        DumpFileBuf(FILE* file) : file_(file) {}
        ~DumpFileBuf()
        {
            if (file_ && fclose(file_)) {
                LOGE("DumpFileBuf close file failed!");
            }
        }

        int_type overflow(int_type c) override
        {
            if (c != EOF) {
                if (fwrite(&c, 1, 1, file_) != 1) {
                    return EOF;
                }
            }
            return c;
        }

        std::streamsize xsputn(const char* str, std::streamsize num) override
        {
            return fwrite(str, 1, num, file_);
        }

    private:
        FILE* file_;
    };

    class DumpFile : public std::ostream {
    public:
        DumpFile(FILE* file) : std::ostream(0), buf_(file)
        {
            rdbuf(&buf_);
        }
    protected:
        DumpFileBuf buf_;
    };

    void SetDumpFile(DumpFile* file)
    {
        ostream_.reset(file);
    }

    void SetDumpFile(std::unique_ptr<std::ostream> file)
    {
        ostream_ = std::move(file);
    }

    const std::unique_ptr<std::ostream>& GetDumpFile() const
    {
        return ostream_;
    }

    void Print(int32_t depth, const std::string& className, int32_t childSize);
    void Print(const std::string& content);
    void Print(int32_t depth, const std::string& content);

    void Reset();

    template<typename T>
    void AddDesc(const T&& t)
    {
        std::stringstream paramStream;
        paramStream << t << "\n";
        description_.push_back(paramStream.str());
    }

    template<typename T>
    void AddDesc(const T& t)
    {
        std::stringstream paramStream;
        paramStream << t << "\n";
        description_.push_back(paramStream.str());
    }

    template<typename... Args>
    void AddDesc(Args&&... args)
    {
        std::stringstream paramStream;
        BuildDesc(paramStream, args...);
    }

    template<typename T, typename... Args>
    void BuildDesc(std::stringstream& stream, T& t, Args&&... args)
    {
        stream << t << " ";
        BuildDesc(stream, args...);
    }

    template<typename T>
    void BuildDesc(std::stringstream& stream, T& t)
    {
        stream << t << "\n";
        description_.push_back(stream.str());
    }

    static const size_t MAX_DUMP_LENGTH = 100000;

private:
    std::vector<std::string> description_;
    std::unique_ptr<std::ostream> ostream_ { nullptr };
    ACE_DISALLOW_MOVE(DumpLog);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_DUMP_LOG_H
