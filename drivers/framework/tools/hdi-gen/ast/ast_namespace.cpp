/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_namespace.h"
#include "ast/ast_interface_type.h"
#include "ast/ast_sequenceable_type.h"

namespace OHOS {
namespace HDI {
ASTNamespace::ASTNamespace(const String &nspaceStr) : name_(nspaceStr), outerNamespace_(nullptr) {}

void ASTNamespace::AddNamespace(const AutoPtr<ASTNamespace> &innerNspace)
{
    if (innerNspace == nullptr) {
        return;
    }

    innerNamespaces_.push_back(innerNspace);
    innerNspace->outerNamespace_ = this;
}

AutoPtr<ASTNamespace> ASTNamespace::FindNamespace(const String &nspaceStr)
{
    if (nspaceStr.IsEmpty()) {
        return nullptr;
    }

    for (auto nspace : innerNamespaces_) {
        if (nspace->name_.Equals(nspaceStr)) {
            return nspace;
        }
    }
    return nullptr;
}

AutoPtr<ASTNamespace> ASTNamespace::GetNamespace(size_t index)
{
    if (index >= innerNamespaces_.size()) {
        return nullptr;
    }

    return innerNamespaces_[index];
}

void ASTNamespace::AddInterface(const AutoPtr<ASTInterfaceType> &interface)
{
    if (interface == nullptr) {
        return;
    }

    interfaces_.push_back(interface);
}

AutoPtr<ASTInterfaceType> ASTNamespace::GetInterface(size_t index)
{
    if (index >= interfaces_.size()) {
        return nullptr;
    }

    return interfaces_[index];
}

void ASTNamespace::AddSequenceable(const AutoPtr<ASTSequenceableType> &sequenceable)
{
    if (sequenceable == nullptr) {
        return;
    }

    sequenceables_.push_back(sequenceable);
}

AutoPtr<ASTSequenceableType> ASTNamespace::GetSequenceable(size_t index)
{
    if (index >= sequenceables_.size()) {
        return nullptr;
    }

    return sequenceables_[index];
}

String ASTNamespace::ToString()
{
    String nspaceStr;
    ASTNamespace *nspace = this;
    while (nspace != nullptr) {
        nspaceStr = nspace->name_ + "." + nspaceStr;
        nspace = nspace->outerNamespace_;
    }
    return nspaceStr;
}
} // namespace HDI
} // namespace OHOS