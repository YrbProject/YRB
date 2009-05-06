/***
* Yahoo! Resource Bundle (C++) Copyright License Agreement (BSD License)
*
* Copyright (c) 2009, Yahoo! Inc.
* All rights reserved.
*
* Redistribution and use of this software in source and binary forms, with or
* without modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above
*   copyright notice, this list of conditions and the
*   following disclaimer.
*
* * Redistributions in binary form must reproduce the above
*   copyright notice, this list of conditions and the
*   following disclaimer in the documentation and/or other
*   materials provided with the distribution.
*
* * Neither the name of Yahoo! Inc. nor the names of its
*   contributors may be used to endorse or promote products
*   derived from this software without specific prior
*   written permission of Yahoo! Inc.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* This code also contains software from the following sources:
*
* sdbm - ndbm work-alike hashed database library based on Per-Aake Larson's
*        Dynamic Hashing algorithms.
* BIT 18 (1978).
*
* Copyright (c) 1991 by Ozan S. Yigit (oz@nexus.yorku.ca)
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation.
*
* This file is provided AS IS with no warranties of any kind.  The author shall
* have no liability with respect to the infringement of copyrights, trade
* secrets or any patents by this file or any part thereof.  In no event will
* the author be liable for any lost revenue or profits or other special,
* indirect and consequential damages.
*
***/
#ifndef _RESOURCEBUNDLE_CACHE_H_
#define _RESOURCEBUNDLE_CACHE_H_

#ifdef __cplusplus

#include <list>
#include <string>
#include <map>
#include <time.h>
#include "Exception.h"
#include "yrbLog.h"

extern "C" {
#include "apr_sdbm.h"
}

#define YRB_CONFIG_FILES "config/config_files"
#define YRB_CONFIG_IDS "config_ids"
#define YRB_CACHE_DIR "/home/y/var/yrb/cache/"
#define YRB_MAX_PAGE_SIZE 32*1024+128
#define YRB_REG_PAGE 4*1024
#define YRB_BIG_PAGE 32*1024+128

namespace yrb {

class ResourceBundleCache
{
    public:
     	ResourceBundleCache();
	ResourceBundleCache(const std::string &cacheFile, const int flag, const int page_size);
        ~ResourceBundleCache();
	const std::string getCacheName();
	const apr_sdbm_t* getDb(); 
        void setDb(apr_sdbm_t *db);
	const std::string getValue(const std::string &key);
        const std::list<std::string> getKeys();
        void store(const std::string &k, const std::string &v, const int flag);
        void store(const std::map<std::string,std::string> &kv, const int flag);
	std::map<std::string,std::string> *getStore();


    private:
	std::string cacheName;
	time_t lastCheckTime;
	std::map<std::string,std::string> *cstore;
        void setLastCheckTime(const time_t &time);
	void setCacheName(const std::string &cacheName);
	apr_sdbm_t* db;
	mode_t pre_umask;
};
}
#endif

#endif // ifndef _RESOURCEBUNDLE_CACHE_H_

