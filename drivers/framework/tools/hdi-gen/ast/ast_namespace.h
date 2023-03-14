/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTNAMESPACE_H
#define OHOS_HDI_ASTNAMESPACE_H

#include <vector>

#include "ast/ast_node.h"
#include "util/autoptr.h"

namespace OHOS {
namespace HDI {
class ASTInterfaceType;
class ASTSequenceableType;

class ASTNamespace : public ASTNode {
public:
    explicit ASTNamespace(const String &nspaceStr);

    virtual ~ASTNamespace() = default;

    inline String GetName()
    {
        return name_;
    }

    void AddNamespace(const AutoPtr<ASTNamespace> &innerNspace);

    AutoPtr<ASTNamespace> FindNamespace(const String &nspaceStr);

    AutoPtr<ASTNamespace> GetNamespace(size_t index);

    inline size_t GetNamespaceNumber()
    {
        return innerNamespaces_.size();
    }

    void AddInterface(const AutoPtr<ASTInterfaceType> &interface);

    AutoPtr<ASTInterfaceType> GetInterface(size_t index);

    inline size_t GetInterfaceNumber()
    {
        return interfaces_.size();
    }

    void AddSequenceable(const AutoPtr<ASTSequenceableType> &sequenceable);

    AutoPtr<ASTSequenceableType> GetSequenceable(size_t index);

    inline size_t GetSequenceableNumber()
    {
        return sequenceables_.size();
    }

    inline String ToShortString()
    {
        return name_;
    }

    String ToString() override;

private:
    String name_;
    ASTNamespace *outerNamespace_;
    std::vector<AutoPtr<ASTNamespace>> innerNamespaces_;
    std::vector<AutoPtr<ASTSequenceableType>> sequenceables_;
    std::vector<AutoPtr<ASTInterfaceType>> interfaces_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTNAMESPACE_H