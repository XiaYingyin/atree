EXTENSION = ftree
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")
TESTS        = $(wildcard test/sql/*.sql)

# more test are added later, after including pgxs
#REGRESS      = hypo_setup \
#	       hypo_index


#REGRESS_OPTS = --inputdir=test

PG_CONFIG ?= pg_config

MODULE_big = ftree 

OBJS = ftcompare.o ftinsert.o ftpage.o ftree.o ftsearch.o \
       ftutils.o ftsort.o ftvalidate.o ftxlog.o

DATA = $(wildcard *--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

DEBUILD_ROOT = /tmp/$(EXTENSION)

deb: release-zip
	mkdir -p $(DEBUILD_ROOT) && rm -rf $(DEBUILD_ROOT)/*
	unzip ./${EXTENSION}-$(EXTVERSION).zip -d $(DEBUILD_ROOT)
	cd $(DEBUILD_ROOT)/${EXTENSION}-$(EXTVERSION) && make -f debian/rules orig
	cd $(DEBUILD_ROOT)/${EXTENSION}-$(EXTVERSION) && debuild -us -uc -sa
