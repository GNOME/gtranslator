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

#ifndef TEAM_HANDLE_H
#define TEAM_HANDLE_H 1

#include "parse-db.h"
#include "team-database.h"

/**
* These functions are for the team-handling of gtranslatord.
**/

/**
* This opens a given different team database file or
*  the default team database.
**/
GtranslatorTeamDatabase *gtranslator_open_team_db(gchar *team_db_file);

#endif
