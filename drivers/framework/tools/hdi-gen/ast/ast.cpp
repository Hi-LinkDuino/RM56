/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast.h"

#include <cstdlib>

#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
AST::AST()
{
    types_["boolean"] = new ASTBooleanType();
    types_["byte"] = new ASTByteType();
    types_["short"] = new ASTShortType();
    types_["int"] = new ASTIntegerType();
    types_["long"] = new ASTLongType();
    types_["float"] = new ASTFloatType();
    types_["double"] = new ASTDoubleType();
    types_["String"] = new ASTStringType();
    types_["unsigned char"] = new ASTUcharType();
    types_["unsigned short"] = new ASTUshortType();
    types_["unsigned int"] = new ASTUintType();
    types_["unsigned long"] = new ASTUlongType();
    types_["void"] = new ASTVoidType();
    types_["FileDescriptor"] = new ASTFdType();
}

void AST::SetIdlFile(const String &idlFile)
{
    idlFilePath_ = idlFile;
#ifdef __MINGW32__
    int index = idlFilePath_.LastIndexOf('\\');
#else
    int index = idlFilePath_.LastIndexOf('/');
#endif
    int end = idlFilePath_.LastIndexOf(".idl");
    name_ = idlFilePath_.Substring((index == -1) ? 0 : (index + 1), end);
}

void AST::SetFullName(const String &fullName)
{
    int index = fullName.LastIndexOf('.');
    if (index != -1) {
        packageName_ = fullName.Substring(0, index);
        name_ = fullName.Substring(index + 1);
    } else {
        packageName_ = "";
        name_ = fullName;
    }
}

void AST::SetPackageName(const String &packageName)
{
    packageName_ = packageName;
}

String AST::GetPackageName()
{
    return packageName_;
}

AutoPtr<ASTNamespace> AST::ParseNamespace(const String &nspaceStr)
{
    AutoPtr<ASTNamespace> currNspace;
    int begin = 0;
    int index = 0;
    while ((index = nspaceStr.IndexOf('.', begin)) != -1) {
        String ns = nspaceStr.Substring(begin, index);
        AutoPtr<ASTNamespace> nspace;
        if (currNspace == nullptr) {
            nspace = FindNamespace(ns);
        } else {
            nspace = currNspace->FindNamespace(ns);
        }
        if (nspace == nullptr) {
            nspace = new ASTNamespace(ns);
            if (currNspace == nullptr) {
                AddNamespace(nspace);
            } else {
                currNspace->AddNamespace(nspace);
            }
        }
        currNspace = nspace;
        begin = index + 1;
    }
    return currNspace;
}

void AST::AddNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    if (nspace == nullptr) {
        return;
    }
    namespaces_.push_back(nspace);
}

AutoPtr<ASTNamespace> AST::FindNamespace(const String &nspaceStr)
{
    for (auto nspace : namespaces_) {
        if (nspace->ToShortString().Equals(nspaceStr)) {
            return nspace;
        }
    }
    return nullptr;
}

AutoPtr<ASTNamespace> AST::GetNamespace(size_t index)
{
    if (index >= namespaces_.size()) {
        return nullptr;
    }

    return namespaces_[index];
}

void AST::AddInterfaceDef(const AutoPtr<ASTInterfaceType> &interface)
{
    if (interface == nullptr) {
        return;
    }

    interfaceDef_ = interface;
    AddType(interface.Get());
}

void AST::AddSequenceableDef(const AutoPtr<ASTSequenceableType> &sequenceable)
{
    if (sequenceable == nullptr) {
        return;
    }

    sequenceableDef_ = sequenceable;
    AddType(sequenceable.Get());
}

void AST::AddType(const AutoPtr<ASTType> &type)
{
    if (type == nullptr) {
        return;
    }

    types_[type->ToString()] = type;
}

AutoPtr<ASTType> AST::FindType(const String &typeName)
{
    if (typeName.IsEmpty()) {
        return nullptr;
    }

    auto it = types_.find(typeName);
    if (it != types_.end()) {
        return it->second;
    }

    AutoPtr<ASTType> type = nullptr;
    for (const auto &importPair : imports_) {
        type = importPair.second->FindType(typeName);
        if (type != nullptr) {
            break;
        }
    }
    return type;
}

void AST::AddTypeDefinition(const AutoPtr<ASTType> &type)
{
    if (type == nullptr) {
        return;
    }

    AddType(type);
    typeDefinitions_.push_back(type);
}

AutoPtr<ASTType> AST::GetTypeDefintion(size_t index)
{
    if (index >= typeDefinitions_.size()) {
        return nullptr;
    }
    return typeDefinitions_[index];
}

String AST::Dump(const String &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);
    sb.Append("AST[");
    sb.Append("name: ").Append(name_).Append(" ");
    sb.Append("file: ").Append(idlFilePath_);
    sb.Append("]\n");

    sb.Append("package ").Append(packageName_).Append(";");
    sb.Append('\n');
    sb.Append('\n');

    if (imports_.size() > 0) {
        for (const auto &import : imports_) {
            sb.AppendFormat("import %s;\n", import.first.string());
        }
        sb.Append("\n");
    }

    if (typeDefinitions_.size() > 0) {
        for (auto type : typeDefinitions_) {
            String info = type->Dump("");
            sb.Append(info).Append("\n");
        }
    }

    if (interfaceDef_ != nullptr) {
        String info = interfaceDef_->Dump("");
        sb.Append(info).Append("\n");
    }

    return sb.ToString();
}

bool AST::AddImport(const AutoPtr<AST> &importAst)
{
    if (imports_.find(importAst->GetFullName()) != imports_.end()) {
        return false;
    }

    imports_[importAst->GetFullName()] = importAst;

    return true;
}

void AST::SetVersion(size_t &majorVer, size_t &minorVer)
{
    majorVersion_ = majorVer;
    minorVersion_ = minorVer;
}
} // namespace HDI
} // namespace OHOS