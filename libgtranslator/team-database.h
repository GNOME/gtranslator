/*
 * (C) 2000-2001 	Fatih Demir <kabalak@gmx.net>
 *
 * libgtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * libgtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef LIBGTRANSLATOR_TEAM_DATABASE_H
#define LIBGTRANSLATOR_TEAM_DATABASE_H 1

/*
 * Team database/handling parts of libgtranslator.
 */

#include "preferences.h"

/*
 * A single application translator is stored in this
 *  GtranslatorTeamAppTranslator structure.
 */
typedef struct {
	gchar	*name;
	gchar	*email;
} GtranslatorTeamAppTranslator;

/*
 * This structure holds the current informations about a
 *  translated application.
 */
typedef struct {
	gchar	*appname;
	gchar	*appversion;
	/*
	 * The "GtranslatorTeamAppTranslators" list.
	 *
	 * Note: The first index should always be the
	 *  current translator of the application.
	 *
	 */
	GList	*translators;
	gboolean full_translation;
} GtranslatorTeamApp;

/*
 * The GtranslatorTeam structure which describes a translation
 *  team.
 */
typedef struct {
	/*
	 * The list of applications for the translation team.
	 */
	GList	*apps;
	/*
	 * The name of the translation team (e.g. "tr").
	 */
	gchar	*name;
	/*
	 * The translation team's mailing list.
	 */
	gchar	*mailing_list;
} GtranslatorTeam;

/*
 * The GtranslatorTeamDatabase main structure.
 */
typedef struct {
	/*
	 * The translation teams' list (consisting of GtranslatorTeam
	 *  pointers).
	 */
	GList	*teams;
	/*
	 * The team database file's date.
	 */
	gchar	*date;
	/*
	 * The team database serial.
	 */
	gint	serial;
} GtranslatorTeamDatabase;

#endif
