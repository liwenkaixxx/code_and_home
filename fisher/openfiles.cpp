
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
using namespace std;

/***** Global Variables *****/
char dir[100] = "/home";
int const MAX_STR_LEN = 200;

/* Show all files under dir_name , do not show directories ! */
void showAllFiles(const char * dir_name)
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
	cout << "Successfully opened the dir !" << endl;

	/* read all the files in the dir ~ */
	while ((filename = readdir(dir)) != NULL)
	{
		// get rid of "." and ".."
		if (strcmp(filename->d_name, ".") == 0 ||
			strcmp(filename->d_name, "..") == 0)
			continue;
		cout << filename->d_name << endl;
	}
}

int main()
{
	// ²âÊÔ
	showAllFiles(dir);

	return 0;
}