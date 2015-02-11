/*
 * main.cpp: ra_loader
 *
 *  created on: 2012-11-01
 *      author: XieChengmin
 */

#include "public.h"


#define PROGRAM  "ra_loader"


int usage()
{
	printf("usage: loader [-h] <-c configure file>\n");
	printf("        -c configure file. <must>\n");
	printf("        -v version info.\n");
	printf("        -h show help.\n");
}


int main(int argc, char **argv)
{
	config_t      cfg = { 0 };
	option_t      opt = { 0 };
	sqlldr_ctl_t  ctl = { 0 };

	if (!get_option(argc, argv, opt))
	{
		usage();
		exit(-1);
	}

	welcome(PROGRAM, _VERSION, __DATE__, __TIME__);

	if (!load_cfg(opt.config_file, cfg))
	{
		printf("loadcfg  failed.\n");
		exit(-1);
	}

	char curtime[30] = { 0 };
	char curr_log_name[128] = { 0 };
	get_curtime(curtime);
	sprintf(curr_log_name, "ra_loader_%s.log", curtime);
	if (LOG_INIT(cfg.log_path, curr_log_name, NULL) == -1)
	{
		printf("ERROR: log init error.\n");
		exit(-1);
	}

	/* single-proc test */
	if (q_write_pid(cfg.data_path, ".pid", getpid()) == -1)
	{
		printf("ERROR: single-proc for a directory.\n");
		exit(-1);
	}


	printf("start_daemon.\n");
	start_daemon();


	/* ignore_all_signal(); */

	if (process(cfg, ctl) != SUCCESS) {
		LOG("ERROR: %s load failed.\n", PROGRAM);
		del_tmpfile(cfg.tmp_path);
	}


	del_tmpfile(cfg.tmp_path);
	delete cfg.hash_file;
	delete cfg.hash_tab;

	LOG("%s exit.\n", PROGRAM);

	return 0;
}
