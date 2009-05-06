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

#include "yrb/ResourceFile.h"

using namespace std;

namespace yrb {

static size_t rbUEscapeToUTF8(char **text, char *target, size_t len)
{
    static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    unsigned short bytesToWrite = 0;
    const UTF32 byteMask = 0xBF;
    const UTF32 byteMark = 0x80;
    UTF32 ch = 0;

    while(len != 0 && **text != '\0') {
        if (!isxdigit(**text)) {
            ch = RB_UNI_REPLACEMENT_CHAR;
            break;
        }
        ch = (ch << 4) + toupper(**text) - '0' - ((**text > '9')? 7 : 0);
        --len;
        ++*text;
    }
    if (ch >= RB_UNI_SUR_HIGH_START && ch <= RB_UNI_SUR_LOW_END) {
        ch = RB_UNI_REPLACEMENT_CHAR;
    }
    if (ch < (UTF32)0x80) {
        bytesToWrite = 1;
    } else if (ch < (UTF32)0x800) {
        bytesToWrite = 2;
    } else if (ch < (UTF32)0x10000) {
        bytesToWrite = 3;
    } else if (ch <= RB_UNI_MAX_LEGAL_UTF32) {
        bytesToWrite = 4;
    } else {
        bytesToWrite = 3;
        ch = RB_UNI_REPLACEMENT_CHAR;
    }
    target += bytesToWrite;
    *target = '\0';
    switch(bytesToWrite) { /* note: everything falls through. */
        case 4:
            *--target = (UTF8)((ch | byteMark) & byteMask);
            ch >>= 6;
        case 3:
            *--target = (UTF8)((ch | byteMark) & byteMask);
            ch >>= 6;
        case 2:
            *--target = (UTF8)((ch | byteMark) & byteMask);
            ch >>= 6;
        case 1:
            *--target = (UTF8)(ch | firstByteMark[bytesToWrite]);
    }
    return bytesToWrite;
}

static char *rbUnEscape(char *value)
{
    char buffer[10];
    size_t len;
    char *src;
    char *dst;

    if (value == NULL) return NULL;
    dst = src = value;
    while(*src != '\0') {
        if (*src == '\\') {
            buffer[0] = '\0';
            buffer[1] = '\0';
            ++src;
            len = 1;
            switch(*src) {
                case '\\':
                case ' ':
                case '#':
                    *buffer = *src;
                    src++;
                    break;
                case 'n':
                    strcpy(buffer, RB_NEWLINE);
                    len = strlen(RB_NEWLINE);
                    src++;
                    break;
                case 't':
                    *buffer = '\t';
                    src++;
                    break;
                case 'u':
                    src++;
                    len = rbUEscapeToUTF8(&src, buffer, 4);
                    break;
                case 'U':
                    src++;
                    len = rbUEscapeToUTF8(&src, buffer, 8);
                    break;
                default:
                    len = 0;
                    break;
            }
            memcpy(dst, buffer, len);
            dst += len;
        } else {
            *dst = *src;
            ++src;
            ++dst;
        }
    }
    *dst = *src;
    dst = src = value;
    return value;
}

   ResourceFile::ResourceFile(const std::string &filename)
   {
      struct stat st;
      if(stat(filename.c_str(), &st) != 0 || st.st_mode & S_IFDIR) {
	 YRB_LOG(YRB_ERROR, (filename + ": No such file").c_str());
	 throw MissingResourceFileException(filename + ": No such file");
      }
      this->filename = filename;
      this->max_kv = 0;
      this->kv_cache = NULL;
   }
 

