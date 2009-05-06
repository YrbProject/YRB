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
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include "yrb/ResourceConfig.h"

using namespace std;

namespace yrb {

   ResourceConfig::ResourceConfig()
   {
      this->configFile = string(YRB_CONFIG_DIR) + string(YRB_CONFIG_FILE_NAME);
      try {
         readConfigFile();
      } catch (RBException &e) {
         throw MissingPropertyResourceBundleException();
      }
   }

   ResourceConfig::ResourceConfig(const string &configFile)
   {
      struct stat st;
      this->configFile = configFile;
      if (this->configFile.empty() || (stat(configFile.c_str(), &st) != 0) || (st.st_mode & S_IFDIR)) {
          this->configFile = string(YRB_CONFIG_DIR) + string(YRB_CONFIG_FILE_NAME);
      }
      try {
         readConfigFile();
      } catch (RBException &e) {
	 throw MissingPropertyResourceBundleException();
      }
   }

/*
   ResourceConfig::ResourceConfig(const string &defaultURI, const string &configFile)
   {
      struct stat st;
      this->configFile = configFile;
      if (this->configFile.empty() || (stat(configFile.c_str(), &st) != 0) || (st.st_mode & S_IFDIR)) {
          this->configFile = string(YRB_CONFIG_DIR) + string(YRB_CONFIG_FILE_NAME);
      }
      this->dir = string(YRB_CONFIG_DIR);
      this->defaultURI = defaultURI;
      this->ttl = CONFIG_DEFAULT_TTL;
      this->setLogging( YRB_NOLOG );
      this->numFiles = -1;
      this->mtime = -1;
      this->enableCache = true;
      readConfigFile();
   }
*/

   ResourceConfig::~ResourceConfig()
   {
   }

   const int ResourceConfig::getCacheTTL()
   {
      return this->ttl;
   }

   int ResourceConfig::setCacheTTL(const int &ttl)
   {
      this->ttl = ttl;
      return 0;
   }

   const string ResourceConfig::getURI()
   {
      return getDir();
   }

   const string ResourceConfig::getDir()
   {
      if(this->dir.empty()) return CONFIG_DEFAULT_DIR;
      return this->dir;
   }

   const string ResourceConfig::getConfigFile()
   {
      return this->configFile;
   }

   int ResourceConfig::setDir(const string &dir)
   {
      this->dir = dir;
      return 0;
   }
   
   const yrbLogLevel ResourceConfig::getLogging()
   {
      return this->logging;
   }

   int ResourceConfig::setLogging(const yrbLogLevel &logging)
   {
      this->logging = logging;
      return 0;
   }

   const void ResourceConfig::readConfigFile()
   {
      this->dir = string(YRB_CONFIG_DIR);
      this->ttl = CONFIG_DEFAULT_TTL;
      this->setLogging( YRB_NOLOG );
      this->numFiles = -1;
      this->mtime = -1;
      this->enableCache = true;

      string line;
      size_t pos; 
      ifstream cfile(this->configFile.c_str());
      if (cfile.is_open()) {
         while (!cfile.eof() ) {
           getline (cfile,line);
	   if((pos = line.find_first_not_of(" ")) == string::npos || line.at(pos) == '#')
	      continue;
	   if((pos = line.find_first_of("=")) != string::npos && (line.size() - pos) > 1) {
	      string key = line.substr(0, pos);
	      string value = line.substr(pos+1, line.size()-pos-1);  
	      this->values[key] = value;
	   }
	 }
         cfile.close();
	 if(!this->values["CACHE_TTL"].empty()) {
	    this->ttl = strtol(this->values["CACHE_TTL"].c_str(), NULL, 0);
	 }
	 struct stat st;
	 if(!this->values["DEFAULT_DIR"].empty() && stat(values["DEFAULT_DIR"].c_str(), &st) == 0 && (st.st_mode & S_IFDIR)) {
	    this->setDir(this->values["DEFAULT_DIR"]);
 	 }else {	
	    YRB_LOG(YRB_ERROR, "Missing config directory");
	    throw MissingResourceConfigException("Missing config directory");
	 }
	 if(!this->values["ENABLE_CACHE"].empty() && this->values["ENABLE_CACHE"].compare("no") == 0) {
	    this->ttl = 0;
	    this->enableCache = false;
	 }
         if (!this->values["FILE_LOGGING"].empty()) {
            this->setLogging((yrbLogLevel)atoi((this->values["FILE_LOGGING"]).c_str()));
         } else if (this->values["FILE_LOGGING"].empty()) {
            this->setLogging( YRB_NOLOG ); // Default logging is no logging
         }
      } else {
         YRB_LOG(YRB_ERROR, "Unable to read config file - %s" , (this->configFile).c_str());
         throw RBException("Unable to read config file - " + this->configFile);
      }
   }

   int ResourceConfig::getDirInfo(const string &dir, int *numFile, time_t *lastModifyTime)
   {
      int nf = 0;
      time_t lmt = 0;
      DIR *pdir;
      struct dirent *pent;
      struct stat st;

      pdir=opendir(dir.c_str()); 
      if (!pdir){
         return(-1);
      }
      errno=0;
      while ((pent=readdir(pdir))){
         string fn = dir + "/" + string(pent->d_name);
         if( (stat(fn.c_str(), &st) == 0) ) {
            if(string(pent->d_name).compare(".") != 0 && string(pent->d_name).compare("..") != 0) {
               if(st.st_mode & S_IFDIR){
                  int n;
                  time_t l;
                  this->getDirInfo((char *)fn.c_str(), &n, &l);
                  nf +=n;
                  if(l>lmt) lmt =l;
               }else {
                  string fn = string(pent->d_name);
                  if(fn.size() > 4 && (fn.substr(fn.size()-4,4).compare(".xml") == 0 || fn.substr(fn.size()-5,5).compare(".pres") == 0 )) {
		     size_t ps, pe;
		     string base = ((ps = dir.find_last_of("/")) != string::npos)
					?(dir.substr(ps+1, dir.size()-ps-1)+"/")
					:""; 
		     if((ps = fn.find_first_of("_")) != string::npos && (pe = fn.find_last_of(".")) != string::npos && pe > ps) {
			langTags.push_back(fn.substr(ps+1, pe-ps-1));
			basenames.push_back(base + fn.substr(0, ps));
		     }else {
			if((pe = fn.find_last_of(".")) != string::npos)
			   basenames.push_back(base + fn.substr(0, pe));
		     }
                     nf++;
                     if( st.st_mtime > lmt)
                        lmt = st.st_mtime;
                  }
               }
            }
         }
      }
      if (errno){
         return(-1);
      }
      *numFile = nf;
      *lastModifyTime = lmt;
      closedir(pdir);
      return 0;
   }

   const int ResourceConfig::getNumFiles()
   {
      if(numFiles >= 0)
	 return numFiles;
      this->getDirInfo(dir, &numFiles, &mtime);
      return numFiles;
   }

   const time_t ResourceConfig::getMtime()
   {
      if(mtime >= 0)
         return mtime;
      this->getDirInfo(dir, &numFiles, &mtime);
      return mtime;
   } 

   const list<string> ResourceConfig::getAllLanguageTags()
   {
      if(this->langTags.empty()){
         this->getDirInfo(dir, &numFiles, &mtime);
	 this->langTags.sort();
	 this->langTags.unique();
      }
      return this->langTags;
   }
 
   const list<string> ResourceConfig::getAllBasenames()
   {
      if(this->basenames.empty()){
         this->getDirInfo(dir, &numFiles, &mtime);
         this->basenames.sort();
         this->basenames.unique();
      }
      return this->basenames;
   }
}
