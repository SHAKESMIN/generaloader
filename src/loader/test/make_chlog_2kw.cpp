/*
 * test.cpp
 *
 *  created on: 2012-11-6
 *      author: XieChengmin
 */
#include <unistd.h>
#include <stdio.h>
#include <fnmatch.h>

#define MAX  20000000


/*

������ܲ��� 20121114��
��һ�Σ�  2kw --- 17min    Լ2w����¼/s
����һЩ������ direct=true
�ڶ��Σ�  2kw --- 4min    Լ7.4w����¼/s



 */
int main(int argc, char **argv)
{

	FILE *f = fopen("oracle_load_test.txt", "w");
	long long i = 1;
	for (i = 0; i < MAX; i++)
	{
		fprintf(f, "%ld&&7542790865000&&6&&100&&104.400000&&SYS&&KCZ&&&&2012-11-03 23:58:19&&-1&&109.490000&&0&&0&&1&&&&0&&\n", i);
		if (i % 5000 == 0)
			fflush(f);
	}
	fprintf(f, "HDR2225660\n");
	fflush(f);
	fclose(f);
	return 0;

}
