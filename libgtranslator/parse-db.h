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
**/

#ifndef LIBGTRANSLATOR_PARSE_DB_H
#define LIBGTRANSLATOR_PARSE_DB_H 1

#include <glib.h>
#include <string.h>

/**
* The libxml includes.
**/
#include <tree.h>
#include <parser.h>
#include <xmlmemory.h>

/**
* Include the database header.
**/
#include "translation-database.h"

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
#    define textdomain(String) (String)
#    define gettext(String) (String)
#    define dgettext(Domain,Message) (Message)
#    define dcgettext(Domain,Message,Type) (Message)
#    define bindtextdomain(Domain,Directory) (Domain)
#    define _(String) (String)
#    define N_(String) (String)
#endif

#endif

/**
* Loads the message database for the specified language
*  and returns the loaded database.
**/
GtranslatorDatabase * parse_db_for_lang(gchar *language_code);

#endif
