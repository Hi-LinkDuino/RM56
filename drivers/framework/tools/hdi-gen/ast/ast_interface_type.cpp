/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_interface_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void ASTInterfaceType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    ASTType::SetNamespace(nspace);
    if (namespace_ != nullptr) {
        namespace_->AddInterface(this);
    }
}

void ASTInterfaceType::AddMethod(const AutoPtr<ASTMethod> &method)
{
    if (method == nullptr) {
        return;
    }
    methods_.push_back(method);
}

AutoPtr<ASTMethod> ASTInterfaceType::GetMethod(size_t index)
{
    if (index >= methods_.size()) {
        return nullptr;
    }

    return methods_[index];
}

bool ASTInterfaceType::IsInterfaceType()
{
    return true;
}

String ASTInterfaceType::ToString()
{
    return name_;
}

String ASTInterfaceType::Dump(const String &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);

    std::vector<String> attributes;
    if (isOneWay_) {
        attributes.push_back("oneway");
    }
    if (isCallback_) {
        attributes.push_back("callback");
    }
    if (isFull_) {
        attributes.push_back("full");
    }
    if (isLite_) {
        attributes.push_back("lite");
    }
    if (attributes.size() > 0) {
        sb.Append("[");
        for (size_t i = 0; i < attributes.size(); i++) {
            sb.Append(attributes[i]);
            if (i < attributes.size() - 1) {
                sb.Append(',');
            }
        }
        sb.Append("] ");
    }

    sb.AppendFormat("interface %s {\n", name_.string());

    for (auto method : methods_) {
        String info = method->Dump(prefix + "  ");
        sb.Append(info);
        if (method != methods_[methods_.size() - 1]) {
            sb.Append('\n');
        }
    }
    sb.Append(prefix).Append("}\n");

    return sb.ToString();
}

TypeKind ASTInterfaceType::GetTypeKind()
{
    return TypeKind::TYPE_INTERFACE;
}

String ASTInterfaceType::GetFullName() const
{
    return namespace_->ToString() + name_;
}

String ASTInterfaceType::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("struct %s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("struct %s*", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("struct %s**", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("struct %s*", name_.string());
        default:
            return "unknow type";
    }
}

String ASTInterfaceType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("%s", name_.string());
        case TypeMode::PARAM_IN:
            return String::Format("const sptr<%s>&", name_.string());
        case TypeMode::PARAM_OUT:
            return String::Format("sptr<%s>&", name_.string());
        case TypeMode::LOCAL_VAR:
            return String::Format("sptr<%s>", name_.string());
        default:
            return "unknow type";
    }
}

String ASTInterfaceType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return name_;
}

void ASTInterfaceType::EmitCWriteVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("if (HdfSbufWriteRemoteService(%s, %s->AsObject(%s)) != 0) {\n", parcelName.string(),
        name.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
}

void ASTInterfaceType::EmitCProxyReadVar(const String &parcelName, const String &name, bool isInnerType,
    const String &ecName, const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String remoteName = String::Format("%sRemote", name.string());
    String baseName = name_.StartsWith("I") ? name_.Substring(1) : name_;

    sb.Append(prefix).AppendFormat(
        "struct HdfRemoteService *%s = HdfSbufReadRemoteService(%s);\n", remoteName.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", remoteName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", remoteName.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");

    sb.Append(prefix).AppendFormat("*%s = %sGet(%s);\n", name.string(), baseName.string(), remoteName.string());
}

void ASTInterfaceType::EmitCStubReadVar(const String &parcelName, const String &name, const String &ecName,
    const String &gotoLabel, StringBuilder &sb, const String &prefix) const
{
    String remoteName = String::Format("%sRemote", name.string());
    String baseName = name_.StartsWith("I") ? name_.Substring(1) : name_;

    sb.Append(prefix).AppendFormat(
        "struct HdfRemoteService *%s = HdfSbufReadRemoteService(%s);\n", remoteName.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", remoteName.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", remoteName.string());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", ecName.string());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.string());
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("%s = %sGet(%s);\n", name.string(), baseName.string(), remoteName.string());
}

void ASTInterfaceType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%s.WriteRemoteObject(%s->AsObject())) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTInterfaceType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("sptr<%s> %s = iface_cast<%s>(%s.ReadRemoteObject());\n", name_.string(),
            name.string(), name_.string(), parcelName.string());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = iface_cast<%s>(%s.ReadRemoteObject());\n", name.string(), name_.string(), parcelName.string());
    }
}

void ASTInterfaceType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeRemoteObject(%s.asObject());\n", parcelName.string(), name.string());
}

void ASTInterfaceType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    String stubName = name_.StartsWith("I") ? (name_.Substring(1) + "Stub") : (name_ + "Stub");
    sb.Append(prefix).AppendFormat(
        "%s = %s.asInterface(%s.readRemoteObject());\n", name.string(), stubName.string(), parcelName.string());
}

void ASTInterfaceType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    String stubName = name_.StartsWith("I") ? (name_.Substring(1) + "Stub") : (name_ + "Stub");
    sb.Append(prefix).AppendFormat("%s %s = %s.asInterface(%s.readRemoteObject());\n",
        EmitJavaType(TypeMode::NO_MODE).string(), name.string(), stubName.string(), parcelName.string());
}
} // namespace HDI
} // namespace OHOS