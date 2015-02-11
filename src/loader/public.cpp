#include "public.h"


void welcome(char *program, char *version, char *date, char *time)
{
	LOG("[program:%s, version:%s, date:%s, time:%s]\n",
			program, version, date, time);
}

void version()
{
	/* _VERSION  _SL_VERSION  macro defines in Makefile.am */
	printf("version: [%s] sl_version: [%s] \n", _VERSION, _SL_VERSION);
}

int get_option(int argc, char** argv, option_t &opt)
{
	int ch = 0;
	const char optstring[] = "hvc:";
	if (argc < 2)
		return FAILURE;
	while ((ch = getopt(argc, argv, optstring)) != -1)
	{
		switch (ch)
		{
			case 'c':
				sprintf(opt.config_file, "%s", optarg);
				break;
			case 'v':
				version();
				return FAILURE;
			case '?':
				printf("unknown opt.\n");
			case 'h':
			default:
				return FAILURE;
		}
	}

	/* check option value: <must> */
	if (-1 == access(opt.config_file, F_OK))
	{
		printf("config file error or not exist.\n");
		return FAILURE;
	}

	printf("opt.config_file = %s\n", opt.config_file);

}

int load_cfg(char *name, config_t &cfg)
{
	QEtc *etc;

	etc = new QEtc();
	if (etc->etc_init(name) == -1)
		return FAILURE;

	/* get cfg value */
	__(loader, log_path);
	__(loader, data_path);
	__(loader, tmp_path);
	__(loader, bad_path);
	__(loader, discard_path);
	__(loader, control_path);
	__(loader, userid);
	__(loader, sqlldr_path);
	__(loader, sleep_time);
	__(loader, patterns);
	__(perform, rows);
	__(perform, skip);
	__(perform, readsize);
	__(perform, bindsize);
	__(perform, direct);


	/* check cfg value*/
	if (-1 == access(cfg.log_path, F_OK))
	{
		printf("[log_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.data_path, F_OK))
	{
		printf("[data_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.tmp_path, F_OK))
	{
		printf("[tmp_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.discard_path, F_OK))
	{
		printf("[discard_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.control_path, F_OK))
	{
		printf("[control_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.bad_path, F_OK))
	{
		printf("[bad_path] error or not exist.\n");
		return FAILURE;
	}
	if (-1 == access(cfg.sqlldr_path, F_OK))
	{
		printf("[sqlldr_path] error or not exist.\n");
		return FAILURE;
	}

	printf("cfg.log_path  = %s\n", cfg.log_path);
	printf("cfg.data_path = %s\n", cfg.data_path);
	printf("cfg.tmp_path = %s\n", cfg.tmp_path);
	printf("cfg.rows = %d\n", cfg.rows);
	printf("cfg.skip = %d\n", cfg.skip);
	printf("cfg.patterns = %s\n", cfg.patterns);
	printf("cfg.direct = %s\n", cfg.direct);
	printf("cfg.discard_path = %s\n", cfg.discard_path);
	printf("cfg.control_path = %s\n", cfg.control_path);
	printf("cfg.sqlldr_path = %s\n", cfg.sqlldr_path);


	/* split special cfg */
	split_t sp_patterns;
	split(';', cfg.patterns, sp_patterns);

	cfg.hash_tab = new ShmHash();
	if (cfg.hash_tab->sh_init(MAX_FILES_TO_TABLE_ONCE, sizeof(hashtabkey_t), sizeof(hashtabvalue_t)) == -1)
	{
		printf("has_tab init error\n");
		return FAILURE;
	}

	cfg.hash_file = new ShmHash();
	int hashsize = split_total(sp_patterns);
	if (cfg.hash_file->sh_init(hashsize, sizeof(hashkey_t), sizeof(hashvalue_t)) == -1)
	{
		printf("hash init error\n");
		return FAILURE;
	}

	for (int i = 0;  i < hashsize; i++)
	{
		hashkey_t k = { 0 };
		hashvalue_t v = { 0 };
	    split_t sp_inner;
	    split(',', split_value(sp_patterns, i), sp_inner);

	    if (split_total(sp_inner) != 6)
	    {
	    	printf("split inner error\n");
	    	return FAILURE;
	    }
	    sprintf(k.file_pattern, "%s", split_value(sp_inner, 0));
	    sprintf(v.table_pattern, "%s", split_value(sp_inner, 1));
	    sprintf(v.separator, "%s", split_value(sp_inner, 4));
	    sprintf(v.load_mode, "%s", split_value(sp_inner, 5));

	    split_t sp_city;
	    split_t sp_yyyymm;
	    split('-', split_value(sp_inner, 2), sp_city);
	    split('-', split_value(sp_inner, 3), sp_yyyymm);

	    if (split_total(sp_city) != 2 || split_total(sp_yyyymm) != 2)
		{
			LOG("ERROR: split city or yyyymmdd error\n");
			return FAILURE;
		}
	    v.city_pos = atoi(split_value(sp_city, 0));
	    v.city_len = atoi(split_value(sp_city, 1));
	    v.yyyy_mm_pos = atoi(split_value(sp_yyyymm, 0));
	    v.yyyy_mm_len = atoi(split_value(sp_yyyymm, 1));

	    /*get load-fileds of table dynamic*/
	    etc->get_etc_value("loader", v.table_pattern, v.table_load_fields, sizeof(v.table_load_fields));
	    printf("v.table_load_fields = %s\n", v.table_load_fields);

	    if (cfg.hash_file->sh_add(&k, &v) == -1)
	    {
	    	LOG("error\n");
	    	return FAILURE;
	    }
	}

	cfg.hash_file->sh_sort_read_all(NULL, hash_file_func_print);

	delete etc;

	return SUCCESS;
}


void fill_comm_ctl(const config_t &cfg, sqlldr_ctl_t &ctl)
{
	ctl.arg.rows = cfg.rows;
	ctl.arg.skip = cfg.skip;
	ctl.arg.bindsize = cfg.bindsize;
	ctl.arg.readsize = cfg.readsize;
	sprintf(ctl.arg.direct, "%s", cfg.direct);
	sprintf(ctl.arg.userid, "%s", cfg.userid);
}


int load_files(sqlldr_ctl_t& ctl, const config_t &cfg)
{
	int ret = sqlldr_load(ctl, cfg.sqlldr_path);

	LOG("invoke sqlldr: ret = %d, errno = %d.\n", ret, errno);
	if (-1 == ret)
	{
		return FAILURE;
	}

	return SUCCESS;

}

int process(const config_t &cfg, sqlldr_ctl_t  &ctl)
{
	DIR *dirp = NULL;
	struct dirent *direntp = NULL;
	int ret = 0;

	if ((dirp = opendir(cfg.data_path)) == NULL)
	{
		LOG("ERROR: indir [%s] open failed: %s\n", cfg.data_path, strerror(errno));
		return FAILURE;
	}

	/* step1:  scanning dir, create hash about table&files */
	int count = 0;
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (direntp->d_name[0] == '.')
			continue;

		if (count > ( MAX_FILES_TO_TABLE_ONCE -1 ))
		{
			LOG("ERROR:max process-files support: %d", MAX_FILES_TO_TABLE_ONCE);
			return FAILURE;
		}

		/* judge the file in the file-tab-pos-len hash or not */
		int match_flag = NOT_MATCH;
		hashkey_t  tmpkey = { 0 };
		hashvalue_t tmpvalue = { 0 };
		cfg.hash_file->sh_reset();
		while (cfg.hash_file->sh_read_next(&tmpkey, &tmpvalue) == 0)
		{
			if (fnmatch(tmpkey.file_pattern, direntp->d_name, 0) != 0)
				continue;
			match_flag = MATCHED;
			break;
		}
		if (NOT_MATCH == match_flag)
			continue;

		/* matched-a-rule */
		count++;

		char table[32 + 1] = { 0 };
		char packet[MAX_PATH] = { 0 };
		char orgin_packet[MAX_PATH] = { 0 };

		snprintf(orgin_packet, sizeof(orgin_packet) - 1, "%s/%s", cfg.data_path, direntp->d_name);

		/* if .Z decompress it */
		if (strncmp(".Z", direntp->d_name + strlen(direntp->d_name) - 2, 2) == 0)
		{
			LOG("[APP] the file is a compressed file .Z, need decompress\n");
			char  no_z_fname[MAX_PATH] = { 0 };
			snprintf(no_z_fname, strlen(direntp->d_name) - 1, "%s", direntp->d_name);
			snprintf(packet, sizeof(packet) - 1, "%s/%s", cfg.tmp_path, no_z_fname);
			CZip  *z = new CZip();
			z->decompress(ZIP_FILE2FILE, orgin_packet, 0, packet, 0);
			delete z;

			if (unlink(orgin_packet) != 0)
				LOG("WARNING: unlink file [%s] failed.\n", orgin_packet);
		}
		else
		{
			LOG("[APP] the file is not a compressed file , no need decompress\n");
			snprintf(packet, sizeof(packet) - 1, "%s", orgin_packet);
		}

		LOG("APP: process file [%s] start.\n", packet);

		/* check the format and field */
		ret = check_format(packet);

		/* judge the table which to load */
		ret = judge_table(direntp->d_name, tmpvalue, table);
		if (SUCCESS != ret)
		{
			LOG("ERROR: judge_table failed.\n");
			return FAILURE;
		}

		/* add files to tab-hash */
		hashtabkey_t k = { 0 };
		hashtabvalue_t v = { 0 };
		sprintf(k.table_name, "%s", table);
		sprintf(v.fname_to_load, "%s", packet);
		sprintf(v.table_load_fields, "%s", tmpvalue.table_load_fields);
		sprintf(v.separator, "%s", tmpvalue.separator);
		sprintf(v.load_mode, "%s", tmpvalue.load_mode);
		if (cfg.hash_tab->sh_add(&k, &v) == -1)
		{
			LOG("ERROR: hash_tab  sh_add failed.\n");
			return FAILURE;
		}
		LOG("APP: process packet [%s] done.\n", packet);
	}
	closedir(dirp);
	if (0 == count)
	{
		LOG("no matched file(s) in [%s]\n", cfg.data_path);
		return SUCCESS;
	}
	cfg.hash_tab->sh_sort_read_all(NULL, hash_table_func_print);


	/* step2: create the control files for sqlldr and load */
	hashtabkey_t tabk = { 0 };
	hashtabvalue_t tabv = { 0 };
	ShmHash *hash = cfg.hash_tab;
	hash->sh_reset();
	while (hash->sh_read_next(&tabk, &tabv) == _SHM_HASH_FOUND)
	{
		memset(&ctl, 0, sizeof(ctl));
		fill_comm_ctl(cfg, ctl);

		sprintf(ctl.into_table, "%s", tabk.table_name);
		char curtime[30] = { 0 };
		get_curtime(curtime);
		sprintf(ctl.ctl_file_name, "%s/%s_%s.ctl", cfg.control_path, tabk.table_name, curtime);
		sprintf(ctl.arg.log, "%s/%s_%s.log", cfg.log_path, tabk.table_name, curtime);
		sprintf(ctl.when, "%s", "WHEN (1:3) <> 'HDR'");

		hashtabvalue_t *pv;
		hash->sh_begin();
		ctl.multi_file_num = 0;
		while (hash->sh_next(&tabk, (void **)&pv) == _SHM_HASH_FOUND)
		{
			sprintf(ctl.infile_name[ctl.multi_file_num], "%s", pv->fname_to_load);

			char *t1 = strrchr(pv->fname_to_load, '/');
			char *t2 = strrchr(pv->fname_to_load, '.');
			char prefix[128] = { 0 };
			snprintf(prefix, t2-t1, "%s", t1+1);

			sprintf(ctl.badfile_name[ctl.multi_file_num], "%s/%s.bad", cfg.bad_path, prefix);
			sprintf(ctl.discard_name[ctl.multi_file_num], "%s/%s.dis", cfg.discard_path, prefix);
			ctl.multi_file_num++;

			sprintf(ctl.loaded_fields, "%s", pv->table_load_fields);
			sprintf(ctl.load_mode, "%s", pv->load_mode);
			sprintf(ctl.separator, "%s", pv->separator);
		}

		/* each table a control file */
		sqlldr_create_ctl(ctl);

		/* loading a table data once a time */
		ret = load_files(ctl, cfg);
		if (ret == FAILURE)
		{
			LOG("ERROR: load_files failed.\n");
			return FAILURE;
		}

		if (hash->sh_delete_all(&tabk) < 0)
		{
			LOG("sh_delete_all: %s error\n", tabk.table_name);
			return FAILURE;
		}
		/* del orgin file (no-Zip) */
		while (hash->sh_next(&tabk, (void **)&pv) == _SHM_HASH_FOUND)
		{
			if (unlink(pv->fname_to_load) != 0)
				LOG("ERROR: unlink file [%s] failed.\n", pv->fname_to_load);
		}
	}

	return SUCCESS;
}

int check_format(char *packet)
{
	LOG("APP: check format...\n");
	return SUCCESS;
}

int judge_table(const char *filename, const hashvalue_t &v, char *table)
{
	/* eg:CXSJDRHJF_A05029AAYYYYMMDDXXXXXX.AVL.Z */
	char city[3] = { 0 };
	char yyyymm[5] = { 0 };

	snprintf(city, v.city_len + 1, "%s", filename + v.city_pos);
	snprintf(yyyymm, v.yyyy_mm_len + 1, "%s", filename + v.yyyy_mm_pos);

	/*q_toupper(city);
	q_toupper(v.table_pattern);*/

	/* validate */
	const char cities[22][3] = {
		"GZ",                  /* 广州 */
		"SZ",                  /* 深圳 */
		"ZH",                  /* 珠海 */
		"FS",                  /* 佛山 */
		"ST",                  /* 汕头 */
		"HZ",                  /* 惠州 */
		"ZJ",                  /* 湛江 */
		"JM",                  /* 江门 */
		"ZQ",                  /* 肇庆 */
		"SG",                  /* 韶关 */
		"MZ",                  /* 梅州 */
		"DG",                  /* 东莞 */
		"ZS",                  /* 中山 */
		"SD",                  /* 顺德 */
		"MM",                  /* 茂名 */
		"SW",                  /* 汕尾 */
		"CZ",                  /* 潮州 */
		"JY",                  /* 揭阳 */
		"YJ",                  /* 阳江 */
		"QY",                  /* 清远 */
		"HY",                  /* 河源 */
		"YF"                   /* 云浮*/
	};
	int city_match = NOT_MATCH;
	for (int i = 0; i < 22; i++)
	{
		DEBUG("city:%s|cities:%s\n", city, cities[i]);
		if (strcmp(city, cities[i]) == 0)
		{
			city_match = MATCHED;
			break;
		}
	}
	if (NOT_MATCH == city_match)
	{
		LOG("ERROR: city invalid of file <%s>. city:%s\n", filename,city);
		return FAILURE;
	}

	/*eg:  RA_RB_WL_EBXLOG_SZ201212 */
	sprintf(table, "RA_%s_%s_%s", v.table_pattern, city, yyyymm);

	LOG("table name : %s\n", table);

	return SUCCESS;
}


int del_tmpfile(const char *tmp_dir)
{
	DIR *dirp = NULL;
	struct dirent *direntp = NULL;
	int ret = 0;
	char tmp_file[MAX_PATH] = { 0 };

	if ((dirp = opendir(tmp_dir)) == NULL)
	{
		LOG("ERROR: indir [%s] open failed: %s\n", tmp_dir, strerror(errno));
		return FAILURE;
	}
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (direntp->d_name[0] == '.')
			continue;

		snprintf(tmp_file, sizeof(tmp_file) - 1, "%s/%s", tmp_dir, direntp->d_name);
		if (unlink(tmp_file) != 0)
		{
			LOG("ERROR: unlink file [%s] failed.\n", tmp_file);
		}
	}
	return SUCCESS;
}


char *get_curtime(char *curtime, int format)
{
    struct tm *lt;
    struct timezone tz;
    struct timeval now;
    gettimeofday(&now, &tz);
    lt = localtime(&now.tv_sec);

    memset(curtime , 0 ,sizeof(curtime));

    if(format==0 || format > 2 || format < 0)
        sprintf(curtime, "%4d-%02d-%02d %02d:%02d:%02d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
    else if(format ==1)
        sprintf(curtime, "%4d%02d%02d%02d%02d%02d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
    else if(format ==2)
        sprintf(curtime, "%4d-%02d-%02d %02d:%02d:%02d.%06d",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec,now.tv_usec);
    else if(format == 3 )
    	sprintf(curtime , "%d" , now.tv_sec * 1000 + now.tv_usec / 1000);
    return curtime;
}
