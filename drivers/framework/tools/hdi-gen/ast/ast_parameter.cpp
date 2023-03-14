/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_parameter.h"
#include "ast/ast_array_type.h"
#include "ast/ast_list_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
String ASTParameter::Dump(const String &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);
    sb.Append('[');

    if (attribute_ == ParamAttr::PARAM_IN) {
        sb.Append("in");
    } else {
        sb.Append("out");
    }

    sb.Append("] ");
    sb.Append(type_->ToString()).Append(' ');
    sb.Append(name_);

    return sb.ToString();
}

String ASTParameter::EmitCParameter()
{
    StringBuilder sb;
    switch (type_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_ENUM:
        case TypeKind::TYPE_FILEDESCRIPTOR:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_VOID: {
            StringBuilder paramStr;
            if (attribute_ == ParamAttr::PARAM_IN) {
                paramStr.AppendFormat("%s %s", type_->EmitCType(TypeMode::PARAM_IN).string(), name_.string());
            } else {
                paramStr.AppendFormat("%s %s", type_->EmitCType(TypeMode::PARAM_OUT).string(), name_.string());
            }
            if (type_->GetTypeKind() == TypeKind::TYPE_STRING && attribute_ == ParamAttr::PARAM_OUT) {
                paramStr.AppendFormat(", uint32_t %sLen", name_.string());
            }
            return paramStr.ToString();
        }
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST: {
            StringBuilder paramStr;
            if (attribute_ == ParamAttr::PARAM_IN) {
                paramStr.AppendFormat("%s %s", type_->EmitCType(TypeMode::PARAM_IN).string(), name_.string());
            } else {
                paramStr.AppendFormat("%s %s", type_->EmitCType(TypeMode::PARAM_OUT).string(), name_.string());
            }
            paramStr.AppendFormat(
                ", uint32_t%s %sLen", (attribute_ == ParamAttr::PARAM_IN) ? "" : "*", name_.string());
            return paramStr.ToString();
        }
        default:
            return String::Format("unknow type %s", name_.string());
    }
    return sb.ToString();
}

String ASTParameter::EmitCppParameter()
{
    if (attribute_ == ParamAttr::PARAM_IN) {
        return String::Format("%s %s", type_->EmitCppType(TypeMode::PARAM_IN).string(), name_.string());
    } else {
        return String::Format("%s %s", type_->EmitCppType(TypeMode::PARAM_OUT).string(), name_.string());
    }
}

String ASTParameter::EmitJavaParameter()
{
    StringBuilder sb;
    switch (type_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_ENUM:
        case TypeKind::TYPE_FILEDESCRIPTOR:
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_SEQUENCEABLE:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_VOID:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST:
        case TypeKind::TYPE_MAP: {
            return String::Format("%s %s", type_->EmitJavaType(TypeMode::NO_MODE, false).string(), name_.string());
        }
        default:
            return String::Format("unknow type %s", name_.string());
    }

    return sb.ToString();
}

String ASTParameter::EmitCLocalVar()
{
    StringBuilder sb;
    sb.AppendFormat("%s %s", type_->EmitCType(TypeMode::LOCAL_VAR).string(), name_.string());
    switch (type_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(" = false");
            break;
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
            sb.Append(" = 0");
            break;
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_INTERFACE:
            sb.Append(" = NULL");
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(" = -1");
            break;
        default:
            break;
    }
    sb.Append(";");
    return sb.ToString();
}

String ASTParameter::EmitCppLocalVar()
{
    StringBuilder sb;
    sb.AppendFormat("%s %s", type_->EmitCppType(TypeMode::LOCAL_VAR).string(), name_.string());
    switch (type_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(" = false");
            break;
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
            sb.Append(" = 0");
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(" = -1");
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(" = nullptr");
            break;
        default:
            break;
    }
    sb.Append(";");
    return sb.ToString();
}

String ASTParameter::EmitJavaLocalVar()
{
    return "";
}

void ASTParameter::EmitCWriteVar(const String &parcelName, const String &ecName, const String &gotoLabel,
    StringBuilder &sb, const String &prefix) const
{
    if (type_ == nullptr) {
        return;
    }

    type_->EmitCWriteVar(parcelName, name_, ecName, gotoLabel, sb, prefix);
}

bool ASTParameter::EmitCProxyWriteOutVar(const String &parcelName, const String &ecName, const String &gotoLabel,
    StringBuilder &sb, const String &prefix) const
{
    if (type_ == nullptr) {
        return false;
    }

    if (type_->IsStringType() || type_->IsArrayType() || type_->IsListType()) {
        type_->EmitCProxyWriteOutVar(parcelName, name_, ecName, gotoLabel, sb, prefix);
        return true;
    }

    return false;
}

void ASTParameter::EmitCStubReadOutVar(const String &buffSizeName, const String &memFlagName, const String &parcelName,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    if (type_ == nullptr) {
        return;
    }

    if (type_->IsStringType() || type_->IsArrayType() || type_->IsListType()) {
        type_->EmitCStubReadOutVar(buffSizeName, memFlagName, parcelName, name_, ecName, gotoLabel, sb, prefix);
    }
}

void ASTParameter::EmitJavaWriteVar(const String &parcelName, StringBuilder &sb, const String &prefix) const
{
    if (attribute_ == ParamAttr::PARAM_IN) {
        type_->EmitJavaWriteVar(parcelName, name_, sb, prefix);
    } else {
        if (type_->GetTypeKind() == TypeKind::TYPE_ARRAY) {
            sb.Append(prefix).AppendFormat("if (%s == null) {\n", name_.string());
            sb.Append(prefix + TAB).AppendFormat("%s.writeInt(-1);\n", parcelName.string());
            sb.Append(prefix).Append("} else {\n");
            sb.Append(prefix + TAB).AppendFormat("%s.writeInt(%s.length);\n", parcelName.string(), name_.string());
            sb.Append(prefix).Append("}\n");
        }
    }
}

void ASTParameter::EmitJavaReadVar(const String &parcelName, StringBuilder &sb, const String &prefix) const
{
    if (attribute_ == ParamAttr::PARAM_OUT) {
        type_->EmitJavaReadVar(parcelName, name_, sb, prefix);
    }
}
} // namespace HDI
} // namespace OHOS