import os
import argparse

SRC_DIRS = ["src", "crt", "ldso"]

def file_search(path, target_type):
    base_srcs = []
    if not os.path.isdir(path):
        return base_srcs
    src_list = os.listdir(path)
    for src in src_list:
        cur_path = os.path.join(path, src)
        if os.path.isdir(cur_path):
            # base_srcs += file_search(cur_path, target_type)
            continue
        elif target_type == "header":
            if (cur_path.endswith(".h")):
                base_srcs.append(cur_path)
        elif target_type == "src":
            if (cur_path.endswith(".c")):
                base_srcs.append(cur_path)
    return base_srcs

def arch_file_search(path, arch):
    arch_srcs = []
    target_path = path + "/" + arch
    if os.path.isdir(target_path):
        src_list = os.listdir(target_path)
        for src in src_list:
            cur_path = os.path.join(target_path, src)
            if os.path.isdir(cur_path):
                # arch_srcs += file_search(cur_path, target_type)
                continue
            elif (cur_path.endswith(".c") or cur_path.endswith(".s") or
                  cur_path.endswith(".S")):
                arch_srcs.append(cur_path)  
    return arch_srcs

def get_base_srcs(path):
    base_srcs = []
    for src_dir in SRC_DIRS:
        src_path = path + "/" + src_dir
        src_list = os.listdir(src_path)
        for src in src_list:
            cur_path = os.path.join(src_path, src)
            if os.path.isdir(cur_path) and src_dir == "src":
                base_srcs += file_search(cur_path, "src")
            else:
                if (cur_path.endswith(".c")):
                    base_srcs.append(cur_path)
    base_srcs.sort()
    return base_srcs

def get_arch_srcs(path, arch):
    arch_srcs = []
    for src_dir in SRC_DIRS:
        if src_dir == "src":
            src_list = os.listdir(path + "/" + src_dir)
            for src in src_list:
                cur_path = os.path.join(path + "/" + src_dir, src)
                arch_srcs += arch_file_search(cur_path, arch)
        elif os.path.isdir(path + "/" + src_dir + "/" + arch):
            src_path = path + "/" + src_dir + "/" + arch
            src_list = os.listdir(src_path)
            for src in src_list:
                cur_path = os.path.join(src_path, src)
                if os.path.isdir(cur_path):
                    continue
                else:
                    if (cur_path.endswith(".c") or cur_path.endswith(".s") or
                        cur_path.endswith(".S")):
                        arch_srcs.append(cur_path)
    arch_srcs.sort()
    return arch_srcs

def change_subfix(name):
    new_name = ""
    if name.endswith(".s"):
        new_name = name.replace(".s", ".c")
    elif name.endswith(".S"):
        new_name = name.replace(".S", ".c")
    else:
        return name
    return new_name

def replace_srcs_with_arch(base_srcs, arch_srcs, arch):
    target_srcs = base_srcs
    arch_removed = []
    for src in arch_srcs:
        fake_name = change_subfix(src.replace("/" + arch, ""))
        if fake_name in target_srcs:
            target_srcs.remove(fake_name)
            arch_removed.append(src)
    return target_srcs, arch_removed

def get_libc_srcs(target_srcs, relative_path):
    libc_srcs = []
    for src in target_srcs:
        clean_path_src = src.replace(relative_path, "")
        if clean_path_src.startswith("/src/") or clean_path_src.startswith("/compat/time32/"):
            libc_srcs.append(src)
    return libc_srcs

def get_ldso_srcs(target_srcs, relative_path):
    ldso_srcs = []
    for src in target_srcs:
        clean_path_src = src.replace(relative_path, "")
        if clean_path_src.startswith("/ldso/"):
            ldso_srcs.append(src)
    return ldso_srcs

def get_crt_srcs(target_srcs, relative_path):
    crt_srcs = []
    for src in target_srcs:
        clean_path_src = src.replace(relative_path, "")
        if clean_path_src.startswith("/crt/"):
            crt_srcs.append(src)
    return crt_srcs

def get_header_dirs(path, arch):
    header_dirs = {
        path + "/arch/" + arch + "/bits",
        path + "/arch/generic/bits",
        path + "/include"
    }
    return header_dirs

def get_generated_header(path):
    generated_headers = [
        path + "/include/bits/alltypes.h",
        path + "/include/bits/syscall.h",
    ]

def get_all_header(header_dirs):
    all_header = []
    for dir in header_dirs:
        file_list = os.listdir(dir)
        for file in file_list:
            cur_path = os.path.join(dir, file)
            if os.path.isdir(cur_path):
                all_header += file_search(cur_path, "header")
            else:
                if (cur_path.endswith(".h")):
                    all_header.append(cur_path)
    all_header.sort()
    return all_header

def rm_dup_header(all_header_files, arch):
    header_files = all_header_files
    header_to_rm = []
    for file in header_files:
        if "/arch/generic/bits" in file:
            substitute_file = file.replace("/arch/generic/bits", "/arch/"+ arch +"/bits")
            if substitute_file in header_files:
                header_to_rm.append(file)

    return  [i for i in header_files if i not in header_to_rm]

def print_outputs(outputs):
    for out in outputs:
        print(out)
        for content in outputs[out]:
            print("    " + content)

def search_for_files(path, arch):
    if arch == "arm":
        SRC_DIRS.append("compat/time32")

    base_srcs = get_base_srcs(path)
    arch_srcs = get_arch_srcs(path, arch)
    target_srcs, arch_removed = replace_srcs_with_arch(base_srcs, arch_srcs, arch)
    target_srcs += arch_srcs
    
    libc_srcs = get_libc_srcs(target_srcs, path)
    ldso_srcs = get_ldso_srcs(target_srcs, path)
    crt_srcs = get_crt_srcs(target_srcs, path)

    all_header_files = get_all_header(get_header_dirs(path, arch))
    header_files = rm_dup_header(all_header_files, arch)

    outputs = {
        "arch": arch_srcs,
        "arch_rm": arch_removed,
        "libc": libc_srcs, 
        "ldso": ldso_srcs, 
        "crt": crt_srcs, 
        "header": header_files,
    }
    # print_outputs(outputs)
    return libc_srcs, ldso_srcs, crt_srcs, header_files

def main():
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('--path',
                        required = True,
                        help = 'The path of musl source files')
    parser.add_argument('--arch',
                        required = True,
                        help = 'The targeting architecture')
    args = parser.parse_args()

    return search_for_files(args.path, args.arch)


if __name__ == "__main__":
    main()
