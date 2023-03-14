/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_C_CODE_EMITTER_H
#define OHOS_HDI_C_CODE_EMITTER_H

#include "ast/ast.h"
#include "codegen/code_emitter.h"
#include "util/autoptr.h"
#include "util/string.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
class CCodeEmitter : public CodeEmitter {
public:
    virtual ~CCodeEmitter() = default;

protected:
    void GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitInterfaceMethodParameter(const AutoPtr<ASTParameter> &parameter, StringBuilder &sb, const String &prefix);

    void EmitInitLoopVar(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const String &prefix);

    void EmitErrorHandle(const AutoPtr<ASTMethod> &method, const String &gotoLabel, bool isClient, StringBuilder &sb,
        const String &prefix);

    void EmitLicense(StringBuilder &sb);

    void EmitHeadMacro(StringBuilder &sb, const String &fullName);

    void EmitTailMacro(StringBuilder &sb, const String &fullName);

    void EmitHeadExternC(StringBuilder &sb);

    void EmitTailExternC(StringBuilder &sb);

    String EmitDescMacroName();

    String MacroName(const String &name);

    String ConstantName(const String &name);

    String SpecificationParam(StringBuilder &sb, const String &prefix);

    static constexpr const char *errorsLabelName_ = "ERRORS";
    static constexpr const char *finishedLabelName_ = "FINISHED";
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_C_CODE_EMITTER_H