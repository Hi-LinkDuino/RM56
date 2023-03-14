import difflib
import os

def compare_file():
    if os.path.exists(r'result.html'):
        os.remove(r'result.html')
    f = open(r'../libc-test/REPORT','r')
    a = f.readlines()
    f.close()
    f = open(r'REPORT','r')
    b = f.readlines()
    f.close()
    if a != b:
        compare = difflib.HtmlDiff()
        c = compare.make_file(a,b)
        f = open(r'result.html','w')
        f.write(c)
        f.close
if __name__ == '__main__':
    compare_file()