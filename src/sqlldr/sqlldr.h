/*
 * sqlldr.h
 *
 *  created on: 2012-11-5
 *      author: XieChengmin
 */

#ifndef SQLLDR_H_
#define SQLLDR_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/access.h>
#include <unistd.h>


#define SQLLDR_MIN_STR_SIZE  		32
#define SQLLDR_MID_STR_SIZE 		128
#define SQLLDR_MAX_STR_SIZE 		1024

#define SQLLDR_MAX_INFILE			1024


typedef struct _sqlldr_arg_t
{
	char userid[SQLLDR_MIN_STR_SIZE];                        //ORACLE username/password
	char control[SQLLDR_MID_STR_SIZE];                       //control file name
	char log[SQLLDR_MID_STR_SIZE];                           //log file name
	char bad[SQLLDR_MID_STR_SIZE];                           //bad file name
	char data[SQLLDR_MID_STR_SIZE];                          //data file name
	char discard[SQLLDR_MID_STR_SIZE];                       //discard file name
	int  discardmax;                                         //number of discards to allow          (Default all)
	int  skip;                                               //number of logical records to skip    (Default 0)
	int  load;                                               //number of logical records to load    (Default all)
	int  errors;                                             //number of errors to allow            (Default 50)
	int  rows;                                               //number of rows in conventional path bind array or between direct path data saves (Default: Conventional path 64, Direct path all)
	long long  bindsize;                                     //size of conventional path bind array in bytes  (Default 256000)
	char silent[SQLLDR_MIN_STR_SIZE];                        //suppress messages during run (header,feedback,errors,discards,partitions)
	char direct[SQLLDR_MIN_STR_SIZE];                        //use direct path                      (Default FALSE)
	char parfile[SQLLDR_MIN_STR_SIZE];                       //parameter file: name of file that contains parameter specifications
	char parallel[SQLLDR_MIN_STR_SIZE];                      //do parallel load                     (Default FALSE)
	char file[SQLLDR_MIN_STR_SIZE];                          //file to allocate extents from
	char skip_unusable_indexes[SQLLDR_MIN_STR_SIZE];         //disallow/allow unusable indexes or index partitions  (Default FALSE)
	char skip_index_maintenance[SQLLDR_MIN_STR_SIZE];        //do not maintain indexes, mark affected indexes as unusable  (Default FALSE)
	char commit_discontinued[SQLLDR_MIN_STR_SIZE];           //commit loaded rows when load is discontinued  (Default FALSE)
	long long  readsize;                                     //size of read buffer                  (Default 1048576)
	char external_table[SQLLDR_MIN_STR_SIZE];                //use external table for load; NOT_USED, GENERATE_ONLY, EXECUTE  (Default NOT_USED)
	int  columnarrayrows;                                    //number of rows for direct path column array  (Default 5000)
	int  streamsize;                                         //size of direct path stream buffer in bytes  (Default 256000)
	char multithreading[SQLLDR_MIN_STR_SIZE];                //use multithreading in direct path
	char resumable[SQLLDR_MIN_STR_SIZE];                     //enable or disable resumable for current session  (Default FALSE)
	char resumable_name[SQLLDR_MIN_STR_SIZE];                //text string to help identify resumable statement
	int  resumable_timeout;                                  //wait time (in seconds) for RESUMABLE  (Default 7200)
	int  date_cache;                                         //size (in entries) of date conversion cache  (Default 1000)

} sqlldr_arg_t;


typedef struct _sqlldr_ctl_t
{
	sqlldr_arg_t arg;                                         //sqlldr arguments can be given in control file

	char ctl_file_name[SQLLDR_MAX_STR_SIZE];                  //control file name (absolute path). eg: demo.ctl

	/* INFILE  mydat1.dat  BADFILE  mydat1.bad  DISCARDFILE mydat1.dis */
	int  multi_file_num;
	char infile_name[SQLLDR_MAX_INFILE][SQLLDR_MID_STR_SIZE]; //data file, one or more
	char badfile_name[SQLLDR_MAX_INFILE][SQLLDR_MID_STR_SIZE];//bad file, one or more
	char discard_name[SQLLDR_MAX_INFILE][SQLLDR_MID_STR_SIZE];//bad file, one or more
	char load_mode[SQLLDR_MIN_STR_SIZE];                      //insert, append, replace, truncate
	char into_table[SQLLDR_MID_STR_SIZE];                     //into table
	char separator[SQLLDR_MIN_STR_SIZE];                      //separator
	char loaded_fields[SQLLDR_MAX_STR_SIZE];                  //eg: (field1,field2,field3,field4...)
	char when[SQLLDR_MID_STR_SIZE];							  //when for discard

} sqlldr_ctl_t;



int sqlldr_load(sqlldr_ctl_t &ctl, const char *cmd_path);

int sqlldr_create_ctl(const sqlldr_ctl_t &ctl);



#endif /* SQLLDR_H_ */
