/*
 * test.cpp
 *
 *  created on: 2012-11-6
 *      author: XieChengmin
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
入库性能测试 20121115：

两个大文件各2kw数据入库:


 */

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("./a.out 1  2000000  oracle_test_loader.txt\n");
		exit(-1);
	}
	FILE *f = fopen(argv[3], "w");
	long long start = atol(argv[1]);
	long long end   = atol(argv[2]);
	for (int i = start; i < end; i++)
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
