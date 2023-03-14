/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_C_SERVICE_IMPL_CODE_EMITTER_H
#define OHOS_HDI_C_SERVICE_IMPL_CODE_EMITTER_H

#include "codegen/c_code_emitter.h"

namespace OHOS {
namespace HDI {
class CServiceImplCodeEmitter : public CCodeEmitter {
public:
    CServiceImplCodeEmitter() : CCodeEmitter() {}

    virtual ~CServiceImplCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitServiceImplHeaderFile();

    void EmitServiceImplHeaderInclusions(StringBuilder &sb);

    void EmitServiceImplConstructDecl(StringBuilder &sb);

    void EmitServiceImplSourceFile();

    void EmitServiceImplSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitServiceImplDef(StringBuilder &sb);

    void EmitServiceImplMethodImpls(StringBuilder &sb, const String &prefix);

    void EmitServiceImplMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitServiceImplGetMethod(StringBuilder &sb);

    void EmitServiceImplReleaseMethod(StringBuilder &sb);

    void EmitServiceImplInstance(StringBuilder &sb);

    void EmitServiceImplRelease(StringBuilder &sb);
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_C_SERVICE_IMPL_CODE_EMITTER_H