#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "vector"
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <vector>
#include <unistd.h>
#include <dirent.h>
using namespace std;


typedef vector<double> doubleVector;


#define dimNum 4   //������ά��


vector<doubleVector>  getSample(char *File);  //��ȡ����
void Fisher(vector<doubleVector> sampleJia, vector<doubleVector> sampleYi);   //��ʼ����
doubleVector getSampleMean(vector<doubleVector> sample);  //��ȡ������ֵ����
vector<doubleVector> getDispersionMatrix(vector<doubleVector> sample, doubleVector meam);   //����������ɢ�Ⱦ���
vector<doubleVector> matMul(vector<doubleVector> Mat1, vector<doubleVector> Mat2);  //�������
vector<doubleVector> matAdd(vector<doubleVector> Mat1, vector<doubleVector> Mat2);  //�������
vector<doubleVector> getComplement(vector<doubleVector> arcs, int n);   //��ȡ����ʽ
double getDet(vector<doubleVector> arcs, int n);   //��������ʽ 
vector<doubleVector> matInv(vector<doubleVector> src);  //��������
void GetAllFiles(const char * dir_name, vector<string>& files);//��ȡ·���������ļ�


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please input: ./a.out FILEPATH\n");
		return -1;
	}
	vector<string> files;

	GetAllFiles(argv[1], files);
	int countline = atoi(argv[2]);

	for (size_t i = 0; i < files.size(); i++)
	{
		const char * p = files[i].data();
		if (i > countline)
		{
			break;
		}

		oncetest();//һ�μ������
	}

	return 0;
}

void oncetest()
{
	char *File1 = "�׵�.txt";
	char *File2 = "�ҵ�.txt";


	vector<doubleVector> sampleJia;
	vector<doubleVector> sampleYi;


	sampleJia = getSample(File1);
	sampleYi = getSample(File2);





	Fisher(sampleJia, sampleYi);


}




//Fisher��ʼ����
void Fisher(vector<doubleVector> sampleJia, vector<doubleVector> sampleYi)
{
	int i, j;


	doubleVector sampleMeanJia;
	doubleVector sampleMeanYi;
	vector<doubleVector> meanJia;
	vector<doubleVector> meanYi;
	vector<doubleVector> Sjia;  //����������ɢ�Ⱦ���
	vector<doubleVector> Syi;   //����������ɢ�Ⱦ���
	vector<doubleVector> Sw;  //��������ɢ�Ⱦ���
	vector<doubleVector> invSw;  //Sw�������
	vector<doubleVector> W;  //��������W
	vector<doubleVector> WT;  //��������W��ת��
	vector<doubleVector> meanDiff;   //��ֵ������
	doubleVector diffTemp;
	doubleVector wTemp;


	sampleMeanJia = getSampleMean(sampleJia);
	sampleMeanYi = getSampleMean(sampleYi);
	meanJia.push_back(sampleMeanJia);
	meanYi.push_back(sampleMeanYi);


	Sjia = getDispersionMatrix(sampleJia, sampleMeanJia);
	Syi = getDispersionMatrix(sampleYi, sampleMeanYi);


	Sw = matAdd(Sjia, Syi);


	invSw = matInv(Sw);


	for (i = 0; i<sampleMeanJia.size(); i++)
		diffTemp.push_back(sampleMeanJia[i] - sampleMeanYi[i]);
	diffTemp.push_back(1);
	meanDiff.push_back(diffTemp);


	W = matMul(meanDiff, invSw);


	for (i = 0; i<dimNum; i++)
	{
		wTemp.push_back(W[0][i]);
		WT.push_back(wTemp);
		wTemp.clear();
	}


	vector<doubleVector> y1, y2, y0;
	vector<doubleVector> W0;


	y1 = matMul(meanJia, WT);
	y2 = matMul(meanYi, WT);
	W0 = matAdd(y1, y2);


	//��������
	doubleVector samTemp;
	vector<doubleVector> sample;
	samTemp.push_back(5);  samTemp.push_back(4.43);  samTemp.push_back(22.4);  samTemp.push_back(54.6);
	sample.push_back(samTemp);

	y0 = matMul(sample, WT);


	if ((y0[0][0] - W0[0][0] / 2)>0)
		printf("�׵�\n");
	else
		printf("�ҵ�\n");


}




//������ֵ
doubleVector getSampleMean(vector<doubleVector> sample)
{
	double temp[dimNum] = { 0 };
	doubleVector dst;
	int i, j;


	for (i = 0; i<sample.size(); i++)
		for (j = 0; j<dimNum; j++)
			temp[j] += sample[i][j];


	for (i = 0; i<dimNum; i++)
	{
		temp[i] /= sample.size();
		dst.push_back(temp[i]);
	}


	return dst;
}




