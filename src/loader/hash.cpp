/*
 * hash.cpp
 *
 *  created on: 2012-11-13
 *      author: XieChengmin
 */
#include "hash.h"



int hash_file_func_print(SHM_READ_ARG(type, key, value))
{
	hashvalue_t *h = (hashvalue_t *)value;
    printf("key=%s, value=[table_pattern=%s, table_load_fields=%s, city_pos=%d, \
city_len=%d, yyyy_mm_dd_pos=%d, yyyy_mm_dd_len=%d, separator=%s, load_mode=%s]\n",
    		(char *)key,
    		h->table_pattern,
    		h->table_load_fields,
    		h->city_pos,
    		h->city_len,
    		h->yyyy_mm_pos,
    		h->yyyy_mm_len,
    		h->separator,
    		h->load_mode);

    return 0;
}

int hash_table_func_print(SHM_READ_ARG(type, key, value))
{
	hashtabvalue_t *h = (hashtabvalue_t *)value;
    printf("key=%s, value=[filename=%s,%s]\n", (char *)key, h->fname_to_load, h->table_load_fields);

    return 0;
}

