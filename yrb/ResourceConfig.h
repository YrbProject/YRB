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
#ifndef _RESOURCECONFIG_H_
#define _RESOURCECONFIG_H_

#ifdef __cplusplus

#include <list>
#include <string>
#include <map>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "Exception.h"
#include "yrbLog.h"

#define YRB_CONFIG_DIR "/home/y/conf/yrb/"
#define YRB_CONFIG_FILE_NAME "yrb.conf"
#define CONFIG_DEFAULT_DIR "/home/y/var/yrb/"
#define CONFIG_DEFAULT_TTL 300
#define CONFIG_DEFAULT_LOG false
#define CONFIG_APACHE_YRB_CFG "YRB_DEFAULT_CFG"

namespace yrb {

/**
 * Resource Bundle Config class.
 *   Encapsulate the configuration settings of yrb.
 *   The default configuration values will be read from file /home/y/conf/yrb/yrb.conf
 */
class ResourceConfig
{
    public:
	/**
        * Constructor
        */
     	ResourceConfig();

	/**
          * ResourceConfig
          *   Constructor that opens and loads a configuration file 
	  *
	  * @param configFile If yrb configuration will need to read from a file different from
          *    default yrb.conf.
	  */	
	ResourceConfig(const std::string &configFile);

	/**
         * Destructor
         */
        ~ResourceConfig();

	/**
       * getCacheTTL
       *   Numbers of seconds representing the max time that a resource bundle
       *   can be cached.
       *   Setting as 0 means that resource bundle cache won't be used
       *   (good for development env. ).
       *   Setting as -1 means it will never expire.
       *
       * @return cacheTTL
       */
	const int getCacheTTL();

      /**
       * getDir
       *   Gets the directory path where the runtime library will search for
       *   resource bundles.
       *
       * @return resource dir.
       */
	const std::string getDir();
	
      /**
       * getAllLanguageTags
       *   Gets all the language tags under given config directory.
       *
       * @return list of language tags.
       */
	const std::list<std::string> getAllLanguageTags();

	/**
       * getAllBasenames
       *   Gets all the basenames under given config directory.
       *
       * @return list of basenames.
       */
        const std::list<std::string> getAllBasenames();

      /**
       * getNumFiles
       *   Gets number of files under given config directory.
       *
       * @return number of files.
       */
	const int getNumFiles();
	
	const std::string getURI();
	const std::string getConfigFile();
	const yrbLogLevel getLogging();
	const time_t getMtime();

    private:
	std::string configFile;
	std::string dir;
	std::string defaultURI; 
 	int ttl;
	yrbLogLevel logging;
	std::map<std::string,std::string> values;
	int numFiles;
	time_t mtime;
	bool enableCache;
	std::list<std::string> langTags;
        std::list<std::string> basenames;

	int setCacheTTL(const int &ttl);
	int setDir(const std::string &dir);
	int setConfigFile(const std::string &configFile);
	int setLogging(const yrbLogLevel &logging);
 	int getDirInfo(const std::string &dir, int *nf, time_t *mt);
	const void readConfigFile();
};
}
#endif

#endif // ifndef _MCONFIG_H_

