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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "PropertyResourceBundle.h"

using namespace std;

namespace yrb {
static bool compare_nocase (string s1, string s2)
{
  size_t pos;
  string first = ((pos = s1.find_last_of(".")) != string::npos)?s1.substr(0,pos):s1;
  string second = ((pos = s2.find_last_of(".")) != string::npos)?s1.substr(0,pos):s1;
  unsigned int i=0;
  while ( (i<first.length()) && (i<second.length()) )
  {
    if (tolower(first[i])<tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  if (first.length()>second.length()) return true;
  else return false;
}

typedef unsigned char BYTE;

static BYTE toHex(const BYTE &x)
{
    return x > 9 ? x + 55: x + 48;
}

static inline string percentEncoding( string &sIn )
{
     string sOut = "";
     for( int ix = 0; ix < (int)sIn.size(); ix++ ) {
         BYTE buf[4];
         memset( buf, 0, 4 );
         if( isalnum( (BYTE)sIn[ix] ) ) {
             buf[0] = sIn[ix];
         }else {
             buf[0] = '%';
             buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
             buf[2] = toHex( (BYTE)sIn[ix] % 16);
         }
         sOut += (char *)buf;
     }
     return sOut;
}

string ugtInstrument(const string &uT, const string &ugtValue, const string &percentEncodingValue, const string &value)
{
    size_t pos = 0;
    string tplt = uT;
    if((pos = tplt.find("{0}")) != string::npos) {
       string t1 = tplt.replace(pos,3, ugtValue);
       if((pos = t1.find("{1}")) != string::npos) {
	  string t2 = t1.replace(pos,3, percentEncodingValue);
	  if((pos = t2.find("{2}")) != string::npos) { 
	     return t2.replace(pos,3, value);
	  }
       }
    }
    return "";
}
	      
   PropertyResourceBundle::PropertyResourceBundle(const string &basename, const string &locale)
   {
      this->config = new ResourceConfig();
      this->basename = basename;
      this->locale = locale;
      try {
         this->init();
      } catch (MissingPropertyResourceBundleException e) {
 	 if(this->config) delete this->config;
         for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
            if(mrf[(*it).c_str()])
               delete mrf[(*it).c_str()];
         }

         throw MissingPropertyResourceBundleException(e.what());
      }

   } 

   PropertyResourceBundle::PropertyResourceBundle(const string &basename, const string &locale, const string &configFile)
   {
      this->basename = basename;
      this->locale = locale;
      // Initialize logging
      yrb::yrbLog *yLog = yrbLog::getInstance(YRB_DEBUG, YRB_LOG_FILE);
      yLog->writeLog(YRB_DEBUG, __FILE__,  __LINE__,  ("PropertyResourceBundle: "+configFile).c_str());
      this->config = new ResourceConfig(configFile);
      yrbLog::getInstance()->setLogLevel(this->config->getLogging());
      try {
            this->init();
      } catch (MissingPropertyResourceBundleException e) {
	    if(this->config) delete this->config;
            for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
               if(mrf[(*it).c_str()])
                  delete mrf[(*it).c_str()];
            }
     	    throw MissingPropertyResourceBundleException(e.what());
      }
      YRB_LOG(YRB_DEBUG, "End PropertyResourceBundle constructor");
   }

