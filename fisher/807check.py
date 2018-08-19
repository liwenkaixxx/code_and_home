# coding:gb18030

"""
main.py
TO:check info to restore
author:Wenkai.Lee
"""

import os
import sys

show_list = [0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
tab_list =  [0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
last_position = 'bookmark'

def print_head(line_pass):
    print ( "A: 原目录错误,预测正确")
    print ( "S: 原目录正确，预测错误")
    print ( "D: 原目录和预测目录都不准确，或者，标题看不出所属商品分类，无法判断")
    print ( "F: 原目录和预测目录都符合，或者，相似的")
    print ( "L: 跳转指定行")
    print ( "上次标注行号:%s" % ( line_pass ))


def get_input():
    while True:
        try:
            s = str(raw_input("pls input A/S/D/F/X(exit)):")).strip().upper()
        except:
            s = 'X'
        if s in ['A','S','D', 'F', 'X','H','L']:
            break
    return s

def get_mark(in_path):
    lines = []
    currnt_num = 0
    total = 0
    with open(in_path,"r") as f_in:
        lines = f_in.readlines()
        total = len(lines)
    if total == 0:
        return 0,0
    last_line = lines[-1].strip().split("\t")
    if last_position == last_line[0]:
        currnt_num = int(last_line[-1])
    else:
        total = total + 1
        new_line = "%s\t%d\t%d"%(last_position,total,currnt_num)
        lines.append(new_line)
        with open(in_path,"w") as f_out:
            f_out.writelines(lines)
    return currnt_num,total
    

def updat_opsition(lines,in_path):
    with open(in_path,"w") as f_in:
        f_in.writelines(lines)


def main(in_path, out_path):
    current_pass,total_pass = get_mark(in_path)
    if total_pass <= 0:
        return
    print_head(current_pass)
    f_in = open(in_path,"a+") # encoding = 'gbk')
    f_out = open(out_path,"a+")
    line_number = 0
    f_in_lines = f_in.readlines()
    while True:
        #for line in f_in_lines:

        line_number = line_number + 1
        if line_number < current_pass:
            continue
        line = f_in_lines[line_number].decode("gbk")
        #line = line.decode("gbk")

        i = -1
        for item in line.strip().split("\t"):
            if last_position == item[0]
                break
            i = i+1
            if show_list[i] == 0:
                continue
            print("%d%s\t%s"%(line_number, ("\t"*tab_list[i]), item))
        confirm = get_input() 
        if confirm == 'X':
            break
        if confirm == "L":
            current_pass = int(raw_input())
            line_number = 0
            continue

        print("-"*100)
        new_line = "%d\t%s\t%s\n"%(line_number, confirm.encode("gbk"),line.strip().encode("gbk"))
        f_out.write(new_line)

    """save mark on zhe last line """
    current_mark_line = "%s\t%d\t%d" % (last_position, total_pass, line_number)
    f_in_lines[-1] = current_mark_line

    f_in.close()
    f_out.close()

    updat_opsition(f_in_lines,in_path)


if __name__ == "__main__":
    if 3 == len(sys.argv):
        in_path,out_path = [i.strip() for i in sys.argv[1:]]
    else:
        in_path = str(raw_input("pls input the path of in-file:")).strip()
        out_path = str(raw_input("pls input the path of out-file:")).strip()
    if not os.path.exists(in_path):
        print("ERROR:in-file not exist")
        sys.exit(1)
    main(in_path,out_path)
