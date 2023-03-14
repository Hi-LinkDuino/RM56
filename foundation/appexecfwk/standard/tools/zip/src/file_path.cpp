/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "file_path.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "zip_utils.h"
#include "directory_ex.h"
#include "app_log_wrapper.h"

using namespace std;
using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
const std::string SEPARATOR = "/";
const std::string ZIP = ".zip";
const std::int32_t ZIP_SIZE = 4;
}
const FilePath::CharType FilePath::kSeparators[] = FILE_PATH_LITERAL("/");
const size_t FilePath::kSeparatorsLength = arraysize(kSeparators);
const FilePath::CharType FilePath::kCurrentDirectory[] = FILE_PATH_LITERAL(".");
const FilePath::CharType FilePath::kParentDirectory[] = FILE_PATH_LITERAL("..");
const FilePath::CharType FilePath::kExtensionSeparator = FILE_PATH_LITERAL('.');

FilePath::FilePath()
{}

FilePath::FilePath(const FilePath &that) : path_(that.path_)
{}

FilePath::FilePath(const std::string &path) : path_(path)
{}

FilePath::~FilePath()
{}

FilePath &FilePath::operator=(const FilePath &that)
{
    if (&that != this) {
        path_ = that.path_;
    }
    return *this;
}

bool FilePath::operator==(const FilePath &that) const
{
    return path_ == that.path_;
}

bool FilePath::operator!=(const FilePath &that) const
{
    return path_ != that.path_;
}

bool FilePath::operator<(const FilePath &that) const
{
    return path_ < that.path_;
}

FilePath FilePath::FromUTF8Unsafe(const std::string &utf8)
{
    return FilePath(utf8);
}

