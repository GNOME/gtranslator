/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#ifndef PARSE_DB_H
#define PARSE_DB_H 1

#include <glib.h>
#include <string.h>

/**
* The libxml includes.
**/
#include <tree.h>
#include <parser.h>
#include <xmlmemory.h>

/**
* The messages structure headers.
**/
#include <messages.h>

/**
* A try to avoid some libxml chaos.
**/
#ifndef xmlRootNode
#define xmlRootNode root
#define xmlChildrenNode childs
#endif

/* Maybe we have already included this stuff */
#ifndef __GNOME_I18N_H__

/**
* If NLS is requested include the headers and set the define and if it's
*  not requested, simply make a foo-define.
*/
#ifdef ENABLE_NLS
#    include <libintl.h>
#    define _(String) gettext(String)
#    ifdef gettext_noop
#        define N_(String) gettext_noop (String)
#    else
#        define N_(String) (String)
#    endif
#else
#    define _(String) String
#    define N_(String) (String)
#endif

#endif

/**
* The message db list.
**/
GList *db_list;

/**
* The doc-pointer.
**/
static xmlDocPtr xmldoc;

/**
* Loads the message db base for the specified language.
**/
void parse_db_for_lang(gchar *language_code);

/**
* Checks the type of the message db file.
**/
void parse_db_check(xmlDocPtr testit);

#endif
