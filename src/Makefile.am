SUBDIRS = loader  digger


SUNRISE_INC_PATH=$(shell sl-config --cflags)
INCLUDES=$(SUNRISE_INC_PATH)

SUNRISE_LD_PATH=$(shell sl-config --libs64)
LDADD=$(SUNRISE_LD_PATH)

export INCLUDES
export LDADD
