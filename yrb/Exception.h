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
/** @file Exception.h*/

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>

/** @namespace yrb
 * Namespace yrb contains all the classes related with the Resource Bundle yrb
 */
namespace yrb {

/** class RBException:
 *  RBException is the super class of all exceptions thrown by
 *  PropertyResourceBundle api's.
 */
class RBException : public std::exception
{
   public:
      /**
       * Constructor 
       */
      RBException() {};

      /**
       * Constructor 
       *
       * @param errMsg
       */	
      RBException(const std::string& errMsg);

      ~RBException() throw() { };

      /**
       * what() - returns the error message as a string
       */
      virtual const char *what() const throw();

   private:
      std::string errMsg;
};
	

/** class MissingResourceException:
 *  class representing exception thrown when the requested configuration bundle 
 *  is not found in the system.
 */
class MissingResourceException : public RBException
{	
   public: 
      MissingResourceException() { };
      MissingResourceException(const std::string& errMsg)
         : RBException(errMsg) { };
};

/** class MissingResourceIDException:
 *  class representing exception thrown when the requested configuration key (or ID) 
 *  is not found in the system.
 */
class MissingResourceIDException : public RBException
{
   public:
      MissingResourceIDException() { };
      MissingResourceIDException(const std::string& errMsg)
         : RBException(errMsg) { };
};

/** class MissingDefaultDirException:
 *  class representing exception thrown when the DIRECTORY specified in yrb.conf
 *  where configuration bundles are located is not found in the system.
 */
class MissingDefaultDirException : public RBException
{
   public: 
      MissingDefaultDirException() { };
      MissingDefaultDirException(const std::string& errMsg)
         : RBException(errMsg) { };
};	

/** class MissingDimensionsFileException:
 *  class representing exception thrown when file describing dimensions is not
 *  found in the system.
 *  dimensions file should be dimensions.xml or dimensions.yml depending
 *  on file types used.
 */
class MissingDimensionsFileException : public RBException
{
    public:
      MissingDimensionsFileException() {};
      MissingDimensionsFileException(const std::string& errMsg)
         : RBException(errMsg) { };
};

/** class MissingResourceFileException:
 *  class representing exception thrown when no configuration files are 
 *  found in the system.
 */
class MissingResourceFileException : public RBException
{
    public:
       MissingResourceFileException() {};
       MissingResourceFileException(const std::string& errMsg)
         : RBException(errMsg) { };
};

class MissingPropertyResourceBundleException : public RBException
{
    public:
       MissingPropertyResourceBundleException() {};
       MissingPropertyResourceBundleException(const std::string& errMsg)
         : RBException(errMsg) { };
};


class MissingResourceCacheException : public RBException
{
    public:
       MissingResourceCacheException() {};
       MissingResourceCacheException(const std::string& errMsg)
         : RBException(errMsg) { };
};

class MissingResourceConfigException : public RBException
{
    public:
       MissingResourceConfigException() {};
       MissingResourceConfigException(const std::string& errMsg)
         : RBException(errMsg) { };
};

/** class ResourceFileParseException:
 *  class representing exception thrown when configuration files cannot
 *  be parsed due to invalid format.
 */
class ResourceFileParseException : public RBException
{
    public:
       ResourceFileParseException() {};
       ResourceFileParseException(const std::string& errMsg)
         : RBException(errMsg) { };
};

} // end of yrb namespace

#endif /* _EXCEPTION_H_ */