   PropertyResourceBundle::~PropertyResourceBundle()
   {
      if(this->config) delete this->config;
      for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
         if(mrf[(*it).c_str()])
	    delete mrf[(*it).c_str()];
      }
      if(this->ugtSegId) delete this->ugtSegId;
   }
   
   void PropertyResourceBundle::init() 
   {
      YRB_LOG(YRB_DEBUG, "Begin PropertyResourceBundle::init()");
      this->ugtSegId = NULL; 
      size_t pos;
      string subdir = "";
      string base = basename;
      if((pos = basename.find_last_of("/")) != string::npos) {
         subdir = basename.substr(0,pos);
         base = basename.substr(pos+1, basename.size()-pos-1);
      }
      string pattern = base + "_" + locale;
      DIR *dir;
      struct dirent entry;
      struct dirent *result;
      struct stat st;
      string path = this->config->getDir();
      path = path.substr(0, path.find_last_not_of("/")+1) + "/" + subdir;
      string pathUgt = this->config->getDir() + "ugt/" + subdir;
      pathUgt = pathUgt.substr(0, pathUgt.find_last_not_of("/")+1) + "/";
      if ((dir = opendir(path.c_str())) != NULL) {
        result = NULL;
	path = path.substr(0, path.find_last_not_of("/")+1) + "/";
	do {
            if (readdir_r(dir, &entry, &result) == 0) {
                if (result != NULL) {
                    if((pos = string(entry.d_name).find_last_of(".")) != string::npos && string(entry.d_name).substr(pos+1).compare(YRB_EXTENSION) == 0) {
                        string filename = string(entry.d_name).substr(0,pos);
                        if(filename.size() > base.size() && filename.compare(pattern.substr(0,filename.size())) == 0) {
                           fileList.push_back(path + string(entry.d_name));
                        }
                    }
                }
            }
        }while(result != NULL);
        closedir(dir);
        if(fileList.size() != 0)
           fileList.sort(compare_nocase);
        if(stat((path + base+"."+YRB_EXTENSION).c_str(), &st) == 0)
           fileList.push_back(path + base+"."+YRB_EXTENSION);
      }
      if(fileList.size() == 0)
          throw MissingPropertyResourceBundleException("Missing resource file(s)");

      // UGT init
      if(stat((path + "/" +base+"-segidmap."+YRB_EXTENSION).c_str(), &st) == 0)
	   ugtSegId = new ResourceFile(path + "/" +base+"-segidmap."+YRB_EXTENSION);
      if(stat((this->config->getDir() + "/" + YRB_UGT_TEMPLATE_FILE).c_str(), &st) == 0) {
	   ResourceFile rf(this->config->getDir() + "/" + YRB_UGT_TEMPLATE_FILE);
	   this->ugtTemplate = rf.getValue("YRB_UGT_TEMPLATE", 0);
      }	    
      if(this->ugtTemplate.empty())
           this->ugtTemplate = YRB_DEFAULT_UGT_TEMPLATE;

      // set actualLocale
      size_t s_pos, e_pos;
      s_pos = fileList.begin()->find_first_of("_");
      e_pos = fileList.begin()->find_last_of(".");
      if(s_pos != string::npos && e_pos != string::npos)
         this->actualLocale = fileList.begin()->substr(s_pos+1, e_pos-s_pos-1);
      else
         this->actualLocale = string(YRB_ROOTNAME);
      for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
         try {
            ResourceFile *rf = new ResourceFile(*it);
            mrf[(*it).c_str()] = rf;
         } catch (MissingResourceFileException e) {
            throw MissingPropertyResourceBundleException(e.what());
         }
      }

      if(this->config->getCacheTTL() != 0) {
	 ResourceBundleCache configFiles(string(YRB_CACHE_DIR) + YRB_CONFIG_FILES, APR_WRITE|APR_CREATE, YRB_REG_PAGE);
           if(!configFiles.getDb()) {
              YRB_LOG(YRB_ERROR, "ResourceBundleCache fail: no configFiles DB");
              throw MissingPropertyResourceBundleException();
           } else {
              string cf = this->config->getConfigFile();
	      if(cf.at(0) != '/') {
                 char buf[1024];
                 getcwd(buf, 1023);
                 cf = string(buf) + "/" + cf;
              }
	      if((configFiles.getValue(cf)).size() == 0) {
                 configFiles.store(cf, YRB_CACHE_DIR + this->getCachePath() + "." + YRB_CONFIG_IDS, APR_SDBM_REPLACE);
	      }
           }
      }
      YRB_LOG(YRB_DEBUG, "End PropertyResourceBundle::init()");
   }

   string PropertyResourceBundle::getCachePath()
   {
      if(this->cachePath.size() > 0)
         return this->cachePath;
      cachePath = this->config->getDir();
      size_t i;
      while((i = cachePath.find_first_of("/", 0)) != string::npos) {
          cachePath.replace(i,1,".");
      }
      return cachePath;
   }


   const string PropertyResourceBundle::getString(const std::string &key)
   {
      for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
	 string value = (mrf[(*it).c_str()])->getValue(key, this->config->getCacheTTL());
	 if(value.size() > 0) return value;
      }
      throw MissingResourceIDException("Missing key " + key +".");	
      return "";
   }   

   const string PropertyResourceBundle::getString(const std::string &key, std::string *langTag)
   {
      for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
         string value = (mrf[(*it).c_str()])->getValue(key, this->config->getCacheTTL());
         if(value.size() > 0) {
	    size_t ps, pe;
	    if((ps = it->find_last_of("_")) != string::npos && (pe = it->find_last_of(".")) != string::npos && pe > ps) {
	       *langTag = it->substr(ps+1, pe-ps-1);
	    }else {
	       *langTag = "";
	    }
            return value;
	 }
      }
      throw MissingResourceIDException("Missing key " + key +".");
      return "";
   }

   const list<string> PropertyResourceBundle::getKeys()
   {
      for(list<string>::iterator it=fileList.begin(); it!=fileList.end(); it++) {
         list<string> k  = (mrf[(*it).c_str()])->getKeys(this->config->getCacheTTL());
	 this->keys.merge(k);
      }
      this->keys.sort();
      this->keys.unique();
      return this->keys;
   }
  
   const string PropertyResourceBundle::getRequestedLocale()
   {
      return this->locale;
   }

   const string PropertyResourceBundle::getActualLocale()
   {
      return this->actualLocale;
   } 

   const string PropertyResourceBundle::getBasename()
   {
      return this->basename;
   }

   const map<string,string> PropertyResourceBundle::getResources()
   {
      map<string, string> stringTable;
      this->getKeys();
      for(list<string>::iterator it=keys.begin(); it!= keys.end(); it++) {
	 stringTable[(*it).c_str()] = (getString(*it)).c_str();
      }
      return stringTable;
   }

   const string PropertyResourceBundle::getUgtTemplate()
   {
      return this->ugtTemplate;
   }
}
