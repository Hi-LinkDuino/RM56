import os
import shutil

import sys
sys.path.append('../script')
from config_args import linux_args

def build_unix():
    if os.path.exists('build') and os.path.isdir('build'):
        shutil.rmtree('build')
    while True:
        if not os.path.exists('build'):
            break
    os.mkdir('build')
    os.chdir('build')
    build_string = 'cmake' + ' '+ (" ".join(linux_args)) + ' '+'..'
    print(build_string)
    res = os.popen(build_string)
    print(res.read())
    res = os.popen('cmake --build .')
    print(res.read())

if __name__ == '__main__':
    build_unix()
