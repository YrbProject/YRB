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
#ifndef RESOURCESTORE_H_
#define RESOURCESTORE_H_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>

#include "ResourceBundleCache.h"

#if __GNUC__ == 3
#include <ext/hash_map>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
#else
#include <hash_map>
using std::hash_map;
using std::hash;
#endif // __GNUC__


/**
 * namespace yrb 
 */
namespace yrb {

struct str_hash{
        size_t operator()(const string& str) const
        {
		unsigned long __h = 0;
                for (size_t i = 0 ; i < str.size() ; i ++)
                __h = 5*__h + str[i];
                return size_t(__h);
        }
};

struct eqstr{
        bool operator()(const string& p1, const string& p2) const{
                return p1 == p2;
        }
};

typedef hash_map <string, string, str_hash, eqstr> HashMap;

typedef unsigned long  UTF32; // at least 32 bits
typedef unsigned short UTF16; // at least 16 bits
typedef unsigned char  UTF8;  // typically 8 bits

#define RB_LIST_ALLOW_DUP 0x0001
#define RB_LIST_NOWARNING 0x0002

#define RB_WHITESPACE_STR " \t\f\v\n\r"

#define RB_BOM "\xef\xbb\xbf"

#define RB_UNI_SUR_HIGH_START  (UTF32)0xD800
#define RB_UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define RB_UNI_SUR_LOW_START   (UTF32)0xDC00
#define RB_UNI_SUR_LOW_END     (UTF32)0xDFFF

#define RB_UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define RB_UNI_MAX_BMP (UTF32)0x0000FFFF
#define RB_UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define RB_UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define RB_UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

#define RB_PATH_SEP '/'
#define RB_NEWLINE  "\n"
#define RB_LINE_MAX 32*1024+1


typedef std::map<std::string, std::string> KeyValueMap;
typedef std::list<std::string> StringList;
typedef std::vector<std::string> StringVector;

/**
 * class ResourceStore
 *   Class that reads files from a given directory and manages 
 *   the generates the configuration key/value pairs
 */
class ResourceFile
{
    public:
        // Empty constructor
     	ResourceFile();
        // Constructor
	ResourceFile(const std::string &filename);

        // Destructor
	~ResourceFile() 
	{
	   if(this->kv_cache)
	      delete this->kv_cache;
	}

	std::string getFileName()
	{
	   return this->filename;
	}

        int getMaxKvSize()
	{
	   return this->max_kv;
	}

	std::string getValue(const std::string &key, const int &cache=true);
	HashMap getHash();
	ResourceBundleCache *getCache();
	std::list<std::string> getKeys(const bool &cache=true);

    private:
        // pointer to a map of key/value pairs
	std::string filename;
	HashMap kv_hash;
	ResourceBundleCache *kv_cache;
 	std::map<std::string, std::string> tm;
	std::list<std::string> keys;
	int max_kv;

	void readFile();
};
}
#endif /* RESOURCESTORE_H_ */
