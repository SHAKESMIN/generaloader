/*
 * sqlldr.cpp:  encapsulation of oracle sqlldr
 *
 *  created on: 2012-11-01
 *      author: XieChengmin
 */
#include "sqlldr.h"


int sqlldr_load(sqlldr_ctl_t &ctl, const char *cmd_path)
{
	char cmd[1024] = { 0 };

	/* check oracle - sqlldr exist or not */
	if (-1 == access(cmd_path, F_OK))
	{
		printf("[sqlldr] %s not the right sqlldr.\n", cmd_path);
		return -1;
	}

	/* sqlldr ra/ra_test123@jftest control=demo.ctl */

	sprintf(cmd, "%s %s control=%s log=%s",
			cmd_path,
			ctl.arg.userid,
			ctl.ctl_file_name,
			ctl.arg.log
	);

	printf("[sqlldr]cmd = %s\n", cmd);

	/*
	 * If SQL*Loader returns any exit code other than zero,
	 * you should consult your system log files and SQL*Loader log files for more detailed diagnostic information.
	 * For UNIX, the exit codes are as follows:
		EX_SUCC 0
		EX_FAIL 1
		EX_WARN 2
		EX_FTL  3
	 */
	int ret = system(cmd);

	printf("[sqlldr ret = %d] sqlldr finished.\n", ret);
	return ret;
}


int sqlldr_create_ctl(const sqlldr_ctl_t &ctl)
{
	/* sqlldr control file, eg:
		OPTIONS (skip=1,rows=128)
		LOAD DATA
		INFILE "demo.txt"
		append
		INTO TABLE RA_TEST_FOR_XCM_TB
		Fields terminated by "|"

		trailing nullcols
		(
		REC_ID,
		CITY,
		BUSI_TYPE,
		KPI,
		BILL_TYPE,
		TIME_POINT DATE "YYYY-MM-DD HH24:MI:SS",
		PERIOD
		)
	 */
	int i =0;
	FILE *f = fopen(ctl.ctl_file_name, "w");

	fprintf(f, "OPTIONS(skip=%d, rows=%d, bindsize=%ld, readsize=%ld, direct=%s, log=\"%s\")\n",
			ctl.arg.skip, ctl.arg.rows, ctl.arg.bindsize, ctl.arg.readsize, ctl.arg.direct,
			ctl.arg.log);
	fprintf(f, "%s\n", "LOAD DATA");

	for(i = 0; i < ctl.multi_file_num; i++)
	{
		fprintf(f, "INFILE \"%s\" BADFILE \"%s\" DISCARDFILE \"%s\" \n",
				ctl.infile_name[i], ctl.badfile_name[i], ctl.discard_name[i]);
	}
	fprintf(f, "%s\n", ctl.load_mode);
	fprintf(f, "INTO TABLE %s\n", ctl.into_table);
	fprintf(f, "%s\n", ctl.when);
	fprintf(f, "Fields terminated by \"%s\"\n", ctl.separator);

	fprintf(f, "trailing nullcols\n(%s)\n", ctl.loaded_fields);

	fflush(f);
	fclose(f);

	return 1;
}
