/*
 * (C) 2001 	Fatih Demir <kabalak@gmx.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_HISTORY_H
#define GTR_HISTORY_H 1

#include "header_stuff.h"

/*
 * The history entry structure.
 */
typedef struct 
{
	gchar *filename;
	gchar *project_name;
	gchar *project_version;
} GtrHistoryEntry;

#define GTR_HISTORY_ENTRY(x) ((GtrHistoryEntry *)x)

/*
 * Adds a history entry.
 */
void gtranslator_history_add(gchar *filename,
	gchar *project_name, gchar *project_version);

/*
 * Returns the history as a GList of GtrHistoryEntry's.
 */
GList *gtranslator_history_get(void);

#endif
