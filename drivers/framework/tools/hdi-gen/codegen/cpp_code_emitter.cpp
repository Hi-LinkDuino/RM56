/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/cpp_code_emitter.h"

#include <regex>
#include <unordered_set>

#include "util/options.h"

namespace OHOS {
namespace HDI {
void CppCodeEmitter::GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    bool includeString = false;
    bool includeList = false;
    bool includeMap = false;
    bool includeSmq = false;

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_STRING: {
                if (!includeString) {
                    headerFiles.emplace(HeaderFile(HeaderFileType::CPP_STD_HEADER_FILE, "string"));
                    includeString = true;
                }
                break;
            }
            case TypeKind::TYPE_ARRAY:
            case TypeKind::TYPE_LIST: {
                if (!includeList) {
                    headerFiles.emplace(HeaderFile(HeaderFileType::CPP_STD_HEADER_FILE, "vector"));
                    includeList = true;
                }
                break;
            }
            case TypeKind::TYPE_MAP: {
                if (!includeMap) {
                    headerFiles.emplace(HeaderFile(HeaderFileType::CPP_STD_HEADER_FILE, "map"));
                    includeMap = true;
                }
                break;
            }
            case TypeKind::TYPE_SMQ: {
                if (!includeSmq) {
                    headerFiles.emplace(HeaderFile(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdi_smq"));
                    includeSmq = true;
                }
            }
            default:
                break;
        }
    }
}

void CppCodeEmitter::GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        String fileName = PackageToFilePath(importAst->GetFullName());
        headerFiles.emplace(HeaderFile(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName));
    }
}

void CppCodeEmitter::EmitInterfaceMethodParameter(
    const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat(param->EmitCppParameter());
}

void CppCodeEmitter::EmitLicense(StringBuilder &sb)
{
    if (ast_->GetLicense().IsEmpty()) {
        return;
    }
    sb.Append(ast_->GetLicense()).Append("\n\n");
}

void CppCodeEmitter::EmitHeadMacro(StringBuilder &sb, const String &fullName)
{
    String macroName = MacroName(fullName);
    sb.Append("#ifndef ").Append(macroName).Append("\n");
    sb.Append("#define ").Append(macroName).Append("\n");
}

void CppCodeEmitter::EmitTailMacro(StringBuilder &sb, const String &fullName)
{
    String macroName = MacroName(fullName);
    sb.Append("#endif // ").Append(macroName);
}

void CppCodeEmitter::EmitHeadExternC(StringBuilder &sb)
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("extern \"C\" {\n");
    sb.Append("#endif /* __cplusplus */\n");
}

void CppCodeEmitter::EmitTailExternC(StringBuilder &sb)
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("}\n");
    sb.Append("#endif /* __cplusplus */\n");
}

bool CppCodeEmitter::isVersion(const String &name)
{
    std::regex rVer("[V|v][0-9]+_[0-9]+");
    return std::regex_match(name.string(), rVer);
}

std::vector<String> CppCodeEmitter::EmitCppNameSpaceVec(const String &namespaceStr)
{
    std::vector<String> result;
    std::vector<String> namespaceVec = namespaceStr.Split(".");
    bool findVersion = false;

    String rootPackage = Options::GetInstance().GetRootPackage(namespaceStr);
    size_t rootPackageNum = rootPackage.Split(".").size();

    for (size_t i = 0; i < namespaceVec.size(); i++) {
        String name;
        if (i < rootPackageNum) {
            name = namespaceVec[i].ToUpperCase();
        } else if (!findVersion && isVersion(namespaceVec[i])) {
            name = namespaceVec[i].Replace('v', 'V');
            findVersion = true;
        } else {
            if (findVersion) {
                name = namespaceVec[i];
            } else {
                name = PascalName(namespaceVec[i]);
            }
        }

        result.emplace_back(name);
    }
    return result;
}

String CppCodeEmitter::EmitPackageToNameSpace(const String &packageName)
{
    if (packageName.IsEmpty()) {
        return packageName;
    }

    StringBuilder nameSpaceStr;
    std::vector<String> namespaceVec = EmitCppNameSpaceVec(packageName);
    for (auto nameIter = namespaceVec.begin(); nameIter != namespaceVec.end(); nameIter++) {
        nameSpaceStr.Append(*nameIter);
        if (nameIter != namespaceVec.end() - 1) {
            nameSpaceStr.Append("::");
        }
    }

    return nameSpaceStr.ToString();
}

void CppCodeEmitter::EmitBeginNamespace(StringBuilder &sb)
{
    std::vector<String> cppNamespaceVec = EmitCppNameSpaceVec(interface_->GetNamespace()->ToString());
    for (const auto &nspace : cppNamespaceVec) {
        sb.AppendFormat("namespace %s {\n", nspace.string());
    }
}

void CppCodeEmitter::EmitEndNamespace(StringBuilder &sb)
{
    std::vector<String> cppNamespaceVec = EmitCppNameSpaceVec(interface_->GetNamespace()->ToString());

    for (auto nspaceIter = cppNamespaceVec.rbegin(); nspaceIter != cppNamespaceVec.rend(); nspaceIter++) {
        sb.AppendFormat("} // %s\n", nspaceIter->string());
    }
}

void CppCodeEmitter::EmitUsingNamespace(StringBuilder &sb)
{
    sb.Append("using namespace OHOS;\n");
    EmitImportUsingNamespace(sb);
}

String CppCodeEmitter::EmitNamespace(const String &packageName)
{
    if (packageName.IsEmpty()) {
        return packageName;
    }

    int index = packageName.LastIndexOf('.');
    return index > 0 ? packageName.Substring(0, index) : packageName;
}

void CppCodeEmitter::EmitImportUsingNamespace(StringBuilder &sb)
{
    using StringSet = std::unordered_set<String, StringHashFunc, StringEqualFunc>;
    StringSet namespaceSet;
    String selfNameSpace = EmitPackageToNameSpace(EmitNamespace(ast_->GetFullName()));

    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> import = importPair.second;
        String nameSpace = EmitPackageToNameSpace(EmitNamespace(import->GetFullName()));
        if (nameSpace.Equals(selfNameSpace)) {
            continue;
        }
        namespaceSet.emplace(nameSpace);
    }

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_SMQ) {
            namespaceSet.emplace("OHOS::HDI::Base");
            break;
        }
    }

    for (const auto &nspace : namespaceSet) {
        sb.Append("using namespace ").AppendFormat("%s;\n", nspace.string());
    }
}

void CppCodeEmitter::EmitWriteMethodParameter(
    const AutoPtr<ASTParameter> &param, const String &parcelName, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();
    type->EmitCppWriteVar(parcelName, param->GetName(), sb, prefix);
}

void CppCodeEmitter::EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, const String &parcelName,
    bool initVariable, StringBuilder &sb, const String &prefix)
{
    AutoPtr<ASTType> type = param->GetType();
    type->EmitCppReadVar(parcelName, param->GetName(), sb, prefix, initVariable);
}

String CppCodeEmitter::MacroName(const String &name)
{
    if (name.IsEmpty()) {
        return name;
    }

    String macro = name.Replace('.', '_').ToUpperCase() + "_H";
    return macro;
}

String CppCodeEmitter::SpecificationParam(StringBuilder &paramSb, const String &prefix)
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