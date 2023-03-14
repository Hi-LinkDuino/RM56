/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_FILE_DETAIL_H
#define OHOS_HDI_FILE_DETAIL_H

#include <unordered_set>

#include "util/options.h"
#include "util/string.h"

namespace OHOS {
namespace HDI {
class FileDetail {
public:
    using StringSet = std::unordered_set<String, StringHashFunc, StringEqualFunc>;

    void SetFilePath(const String &filePath);

    inline String GetFilePath() const
    {
        return idlFilePath_;
    }

    void SetPackageName(const String &packageName)
    {
        packageName_ = packageName;
    }

    inline String GetPackageName() const
    {
        return packageName_;
    }

    inline String GetName() const
    {
        return idlName_;
    }

    inline String GetFullName() const
    {
        return packageName_ + "." + idlName_;
    }

    bool AddImport(const String &packageName);

    void DelImport(const String &packageName)
    {
        imports_.erase(packageName);
    }

    inline size_t GetImportSize() const
    {
        return imports_.size();
    }

    inline const StringSet &GetImports() const
    {
        return imports_;
    }

    inline static String ImportsToPath(const String &importPkgName)
    {
        return Options::GetInstance().GetPackagePath(importPkgName) + ".idl";
    }

    String Dump();

private:
    String idlFilePath_;
    String rootPackageName_;
    String packageName_;
    String idlName_;
    StringSet imports_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_FILE_DETAIL_H