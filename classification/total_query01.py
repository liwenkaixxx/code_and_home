# -*- coding: utf-8 -*-

import urllib
import urllib2
import urllib3
import json
import string
import time
import sys
import os
from multiprocessing import *





def loadDataSet(in_path):
    dataMat = []
    i = 0
    with open(in_path, "r") as f_in:
        for line in f_in.readlines():
            dataMat.append(line)
    return dataMat

def url_request_page(url,page_num,count_current):
    data_list = []

    new_url = url + "&cp=" + str(page_num)
    timeout = 60
    maxUrlRead = 4096000
    time.sleep(0.1)
    try:
        rsp = urllib2.urlopen(new_url, timeout=timeout).read(maxUrlRead)
    except:
        return -1
    try:
        data = json.loads(rsp)
    except ValueError:
        return -1
    try:
        goods = data['goods']
    except KeyError:
        return -1

    for i in range(len(goods)):
        count_current += 1
        try:
            score_json = goods[i]['scoreJsonStr']
        except KeyError:
            continue
        try:
            doc_score = json.loads(score_json)
        except ValueError:
            continue

        if len(doc_score['_DY_price']) < 1:
            continue
        if string.atof(doc_score['_DY_price']) <= 0:
            continue

        data_doc = str(count_current) + '\t' + goods[i]['partnumberVendorId'] + '\t' + doc_score['totalCount'] + '\t' + \
                   doc_score['_DY_price'] + '\t' + doc_score['goodsQualityScore'] + '\t' + doc_score[
                       'praiseRate'] + '\t' + doc_score['countOfarticle'] + '\t' + doc_score['shopQualityScore'] + '\n'
        data_list.append(data_doc)

    return data_list




def url_request(index, url, out_path):
    file_name = out_path + '/' + str(index) + '.csv'
    with open(file_name, 'w') as data_file:
        data_list = []
        list_lenth = 0

        new_url = url + "&cp=" + str(0)
        timeout = 60
        maxUrlRead = 4096000
        time.sleep(0.001)
        try:
            rsp = urllib2.urlopen(new_url, timeout=timeout).read(maxUrlRead)
        except:
            return False
        try:
            data = json.loads(rsp)
        except ValueError:
            return False

        page_num = data['pageNumbers']

        for temp in range(page_num):
            data_list[len(data_list):len(data_list)] = url_request_page(url,temp,list_lenth)
            list_lenth = len(data_list)
        data_file.writelines(data_list)
    return True


def main(in_path,out_path):
    query_list = loadDataSet(in_path)
    cpu_num = 0
    try:
        cpu_num = os.sysconf('SC_NPROCESSORS_ONLN')
    except (ValueError, OSError, AttributeError):
        cpu_num = 0
    process_count = cpu_num * 3 / 4
    print("CPU: " + str(process_count) + " / " + str(cpu_num))

    if len(query_list) < process_count:
        process_count = len(query_list)

    process = []
    for i in range(process_count):
        p = Process(target=main, args=(query_list, out_path, process_count, i))
        process.append(p)

    for p in process:
        p.start()
        p.join()

    end = time.clock()
    print('finish all in %s' % str(end - start))



    base_param = 'http://search.cnsuning.com/emall/productList.do?pg=01&cc=010&preview=true&st=9&keyword='

    query_seg = query_list[process_index::process_count]

    if len(query_seg) < 1:
        return True
    for i in range(len(query_seg)):
        index = query_seg[i].split()[0]
        src_q = query_seg[i].split()[1]

        url = base_param + urllib.quote(src_q)

        time.sleep(0.001)

        result = url_request(index, url, out_path)
        if result == True:
            print(index, src_q, " succ")
        else:
            result = url_request(index, url, out_path)
            if result == True:
                print(index, src_q, " succ")
    return True


if __name__ == '__main__':
    start = time.clock()

    in_path = sys.argv[1]
    out_path = sys.argv[2]


    maim(in_path,out_path)

