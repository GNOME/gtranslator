/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
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

#ifndef LIBGTRANSLATOR_TEAM_HANDLE_H
#define LIBGTRANSLATOR_TEAM_HANDLE_H 1

/*
 * The team management functions of gtranslator(d).
 */ 

#include "parse-db.h"
#include "team-database.h"

/*
 * This initializes the default team database.
 */
GtranslatorTeamDatabase 	*gtranslator_init_team_db(void);

#endif
