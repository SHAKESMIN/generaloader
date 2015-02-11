/*
 * test.cpp
 *
 *  created on: 2012-11-6
 *      author: XieChengmin
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv)
{

	int nFormat = atoi(argv[1]);
	char curtime[30];
	struct tm *lt;
	struct timezone tz;
	struct timeval now;
	gettimeofday(&now, &tz);
	lt = localtime(&now.tv_sec);

	memset(curtime , 0 ,sizeof(curtime));

	if(nFormat==0 || nFormat > 2 || nFormat < 0)
		sprintf(curtime, "%4d-%02d-%02d %02d:%02d:%02d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	else if(nFormat ==1)
		sprintf(curtime, "%4d%02d%02d%02d%02d%02d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	else if(nFormat ==2)
		sprintf(curtime, "%4d-%02d-%02d %02d:%02d:%02d.%06d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec,now.tv_usec);
	else if(nFormat == 3 )
		sprintf(curtime , "%d" , now.tv_sec * 1000 + now.tv_usec / 1000);


	printf("curtime = %s\n", curtime);

	return 0;

}
