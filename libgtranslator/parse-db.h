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
* The doc-pointer.
**/
static xmlDocPtr xmldoc;

/**
* Loads the message db base for the specified language.
**/
void parse_db_for_lang(gchar *language_code);

/**
* Simply reloads the current message db.
**/
static void parse_db_reload();

#endif
