#incoding:utf-8

import sys
import os


def loadDataSet(in_path,out_path):
    dataMat = []
    i = 0
    with open(in_path,"r",encoding="UTF-8") as f_in,open(out_path,"w") as f_out:
        for line in f_in.readlines():
            i += 1
            temp = line.split("\t")
            in_data = "%d\t%s\n"%(i,temp[0])
            dataMat.append(in_data)
            print(in_data)
        f_out.writelines(dataMat)


if __name__ == "__main__":
    if 3 == len(sys.argv):
        in_path,out_path = [i.strip() for i in sys.argv[1:]]
    else:
        in_path = str(input("pls input the path of in-file:")).strip()
        out_path = str(input("pls input the path of out-file:")).strip()
    if not os.path.exists(in_path):
        print("ERROR:in-file not exist")
        sys.exit(1)
    loadDataSet(in_path,out_path)