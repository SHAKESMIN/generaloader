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
