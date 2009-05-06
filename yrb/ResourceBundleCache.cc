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
#include <sys/time.h>
#include <set>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include "yrb/ResourceBundleCache.h"

using namespace std;

namespace yrb {

   ResourceBundleCache::ResourceBundleCache()
   {
   }

   ResourceBundleCache::~ResourceBundleCache()
   {
      if(this->db) {
         apr_sdbm_close(this->db);
	 this->db = NULL;
	 umask(this->pre_umask); 
      }
   }

   ResourceBundleCache::ResourceBundleCache(const string &cacheFile, const int flag, const int page_size)
   {
      this->cacheName = cacheFile; 
      this->pre_umask = umask(0);
      apr_status_t rv;
      apr_pool_t *mp;
      rv = apr_initialize();
      if (rv != APR_SUCCESS) {
	YRB_LOG(YRB_ERROR, "apr_initialize failed");
	throw MissingResourceCacheException();
      }
      apr_pool_create(&mp, NULL);
      apr_status_t ret = apr_sdbm_open((apr_sdbm_t **)&this->db, cacheFile.c_str(),flag, APR_OS_DEFAULT, mp); // page_size, 0);
      if(!this->db) {	
	 YRB_LOG(YRB_ERROR, "apr_sdbm_open fail: %s, error: %d" , cacheFile.c_str(), ret);
	 throw MissingResourceCacheException();
      }
   }

   map<string,string> *ResourceBundleCache::getStore()
   {
      cstore = new map<string,string>;
      apr_sdbm_datum_t key, value;
      apr_sdbm_lock(db, APR_FLOCK_EXCLUSIVE);
      apr_sdbm_firstkey(this->db, &key);
      apr_sdbm_fetch(this->db, &value, key);
      while (key.dptr) {
        cstore->insert(pair<string,string>(string(key.dptr, key.dsize), string(value.dptr, value.dsize)));
	apr_sdbm_nextkey(this->db, &key);
        apr_sdbm_fetch(this->db, &value, key);
      }
     
      apr_sdbm_unlock(db);
      return cstore;
   }

   const string ResourceBundleCache::getValue(const string &key)
   {
      apr_sdbm_datum_t k, val;
      int len = key.size();
      k.dptr = (char *)key.c_str();
      k.dsize = len;
      apr_sdbm_lock(this->db, APR_FLOCK_SHARED);
      apr_sdbm_fetch(this->db,&val, k);
      apr_sdbm_unlock(this->db);
      return(string(val.dptr, val.dsize));
   }

   const string ResourceBundleCache::getCacheName()
   {
      return this->cacheName;
   }

   const list<string> ResourceBundleCache::getKeys()
   {
      list<string> keys;
      apr_sdbm_datum_t k;
      apr_sdbm_lock(this->db, APR_FLOCK_SHARED);
      apr_sdbm_firstkey(this->db, &k);
      while (k.dptr) {
	keys.push_back(string(k.dptr, k.dsize));
        apr_sdbm_nextkey(this->db, &k);
      }
      apr_sdbm_unlock(this->db);
      return keys;
   }

   void ResourceBundleCache::store(const std::string &k, const std::string &v, const int flag)
   {
      apr_sdbm_datum_t key, val;
      apr_status_t ret;
      key.dptr = (char *)k.c_str();
      key.dsize = k.size();
      val.dptr = (char *)v.c_str();
      val.dsize = v.size();
      apr_sdbm_lock(this->db, APR_FLOCK_EXCLUSIVE);
      ret = apr_sdbm_store(db,key,val,flag);
      apr_sdbm_unlock(this->db);
   }

   void ResourceBundleCache::store(const std::map<std::string,std::string> &kv, const int flag)
   {
      apr_sdbm_datum_t key, val;
      apr_status_t ret;
      apr_sdbm_lock(this->db, APR_FLOCK_EXCLUSIVE);
      map<string,string>::iterator it;
      map<string,string> keyValue = kv;
      for(it=keyValue.begin(); it!=keyValue.end(); it++) {
        key.dptr = (char *)(*it).first.c_str();
        key.dsize = (*it).first.size();
        val.dptr = (char *)(*it).second.c_str();
        val.dsize = (*it).second.size();
        ret = apr_sdbm_store(db,key,val,flag);
      }
      apr_sdbm_unlock(this->db);
   }

   const apr_sdbm_t* ResourceBundleCache::getDb()
   {
      return this->db;
   }

   void ResourceBundleCache::setDb(apr_sdbm_t *db)
   {
      this->db = db;
   }
    
   void ResourceBundleCache::setCacheName(const std::string &cacheName) 
   {
      this->cacheName = cacheName;
   }

   void ResourceBundleCache::setLastCheckTime(const time_t &time)
   {
      this->lastCheckTime = time;
   }

}
