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
import multiprocessing



def loadDataSet(in_path):
	dataMat = []
	i = 0
	with open(in_path,"r") as f_in:
		for line in f_in.readlines():
			i += 1
			temp = line.strip().split("\t")
			in_data = "%d\t%s"%(i,temp[0])
			dataMat.append(in_data)
	return dataMat

def parse_page_data_low(data_file, goods, doc_sort, wwsy_result):
	for i in range(len(goods)):
		doc_sort += 1
		try:
			score_json = goods[i]['scoreJsonStr']
		except KeyError:
			continue
		try:
			doc_score = json.loads(score_json)
		except ValueError:
			#print index, "json loads error"
			continue

		if len(doc_score['_DY_price']) < 1:
			continue
		if string.atof(doc_score['_DY_price']) <= 0:
			continue

		data_doc = str(doc_sort) + '\t' + goods[i]['partnumberVendorId'] + '\t' + doc_score['totalCount'] + '\t' + doc_score['_DY_price'] + '\t' + doc_score['goodsQualityScore'] + '\t' + doc_score['praiseRate'] + '\t' + doc_score['countOfarticle'] + '\t' + doc_score['shopQualityScore'] + '\n'
		data_file.write(data_doc)
	return doc_sort

def parse_page_data(data_file, goods, doc_sort, wwsy_result):
	for i in range(len(goods)):
		doc_sort += 1
		try:
			score_json = goods[i]['scoreJsonStr']
			print(score_json)
			w = input()
		except KeyError:
			continue
		try:
			doc_score = json.loads(score_json)
			print(doc_score)
		except ValueError:
			#print index, "json loads error"
			continue
		#print i,goods[i]['partnumberVendorId'],doc_score['_DY_price']
		if len(doc_score['_DY_price']) < 1:
			continue
		if string.atof(doc_score['_DY_price']) <= 0:
			continue


		"""
		if len(doc_score['goodQualityScore']) < 1:
			goodsQualityScore = '0'
		else:
			goodsQualityScore = doc_score['goodQualityScore']

		if len(doc_score['articleCountScore']) < 1:
			totalCount = '0'
		else:
			totalCount = doc_score['articleCountScore']

		if len(doc_score['articleCount']) < 1:
			countOfarticle = '0'
		else:
			countOfarticle = doc_score['articleCount']
		
		newLine = [goodsQualityScore, countOfarticle]
		floatLine = map(float,newLine)
		if floatLine[0] < 0.000001 or floatLine[1] < 0.000001:
			continue
		"""


		if doc_score['cqHas'] == "1" and doc_score['cqHit'] == "0":
			continue

		data_doc = str(doc_sort) + '\t' + goods[i]['partnumberVendorId'] + '\t' + doc_score['totalCount'] + '\t' + doc_score['_DY_price'] + '\t' + doc_score['goodsQualityScore'] + '\t' + doc_score['praiseRate'] + '\t' + doc_score['countOfarticle'] + '\t' + doc_score['articleCount'] + '\n'
		data_file.write(data_doc)


	return doc_sort

def url_request_wwsy(wwsy_url, wwsy_result):
	print (wwsy_url)
	timeout = 60
	maxUrlRead = 4096000
	time.sleep(0.1)
	try:
		rsp = urllib2.urlopen(wwsy_url, timeout=timeout).read(maxUrlRead)
	except:
		return -1
	#print rsp
	try:
		data = json.loads(rsp)
	except ValueError:
		#rint "ValueError: ",url
		return -1
	#print data
	try:
		category = data['category_entity']
		wwsy_result = category['entity_id']
	except KeyError:
		#print "KeyError: ",url
		return -1
	print (wwsy_result)


