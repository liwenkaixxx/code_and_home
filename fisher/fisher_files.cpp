#ifndef PRICE_RANGE_H_
#define PRICE_RANGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <vector>
#include <unistd.h>
#include <dirent.h>


using namespace std;

#define E 3
#define LOG_E 0

#define NUM 60
#define INF 9999
//#define INF 99999999999
#define B_CNT 3

struct IntA {
	int v[NUM];
};

struct IntAA {
	IntA a[NUM];
};

struct DoubleA {
	double v[NUM];
};

struct DoubleAA {
	DoubleA a[NUM];
};

void FisherDiv(DoubleA& price, int K, double& result);
void GetDistance(DoubleA& price, DoubleAA& D);   //获取直径距离
void CalcLoss(DoubleAA& D, DoubleAA& S, IntAA& J);   //计算损失函数
void GetBreak(DoubleA& price, DoubleAA& S, IntAA& J, int K, double& result);   //获取拐点
void split(const std::string& str, const std::string& sp, std::vector<std::string>& out);
void GetAllFiles(const char * dir_name, vector<string>& files);
void ReadFile(const char* fpath);






int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please input: ./a.out FILEPATH\n");
		return -1;
	}
	vector<string> files;

	GetAllFiles(argv[1], files);

	for (size_t i = 0; i < files.size(); i++)
	{
		const char * p = files[i].data();
		
		 ReadFile(p);
	}

	return 0;
}

void ReadFile(const char* fpath)
{

	FILE *fp = fopen(fpath, "rb");
	if (!fp)
	{
		printf("Fopen %s fail!\n", fpath);
		return;
	}
	DoubleA price;
	memset(&price, 0, sizeof(price));
	int count = 0;
	char buff[1024] = "";
	while (fgets(buff, sizeof(buff), fp))
	{
		std::vector<std::string> out;
		split(buff, "\t", out);
		if (out.size() < 4)
			continue;
		double oriPrice = atof(out[3].c_str());
#if LOG_E
		price.v[count++] = log(oriPrice + E);
#else
		price.v[count++] = oriPrice;
#endif
		if (count >= NUM)
			break;
	}
	fclose(fp);
	if (count < NUM)
	{
		printf("Price count is less than %d!\n", NUM);
		return;
	}
	printf("\n分类结果如下：%s\n", fpath);
	int clust = B_CNT;  //分的类数，由计算结果观察可知在分为四类的时候出现一个转折点;
	double result = 0;
	FisherDiv(price, clust, result);
	if (result < 0.000001)
		printf("price range: not need\n");
	else
		printf("price range: %0.3lf\n", result);

	return;
}

void FisherDiv(DoubleA& price, int K, double& result)
{
	DoubleAA D;
	DoubleAA S;
	IntAA J;
	memset(&D, 0, sizeof(D));
	memset(&S, 0, sizeof(S));
	memset(&J, 0, sizeof(J));

#if 0
	double temp = 0;
	for (int i = 0; i < NUM / 2; i++)
	{
		temp = price.v[i];
		price.v[i] = price.v[NUM - 1 - i];
		price.v[NUM - 1 - i] = temp;
	}
#endif
#if 0
	double sample[NUM] = { 5,6,9.8999996185302734,11.899999618530273,12,12.899999618530273,13.5,13.899999618530273,16.5,16.799999237060547, 16.799999237060547, 16.899999618530273, 17.399999618530273, 18.899999618530273, 18.899999618530273, 19, 19, 19, 19, 19.899999618530273, 19.899999618530273, 19.899999618530273, 19.899999618530273, 19.899999618530273, 19.899999618530273, 21.899999618530273, 22.899999618530273, 22.899999618530273, 23.899999618530273, 24.799999237060547, 24.899999618530273, 25, 25, 25.799999237060547, 25.899999618530273, 25.899999618530273, 26.799999237060547, 26.799999237060547, 26.799999237060547, 28, 28, 28, 28, 28.799999237060547, 28.899999618530273, 28.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 29.899999618530273, 30, 30.899999618530273, 31.100000381469727 };
	for (int i = 0; i < NUM; i++)
	{
		price.v[i] = sample[i];
	}
#endif
	GetDistance(price, D);
	CalcLoss(D, S, J);
	GetBreak(price, S, J, K, result);
}

//获取直径距离
void GetDistance(DoubleA& price, DoubleAA& D)
{
	int i, j, k, l, m, n;
	double mean;
	double sum = 0;
	DoubleA X;
	memset(&X, 0, sizeof(X));

	for (i = 0; i<NUM - 1; i++)
	{
		for (j = i + 1; j<NUM; j++)
		{
			sum = 0;
			l = 0;
			for (k = i; k <= j; k++)
			{
				X.v[l++] = price.v[k];
			}
			for (m = 0; m<l; m++)
			{
				sum += X.v[m];
			}
			mean = sum / l;
			for (m = 0; m<l; m++)
			{
				D.a[i].v[j] += (X.v[m] - mean)*(X.v[m] - mean);
			}
		}
	}
}

