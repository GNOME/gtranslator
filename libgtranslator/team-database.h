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

#ifndef TEAM_DATABASE_H
#define TEAM_DATABASE_H 1

#include <libgtranslator/preferences.h>

/**
* This defines the GtranslatorTeamDatabase structure and
*  all other structures in use within the team database
*   feature.
**/

/**
* The translator's structure.
**/
typedef struct {
	/**
	* The name.
	**/
	gchar	*name;
	/**
	* The email.
	**/
	gchar	*email;
} GtranslatorTeamAppTranslator;

/**
* The application structure.
**/
typedef struct {
	/**
	* The application name.
	**/
	gchar	*appname;
	/**
	* The application version.
	**/
	gchar	*appversion;
	/**
	* The GtranslatorTeamAppTranslators' list.
	*
	* Note: The first index should always be the
	*	 current translator of the app.
	*
	**/
	GList	*translators;
	/**
	* Is this application fully translated?
	**/
	gboolean full_translation;
} GtranslatorTeamApp;

/**
* The team structure.
**/
typedef struct {
	/**
	* The GtranslatorTeamApps' list.
	**/
	GList	*apps;
	/**
	* The team name (e.g. "tr").
	**/
	gchar	*name;
	/**
	* Teams' mailing list.
	**/
	gchar	*mailing_list;
} GtranslatorTeam;

/**
* The GtranslatorTeamDatabase structure itself.
**/
typedef struct {
	/**
	* The list of GtranslatorTeams.
	**/
	GList	*teams;
	/**
	* The team database date.
	**/
	gchar	*date;
	/**
	* The team database serial.
	**/
	gint	serial;
} GtranslatorTeamDatabase;

#endif
