/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_DECOMPILE_GEN_H
#define HC_GEN_DECOMPILE_GEN_H

#include <fstream>
#include <string>

#include "ast.h"

namespace OHOS {
namespace Hardware {
class DecompileGen {
public:
    DecompileGen(std::shared_ptr<Ast> ast, std::string outPutFileName);

    ~DecompileGen() = default;

    bool OutPut();

private:
    bool Init();

    void WriteFile(const std::string &str);

    int32_t PrintBaseType(const std::shared_ptr<AstObject> &astObj);

    std::string GetNodeRefPath(uint32_t hash);

    int32_t PrintArrayType(const std::shared_ptr<AstObject> &astObj);

    int32_t OutPutWalk(const std::shared_ptr<AstObject> &astObj, int32_t walkDepth);

    int32_t CloseBrace(const std::shared_ptr<AstObject> &astObj, int32_t walkDepth);

    const std::string fileHeader_ = "/*\n * HDF decompile hcs file\n */\n\n";
    std::string outPutFileName_;
    std::ofstream file_;
    std::shared_ptr<Ast> ast_;
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_DECOMPILE_GEN_H
