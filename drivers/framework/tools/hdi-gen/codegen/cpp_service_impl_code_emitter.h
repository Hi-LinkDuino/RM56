/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CPP_SERVICE_IMPL_CODE_EMITTER_H
#define OHOS_HDI_CPP_SERVICE_IMPL_CODE_EMITTER_H

#include "codegen/cpp_code_emitter.h"

namespace OHOS {
namespace HDI {
class CppServiceImplCodeEmitter : public CppCodeEmitter {
public:
    CppServiceImplCodeEmitter() : CppCodeEmitter() {}

    virtual ~CppServiceImplCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitImplHeaderFile();

    void EmitServiceImplInclusions(StringBuilder &sb);

    void EmitServiceImplDecl(StringBuilder &sb);

    void EmitServiceImplBody(StringBuilder &sb, const String &prefix);

    void EmitServiceImplDestruction(StringBuilder &sb, const String &prefix);

    void EmitServiceImplMethodDecls(StringBuilder &sb, const String &prefix);

    void EmitServiceImplMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitImplSourceFile();

    void EmitImplSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitServiceImplMethodImpls(StringBuilder &sb, const String &prefix);

    void EmitServiceImplMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitExternalGetMethodImpl(StringBuilder &sb);

    void EmitExternalReleaseMethodImpl(StringBuilder &sb);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CPP_SERVICE_IMPL_CODE_EMITTER_H