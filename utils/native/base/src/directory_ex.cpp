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

#include "directory_ex.h"
#include <dirent.h>
#include <errno.h>
#include "securec.h"
#include "unistd.h"
#include "utils_log.h"
using namespace std;

namespace OHOS {

string GetCurrentProcFullFileName()
{
    char procFile[PATH_MAX + 1] = {0};
    int ret = readlink("/proc/self/exe", procFile, PATH_MAX);
    if (ret < 0 || ret > PATH_MAX) {
        UTILS_LOGE("Get proc name failed, ret is: %{public}d!", ret);
        return string();
    }
    procFile[ret] = '\0';
    return string(procFile);
}

string GetCurrentProcPath()
{
    return ExtractFilePath(GetCurrentProcFullFileName());
}

string ExtractFilePath(const string& fileFullName)
{
    return string(fileFullName).substr(0, fileFullName.rfind("/") + 1);
}

std::string ExtractFileName(const std::string& fileFullName)
{
    return string(fileFullName).substr(fileFullName.rfind("/") + 1, fileFullName.size());
}

string ExtractFileExt(const string& fileName)
{
    string::size_type pos = fileName.rfind(".");
    if (pos == string::npos) {
        return "";
    }

    return string(fileName).substr(pos + 1, fileName.size());
}

string ExcludeTrailingPathDelimiter(const std::string& path)
{
    if (path.rfind("/") != path.size() - 1) {
        return path;
    }

    if (!path.empty()) {
        return path.substr(0, (int)path.size() - 1);
    }

    return path;
}

string IncludeTrailingPathDelimiter(const std::string& path)
{
    if (path.rfind("/") != path.size() - 1) {
        return path + "/";
    }

    return path;
}

void GetDirFiles(const string& path, vector<string>& files)
{
    string pathStringWithDelimiter;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return;
    }

    while (true) {
        struct dirent *ptr = readdir(dir);
        if (ptr == nullptr) {
            break;
        }

        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } else if (ptr->d_type == DT_DIR) {
            pathStringWithDelimiter = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
            GetDirFiles(pathStringWithDelimiter, files);
        } else {
            files.push_back(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
        }
    }
    closedir(dir);
}

bool ForceCreateDirectory(const string& path)
{
    string::size_type index = 0;
    do {
        string subPath;
        index = path.find('/', index + 1);
        if (index == string::npos) {
            subPath = path;
        } else {
            subPath = path.substr(0, index);
        }

        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) != 0 && errno != EEXIST) {
                return false;
            }
        }
    } while (index != string::npos);

    return access(path.c_str(), F_OK) == 0;
}

bool ForceRemoveDirectory(const string& path)
{
    string subPath;
    bool ret = true;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }

    while (true) {
        struct dirent *ptr = readdir(dir);
        if (ptr == nullptr) {
            break;
        }

        // current dir OR parent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        subPath = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
        if (ptr->d_type == DT_DIR) {
            ret = ForceRemoveDirectory(subPath);
        } else {
            if (access(subPath.c_str(), F_OK) == 0) {
                if (remove(subPath.c_str()) != 0) {
                    closedir(dir);
                    return false;
                }
            }
        }
    }
    closedir(dir);

    string currentPath = ExcludeTrailingPathDelimiter(path);
    if (access(currentPath.c_str(), F_OK) == 0) {
        if (remove(currentPath.c_str()) != 0) {
            return false;
        }
    }

    return ret && (access(path.c_str(), F_OK) != 0);
}

bool RemoveFile(const string& fileName)
{
    if (access(fileName.c_str(), F_OK) == 0) {
        return remove(fileName.c_str()) == 0;
    }

    return true;
}

bool IsEmptyFolder(const string& path)
{
    vector<string> files;
    GetDirFiles(path, files);
    return files.empty();
}

uint64_t GetFolderSize(const string& path)
{
    vector<string> files;
    struct stat statbuf = {0};
    GetDirFiles(path, files);
    uint64_t totalSize = 0;
    for (auto& file : files) {
        if (stat(file.c_str(), &statbuf) == 0) {
            totalSize += statbuf.st_size;
        }
    }

    return totalSize;
}

// inner function, and param is legitimate
bool ChangeMode(const string& fileName, const mode_t& mode)
{
    return (chmod(fileName.c_str(), mode) == 0);
}

bool ChangeModeFile(const string& fileName, const mode_t& mode)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        return false;
    }

    return ChangeMode(fileName, mode);
}

bool ChangeModeDirectory(const string& path, const mode_t& mode)
{
    string subPath;
    bool ret = true;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }

    while (true) {
        struct dirent *ptr = readdir(dir);
        if (ptr == nullptr) {
            break;
        }

        // current dir OR parent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        subPath = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
        if (ptr->d_type == DT_DIR) {
            ret = ChangeModeDirectory(subPath, mode);
        } else {
            if (access(subPath.c_str(), F_OK) == 0) {
                if (!ChangeMode(subPath, mode)) {
                    UTILS_LOGE("Failed to exec ChangeMode");
                    closedir(dir);
                    return false;
                }
            }
        }
    }
    closedir(dir);
    string currentPath = ExcludeTrailingPathDelimiter(path);
    if (access(currentPath.c_str(), F_OK) == 0) {
        if (!ChangeMode(currentPath, mode)) {
            UTILS_LOGE("Failed to exec ChangeMode");
            return false;
        }
    }
    return ret;
}

bool PathToRealPath(const string& path, string& realPath)
{
    if (path.empty()) {
        UTILS_LOGE("path is empty!");
        return false;
    }

    if ((path.length() >= PATH_MAX)) {
        UTILS_LOGE("path len is error, the len is: [%{public}zu]", path.length());
        return false;
    }

    char tmpPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), tmpPath) == nullptr) {
        UTILS_LOGE("path to realpath error");
        return false;
    }

    realPath = tmpPath;
    if (access(realPath.c_str(), F_OK) != 0) {
        UTILS_LOGE("check realpath (%{private}s) error", realPath.c_str());
        return false;
    }
    return true;
}

} // OHOS