//����������ɢ�Ⱦ���
vector<doubleVector> getDispersionMatrix(vector<doubleVector> sample, doubleVector meam)
{
	int i, j;
	vector<doubleVector> Mat;
	vector<doubleVector> MatT;
	vector<doubleVector> mMul;
	doubleVector temp;
	doubleVector dstTemp(sample.size(), 0);
	vector<doubleVector> dst(sample.size(), dstTemp);


	for (i = 0; i<sample.size(); i++)
	{
		temp.clear();
		Mat.clear();
		MatT.clear();
		for (j = 0; j<dimNum; j++)
			temp.push_back(sample[i][j] - meam[j]);
		Mat.push_back(temp);


		//ת��
		for (j = 0; j<Mat[0].size(); j++)
		{
			temp.clear();
			temp.push_back(Mat[0][j]);
			MatT.push_back(temp);
		}


		//�������
		mMul = matMul(MatT, Mat);
		dst = matAdd(mMul, dst);
	}


	return dst;
}




//�������
vector<doubleVector> matMul(vector<doubleVector> Mat1, vector<doubleVector> Mat2)
{
	int i, j, k;
	doubleVector temp(Mat2[0].size(), 0);
	vector<doubleVector> dstMat(Mat1.size(), temp);


	for (i = 0; i<Mat1.size(); i++)
		for (j = 0; j<Mat2[0].size(); j++)
			for (k = 0; k<Mat2.size(); k++)
				dstMat[i][j] += Mat1[i][k] * Mat2[k][j];


	return dstMat;
}




//�������
vector<doubleVector> matAdd(vector<doubleVector> Mat1, vector<doubleVector> Mat2)
{
	int i, j;
	vector<doubleVector> dstMat;
	doubleVector temp;

	for (i = 0; i<Mat1.size(); i++)
	{
		temp.clear();
		for (j = 0; j<Mat1[0].size(); j++)
			temp.push_back(Mat1[i][j] + Mat2[i][j]);
		dstMat.push_back(temp);
	}



	return dstMat;
}




//��ȡ����
vector<doubleVector> getSample(char *File)
{
	int i = 1;
	vector<doubleVector> dst;
	doubleVector temp;


	FILE *fp = fopen(File, "r");


	if (fp == NULL)
	{
		printf("Open file error!!!\n");
		return dst;
	}


	double num;
	while (fscanf(fp, "%lf", &num) != EOF)
	{
		temp.push_back(num);
		if (i%dimNum == 0)
		{
			dst.push_back(temp);
			temp.clear();
		}
		i++;
	}


	return dst;
}




//��������
vector<doubleVector> matInv(vector<doubleVector> src)
{
	int i, j;
	vector<doubleVector> matCom;  //����ʽ
	vector<doubleVector> dst;
	doubleVector temp;


	double a = getDet(src, src.size());   //������������ʽ


	if (a == 0)
	{
		printf("�þ���������!\n");
	}
	else
	{
		matCom = getComplement(src, src.size());


		for (i = 0; i<src.size(); i++)
		{
			temp.clear();
			for (j = 0; j<src.size(); j++)
			{
				temp.push_back(matCom[i][j] / a);
			}
			dst.push_back(temp);
		}
	}


	return dst;
}




//���������ʽ
double getDet(vector<doubleVector> arcs, int n)
{
	if (arcs.size() == 1)
	{
		return arcs[0][0];
	}
	double ans = 0;
	doubleVector tmp(n - 1);
	vector<doubleVector> temp(n - 1, tmp);
	int i, j, k;
	int p, q;


	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n - 1; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];
			}

		}


		double t = getDet(temp, n - 1);
		if (i % 2 == 0)
		{
			ans += arcs[0][i] * t;
		}
		else
		{
			ans -= arcs[0][i] * t;
		}
	}
	return ans;
}



//����ÿһ��ÿһ�е�ÿ��Ԫ������Ӧ������ʽ�����A*  
vector<doubleVector> getComplement(vector<doubleVector> arcs, int n)
{
	int p, q;
	int i, j, k, t;
	double dTemp;

	doubleVector tmp(n - 1);
	doubleVector tmp2(n);
	vector<doubleVector> ans(n, tmp2);
	vector<doubleVector> temp(n - 1, tmp);


	if (n == 1)
	{
		tmp.push_back(1);
		ans.push_back(tmp);
		return ans;
	}

	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				for (t = 0; t<n - 1; t++)
				{
					temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}

			ans[i][j] = getDet(temp, n - 1);
			if ((i + j) % 2 == 1)
			{
				ans[i][j] = -ans[i][j];
			}
		}
	}



	doubleVector T;
	vector<doubleVector> ansT;
	for (i = 0; i<ans.size(); i++)
	{
		T.clear();
		for (j = 0; j<ans[i].size(); j++)
			T.push_back(ans[j][i]);
		ansT.push_back(T);
	}


	return ansT;
}


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