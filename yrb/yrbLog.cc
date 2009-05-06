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
#include "yrb/yrbLog.h"
#include <iostream>

using namespace std;

namespace yrb {

   yrbLog *yrbLog::_inst = NULL;
   
   yrbLog::yrbLog(yrbLogLevel level, const string& file) 
   {
       logFile = file;
       logLevel = level;
   }

   yrbLog::~yrbLog() 
   {
   }
   
   yrbLog *yrbLog::getInstance(yrbLogLevel level, const string &logFile)
   {
       if (_inst == NULL) { // create a log instance
          _inst = new yrbLog(level, logFile);
       }
       return _inst;
   }

   yrbLog *yrbLog::getInstance()
   {
        if (_inst == NULL) {
           _inst = new yrbLog(YRB_NOLOG, YRB_LOG_FILE);
        }
        return _inst;
   }

   void yrbLog::setLogLevel(yrb::yrbLogLevel level)
   {
        logLevel = level;
   }

    // Get the current time
   char * yrbLog::getTimeStamp()
   {
      const struct tm *tm;
      size_t len;
      time_t now;
      char *s = (char *)malloc(sizeof(char)*64);

      now = time ( NULL );
      tm = localtime ( &now );
      len = strftime ( s, 64, "%d %B %Y %I:%M:%S %p", tm );
      return (char *)s;
   }

   void yrbLog::writeLog(yrb::yrbLogLevel level, const char *file, int line, ...)
   {
      va_list args;
      va_start(args, line);

      char* msg = va_arg(args, char *);
      char fullmsg[1024];

      vsprintf(fullmsg, msg, args);
      // open file
      //write to file
      if (isPrintable(level)) { // write to file only if this log level
                               // is same as requested in yrb.conf file
          fstream fs(logFile.c_str(), fstream::out | fstream::app);
          char *ts = getTimeStamp();
          if (!fs.fail()) {
             fs << ts << " :: " << file  << " :: " << line << " :: " << fullmsg << std::endl;
             free(ts);
             // close file
             fs.close();
          }
      }
   }

   bool yrbLog::isPrintable(yrb::yrbLogLevel level)
   {
       // Print log for both YRB_DEBUG and YRB_ERROR
       // YRB_ERROR is special case as it needs to be printed for YRB_DEBUG
       // level also
       if ((level == YRB_DEBUG) && (level == logLevel)) { 
           return true;
       } else if ((level == YRB_ERROR) && ((level == logLevel) || (logLevel == YRB_DEBUG)))
           return true;
       else
           return false;
   }
}
