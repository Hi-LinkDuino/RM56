/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "parser/module_parser.h"

#include <queue>

#include "parser/parser.h"
#include "util/logger.h"

namespace OHOS {
namespace HDI {
const char *ModuleParser::TAG = "ModuleParser";

AutoPtr<ASTModule> ModuleParser::Parse()
{
    if (!ParserDependencies()) {
        return nullptr;
    }

    if (!CompileFiles()) {
        return nullptr;
    }

    return module_;
}

bool ModuleParser::ParserDependencies()
{
    if (option_.GetSourceFiles().size() == 1) {
        if (!ParserAllImports(option_.GetSourceFiles()[0])) {
            Logger::E(TAG, "Parsing all idl file failed.");
            return false;
        }
    } else {
        if (!ParserAllidlFile(option_.GetSourceFiles())) {
            Logger::E(TAG, "Parsing all given idl file failed.");
            return false;
        }
    }

    if (!CheckCircularReference()) {
        Logger::E(TAG, "has circle reference.");
        return false;
    }

    return true;
}

bool ModuleParser::CompileFiles()
{
    std::unique_ptr<Parser> parserPtr = std::make_unique<Parser>(option_, module_);

    for (const auto &filePath : compileFiles_) {
        if (!parserPtr->Parse(filePath)) {
            Logger::E(TAG, "parse %s failed", filePath.string());
            return false;
        }
    }

    return true;
}

bool ModuleParser::ParserAllImports(const String &rootFilePath)
{
    std::unique_ptr<Parser> parserPtr = std::make_unique<Parser>(option_);
    std::shared_ptr<FileDetail> fileInfo = nullptr;

    if (!parserPtr->Parse(rootFilePath, fileInfo)) {
        return false;
    }

    if (fileInfo == nullptr) {
        return false;
    }

    sourceFiles_[fileInfo->GetFullName()] = fileInfo;
    return ParserAllImportsRecursion(fileInfo);
}

bool ModuleParser::ParserAllImportsRecursion(const std::shared_ptr<FileDetail> &fileInfo)
{
    for (const auto &importName : fileInfo->GetImports()) {
        if (sourceFiles_.find(importName) != sourceFiles_.end()) {
            continue;
        }

        String filePath = FileDetail::ImportsToPath(importName);

        std::unique_ptr<Parser> parserPtr = std::make_unique<Parser>(option_);
        std::shared_ptr<FileDetail> file = nullptr;
        if (!parserPtr->Parse(filePath, file)) {
            Logger::E(TAG, "Parsing %s failed.", filePath.string());
            return false;
        }

        if (file == nullptr) {
            Logger::E(TAG, "Parsing %s failed, generator filedetail is nullptr.", filePath.string());
            return false;
        }

        sourceFiles_[file->GetFullName()] = file;

        if (!ParserAllImportsRecursion(file)) {
            Logger::E(TAG, "Parsing %s file's import failed.", file->GetFilePath().string());
            return false;
        }
    }

    return true;
}

bool ModuleParser::ParserAllidlFile(const std::vector<String> &idlSourceFile)
{
    std::unique_ptr<Parser> parserPtr = std::make_unique<Parser>(option_);

    for (const auto &idlSourceFile : idlSourceFile) {
        std::shared_ptr<FileDetail> fileInfo = nullptr;
        if (!parserPtr->Parse(idlSourceFile, fileInfo)) {
            return false;
        }

        if (fileInfo == nullptr) {
            return false;
        }

        sourceFiles_[fileInfo->GetFullName()] = fileInfo;
    }

    return true;
}

bool ModuleParser::CheckCircularReference()
{
    std::queue<std::shared_ptr<FileDetail>> fileQueue;
    for (const auto &filePair : sourceFiles_) {
        std::shared_ptr<FileDetail> fileNode = filePair.second;
        if (fileNode->GetImportSize() == 0) {
            fileQueue.push(fileNode);
        }
    }

    compileFiles_.clear();
    while (!fileQueue.empty()) {
        std::shared_ptr<FileDetail> importFile = fileQueue.front();
        fileQueue.pop();
        compileFiles_.push_back(importFile->GetFilePath());

        for (const auto &filePair : sourceFiles_) {
            std::shared_ptr<FileDetail> fileNode = filePair.second;
            if (fileNode->GetImportSize() > 0) {
                fileNode->DelImport(importFile->GetFullName());
                if (fileNode->GetImportSize() == 0) {
                    fileQueue.push(fileNode);
                }
            }
        }
    }

    if (compileFiles_.size() < sourceFiles_.size()) {
        return false;
    }

    return true;
}
} // namespace HDI
} // namespace OHOS