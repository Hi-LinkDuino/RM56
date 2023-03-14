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

#include "file_util.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#ifdef __gnu_linux__
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
#include <fstream>
#include <iostream>
#include <iterator>
namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
using namespace std;
#ifdef __gnu_linux__
namespace fs = experimental::filesystem;
#else
namespace fs = std::filesystem;
constexpr int PATH_MAX = 4096;
#endif
constexpr int MODE_MAX = 07777;
constexpr int MAX_FILE_LENGTH = 32 * 1024 * 1024; // 32MB
bool LoadStringFromFile(const string& filePath, string& content)
{
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, ios::end);
    const long fileLength = static_cast<long>(file.tellg());
    if (fileLength > MAX_FILE_LENGTH) {
        return false;
    }

    content.clear();
    file.seekg(0, ios::beg);
    copy(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), back_inserter(content));
    return true;
}

bool LoadBufferFromNodeFile(const string& filePath, vector<char>& content)
{
    string realPath;
    if (!PathToRealPath(filePath, realPath)) {
        return false;
    }

    FILE* fp = fopen(realPath.c_str(), "r");
    if (fp == nullptr) {
        return false;
    }

    char ch = fgetc(fp);
    int byteCount = 1;
    while (!feof(fp)) {
        if (byteCount > MAX_FILE_LENGTH) {
            fclose(fp);
            fp = nullptr;
            content.clear();
            return false;
        }

        content.push_back(ch);
        ch = fgetc(fp);
        byteCount++;
    }

    fclose(fp);
    fp = nullptr;
    return true;
}

bool LoadBufferFromFile(const std::string& filePath, std::vector<char>& content)
{
    ifstream file;
    file.open(filePath.c_str(), ios::in | ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    const long fileLength = static_cast<long>(file.tellg());
    if (fileLength > MAX_FILE_LENGTH) {
        return false;
    }

    // lseek is not support the linux file node
    if (fileLength < 0) {
        return LoadBufferFromNodeFile(filePath, content);
    }

    if (fileLength == 0) {
        content.clear();
        return true;
    }

    file.seekg(0, std::ios::beg);
    if (file.fail()) {
        return false;
    }

    content.resize(fileLength);
    file.read(&content[0], fileLength);
    return true;
}

bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated)
{
    if (content.empty()) {
        return true;
    }

    ofstream file;
    if (truncated) {
        file.open(filePath.c_str(), ios::out | ios::trunc);
    } else {
        file.open(filePath.c_str(), ios::out | ios::app);
    }

    if (!file.is_open()) {
        return false;
    }

    file.write(content.c_str(), content.length());
    if (file.fail()) {
        return false;
    }

    return true;
}

bool SaveBufferToFile(const string& filePath, const vector<char>& content, bool truncated)
{
    // if the file is not exist,create it first!

    ios_base::openmode mode = truncated ? (ios::out | ios::binary | ios::trunc) : (ios::out | ios::binary | ios::app);
    ofstream file;
    file.open(filePath.c_str(), mode);
    if (!file.is_open()) {
        return false;
    }

    if (content.empty()) {
        return true;
    }

    file.write(&content[0], content.size());
    return true;
}

bool FileExists(const std::string& fileName)
{
    error_code ec;
    bool ret = fs::exists(fileName, ec); // noexcept
    if (ec) {
        printf("%s\n", ec.message().c_str());
    }
    return ret;
}

std::string ExtractFilePath(const std::string& fileFullName)
{
    return fs::path(fileFullName).parent_path().string();
}

std::string ExtractFileName(const std::string& fileFullName)
{
    return fs::path(fileFullName).filename().string();
}

void GetDirFiles(const std::string& path, std::vector<std::string>& files)
{
    files.clear();
    for (const auto& p : fs::directory_iterator(path)) {
        files.push_back(p.path().string());
    }
    return;
}


bool ForceCreateDirectory(const std::string& path)
{
    error_code ec;
    if (fs::exists(path, ec)) {
        return true;
    }
    if (!fs::create_directories(path, ec)) {
        return false;
    }
    return fs::exists(path, ec);
}

