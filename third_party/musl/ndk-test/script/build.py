from distutils.command.build import build
import os
import fnmatch
from pickletools import string1
import shutil
import datetime

compiler_list = [
    'ndk-test',
    #'sanitize',
]

# Check the output directory
def check_target_dir():
    target_dir = os.path.join(os.getcwd(),os.path.pardir,'target')
    print(target_dir)
    if os.path.exists(target_dir) and os.path.isdir(target_dir):
        # print('target directory exist')
        for filename in os.listdir(target_dir):
            if os.path.isdir(os.path.join(target_dir,filename)):
                shutil.rmtree(os.path.join(target_dir,filename))
    else:
        os.mkdir(target_dir)

# Traverse the directory, find the test case that needs to be compiled and compile it
def build_compiler():
    check_target_dir()
    build_start = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('build_start:' + build_start)
    comliler_dir = os.path.join(os.getcwd(),os.path.pardir)
    # print(comliler_dir)
    for name in compiler_list:
        file_path = os.path.join(comliler_dir,name)
        # print(file_path)
        if os.path.isdir(file_path):
            for f_name in os.listdir(file_path):
                if fnmatch.fnmatch(f_name, 'build.py'):
                    os.chdir(file_path)
                    string_build = 'python {}'.format('build.py')
                    os.system(string_build)
    return_dir = os.path.join(os.getcwd(),os.path.pardir,'script')
    os.chdir(return_dir)
    build_end = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('build_end:' + build_end)
    copy_target()
    del_Intermediate_file()
    return 0    

# Copy the compiled target product to the target directory
def copy_target():
    copy_start = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('copy_target_start:' + copy_start)
   
    find_dir = os.path.join(os.getcwd(),os.path.pardir)
    check_target_dir()
    for name in compiler_list:
        file_path = os.path.join(find_dir,name)
        for curdir, dirs, files in os.walk(file_path):
            for dirname in dirs:
                if dirname == 'output':
                    copy_tar_out = os.path.join(find_dir,'target',name,os.path.basename(curdir))
                    # print(copy_tar_out)
                    if not os.path.isdir(copy_tar_out):
                        os.makedirs(copy_tar_out)
                    tar_out = os.path.join(curdir,dirname)
                    # print(tar_out)
                    print(tar_out + '----->' + copy_tar_out)
                    for filename in os.listdir(tar_out):
                        src = os.path.join(tar_out, filename)
                        dst = os.path.join(copy_tar_out, filename)
                        print(src + '----->' + dst)
                        shutil.copy(src, dst)
                        
    copy_end = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('copy_target_end:' + copy_end)
    
    return 0

# Remove intermediate files from source files
def del_Intermediate_file():
    del_start = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('copy_target_start:' + del_start)
    find_dir = os.path.join(os.getcwd(),os.path.pardir)
    for name in compiler_list:
        file_path = os.path.join(find_dir,name)
        del_file_path = os.path.join(file_path)
        for curdir, dirs, files in os.walk(del_file_path):
            for dirname in dirs:
                if dirname == 'build':
                    # print(os.path.join(curdir,dirname))
                    shutil.rmtree(os.path.join(curdir,dirname))
    del_end = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print('copy_target_end:' + del_end)

if __name__ == '__main__':
    # check_target_dir()
    build_compiler()
    # copy_target()
    # del_Intermediate_file()
