/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_ASTTYPE_H
#define OHOS_HDI_ASTTYPE_H

#include "ast/ast_namespace.h"
#include "ast/ast_node.h"
#include "util/autoptr.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
enum class TypeKind {
    TYPE_UNKNOWN = 0,
    TYPE_BOOLEAN,
    TYPE_BYTE,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_UCHAR,
    TYPE_USHORT,
    TYPE_UINT,
    TYPE_ULONG,
    TYPE_STRING,
    TYPE_FILEDESCRIPTOR,
    TYPE_VOID,
    TYPE_SEQUENCEABLE,
    TYPE_INTERFACE,
    TYPE_LIST,
    TYPE_MAP,
    TYPE_ARRAY,
    TYPE_ENUM,
    TYPE_STRUCT,
    TYPE_UNION,
    TYPE_SMQ,
};

enum class TypeMode {
    NO_MODE,   // only type
    PARAM_IN,  // type of the in attribute parameter
    PARAM_OUT, // type of the out attribute parameter
    LOCAL_VAR, // type of the local variable
};

enum class LanguageType {
    LANG_C,
    LANG_CPP,
    LANG_JAVA,
};

class ASTType : public ASTNode {
public:
    virtual void SetName(const String &name);

    virtual String GetName();

    virtual void SetNamespace(const AutoPtr<ASTNamespace> &nspace);

    virtual AutoPtr<ASTNamespace> GetNamespace();

    virtual bool IsBooleanType();

    virtual bool IsByteType();

    virtual bool IsShortType();

    virtual bool IsIntegerType();

    virtual bool IsLongType();

    virtual bool IsUcharType();

    virtual bool IsUshortType();

    virtual bool IsUintType();

    virtual bool IsUlongType();

    virtual bool IsFloatType();

    virtual bool IsDoubleType();

    virtual bool IsStringType();

    virtual bool IsListType();

    virtual bool IsMapType();

    virtual bool IsEnumType();

    virtual bool IsStructType();

    virtual bool IsUnionType();

    virtual bool IsInterfaceType();

    virtual bool IsSequenceableType();

    virtual bool IsVoidType();

    virtual bool IsArrayType();

    virtual bool IsFdType();

    virtual bool IsSmqType();

    virtual String ToShortString();

    String ToString() override;

    virtual TypeKind GetTypeKind();

    virtual String EmitCType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual String EmitCppType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual String EmitJavaType(TypeMode mode, bool isInnerType = false) const;

    virtual void EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCProxyWriteOutVar(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
        const String &gotoLabel, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCStubReadOutVar(const String &buffSizeName, const String &memFlagName, const String &parcelName,
        const String &name, const String &ecName, const String &gotoLabel, StringBuilder &sb,
        const String &prefix) const;

    virtual void EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        unsigned int innerLevel = 0) const;

    virtual void EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
        bool initVariable, unsigned int innerLevel = 0) const;

    virtual void EmitCMarshalling(const String &name, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCUnMarshalling(const String &name, const String &gotoLabel, StringBuilder &sb,
        const String &prefix, std::vector<String> &freeObjStatements) const;

    void EmitFreeStatements(
        const std::vector<String> &freeObjStatements, StringBuilder &sb, const String &prefix) const;

    virtual void EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
        const String &prefix, unsigned int innerLevel = 0) const;

    virtual void EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
        const String &prefix, bool emitType, unsigned int innerLevel = 0) const;

    virtual void EmitMemoryRecycle(
        const String &name, bool isClient, bool ownership, StringBuilder &sb, const String &prefix) const;

    virtual void EmitJavaWriteVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const;

    virtual void EmitJavaReadVar(
        const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const;

    virtual void EmitJavaReadInnerVar(
        const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const;

protected:
    String name_;
    AutoPtr<ASTNamespace> namespace_;
    TypeKind typeKind_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_ASTTYPE_H