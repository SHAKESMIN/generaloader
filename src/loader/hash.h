/*
 * hash.h
 *
 *  created on: 2012-11-13
 *      author: XieChengmin
 */

#ifndef HASH_H_
#define HASH_H_

#include "shm_hash.h"

#define HASH_MIN_STR_SIZE  		32
#define HASH_MID_STR_SIZE 		128
#define HASH_MAX_STR_SIZE 		1024


typedef struct _hashkey_t
{
	char file_pattern[HASH_MID_STR_SIZE];
} hashkey_t;


typedef struct _hashvalue_t
{
	char table_pattern[HASH_MIN_STR_SIZE];
	int  city_pos;
	int  city_len;
	int  yyyy_mm_pos;
	int  yyyy_mm_len;
	char separator[HASH_MIN_STR_SIZE];
	char load_mode[HASH_MIN_STR_SIZE];
	char table_load_fields[HASH_MAX_STR_SIZE];
} hashvalue_t;


/*
hash:
"RA_RB_WL_EBXLOG_ST201211" -> "/input/a201211.unl" -> "/input/b201211.unl" ->...
"RA_RB_WL_EBXLOG_ST201212" -> "/input/a201212.unl" -> "/input/b201212.unl" ->...

*/
#define  MAX_FILES_TO_TABLE_ONCE   1000

typedef struct _hashtabkey_t
{
	char table_name[HASH_MID_STR_SIZE];
} hashtabkey_t;

typedef struct _hashtabvalue_t
{
	char fname_to_load[HASH_MAX_STR_SIZE];
	char table_load_fields[HASH_MAX_STR_SIZE];
	char separator[HASH_MIN_STR_SIZE];
	char load_mode[HASH_MIN_STR_SIZE];
} hashtabvalue_t;


int hash_file_func_print(SHM_READ_ARG(type, key, value));

int hash_table_func_print(SHM_READ_ARG(type, key, value));



#endif /* HASH_H_ */
