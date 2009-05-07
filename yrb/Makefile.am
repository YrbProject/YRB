## Source directory

h_sources = ResourceBundleCache.h ResourceFile.h yrbLog.h Exception.h PropertyResourceBundle.h ResourceConfig.h
cc_sources = ResourceBundleCache.cc ResourceFile.cc yrbLog.cc Exception.cc PropertyResourceBundle.cc ResourceConfig.cc

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)
i
yrb_cachedir=/home/y/var/yrb/cache/config
yrb_cache_DATA=config.dir
yrb_configdir=/home/y/conf/yrb
yrb_config_DATA=yrb.conf

INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES= libyrb-1.0.la
libyrb_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libyrb_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)

