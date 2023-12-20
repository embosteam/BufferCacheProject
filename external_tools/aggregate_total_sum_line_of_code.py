from glob import glob
import os
import re
def getFolderFileList(folder):
    tmp = glob(os.path.join(folder,"*.c"),recursive=True)
    tmp += glob(os.path.join(folder,"*.h"),recursive=True)
    tmp += glob(os.path.join(folder,"Makefile"),recursive=True)
    tmp   = list(filter(lambda x:not (len(str(x))>0 and "hashmap" in x),tmp))
    tmp   = list(filter(lambda x:not (len(str(x))>0 and "md5" in x),tmp))
    tmp   = list(filter(lambda x:not (len(str(x))>0 and "crc" in x),tmp))
    return tmp
def getFileLineOfCode(file_path):
    lines = []
    with open(file_path,"r") as fp:
        lines = list(filter(lambda x: str(x).strip()!="",fp.read().split("\n")))
    return len((lines))
folders = ["../","../delayed_write","../shared","../buffered_read","../replacement_policy"]
files = []
for folder in folders:
    files += getFolderFileList(folder)
sum_line_of_code = 0
for file_path in files:
    sum_line_of_code += getFileLineOfCode(file_path)
print(*files,sep="\n")
print("line of code 총계: ",sum_line_of_code)
