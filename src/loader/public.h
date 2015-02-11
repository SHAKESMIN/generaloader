#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fnmatch.h>
#include <sys/access.h>
#include <sys/dir.h>
#include "q_etc.h"
#include "q_util.h"
#include "q_log.h"
#include "zcomp.h"
#include "hash.h"
#include "../sqlldr/sqlldr.h"




#define MAX_PATH  					1024
#define LOADER_MIN_STR_SIZE  		32
#define LOADER_MID_STR_SIZE 		128
#define LOADER_MAX_STR_SIZE 		1024

#define SUCCESS 1
#define FAILURE 0

#define NOT_MATCH 0
#define MATCHED 1

#define _(y)  etc->get_etc_value(#y, cfg.y, sizeof(cfg.y))
#define __(x, y) etc->get_etc_value(#x, #y, cfg.y, sizeof(cfg.y))

typedef struct _config_t
{
	char log_path[MAX_PATH];
	char data_path[MAX_PATH];
	char tmp_path[MAX_PATH];
	char bad_path[MAX_PATH];
	char discard_path[MAX_PATH];
	char control_path[MAX_PATH];
	char userid[LOADER_MID_STR_SIZE];
	char patterns[3 * LOADER_MAX_STR_SIZE];
	int sleep_time;
	int rows;
	int skip;
	long long readsize;
	long long bindsize;
	char direct[LOADER_MIN_STR_SIZE];

	char sqlldr_path[MAX_PATH];

	ShmHash *hash_file;
	ShmHash *hash_tab;
} config_t;


typedef struct _option_t
{
	char config_file[MAX_PATH];

} option_t;


void version();

void welcome(char *program, char *version, char *date, char *time);

/*
 *@return  1: success    0:failed
 */
int get_option(int argc, char** argv, option_t &opt);


/*
 *@return  1: success    0:failed
 */
int load_cfg(char *name, config_t &cfg);


/**
 * @ctl :  fill sqlldr ctl file
 */
void fill_comm_ctl(const config_t &cfg, sqlldr_ctl_t &ctl);


/*
 *@return  1: success    0:failed
 *@input   file:  only a file
 */
int load_files(sqlldr_ctl_t& ctl, const config_t &cfg);

/*
 *@return  1: success    0:failed
 *@input   indir   input dir
 */
int process(const config_t &cfg, sqlldr_ctl_t  &ctl);

int check_format(char *packet);

/*
 * @filename	input file name
 * @v			a struct of load-rule
 * @table		generate the table name
 */
int judge_table(const char *filename, const hashvalue_t &v, char *table);


int del_tmpfile(const char *tmp_dir);

/**
 *  get current time
 *  @input format you want:  eg. 1 -> 20121128092519  2 -> 2012-11-28 09:25:06.525728
 *  @return curtime
 */
char *get_curtime(char *curtime, int format = 1);
