/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CPP_CUSTOM_TYPES_CODE_EMITTER_H
#define OHOS_HDI_CPP_CUSTOM_TYPES_CODE_EMITTER_H

#include "codegen/cpp_code_emitter.h"

namespace OHOS {
namespace HDI {
class CppCustomTypesCodeEmitter : public CppCodeEmitter {
public:
    CppCustomTypesCodeEmitter() : CppCodeEmitter() {}

    virtual ~CppCustomTypesCodeEmitter() = default;

private:
    bool ResolveDirectory(const String &targetDirectory) override;

    void EmitCode() override;

    void EmitCustomTypesHeaderFile();

    void EmitHeaderFileInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitCustomTypeDecls(StringBuilder &sb);

    void EmitCustomTypeDecl(StringBuilder &sb, const AutoPtr<ASTType> &type);

    void EmitCustomTypeFuncDecl(StringBuilder &sb);

    void EmitCustomTypeMarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitCustomTypeUnmarshallingDecl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitCustomTypesSourceFile();

    void EmitSourceFileInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitCustomTypeDataProcess(StringBuilder &sb);

    void EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitBeginNamespace(StringBuilder &sb) override;

    void EmitEndNamespace(StringBuilder &sb) override;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CPP_CUSTOM_TYPES_CODE_EMITTER_H