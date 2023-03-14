/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_CODE_EMITTER_H
#define OHOS_HDI_CODE_EMITTER_H

#include <set>

#include "ast/ast.h"
#include "util/autoptr.h"
#include "util/light_refcount_base.h"
#include "util/string.h"

namespace OHOS {
namespace HDI {
enum class HeaderFileType {
    OWN_HEADER_FILE,
    SYSTEM_HEADER_FILE,
    C_STD_HEADER_FILE,
    CPP_STD_HEADER_FILE,
    OTHER_MODULES_HEADER_FILE,
    OWN_MODULE_HEADER_FILE,
};

struct HeaderFile {
    HeaderFile(HeaderFileType type, String fileName) : type_(type), fileName_(fileName) {}

    struct compare {
        bool operator()(const HeaderFile &lhs, const HeaderFile &rhs)
        {
            if (lhs.type_ < rhs.type_) {
                return true;
            } else if (lhs.type_ > rhs.type_) {
                return false;
            }

            return lhs.fileName_.Compare(rhs.fileName_) <= 0;
        }
    };

    String ToString() const
    {
        switch (type_) {
            case HeaderFileType::OWN_HEADER_FILE:
            case HeaderFileType::OWN_MODULE_HEADER_FILE:
                return String::Format("#include \"%s.h\"", fileName_.string());
            case HeaderFileType::SYSTEM_HEADER_FILE:
            case HeaderFileType::C_STD_HEADER_FILE:
            case HeaderFileType::OTHER_MODULES_HEADER_FILE:
                return String::Format("#include <%s.h>", fileName_.string());
            case HeaderFileType::CPP_STD_HEADER_FILE:
                return String::Format("#include <%s>", fileName_.string());
            default:
                return String::Format("//");
        }
    }

    using HeaderFileSet = std::set<HeaderFile, HeaderFile::compare>;

    HeaderFileType type_;
    String fileName_;
};

class CodeEmitter : public LightRefCountBase {
public:
    virtual ~CodeEmitter() = default;

    bool OutPut(const AutoPtr<AST> &ast, const String &targetDirectory, bool isKernelCode = false);

protected:
    bool Reset(const AutoPtr<AST> &ast, const String &targetDirectory, bool isKernelCode);

    void CleanData();

    virtual bool ResolveDirectory(const String &targetDirectory) = 0;

    virtual void EmitCode() = 0;

    bool NeedFlag(const AutoPtr<ASTMethod> &method);

    String GetFilePath(const String &outDir);

    String PackageToFilePath(const String &packageName);

    String EmitMethodCmdID(const AutoPtr<ASTMethod> &method);

    void EmitInterfaceMethodCommands(StringBuilder &sb, const String &prefix);

    /* ForExample:
     * MajorVersion: 1
     * MinorVersion: 0
     * name: IFoo
     * result: v1_0/ifoo.h
     */
    String EmitVersionHeaderName(const String &name);

    // file_name -> FILE_NAME
    String ConstantName(const String &name);

    // file_name -> FileName
    String PascalName(const String &name);

    // FileName -> file_name
    String FileName(const String &name);

protected:
    static constexpr char* TAB = "    ";
    bool isKernelCode_ = false;
    AutoPtr<AST> ast_ = nullptr;
    AutoPtr<ASTInterfaceType> interface_ = nullptr;
    String directory_;

    String interfaceName_;
    String interfaceFullName_;
    String baseName_;
    String proxyName_;
    String proxyFullName_;
    String stubName_;
    String stubFullName_;
    String implName_;
    String implFullName_;
    String majorVerName_;
    String minorVerName_;
    String bufferSizeMacroName_;

    String dataParcelName_;
    String replyParcelName_;
    String optionName_;
    String errorCodeName_;
    String flagOfSetMemName_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_CODE_EMITTER_H
