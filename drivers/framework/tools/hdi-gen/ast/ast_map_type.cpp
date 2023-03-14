/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_map_type.h"

namespace OHOS {
namespace HDI {
bool ASTMapType::IsMapType()
{
    return true;
}

String ASTMapType::ToString()
{
    return String::Format("Map<%s, %s>", keyType_->ToString().string(), valueType_->ToString().string());
}

TypeKind ASTMapType::GetTypeKind()
{
    return TypeKind::TYPE_MAP;
}

String ASTMapType::EmitCType(TypeMode mode) const
{
    // c language has no map type
    return "/";
}

String ASTMapType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format(
                "std::map<%s, %s>", keyType_->EmitCppType().string(), valueType_->EmitCppType().string());
        case TypeMode::PARAM_IN:
            return String::Format(
                "const std::map<%s, %s>&", keyType_->EmitCppType().string(), valueType_->EmitCppType().string());
        case TypeMode::PARAM_OUT:
            return String::Format(
                "std::map<%s, %s>&", keyType_->EmitCppType().string(), valueType_->EmitCppType().string());
        case TypeMode::LOCAL_VAR:
            return String::Format(
                "std::map<%s, %s>", keyType_->EmitCppType().string(), valueType_->EmitCppType().string());
        default:
            return "unknow type";
    }
}

String ASTMapType::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return String::Format(
        "HashMap<%s, %s>", keyType_->EmitJavaType(mode, true).string(), valueType_->EmitJavaType(mode, true).string());
}

void ASTMapType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    String elementName = String::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (auto %s : %s) {\n", elementName.string(), name.string());

    String keyName = String::Format("(%s.first)", elementName.string());
    String valueName = String::Format("(%s.second)", elementName.string());
    keyType_->EmitCppWriteVar(parcelName, keyName, sb, prefix + TAB, innerLevel);
    valueType_->EmitCppWriteVar(parcelName, valueName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    if (initVariable) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = %s.ReadUint32();\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %sSize; ++i) {\n", name.string());

    String KeyName = String::Format("key%d", innerLevel);
    String valueName = String::Format("value%d", innerLevel);
    innerLevel++;
    keyType_->EmitCppReadVar(parcelName, KeyName, sb, prefix + TAB, true, innerLevel);
    valueType_->EmitCppReadVar(parcelName, valueName, sb, prefix + TAB, true, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.string(), KeyName.string(), valueName.string());
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitCppMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.string(), name.string());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: write %s.size failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
    String elementName = String::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.string(), name.string());

    String keyName = String::Format("(%s.first)", elementName.string());
    String valName = String::Format("(%s.second)", elementName.string());
    keyType_->EmitCppMarshalling(parcelName, keyName, sb, prefix + TAB, innerLevel);
    valueType_->EmitCppMarshalling(parcelName, valName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitCppUnMarshalling(const String &parcelName, const String &name, StringBuilder &sb,
    const String &prefix, bool emitType, unsigned int innerLevel) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().string(), name.string());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = %s.ReadUint32();\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %sSize; ++i) {\n", name.string());

    String KeyName = String::Format("key%d", innerLevel);
    String valueName = String::Format("value%d", innerLevel);
    innerLevel++;
    keyType_->EmitCppUnMarshalling(parcelName, KeyName, sb, prefix + TAB, true, innerLevel);
    valueType_->EmitCppUnMarshalling(parcelName, valueName, sb, prefix + TAB, true, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.string(), KeyName.string(), valueName.string());
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitJavaWriteVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s.size());\n", parcelName.string(), name.string());
    sb.Append(prefix).AppendFormat("for (Map.Entry<%s, %s> entry : %s.entrySet()) {\n",
        keyType_->EmitJavaType(TypeMode::NO_MODE, true).string(),
        valueType_->EmitJavaType(TypeMode::NO_MODE, true).string(), name.string());
    keyType_->EmitJavaWriteVar(parcelName, "entry.getKey()", sb, prefix + TAB);
    valueType_->EmitJavaWriteVar(parcelName, "entry.getValue()", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitJavaReadVar(
    const String &parcelName, const String &name, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.string());

    keyType_->EmitJavaReadInnerVar(parcelName, "key", false, sb, prefix + TAB);
    valueType_->EmitJavaReadInnerVar(parcelName, "value", false, sb, prefix + TAB);

    sb.Append(prefix + TAB).AppendFormat("%s.put(key, value);\n", name.string());
    sb.Append(prefix).Append("}\n");
}

void ASTMapType::EmitJavaReadInnerVar(
    const String &parcelName, const String &name, bool isInner, StringBuilder &sb, const String &prefix) const
{
    sb.Append(prefix).AppendFormat("%s %s = new Hash%s();\n", EmitJavaType(TypeMode::NO_MODE).string(), name.string(),
        EmitJavaType(TypeMode::NO_MODE).string());
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.string(), parcelName.string());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.string());

    keyType_->EmitJavaReadInnerVar(parcelName, "key", true, sb, prefix + TAB);
    valueType_->EmitJavaReadInnerVar(parcelName, "value", true, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("%s.put(key, value);\n", name.string());
    sb.Append(prefix).Append("}\n");
}
} // namespace HDI
} // namespace OHOS