//计算损失函数
void CalcLoss(DoubleAA& D, DoubleAA& S, IntAA& J)
{
	int i, j, k, l;
	double min = INF;
	int minj;
	double temp;

	for (k = 1; k<NUM - 1; k++)
	{
		for (l = 2; l<NUM; l++)
		{
			if (k<l)
			{
				min = INF;
			}
			for (j = 1; j <= l; j++)
			{
				if (k == 1)
				{
					temp = D.a[0].v[j - 1] + D.a[j].v[l];
					if (temp<min)
					{
						min = temp;
						minj = j;
					}
				}
				else
				{
					temp = S.a[k - 1].v[j - 1] + D.a[j].v[l];
					if (temp<min)
					{
						min = temp;
						minj = j;
					}
				}
			}
			S.a[k].v[l] = min;
			J.a[k].v[l] = minj;
		}
	}
}

void GetBreak(DoubleA& price, DoubleAA& S,
	IntAA& J, int K, double& result)
{
	int i = 0;
	int j = 0;
	int l = 0;
	double minPoint = INF;
	vector<int> breakPoint(K);
	vector<double> mean(K);
	vector<vector<double> > range(K);

	breakPoint[K - 1] = J.a[K - 1].v[NUM - 1]; //第一个断点
	for (i = K - 2; i>0; i--)
	{
		l = i;
		minPoint = INF;
		for (j = i + 2; j<NUM; j++)
		{
			if (minPoint>S.a[i].v[j])
			{
				minPoint = S.a[i].v[j];
				l = j;
			}
		}
		breakPoint[i] = J.a[i].v[l];
	}

	//分类
	j = 1;
	int sumCnt = 0;
	double sumScore = 0;
	int rangeNum = 0;
	for (i = 0; i<K; i++)
	{
		printf("break[%d]: %d\n", i, breakPoint[i]);
	}
	for (i = 0; i<NUM; i++)
	{
		//printf("i:%d j:%d rangeNum:%d\n", i, j, rangeNum);
		if (j<K)
		{
			if (i == breakPoint[j])
			{
				j++;
				if (sumCnt > 0)
				{
					printf("mean: %0.3lf", sumScore / sumCnt);
					mean[rangeNum] = sumScore / sumCnt;
					rangeNum++;
				}
				sumCnt = 0;
				sumScore = 0;
				printf("\n");
			}
		}
		sumCnt++;
#if LOG_E
		printf("%0.3lf  ", exp(price.v[i]) - E);
		sumScore += exp(price.v[i]) - E;
		range[rangeNum].push_back(exp(price.v[i]) - E);
#else
		printf("%0.3lf  ", price.v[i]);
		sumScore += price.v[i];
		range[rangeNum].push_back(price.v[i]);
#endif
	}
	if (sumCnt > 0)
	{
		printf("mean: %0.3lf", sumScore / sumCnt);
		mean[rangeNum] = sumScore / sumCnt;
	}
	printf("\n");


	for (i = 1; i<(int32_t)mean.size(); i++)
	{
		if (mean[i - 1]<0.000001)
		{
			continue;
		}
		printf("mean%d/mean%d: %0.3lf\n", i, i - 1, mean[i] / mean[i - 1]);
	}
#if 1
	double asc_thres = 1.8;
	for (i = 1; i < (int)mean.size(); i++)
	{
		if (mean[i - 1] < 0.000001)
			continue;
		if (mean[i] / mean[i - 1] < asc_thres)
		{
			if (i == 1)
			{
				result = -1;
			}
			else
			{
				result = range[i - 1][0];
			}
			break;
		}
	}
	if (i == (int)mean.size())
	{
		if (range[i - 1].size()<1)
		{
			result = -1;
		}
		else
		{
			result = range[i - 1][0];
		}
	}
#else
	double desc_thres = 5;
	for (i = (int)mean.size() - 1; i > 0; i--)
	{
		if (mean[i - 1] < 0.000001)
			continue;
		if (mean[i] / mean[i - 1] < desc_thres /* || range[i-1].size() > 10 */)
		{
			if (i == ((int)mean.size() - 1))
				result = -1;
			else
				result = range[i + 1][0];
			break;
		}
	}
	if (i == 0)
	{
		result = range[i + 1][0];
	}
#endif
}

void split(const std::string& str,
	const std::string& sp,
	std::vector<std::string>& out)
{
	out.clear();
	std::string s = str;
	size_t beg, end;
	while (!s.empty())
	{
		beg = s.find_first_not_of(sp);
		if (beg == std::string::npos)
		{
			break;
		}
		end = s.find(sp, beg);
		out.push_back(s.substr(beg, end - beg));
		if (end == std::string::npos)
		{
			break;
		}
		s = s.substr(end, s.size() - end);
	}
}

#endif

void GetAllFiles(const char * dir_name, vector<string>& files)
{
	// check the parameter !
	if (NULL == dir_name)
	{
		cout << " dir_name is null ! " << endl;
		return;
	}

	// check if dir_name is a valid dir
	struct stat s;
	lstat(dir_name, &s);
	if (!S_ISDIR(s.st_mode))
	{
		cout << "dir_name is not a valid directory !" << endl;
		return;
	}

	struct dirent * filename;    // return value for readdir()
	DIR * dir;                   // return value for opendir()
	dir = opendir(dir_name);
	if (NULL == dir)
	{
		cout << "Can not open dir " << dir_name << endl;
		return;
	}


	/* read all the files in the dir ~ */
	while ((filename = readdir(dir)) != NULL)
	{
		// get rid of "." and ".."
		if (strcmp(filename->d_name, ".") == 0 ||
			strcmp(filename->d_name, "..") == 0)
			continue;

		char strpath[20];
		sprintf(strpath, "%s%s", dir_name, filename->d_name);
		files.push_back(strpath);
	}
}