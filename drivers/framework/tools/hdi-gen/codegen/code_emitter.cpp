/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "codegen/code_emitter.h"

#include <cctype>

#include "util/file.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
bool CodeEmitter::OutPut(const AutoPtr<AST> &ast, const String &targetDirectory, bool isKernelCode)
{
    if (!Reset(ast, targetDirectory, isKernelCode)) {
        return false;
    }

    EmitCode();
    return true;
}

bool CodeEmitter::Reset(const AutoPtr<AST> &ast, const String &targetDirectory, bool isKernelCode)
{
    if (ast == nullptr || targetDirectory.Equals("")) {
        return false;
    }

    CleanData();

    isKernelCode_ = isKernelCode;
    ast_ = ast;
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        interface_ = ast_->GetInterfaceDef();
        interfaceName_ = interface_->GetName();
        interfaceFullName_ = interface_->GetNamespace()->ToString() + interfaceName_;
        baseName_ = interfaceName_.StartsWith("I") ? interfaceName_.Substring(1) : interfaceName_;
        proxyName_ = baseName_ + "Proxy";
        proxyFullName_ = interface_->GetNamespace()->ToString() + proxyName_;

        stubName_ = baseName_ + "Stub";
        stubFullName_ = interface_->GetNamespace()->ToString() + stubName_;

        implName_ = baseName_ + "Service";
        implFullName_ = interface_->GetNamespace()->ToString() + implName_;
    } else if (ast_->GetASTFileType() == ASTFileType::AST_TYPES) {
        baseName_ = ast_->GetName();
    }

    majorVerName_ = String::Format("%s_MAJOR_VERSION", ConstantName(interfaceName_).string());
    minorVerName_ = String::Format("%s_MINOR_VERSION", ConstantName(interfaceName_).string());
    bufferSizeMacroName_ = String::Format("%s_BUFF_MAX_SIZE", ConstantName(baseName_).string());

    String prefix = String::Format("%c%s", tolower(baseName_[0]), baseName_.Substring(1).string());
    dataParcelName_ = prefix + "Data";
    replyParcelName_ = prefix + "Reply";
    optionName_ = prefix + "Option";
    errorCodeName_ = prefix + "Ret";
    flagOfSetMemName_ = prefix + "MemSet";

    if (!ResolveDirectory(targetDirectory)) {
        return false;
    }

    return true;
}

void CodeEmitter::CleanData()
{
    isKernelCode_ = false;
    ast_ = nullptr;
    interface_ = nullptr;
    directory_ = "";
    interfaceName_ = "";
    interfaceFullName_ = "";
    baseName_ = "";
    proxyName_ = "";
    proxyFullName_ = "";
    stubName_ = "";
    stubFullName_ = "";
    implName_ = "";
    implFullName_ = "";
    dataParcelName_ = "";
    replyParcelName_ = "";
    optionName_ = "";
    errorCodeName_ = "";
}

bool CodeEmitter::NeedFlag(const AutoPtr<ASTMethod> &method)
{
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        AutoPtr<ASTType> type = param->GetType();
        if (param->GetAttribute() == ParamAttr::PARAM_OUT && type->IsStringType() || type->IsArrayType() ||
            type->IsListType()) {
            return true;
        }
    }
    return false;
}

String CodeEmitter::GetFilePath(const String &outDir)
{
    String outPath = outDir.EndsWith(File::pathSeparator) ? outDir.Substring(0, outDir.GetLength() - 1) : outDir;
    String packagePath = Options::GetInstance().GetPackagePath(ast_->GetPackageName());
    if (packagePath.EndsWith(File::pathSeparator)) {
        return String::Format("%s/%s", outPath.string(), packagePath.string());
    } else {
        return String::Format("%s/%s/", outPath.string(), packagePath.string());
    }
}

String CodeEmitter::PackageToFilePath(const String &packageName)
{
    std::vector<String> packageVec = Options::GetInstance().GetSubPackage(packageName).Split(".");
    StringBuilder filePath;
    for (auto iter = packageVec.begin(); iter != packageVec.end(); iter++) {
        filePath.Append(FileName(*iter));
        if (iter != packageVec.end() - 1) {
            filePath.Append("/");
        }
    }

    return filePath.ToString();
}

String CodeEmitter::EmitMethodCmdID(const AutoPtr<ASTMethod> &method)
{
    return String::Format("CMD_%s_%s", ConstantName(baseName_).string(), ConstantName(method->GetName()).string());
}

void CodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const String &prefix)
{
    sb.Append(prefix).AppendFormat("enum {\n");
    for (size_t i = 0; i < interface_->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(prefix + TAB).Append(EmitMethodCmdID(method)).Append(",\n");
    }

    sb.Append(prefix + TAB).Append(EmitMethodCmdID(interface_->GetVersionMethod())).Append(",\n");
    sb.Append(prefix).Append("};\n");
}

String CodeEmitter::EmitVersionHeaderName(const String &name)
{
    return String::Format("v%u_%u/%s", ast_->GetMajorVer(), ast_->GetMinorVer(), FileName(name).string());
}

String CodeEmitter::ConstantName(const String &name)
{
    if (name.IsEmpty()) {
        return name;
    }

    StringBuilder sb;

    for (int i = 0; i < name.GetLength(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            if (i > 1) {
                sb.Append('_');
            }
            sb.Append(c);
        } else {
            sb.Append(toupper(c));
        }
    }

    return sb.ToString();
}

String CodeEmitter::PascalName(const String &name)
{
    if (name.IsEmpty()) {
        return name;
    }

    StringBuilder sb;
    for (int i = 0; i < name.GetLength(); i++) {
        char c = name[i];
        if (i == 0) {
            if (islower(c)) {
                c = toupper(c);
            }
            sb.Append(c);
        } else {
            if (c == '_') {
                continue;
            }

            if (islower(c) && name[i - 1] == '_') {
                c = toupper(c);
            }
            sb.Append(c);
        }
    }

    return sb.ToString();
}

String CodeEmitter::FileName(const String &name)
{
    if (name.IsEmpty()) {
        return name;
    }

    StringBuilder sb;
    for (int i = 0; i < name.GetLength(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            // 2->Index of the last char array.
            if (i > 1) {
                sb.Append('_');
            }
            sb.Append(tolower(c));
        } else {
            sb.Append(c);
        }
    }

    return sb.ToString();
}
} // namespace HDI
} // namespace OHOS
