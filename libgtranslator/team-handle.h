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

#ifndef TEAM_HANDLE_H
#define TEAM_HANDLE_H 1

#include <parse-db.h>

/**
* These functions are for the team-handling of gtranslatord.
**/

/**
* The list with the translated apps.
**/
GList *translations;


/**
* Inits the team's db if not done yet.
**/
void team_handle_new(gchar *team_code);

/**
* And this corresponding one deletes a registered team
*  ( will produce errors is the team isn't registered yet. )
**/
void team_handle_delete(gchar *team_code);

/**
* Looks if we've got a conflict with another translator.
**/
gboolean team_handle_lookup_app(gchar *appname);

/**
* Returns a list of all translation from a special author.
**/
GList *team_handle_lookup_author(gchar *author);

/**
* "Registers" the translation with author+appname.
**/
void team_handle_add_translation(gchar *appname, gchar *author);

/**
* Changes the author of a translation.
**/
void team_handle_set_author(gchar *appname, gchar *author);

/**
* Returns a list of all translated apps.
**/
GList *team_handle_get_all_translations_for_team(gchar *teamname);

/**
* Simply looks if the "req" is already in the doc.
**/
gint lookup_in_doc(xmlDocPtr doc,gchar *req);

#endif