   void ResourceFile::readFile()
   {
      FILE *fh;
      char line[RB_LINE_MAX];
      char value[RB_LINE_MAX];
      char key[RB_LINE_MAX];
      short firstLine = 1;
      char *bLine;
      char *bTag;
      char *pEqual;
      size_t lineno = 0;
      size_t eLine;
      size_t cLine;
      size_t curls;
      size_t eKey;
      short hereDoc;
      char hereDocTag[RB_LINE_MAX];

      if ((fh = fopen(this->filename.c_str(), "r")) != NULL) {
          *value = '\0';
          hereDoc = 0;
          while(fgets(line, RB_LINE_MAX, fh) != NULL) {
             lineno++;
             bLine = line;
             if (firstLine) {
                firstLine = 0;
                if (strncmp(RB_BOM, line, 3) == 0) {
                    bLine += 3;
                }
             }
             while(!hereDoc && *bLine != '\0' && strchr(RB_WHITESPACE_STR,
                    *bLine) != NULL) {
                ++bLine;
             }
             if (*bLine == '#') continue;
             if ((eLine = strlen(bLine)) != 0) {
                while(eLine != 0) {
                    --eLine;
                    if (strchr(RB_WHITESPACE_STR, bLine[eLine]) == NULL) {
                        cLine = eLine;
                        while(cLine > 0 && bLine[cLine] == '\\')
                            --cLine;
                        curls = eLine - cLine;
                        if (curls % 2 != 0) {
                            if (eLine >= strlen(bLine) - 1) {
                                if (bLine[eLine + 1] == ' ') eLine++;
                            } else {
                                bLine[eLine + 1] = ' ';
                                ++eLine;
                            }
                        }
                        break;
                    } else {
                        if (eLine == 0) *bLine = '\0';
                    }
                }
                bLine[eLine + 1] = '\0';
             } else continue;
             if (hereDoc) {
                if (strcmp(line, hereDocTag) == 0) {
		    this->tm[key] = rbUnEscape(value);
		    if((int)(strlen(key) + strlen(value)) > max_kv)
		       max_kv = strlen(key) + strlen(value);	
		    this->keys.push_back(string(key));
                    hereDoc = 0;
                    *value = '\0';
                } else {
//std::cout<<"++++"<<string(line)<<endl;
                    if (strlen(value) + strlen(line) + strlen(RB_NEWLINE)
                            < RB_LINE_MAX) {
                        if (*value != '\0') strcat(value, RB_NEWLINE);
                        strcat(value, line);
                    } else {
			// log
                    }
                }
             } else {
                if ((pEqual = strchr(bLine, '=')) == NULL) {
//                    rbLog(RBERROR, "%s: expected \"=\" at line %ld",
//                            fileName, (long)lineno);
                } else {
                    *pEqual = '\0';
                    if ((eKey = strlen(bLine)) != 0) {
                        --eKey;
                        while(eKey != 0 && strchr(" \t", bLine[eKey]) != NULL)
                            --eKey;
                        bLine[eKey + 1] = '\0';
                    }
                    if (*bLine != '\0') {
                        strcpy(key, bLine);
                        ++pEqual;
                        while(*pEqual != '\0' && strchr(" \t", *pEqual))
                            ++pEqual;
                        if (strncmp(pEqual, "<<<", 3) == 0) {
                            pEqual += 3;
                            while(*pEqual != '\0' && *pEqual == ' ')
                                ++pEqual;
                            bTag = pEqual;
                            while(*bTag != '\0' && strchr(RB_WHITESPACE_STR,
                                    *bTag) == NULL) {
                                ++bTag;
                            }
                            *bTag = '\0';
                            if (*pEqual != '\0') {
                                strcpy(hereDocTag, pEqual);
                                hereDoc = 1;
                            } else {
				char buf[1024];
				sprintf(buf, "%s: invalid null heredoc tag at line %ld",this->filename.c_str(), (long)lineno);
				YRB_LOG(YRB_ERROR, buf);
                            }
                        } else {
			    this->tm[key] = rbUnEscape(pEqual);
			    if((int)(strlen(key) + strlen(pEqual)) > max_kv)
                               max_kv = strlen(key) + strlen(pEqual);
			    this->keys.push_back(string(key));
                        }
                     } else {
			char buf[1024];
                        sprintf(buf, "%s: null keyword is not allowed at line %ld", this->filename.c_str(), (long)lineno);
                        YRB_LOG(YRB_ERROR, buf);

//                        rbLog(
//                                RBERROR,
//                                "%s: null keyword is not allowed at line %ld",
//                                fileName, (long)lineno);
                     }
                  }
              }
           }
           fclose(fh);
       }
   }

   HashMap ResourceFile::getHash()
   {
      if(this->kv_hash.size() > 0)
	 return this->kv_hash;
      this->readFile();
      for(map<string,string>::iterator it=tm.begin(); it!=tm.end(); it++) {
	    kv_hash[it->first] =  it->second;
      }

      return this->kv_hash;  
   }

   ResourceBundleCache *ResourceFile::getCache()
   {
      if(this->kv_cache)
	 return this->kv_cache;
      string cf = this->filename + ".mdb";
      size_t i;
      while((i = cf.find_first_of("/", 0)) != string::npos) {
          cf.replace(i,1,".");
      }
      struct stat st;
      if(stat((string(YRB_CACHE_DIR)+"/"+cf).c_str(), &st) != 0)
	 this->readFile();
      kv_cache = new ResourceBundleCache(string(YRB_CACHE_DIR)+"/"+cf, APR_WRITE|APR_CREATE, (max_kv>YRB_REG_PAGE)?YRB_BIG_PAGE:YRB_REG_PAGE);
      if(max_kv>YRB_REG_PAGE) {
	 ostringstream oss;
	 oss<<cf<<": Large record ("<<max_kv<<") found"<<endl;
	 YRB_LOG(YRB_ERROR, oss.str().c_str())
      }
	   
      if(kv_cache && !kv_cache->getDb()) {
	 delete kv_cache;
	 throw MissingResourceCacheException();
      } else {
	 apr_sdbm_datum_t key;
         apr_sdbm_firstkey((apr_sdbm_t *)kv_cache->getDb(), &key);
	 if(key.dsize == 0) {
	    if(max_kv == 0)
	       this->readFile();
            for(map<string,string>::iterator it=tm.begin(); it!=tm.end(); it++) {
               kv_cache->store((*it).first, (*it).second, APR_SDBM_REPLACE);
            }
	 }
      } 
      return this->kv_cache;
   }

   string ResourceFile::getValue(const std::string &key, const int &cache)
   {
      if(!cache) {
         if(this->kv_hash.size() == 0)
	    getHash();

	 if(!this->kv_hash[key].empty())
	    return this->kv_hash[key];  
	 return "";
      }else {
         getCache();
	 return kv_cache->getValue(key);
      }
   }

   list<string> ResourceFile::getKeys(const bool &cache)
   {
      if(!cache) {
         if(this->kv_hash.size() == 0)
            this->readFile();
	 return this->keys;
      }else {
         getCache();
         return kv_cache->getKeys();
      }
   }
}