bool ForceCreateDirectory(const std::string& path, int mode)
{
    if (!ForceCreateDirectory(path)) {
        return false;
    }
    return ChangeModeDirectory(path, mode);
}

bool ForceRemoveDirectory(const std::string& path, bool isNeedDeleteGivenDirSelf)
{
    error_code ec;
    fs::remove_all(path, ec);
    return true;
}

/* is file remove successed return true, is file not exist return false */
bool RemoveFile(const std::string& fileName)
{
    error_code ec;
    return fs::remove(fileName, ec);
}

uint64_t GetFileSize(const std::string& path)
{
    error_code ec;
    return fs::file_size(path, ec);
}

uint64_t GetFolderSize(const std::string& path)
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

int CreateFile(const std::string &path, mode_t mode)
{
    if (FileExists(path)) {
        return 0;
    } else {
        std::ofstream fout(path);
        if (!fout.is_open()) {
            return -1;
        }
        fout.flush();
        fout.close();
        if (ChangeMode(path, mode) != 0) {
            return -1;
        }
    }
    return 0;
}

int CopyFile(const std::string &src, const std::string &des)
{
    std::ifstream fin(src);
    std::ofstream fout(des);
    if (!fin.is_open()) {
        return -1;
    }
    if (!fout.is_open()) {
        return -1;
    }
    fout << fin.rdbuf();
    if (fout.fail()) {
        fout.clear();
    }
    fout.flush();
    return 0;
}

bool IsDirectory(const std::string &path)
{
    return fs::is_directory(path);
}

bool GetLastLine(std::istream &fin, std::string &line, uint32_t maxLen)
{
    if (fin.tellg() <= 0) {
        return false;
    } else {
        fin.seekg(-1, fin.cur);
    }
    uint32_t count = 0;
    while (fin.good() && fin.peek() == fin.widen('\n') && fin.tellg() > 0 && count < maxLen) {
        fin.seekg(-1, fin.cur);
        count++;
    }
    if (!fin.good() || count >= maxLen) {
        return false;
    }
    if (fin.tellg() == 0) {
        return true;
    }
    count = 0;
    while (fin.good() && fin.peek() != fin.widen('\n') && fin.tellg() > 0 && count < maxLen) {
        fin.seekg(-1, fin.cur);
        count++;
    }
    if (!fin.good() || count >= maxLen) {
        return false;
    }
    if (fin.tellg() != 0) {
        fin.seekg(1, fin.cur);
    }
    auto oldPos = fin.tellg();
    getline(fin, line);
    fin.seekg(oldPos);
    return true;
}

std::string GetParentDir(const std::string &path)
{
    string str = ExtractFilePath(path);
    if (str.empty()) {
        return "";
    }
    return str.substr(0, str.size() - 1);
}

// inner function, and param is legitimate
bool ChangeMode(const string& fileName, const mode_t& mode)
{
    if (!FileExists(fileName)) {
        return false;
    }
    if (mode > MODE_MAX) {
        return false;
    }
    auto perm = static_cast<fs::perms>(mode);
    fs::permissions(fileName, perm);
    return true;
}

bool ChangeModeDirectory(const string& path, const mode_t& mode)
{
    if (!FileExists(path)) {
        return false;
    }

    for (auto& p : fs::recursive_directory_iterator(path)) {
        string tmpPath = p.path().string();
        replace(tmpPath.begin(), tmpPath.end(), '\\', '/');
        if (!ChangeMode(tmpPath, mode)) {
            return false;
        }
    }
    return ChangeMode(path, mode);
}

bool PathToRealPath(const std::string& path, std::string& realPath)
{
    if (path.empty()) {
        return false;
    }

    if ((path.length() >= PATH_MAX)) {
        return false;
    }
    if (!fs::exists(path)) {
        return false;
    }
    fs::path absolutePath = fs::canonical(path);

    realPath = absolutePath.string();
#ifdef _WIN32
    FormatPath2UnixStyle(realPath);
#endif
    return true;
}
}
}
}
