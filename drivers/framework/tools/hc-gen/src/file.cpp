/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "file.h"
#include <climits>
#include <cstdlib>
#include "types.h"

using namespace OHOS::Hardware::Util;

std::string File::AbsPath(const std::string &path)
{
    char realPath[PATH_MAX];
#ifdef MINGW32
    char *p = _fullpath(realPath, path.data(), PATH_MAX);
    if (p != nullptr && access(p, F_OK) != 0) {
        p = nullptr;
    }
#else
    char *p = realpath(path.data(), realPath);
#endif
    return p == nullptr ? "" : p;
}

std::string File::StripSuffix(std::string path)
{
    auto sepPos = path.rfind(OS_SEPARATOR);
    auto dotPos = path.rfind('.');
    if (sepPos == std::string::npos || dotPos > sepPos) {
        return path.substr(0, dotPos);
    } else {
        return path;
    }
}

std::string File::GetDir(std::string path)
{
    auto separatorPos = path.rfind(OS_SEPARATOR);
    if (separatorPos == std::string::npos) {
        return path;
    }
    return path.substr(0, separatorPos + 1);
}

std::string File::FileNameBase(const std::string &path)
{
    auto sepPos = path.rfind(OS_SEPARATOR);
    auto dotPos = path.rfind('.');

    if (sepPos == std::string::npos) {
        sepPos = 0;
    } else {
        sepPos++;
    }

    if (dotPos == std::string::npos || dotPos < sepPos) {
        dotPos = path.size();
    }

    auto len = path.size() - 1;
    if (dotPos != std::string::npos && dotPos > sepPos) {
        len = dotPos - sepPos;
    }
    return path.substr(sepPos, len);
}
