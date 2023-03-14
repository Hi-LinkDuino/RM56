/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_JAVA_CLIENT_INTERFACE_CODE_EMITTER_H
#define OHOS_HDI_JAVA_CLIENT_INTERFACE_CODE_EMITTER_H

#include "java_code_emitter.h"

namespace OHOS {
namespace HDI {
class JavaClientInterfaceCodeEmitter : public JavaCodeEmitter {
public:
    JavaClientInterfaceCodeEmitter() : JavaCodeEmitter() {}

    virtual ~JavaClientInterfaceCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitInterfaceFile();

    void EmitInterfaceImports(StringBuilder &sb);

    void EmitInterfaceCorelibImports(StringBuilder &sb);

    void EmitInterfaceDBinderImports(StringBuilder &sb);

    void EmitInterfaceSelfDefinedTypeImports(StringBuilder &sb);

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceMethods(StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethod(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethodParameter(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_JAVA_CLIENT_INTERFACE_CODE_EMITTER_H