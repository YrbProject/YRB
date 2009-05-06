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
/** @file ResourceBundle.h  file representing an abstract Resource Bundle. */

#ifndef _RESOURCEBUNDLE_H_
#define _RESOURCEBUNDLE_H_

#ifdef __cplusplus

#include <list>

//#include <yrb/rblib.h>
//#include <yrb/Config.h>
#include <yrb/Exception.h>

/*! \mainpage Resource Bundle yrb - C++ and C API Documentation
 * \section intro_sec Introduction
 *
 * One of the great commandments of internationalization is to separate text, labels, messages, and other locale-sensitive objects from the core source code. This helps to maintain a single source code base for all language versions of your product. It also facilitates translations because all localizable resources are identified and isolated. A widely adopted way (Java/Windows world) to achieve this separation between code and locale-sensitive is using resource bundle.
 *
 * Yahoo is investing on a centralized localization services as explained by YUser:brunofr on <a href="http://twiki.corp.yahoo.com/view/Intlservarch/TS6PublishingArchitecturesTranslationBlueprint">L10N Services blueprint</a> and one of the approaches to integrate applications with this Service is using resource bundle. Maple is already going towards this model as described on <a href="http://twiki.corp.yahoo.com/view/Media/PresentationFrameworkLocalization">Maple Localization</a>.
 *
 * This is a common framework of resource bundle for the yahoo main languages (C++/PHP) and Presentation Frameworks (Maple/Yahoo Application Platform) .
 *
 * \section Reference_Usage API Reference Usage
 *
 * This is the reference usage.
 *
 * \subsection Cpp C++ Programmers:
 *
 * Use <A href="hierarchy.html">Class Hierarchy</A> or <A href="annotated.html">Compound List</A> to find the class you are interested in. For example, to find <A href="classyrb_1_1Config.html">Config</A>, you can go to the <A href="hierarchy.html">Class Hierarchy</A>, then click on "Config"". Once you are at the class, you will find an inheritance chart, a list of the public members, a detailed description of the class, then detailed member descriptions.
 *
 * \subsection C C Programmers:
 *
 * Use <A HREF="files.html">File Members</A> to find a list of all the functions and constants. For example, to find <A href="Config_8h.html">Config</A> functions you would click on <A HREF="files.html">File Members</A>, then find "Config.h" and click on it. You will find descriptions of Defines, Typedefs, Enumerations, and Functions, with detailed descriptions below. If you want to find a specific function, such as ConfigOpen(), then click first on <A HREF="files.html">File Members</A>, then use your browser Find dialog to search for "ConfigOpen()".
 *
 *
 */

namespace yrb {

    /**
     * ResourceBundle class is an abstract class representing a Resource Bundle.
     * Encapsulate the logic of loading and reading resource bundles as
     * described at: http://twiki.corp.yahoo.com/view/LionTeam/L10NResourceBundlesSpec.
     */
    class ResourceBundle
    {
        public:

        /**
         * Constructor
         */
        ResourceBundle();

        /**
         * Constructor
         */
        ResourceBundle(const std::string basename, const std::string locale);

        /**
         * Destructor
         */
        virtual ~ResourceBundle();

        /**
         * getRequestedLocale
         *    Get Resource bundle requested locale
         *
         * @return resource bundle requested locale
         */
        const std::string getRequestedLocale() const;

        /**
         * getActualLocale
         *    Resource bundle actual locale
         *
         * @return resource bundle actual locale
         */
        virtual const std::string getActualLocale() const = 0;

        /**
         * getBasename
         *    Resource bundle basename
         *
         * @return resource bundle basename
         */
        const std::string getBasename() const;

        // Methods to be implemented by subclasses

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
        virtual const std::string getString(const std::string &key) = 0;

        /**
         * getKeys
         *    This method gets the keynames from the resource bundle hierarchy till
         *     root resource bundle to get all the unique keynames.
         *
         * @return returns a list of key names for a resource bundle
         */
        virtual const std::list<std::string> getKeys() = 0;

        protected:

        std::string basename;
        std::string locale;

    }; // end of class

} // end of namespace

#endif

#endif // ifndef ResourceBundle.H
