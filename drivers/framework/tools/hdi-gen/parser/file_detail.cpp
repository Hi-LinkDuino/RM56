/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "parser/file_detail.h"
#include "util/file.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
void FileDetail::SetFilePath(const String &filePath)
{
    idlFilePath_ = filePath;
#ifdef __MINGW32__
    int index = idlFilePath_.LastIndexOf('\\');
#else
    int index = idlFilePath_.LastIndexOf('/');
#endif
    int end = idlFilePath_.LastIndexOf(".idl");
    idlName_ = idlFilePath_.Substring((index == -1) ? 0 : (index + 1), end);
}

bool FileDetail::AddImport(const String &packageName)
{
    if (imports_.find(packageName) != imports_.end()) {
        return false;
    }

    imports_.emplace(packageName);
    return true;
}

String FileDetail::Dump()
{
    StringBuilder sb;
    sb.AppendFormat("filePath:%s\n", idlFilePath_.string());
    sb.AppendFormat("package:%s\n", packageName_.string());
    sb.AppendFormat("import number:%d\n", imports_.size());
    for (const auto &importName : imports_) {
        sb.AppendFormat("import %s\n", importName.string());
    }
    return sb.ToString();
}
} // namespace HDI
} // namespace OHOS