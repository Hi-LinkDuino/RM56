/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "codegen/code_generator.h"
#include "codegen/c_client_proxy_code_emitter.h"
#include "codegen/c_custom_types_code_emitter.h"
#include "codegen/c_interface_code_emitter.h"
#include "codegen/c_service_driver_code_emitter.h"
#include "codegen/c_service_impl_code_emitter.h"
#include "codegen/c_service_stub_code_emitter.h"
#include "codegen/cpp_client_proxy_code_emitter.h"
#include "codegen/cpp_custom_types_code_emitter.h"
#include "codegen/cpp_interface_code_emitter.h"
#include "codegen/cpp_service_driver_code_emitter.h"
#include "codegen/cpp_service_impl_code_emitter.h"
#include "codegen/cpp_service_stub_code_emitter.h"
#include "codegen/java_client_interface_code_emitter.h"
#include "codegen/java_client_proxy_code_emitter.h"
#include "util/options.h"

namespace OHOS {
namespace HDI {
CodeEmitMap CodeGenerator::cCodeEmitters_ = {
    {"types",     new CCustomTypesCodeEmitter()  },
    {"interface", new CInterfaceCodeEmitter()    },
    {"proxy",     new CClientProxyCodeEmitter()  },
    {"driver",    new CServiceDriverCodeEmitter()},
    {"stub",      new CServiceStubCodeEmitter()  },
    {"impl",      new CServiceImplCodeEmitter()  },
};

CodeEmitMap CodeGenerator::cppCodeEmitters_ = {
    {"types",     new CppCustomTypesCodeEmitter()  },
    {"interface", new CppInterfaceCodeEmitter()    },
    {"proxy",     new CppClientProxyCodeEmitter()  },
    {"driver",    new CppServiceDriverCodeEmitter()},
    {"stub",      new CppServiceStubCodeEmitter()  },
    {"impl",      new CppServiceImplCodeEmitter()  },
};

CodeEmitMap CodeGenerator::javaCodeEmitters_ = {
    {"clientIface", new JavaClientInterfaceCodeEmitter()},
    {"proxy",       new JavaClientProxyCodeEmitter()    },
};

bool CodeGenerator::Generate()
{
    const Options &options = Options::GetInstance();
    String dir = options.GetGenerationDirectory();
    String language = options.GetTargetLanguage();
    bool isModeKernel = options.DoGenerateKernelCode();
    String codePart = options.GetCodePart();

    for (auto &astPair : astModule_->GetAllAsts()) {
        AutoPtr<AST> ast = astPair.second;
        if (language.Equals("c")) {
            GenerateCCode(ast, dir, codePart, isModeKernel);
        } else if (language.Equals("cpp")) {
            GenerateCppCode(ast, dir, codePart);
        } else if (language.Equals("java")) {
            GenerateJavaCode(ast, dir, codePart);
        }
    }

    return true;
}

void CodeGenerator::GenerateCCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart, bool isKernel)
{
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cCodeEmitters_["types"]->OutPut(ast, outDir, isKernel);
            break;
        }
        case ASTFileType::AST_IFACE: {
            if (codePart.Equals("client")) {
                cCodeEmitters_["interface"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["proxy"]->OutPut(ast, outDir, isKernel);
                break;
            } else if (codePart.Equals("server")) {
                cCodeEmitters_["interface"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["driver"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["stub"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["impl"]->OutPut(ast, outDir, isKernel);
                break;
            } else {
                cCodeEmitters_["interface"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["proxy"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["driver"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["stub"]->OutPut(ast, outDir, isKernel);
                cCodeEmitters_["impl"]->OutPut(ast, outDir, isKernel);
            }
            break;
        }
        case ASTFileType::AST_ICALLBACK:
            // khdf doesn't support callback
            cCodeEmitters_["interface"]->OutPut(ast, outDir);
            cCodeEmitters_["proxy"]->OutPut(ast, outDir);
            cCodeEmitters_["driver"]->OutPut(ast, outDir);
            cCodeEmitters_["stub"]->OutPut(ast, outDir);
            cCodeEmitters_["impl"]->OutPut(ast, outDir);
            break;
        default:
            break;
    }
}

void CodeGenerator::GenerateCppCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart)
{
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES:
            cppCodeEmitters_["types"]->OutPut(ast, outDir);
            break;
        case ASTFileType::AST_IFACE: {
            if (codePart.Equals("client")) {
                cppCodeEmitters_["interface"]->OutPut(ast, outDir);
                cppCodeEmitters_["proxy"]->OutPut(ast, outDir);
            } else if (codePart.Equals("server")) {
                cppCodeEmitters_["interface"]->OutPut(ast, outDir);
                cppCodeEmitters_["driver"]->OutPut(ast, outDir);
                cppCodeEmitters_["stub"]->OutPut(ast, outDir);
                cppCodeEmitters_["impl"]->OutPut(ast, outDir);
            } else {
                cppCodeEmitters_["interface"]->OutPut(ast, outDir);
                cppCodeEmitters_["proxy"]->OutPut(ast, outDir);
                cppCodeEmitters_["driver"]->OutPut(ast, outDir);
                cppCodeEmitters_["stub"]->OutPut(ast, outDir);
                cppCodeEmitters_["impl"]->OutPut(ast, outDir);
            }
            break;
        }
        case ASTFileType::AST_ICALLBACK:
            cppCodeEmitters_["interface"]->OutPut(ast, outDir);
            cppCodeEmitters_["proxy"]->OutPut(ast, outDir);
            cppCodeEmitters_["driver"]->OutPut(ast, outDir);
            cppCodeEmitters_["stub"]->OutPut(ast, outDir);
            cppCodeEmitters_["impl"]->OutPut(ast, outDir);
            break;
        default:
            break;
    }
}

void CodeGenerator::GenerateJavaCode(const AutoPtr<AST> &ast, const String &outDir, const String &codePart)
{
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_IFACE:
            javaCodeEmitters_["clientIface"]->OutPut(ast, outDir);
            javaCodeEmitters_["proxy"]->OutPut(ast, outDir);
            break;
        case ASTFileType::AST_ICALLBACK:
            javaCodeEmitters_["clientIface"]->OutPut(ast, outDir);
            javaCodeEmitters_["proxy"]->OutPut(ast, outDir);
            break;
        default:
            break;
    }
}
} // namespace HDI
} // namespace OHOS