def url_request_page(url, page_num, page_cnt, data_file, doc_sort, wwsy_result):
	new_url = url + "&cp=" + str(page_num)
	#print new_url
	timeout = 60
	maxUrlRead = 4096000
	time.sleep(0.1)
	try:
		rsp = urllib2.urlopen(new_url, timeout=timeout).read(maxUrlRead)
	except:
		return -1
	#print rsp
	try:
		data = json.loads(rsp)
	except ValueError:
		#rint "ValueError: ",url
		return -1
	#print data
	try:
		goods = data['goods']
		num_fount = data['numFound']
	except KeyError:
		#print "KeyError: ",url
		return -1

	before_sort = doc_sort
	#print "before0: ",doc_sort,num_fount
	doc_sort = parse_page_data_low(data_file, goods, doc_sort, wwsy_result)
	#print "after0: ",doc_sort,num_fount
	if doc_sort >= 600:
		return 0
	if abs(before_sort - doc_sort) <= 3:
		return 0
	if abs(doc_sort - num_fount) <= 3:
		return 0
	elif doc_sort < num_fount:
		if page_cnt == 0:
			page_cnt = (doc_sort + 3) / 60
		if (page_cnt == 0) or (num_fount - doc_sort < page_cnt * 60):
			return 0
		page_num += page_cnt
		#print "before1: ",doc_sort,num_fount
		doc_sort = url_request_page(url, page_num, page_cnt, data_file, doc_sort, wwsy_result)
		#print "after1: ",doc_sort,num_fount
		return doc_sort
	else:
		return 0
	return 0

def url_request(index, url, wwsy_url, data_path):
	file_name = data_path + '/'+ str(index)
	data_file = open(file_name, 'w')

	wwsy_result = "none"
	"""
	url_request_wwsy(wwsy_url, wwsy_result)
	print wwsy_result
	"""
	result = url_request_page(url, 0, 0, data_file, 0, wwsy_result)
	#print "result: ",result
	data_file.close()
	if result == 0:
		return True
	if result < 0:
		return False
	return True

def main(query, data_path, fail_name, process_count, process_index, lock):
	base_param = 'http://search.cnsuning.com/emall/productList.do?pg=01&cc=010&preview=true&st=9&keyword='
	wwsy_param = 'http://10.104.242.58/qa?rewrite=0&fmt=json&gender=1&import=1&rett=1&semantic=1&shop=1&source=search&gbk=0&q='
	failSet = []
	query_seg = query[process_index::process_count]
	#print len(query_seg),query_seg
	if len(query_seg) < 1:
		return True
	for i in range(len(query_seg)):
		index = query_seg[i].split()[0]
		src_q = query_seg[i].split()[1]
		#print process_index,index,src_q
		url = base_param + urllib.quote(src_q)
		wwsy_url = wwsy_param + urllib.quote(src_q)
		#print url
		time.sleep(0.1)
		result = url_request(index, url, wwsy_url, data_path)
		if result == True:
			print (index,src_q," succ")
		else:
			result = url_request(index, url, wwsy_url, data_path)
			if result == True:
				print (index,src_q," succ")
			else:
				print (index,src_q," fail")
				failSet.append(query_seg[i])

	lock.acquire()
	fail_file = open(fail_name, 'a+')
	for i in range(len(failSet)):
		fail_file.write(failSet[i])
	fail_file.close()
	lock.release()
	return True

def cpu_info():
	cpu_count = 0
	fine = open('/proc/cpuinfo')
	for line in fine.readlines():
		if len(line.strip()) > 0:
			if len(line.split(':')) == 2:
				if line.split(':')[0].strip() == "processor":
					cpu_count += 1
	return cpu_count

if __name__ == '__main__':
	start = time.clock()

	in_path = sys.argv[1]
	out_path = sys.argv[2]
	fail_name = sys.argv[3]
	query = loadDataSet(in_path)

	cpuinfo = cpu_info()
	#print cpuinfo
	process_count = cpuinfo * 3 / 4
	print ("CPU: " + str(process_count) + " / " + str(cpuinfo))

	lock = Lock()
	if len(query) < process_count:
		#main(query, data_path, fail_name, 1, 0, lock)
		process_count = len(query)

	process = []
	for i in range(process_count):
		p = Process(target=main, args=(query, out_path, fail_name, process_count, i, lock))
		process.append(p)
		# p.setDaemon(True)
		p.start()
	for p in process:
		p.join()

	end = time.clock()
	print('finish all in %s' % str(end - start))
