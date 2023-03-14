/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/c_code_emitter.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
void CCodeEmitter::GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        String fileName = PackageToFilePath(importAst->GetFullName());
        headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName));
    }
}

void CCodeEmitter::EmitInterfaceMethodParameter(
    const AutoPtr<ASTParameter> &parameter, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = parameter->GetType();
    sb.Append(prefix).Append(parameter->EmitCParameter());
}

void CCodeEmitter::EmitInitLoopVar(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix)
{
    if (isKernelCode_) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            AutoPtr<ASTType> type = param->GetType();
            if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
                sb.Append(prefix).Append("uint32_t i = 0;\n");
                break;
            }
        }
    }
}

void CCodeEmitter::EmitErrorHandle(
    const AutoPtr<ASTMethod> &method, const String &gotoLabel, bool isClient, StringBuilder &sb, const String &prefix)
{
    if (!isClient) {
        sb.Append(prefix).AppendFormat("%s:\n", gotoLabel.string());
        for (int i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            AutoPtr<ASTType> paramType = param->GetType();
            paramType->EmitMemoryRecycle(param->GetName(), isClient, true, sb, prefix + TAB);
        }
        return;
    }
}

void CCodeEmitter::EmitLicense(StringBuilder &sb)
{
    if (ast_->GetLicense().IsEmpty()) {
        return;
    }
    sb.Append(ast_->GetLicense()).Append("\n\n");
}

void CCodeEmitter::EmitHeadMacro(StringBuilder &sb, const String &fullName)
{
    String macroName = MacroName(fullName);
    sb.Append("#ifndef ").Append(macroName).Append("\n");
    sb.Append("#define ").Append(macroName).Append("\n");
}

void CCodeEmitter::EmitTailMacro(StringBuilder &sb, const String &fullName)
{
    String macroName = MacroName(fullName);
    sb.Append("#endif // ").Append(macroName);
}

void CCodeEmitter::EmitHeadExternC(StringBuilder &sb)
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("extern \"C\" {\n");
    sb.Append("#endif /* __cplusplus */\n");
}

void CCodeEmitter::EmitTailExternC(StringBuilder &sb)
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("}\n");
    sb.Append("#endif /* __cplusplus */\n");
}

String CCodeEmitter::EmitDescMacroName()
{
    return String::Format("%s_INTERFACE_DESC", interfaceName_.ToUpperCase().string());
}

String CCodeEmitter::MacroName(const String &name)
{
    if (name.IsEmpty()) {
        return name;
    }

    String macro = name.Replace('.', '_').ToUpperCase() + "_H";
    return macro;
}

String CCodeEmitter::SpecificationParam(StringBuilder &paramSb, const String &prefix)
{
    int maxLineLen = 120;
    int replaceLen = 2;
    String paramStr = paramSb.ToString();
    int preIndex = 0;
    int curIndex = 0;

    String insertStr = String::Format("\n%s", prefix.string());
    for (; curIndex < paramStr.GetLength(); curIndex++) {
        if (curIndex == maxLineLen && preIndex > 0) {
            paramStr.Replace(preIndex, replaceLen, ",");
            paramStr.insert(preIndex + 1, insertStr);
        } else {
            if (paramStr[curIndex] == ',') {
                preIndex = curIndex;
            }
        }
    }
    return paramStr;
}
} // namespace HDI
} // namespace OHOS