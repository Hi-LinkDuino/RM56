/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_type.h"

namespace OHOS {
namespace HDI {
const char *TAB = TAB;

void ASTType::SetName(const String &name)
{
    name_ = name;
}

String ASTType::GetName()
{
    return name_;
}

void ASTType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    namespace_ = nspace;
}

AutoPtr<ASTNamespace> ASTType::GetNamespace()
{
    return namespace_;
}

bool ASTType::IsBooleanType()
{
    return false;
}

bool ASTType::IsByteType()
{
    return false;
}

bool ASTType::IsShortType()
{
    return false;
}

bool ASTType::IsIntegerType()
{
    return false;
}

bool ASTType::IsLongType()
{
    return false;
}

bool ASTType::IsUcharType()
{
    return false;
}

bool ASTType::IsUshortType()
{
    return false;
}

bool ASTType::IsUintType()
{
    return false;
}

bool ASTType::IsUlongType()
{
    return false;
}

bool ASTType::IsFloatType()
{
    return false;
}

bool ASTType::IsDoubleType()
{
    return false;
}

bool ASTType::IsStringType()
{
    return false;
}

bool ASTType::IsListType()
{
    return false;
}

bool ASTType::IsMapType()
{
    return false;
}

bool ASTType::IsEnumType()
{
    return false;
}

bool ASTType::IsStructType()
{
    return false;
}

bool ASTType::IsUnionType()
{
    return false;
}

bool ASTType::IsInterfaceType()
{
    return false;
}

bool ASTType::IsSequenceableType()
{
    return false;
}

bool ASTType::IsVoidType()
{
    return false;
}

bool ASTType::IsArrayType()
{
    return false;
}

bool ASTType::IsFdType()
{
    return false;
}

bool ASTType::IsSmqType()
{
    return false;
}

String ASTType::ToShortString()
{
    return name_;
}

String ASTType::ToString()
{
    return (namespace_ == nullptr) ? name_ : (namespace_->ToString() + name_);
}

TypeKind ASTType::GetTypeKind()
{
    return TypeKind::TYPE_UNKNOWN;
}

String ASTType::EmitCType(TypeMode mode) const
{
    return "unknow";
}

String ASTType::EmitCppType(TypeMode mode) const
{
    return "unknow";
}

String ASTType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return "unknow";
}

void ASTType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName, const String &gotoLabel,
    StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCProxyWriteOutVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCStubReadOutVar(const String &buffSizeName, const String &memFlagName, const String &parcelName,
    const String &name, const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb, const String &prefix,
    std::vector<String> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitFreeStatements(
    const std::vector<String> &freeObjStatements, StringBuilder &sb, const String &prefix) const
{
    for (auto it = freeObjStatements.rbegin(); it != freeObjStatements.rend(); it++) {
        sb.Append(prefix).Append(*it);
    }
}

void ASTType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitMemoryRecycle(
    const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const
{
    // only String, Array, List, struct and union type need recycle memory
}

void ASTType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}

void ASTType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.string());
}
} // namespace HDI
} // namespace OHOS