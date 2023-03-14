/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H
#define OHOS_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H

#include "codegen/c_code_emitter.h"

namespace OHOS {
namespace HDI {
class CInterfaceCodeEmitter : public CCodeEmitter {
public:
    CInterfaceCodeEmitter() : CCodeEmitter() {}

    virtual ~CInterfaceCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitInterfaceHeaderFile();

    void EmitImportInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitPreDeclaration(StringBuilder &sb);

    void EmitInterfaceDesc(StringBuilder &sb);

    void EmitInterfaceVersionMacro(StringBuilder &sb);

    void EmitInterfaceBuffSizeMacro(StringBuilder &sb);

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceMethods(StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethod(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitAsObjectMethod(StringBuilder &sb, const String &prefix);

    void EmitInterfaceGetMethodDecl(StringBuilder &sb);

    void EmitInterfaceReleaseMethodDecl(StringBuilder &sb);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H