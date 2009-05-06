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
/** @file ResourceBundle.h File representing a Resource Bundle */

#ifndef _RESOURCEBUNDLE_H_
#define _RESOURCEBUNDLE_H_

//#include "yrb/ResourceBundle.h"
#ifdef __cplusplus

#include <list>
#include <string>
#include <map>

#include "Exception.h"
#include "yrbLog.h"
#include "ResourceConfig.h"
#include "ResourceFile.h"
#include "ResourceBundleCache.h"

/** @namespace yrb
 * Namespace yrb contains all the classes related with the Resource Bundle yrb
 */
namespace yrb {

#define CONFIG_PATH_SEPARATOR "/"
#define YRB_EXTENSION "pres"
#define YRB_ROOTNAME "root"
#define YRB_UGT_TEMPLATE_FILE "/ugt/template.pres"
#define YRB_DEFAULT_UGT_TEMPLATE "<!-- yrb:start yrb:sid={0} yrb:source={1} -->{2}<!-- yrb:end -->"
// getString flags
#define YRB_INSTRUMENT_FOR_UGT  0x0001
#define YRB_NO_FALL_BACK 	0x0002
#define YRB_WITH_LANGUAGE_TAG   0x0004

/**
 * PropertyResourceBundle class. ResourceBundle specific to streams 	  
 * ref.: http://twiki.corp.yahoo.com/view/LionTeam/L10NResourceBundlesSpec
 */

class PropertyResourceBundle
{
    public:
       /**
        * Constructor
        */
     	PropertyResourceBundle();

	 /** 	  
          * PropertyResourceBundle 	         
          *   Constructor that opens and loads  a resource bundle 	        
          * 	          *
          * @param basename it is a unique identifier of the resource bundle  
          *    (Ex: news/ui, sports/ui, ...) 	         
          * @param locale it is the locale identifier ("en-US", "fr-FR", 	         
          *    "es-MX", ...). Language priority list 	 
          *    is also supported ("pt-BR, en-US", "en, it-IT, es"). 	 
          *    This parameter should comply with RFC4647 Basic language range. 	 
          * @param configFile If yrb configuration will need to read from a file different from 	 
          *    default yrb.conf. 	 
          * @return returns the resource  bundle if available or the more general 	 
          *    one if the exact one is absent 	          *    one if the exact one is absent
          * @exception MissingResourceException if no resource bundle for the 	     
          *    specified base name can be found 	        
          */
        PropertyResourceBundle(const std::string &basename, const std::string &locale, const std::string& configFile);
	PropertyResourceBundle(const std::string &basename, const std::string &locale);
	/** 
	 * Destructor
	 */
	~PropertyResourceBundle();

	/**
       * getString
       *    Method that retrieves string associated with a resource id.
       *
       * @param  key is the unique identifier of the resource.
       * @return returns the resource STL string in UTF-8 encoding if the
       *  resource exists.
       * @exception MissingResourceIDException  if no resource string for the
       *  specified resource id can be found
       */
       const  std::string getString(const std::string &key);
       const  std::string getString(const std::string &key, const int flags);
       const  std::string getString(const std::string &key, std::string *langTag);

       /**
        * getKeys
        *    Method that retrieves all the keys for given dimensions
        *
        * @return List of key names (Resource id's) for a configuration bundle
        */
        const std::list<string> getKeys();

       /**
        * getActualLocale 	 
        *  Gets actual resource bundle locale loaded 	 
        * 	 
        * @return Actual locale of loaded Resource bundle 	 
        */ 	 
	const std::string getActualLocale(); 	 
	  	 
	const std::map<std::string, std::string> getResources();	
	const std::string getRequestedLocale();
	const std::string getBasename();
	const std::string getUgtTemplate();		

    private:
	void init();
	std::string getCachePath();
	std::map<std::string, std::string> tokenize(const std::string &str);
	std::list<std::string> getPartialMatchKeys(const std::string &key); 


	std::string path;
	std::string basename;
	std::string locale;
	std::string actualLocale;
	std::string cachePath;
 	std::list<std::string> fileList;	
	ResourceConfig *config;
        std::map<std::string, ResourceFile *> mrf;
        std::list<std::string> keys;
	ResourceFile *ugtSegId;
	std::string ugtTemplate;
};
}
#endif

#endif // ifndef _RESOURCEBUNDLE_H_

