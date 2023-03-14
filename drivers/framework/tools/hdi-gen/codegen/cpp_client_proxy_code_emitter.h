/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CPP_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_HDI_CPP_CLIENT_PROXY_CODE_EMITTER_H

#include "codegen/cpp_code_emitter.h"

namespace OHOS {
namespace HDI {
class CppClientProxyCodeEmitter : public CppCodeEmitter {
public:
    CppClientProxyCodeEmitter() : CppCodeEmitter() {}

    virtual ~CppClientProxyCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitProxyHeaderFile();

    void EmitProxyHeaderInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitProxyDecl(StringBuilder &sb, const String &prefix);

    void EmitProxyConstructor(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodDecls(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitProxyConstants(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodParameter(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const String &prefix);

    void EmitProxySourceFile();

    void EmitProxySourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitGetMethodImpl(StringBuilder &sb, const String &prefix);

    void EmitGetInstanceMethodImpl(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodImpls(StringBuilder &sb, const String &prefix);

    void EmitProxyMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitProxyMethodBody(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitWriteInterfaceToken(const String &parcelName, StringBuilder &sb, const String &prefix);

    void EmitWriteFlagOfNeedSetMem(
        const AutoPtr<ASTMethod> &method, const String &dataBufName, StringBuilder &sb, const String &prefix);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CPP_CLIENT_PROXY_CODE_EMITTER_H