/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_JAVA_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_HDI_JAVA_CLIENT_PROXY_CODE_EMITTER_H

#include "java_code_emitter.h"
#include "util/file.h"

namespace OHOS {
namespace HDI {
class JavaClientProxyCodeEmitter : public JavaCodeEmitter {
public:
    JavaClientProxyCodeEmitter() : JavaCodeEmitter() {}

    virtual ~JavaClientProxyCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitProxyFile();

    void EmitProxyImports(StringBuilder &sb);

    void EmitProxyCorelibImports(StringBuilder &sb);

    void EmitProxySelfDefinedTypeImports(StringBuilder &sb);

    void EmitProxyDBinderImports(StringBuilder &sb);

    void EmitProxyImpl(StringBuilder &sb);

    void EmitProxyConstants(StringBuilder &sb, const String &prefix);

    void EmitProxyConstructor(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodImpls(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitInterfaceMethodParameter(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix);

    void EmitProxyMethodBody(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitReadMethodParameter(
        const AutoPtr<ASTParameter> &param, const String &parcelName, StringBuilder &sb, const String &prefix);

    void EmitReadVariable(const String &parcelName, const String &name, const AutoPtr<ASTType> &type,
        ParamAttr attribute, StringBuilder &sb, const String &prefix);

    void EmitReadArrayVariable(const String &parcelName, const String &name, const AutoPtr<ASTArrayType> &arrayType,
        ParamAttr attribute, StringBuilder &sb, const String &prefix);

    void EmitReadOutArrayVariable(const String &parcelName, const String &name, const AutoPtr<ASTArrayType> &arrayType,
        StringBuilder &sb, const String &prefix);

    void EmitReadOutVariable(const String &parcelName, const String &name, const AutoPtr<ASTType> &type,
        StringBuilder &sb, const String &prefix);

    void EmitLocalVariable(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_JAVA_CLIENT_PROXY_CODE_EMITTER_H