bin_PROGRAMS     =   raloader
raloader_SOURCES =   main.cpp \
                     public.cpp \
                     public.h \
                     hash.cpp \
                     hash.h \
                     ../sqlldr/sqlldr.cpp \
                     ../sqlldr/sqlldr.h


version = $(shell svn info|grep 'Last Changed Rev'|head -1|sed 's/.* \([0-9]\)/\1/')
sl_version = $(shell sl-config --version | perl -pechomp)
sunrise_inc_path=$(shell sl-config --cflags)

sunrise_ld_path=$(shell sl-config --libs64)

raloader_CPPFLAGS=$(sunrise_inc_path) -D_VERSION="\"$(version)\"" -D_SL_VERSION="\"sunrise-library: $(sl_version)\""
raloader_LDFLAGS=$(sunrise_ld_path)