bool FilePath::ReferencesParent()
{
    std::vector<std::string> components;
    GetComponents(components);

    for (size_t i = 0; i < components.size(); i++) {
        if (components[i].find_first_not_of(FILE_PATH_LITERAL(". \n\r\t")) == std::string::npos &&
            components[i].find(kParentDirectory) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void FilePath::GetComponents(std::vector<std::string> &components)
{
    components.clear();
    if (path_.empty()) {
        return;
    }

    FilePath current = *this;
    FilePath base;
    // Capture path components.
    while (current != current.DirName()) {
        base = current.BaseName();
        if (!AreAllSeparators(base.path_))
            components.push_back(base.path_);
        current = current.DirName();
    }

    // Capture root, if any.
    base = current.BaseName();
    if (!base.path_.empty() && base.path_ != kCurrentDirectory) {
        components.push_back(current.BaseName().path_);
    }
}

FilePath FilePath::DirName()
{
    FilePath newPath(path_);
    newPath.StripTrailingSeparatorsInternal();

    std::string::size_type lastSeparator =
        newPath.path_.find_last_of(kSeparators, std::string::npos, kSeparatorsLength - 1);
    std::string::size_type zero = 0;
    std::string::size_type one = 1;
    std::string::size_type two = 2;

    if (lastSeparator == std::string::npos) {
        // path_ is in the current directory.
        newPath.path_.resize(zero);
    } else if (lastSeparator == zero) {
        // path_ is in the root directory.
        newPath.path_.resize(one);
    } else if (lastSeparator == one && IsSeparator(newPath.path_[zero])) {
        // path_ is in "//" (possibly with a drive letter); leave the double
        // separator intact indicating alternate root.
        newPath.path_.resize(two);
    } else if (lastSeparator != 0) {
        // path_ is somewhere else, trim the basename.
        newPath.path_.resize(lastSeparator);
    }

    newPath.StripTrailingSeparatorsInternal();
    if (!newPath.path_.length()) {
        newPath.path_ = kCurrentDirectory;
    }

    return newPath;
}

FilePath FilePath::BaseName()
{
    FilePath newPath(path_);
    newPath.StripTrailingSeparatorsInternal();

    // Keep everything after the final separator, but if the pathname is only
    // one character and it's a separator, leave it alone.
    std::string::size_type lastSeparator =
        newPath.path_.find_last_of(kSeparators, std::string::npos, kSeparatorsLength - 1);
    if (lastSeparator != std::string::npos && lastSeparator < newPath.path_.length() - 1) {
        newPath.path_.erase(0, lastSeparator + 1);
    }

    return newPath;
}

void FilePath::StripTrailingSeparatorsInternal()
{
    if (path_.size() == 0) {
        return;
    }
    uint32_t one = 1;
    uint32_t two = 2;
    uint32_t start = 1;
    std::string::size_type lastStripped = std::string::npos;
    for (std::string::size_type pos = path_.length(); pos > start && FilePath::IsSeparator(path_[pos - one]); --pos) {
        if (pos != start + one || lastStripped == start + two || !FilePath::IsSeparator(path_[start - one])) {
            path_.resize(pos - one);
            lastStripped = pos;
        }
    }
}

bool FilePath::AreAllSeparators(const std::string &input)
{
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if (!FilePath::IsSeparator(*it)) {
            return false;
        }
    }

    return true;
}

bool FilePath::IsAbsolute()
{
    return path_.length() > 0 && FilePath::IsSeparator(path_[0]);
}

std::string FilePath::Value()
{
    return path_;
}
bool FilePath::IsSeparator(CharType character)
{
    for (size_t i = 0; i < kSeparatorsLength - 1; ++i) {
        if (character == kSeparators[i]) {
            return true;
        }
    }
    return false;
}
bool FilePath::CreateDirectory(const FilePath &fullPath)
{
    std::vector<FilePath> subpaths;

    // Collect a list of all parent directories.
    FilePath lastPath = fullPath;
    subpaths.push_back(fullPath);
    for (FilePath path = const_cast<FilePath &>(fullPath).DirName(); path.Value() != lastPath.Value();
         path = path.DirName()) {
        subpaths.push_back(path);
        lastPath = path;
    }
    mode_t rootMode = 0777;
    // Iterate through the parents and create the missing ones.
    for (std::vector<FilePath>::reverse_iterator i = subpaths.rbegin(); i != subpaths.rend(); ++i) {
        if (DirectoryExists(*i)) {
            continue;
        }
        if (mkdir(i->Value().c_str(), rootMode) == 0) {
            continue;
        }

        if (!DirectoryExists(*i)) {
            return false;
        }
    }
    return true;
}
// static
bool FilePath::DirectoryExists(const FilePath &path)
{
    struct stat fileInfo;
    if (stat(const_cast<FilePath &>(path).Value().c_str(), &fileInfo) == 0) {
        return S_ISDIR(fileInfo.st_mode);
    }

    APP_LOGI("!!!stat returns an error.!!!");
    return false;
}
bool FilePath::IsDir(const FilePath &path)
{
    std::string mPath(const_cast<FilePath &>(path).Value());
    if (mPath.empty() || mPath == kCurrentDirectory || mPath == kParentDirectory) {
        return true;
    } else {
        struct stat fileInfo;
        int ret = stat(mPath.c_str(), &fileInfo);
        return (ret == 0 && S_ISDIR(fileInfo.st_mode));
    }
}

bool FilePath::PathIsValid(const FilePath &path)
{
    return access(const_cast<FilePath &>(path).Value().c_str(), F_OK) == 0;
}

// Returns a FilePath by appending a separator and the supplied path
// component to this object's path.  Append takes care to avoid adding
// excessive separators if this object's path already ends with a separator.
// If this object's path is kCurrentDirectory, a new FilePath corresponding
// only to |component| is returned.  |component| must be a relative path;
// it is an error to pass an absolute path.
FilePath FilePath::Append(const std::string &component)
{
    if (component.empty()) {
        FilePath pathOrg(path_);
        return pathOrg;
    }

    std::string newPathString;
    // empty
    if (Value().empty()) {
        newPathString += component;
    } else if (EndsWith(Value(), SEPARATOR)) {
        if (StartsWith(component, SEPARATOR)) {
            newPathString = component.substr(1, component.size());
        } else {
            newPathString = path_ + SEPARATOR + component;
        }
    } else {
        if (StartsWith(component, SEPARATOR)) {
            newPathString = path_ + component;
        } else {
            newPathString = path_ + SEPARATOR + component;
        }
    }

    FilePath newPath(newPathString);
    return newPath;
}

FilePath FilePath::Append(FilePath &component)
{
    if (component.path_.empty()) {
        FilePath pathOrg(path_);
        return pathOrg;
    }

    return Append(component.path_);
}

void FilePath::AppendSeparator(void)
{
    if (path_.empty()) {
        path_ = SEPARATOR;
    } else {
        path_ += SEPARATOR;
    }
}
// If IsParent(child) holds, appends to path (if non-NULL) the
// relative path to child and returns true.
bool FilePath::AppendRelativePath(const FilePath &child, FilePath *path)
{
    FilePath childPath = child;
    std::vector<std::string> parentComponents;
    std::vector<std::string> childComponents;
    GetComponents(parentComponents);
    childPath.GetComponents(childComponents);

    if (parentComponents.empty() || parentComponents.size() >= childComponents.size()) {
        return false;
    }

    std::vector<std::string> parentComponentsReverse;
    std::vector<std::string> childComponentsReverse;

    std::vector<std::string>::reverse_iterator riter;
    for (riter = parentComponents.rbegin(); riter != parentComponents.rend(); ++riter) {
        parentComponentsReverse.push_back(*riter);
    }
    for (riter = childComponents.rbegin(); riter != childComponents.rend(); ++riter) {
        childComponentsReverse.push_back(*riter);
    }
    std::vector<std::string>::const_iterator parentIt = parentComponentsReverse.begin();
    std::vector<std::string>::const_iterator childIt = childComponentsReverse.begin();
    while (parentIt != parentComponentsReverse.end()) {
        if (*parentIt != *childIt)
            return false;
        ++parentIt;
        ++childIt;
    }

    if (path != nullptr) {
        // Relative paths do not include separator
        if ((childIt != childComponentsReverse.end()) && (*childIt == SEPARATOR)) {
            ++childIt;
        }

        for (; childIt != childComponentsReverse.end(); ++childIt) {
            *path = path->Append(*childIt);
        }
    }
    return true;
}
bool FilePath::GetZipAllDirFiles(const std::string &path, std::vector<std::string> &files)
{
    std::string pathStringWithDelimiter;
    if (path.empty() || path == kCurrentDirectory || path == kParentDirectory) {
        return true;
    }
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }

    bool result = false;
    while (true) {
        result = true;
        struct dirent *ptr = readdir(dir);
        if (ptr == nullptr) {
            break;
        }
        // current dir OR parent dir
        if ((strcmp(ptr->d_name, kCurrentDirectory) == 0) || (strcmp(ptr->d_name, kParentDirectory) == 0)) {
            continue;
        } else if (ptr->d_type == DT_DIR) {
            pathStringWithDelimiter = IncludeTrailingPathDelimiter(path) + string(ptr->d_name);
            std::vector<std::string> itemFiles;
            result = GetZipAllDirFiles(pathStringWithDelimiter, itemFiles);

            if (itemFiles.empty()) {
                files.push_back(pathStringWithDelimiter);
            } else {
                files.insert(files.end(), itemFiles.begin(), itemFiles.end());
            }
        } else {
            files.push_back(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
        }
    }
    closedir(dir);
    return result;
}

std::string FilePath::CheckDestDirTail()
{
    if (path_.substr(path_.size()-ZIP_SIZE, ZIP_SIZE) == ZIP) {
        return path_;
    } else {
        return path_ + ZIP;
    }
}
}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
