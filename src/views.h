/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#ifndef GTR_VIEWS_H
#define GTR_VIEWS_H 1

#include <glib.h>

/*
 * gtranslator's views are all part of this enumeration.
 *
 * Order should be kept in sync with gtranslator_sidebar_activate_views().
 */
typedef enum {
	GTR_MESSAGE_VIEW,
	GTR_NUMBER_VIEW,
	GTR_C_FORMAT_VIEW,
	GTR_HOTKEY_VIEW,
	GTR_LAST_VIEW
} GtrView;

/*
 * Switches to the given view for the current message -- returns FALSE on 
 *  failure.
 */
gboolean gtranslator_views_set(GtrView view);

/*
 * Return the name of the current/previously used view.
 */
GtrView gtranslator_views_get_current(void);
GtrView gtranslator_views_get_previous(void);

#endif
