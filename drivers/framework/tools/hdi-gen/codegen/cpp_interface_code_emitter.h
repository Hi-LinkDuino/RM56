/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CPP_INTERFACE_CODE_EMITTER_H
#define OHOS_HDI_CPP_INTERFACE_CODE_EMITTER_H

#include "codegen/cpp_code_emitter.h"

namespace OHOS {
namespace HDI {
class CppInterfaceCodeEmitter : public CppCodeEmitter {
public:
    CppInterfaceCodeEmitter() : CppCodeEmitter() {}

    virtual ~CppInterfaceCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitInterfaceHeaderFile();

    void EmitInterfaceInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitInterfaceVersionMacro(StringBuilder &sb);

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceDescriptor(StringBuilder &sb, const String &prefix);

    void EmitGetMethodDecl(StringBuilder &sb, const String &prefix);

    void EmitGetInstanceMethodDecl(StringBuilder &sb, const String &prefix);

    void EmitInterfaceDestruction(StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethodsDecl(StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethodParameter(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CPP_INTERFACE_CODE_EMITTER_H