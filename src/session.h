/*
 * (C) 2000 	Fatih Demir <kabalak@gmx.net>
 *		Gediminas Paulauskas <menesis@delfi.lt>
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

#ifndef GTR_SESSION_H
#define GTR_SESSION_H 1

#include "gui.h"
#include "parse.h"

/*
 * Quits gtranslator silently because of a closing session.
 */
gint gtranslator_dies_for_you(GnomeClient * client, gpointer data);

/*
 * Saves gtranslator's current state on a save_yourself call through
 *  the session manager.
 */
gint gtranslator_sleep(GnomeClient * client, gint phase,
		       GnomeSaveStyle s_style, gint shutdown,
		       GnomeInteractStyle i_style, gint fast, gpointer data);

/*
 * Restores state saved during save_yourself
 */
void restore_session(GnomeClient * client);

#endif
