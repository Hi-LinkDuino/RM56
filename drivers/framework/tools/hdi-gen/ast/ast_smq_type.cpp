/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast/ast_smq_type.h"

namespace OHOS {
namespace HDI {
bool ASTSmqType::IsSmqType()
{
    return true;
}

String ASTSmqType::ToString()
{
    return String::Format("SharedMemQueue<%s>", innerType_->ToString().string());
}

TypeKind ASTSmqType::GetTypeKind()
{
    return TypeKind::TYPE_SMQ;
}

String ASTSmqType::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return String::Format("SharedMemQueue<%s>", innerType_->EmitCppType().string());
        case TypeMode::PARAM_IN:
            return String::Format("const std::shared_ptr<SharedMemQueue<%s>>&", innerType_->EmitCppType().string());
        case TypeMode::PARAM_OUT:
            return String::Format("std::shared_ptr<SharedMemQueue<%s>>&", innerType_->EmitCppType().string());
        case TypeMode::LOCAL_VAR:
            return String::Format("std::shared_ptr<SharedMemQueue<%s>>", innerType_->EmitCppType().string());
        default:
            return "unknow type";
    }
}

void ASTSmqType::EmitCppWriteVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s->IsGood() || %s->GetMeta() == nullptr || ", name.string(), name.string());
    sb.AppendFormat("!%s->GetMeta()->Marshalling(%s)) {\n", name.string(), parcelName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.string());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void ASTSmqType::EmitCppReadVar(const String &parcelName, const String &name, StringBuilder &sb, const String &prefix,
    bool initVariable, unsigned int innerLevel) const
{
    String metaVarName = String::Format("%sMeta_", name.string());
    sb.Append(prefix).AppendFormat(
        "std::shared_ptr<SharedMemQueueMeta<%s>> %s = ", innerType_->EmitCppType().string(), metaVarName.string());
    sb.AppendFormat(
        "SharedMemQueueMeta<%s>::UnMarshalling(%s);\n", innerType_->EmitCppType().string(), parcelName.string());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", metaVarName.string());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: SharedMemQueueMeta is nullptr\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat(
        "std::shared_ptr<SharedMemQueue<%s>> %s = ", innerType_->EmitCppType().string(), name.string());
    sb.AppendFormat(
        "std::make_shared<SharedMemQueue<%s>>(*%s);\n", innerType_->EmitCppType().string(), metaVarName.string());
}
} // namespace HDI
} // namespace OHOS